#include "dht20_sensor.h"


#define DEBUG_SENSOR true// specifically for dht20 sensor debugging

// FROM THE DHT20 DATA SHEET
// 1.After power-on, wait no less than 100ms. Before reading the temperature and 
// humidity value, get a byte of status word by sending 0x71. If the status word
// and 0x18 are not equal to 0x18, initialize the 0x1B, 0x1C, 0x1E registers,
// details Please refer to our official website routine for the initialization
// process; if they are equal, proceed to the next step.

// 2.Wait 10ms to send the 0xAC command (trigger measurement). This command 
// parameter has two bytes, the first byte is 0x33, and the second byte is 0x00.

// 3.Wait 80ms for the measurement to be completed, if the read status word 
// Bit [7] is 0, it means the measurement is completed, and then six bytes can 
// be read continuously; otherwise, continue to wait.

// 4.After receiving six bytes, the next byte is the CRC check data. The user
// can read it out as needed. If the receiving end needs CRC check, an ACK will
// be sent after the sixth byte is received. Reply, otherwise send NACK to end,
// the initial value of CRC is 0XFF, and the CRC8 check polynomial is:
// CRC [7:0] = 1+X4+X5+X8

// 5.Calculate the temperature and humidity value


// pins 6 & 7 (GPIO 4 & 5)  are on I2C0
i2c_inst_t * i2c_channel = i2c0;

// sensor address - this must be 7 bytes long
static const uint8_t HARDWARE_ADDR = 0x38;

static const uint8_t READY_STATUS = 0x18;

//static const uint8_t[] TRIGGER_MEASUREMENT = 0xAC; // has two byte parameter 0x33 and 0x00
//static const 
const uint8_t TRIGGER_MEASUREMENT = { 0xAC, 0x33, 0x00 };
//const uint8_t TRIGGER_MEASUREMENT[] = { 0xAC, 0x33, 0x00 };

// returns the number of bytes read. stores read contents into response.
int take_measurement(){

  uint8_t raw_data[8];

  // first, wait 10ms to send 0xAC
  sleep_ms(10);

  // send the command to trigger measurement
  int bytes_written = i2c_write_blocking(i2c_channel, HARDWARE_ADDR, &TRIGGER_MEASUREMENT, 3, 0);
  
  // wait 80 ms for the sensor to take the measurement
  sleep_ms(80);

  // read the status word to see if measurement has completed (bit 7 should be 0)
  int bytes_read = i2c_read_blocking(i2c_channel, HARDWARE_ADDR, raw_data, 1, 0);

  if (DEBUG_SENSOR) { 
    printf("read %d bytes from sensor\r\n", bytes_read);
    printf("response is: %x\r\n", raw_data[0]);
  }


  //uint8_t response[8];
  //int bytes_read = i2c_read_blocking(i2c_channel, HARDWARE_ADDR, response, 1, 0);

  //int i2c_write_blocking (i2c_inst_t * i2c, uint8_t addr, const uint8_t * src, size_t len, bool nostop)
  //int bytes_written = i2c_write_blocking(i2c_bus, HARDWARE_ADDR, &GET_STATUS_WORD, 1, false);

  return bytes_written;
}



int setup_sensor(uint sensor_sda_pin, uint sensor_scl_pin) {
  bool sensor_ready = false;
  
  if (DEBUG_SENSOR) {
    sleep_ms(5000);   // sleep long enough to catch logging
    printf("starting to initialize humidity sensor...\r\n");
  }

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

  if (DEBUG_SENSOR) {
    printf("getting status of register...\r\n");
  }

  // datasheet is misleading. says to send status word of 0x71, but that really
  // is just a 7-bit 0x38 (sensor's address) plus a read bit of '1' tacked onto
  // the end - meaning we just need to do a read on the sensor.
  // https://www.raspberrypi.com/documentation/pico-sdk/hardware.html#group_hardware_i2c_1gaba4ea440285edec58eba507308c568c6

  uint8_t response = 0;
  int bytes_read = i2c_read_blocking(i2c_channel, HARDWARE_ADDR, &response, 1, 0);

  if (DEBUG_SENSOR) { 
    printf("read %d bytes from sensor\r\n", bytes_read);
    printf("response is: %x\r\n", response);
  }

  // ensure we actually read something
  if (bytes_read < 1){
    return 1;
  }

  // sensor's replay should equal 0x18, otherwise need to initialize registers
  if (response |= READY_STATUS){
    sensor_ready = true;
  } 
 
  // TODO: calibrate sensor if it returns anything other than 0x18

  if (DEBUG_SENSOR) { 
    if (sensor_ready) {
      printf("DHT20 sensor initialized\r\n");
    } else {
      printf("DHT20 sensor failed to initialize!\r\n");
    }
  }

  return sensor_ready;
}

