#include "mqtt.h"

#define DEBUG_MQTT 1

// TODO: is there a way to disable cyw43 debug logging
//#define PICO_CYW43_LOGGING_ENABLED 0

#define WIFI_TIMEOUT 120000       // how many ms to wait before failing to connect
#define PICO_MQTT_PORT 1883       // 1883 is the standard MQTT port
#define MQTT_KEEP_ALIVE 60        // keepalive in seconds
#define MQTT_QOS 0
#define MQTT_RETAIN 0

// TODO: this bit right here, 
#ifndef PICO_MQTT_SERVER
#error Need to define PICO_MQTT_SERVER
#endif


static mqtt_client_t *mqtt_client;
static uint8_t mac_address_bytes[6];
static char mac_address[12];
static char sensor_id[8];
static struct mqtt_connect_client_info_t client_info;
static int connected_to_broker = 0;  // bool?

// ---------------------------------------------------------------------------
// PUBLIC FUNCTIONS
// ---------------------------------------------------------------------------

/** The Init_Network_Comms function is to be called from the main/core0 loop
 * during the initialization stage of program startup. This function will call
 * helper methods to connect to the wireless network and then connect to the 
 * MQTT broker.
 *
 * Both the wireless network and MQTT server's connection variables should be 
 * either defined in the environment or passed through to CMake at build time.
 *
 * @envar PICO_WIFI_SSID   The network name
 * @envar PICO_WIFI_PASS   The WPA2 password for the wireless network
 * @envar PICO_MQTT_SERVER The IP address of the MQTT server/broker
 * @envar PICO_MQTT_USER   The username to use to authenticate this MQTT client
 * @envar PICO_MQTT_PASS   The password to use to authenticate this MQTT client
 *
 * Returns 0 on a successful init, otherwise returns 1.
 */
int Init_Network_Comms(){

  // initialize & connect to wifi - this must be done before getting MAC address 
  if (init_wifi()){
    #if DEBUG_MQTT
    printf("Error initializing & connecting to wifi network\n");
    #endif
  }

  // TODO: change sensor ID to something passed from envar
  // get MAC address as sensor id
  //init_sensor_id();

  // initialize connection to the MQTT broker
  init_mqtt();

  // blocking - wait for the callback function to update status before continuing
  while (connected_to_broker == 0){
    sleep_ms(10);
    #if DEBUG_MQTT
    printf("attempting to connect to mqtt broker...\n");
    #endif
  }

  if (connected_to_broker == -1){
    #if DEBUG_MQTT
    printf("Failed to connect to MQTT broker at %s\n", PICO_MQTT_SERVER);
    #endif
    return 1;
  }


  // NOTE: everything below this is debug only and should be removed
  Publish_Data("/test", "testing more data", 34); 

  sleep_ms(10000);
  printf("\n-----------------\n\n");
  return 0;
}

//TODO: docstring
int Publish_Data(char *topic, char *payload, uint16_t payload_len){

  //ensure still connected to mqtt broker before attempting to send
  if (mqtt_client_is_connected(mqtt_client) == 0){
    #if DEBUG_MQTT
    printf("ERROR SENDING DATA: MQTT client no longer connected\n");
    #endif

    return 1;
  }
  printf("connected state is: %d\n", mqtt_client->conn_state);


  if (mqtt_publish(mqtt_client, topic, payload, payload_len, MQTT_QOS, MQTT_RETAIN, callback_mqtt_publish, 0) != ERR_OK){
    return 1;
    #if DEBUG_MQTT
    printf("ERROR: Unable to publish data to MQTT broker.\n");
    #endif
  }


  return 0;

}

// ---------------------------------------------------------------------------
// PRIVATE FUNCTIONS
// ---------------------------------------------------------------------------


/**
  * Attempts to connect the Pico board to the wireless network using environment
  * variables defined either in the environment or passed through to CMake when
  * building this program.
  *
  * @envar PICO_WIFI_SSID   The network name
  * @envar PICO_WIFI_PASS   The WPA2 password for the wireless network
  * 
  * Returns 0 if successful, or 1 if there were any errors.
  */
int init_wifi(){

  // initilize the wifi (CYW43) driver
  if (cyw43_arch_init())
    return 1;     
  
  // set to station mode and then attempt to connect to wifi
  cyw43_arch_enable_sta_mode();
  if (cyw43_arch_wifi_connect_timeout_ms(PICO_WIFI_SSID, PICO_WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, WIFI_TIMEOUT))
    return 1;
  
  // wifi connected successfully
  return 0;
}


