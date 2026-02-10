#ifndef __DATA_FLOW__
#define __DATA_FLOW__

#include <stdint.h>
#include "pico/multicore.h"

// DHT20_Reading struct to contain temp & humidity measurements for a single data point
typedef struct {
  float humidity;
  float temperature_c;
  float temperature_f;
} DHT20_Reading;

// Payload Data Struct for exchanging data between Core0 and Core1
typedef struct {
    volatile uint64_t time_stamp;
    volatile uint16_t ADC_Data; // this only has 12 bits of precision, we lose 4 bits
    volatile DHT20_Reading *DHT20_Data;   //  store temp & humidity sensor data
    volatile int DHT20_Data_Valid;     
} Payload_Data;

#endif

