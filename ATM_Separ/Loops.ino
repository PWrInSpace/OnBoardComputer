void i2cLoop() {

  if(Wire.available()) {
    uint8_t rxByte = Wire.read();

    if(rxByte == 8) {
      
      state = ARMED;
      SeparationFrame |= (1<<0);
    }
    
    else if(rxByte == 24) doFirstSeparation();
    else if(rxByte == 56) doSecondSeparation();
  }
}

/*******************************************************/

void separationMonitoringLoop() {

  if(state == IDLE_STATE) return;
  
  // Monitoring pinów portu C:
  
  if(PINC & ALTI_TEST1_Pin) { // Altimax apogeum

    SeparationFrame |= (1<<4);

    if(state == ARMED) doFirstSeparation();
  }

  if(PINC & ALTI_TEST2_Pin) { // Altimax main

    SeparationFrame |= (1<<5);

    if(state == FIRST_SEP) doSecondSeparation();
  }

  if(PINC & TELE_TEST1_Pin) { // Tele apogeum

    SeparationFrame |= (1<<6);

    if(state == ARMED) doFirstSeparation();
  }

  if(PINC & TELE_TEST2_Pin) { // Tele main

    SeparationFrame |= (1<<7);

    if(state == FIRST_SEP) doSecondSeparation();
  }
}
