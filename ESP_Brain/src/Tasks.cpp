#include "Tasks.h"

/* Zadanie zajmujące się wszystkim, co łączy się przez i2c:
 *   1. Atmega odzyskowa, [ALMOST DONE]
 *   2. BME280.           [TODO]
 */

void i2cTask(void *arg) { // Trochę jest bałagan w tej funkcji. Będzie tego mniej docelowo.

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

/* Zadanie odpowiedzialne za logowanie wszystkiego na SD -> pobierając dane z kolejki. [TODO]
 */

void sdTask(void *arg) {

    while(1) {
        ;
    }
}

/**********************************************************************************************/

/* Zadanie odpowiedzialne za uruchomienie ESP-now i ustawienie przerwań dla wysłania i odbioru. Komunikacja z:
 *   1. Pitot,          [TODO - Done earlier]
 *   2. Główny zawór.   [TODO]
 */

void espNowTask(void *arg) {

    nowInit();

    /*nowAddPeer(adressPitot, 0);
    nowAddPeer(adressMValve, 0);*/

    /*char message[] = "wazna wiadomosc do przeslania\n";
    if(esp_now_send(adressPitot, (uint8_t *) message, strlen(message)))
        mainDataFrame.espNowErrorCounter++;*/

    while(1) {
        ;
    }
}

/**********************************************************************************************/

/* Zadanie odpowiedzialne za pomiary analogowe:
 *   1. Czujnik ciśnienia butli,
 *   2. 5 czuników halla,
 *   3. Napięcie zasilania,
 *   4. Krańcówki (I/O),
 *   5. Potencjometr zaworu upustowego.
 */

void adcTask(void *arg) {

    while(1) {
        ;
    }
}