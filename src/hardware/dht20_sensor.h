// Standard libraries
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>

// Pico SDK
#include "pico/stdlib.h"
#include "hardware/i2c.h"

int setup_sensor(uint sensor_sda_pin, uint sensor_scl_pin);

int take_measurement();
