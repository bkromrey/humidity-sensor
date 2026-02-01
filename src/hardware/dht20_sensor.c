#include "dht20_sensor.h"

#define DEBUG_SENSOR false  // whether to print sensor readings 
#define DEBUG_SENSOR_VERBOSE false// whether to print raw data readings etc.


// pins 6 & 7 (GPIO 4 & 5) are on I2C0
i2c_inst_t * i2c_channel = i2c0;

static const uint8_t HARDWARE_ADDR = 0x38;                            // sensor address 
static const uint8_t READY_STATUS = 0x18;                             // sensor sends this when ready to take a measurement
static const uint8_t TRIGGER_MEASUREMENT[3] = { 0xAC, 0x33, 0x00 };      // has two byte parameter 0x33 and 0x00
static uint8_t INITIAL_CRC_VAL = 0xFF;
static const uint8_t CRC_POLYNOMIAL = 0x31;                           //  CRC8 check polynomial is CRC [7:0] = 1+X4+X5+X8


// TODO: calibrate sensor if it returns anything other than 0x18
// From the DHT20 datasheet, to calibrate: "... initialize the 0x1B, 0x1C, 0x1E registers,
// details Please refer to our official website routine for the initialization process."
int reset_sensor_register(uint8_t register_address){

  uint8_t register_data[3];
  uint8_t calibration_data[3] = {register_address, 0x00, 0x00};


  // send calibration data to register being calibrated
  int bytes_written = i2c_write_blocking(i2c_channel, HARDWARE_ADDR, calibration_data, 3, 0);
  if (bytes_written < 0){
    return 1;
  }

  sleep_ms(5);

  // read 3 bytes from register. first byte will be ignored/overwritten before data is sent back
  int bytes_read = i2c_read_blocking(i2c_channel, HARDWARE_ADDR, register_data, 3, 0);
  if (bytes_read < 0){
    return 1;
  }

  sleep_ms(10);

  // we need to OR 0x80 and the address of the register, and then send it back with the 2nd & 3rd bytes we just recieved per vendor example
  register_data[0] = register_address | 0x80;
  bytes_written = i2c_write_blocking(i2c_channel, HARDWARE_ADDR, register_data, 3, 0);
  if (bytes_written < 0){
    return 1;
  }

  return 0;
}


int setup_sensor(uint sensor_sda_pin, uint sensor_scl_pin) {
  bool sensor_ready = false;
  
  #if DEBUG_SENSOR
  sleep_ms(5000);   // sleep long enough to catch logging
  printf("initializing humidity sensor...\r\n");
  #endif

  // most devices clock at either 100 or 400 kHertz. SDK says controller does
  // not support high speed mode (though other sources on the internet indicate
  // that it does) - setting to 400 kHz, but if we have issues, bump back down to 100 kHz 
  i2c_init(i2c_channel, 400000);

  // define sda & scl pins to function as i2c
  gpio_set_function(sensor_sda_pin, GPIO_FUNC_I2C);
  gpio_set_function(sensor_scl_pin, GPIO_FUNC_I2C);

  // configure pins high
  gpio_pull_up(sensor_sda_pin);
  gpio_pull_up(sensor_scl_pin);

  // sleep at minimum 100ms per datasheet
  sleep_ms(100);

  #if DEBUG_SENSOR_VERBOSE
  printf("getting status of register...\r\n");
  #endif

  // datasheet is misleading. says to send status word of 0x71, but that really
  // is just a 7-bit 0x38 (sensor's address) plus a read bit of '1' tacked onto
  // the end - meaning we just need to do a read on the sensor.

  uint8_t response = 0;
  int bytes_read = i2c_read_blocking(i2c_channel, HARDWARE_ADDR, &response, 1, 0);

  #if DEBUG_SENSOR_VERBOSE
  printf("response is: %x\r\n", response);
  #endif

  // error if we cannot read anything
  if (bytes_read < 1){
    return 1;
  }

  // sensor's replay should equal 0x18, otherwise need to initialize registers to calibrate
  if (response |= READY_STATUS){
    sensor_ready = true;
  } else{
    // per datasheet, the following 3 registers need to be initialized/reset in order to calibrate
    reset_sensor_register(0x1B);
    reset_sensor_register(0x1C);
    reset_sensor_register(0x1E);
    sensor_ready = true;
  }
 
  #if DEBUG_SENSOR 
  if (sensor_ready) {
    printf("DHT20 sensor initialized\r\n");
  } else {
    printf("DHT20 sensor failed to initialize!\r\n");
  }
  #endif

  return sensor_ready;
}

