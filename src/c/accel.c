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

/// Include user headers
#include <src/c/display.h>
#include <src/c/accel.h>

#define NB_SAMPLE         25

#define WINDOW_SIZE       4
#define WINDOW_SIZE_LOG_2 2

#define STEP_ORDER        2
#define NOISY_DATA        1050


///< Circular array storing the last NB_SAMPLE normed acceleromter values
static uint32_t last_data[NB_SAMPLE];
///< Circular array storing the last NB_SAMPLE averaged and normed acceleromter values
static uint16_t last_avg[NB_SAMPLE];
///< Number of steps counted
static uint16_t window_step_count = 0;

///< Integer square root implementation
uint32_t wilco_sqrt(uint32_t x);
///< Count the number of steps in a dataframe
uint16_t count_steps(uint16_t * data);
///< Moving average implementation
uint32_t mov_avg(uint32_t * data, int start_idx);


/// Make the number of steps available to display
int return_windows_step_count(){
  return window_step_count;
}

/// Windowed average implementation
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

/// Integer square root
// Source : http://www.verycomputer.com/24_e95a6e361498c566_1.htm
#define iter1(N) try = root + (1 << (N)); if (n >= try << (N)) { n -= try << (N); root |= 2 << (N); }
uint32_t wilco_sqrt (uint32_t n)
{
  uint32_t root = 0, try;
  iter1 (15); iter1 (14); iter1 (13); iter1 (12); iter1 (11);
  iter1 (10); iter1 ( 9); iter1 ( 8); iter1 ( 7); iter1 ( 6);
  iter1 ( 5); iter1 ( 4); iter1 ( 3); iter1 ( 2); iter1 ( 1); iter1 ( 0);
  return root >> 1;
}

/// Count the number of steps in a given data window
/// See report for detailed explanation of algorithm
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
       if (tot[l] == true && data[l] > NOISY_DATA)
         cnt++;
    }

    return cnt;
}

/// Accelerometer data handler
/// NB: called when "num_samples" accelerometer samples are ready 
void accel_data_handler(AccelData *data, uint32_t num_samples)
{
  // Store the new accelerometer values with the last NB_SAMPLE ones
  // We precompute the euclidean distance here. But since this is a costly operation
  // we don't want to perform it all the time (i.e. full data batches at once). So last_data
  // is filled with the last num_samples data at a time
    for (uint16_t i = 0; i < num_samples; i++)
    {
        uint32_t sum_sq = (data[i].x * data[i].x + data[i].y * data[i].y + data[i].z * data[i].z);
        last_data[i] = (uint16_t) wilco_sqrt(sum_sq);
    }

    // Compute the moving average for the last data
    for (int j = 0; j < WINDOW_SIZE; j++) {
        last_avg[j] = mov_avg(last_data, j*WINDOW_SIZE);
    }
  
    // Check if we saw some steps if the last window and incremeent the step counter
    window_step_count = window_step_count + count_steps(last_avg);

    // Display the results
    if(return_display_type() == DISPLAY_GOAL)
    {
      static char data[10];

      APP_LOG(APP_LOG_LEVEL_INFO, "%d", steps_to_goal());
      snprintf(data,10,"%d",steps_to_goal());
      text_layer_set_text(return_display_layer(), data);
    }
    else 
    {
        APP_LOG(APP_LOG_LEVEL_INFO, "steps: %u", window_step_count);
        // tab of chars to print the results on the watch
        static char results[60];
        //Print the results on the watch
        snprintf(results, 60, "steps: %u", window_step_count);
        text_layer_set_text(return_data_layer(), results);
    }

}
