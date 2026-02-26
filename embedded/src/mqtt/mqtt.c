#include "mqtt.h"

#define DEBUG_MQTT 1

#define WIFI_TIMEOUT 30000       // how many ms to wait before failing to connect (30s)
#define PICO_MQTT_PORT 1883       // 1883 is the standard MQTT port
#define MQTT_KEEP_ALIVE 60        // keepalive in seconds
#define MQTT_QOS 0
#define MQTT_RETAIN 0
#define MAX_PAYLOAD_SIZE 100      // number of chars for JSON string

#ifndef PICO_MQTT_SERVER
#error Required to define PICO_MQTT_SERVER
#endif

#ifndef PICO_MQTT_USER 
#error Required to define PICO_MQTT_USER 
#endif

#ifndef PICO_MQTT_PASS 
#error Required to define PICO_MQTT_PASS 
#endif

#ifndef PICO_WIFI_SSID 
#error Required to define PICO_WIFI_SSID 
#endif

#ifndef PICO_WIFI_PASS 
#error Required to define PICO_WIFI_PASS
#endif

#ifndef PICO_SENSOR_ID 
#error Required to define PICO_SENSOR_ID 
#endif

// ---------------------------------------------------------------------------
// GLOBAL VARS 
// ---------------------------------------------------------------------------

static mqtt_client_t *mqtt_client;
static struct mqtt_connect_client_info_t client_info;

enum wireless_connectivity{
  WIFI_NOT_INIT,
  WIFI_CONNECTED,
  WIFI_DISCONNECTED,
  WIFI_DISABLED
} wireless_connectivity = WIFI_NOT_INIT;

enum connected_to_broker{
  NOT_YET_INITIALIZED,
  CONNECTED,
  FAILED_TO_CONNECT
} connected_to_broker = NOT_YET_INITIALIZED;


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



  // initialize & connect to wifi
  if (init_wifi()){
    #if DEBUG_MQTT
    printf("Error initializing & connecting to wifi network; wireless communications disabled.\n");
    #endif
    wireless_connectivity = WIFI_DISABLED;
    return 1;
  } else
    wireless_connectivity = WIFI_CONNECTED;

  // initialize connection to the MQTT broker
  init_mqtt();

  // blocking - wait for the callback function to update status before continuing
  // the connection has a timeout of 60 seconds (or as MQTT_CONNECT_TIMOUT is
  // defined in lwipopts.h)
  while (connected_to_broker == NOT_YET_INITIALIZED)
    sleep_ms(10);
  

  if (connected_to_broker == FAILED_TO_CONNECT){
    #if DEBUG_MQTT
    printf("Failed to connect to MQTT broker at %s\n", PICO_MQTT_SERVER);
    #endif
    return 1;
  }

  return 0;
}

/** This function converts to JSON and transmits a payload of sensor data 
 * (temperature, humidity, and light levels) via MQTT to the web application. 
 * The MQTT 'topic' is the sensor ID defined at build time, and the MQTT 
 * message is the JSON payload.
 *
 * If the Pico is no longer connected to the MQTT broker, will attempt to
 * reconnect, so that the next time this function is called, it might succeed.
 *
 * @env   PICO_SENSOR_ID  Sends the sensor ID as the MQTT topic.
 * @param Sensor_Data     Current sensor data to be transmitted.
 *
 * Returns 0 if successful or 1 if an error is encountered.
 */
int Publish_Data(const Payload_Data *Sensor_Data){
  
  if (wireless_connectivity == WIFI_DISABLED)
    return 1;

  char data_as_json[MAX_PAYLOAD_SIZE];
  if (generate_payload(Sensor_Data, data_as_json)){
    #if DEBUG_MQTT
    printf("MQTT payload not generated successfully\n");
    #endif
    return 1; 
  }
  
  #if DEBUG_MQTT
  printf("%s\n\n", data_as_json);
  #endif

  // validate wifi connection
  if (cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) != 1){
    #if DEBUG_MQTT
    printf("ERROR SENDING DATA: Not connected to wifi network!\n");
    #endif

    wireless_connectivity = WIFI_DISCONNECTED;

    return 1;
  }

  //ensure still connected to mqtt broker before attempting to send
  if (mqtt_client_is_connected(mqtt_client) == 0){
    #if DEBUG_MQTT
    printf("ERROR SENDING DATA: MQTT client no longer connected\n");
    #endif
    
    init_mqtt();
  
    return 1;
  }

  #if DEBUG_MQTT
  printf("publishing data using topic %s\n", PICO_SENSOR_ID);
  #endif
  
  // publish the data to mqtt broker
  if (mqtt_publish(mqtt_client, PICO_SENSOR_ID, data_as_json, strlen(data_as_json), MQTT_QOS, MQTT_RETAIN, callback_mqtt_publish, 0) != ERR_OK){
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
 * Converts the sensor data into a JSON payload to be transmitted via MQTT to
 * the web application.
 *
 * @parm    Sensor_Data   The data to be converted into a JSON string
 * @param   json_payload  The JSON string is stored here for callign function.
 *
 * Returns 0 if successful or 1 if an error occurs.
 */
int generate_payload(const Payload_Data *Sensor_Data, char json_payload[]){

  if (Sensor_Data->DHT20_Data_Valid){
    sprintf(json_payload, "{\"temperature_f\": \"%2.1f\","
                   "\"temperature_c\": \"%2.1f\","
                   "\"humidity\": \"%2.1f\","
                   "\"light\": \"%d\"}",Sensor_Data->DHT20_Data.temperature_f,
          Sensor_Data->DHT20_Data.temperature_c, Sensor_Data->DHT20_Data.humidity, 
          Sensor_Data->ADC_Data);

  } else {
    #ifdef DEBUG_MQTT
    printf("Sensor data is not valid, not sending to MQTT broker");
    #endif
    return 1;
  }

  return 0;
}

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


/*
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
  
  // if this is the first time this connection is attempted, init required vars
  if (connected_to_broker == NOT_YET_INITIALIZED){

    mqtt_client = mqtt_client_new();
  
    // set client connection parameters
    client_info.client_id = PICO_SENSOR_ID;
    client_info.client_user = PICO_MQTT_USER;
    client_info.client_pass = PICO_MQTT_PASS;
    client_info.keep_alive = MQTT_KEEP_ALIVE;
  }

  #if DEBUG_MQTT
  printf("MQTT Server is: %s\n", PICO_MQTT_SERVER);
  #endif

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
  connected_to_broker = CONNECTED;
  } else 
    connected_to_broker = FAILED_TO_CONNECT;
  
}



