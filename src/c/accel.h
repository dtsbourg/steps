#pragma once

#ifndef ACCEL_H
#define ACCEL_H

#include <pebble.h>

void accel_data_handler(AccelData *data, uint32_t num_samples);

uint16_t int_sqrt32(uint32_t x);
uint16_t count_steps(uint32_t * data);

int return_windows_step_count();
  
uint32_t mov_avg(uint32_t * data, int start_idx);

#endif