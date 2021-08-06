/* Poniżej znajduje się ramka danych wysyłana po i2c. Każdy bit oznacza odpowiednią rzecz:
 *  
 * Kolejność:
 * MSB -> 76543210 <- LSB:
 * 
 * 0. Czy Atmega jest uzbrojona softowo? 0 - nie, 1 - tak,
 * 1. Czy zrobiono separację 1 stopnia?
 * 2. Czy zrobiono separację 2 stopnia?
 * 3. Czy Altimax wykrył apogeum?
 * 4. Czy Altimax wykrył 200m na ziemią?
 * 5. Czy telemetrum wykrył apogeum?
 * 6. Czy telemetrum wykrył 200m nad ziemią?
 * 7. Kontrola parzystości - jeśli w poprzednich bitach jest parzysta liczba jedynek, to tutaj ma być 1.
 */

uint8_t SeparationFrame = 0;

/*******************************************************/

enum States {

  READY = 0,
  ARMED,
  FIRST_SEP,
  SECOND_SEP
};
