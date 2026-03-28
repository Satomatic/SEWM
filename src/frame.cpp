#include <global.h>
#include <config.h>
#include <cstring>
#include <frame.h>

Window frame::create_window_frame(){
    XSetWindowAttributes attributes = {
        .background_pixel = BlackPixel(wm::dpy, wm::screen),
        .event_mask = ExposureMask | KeyPressMask
    };

    Window window = XCreateWindow(
        wm::dpy,
        wm::root,
        0, 0, 500, 500, 0,
        DefaultDepth(wm::dpy, wm::screen),
        InputOutput,
        DefaultVisual(wm::dpy, wm::screen),
        CWBackPixel | CWEventMask,
        &attributes
    );

    XStoreName(wm::dpy, window, "sewm_window_frame");

    return window;
}

void frame::update_window_frames(){
    for (int i = 0; i < wm::windows.size(); i++){
        /**
         *  If the window frame is null or the frame fraphics context
         *  is null then we can just skip to avoid errors.
         */
        if (wm::windows[i]._frame_window == NULL ||
            wm::windows[i]._frame_window_gc == NULL
        ) continue;

        /**
         *  It's not impossible for windows to cloes without properly
         *  deleting the frame, this will check if the frame has any
         *  children and delete it if it doesn't.
         */
        Window root_return;
        Window parent_return;
        Window* children_return;
        unsigned int nchildren_return;

        if (XQueryTree(wm::dpy, wm::windows[i]._frame_window, &root_return, &parent_return, &children_return, &nchildren_return) != 0){
            if (nchildren_return == 0){
                XDestroyWindow(wm::dpy, wm::windows[i]._frame_window);
                XFreeGC(wm::dpy, wm::windows[i]._frame_window_gc);
                break;
            }
        }

        XWindowAttributes win_attr;
        XGetWindowAttributes(
            wm::dpy,
            wm::windows[i]._window,
            &win_attr
        );

        /**
        *  It's not impossible for the frame window and the application
        *  window to move apart from eachother, especially when apps try
        *  to be clever with window movements. So this line should solve
        *  that.
        */
        XMoveResizeWindow(
            wm::dpy,
            wm::windows[i]._window,
            win_attr.width,
            win_attr.height,
            0,
            (wm::font->ascent + wm::font->descent)
        );

        /**
         *  Get the current name of the window, if this fails, then we
         *  will use the last known name of the window.
         */
        char* name;
        int name_len;
        if (!XFetchName(wm::dpy, wm::windows[i]._window, &name)){
            name = wm::windows[i]._window_name;
        }

        name_len = strlen(name);

        /**
         *  Get text width for centering.
         */
        int string_width = XTextWidth(
            wm::font,
            name,
            name_len
        );

        int title_x = (win_attr.width - string_width) / 2;

        /**
         *  Adjust window border with and height depending on if the
         *  window is focused.
         *
         *  @todo: This doesn't actually need to be done every cycle
         *         but right now it is.
         */
        if (wm::windows[i]._frame_window == wm::fwindow){
            XSetWindowBorder(wm::dpy, wm::windows[i]._frame_window, config::_global_infocus_border_color);
            XSetWindowBorderWidth(wm::dpy, wm::windows[i]._frame_window, config::_global_infocus_border_width);
        } else {
            XSetWindowBorder(wm::dpy, wm::windows[i]._frame_window, config::_global_unfocus_border_color);
            XSetWindowBorderWidth(wm::dpy, wm::windows[i]._frame_window, config::_global_unfocus_border_width);
        }

        /**
         *  Draw the window gradient.
         *
         *  @note: There is definitely a better way of doing this,
         *         but this doesn't work too bad for now.
         */
        int grad_seg_size = (win_attr.width + 10) / 10;
        int blue_grad = 255;

        for (int x = 0; x < 10; x++){
            int color_value = (x * 15) + 50;

            /**
             *  If the window isn't highlighted then we want to make it
             *  grayscale. To do this we just match the blue value to the
             *  red and green.
             */
            if (wm::windows[i]._frame_window != wm::fwindow)
                blue_grad = color_value;

            XSetForeground(
                wm::dpy,
                wm::windows[i]._frame_window_gc,
                ((color_value & 0xff) << 16) + ((color_value & 0xff) << 8) + (blue_grad & 0xff)
            );

            XFillRectangle(
                wm::dpy,
                wm::windows[i]._frame_window,
                wm::windows[i]._frame_window_gc,
                x * grad_seg_size,
                0,
                grad_seg_size,
                200
            );
        }

        /**
         *  Draw the window tirle text.
         *
         *  @note: We do this twice with a 1 pixel offset to get the
         *         light stroke effect, there is probably a better way
         *         of doing this.
         */
        XSetForeground(wm::dpy, wm::windows[i]._frame_window_gc, 0x000000);
        XDrawString(
            wm::dpy,
            wm::windows[i]._frame_window,
            wm::windows[i]._frame_window_gc,
            title_x + 1,
            10,
            name,
            name_len
        );

        XSetForeground(wm::dpy, wm::windows[i]._frame_window_gc, 0xffffff);
        XDrawString(
            wm::dpy,
            wm::windows[i]._frame_window,
            wm::windows[i]._frame_window_gc,
            title_x,
            10,
            name,
            name_len
        );
    }
}