#ifndef __DATA_FLOW__
#define __DATA_FLOW__

#include <stdint.h>

typedef struct {
    uint64_t time_stamp;
    uint16_t ADC_Data; // this only has 12 bits of precision, we lose 4 bits
} Payload_Data;

#endif