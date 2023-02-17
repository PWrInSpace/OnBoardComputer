#include <Arduino.h>
#include <avr/io.h>

#define CLOSED 0x0A // Pozycja 0 deg
#define OPEN 0x2D // Pozycja 180 deg

void setup() {

  DDRB = 0x01;
  PORTB = 0x10;
  TCCR0A |= (1<<COM0A1) | (1<<WGM02) | (1<<WGM01) | (1<<WGM00);
  TCCR0B |= (1<<CS01) | (1<<CS00);
  OCR0A = CLOSED;   
   
}

void loop() {

      if(!(PINB & 0x10)){
        OCR0A = OPEN; 
      }  
}