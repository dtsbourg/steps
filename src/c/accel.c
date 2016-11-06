/*---------------------------------------------------------------------------
    PEBBLE PODOMETER PROJECT
    systems engineering Fall 2016
    GROUP :
        * Sebastien Savidan
        * Jean Gschwind
        * Tristan Besson
        * Dylan Bourgeois

\file accel.c
\description Handles the acceleration data coming from the Pebble.

---------------------------------------------------------------------------*/

#include "accel.h"
#include "display.h"

#define NB_SAMPLE         25
#define DATA_NUM          3
#define WINDOW_SIZE       4
#define WINDOW_SIZE_LOG_2 2
#define STEP_ORDER        5

// Circular array storing the last NB_SAMPLE acceleromter values
static uint32_t last_data[NB_SAMPLE];
static uint32_t last_avg[NB_SAMPLE];
static int idx=0;
static int nb_pts=0;
static int window_step_count = 0;

///< Integer square root implementation
uint32_t wilco_sqrt(uint32_t x);
///< Count the number of steps in a dataframe
uint16_t count_steps(uint32_t * data);
///< Moving average implementation
uint32_t mov_avg(uint32_t * data, int start_idx);


///< Make the number of steps available to display
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

    // We chose the window to be a power of 2 so we can leverage
    // the efficiency of bit shift division
    return avg >> WINDOW_SIZE_LOG_2;
}

// Integer square root
// Source :
#define iter1(N) try = root + (1 << (N)); if (n >= try << (N)) { n -= try << (N); root |= 2 << (N); }
uint32_t wilco_sqrt (uint32_t n)
{
  uint32_t root = 0, try;
  iter1 (15); iter1 (14); iter1 (13); iter1 (12); iter1 (11);
  iter1 (10); iter1 ( 9); iter1 ( 8); iter1 ( 7); iter1 ( 6);
  iter1 ( 5); iter1 ( 4); iter1 ( 3); iter1 ( 2); iter1 ( 1); iter1 ( 0);
  return root >> 1;
}

// Count the number of steps in a given data window
uint16_t count_steps(uint16_t * data)
{
    bool tot[WINDOW_SIZE];
    memset(&tot, true, sizeof(bool) * WINDOW_SIZE);

    for (int i = -STEP_ORDER; i <= STEP_ORDER; i++)
    {
        if (i==0)
          continue; //Don't need to compare to self

        for (int j=0; j < WINDOW_SIZE; j++)
        {
            tot[j] = tot[j] && (data[j] > data[(j+i) % NB_SAMPLE]);
        }
    }

    uint16_t cnt = 0;
    for (int l=0; l<WINDOW_SIZE; l++)
    {
       APP_LOG(APP_LOG_LEVEL_INFO, " tot: %i", tot[l]);
       if (tot[l] == true)
         cnt++;
    }

    return cnt;
}

// Function called when "num_samples" accelerometer samples are ready
static void accel_data_handler(AccelData *data, uint32_t num_samples)
{

  // Store the new accelerometer values with the last NB_SAMPLE ones
  // We precompute the euclidean distance here. But since this is a costly operation
  // we don't want to perform it all the time (i.e. full data batches at once). So last_data
  // is filled with the last num_samples data at a time
    for (uint16_t i = 0; i < num_samples; i++)
    {
        uint32_t sum_sq = (data[i].x * data[i].x + data[i].y * data[i].y + data[i].z * data[i].z);
        last_data[i] = (uint16_t) wilco_sqrt(sum_sq);

        if (i % WINDOW_SIZE == 0 && i > 0)
        {
          last_avg[nb_pts] = mov_avg(last_data, (i - WINDOW_SIZE) % NB_SAMPLE);
          APP_LOG(APP_LOG_LEVEL_INFO, "last_avg: %u", last_avg[nb_pts]);
          nb_pts++;
        }
    }

    if (last_avg[nb_pts] > 1200) {
        window_step_count = window_step_count + count_steps(last_avg);
        nb_pts = 0;
    }

    if(return_display_type() == 1)
      {
      static char data[10];

      APP_LOG(APP_LOG_LEVEL_INFO, "%d", steps_to_goal());
      snprintf(data,10,"%d",steps_to_goal());
      text_layer_set_text(return_display_layer(), data);
    }
    else if(return_display_type() == 2) {
        APP_LOG(APP_LOG_LEVEL_INFO, "steps: %u", window_step_count);
        // tab of chars to print the results on the watch
        static char results[60];
        //Print the results on the watch
        snprintf(results, 60, "steps: %u", window_step_count);
        text_layer_set_text(data_layer, results);
    }

}
