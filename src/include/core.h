#ifndef CORE_H
#define CORE_H

#include <global.h>

#include <functional>
#include <vector>
#include <string>

typedef struct {
    std::string identifier;
    std::function <int(Display*, XEvent*)> callback;
} core_function_t;

namespace builtins {
    extern std::vector <core_function_t> functions;

    int extend_right(Display*, XEvent*);
    int extend_left(Display*, XEvent*);
    int extend_up(Display*, XEvent*);
    int extend_down(Display*, XEvent*);

    int shrink_down(Display*, XEvent*);
    int shrink_up(Display*, XEvent*);
    int shrink_left(Display*, XEvent*);
    int shrink_right(Display*, XEvent*);

    int exit_program(Display*, XEvent*);
}

#endif
