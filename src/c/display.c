/*---------------------------------------------------------------------------
    PEBBLE PODOMETER PROJECT
    systems engineering Fall 2016
    GROUP :
        * Sebastien Savidan
        * Jean Gschwind
        * Tristan Besson
        * Dylan Bourgeois

\file display.c
\description Handles the GUI for the application.

---------------------------------------------------------------------------*/

/// Include user headers
#include <src/c/display.h>
#include <src/c/accel.h>

// Definitions for simple menu
#define NB_MENU_SECTIONS  1

#define NB_DISPLAY        4

#define NB_SETTINGS_ITEMS 4
#define NB_GOAL_ITEMS     6
#define NB_HEIGHT_ITEMS   7
#define NB_MIN_HEIGHT     140

#define STRIDE_FACTOR_M   415
#define STRIDE_FACTOR_F   413

/// Icons
static GBitmap *happy_doge, *sad_doge ,*wheel;
static GBitmap *wheel;
static BitmapLayer *happy_doge_layer;
static BitmapLayer *wheel_layer;

/// Declare the main window and two text layers
Window *main_window, *menu_window, *height_window, *goal_window;
TextLayer *background_layer,*title_layer, *data_layer, *subtitle_layer, *display_layer;

/// Declare main variable & layers for simple menu
static SimpleMenuLayer *main_menu_layer;
static SimpleMenuSection menu_sections[NB_MENU_SECTIONS];
static SimpleMenuItem settings_items[NB_SETTINGS_ITEMS];
static char size[] = "Choose your size";
static bool gender_flag = false;
static char gender[] = "Choose your gender";
static int display_type = 0;
static char display[] = "Choose your display";

/// Variables & layer for height
static SimpleMenuLayer *height_layer;
static SimpleMenuSection height_sections[1];
static SimpleMenuItem height_items[NB_HEIGHT_ITEMS];
static int height_index = 3;

/// Variables & layer for daily goal
static SimpleMenuLayer *goal_layer;
static SimpleMenuSection goal_sections[1];
static SimpleMenuItem goal_items[NB_GOAL_ITEMS];
static int goal_index = 0;
static char goal[] = "Choose your goal";
static int goal_nb = 10000;

//////////////////////////////////////////////////////////////////////////////////////

// Init function called when app is launched
void init() {

    uint32_t num_samples = 16;

    // Allow accelerometer event
    accel_data_service_subscribe(num_samples, accel_data_handler);

    // Define accelerometer sampling rate
    accel_service_set_sampling_rate(ACCEL_SAMPLING_25HZ);
}

// deinit function called when the app is closed
void deinit() {

    // Destroy layers and main window
    accel_data_service_unsubscribe();
    text_layer_destroy(background_layer);
	text_layer_destroy(data_layer);
    text_layer_destroy(display_layer);
    gbitmap_destroy(happy_doge);
    gbitmap_destroy(sad_doge);
    bitmap_layer_destroy(happy_doge_layer);
    gbitmap_destroy(wheel);
    bitmap_layer_destroy(wheel_layer);
    window_destroy(main_window);
}

