/*---------------------------------------------------------------------------
    PEBBLE PODOMETER PROJECT
    systems engineering Fall 2016
    GROUP :
        * Sebastien Savidan
        * Jean Gschwind
        * Tristan Besson
        * Dylan Bourgeois

\file accel.h
\description Handles the acceleration data coming from the Pebble.

---------------------------------------------------------------------------*/

#pragma once

#ifndef ACCEL_H
#define ACCEL_H

/// Include Pebble library
#include <pebble.h>

/// Include Standard lib
#include <stdbool.h>

/// Main data handler
void accel_data_handler(AccelData *data, uint32_t num_samples);

/// Helpers
///< Returns the number of steps counted in the acc data
int return_windows_step_count();

#endif
