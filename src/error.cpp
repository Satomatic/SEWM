#include <error.h>
#include <stdio.h>

int error::_last_error = -1;
int error::_error_count = 0;

/**
 *  Simple error output for unhandled events. If the same event happens
 *  multiple times it will just add a count at the end instead of flooding
 *  the console output.
 *
 *  @param XEvent* Pointer to unhandled event
 */
void error::unhandled(XEvent* ev){
    if (ev->type == error::_last_error){
        printf("\033[1F [ SEWM ] ! Unhandled event : %d (%d)\n",
            ev->type,
            error::_error_count
        );

        error::_error_count ++;
        return;
    }

    printf(" [ SEWM ] ! Unhandled event : %d\n", ev->type);
    error::_error_count = 0;
    error::_last_error = ev->type;
}
