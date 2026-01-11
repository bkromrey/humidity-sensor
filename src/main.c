#include "pico/stdlib.h"

int main(){

  const uint BOARD_LED = PICO_DEFAULT_LED_PIN;
  gpio_init(BOARD_LED);
  gpio_set_dir(BOARD_LED, GPIO_OUT);

  while(true){
    gpio_put(BOARD_LED, 1);
    sleep_ms(500);
    gpio_put(BOARD_LED, 0);
    sleep_ms(2000);
  }

  return 0;
}
