#include <Arduino.h>
#include <avr/io.h>

void setup() {

  DDRB = 0x05;  // PORTY PB0 I PB2 USTAWIANE JAKO OUTPUT
  PORTB = 0x08; // PORT PB3 USTAWIONY JAKO INPUT_PULLUP, PB4 JAKO INPUT
}

void loop() {

  /*******************************************************
  * (PINB & 0x10) -> GDY NA PB4 POJAWI SIE STAN WYSOKI   *
  *                  LUB                                 *
  * !(PINB & 0x08) -> STAN NASKI NA PB3                  *
  *******************************************************/

  if((PINB & 0x10) | !(PINB & 0x08)){

    PORTB |= (1 << PB2) | (1 << PB0); // STAN PB2 ORAZ PB0 USTAWIANY JAKO WYSOKI
    delay(9000);
    PORTB &= ~(1 << PB0);
  }
}