void build_ui() {

    /* == MAIN WINDOW == */
    // Create main Window element and assign to pointer
  	main_window = window_create();
    Layer *window_layer = window_get_root_layer(main_window);
    window_set_click_config_provider(main_window, click_config_provider);

    /* == BACKGROUND == */
	// Create background Layer
	background_layer = text_layer_create(GRect(0, 0, 144, 168));
	// Setup background layer color (black)
	text_layer_set_background_color(background_layer, GColorBlack);

    /* == IMAGE LAYER == */
    //Create Bitmap & image layer
    happy_doge = gbitmap_create_with_resource(RESOURCE_ID_HAPPY_DOGE);
    sad_doge = gbitmap_create_with_resource(RESOURCE_ID_SAD_DOGE);
    wheel = gbitmap_create_with_resource(RESOURCE_ID_WHEEL);

    happy_doge_layer = bitmap_layer_create(GRect(42, 100, 60, 50));

    wheel_layer = bitmap_layer_create(GRect(132, 150, 14, 18));

    bitmap_layer_set_compositing_mode(happy_doge_layer, GCompOpSet);
    bitmap_layer_set_compositing_mode(wheel_layer, GCompOpSet);

    bitmap_layer_set_bitmap(wheel_layer, wheel);

    /* TEXT LAYER */
	// Create text Layer
    title_layer = text_layer_create(GRect(0, 15, 144, 30));
	data_layer = text_layer_create(GRect(0, 50, 144, 30));
    subtitle_layer = text_layer_create(GRect(0, 100, 144, 30));
    display_layer = text_layer_create(GRect(0,115,144,30));

	// Setup layer Information
    text_layer_set_background_color(title_layer, GColorClear);
	text_layer_set_text_color(title_layer, GColorWhite);
	text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  	text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
    text_layer_set_text(title_layer, "STEPS");

    text_layer_set_background_color(subtitle_layer, GColorClear);
	text_layer_set_text_color(subtitle_layer, GColorWhite);
	text_layer_set_font(subtitle_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  	text_layer_set_text_alignment(subtitle_layer, GTextAlignmentCenter);

	text_layer_set_background_color(data_layer, GColorClear);
	text_layer_set_text_color(data_layer, GColorWhite);
	text_layer_set_font(data_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  	text_layer_set_text_alignment(data_layer, GTextAlignmentCenter);

    text_layer_set_background_color(display_layer, GColorClear);
	text_layer_set_text_color(display_layer, GColorWhite);
	text_layer_set_font(display_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  	text_layer_set_text_alignment(display_layer, GTextAlignmentCenter);

    /* == HIERARCHY ==*/
  	// Add layers as childs layers to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(background_layer));
    layer_add_child(window_layer, text_layer_get_layer(title_layer));
    layer_add_child(window_layer, text_layer_get_layer(subtitle_layer));
	layer_add_child(window_layer, text_layer_get_layer(data_layer));
    layer_add_child(window_layer, text_layer_get_layer(display_layer));
    layer_add_child(window_layer, bitmap_layer_get_layer(happy_doge_layer));
    layer_add_child(window_layer, bitmap_layer_get_layer(wheel_layer));

  switch(display_type){
        case DISPLAY_DOGE :
          if(get_step_count() <= (goal_nb/3))
            bitmap_layer_set_bitmap(happy_doge_layer, sad_doge);
          else
            bitmap_layer_set_bitmap(happy_doge_layer, happy_doge);
        break;

        case DISPLAY_GOAL :
          text_layer_set_text(subtitle_layer, "Steps to goal");
          layer_remove_from_parent(text_layer_get_layer(display_layer));
          layer_add_child(window_layer, text_layer_get_layer(display_layer));
        break;

        case DISPLAY_DISTANCE :
          text_layer_set_text(subtitle_layer, "Distance");
          layer_remove_from_parent(text_layer_get_layer(display_layer));
          layer_add_child(window_layer, text_layer_get_layer(display_layer));
        break;

        case DISPLAY_SPEED :
          text_layer_set_text(subtitle_layer, "Speed not implemented");
        break;

    }

    /* == MENU WINDOW == */
    // Create main Window element and assign to pointer
  	menu_window = window_create();

  // Layer *menu_window_layer = window_get_root_layer(menu_window);
    window_set_window_handlers(menu_window, (WindowHandlers) {
    .load = menu_window_load,
    .unload = menu_window_unload,
  });

   /* == HEIGHT SELECTION WINDOW == */
    // Create main Window element and assign to pointer
  	height_window = window_create();
   // Layer *height_layer = window_get_root_layer(height_window);

    window_set_window_handlers(height_window, (WindowHandlers) {
    .load = height_window_load,
    .unload = height_window_unload,
  });

    /* == GOAL SELECTION WINDOW == */
    // Create main Window element and assign to pointer
  	goal_window = window_create();
  //  Layer *goal_layer = window_get_root_layer(goal_window);

    window_set_window_handlers(goal_window, (WindowHandlers) {
    .load = goal_window_load,
    .unload = goal_window_unload,
  });

    // Show the main window on the watch, with animated = true
  	window_stack_push(main_window, true);
}

/////// We configure the clicks on the main window//////////////////////////////////////
void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  // A single click has just occured
    	window_stack_push(menu_window, false);
}

void click_config_provider(void *context) {
  // Subcribe to button click events here
  ButtonId id = BUTTON_ID_DOWN;  // The Select button

  window_single_click_subscribe(id, down_click_handler);
}

////////////////////////////////////////////////////////////////////////////////////////

//Function that compute de distance in function of gender, size and step count
int get_distance(){
  int size = height_index*10 + NB_MIN_HEIGHT;
  int stride;
  int distance;

  if(gender_flag)
    stride = STRIDE_FACTOR_M*size;
  else
    stride = STRIDE_FACTOR_F*size;

  distance = (stride*get_step_count())/100000; //the stride factors were multiplied by 1000 to avoid floats,
                                                          //and we have to devide again by 100 to have a result in meters
  return distance;
}

// Compute the number of steps remaining to reach goal
int steps_to_goal(){
  int steps_remaining;

  steps_remaining = goal_nb - get_step_count();;

  return steps_remaining;
}

// Return value for accel.c
TextLayer* return_data_layer(){
  return data_layer;
}
TextLayer* return_display_layer(){
  return display_layer;
}
int return_display_type(){
  return display_type;
}

/////// Menu part ////////////////////////////////////////////////////////////////////

// Gender callback
void gender_select_callback(int index, void *ctx) {
  gender_flag = !gender_flag;


  if (gender_flag) {
     snprintf(gender, 20, "Male");
  } else {
     snprintf(gender, 20, "Female");
  }
  layer_mark_dirty(simple_menu_layer_get_layer(main_menu_layer));
}


// Display callback
void display_select_callback(int index, void *ctx) {

  display_type = (display_type + 1) % NB_DISPLAY;

  switch(display_type)
    {
      case DISPLAY_DOGE :
        snprintf(display, 20,"Doge coach");
      break;

      case DISPLAY_GOAL :
        snprintf(display, 20, "Steps to goal");
      break;

      case DISPLAY_DISTANCE :
        snprintf(display, 20, "Distance");
      break;

      case DISPLAY_SPEED :
        snprintf(display, 20, "Speed");
      break;
  }
  layer_mark_dirty(simple_menu_layer_get_layer(main_menu_layer));
}


// Height menu callback
void height_select_callback(int index, void *ctx) {
  window_stack_push(height_window, false);
}

void height_callback(int index, void *ctx) {

  height_index = index;

  switch(height_index){

    case 0 :
      snprintf(size, 20, "1m40");
    break;

    case 1 :
      snprintf(size, 20, "1m50");
    break;

    case 2 :
      snprintf(size, 20, "1m60");
    break;

    case 3 :
      snprintf(size, 20, "1m70");
    break;

    case 4 :
      snprintf(size, 20, "1m80");
    break;

    case 5 :
      snprintf(size, 20, "1m90");
    break;

    case 6 :
      snprintf(size, 20, "2m00");
    break;
  }

    window_stack_push(menu_window, true);
    simple_menu_layer_destroy(height_layer);
}


// Goal callback
void goal_select_callback(int index, void *ctx) {

  window_stack_push(goal_window, false);
}

void goal_selection_callback(int index, void *ctx) {
  goal_index=index;
  switch(goal_index){
    case 0 :
    snprintf(goal, 20, "3000 steps");
    goal_nb=3000;
    break;
    case 1 :
    snprintf(goal, 20, "5000 steps");
    goal_nb=5000;
    break;
    case 2 :
    snprintf(goal, 20, "7000 steps");
    goal_nb=7000;
    break;
    case 3 :
    snprintf(goal, 20, "10 000 steps");
    goal_nb=10000;
    break;
    case 4 :
    snprintf(goal, 20, "15 000 steps");
    goal_nb=15000;
    break;
    case 5 :
    snprintf(goal, 20, "20 000 steps");
    goal_nb=20000;
    break;
  }

    window_stack_push(menu_window, true);
    simple_menu_layer_destroy(goal_layer);
}


// Menu load & unload
void menu_window_load(Window *window) {
  settings_items[0] = (SimpleMenuItem) {
    .title = "Height",
    .subtitle = size,
    .callback = height_select_callback,
  };
  settings_items[1] = (SimpleMenuItem) {
    .title = "Gender",
    .subtitle = gender,
    .callback = gender_select_callback,
  };
  settings_items[2] = (SimpleMenuItem) {
    .title = "Daily goal",
    .subtitle = goal,
    .callback = goal_select_callback,
  };
  settings_items[3] = (SimpleMenuItem) {
    .title = "Display",
     .subtitle = display,
    .callback = display_select_callback,
  };

  menu_sections[0] = (SimpleMenuSection) {
    .title = "Settings",
    .num_items = NB_SETTINGS_ITEMS,
    .items = settings_items,
  };

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  main_menu_layer = simple_menu_layer_create(bounds, window, menu_sections, NB_MENU_SECTIONS, NULL);

  layer_add_child(window_layer, simple_menu_layer_get_layer(main_menu_layer));
}

void menu_window_unload(Window *window) {
  simple_menu_layer_destroy(main_menu_layer);
   build_ui();
}


// Height menu load & unload
void height_window_load(Window *window) {
  height_items[0] = (SimpleMenuItem) {
    .title = "1m40",
    .callback = height_callback,
  };
  height_items[1] = (SimpleMenuItem) {
    .title = "1m50",
    .callback = height_callback,
  };
  height_items[2] = (SimpleMenuItem) {
    .title = "1m60",
    .callback = height_callback,
  };
  height_items[3] = (SimpleMenuItem) {
    .title = "1m70",
    .callback = height_callback,
  };
  height_items[4] = (SimpleMenuItem) {
    .title = "1m80",
    .callback = height_callback,
  };
  height_items[5] = (SimpleMenuItem) {
    .title = "1m90",
    .callback = height_callback,
  };
  height_items[6] = (SimpleMenuItem) {
    .title = "2m00",
    .callback = height_callback,
  };
  height_sections[0] = (SimpleMenuSection) {
    .title = "Select you size",
    .num_items = NB_HEIGHT_ITEMS,
    .items = height_items,
  };

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  height_layer = simple_menu_layer_create(bounds, window, height_sections, 1, NULL);

  layer_add_child(window_layer, simple_menu_layer_get_layer(height_layer));
}

void height_window_unload(Window *window) {
  simple_menu_layer_destroy(height_layer);

}

// Height menu load & unload
void goal_window_load(Window *window) {
  goal_items[0] = (SimpleMenuItem) {
    .title = "3000 steps",
    .callback = goal_selection_callback,
  };
  goal_items[1] = (SimpleMenuItem) {
    .title = "5000 steps",
   .callback = goal_selection_callback,
  };
  goal_items[2] = (SimpleMenuItem) {
    .title = "7000 steps",
    .callback = goal_selection_callback,
  };
  goal_items[3] = (SimpleMenuItem) {
    .title = "10000 steps",
    .callback = goal_selection_callback,
  };
  goal_items[4] = (SimpleMenuItem) {
    .title = "15000 steps",
    .callback = goal_selection_callback,
  };
  goal_items[5] = (SimpleMenuItem) {
    .title = "20000 steps",
    .callback = goal_selection_callback,
  };
  goal_sections[0] = (SimpleMenuSection) {
    .title = "Select your daily goal",
    .num_items = NB_GOAL_ITEMS,
    .items = goal_items,
  };

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  goal_layer = simple_menu_layer_create(bounds, window, goal_sections, 1, NULL);

  layer_add_child(window_layer, simple_menu_layer_get_layer(goal_layer));
}

void goal_window_unload(Window *window) {
  simple_menu_layer_destroy(goal_layer);
}

void update_count_display()
{
    // tab of chars to print the results on the watch
    static char results[60];
    //Print the results on the watch
    switch (return_display_type()) {
        case DISPLAY_GOAL:
            snprintf(results, 60, "%d", steps_to_goal());
            break;
        case DISPLAY_DISTANCE:
            snprintf(results, 60, "%u m", get_distance());
            break;
        case DISPLAY_DOGE:
            snprintf(results, 60, "%u", get_step_count());
            break;
        case DISPLAY_SPEED:
            snprintf(results, 60, "it's over 9000 !");
            break;
        default:
            break;
    }
    
    text_layer_set_text(return_data_layer(), results);
}