// check CRC validity - DHT20 sensor uses CRC8/NRSC-5 (x⁸ + x⁵ + x⁴ + 1), which is 0x31
uint8_t calculate_crc8(uint8_t * data, int num_bytes){

  uint8_t crc = INITIAL_CRC_VAL;

  // look at one byte at a time
  for (int b = 0; b < num_bytes; b++){

    // put current byte into crc with xor current byte into crc
    crc = crc ^ data[b];

    // and within a byte, look one bit at a time
    for (int c = 0; c < 8; c++){
      
      // most significant bit of crc is a 1 - shift by one bit & xor with check polynomial
      if (crc & 0x80){
        crc = crc << 1;
        crc = crc ^ CRC_POLYNOMIAL;
     
      // most significant bit of crc is a 0 - shift by one (no xor)
      } else {
        crc = crc << 1;
      }      
    }
  }

  #if DEBUG_SENSOR_VERBOSE
  printf("crc is: %x\r\n", crc);
  #endif

  return crc;
}

// TODO: clean up docstring
// returns the number of bytes read. stores read contents into response. 
int take_measurement(struct dht20_reading * current_measurement){

  uint8_t raw_data[8];

  // first, wait 10ms to send 0xAC
  sleep_ms(10);

  // send the command to trigger measurement
  int bytes_written = i2c_write_blocking(i2c_channel, HARDWARE_ADDR, TRIGGER_MEASUREMENT, 3, 0);
  if (bytes_written < 1){
    return 1;
  }

  // the MSB is set when the sensor has completed its reading
  while (raw_data[0] & (1 << 7) == 0){
  
    // wait 80 ms for the sensor to take the measurement, per datasheet
    sleep_ms(80);

    // read the status word to see if measurement has completed (bit 7 should be 0)
    int bytes_read = i2c_read_blocking(i2c_channel, HARDWARE_ADDR, raw_data, 1, 0);
    if (bytes_read < 1){
      return 1;
    }
  }
  
  // read 6 bytes of data + 1 byte CRC
  int bytes_read = i2c_read_blocking(i2c_channel, HARDWARE_ADDR, &raw_data[1], 7, 0);
  if (bytes_read < 1){
    return 1;
  }

  #if DEBUG_SENSOR_VERBOSE
  printf("raw data: %x %x %x %x %x %x [CRC: %x]\r\n", raw_data[1], raw_data[2], raw_data[3], raw_data[4], raw_data[5], raw_data[6], raw_data[7]);
  #endif

  // validate data via CRC and exit function with an error if data isn't valid
  uint8_t calculated_crc = calculate_crc8(&raw_data[1], 6);

  #if DEBUG_SENSOR_VERBOSE
  printf("the calculated CRC is: %x\r\n", calculated_crc);
  #endif

  if (raw_data[7] != calculated_crc){
    return 1;
  }

  // make a copy of the byte to be split in half so bitwise operations don't mess with data
  uint8_t half_byte = raw_data[4];
  half_byte = half_byte << 4;
  half_byte = half_byte >> 4;

  // get raw humidity
  uint32_t raw_humidity = 0;
  raw_humidity += raw_data[2] << 12;
  raw_humidity += raw_data[3] << 4;
  raw_humidity += raw_data[4] >> 4;

  // get raw temperature
  uint32_t raw_temp = 0;
  raw_temp += half_byte << 16;  
  raw_temp += raw_data[5] << 8; 
  raw_temp += raw_data[6];

  #if DEBUG_SENSOR_VERBOSE 
  printf("raw humidity: %" PRIu32 "\r\n", raw_humidity); 
  printf("raw temp: %" PRIu32 "\r\n", raw_temp); 
  #endif

  // formulas for humidity & temperature taken from datasheet 
  float denominator = pow(2, 20);
  current_measurement->humidity = (raw_humidity / denominator) * 100;
  current_measurement->temperature_c = (raw_temp / denominator ) * 200 - 50;
  current_measurement->temperature_f = (current_measurement->temperature_c * 1.8) + 32;

  #if DEBUG_SENSOR
  printf("HUMIDITY: %f %%\tTEMP: %f °C (%f °F)\r\n", current_data->humidity, current_data->temperature_c, current_data->temperature_f);
  #endif

  return bytes_written;
}
