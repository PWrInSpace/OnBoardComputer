#include "Tasks.h"

/* Zadanie zajmujące się wszystkim, co łączy się przez i2c:
 *   1. Atmega odzyskowa, [ALMOST DONE]
 *   2. BME280.           [TODO]
 */

void i2cTask(void *arg) {

    Wire.begin();

    while(1) {

        for (int i = 0; i < 5; i++) {
        
            Wire.requestFrom(3, 1);
            vTaskDelay(10 / portTICK_PERIOD_MS);
            if (Wire.available()) {
                int val = Wire.read();
                Serial.println(val); // Tylko do debugu
            }
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }

        Wire.beginTransmission(3);
        Wire.write(8);
        Wire.endTransmission();
        vTaskDelay(5 / portTICK_PERIOD_MS);

        for (;;) {
            
            Wire.requestFrom(3, 1);
            vTaskDelay(10 / portTICK_PERIOD_MS);
            if (Wire.available()) {
                int val = Wire.read();
                Serial.println(val); // Tylko do debugu
            }
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
}

/**********************************************************************************************/

/* Zadanie odpowiedzialne za obsługę poleceń przychodzących po uartcie z płytki 3-antenowej. Obsługuje:
 *   1. Odczyt gpsa z płytki 3-antenowej,   [TODO]
 *   2. Stan Tanwy,                         [TODO]
 *   3. Polecenia przychodzące z LoRy,      [TODO]
 *   4. Ramki, które chcemy wysłać do LoRy. [TODO]
 */

void uart3AntsTask(void *arg) {

    while(1) {
        ;
    }
}

/**********************************************************************************************/

/* Zadanie odpowiedzialne za logowanie wszystkiego na SD -> pobierając dane z kolejki.
 */

void sdTask(void *arg) {

    while(1) {
        ;
    }
}

/**********************************************************************************************/

/* Zadanie odpowiedzialne za uruchomienie ESP-now i ustawienie przerwań dla wysłania i odbioru. Komunikacja z:
 *   1. Pitot,
 *   2. Główny zawór.
 */

void espNowTask(void *arg) {

    nowInit();

    nowAddPeer(adressPitot, 0);
    nowAddPeer(adressMValve, 0);

    while(1) {
        ;
    }
}