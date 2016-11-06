/*---------------------------------------------------------------------------
    PEBBLE PODOMETER PROJECT
    systems engineering Fall 2016
    GROUP :
        * Sebastien Savidan
        * Jean Gschwind
        * Tristan Besson
        * Dylan Bourgeois

\file main.c
\description This file is the main entry point for the podometer application.

---------------------------------------------------------------------------*/

/// Include Pebble library
#include <pebble.h>

/// Include standard libs
#include <stdbool.h>

/// Include user headers
#include <src/c/display.h>
#include <src/c/accel.h>

/// Main application loop
int main(void) {
    init();
    build_ui();
    app_event_loop();
    deinit();
}
