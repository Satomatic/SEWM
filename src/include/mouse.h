#ifndef MOUSE_H
#define MOUSE_H

#include <X11/Xlib.h>

namespace mouse {
    int init(Display*);
    int handle_press(Display*, XEvent*);
    int handle_release(Display*, XEvent*);
    int handle_motion(Display*, XEvent*);
}

#endif //MOUSE_H
