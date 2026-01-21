#include "dht20_sensor.h"


#define DEBUG_SENSOR true // specifically for dht20 sensor debugging

static uint8_t HARDWARE_ADDR = 0x38;

void setup_sensor(uint sensor_sda_pin, uint sensor_scl_pin) {
  i2c_init(i2c_default, 400 * 1000);

  //NOTE: (to self): Per the SDK, the GPIO_FUNC_I2C is enum defining function of the pin

  // define sda & scl pins to function as i2c
  gpio_set_function(sensor_sda_pin, GPIO_FUNC_I2C);
  gpio_set_function(sensor_scl_pin, GPIO_FUNC_I2C);

  // configure pins to pull up
  gpio_pull_up(sensor_sda_pin);
  gpio_pull_up(sensor_scl_pin);

  if (DEBUG_SENSOR) { 
      printf("DHT20 sensor initialized");
  }

  //TODO: what does this do? exactly? need to look into bi_decl(), something to do with 
  //picotool and binaries.
  // Make the I2C pins available to picotool
  // bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
}
