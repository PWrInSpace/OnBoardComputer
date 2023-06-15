#include <Arduino.h>
#include <avr/io.h>

#define CLOSED 0x0A // Pozycja 0 deg
#define OPEN 0x2D // Pozycja 180 deg

void setTimers(){
  /*********************************************************
  * USTAWIENIE REJSETRÓW                                   *
  * TCCR0A - Fast PWM, maksymalna wartośc licznika OCR0A   *
  * TCCR0B - Prescaler ckl/64                              *
  *   MIEJSCE NA OBLICZENIA !!!!                           *
  **********************************************************/
  TCCR0A |= (1<<COM0A1) | (1<<WGM02) | (1<<WGM01) | (1<<WGM00);
  TCCR0B |= (1<<CS01) | (1<<CS00);
  OCR0A = CLOSED; 
}

void setup() {

  DDRB = 0x05;  // PORTY PB0 I PB2 USTAWIANE JAKO OUTPUT
  PORTB = 0x08; // PORT PB3 USTAWIONY JAKO INPUT_PULLUP, PB4 JAKO INPUT
  setTimers();
   
}

void loop() {

       /*******************************************************
       * (PINB & 0x10) -> GDY NA PB4 POJAWI SIE STAN WYSOKI   *
       *                  LUB                                 *
       * !(PINB & 0x08) -> STAN NASKI NA PB3                  *
       ********************************************************/ 
      if((PINB & 0x10) | !(PINB & 0x08)){
        
        PORTB |= (1<<PB2); // STAN PB2 USTAWIANY JAKO WYSOKI

        for(int i =0; i<9; i++){
          OCR0A = OPEN;
          delay(2000);
          OCR0A = CLOSED;
          delay(2000);
        }
        
        OCR0A = OPEN; // ZMIANA WARTOSCI LICZNIKA, ZMINIA WYPELNIENIE PWM
      
      }  
}