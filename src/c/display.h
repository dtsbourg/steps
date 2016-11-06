/*---------------------------------------------------------------------------
    PEBBLE PODOMETER PROJECT
    systems engineering Fall 2016
    GROUP :
        * Sebastien Savidan
        * Jean Gschwind
        * Tristan Besson
        * Dylan Bourgeois

\file display.h
\description Handles the GUI for the application.

---------------------------------------------------------------------------*/

#ifndef DISPLAY_H
#define DISPLAY_H

/// Include Pebble library
#include <pebble.h>

/// Standard lib include
#include <math.h>

/// Control
void init();        ///< Initialises the application
void deinit();      ///< Deallocate the application's resources
void build_ui();    ///< Main builder for the user interface

/// Callbacks
/// User preferences callbacks for :
void gender_select_callback(int index, void *ctx);  ///< Gender
void display_select_callback(int index, void *ctx); ///< Display type
void height_select_callback(int index, void *ctx);  ///< Height
void height_callback(int index, void *ctx);         ///< Height
void goal_select_callback(int index, void *ctx);    ///< Goal (steps)
void goal_selection_callback(int index, void *ctx); ///< Goal (steps)

/// Handlers
void down_click_handler(ClickRecognizerRef recognizer, void *context); ///< Click recognizer

/// Providers
void click_config_provider(void *context); ///< Provides the interface for the click handler

/// Graphical elements
/// Build and load GUI elements
TextLayer* return_data_layer();             ///< Data layer
TextLayer* return_display_layer();          ///< Display layer
void menu_window_load(Window *window);      ///< Menu window (load)
void menu_window_unload(Window *window);    ///< Menu window (unload)

void height_window_load(Window *window);    ///< Height window (load)
void height_window_unload(Window *window);  ///< Height window (unload)

void goal_window_load(Window *window);      ///< Goal window (load)
void goal_window_unload(Window *window);    ///< Goal window (unload)

/// Helpers
int distance();
int steps_to_goal();
int return_display_type();



#endif
