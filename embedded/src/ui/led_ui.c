#include "led_ui.h"

uint32_t Scale_Humidity_Data(float humidity_raw){
  return (uint32_t) (humidity_raw / (HUMIDITY_MAX / (LED_LENGTH + 1)));
}

void Display_Humidity_LED(float humidity_raw){
  uint32_t idx = Scale_Humidity_Data(humidity_raw);
  Display_LED_Array(idx);
}