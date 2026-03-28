#ifndef FRAME_H
#define FRAME_H

#include <X11/Xlib.h>

namespace frame {
    Window create_window_frame();
    void update_window_frames();
}

#endif