#include "mouse.h"

#include <config.h>
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
    
    if (event->xbutton.subwindow == None){
        wm::update_focus_border(None);
        return -1;
    }

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
    //if (event->xbutton.subwindow == None)
    //    return -1;

    if ((event->xbutton.state & Mod4Mask) == 0)
        return -1;
    
    int rel_x = wm::start.x - wm::attr.x;
    int rel_y = wm::start.y - wm::attr.y;

    int xdiff = event->xbutton.x_root - wm::start.x_root;
    int ydiff = event->xbutton.y_root - wm::start.y_root;
    
    /**
     *  Top left corner pinch
     */
    if (rel_x >= 0 && rel_x <= config::_global_corner_drag_amount &&
        rel_y >= 0 && rel_y <= config::_global_corner_drag_amount
    ){
        XMoveResizeWindow(dpy, wm::fwindow,
            wm::attr.x + xdiff,
            wm::attr.y + ydiff,
            MAX(1, wm::attr.width - xdiff),
            MAX(1, wm::attr.height - ydiff)
        );

        return 0;
    
    /**
     *  Bottom left corner pinch
     */
    } else if (rel_x >= 0 && rel_x <= config::_global_corner_drag_amount &&
               rel_y >= wm::attr.height - config::_global_corner_drag_amount && rel_y <= wm::attr.height){
        XMoveResizeWindow(dpy, wm::fwindow,
            wm::attr.x + xdiff,
            wm::attr.y,
            MAX(1, wm::attr.width - xdiff),
            MAX(1, wm::attr.height + ydiff)
        );

        return 0;

    /**
     *  Bottom right corner pinch
     */
    } else if (rel_x >= wm::attr.width - config::_global_corner_drag_amount && rel_x <= wm::attr.width &&
               rel_y >= wm::attr.height - config::_global_corner_drag_amount && rel_y <= wm::attr.height){
        XResizeWindow(dpy, wm::fwindow,
            MAX(1, wm::attr.width + xdiff),
            MAX(1, wm::attr.height + ydiff)
        );

        return 0;

    /**
     *  Top right corner pinch
     */
    } else if (rel_x >= wm::attr.width - config::_global_corner_drag_amount && rel_x <= wm::attr.width &&
               rel_y >= 0 && rel_y <= config::_global_corner_drag_amount){
        XMoveResizeWindow(dpy, wm::fwindow,
            wm::attr.x,
            wm::attr.y + ydiff,
            MAX(1, wm::attr.width + xdiff),
            MAX(1, wm::attr.height - ydiff)
        );

        return 0;

    }
    
    /**
     *  If we aren't dragging a corner then we can just move the window
     */
    XMoveWindow(dpy, wm::fwindow, wm::attr.x + xdiff, wm::attr.y + ydiff);
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
