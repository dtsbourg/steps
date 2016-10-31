/*---------------------------------------------------------------------------
Template for TP of the course "System Engineering" 2016, EPFL

Authors: Flavien Bardyn & Martin Savary
Version: 1.0
Date: 10.08.2016

Use this "HelloWorld" example as basis to code your own app, which should at least 
count steps precisely based on accelerometer data. 

- Add the accelerometer data acquisition
- Implement your own pedometer using these data
- (Add an estimation of the distance travelled)

- Make an effort on the design of the app, try to do something fun!
- Comment and indent your code properly!
- Try to use your imagination and not google (we already did it, and it's disappointing!)
  to offer us a smart and original solution of pedometer

Don't hesitate to ask us questions.
Good luck and have fun!
---------------------------------------------------------------------------*/

// Include Pebble library
#include <pebble.h>

#define NB_SAMPLE   25
#define DATA_NUM    3

// Definitions for simmple menu
#define NB_MENU_SECTIONS 1
#define NB_SETTINGS_ITEMS 4
#define NB_DISPLAY 4

// Circular array storing the last NB_SAMPLE acceleromter values
static int last_data[DATA_NUM][NB_SAMPLE] = {{0}, {0}, {0}};
static int idx=0;

static void accel_data_handler(AccelData *data, uint32_t num_samples);

static GBitmap *happy_smiley;
static GBitmap *wheel;
static BitmapLayer *happy_smiley_layer;
static BitmapLayer *wheel_layer;
// Declare the main window and two text layers
Window *main_window, *menu_window;
TextLayer *background_layer,*title_layer, *data_layer;

// Declare main variable & layers for simple menu
static SimpleMenuLayer *main_menu_layer;
static SimpleMenuSection menu_sections[NB_MENU_SECTIONS];
static SimpleMenuItem settings_items[NB_SETTINGS_ITEMS];
static bool gender_flag = false;
static int display_type = 0;

/////// We configure the clicks on the main window//////////////////////////////////////
static void down_click_handler(ClickRecognizerRef recognizer, void *context) {        //
  // A single click has just occured                                                  //
    	window_stack_push(menu_window, false);

}

static void click_config_provider(void *context) {
  // Subcribe to button click events here
  ButtonId id = BUTTON_ID_DOWN;  // The Select button

  window_single_click_subscribe(id, down_click_handler);                              //
}                                                                                     //
////////////////////////////////////////////////////////////////////////////////////////

///////"Menu part"////////////////////////////////////////////////////////////////////
                                                                                    //
//Gender callback                                                                   //
static void gender_select_callback(int index, void *ctx) {                          //
  gender_flag = !gender_flag;                                                       //
  
  SimpleMenuItem *menu_item = &settings_items[index];
  
  if (gender_flag) {
    menu_item->subtitle = "Male";
  } else {
    menu_item->subtitle = "Female";
  }
  layer_mark_dirty(simple_menu_layer_get_layer(main_menu_layer));
}

static void display_select_callback(int index, void *ctx) {                          //
                                                                                     //
  display_type = (display_type + 1) % NB_DISPLAY;
  SimpleMenuItem *menu_item = &settings_items[index];
  
  if (gender_flag) {
    menu_item->subtitle = "Male";
  } else {
    menu_item->subtitle = "Female";
  }
  layer_mark_dirty(simple_menu_layer_get_layer(main_menu_layer));
}

