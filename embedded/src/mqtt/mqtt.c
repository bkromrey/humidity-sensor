#include "mqtt.h"

#define DEBUG_MQTT 1

#define WIFI_TIMEOUT 120000       // how many ms to wait before failing to connect

// TODO: this bit right here, 
#ifndef PICO_MQTT_SERVER
#error Need to define PICO_MQTT_SERVER
#endif

int test_mqtt(){
  sleep_ms(3000);
  printf("MQTT Server is: %s\n", PICO_MQTT_SERVER);
  
  int test_init_err = init_wifi();     // must init before getting mac
  
  #if DEBUG_MQTT
    if (test_init_err)
      printf("init error: %d\n", test_init_err);
    else 
      printf("init success\n");
  #endif

  uint8_t mac_address[6];
  cyw43_wifi_get_mac(&cyw43_state,CYW43_ITF_STA, mac_address);

  #if DEBUG_MQTT
  printf("MAC Address is: %02x:%02x:%02x:%02x:%02hhx:%02hhx \n", 
         mac_address[0], mac_address[1], mac_address[2], 
         mac_address[3], mac_address[4], mac_address[5]);
  #endif


  return 0;
}

int init_wifi(){

  #if DEBUG_MQTT
  printf("Initializing wifi driver\n");
  #endif

  // initilize the wifi (CYW43) driver
  if (cyw43_arch_init()){
    return 1;     
  };

  
  #if DEBUG_MQTT
  printf("Connecting to wifi...\n");
  #endif

  // set to station mode and then attempt to connet to wifi
  cyw43_arch_enable_sta_mode();
  if (cyw43_arch_wifi_connect_timeout_ms(PICO_WIFI_SSID, PICO_WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, WIFI_TIMEOUT)){
    return 1;
  }

  


  return 0;
}
