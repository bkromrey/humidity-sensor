#ifndef __CORE_1_H__
#define __CORE_1_H__

// Standard Library
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Pico SDK
#include "pico/time.h"
#include "hardware/sync.h"

// User Modules
#include "../data_flow/data_flow.h"
#include "../hardware/photores.h"

// Global data types
typedef struct {
    volatile uint16_t head;
    volatile uint16_t tail;
    Payload_Data *buffer;
} Ring_Buffer;

// Main Process
void Core_1_Entry(void);

// Defines
#define DATA_BUFFER_SIZE 100

// Shared Variables
extern Payload_Data Data_Buffer[DATA_BUFFER_SIZE];
extern Ring_Buffer Data_Ring_Buffer;

#endif