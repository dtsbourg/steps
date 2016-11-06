/*---------------------------------------------------------------------------

---------------------------------------------------------------------------*/

// Include Pebble library
#include <pebble.h>
#include <stdbool.h>

#include <src/c/display.h>
#include <src/c/accel.h>

int main(void) {
    init();
    build_ui();
    app_event_loop();
    deinit();
}