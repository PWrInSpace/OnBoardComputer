void doFirstSeparation() {
  
  PORTB |= SEPAR1A_Pin;
  PORTD |= SEPAR1B_Pin;

  _delay_ms(2000);

  PORTB &= ~SEPAR1A_Pin;
  PORTD &= ~SEPAR1B_Pin;

  SeparationFrame |= (1<<1);
}

/*******************************************************/

void doSecondSeparation() {

  SeparationFrame |= (1<<2);

  // Zapalniki podstawowe:
  PORTD |= SEPAR2A_Pin;

  _delay_ms(2000);

  if(PIND & TEST2_Pin) {

    PORTD &= ~SEPAR2A_Pin;
    return; // Wyjście z funkcji, jeśli separacja uda się przy zapalnikach podstawowych
  }

  // Zapalniki awaryjne:
  PORTD |= SEPAR2B_Pin;
  
  _delay_ms(2000);
  
  PORTD &= ~SEPAR2A_Pin;
  PORTD &= ~SEPAR2B_Pin;

  SeparationFrame |= (1<<3);
}
