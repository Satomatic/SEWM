#ifndef GLOBAL_H
#define GLOBAL_H

#include <X11/Xlib.h>

#include <vector>
#include <string>

#define WM_NAME "SEWM ( Sai's Experimental Window Manager )"
#define WM_VERS "0.2.0"

typedef struct {
    Window _window;
    
    Window _frame_window;
    GC _frame_window_gc;

    char* _window_name;

    bool _fill;

    int _saved_width;
    int _saved_height;
    int _saved_x;
    int _saved_y;
} wm_window_t;

namespace wm {
    extern Display* dpy;
    extern Window root;
    extern int screen;
    extern XWindowAttributes attr;
    extern XButtonEvent start;
    extern Window fwindow;

    extern int display_width;
    extern int display_height;

    extern XFontStruct* font;

    void update_focus_border(Window);

    extern std::vector <wm_window_t> windows;

    extern bool window_select_mode;
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int spawn_process(const char*);

std::vector <std::string> split_str(std::string, char);
std::string strip(std::string &str);

std::string to_lower(std::string);
std::string to_upper(std::string);

#endif //GLOBAL_H
