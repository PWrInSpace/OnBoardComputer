void doFirstSeparation() {
  
  PORTB |= SEPAR1A_Pin;
  PORTD |= SEPAR1B_Pin;

  SeparationFrame |= FRAME_1ST_SEPAR_DONE;
  _delay_ms(2000);

  PORTB &= ~SEPAR1A_Pin;
  PORTD &= ~SEPAR1B_Pin;
}

/*******************************************************/

void doSecondSeparation() {

  PORTD |= SEPAR2A_Pin;
  PORTD |= SEPAR2B_Pin;

  SeparationFrame |= FRAME_2ND_SEPAR_DONE;
  _delay_ms(2000);
  
  PORTD &= ~SEPAR2A_Pin;
  PORTD &= ~SEPAR2B_Pin;
}
