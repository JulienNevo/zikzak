
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

#include "gpio.h"

void gpio_toggle_blinkenlight ( void ) {
  gpio_toggle (GPIOB, GPIO13 ); /* LED on/off. */
}

void gpio_set_blinkenlight ( unsigned char onoff ) {
  if ( onoff ) {
    gpio_set ( GPIOB, GPIO13 ); // LED
    return;
  }
  gpio_clear ( GPIOB, GPIO13 ); // LED
}
