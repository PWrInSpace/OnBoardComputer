#include "LoopTasks.h"

extern MainDataFrame mainDataFrame;

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

        Serial.println("dupa");
        Wire.beginTransmission(3);
        Wire.write(8);
        Wire.endTransmission();
        vTaskDelay(10 / portTICK_PERIOD_MS);

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
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

/**********************************************************************************************/

/* Zadanie odpowiedzialne za uruchomienie ESP-now i ustawienie przerwań dla wysłania i odbioru. Komunikacja z:
 *   1. Pitot,          [TODO - Done earlier]
 *   2. Główny zawór.   [TODO]
 */

void espNowTask(void *arg) {

    if(!nowInit()) Serial.println("nie działa now");

    /*nowAddPeer(adressPitot, 0);
    nowAddPeer(adressMValve, 0);*/

    /*char message[] = "wazna wiadomosc do przeslania\n";
    if(esp_now_send(adressPitot, (uint8_t *) message, strlen(message)))
        mainDataFrame.espNowErrorCounter++;*/

    while(1) {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

/**********************************************************************************************/

/* Zadanie odpowiedzialne za pomiary analogowe:
 *   1. Czujnik ciśnienia butli,                    [ALMOST_DONE]
 *   2. 3 czuniki halla (2 przejdą na moduł GPS),   [TODO]
 *   3. Napięcie zasilania,                         [ALMOST_DONE]
 *   4. Krańcówki (I/O),                            [ALMOST_DONE]
 *   5. Potencjometr zaworu upustowego.             [ALMOST_DONE]
 */

void adcTask(void *arg) {

    const int gpioHalSensor[] = {GPIO_NUM_33, GPIO_NUM_32, GPIO_NUM_35};

    // Pullup dla krańcówek:
    pinMode(GPIO_NUM_27, INPUT_PULLUP);
    pinMode(GPIO_NUM_14, INPUT_PULLUP);

    while(1) {

        // Ciśnienie butli:
        mainDataFrame.tankPressure = analogRead(GPIO_NUM_34) / 1.0; // Dorobić dzielnik lub mnożnik lub funkcję map!

        // Czujniki halla:
        for (uint8_t i = 0; i < 3; i++) {
            
            mainDataFrame.halSensor[i] = analogRead(gpioHalSensor[i]);
        }

        // Akumulator:
        mainDataFrame.battery = analogRead(GPIO_NUM_36) / 254.0;
        
        // Krańcówki:
        mainDataFrame.upust.endStop1 = !digitalRead(GPIO_NUM_27);
        mainDataFrame.upust.endStop1 = !digitalRead(GPIO_NUM_14);

        // Potencjometr zaworu upustowego:
        mainDataFrame.upust.potentiometer = analogRead(GPIO_NUM_39);        

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}