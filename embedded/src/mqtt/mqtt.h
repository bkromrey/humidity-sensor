// Standard Libraries
#include <stdio.h>
//#include <stdint.h>

// Pico SDK
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"      // wifi chip
#include "lwip/apps/mqtt.h"       // lightweight IP mqtt
#include "lwip/apps/mqtt_priv.h"  // needed when verifiying mqtt client connect state

//typedef struct {
//  int wifi_connected; 
//  ip_addr_t *server_ip;
//  //mqtt_client_t *mqtt_client;
//} MQTT_Client;

// Initialization Functions
int init_wifi();
int init_mqtt();
int init_sensor_id();  // TODO: rework this, don't think this will be needed
int Init_Network_Comms();

// Callback Functions used by lwIP MQTT Functions
void callback_mqtt_publish(void *arg, err_t err);
void callback_mqtt_connect(mqtt_client_t *mqtt_client, void *arg, mqtt_connection_status_t status);

// Communications With MQTT Broker
int generate_JSON();
int publish_data(char *topic, char *payload, uint16_t payload_len);

