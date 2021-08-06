/* Poniżej znajduje się ramka danych wysyłana po i2c. Każdy bit oznacza odpowiednią rzecz:
 *  
 * Kolejność:
 * MSB -> 76543210 <- LSB:
 * 
 * 0.
 * 1.
 * 2.
 * 3.
 * 4.
 * 5.
 * 6.
 * 7.
 */

uint8_t SeparationFrame = 0;

/*******************************************************/

enum States {

  READY = 0,
  ARMED,
  FIRST_SEP,
  SECOND_SEP
};
