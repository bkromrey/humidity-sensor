// Standard Libraries
#include <stdio.h>
//#include <stdint.h>

// Pico SDK
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"    // wifi chip
#include "lwip/apps/mqtt.h"     // (Lightweight IP mqtt)

typedef struct {
  ip_addr_t *server_ip;
} MQTT_Client;

//
int init_wifi();

int generate_JSON();


int test_mqtt();

