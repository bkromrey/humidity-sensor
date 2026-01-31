#include "dht20_sensor.h"

#define DEBUG_SENSOR false  // whether to print sensor readings 
#define DEBUG_SENSOR_VERBOSE false// whether to print raw data readings etc.


// pins 6 & 7 (GPIO 4 & 5) are on I2C0
i2c_inst_t * i2c_channel = i2c0;

static const uint8_t HARDWARE_ADDR = 0x38;                            // sensor address 
static const uint8_t READY_STATUS = 0x18;                             // sensor sends this when ready to take a measurement
static const uint8_t TRIGGER_MEASUREMENT = { 0xAC, 0x33, 0x00 };      // has two byte parameter 0x33 and 0x00




// TODO: clean up docstring
// returns the number of bytes read. stores read contents into response. 
int take_measurement(struct sensor_reading * current_data){

  uint8_t raw_data[8];

  // first, wait 10ms to send 0xAC
  sleep_ms(10);

  // send the command to trigger measurement
  int bytes_written = i2c_write_blocking(i2c_channel, HARDWARE_ADDR, &TRIGGER_MEASUREMENT, 3, 0);
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

  // TODO: check CRC validity - DHT20 sensor uses CRC8/NRSC-5 (x⁸ + x⁵ + x⁴ + 1)
  uint8_t crc_data = raw_data[7];

  // 4.After receiving six bytes, the next byte is the CRC check data. The user
  // can read it out as needed. If the receiving end needs CRC check, an ACK will
  // be sent after the sixth byte is received. Reply, otherwise send NACK to end,
  // the initial value of CRC is 0XFF, and the CRC8 check polynomial is:
  // CRC [7:0] = 1+X4+X5+X8


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
  current_data->humidity = (raw_humidity / denominator) * 100;
  current_data->temperature_c = (raw_temp / denominator ) * 200 - 50;
  current_data->temperature_f = (current_data->temperature_c * 1.8) + 32;

  #if DEBUG_SENSOR
  printf("HUMIDITY: %f %%\tTEMP: %f °C (%f °F)\r\n", current_data->humidity, current_data->temperature_c, current_data->temperature_f);
  #endif

  return bytes_written;
}


int setup_sensor(uint sensor_sda_pin, uint sensor_scl_pin) {
  bool sensor_ready = false;
  
  #if DEBUG_SENSOR
  sleep_ms(5000);   // sleep long enough to catch logging
  printf("initializing humidity sensor...\r\n");
  #endif

  // most devices clock at either 100 or 400 kHertz. SDK says controller does
  // not support high speed mode (though other sources on the internet indicate
  // that it does), so initialize at standard rate to be on the safe side
  i2c_init(i2c_channel, 100000);

  // define sda & scl pins to function as i2c
  //NOTE: (to self): Per the SDK, the GPIO_FUNC_I2C is enum defining function of the pin
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

  // sensor's replay should equal 0x18, otherwise need to initialize registers
  if (response |= READY_STATUS){
    sensor_ready = true;
  } 
 
  // TODO: calibrate sensor if it returns anything other than 0x18
  // FROM THE DHT20 DATA SHEET
  // 1.After power-on, wait no less than 100ms. Before reading the temperature and 
  // humidity value, get a byte of status word by sending 0x71. If the status word
  // and 0x18 are not equal to 0x18, initialize the 0x1B, 0x1C, 0x1E registers,
  // details Please refer to our official website routine for the initialization
  // process; if they are equal, proceed to the next step.
  
  #if DEBUG_SENSOR 
  if (sensor_ready) {
    printf("DHT20 sensor initialized\r\n");
  } else {
    printf("DHT20 sensor failed to initialize!\r\n");
  }
  #endif

  return sensor_ready;
}

