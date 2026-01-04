#include <core.h>
#include <config.h>

std::vector <core_function_t> builtins::functions = {
    { "extend-right", builtins::extend_right },
    { "extend-left",  builtins::extend_left  },
    { "extend-down",  builtins::extend_down  },
    { "extend-up",    builtins::extend_up    },

    { "shrink-down",  builtins::shrink_down  },
    { "shrink-up",    builtins::shrink_up    },
    { "shrink-left" , builtins::shrink_left  },
    { "shrink-right", builtins::shrink_right },

    { "fill-right-half", builtins::fill_right_half },
    { "fill-left-half",  builtins::fill_left_half  },
    { "fill-fullscreen", builtins::fill_fullscreen },
    { "fill-revert",     builtins::fill_revert     },

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
 *
 *  Functions for shrinking windows in directions, these will just
 *  call the extend functions but will flip the extend amount to be
 *  minus.
 *
 *  @note: This has no added benifit aside from having less code to
 *         write.
 *
 */
int builtins::shrink_down(Display* dpy, XEvent* event){
    config::_global_extend_amount *= -1;
    builtins::extend_up(dpy, event);
    config::_global_extend_amount *= -1;
    return 0;
}

int builtins::shrink_up(Display* dpy, XEvent* event){
    config::_global_extend_amount *= -1;
    builtins::extend_down(dpy, event);
    config::_global_extend_amount *= -1;
    return 0;
}
int builtins::shrink_left(Display* dpy, XEvent* event){
    config::_global_extend_amount *= -1;
    builtins::extend_right(dpy, event);
    config::_global_extend_amount *= -1;
    return 0;
}

int builtins::shrink_right(Display* dpy, XEvent* event){
    config::_global_extend_amount *= -1;
    builtins::extend_left(dpy, event);
    config::_global_extend_amount *= -1;
    return 0;
}

/**
 *
 *  Fill functions (Left, Right, Full, Revert)
 *
 */
void save_window_state(Display* dpy, Window window){
    XGetWindowAttributes(dpy, window, &wm::attr);

    int i = 0;
    for (; i < wm::windows.size() && wm::windows[i]._window != window; i++)
        continue;

    if (!wm::windows[i]._fill){
        wm::windows[i]._saved_x = wm::attr.x;
        wm::windows[i]._saved_y = wm::attr.y;
        wm::windows[i]._saved_width = wm::attr.width;
        wm::windows[i]._saved_height = wm::attr.height;
        wm::windows[i]._fill = true;
    }
}

#define PADDING 10

int builtins::fill_right_half(Display* dpy, XEvent* event){
    save_window_state(dpy, event->xkey.subwindow);

    XMoveResizeWindow(dpy, event->xkey.subwindow,
        wm::display_width / 2 + PADDING,
        PADDING,
        wm::display_width / 2 - (PADDING * 2),
        wm::display_height - (PADDING * 2)
    );

    return 0;
}

int builtins::fill_left_half(Display* dpy, XEvent* event){
    save_window_state(dpy, event->xkey.subwindow);

    XMoveResizeWindow(dpy, event->xkey.subwindow,
        PADDING,
        PADDING,
        wm::display_width / 2 - (PADDING * 2),
        wm::display_height - (PADDING * 2)
    );

    return 0;
}

int builtins::fill_fullscreen(Display* dpy, XEvent* event){
    save_window_state(dpy, event->xkey.subwindow);

    XMoveResizeWindow(dpy, event->xkey.subwindow,
        PADDING,
        PADDING,
        wm::display_width - (PADDING * 2),
        wm::display_height - (PADDING * 2)
    );

    return 0;
}

int builtins::fill_revert(Display* dpy, XEvent* event){
    int i = 0;
    for (; i < wm::windows.size() && wm::windows[i]._window != event->xkey.subwindow; i++)
        continue;

    if (!wm::windows[i]._fill)
        return 0;

    wm::windows[i]._fill = false;

    XMoveResizeWindow(dpy, event->xkey.subwindow,
        wm::windows[i]._saved_x,
        wm::windows[i]._saved_y,
        wm::windows[i]._saved_width,
        wm::windows[i]._saved_height
    );

    return 0;
}

/**
 *  Close the current window and focus another
 */
int builtins::exit_program(Display* dpy, XEvent* event){
    for (int i = 0; i < wm::windows.size(); i++){
        if (wm::windows[i]._window == wm::fwindow){
            wm::windows.erase(wm::windows.begin() + i);
            break;
        }
    }

    XDestroyWindow(dpy, wm::fwindow);

    if (wm::windows.size() == 0)
        wm::fwindow = wm::root;
    else
        wm::fwindow = wm::windows[wm::windows.size() - 1]._window;

    wm::update_focus_border(wm::fwindow);

    return 0;
}
