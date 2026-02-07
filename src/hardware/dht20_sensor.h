// Standard Libraries
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>

// Pico SDK
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// define struct to contain data from a single sensor reading
struct dht20_reading {
  float humidity;
  float temperature_c;
  float temperature_f;
};


int setup_sensor(uint sensor_sda_pin, uint sensor_scl_pin, i2c_inst_t *channel);

int reset_sensor_register(uint8_t register_address);

int take_measurement(struct dht20_reading *current_measurement);

uint8_t calculate_crc8(uint8_t *data, int num_bytes);
