#ifndef GLOBAL_H
#define GLOBAL_H

#include <X11/Xlib.h>

#include <vector>
#include <string>

typedef struct {
    Window _window;

    bool _fill;

    int _saved_width;
    int _saved_height;
    int _saved_x;
    int _saved_y;
} wm_window_t;

namespace wm {
    extern Display* dpy;
    extern Window root;
    extern XWindowAttributes attr;
    extern XButtonEvent start;
    extern Window fwindow;

    extern int display_width;
    extern int display_height;

    void update_focus_border(Window);

    extern std::vector <wm_window_t> windows;
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int spawn_process(const char*);

std::vector <std::string> split_str(std::string, char);
std::string strip(std::string &str);

std::string to_lower(std::string);
std::string to_upper(std::string);

#endif //GLOBAL_H
