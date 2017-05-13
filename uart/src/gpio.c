#ifndef __GPIO_C__
#define __GPIO_C__

#include <gpio.h>


/* Global instance */
peripheral_ctx_t gpio;

void GPIO_PUT32(unsigned int addr, unsigned int value) {
  *(gpio.base_addr + addr) = value;

}/*GPIO_PUT32*/

unsigned int GPIO_GET32(unsigned int addr) {
  return(*(gpio.base_addr + addr));

}/*GPIO_GET32*/

#endif /*__GPIO_C__*/
