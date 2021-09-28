void separationMonitoringLoop() {

  if(state == IDLE_STATE) return;
  
  // Monitoring pinów portu C:
  
  if(!(PINC & ALTI_TEST1_Pin)) { // Altimax apogeum

    SeparationFrame |= FRAME_ALTI_APOG;

    if(state == ARMED) doFirstSeparation();
  }

  if(!(PINC & ALTI_TEST2_Pin)) { // Altimax main

    SeparationFrame |= FRAME_ALTI_MAIN;

    if(state == ARMED) doSecondSeparation();
  }

  if(!(PINC & TELE_TEST1_Pin)) { // Tele apogeum

    SeparationFrame |= FRAME_TELE_APOG;

    if(state == ARMED) doFirstSeparation();
  }

  if(!(PINC & TELE_TEST2_Pin)) { // Tele main

    SeparationFrame |= FRAME_TELE_MAIN;

    if(state == ARMED) doSecondSeparation();
  }
}

/*******************************************************/

void continuityMonitoringLoop() {

  // Ciągłość 1:
  if(PINB & CONT1_Pin) SeparationFrame |= FRAME_1ST_CONTINUITY;
  else SeparationFrame &= ~FRAME_1ST_CONTINUITY;

  // Ciągłość 2:
  if(PINB & CONT2_Pin) SeparationFrame |= FRAME_2ND_CONTINUITY;
  else SeparationFrame &= ~FRAME_2ND_CONTINUITY;

  // Wylecenie stożka:
  if(PIND & CONT_HEAD_Pin) SeparationFrame |= FRAME_HEAD_DETACHED;
  else SeparationFrame &= ~FRAME_HEAD_DETACHED;
}
