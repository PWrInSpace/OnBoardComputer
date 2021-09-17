void initSeparationSystem() {

  // Inity pinów portu D:

  // Test, czy wyleciał stożek - INPUT_PULLUP:
  PORTD |= CONT_HEAD_Pin;
  
  // Zapalniki 2A, 2B, 1B - OUTPUT:
  DDRD |= (SEPAR2A_Pin | SEPAR2B_Pin | SEPAR1B_Pin);
  
  // Inity pinów portu B:
  
  // Zapalnik 1A - OUTPUT:
  DDRB |= SEPAR1A_Pin;
  
  // Definicje pinów portu C:
  
  // Altimax apogeum, Altimax main, Tele apogeum, Tele main - INPUT_PULLUP:
  PORTC |= (ALTI_TEST1_Pin | ALTI_TEST2_Pin | TELE_TEST1_Pin | TELE_TEST2_Pin);

  // i2c:
  Wire.begin(3);
  Wire.onRequest(sendData);
  Wire.onReceive(receiveData);
  Wire.setTimeout(100);
}

/*******************************************************/

void sendData() {

  Wire.write((uint8_t*) &SeparationFrame, sizeof(SeparationFrame));
}

/*******************************************************/

void receiveData() {

  char rxByte = Wire.read();
  
  if(rxByte == 8) {
    
    state = ARMED;
    SeparationFrame |= FRAME_ARMED;
  }
  
  else if(rxByte == 24) doFirstSeparation();
  else if(rxByte == 56) doSecondSeparation();
}