static void menu_window_load(Window *window) {
  settings_items[0] = (SimpleMenuItem) {
    .title = "Hight",
    .subtitle = "Select your size",
  };
  settings_items[1] = (SimpleMenuItem) {
    .title = "Gender",
    .subtitle = "Click to choose",
    .callback = gender_select_callback,
  };

  settings_items[2] = (SimpleMenuItem) {
    .title = "Daily goal",
  };
   settings_items[3] = (SimpleMenuItem) {
    .title = "Display",
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

void main_window_unload(Window *window) {
  simple_menu_layer_destroy(main_menu_layer);
                                                                                    //
}                                                                                   //
//////////////////////////////////////////////////////////////////////////////////////

// Init function called when app is launched
static void init(void) {
  
    uint32_t num_samples = 16;
  
    // Allow accelerometer event
    accel_data_service_subscribe(num_samples, accel_data_handler);
  
    // Define accelerometer sampling rate
    accel_service_set_sampling_rate(ACCEL_SAMPLING_25HZ);
}



static void build_ui() {
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
    happy_smiley = gbitmap_create_with_resource(RESOURCE_ID_HAPPY_SMILEY);
    wheel = gbitmap_create_with_resource(RESOURCE_ID_WHEEL);
  
    happy_smiley_layer = bitmap_layer_create(GRect(57, 105, 30, 30));
    wheel_layer = bitmap_layer_create(GRect(132, 150, 14, 18));
  
    bitmap_layer_set_compositing_mode(happy_smiley_layer, GCompOpSet);
    bitmap_layer_set_compositing_mode(wheel_layer, GCompOpSet);
  
    bitmap_layer_set_bitmap(happy_smiley_layer, happy_smiley); 
    bitmap_layer_set_bitmap(wheel_layer, wheel);

    /* TEXT LAYER */
		// Create text Layer
    title_layer = text_layer_create(GRect(20, 25, 100, 30)); 
		data_layer = text_layer_create(GRect(25, 65, 100, 20));
		// Setup layer Information
    text_layer_set_background_color(title_layer, GColorClear);
		text_layer_set_text_color(title_layer, GColorWhite);	
		text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  	text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
    text_layer_set_text(title_layer, "STEPS");
  
		text_layer_set_background_color(data_layer, GColorClear);
		text_layer_set_text_color(data_layer, GColorWhite);	
		text_layer_set_font(data_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  	text_layer_set_text_alignment(data_layer, GTextAlignmentCenter);
    text_layer_set_text(data_layer, "x:0 y:0 z:0");
  
    /* == HIERARCHY ==*/
  	// Add layers as childs layers to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(background_layer));
    layer_add_child(window_layer, text_layer_get_layer(title_layer));
	  layer_add_child(window_layer, text_layer_get_layer(data_layer));
    layer_add_child(window_layer, bitmap_layer_get_layer(happy_smiley_layer));
    layer_add_child(window_layer, bitmap_layer_get_layer(wheel_layer));
  	
  
    /* == MENU WINDOW == */
    // Create main Window element and assign to pointer
  	menu_window = window_create();
    Layer *menu_window_layer = window_get_root_layer(menu_window);  
    
    window_set_window_handlers(menu_window, (WindowHandlers) {
    .load = menu_window_load,
    .unload = main_window_unload,
  });
  
    // Show the main window on the watch, with animated = true
  	window_stack_push(main_window, true);
}

// Function called when "num_samples" accelerometer samples are ready
static void accel_data_handler(AccelData *data,uint32_t num_samples)
{
  // Read sample 0's x,y and z values
  int16_t x = data[0].x;
  int16_t y = data[0].y;
  int16_t z = data[0].z;
  
  // Store the new accelerometer values with the last NB_SAMPLE ones
  last_data[0][idx] = data[0].x;
  last_data[1][idx] = data[0].y;
  last_data[2][idx] = data[0].z;
  idx = (idx + 1) % NB_SAMPLE;
  
  // tab of chars to print the results on the watch
  static char results[60];
  
  //Print the results in the LOG
  APP_LOG(APP_LOG_LEVEL_INFO, "x: %d, y: %d, z: %d", x, y, z);
 
  
  //Print the results on the watch
  snprintf(results, 60, "x: %d, y: %d, z: %d", x, y, z);
  text_layer_set_text(data_layer, results);
}


// deinit function called when the app is closed
static void deinit(void) {
  
    // Destroy layers and main window
    accel_data_service_unsubscribe();
    text_layer_destroy(background_layer);
	  text_layer_destroy(data_layer);
    gbitmap_destroy(happy_smiley);
    bitmap_layer_destroy(happy_smiley_layer);
    gbitmap_destroy(wheel);
    bitmap_layer_destroy(wheel_layer);
    window_destroy(main_window);
}

int main(void) {
    init();
    build_ui();
    app_event_loop();
    deinit();
}
