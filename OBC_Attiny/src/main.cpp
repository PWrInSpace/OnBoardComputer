#include <Arduino.h>
#include <avr/io.h>

void setup() {

  DDRB |= (1 << PB2) | (1 << PB0);  // Pins PB2 and PB0 are outputs,
  PORTB = 0x08; // Pin PB3 is pulled up input, PB4 is floating input.
}

void loop() {

  /**************************************
  * (PINB & 0x10) -> if PB4 goes high   *
  *                  or                 *
  * !(PINB & 0x08) -> if PB3 goes low   *
  **************************************/

  if((PINB & 0x10) | !(PINB & 0x08)){

    PORTB |= (1 << PB2) | (1 << PB0); // Set PB0 and PB2 to high state.
    delay(19000);
    PORTB &= ~(1 << PB0);

    while(1) {
      delay(1);
    }
  }
}
