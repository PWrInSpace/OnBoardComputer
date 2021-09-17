/* Poniżej znajduje się ramka danych wysyłana po i2c. Każdy bit oznacza odpowiednią rzecz:
 *  
 * Kolejność:
 * MSB -> 76543210 <- LSB:
 * 
 * 0. Czy Atmega jest uzbrojona softowo? 0 - nie, 1 - tak,
 * 1. Czy zrobiono separację 1 stopnia?
 * 2. Czy zrobiono separację podstawową 2 stopnia?
 * 3. Czy jest ciągłość zapalnika 1?
 * 4. Czy jest ciągłość zapalnika 2?
 * 5. Czy Altimax wykrył apogeum?
 * 6. Czy Altimax wykrył 200m na ziemią?
 * 7. Czy telemetrum wykrył apogeum?
 * 8. Czy telemetrum wykrył 200m nad ziemią?
 * 9. Czy wyleciał stożek?
 */

#define FRAME_ARMED          (1<<0)
#define FRAME_1ST_SEPAR_DONE (1<<1)
#define FRAME_2ND_SEPAR_DONE (1<<2)
#define FRAME_1ST_CONTINUITY (1<<3)
#define FRAME_2ND_CONTINUITY (1<<4)
#define FRAME_ALTI_APOG      (1<<5)
#define FRAME_ALTI_MAIN      (1<<6)
#define FRAME_TELE_APOG      (1<<7)
#define FRAME_TELE_MAIN      (1<<8)
#define FRAME_HEAD_DETACHED  (1<<9)

uint16_t SeparationFrame = 0;

/*******************************************************/

enum States {

  IDLE_STATE = 0,
  ARMED
};

States state = IDLE_STATE;

/*******************************************************/

// Definicje pinów portu D:

// Test wypadnięcia stożka:
#define CONT_HEAD_Pin   (1<<3)

// Zapalnik 2A:
#define SEPAR2A_Pin (1<<5)

// Zapalnik 2B:
#define SEPAR2B_Pin (1<<6)

// Zapalnik 1B:
#define SEPAR1B_Pin (1<<7)

/*----------------------------*/

// Definicje pinów portu B:

// Zapalnik 1A:
#define SEPAR1A_Pin (1<<0)

/*----------------------------*/

// Definicje pinów portu C:

// Altimax apogeum:
#define ALTI_TEST1_Pin (1<<0)

// Altimax main:
#define ALTI_TEST2_Pin (1<<1)

// Tele apogeum:
#define TELE_TEST1_Pin (1<<2)

// Tele main:
#define TELE_TEST2_Pin (1<<3)
