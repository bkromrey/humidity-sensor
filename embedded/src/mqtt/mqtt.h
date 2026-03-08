// Pico SDK & Standard Libraries
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"      // wifi chip
#include "lwip/apps/mqtt.h"       // lightweight IP mqtt
#include "lwip/apps/mqtt_priv.h"  // needed when verifiying mqtt client connect state

// Our Modules
#include "../data_flow/data_flow.h"


int Init_Network_Comms();
int Publish_Data(const Payload_Data *Sensor_Data);

// Initialization Helper Functions
int init_wifi();
int init_mqtt();
int reconnect_mqtt();

// Callback Functions used by lwIP MQTT Functions
void callback_mqtt_publish(void *arg, err_t err);
void callback_mqtt_connect(mqtt_client_t *mqtt_client, void *arg, mqtt_connection_status_t status);

// Communication Helper Functions 
int generate_payload(const Payload_Data *Sensor_Data, char json_payload[]);
