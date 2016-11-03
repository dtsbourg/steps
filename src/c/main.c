/*---------------------------------------------------------------------------

---------------------------------------------------------------------------*/

// Include Pebble library
#include <pebble.h>
#include <stdbool.h>

#define NB_SAMPLE   25
#define DATA_NUM    3
#define WINDOW_SIZE 4
#define WINDOW_SIZE_LOG_2 2
#define STEP_ORDER 5

// Definitions for simmple menu
#define NB_MENU_SECTIONS 1
#define NB_SETTINGS_ITEMS 4
#define NB_DISPLAY 4

#define NB_HEIGHT_ITEMS 7

#define NB_GOAL_ITEMS 6

// Circular array storing the last NB_SAMPLE acceleromter values
static uint32_t last_data[NB_SAMPLE];
static uint32_t last_avg[NB_SAMPLE];
static int idx=0;
static int nb_pts=0;

//Accelaration data 
static void accel_data_handler(AccelData *data, uint32_t num_samples);

//Icons
static GBitmap *happy_smiley;
static GBitmap *wheel;
static BitmapLayer *happy_smiley_layer;
static BitmapLayer *wheel_layer;

// Declare the main window and two text layers
Window *main_window, *menu_window, *height_window, *goal_window;
TextLayer *background_layer,*title_layer, *data_layer;

// Declare main variable & layers for simple menu
static SimpleMenuLayer *main_menu_layer;
static SimpleMenuSection menu_sections[NB_MENU_SECTIONS];
static SimpleMenuItem settings_items[NB_SETTINGS_ITEMS];
static bool gender_flag = false;
static int display_type = 0;

// Variables & layer for height
static SimpleMenuLayer *height_layer;
static SimpleMenuSection height_sections[1];
static SimpleMenuItem height_items[NB_HEIGHT_ITEMS];
static int height_index = 0;

// Variables & layer for daily goal
static SimpleMenuLayer *goal_layer;
static SimpleMenuSection goal_sections[1];
static SimpleMenuItem goal_items[NB_GOAL_ITEMS];
static int goal_index = 0;



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

///////Menu part////////////////////////////////////////////////////////////////////
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
}                       //

//Display callback
static void display_select_callback(int index, void *ctx) {                          //
                                                                                     //
  display_type = (display_type + 1) % NB_DISPLAY;
  SimpleMenuItem *menu_item = &settings_items[index];
  
  switch(display_type)
    {
      case 0 : 
      menu_item->subtitle = "Doge coach";
      break;
      
      case 1 :
      menu_item->subtitle = "Steps to goal";
      break;
      
      case 2 :
      menu_item->subtitle = "Distance";
      break;
    
      case 3 : 
      menu_item->subtitle = "Speed";
      break;
  }
  layer_mark_dirty(simple_menu_layer_get_layer(main_menu_layer));
}

//Height callback
static void height_select_callback(int index, void *ctx) {                          //
                                                                                     //
  window_stack_push(height_window, false);
}

//Goal callback
static void goal_select_callback(int index, void *ctx) {                          //
                                                                                     //
  window_stack_push(goal_window, false);
}






