// Standard Libraries
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>

// Pico SDK
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "../data_flow/data_flow.h"

// Structure definition moved to data_flow.h to avoid cyclical dependencies

int setup_sensor(uint sensor_sda_pin, uint sensor_scl_pin, i2c_inst_t *channel);

int reset_sensor_register(uint8_t register_address);

int take_measurement(DHT20_Reading *current_measurement);

uint8_t calculate_crc8(uint8_t *data, int num_bytes);
