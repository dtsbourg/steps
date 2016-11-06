#ifndef DISPLAY_H
#define DISPLAY_H

#include <pebble.h>

void init();
void deinit();
void build_ui();

void down_click_handler(ClickRecognizerRef recognizer, void *context);
void click_config_provider(void *context);

int distance();
int steps_to_goal();

TextLayer* return_data_layer();
TextLayer* return_display_layer();
int return_display_type();

void gender_select_callback(int index, void *ctx);

void display_select_callback(int index, void *ctx);

void height_select_callback(int index, void *ctx);
void height_callback(int index, void *ctx);

void goal_select_callback(int index, void *ctx);
void goal_selection_callback(int index, void *ctx);

void menu_window_load(Window *window);
void menu_window_unload(Window *window);  

void height_window_load(Window *window);
void height_window_unload(Window *window);

void goal_window_load(Window *window);
void goal_window_unload(Window *window);


#endif