
#include "serialclient.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

// install path: /usr/lib/avr/include/avr

#include <util/delay.h>
#include <avr/io.h>
#include <util/setbaud.h>
#include <stdio.h>

#include "lib_serial.h"

void uart_setup ( void ) {

  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;

#if USE_2X
  UCSR0A |= _BV(U2X0);
#else
  UCSR0A &= ~(_BV(U2X0));
#endif

  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */

  return;
}

void uart_stream_setup ( void ) {
  FILE uart_output = FDEV_SETUP_STREAM(uart_putchar_stream, NULL, _FDEV_SETUP_WRITE);
  FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar_stream, _FDEV_SETUP_READ);

  stdout = &uart_output;
  stdin  = &uart_input;

  return;
}

void uart_putchar_pre ( char c ) {
  loop_until_bit_is_set ( UCSR0A, UDRE0 ); /* Wait until data register empty. */
  UDR0 = c;
  return;
}

void uart_putchar_post ( char c ) {
  UDR0 = c;
  loop_until_bit_is_set ( UCSR0A, TXC0 ); /* Wait until transmission ready. */
  return;
}

char uart_getchar ( void ) {
  loop_until_bit_is_set ( UCSR0A, RXC0 ); /* Wait until data exists. */
  return UDR0;
}

void uart_putchar_stream ( char c, FILE *stream ) {
  if (c == '\n') {
    uart_putchar_stream('\r', stream);
  }
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
}

char uart_getchar_stream ( FILE *stream ) {
  loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
  return UDR0;
}
