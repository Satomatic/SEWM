#include <X11/Xlib.h>

#include <stdlib.h>
#include <stdio.h>
#include <cstring>

#include <keyboard.h>
#include <config.h>
#include <mouse.h>
#include <global.h>
#include <error.h>
#include <frame.h>
#include <args.h>

int error_handle (Display* dpy, XErrorEvent* ev){
    return 0;
}

int error_fatal_handle(Display* dpy){
    return 0;
}

Display* wm::dpy;
Window wm::root;
XWindowAttributes wm::attr;
XButtonEvent wm::start;
Window wm::fwindow;
XFontStruct* wm::font;
int wm::screen;

int wm::display_width = 0;
int wm::display_height = 0;

bool wm::window_select_mode = false;

std::vector <wm_window_t> wm::windows = {};

/**
 *  Change the input focus and update the borders to
 *  a new window.
 *
 *  @param Window Window to focus
 */
void wm::update_focus_border(Window win){
    XWindowChanges win_config;
    win_config.stack_mode = Above;
    XConfigureWindow(dpy, win, CWStackMode, &win_config);
    
    wm::fwindow = win;

    if (wm::fwindow)
        XSetInputFocus(wm::dpy, wm::fwindow, RevertToParent, CurrentTime);
}

int main(int argc, char** argv)
{
    wm::process_args(argc, argv);

    XEvent ev;

    if(!(wm::dpy = XOpenDisplay(0x0))) return 1;
    wm::root = DefaultRootWindow(wm::dpy);
    wm::screen = DefaultScreen(wm::dpy);
    XSetInputFocus(wm::dpy, wm::root, RevertToParent, CurrentTime);
    wm::fwindow = wm::root;

    wm::display_width = DisplayWidth(wm::dpy, DefaultScreen(wm::dpy));
    wm::display_height = DisplayHeight(wm::dpy, DefaultScreen(wm::dpy));
    
    wm::font = XLoadQueryFont(
        wm::dpy,
        "Fixed"
    );

    /**
     *  We only really set the error handlers to stop the default one
     *  from exiting every time something minor happens.
     */
    XSetErrorHandler(error_handle);
    XSetIOErrorHandler(error_fatal_handle);

    /**
     *  Intialize the keyboard and mouse inputs
     */
    keyboard::init(wm::dpy);
    mouse::init(wm::dpy);

    XSelectInput(wm::dpy, wm::root, SubstructureNotifyMask);

    wm::start.subwindow = None;
    for(;;){
        XNextEvent(wm::dpy, &ev);

        switch (ev.type) {
            case KeyPress:
                keyboard::handle(wm::dpy, &ev);
                break;

            case KeyRelease:
                break;

            case ButtonPress:
                mouse::handle_press(wm::dpy, &ev);
                break;

            case MotionNotify:
                mouse::handle_motion(wm::dpy, &ev);
                break;

            case ButtonRelease:
                mouse::handle_release(wm::dpy, &ev);
                break;

            case CreateNotify:
                wm::update_focus_border(ev.xcreatewindow.window);

                XGetWindowAttributes(
                    wm::dpy,
                    ev.xcreatewindow.window,
                    &wm::attr
                );
                break;

            case MapNotify: {
                /**
                 *  @note: This is a pretty dog shit method for checking if we are
                 *         trying to handle a map of a window frame, if I actually
                 *         did this properly then this would not be an issue.
                 */
                char* name;
                if (XFetchName(wm::dpy, ev.xmap.window, &name) && strcmp(name, "sewm_window_frame") == 0)
                    break;

                /**
                 *  @note: Some applications (like chrome) don't set a window name
                 *         or it will get set to null for some dogshit reason. Now
                 *         to prevent issues we will just set it to an empty when
                 *         happens.
                 */
                if (name == NULL)
                    name = "";

                XGetWindowAttributes(
                    wm::dpy,
                    ev.xmap.window,
                    &wm::attr
                );

                /**
                 *  Set the intial variables to NULL, this makes it easier
                 *  for override redirect to work later on.
                 */
                Window window_frame = NULL;
                GC gc = NULL;

                if (!wm::attr.override_redirect){
                    window_frame = frame::create_window_frame();
                    gc = XCreateGC(wm::dpy, window_frame, 0, NULL);

                    XMapWindow(wm::dpy, window_frame);

                    XResizeWindow(
                        wm::dpy,
                        window_frame,
                        wm::attr.width,
                        wm::attr.height + (wm::font->ascent + wm::font->descent)
                    );

                    XReparentWindow(
                        wm::dpy,
                        ev.xmap.window,
                        window_frame,
                        0, (wm::font->ascent + wm::font->descent)
                    );

                    XSetWindowBorderWidth(wm::dpy, ev.xmap.window, 0);
                    XSetFont(wm::dpy, gc, wm::font->fid);
                }

                wm::windows.push_back({
                    ev.xmap.window,
                    window_frame,
                    gc,
                    name,
                    false,
                    wm::attr.width,
                    wm::attr.height,
                    wm::attr.x,
                    wm::attr.y
                });

                break;
            }

            case UnmapNotify:
                for (int i = 0; i < wm::windows.size(); i++)
                    if (wm::windows[i]._frame_window == ev.xunmap.window)
                        wm::windows.erase(wm::windows.begin() + i); 
                break;

            default: 
                error::unhandled(&ev);
                break;
        }

        frame::update_window_frames();
    }
}