// Menu load
static void menu_window_load(Window *window) {
  settings_items[0] = (SimpleMenuItem) {
    .title = "Height",
    .subtitle = "Select your size",
    .callback = height_select_callback,
  };
  settings_items[1] = (SimpleMenuItem) {
    .title = "Gender",
    .subtitle = "Click to choose",
    .callback = gender_select_callback,
  };

  settings_items[2] = (SimpleMenuItem) {
    .title = "Daily goal",
    .subtitle = "Select your goal",
    .callback = goal_select_callback,
  };
   settings_items[3] = (SimpleMenuItem) {
    .title = "Display",
     .subtitle = "Doge coach - click to change",
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
// Menu unload
void main_window_unload(Window *window) {
  simple_menu_layer_destroy(main_menu_layer);
                                                                                    //
}


// Height menu load
static void height_window_load(Window *window) {
  height_items[0] = (SimpleMenuItem) {
    .title = "1m40",
  };
  height_items[1] = (SimpleMenuItem) {
    .title = "1m50",
  };
  height_items[2] = (SimpleMenuItem) {
    .title = "1m60",
  };
  height_items[3] = (SimpleMenuItem) {
    .title = "1m70",
  };
  height_items[4] = (SimpleMenuItem) {
    .title = "1m80",
  };
  height_items[5] = (SimpleMenuItem) {
    .title = "1m90",
  };
  height_items[6] = (SimpleMenuItem) {
    .title = "2m00",
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
//Height menu unload
void height_window_unload(Window *window) {
  simple_menu_layer_destroy(height_layer);
                                                                                    //
}


// Height menu load
static void goal_window_load(Window *window) {
  goal_items[0] = (SimpleMenuItem) {
    .title = "3000 steps",
  };
 goal_items[1] = (SimpleMenuItem) {
    .title = "5000 steps",
  };
  goal_items[2] = (SimpleMenuItem) {
    .title = "7000 steps",
  };
  goal_items[3] = (SimpleMenuItem) {
    .title = "10000 steps",
  };
  goal_items[4] = (SimpleMenuItem) {
    .title = "15000 steps",
  };
  goal_items[5] = (SimpleMenuItem) {
    .title = "20000 steps",
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
//Height menu unload
void goal_window_unload(Window *window) {
  simple_menu_layer_destroy(goal_layer);
                                                                                    //
}//
//////////////////////////////////////////////////////////////////////////////////////






// Init function called when app is launched
static void init(void) {
  
    uint32_t num_samples = 25;
  
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
  
   /* == HEIGHT SELECTION WINDOW == */
    // Create main Window element and assign to pointer
  	height_window = window_create();
    Layer *height_layer = window_get_root_layer(height_window);  
    
    window_set_window_handlers(height_window, (WindowHandlers) {
    .load = height_window_load,
    .unload = height_window_unload,
  });
    
    /* == GOAL SELECTION WINDOW == */
    // Create main Window element and assign to pointer
  	goal_window = window_create();
    Layer *goal_layer = window_get_root_layer(goal_window);  
    
    window_set_window_handlers(goal_window, (WindowHandlers) {
    .load = goal_window_load,
    .unload = goal_window_unload,
  });
  
    
    // Show the main window on the watch, with animated = true
  	window_stack_push(main_window, true);
}

///////"Signal Processing part"///////////////////////////////////////////////////////
                                                                                    //
                                                                                    //
// 16 bit implementation of an integer square root
// Source : Ross M. Fosler, Microchip Technology Inc
// http://ww1.microchip.com/downloads/en/AppNotes/91040a.pdf
uint16_t int_sqrt32(uint32_t x)
{
    uint16_t res=0;
    uint16_t add= 0b10000000;
    int i;
    for(i=0;i<16;i++)
    {
        uint16_t tmp=res | add;
        uint32_t g2=tmp*tmp;
        if (x>=g2)
        {
            res=tmp;           
        }
        add>>=1;
    }
    return res;
}

// Count the number of steps in a given data window
uint16_t count_steps(uint32_t * data)
{
    bool tot[NB_SAMPLE] = {true};
  
    for (int i = -STEP_ORDER; i <= STEP_ORDER; i++)
    {
        if (i==0)
          continue; //Don't need to compare to self
      
        for (int j=0; j < NB_SAMPLE; j++) 
        {
            tot[j] = tot[j] && (data[j] > data[(j+i)%NB_SAMPLE]);
        }
    }
  
    uint16_t cnt=0;
    for (int l=0; l<NB_SAMPLE; l++)
    {
       if (tot[l] == true)
         cnt++;
    }
  
    return cnt;
  
}

// Windowed average implementation
static uint32_t mov_avg(uint32_t * data, int start_idx)
{
    uint32_t avg = 0;
  
    for (int16_t i = 0; i < WINDOW_SIZE; i++) 
    {
        avg += data[start_idx + i];
    }
  
    // RAPPORT :
    // We chose the window to be a power of 2 so we can leverage
    // the efficiency of bit shift division
    return avg >> WINDOW_SIZE_LOG_2; 
}

// Function called when "num_samples" accelerometer samples are ready
static void accel_data_handler(AccelData *data, uint32_t num_samples)
{
 
  // Store the new accelerometer values with the last NB_SAMPLE ones
  // RAPPORT :
  // We precompute the euclidean distance here. But since this is a costly operation
  // we don't want to perform it all the time (i.e. full data batches at once). So last_data
  // is filled with the last n_samples data at a time
  for (uint32_t i = 0; i < num_samples; i++)
  {
      uint32_t sum_sq = (uint32_t)(data[i].x * data[i].x + data[i].y * data[i].y + data[i].z * data[i].z);
      last_data[idx+i] = int_sqrt32(sum_sq);
  }
  
  // We have enough data to compute the avg
  if (nb_pts%WINDOW_SIZE == 0) 
  {
      last_avg[idx] = mov_avg(last_data, idx);
  }
  
  idx = (idx + num_samples) % NB_SAMPLE;
  nb_pts += num_samples;
  
  // if we have enough samples to check the number of points
  if (nb_pts == NB_SAMPLE) 
  {
      int window_step_count = count_steps(last_data);
      nb_pts = 0;
    
      static char results[60];
      APP_LOG(APP_LOG_LEVEL_INFO, "steps: %d", window_step_count);
      // tab of chars to print the results on the watch
      
      //Print the results on the watch
      snprintf(results, 60, "steps: %d", window_step_count);
      text_layer_set_text(data_layer, results);
  }
  

}                                                                                 //
                                                                                  //
////////////////////////////////////////////////////////////////////////////////////


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