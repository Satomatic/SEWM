#ifndef GLOBAL_H
#define GLOBAL_H

#include <X11/Xlib.h>

#include <vector>
#include <string>

namespace wm {
    extern Display* dpy;
    extern Window root;
    extern XWindowAttributes attr;
    extern XButtonEvent start;
    extern Window fwindow;

    void update_focus_border(Window);

    extern std::vector <Window> windows;
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int spawn_process(const char*);

std::vector <std::string> split_str(std::string, char);
std::string strip(std::string &str);

std::string to_lower(std::string);
std::string to_upper(std::string);

#endif //GLOBAL_H
