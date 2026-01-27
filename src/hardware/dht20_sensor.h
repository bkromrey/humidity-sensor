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
//int take_measurement(i2c_inst_t * bus, uint8_t sensor_addr, uint8_t read_from_register, int num_bytes, uint8_t * response);
