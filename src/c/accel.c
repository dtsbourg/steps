#include <pebble.h>
#include "accel.h"
#include "display.h"

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
#define NB_MIN_HEIGHT 140
#define STRIDE_FACTOR_M 415
#define STRIDE_FACTOR_F 413

#define NB_GOAL_ITEMS 6

// Circular array storing the last NB_SAMPLE acceleromter values
static uint32_t last_data[NB_SAMPLE];
static uint32_t last_avg[NB_SAMPLE];
static int idx=0;
static int nb_pts=0;
static int window_step_count = 0;

///////"Signal Processing part"///////////////////////////////////////////////////////
                                                                                    
                                                                                    
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

int return_windows_step_count(){
  return window_step_count;  
}

// Windowed average implementation
uint32_t mov_avg(uint32_t * data, int start_idx)
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
void accel_data_handler(AccelData *data, uint32_t num_samples)
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
      window_step_count = count_steps(last_data);
      nb_pts = 0;
    
      static char results[60];
      APP_LOG(APP_LOG_LEVEL_INFO, "%d", window_step_count);
      // tab of chars to print the results on the watch

      //Print the results on the watch
      snprintf(results, 60, "%d", window_step_count);
      text_layer_set_text(return_data_layer(), results);
      
      if(return_display_type() == 1)
        {
        static char data[10];
       
        APP_LOG(APP_LOG_LEVEL_INFO, "%d", steps_to_goal());
        snprintf(data,10,"%d",steps_to_goal());
        text_layer_set_text(return_display_layer(), data);
      }
      else if(return_display_type() == 2)
        {
        static char data[10];
       
        APP_LOG(APP_LOG_LEVEL_INFO, "%d", distance());
        snprintf(data,10,"%d",distance());
        text_layer_set_text(return_display_layer(), data);
      }
  }
  

}                                                                               
                                                                               
////////////////////////////////////////////////////////////////////////////////////