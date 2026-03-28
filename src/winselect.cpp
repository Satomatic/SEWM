#include <core.h>

#include <iostream>

typedef struct {
    Window _window_frame;
    Window _window;

    int _x;
    int _y;
    int _w;
    int _h;
} _select_window_store_t;

std::vector <_select_window_store_t> _window_store = {};

int builtins::window_select_open(Display* dpy, XEvent* event){
    /**
     *  Check if we are in the window select mode first, if we are then
     *  we return to stop the corruption of the store data.
     */
    if (wm::window_select_mode)
        return -1;

    wm::window_select_mode = true;

    /**
     *  Calculate how large our windows should be resized to.
     */
    int window_width = (wm::display_width - (20 * wm::windows.size())) / wm::windows.size();
    int window_height = wm::display_height;
    int x_count = 10;

    XWindowAttributes attr;

    for (int i = 0; i < wm::windows.size(); i++){
        /**
         *  Store old window position and size data so it can be reverted
         *  by `window_select_close`.
         */
        XGetWindowAttributes(dpy, wm::windows[i]._frame_window, &attr);
        _window_store.push_back({
            wm::windows[i]._frame_window,
            wm::windows[i]._window,
            attr.x,
            attr.y,
            attr.width,
            attr.height
        });

        /**
         *  Move and resize the windows.
         */
        XMoveResizeWindow(dpy, wm::windows[i]._frame_window,
            x_count,
            10,
            window_width,
            window_height - 20
        );

        XResizeWindow(dpy, wm::windows[i]._window,
            window_width,
            window_height - 20 - (wm::font->ascent + wm::font->descent)
        );

        x_count += window_width + 20;
    }

    return 0;
}


int builtins::window_select_close(Display* dpy, XEvent* event){
    for (int i = 0; i < _window_store.size(); i++){
        XMoveResizeWindow(dpy, _window_store[i]._window_frame,
            _window_store[i]._x,
            _window_store[i]._y,
            _window_store[i]._w,
            _window_store[i]._h
        );

        XResizeWindow(dpy, _window_store[i]._window,
            _window_store[i]._w,
            _window_store[i]._h - (wm::font->ascent + wm::font->descent)
        );
    }

    _window_store.clear();

    wm::window_select_mode = false;

    return 0;
}
