#ifndef __DATA_FLOW__
#define __DATA_FLOW__

#include <stdint.h>
#include "pico/multicore.h"

// Payload Data Struct for exchanging data between Core0 and Core1
typedef struct {
    volatile uint64_t time_stamp;
    volatile uint16_t ADC_Data; // this only has 12 bits of precision, we lose 4 bits
    volatile uint32_t Humidity_Data; // place holders for when we implement this
    volatile uint32_t Temperature_Data;
} Payload_Data;

#endif