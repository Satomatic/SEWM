#include "mouse.h"

#include <global.h>

/**
 *  Initialize mouse events (ButtonPress / ButtonMotion grabs)
 *
 *  @todo: Mouse binds should really be loaded from the configuration file.
 */
int mouse::init(Display* dpy) {
    Window root = DefaultRootWindow(dpy);

    XGrabButton(dpy, 1, Mod4Mask, root, True, ButtonPressMask | ButtonMotionMask, GrabModeAsync,
                GrabModeAsync, None, None);

    XGrabButton(dpy, 3, Mod4Mask, root, True, ButtonPressMask | ButtonMotionMask, GrabModeAsync,
                GrabModeAsync, None, None);

    XGrabButton(dpy, 1, 0, root, False, ButtonPressMask | ButtonMotionMask, GrabModeSync,
                GrabModeAsync, None, None);

    return 0;
}

/**
 *  Handle mouse button press events
 *
 *  @param Display* Pointer to window manager display.
 *  @param XEvent* Pointer to window manager event.
 *
 *  @return (int) 0 == done :: -1 == unhandled
 */
int mouse::handle_press(Display* dpy, XEvent* event) {
    XAllowEvents(wm::dpy, ReplayPointer, event->xbutton.time);
    XSync(wm::dpy, 0);
    
    if (event->xbutton.subwindow == None)
        return -1;

    XGetWindowAttributes(dpy, event->xbutton.subwindow, &wm::attr);
    wm::start = event->xbutton;

    XWindowChanges win_config;
    win_config.stack_mode = Above;

    XConfigureWindow(dpy, event->xbutton.subwindow, CWStackMode, &win_config);

    wm::update_focus_border(event->xbutton.subwindow);

    return 0;
}

/**
 *  Handle mouse motion events
 *
 *  @todo: This needs to account for which direction / half you
 *         want to be resizing from to make it more intuitive.
 *  
 *  @param Display* Pointer to window manager display.
 *  @param XEvent* Pointer to window manager event.
 *
 *  @return (int) 0 == done :: -1 == unhandled
 */
int mouse::handle_motion(Display* dpy, XEvent* event) {
    if (event->xbutton.subwindow == None)
        return -1;
    
    if ((event->xbutton.state & Mod4Mask) == 0)
        return -1;

    int xdiff = event->xbutton.x_root - wm::start.x_root;
    int ydiff = event->xbutton.y_root - wm::start.y_root;
    XMoveResizeWindow(dpy, wm::start.subwindow,
                      wm::attr.x + (wm::start.button==1 ? xdiff : 0),
                      wm::attr.y + (wm::start.button==1 ? ydiff : 0),
                      MAX(1, wm::attr.width + (wm::start.button==3 ? xdiff : 0)),
                      MAX(1, wm::attr.height + (wm::start.button==3 ? ydiff : 0)));

    return 0;
}

/**
 *  Handle mouse button release events
 *
 *  @param Display* Pointer to window manager display.
 *  @param XEvent* Pointer to window manager event.
 *
 *  @return (int) 0 == done :: -1 == unhandled
 */
int mouse::handle_release(Display* dpy, XEvent* event) {
    wm::start.subwindow = None;
    return 0;
}
