void initSeparationSystem() {

  // Inity pinów portu D:

  // Test 2 stopnia separacji - INPUT_PULLUP:
  PORTD |= TEST2_Pin;
  
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
}

/*******************************************************/

void sendData() {

  Wire.write(SeparationFrame);
}

/*******************************************************/

void receiveData() {

  if(Wire.available()) {
    char rxByte = Wire.read();

    if(rxByte == 8) {
      
      state = ARMED;
      SeparationFrame |= (1<<0);
    }
    
    else if(rxByte == 24) doFirstSeparation();
    else if(rxByte == 56) doSecondSeparation();
  }
}