/**
 * Attempts to connect to the specificed MQTT broker/server. Defaults to the 
 * standard MQTT port of 1883.
 *
 * The MQTT server's connection variables should be defined either in the
 * environment or passed through to CMake at build time.
 *
 * @envar PICO_MQTT_SERVER The IP address of the MQTT server/broker
 * @envar PICO_MQTT_USER   The username to use to authenticate this MQTT client
 * @envar PICO_MQTT_PASS   The password to use to authenticate this MQTT client
 *
 * Returns a 0 if the connection is successful, otherwise returns a 1.
 */
int init_mqtt(){
  
  #if DEBUG_MQTT
  printf("MQTT Server is: %s\n", PICO_MQTT_SERVER);
  #endif

  // TODO: add error checking here
  mqtt_client = mqtt_client_new();
  
  // set client connection parameters
  client_info.client_id = sensor_id;
  client_info.client_user = PICO_MQTT_USER;
  client_info.client_pass = PICO_MQTT_PASS;
  client_info.keep_alive = MQTT_KEEP_ALIVE;
  //client_info.will_topic;   // TODO: implement last will
  //client_info.will_msg;
  //client_info.will_qos;
  //client_info.will_retain;
  //client_info.tls_config = NULL; // altcp_tls_create_config_client(NULL, 0);

  // convert IP address from a string before passing through to the mqtt connect call
  ip_addr_t server_ip;
  ipaddr_aton(PICO_MQTT_SERVER, &server_ip);

  cyw43_arch_lwip_begin();

  if (mqtt_client_connect(mqtt_client, &server_ip, PICO_MQTT_PORT, callback_mqtt_connect, 0, &client_info) != ERR_OK){
    #if DEBUG_MQTT
    printf("ERROR initiating connection to MQTT server!\n");
    #endif
    return 1;
  }

  cyw43_arch_lwip_end();

 
  return 0;
}

// TODO: this entire function is probably not needed, if we allow user
// definable sensor id
int init_sensor_id(){
 
  // uint8_t mac_address_bytes[6];
  cyw43_wifi_get_mac(&cyw43_state,CYW43_ITF_STA, mac_address_bytes);

 // char mac_address[12]; 
  sprintf(mac_address, "%02x%02x%02x%02x%02x%02x", 
         mac_address_bytes[0], mac_address_bytes[1], mac_address_bytes[2], 
         mac_address_bytes[3], mac_address_bytes[4], mac_address_bytes[5]);

 // char sensor_id[8];
  sprintf(sensor_id, "PICO%02X%02X", mac_address_bytes[4], mac_address_bytes[5]);

  #if DEBUG_MQTT
  printf("MAC Address is: %s\n", mac_address); 
  printf("Using %s as the sensor id.\n", sensor_id);
  #endif

  return 0;
}

// called when the mqtt publish function finishes


/* Callback function for the mqtt_publish function. This function is called
 * after the MQTT publish request completes, regardless of if that publish
 * completed successfully. When debug mode is enabled, logs errors to the
 * terminal.
 *
 * @param arg         Any arguments passed to this callback from calling function
 *                    (currently none)
 * @param err         Error enum value indicating success or failure.
 */
void callback_mqtt_publish(void *arg, err_t err){
  #if DEBUG_MQTT
  if (err != ERR_OK)
    printf("ERROR: Failed to publish mqtt message!\n");
  #endif
}


/**
 * Callback function for the mqtt_client_connect function. This function is
 * called after the MQTT connection attempt completes, regardless of if that
 * connection request was successful or not.
 *
 * @param mqtt_client The MQTT client instance
 * @param arg         Any arguments passed to this callback from the calling function
 *                    (currently passing no args)
 * @param status      The status of the MQTT connection. 
 *
 * Does not return anything, but updates the connected_to_broker global variables
 * from 0 to either 1 (success) or -1 (error). 
  */
void callback_mqtt_connect(mqtt_client_t *mqtt_client, void *arg, mqtt_connection_status_t status){
  if (status == MQTT_CONNECT_ACCEPTED){
  #if DEBUG_MQTT
  printf("Successfully connected to MQTT server!\n");
  #endif
  connected_to_broker = 1;
  } else 
    connected_to_broker = -1;
  
}




