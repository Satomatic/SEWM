#ifndef CONFIG_H
#define CONFIG_H

#include <X11/Xlib.h>

#include <vector>
#include <string>

#define _KEY_BIND_TYPE_EXEC 0
#define _KEY_BIND_TYPE_FUNC 1

typedef struct {
    int _type;
    std::string _key;
    unsigned int _mod;
    std::string _command;
} _key_bind_t;

namespace config {
    int init(char*);

    extern int _global_border_width;
    extern int _global_border_focus_color;
    extern int _global_border_unfocus_color;

    extern int _global_infocus_border_width;
    extern int _global_unfocus_border_width;
    extern int _global_infocus_border_color;
    extern int _global_unfocus_border_color;

    extern int _global_extend_amount;

    extern std::vector <_key_bind_t> _key_bind_table;
}

#endif
