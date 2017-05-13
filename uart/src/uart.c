#ifndef __UART_PLL011_C__
#define __UART_PLL011_C__

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "gpio.h"
#include "uart.h"


/* Global instance */
static peripheral_ctx_t uart0;

/* extern declaration */
extern peripheral_ctx_t gpio;

int memmap (peripheral_ctx_t *pi3b_uart, unsigned int base_addr) {

  if ((pi3b_uart->mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
    fprintf(stderr, "Failed to open mem device\n");
    return -1;
  }
  
  /* Mapping GPIO Physical memeory into virtual memory */
  pi3b_uart->map = mmap(NULL, /*Let the Kernel decisides for us*/
                        BLOCK_SIZE,
                        (PROT_READ | PROT_WRITE), /*The Mapped region is protected*/
                        MAP_SHARED, /*Shared accross Process*/
                        pi3b_uart->mem_fd, /*File Descriptor of mapped region*/
                        base_addr);

  if (pi3b_uart->map == MAP_FAILED) {
    perror("mmap");
    return -1;		
  }	
  
  /* Virtual Address mapped by Kernel */
  pi3b_uart->base_addr = (volatile unsigned int *)pi3b_uart->map;
  return 0;
}/*memmap*/

void UART0_PUT32(unsigned int reg, unsigned int value) {
  *(uart0.base_addr + reg) = value;

}/*UART0_PUT32*/

unsigned int UART0_GET32(unsigned int reg) {
  return(*(uart0.base_addr + reg));

}/*UART0_GET32*/

void disable_uart(void) {
  unsigned int data = 0x00;
  data = UART0_GET32(CR);
  data &= ~(CR_UARTEN);
  UART0_PUT32(CR, data);

}/*disable_uart*/

void enable_uart(void) {
  unsigned int data = 0x00;
  data = UART0_GET32(CR);
  /*Clear the bit0 first*/
  data &= ~(CR_UARTEN);
  /*set the bit0 now*/
  data |= CR_UARTEN;
  UART0_PUT32(CR, data);

}/*disable_uart*/

void enable_tx(void) {
  unsigned int data = 0x00;
  data = UART0_GET32(CR);
  data &= ~(CR_TXE);
  data |= CR_TXE;
  UART0_PUT32(CR, data);

}/*enable_tx*/

void enable_rx(void) {
  unsigned int data = 0x00;
  data = UART0_GET32(CR);
  data &= ~(CR_RXE);
  data |= CR_RXE;
  UART0_PUT32(CR, data);

}/*enable_rx*/

void disable_tx(void) {
  unsigned int data = 0x00;
  data = UART0_GET32(CR);
  data &= ~(CR_TXE);
  UART0_PUT32(CR, data);

}/*disable_tx*/

void disable_rx(void) {
  unsigned int data = 0x00;
  data = UART0_GET32(CR);
  data &= ~(CR_RXE);
  UART0_PUT32(CR, data);

}/*disable_rx*/

void flush_fifo(void) {
  unsigned int data = 0x00;
  data = UART0_GET32(LCRH);
  data &= ~(LCRH_FEN);
  UART0_PUT32(LCRH, data);
  
}/*flush_fifo*/

void enable_fifo(void) {
  unsigned int data = 0x00;
  data = UART0_GET32(LCRH);
  data &= ~(LCRH_FEN);
  data |= LCRH_FEN;
  UART0_PUT32(LCRH, data);

}/*enable_fifo*/
//------------------------------------------------------------------------
void uart_putc ( unsigned char c )
{
   unsigned int data;

    while(1)
    {
      /*Wait Until TX-FIFO has a room for new byte*/
      if((UART0_GET32(FR) & FR_TXFF) == 0) break;
    }

    data = UART0_GET32(DR);
    data &= ~(DR_DATA);
    data |= c;
    UART0_PUT32(DR, data);
}

void uart_init() {
  unsigned int data = 0;

  /*Disable the UART*/
  UART0_PUT32(CR, 0);
  
  data = GPIO_GET32(GPFSEL1);
  data &= ~(7<<12); //gpio14
  data |= 4<<12;    //alt0
  data &= ~(7<<15); //gpio15
  data |= 4<<15;    //alt0
  
  GPIO_PUT32(GPFSEL1, data);
  GPIO_PUT32(GPPUD, 0);
  
  for(data = 0; data < 150; data++) {
    ;
  }
  GPIO_PUT32(GPPUDCLK0, ((1 << 14) | (1 << 15))); 
  
  for(data = 0; data < 150; data++) {
    ;
  }
  
  GPIO_PUT32(GPPUDCLK0, 0); 

  UART0_PUT32(ICR,0x7FF);
  /*1)//(3000000 / (16 * 115200) = 1.627 ==> IBRD=1
      //(0.627*64)+0.5 = 40 ==> FBRD=40
      //int 1 frac 40
    2)//(3000000 / (16 * 9600)) = 19.2 ==> IBRD=19
      //(0.2 * 64 + 0.5) = 13 ==>FBRD=13
  */
  UART0_PUT32(IBRD, 19); 
  UART0_PUT32(FBRD, 13);
  /*8Bits word no Parity and no stop bits*/
  UART0_PUT32(LCRH, (LCRH_WLEN8 |LCRH_FEN));

  UART0_PUT32(CR, (CR_TXE |CR_RXE |CR_UARTEN)); 
}/*uart_init*/

void *poll_receiver(void *thread_arg) {
  (void) thread_arg;
  unsigned int cc;

  while(1) {
    /*Wait until RX-FIFO is not empty*/
    if(!(UART0_GET32(FR) & FR_RXFE)) {
      cc = UART0_GET32(DR);
      fprintf(stderr, "0x%X .... %c \n", cc, cc);
    } 
  }

}/*poll_receiver*/


int main() {
  unsigned int ra = 0;
  int rc = -1;
  pthread_t    thread_id;
  
  memmap(&uart0, UART_BASE);
  memmap(&gpio,  GPIO_IO);
  uart_init();
  
  rc = pthread_create(&thread_id, (void *)0, poll_receiver, (void *)0);
  /* 1) Disable UART
     2) Flush TX FIFO
     3) Programm the CR - Control Register
     4) Enable UART
   */ 
  disable_uart();
  flush_fifo();
  enable_fifo();
  enable_uart();
  usleep(100);
  
  for(ra=0;ra<4;ra++)
  {
    uart_putc(0x59);
    uart_putc(0x47);
    uart_putc(0x48);
  }

  while(1)
  {
#if 0
    ra=uart_getc();
    if(ra==0x0D) uart_putc(0x0A);
      uart_putc(ra);
#endif
  } 
}
#endif  /*__UART_PLL011_C__*/
