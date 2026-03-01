#ifndef __CORE_1_H__
#define __CORE_1_H__

// Standard Library
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Pico SDK
#include "pico/time.h"
#include "hardware/sync.h"


// User Modules
#include "../data_flow/data_flow.h"
#include "../hardware/photores.h"
#include "../hardware/dht20_sensor.h"

// Main Process
void Core_1_Entry(void);

#endif
