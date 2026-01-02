#ifndef ERROR_H
#define ERROR_H

#include <X11/Xlib.h>

namespace error {
    extern int _last_error;
    extern int _error_count;
    
    void unhandled(XEvent*);
}

#endif
