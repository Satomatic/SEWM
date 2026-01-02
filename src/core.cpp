#include <core.h>
#include <config.h>

std::vector <core_function_t> builtins::functions = {
    { "extend-right", builtins::extend_right },
    { "extend-left",  builtins::extend_left  },
    { "extend-down",  builtins::extend_down  },
    { "extend-up",    builtins::extend_up    },

    { "exit-program", builtins::exit_program },
};

/**
 *  Extend the current window to the right.
 */
int builtins::extend_right(Display* dpy, XEvent* event){
    XGetWindowAttributes(dpy, event->xkey.subwindow, &wm::attr);
    XResizeWindow(dpy, event->xkey.subwindow,
        wm::attr.width + config::_global_extend_amount,
        wm::attr.height
    );
    return 0;
}

/**
 *  Extend the current window to the left
 */
int builtins::extend_left(Display* dpy, XEvent* event){
    XGetWindowAttributes(dpy, event->xkey.subwindow, &wm::attr);
    XMoveResizeWindow(dpy, event->xkey.subwindow,
        wm::attr.x - config::_global_extend_amount,
        wm::attr.y,
        wm::attr.width + config::_global_extend_amount,
        wm::attr.height);
    return 0;
}

/**
 *  Extend the current window up
 */
int builtins::extend_up(Display* dpy, XEvent* event){
    XGetWindowAttributes(dpy, event->xkey.subwindow, &wm::attr);
    XMoveResizeWindow(dpy, event->xkey.subwindow,
        wm::attr.x,
        wm::attr.y - config::_global_extend_amount,
        wm::attr.width,
        wm::attr.height + config::_global_extend_amount);
    return 0;
}

/**
 *  Extend the current window down
 */
int builtins::extend_down(Display* dpy, XEvent* event){
    XGetWindowAttributes(dpy, event->xkey.subwindow, &wm::attr);
    XResizeWindow(dpy, event->xkey.subwindow,
        wm::attr.width,
        wm::attr.height + config::_global_extend_amount);
    return 0;
}

/**
 *  Close the current window and focus another
 */
int builtins::exit_program(Display* dpy, XEvent* event){
    for (int i = 0; i < wm::windows.size(); i++){
        if (wm::windows[i] == wm::fwindow){
            wm::windows.erase(wm::windows.begin() + i);
            break;
        }
    }

    XDestroyWindow(dpy, wm::fwindow);

    if (wm::windows.size() == 0)
        wm::fwindow = wm::root;
    else
        wm::fwindow = wm::windows[wm::windows.size() - 1];

    wm::update_focus_border(wm::fwindow);

    return 0;
}
