#include <X11/Xlib.h>

#include <stdlib.h>
#include <stdio.h>

#include <keyboard.h>
#include <config.h>
#include <mouse.h>
#include <global.h>
#include <error.h>
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

int wm::display_width = 0;
int wm::display_height = 0;

std::vector <wm_window_t> wm::windows = {};

/**
 *  Change the input focus and update the borders to
 *  a new window.
 *
 *  @param Window Window to focus
 */
void wm::update_focus_border(Window win){
    XSetWindowBorder(wm::dpy, wm::fwindow, config::_global_unfocus_border_color);
    XSetWindowBorderWidth(wm::dpy, wm::fwindow, config::_global_unfocus_border_width);
 
    wm::fwindow = win;
    XSetInputFocus(wm::dpy, wm::fwindow, RevertToParent, CurrentTime);
    
    XSetWindowBorder(wm::dpy, wm::fwindow, config::_global_infocus_border_color);
    XSetWindowBorderWidth(wm::dpy, wm::fwindow, config::_global_infocus_border_width);
}

int main(int argc, char** argv)
{
    wm::process_args(argc, argv);

    XEvent ev;

    if(!(wm::dpy = XOpenDisplay(0x0))) return 1;
    wm::root = DefaultRootWindow(wm::dpy);
    XSetInputFocus(wm::dpy, wm::root, RevertToParent, CurrentTime);
    wm::fwindow = wm::root;

    wm::display_width = DisplayWidth(wm::dpy, DefaultScreen(wm::dpy));
    wm::display_height = DisplayHeight(wm::dpy, DefaultScreen(wm::dpy));
    
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

                wm::windows.push_back({
                    ev.xcreatewindow.window,
                    false,
                    wm::attr.width,
                    wm::attr.height,
                    wm::attr.x,
                    wm::attr.y
                });

                break;

            default:
                error::unhandled(&ev);
                break;
        }
    }
}
