#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <X11/Xlib.h>

namespace keyboard {
    int init(Display*);
    int handle(Display*, XEvent*);
}

#endif //KEYBOARD_H
