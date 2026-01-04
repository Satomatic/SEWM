#include "keyboard.h"

#include <global.h>
#include <config.h>
#include <core.h>

#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include <string.h>

#include <iostream>

/**
 *  Initialize keyboard events (KeyGrabs)
 *
 *  @param Display* Pointer to main WM display
 * 
 *  @return int (0 == success :: -1 == fail)
 */
int keyboard::init(Display* dpy) {
    /**
     *  Bind keys loaded from the configuration file.
     */
    #define XStringToKeycode(DPY, C) XKeysymToKeycode(DPY, XStringToKeysym(C))
    for (int i = 0; i < config::_key_bind_table.size(); i++){
        XGrabKey(dpy,
            XStringToKeycode(dpy, config::_key_bind_table[i]._key.c_str()),
            config::_key_bind_table[i]._mod,
            DefaultRootWindow(dpy),
            True,
            GrabModeAsync, GrabModeAsync
        );
    }

    return 0;
}

/**
 *  Handle keyboard events
 *
 *  @param Display* Pointer to window manager display.
 *  @param XEvent* Pointer to window manager event.
 *
 *  @return (int) 0 == done :: -1 == unhandled
 */
int keyboard::handle(Display* dpy, XEvent* event) {
    /**
     *  Take the inputted key as an X11 keycode, and convert it
     *  to a standard string.
     */
    KeySym key = XkbKeycodeToKeysym(dpy, event->xkey.keycode, 0, 0);
    char* key_str = XKeysymToString(key);

    for (int i = 0; i < config::_key_bind_table.size(); i++){
        /**
         *  If the keys or modifiers don't match then skip to the next
         *  config entry.
         */
        if (strcmp(config::_key_bind_table[i]._key.c_str(), key_str) != 0 ||
            config::_key_bind_table[i]._mod != event->xkey.state)
            continue;

        /**
         *  Once found, figure out what type of key bind is
         *  being used.
         */
        switch (config::_key_bind_table[i]._type){
            case _KEY_BIND_TYPE_EXEC:
                spawn_process(config::_key_bind_table[i]._command.c_str());
                break;

            case _KEY_BIND_TYPE_FUNC:
                for (int b = 0; b < builtins::functions.size(); b++){
                    if (builtins::functions[b].identifier == config::_key_bind_table[i]._command){
                        builtins::functions[b].callback(dpy, event);
                        break;
                    }
                }

                break;

            default:
                break;
        }
    }

    if (event->xkey.subwindow != None)
        XRaiseWindow(dpy, event->xkey.subwindow);

    return 0;
}
