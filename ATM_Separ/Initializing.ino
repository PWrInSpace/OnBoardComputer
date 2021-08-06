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
}
