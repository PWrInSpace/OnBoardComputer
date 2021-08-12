#include "LoopTasks.h"

extern Queue queue;

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
    SPIClass SPISD(HSPI); 
    SPISD.begin(GPIO_NUM_10, GPIO_NUM_11, GPIO_NUM_15);
    
    if(!SD.begin(SD_CS, SPISD)){

        // Tu się wstawi dodawanie liczby błędów SD o 1, albo o 1000
        Serial.println("Brak karty");
        //while(1);
    }
    SPI.setClockDivider(SPI_CLOCK_DIV2);

    /*SD_write("/R4_data.txt", "cos;cos;cos;cos;\n");
    SD_write("/R4_tanwa.txt", "cos;cos;cos;cos;\n");
    SD_write("/R4_gps.txt", "cos;cos;cos;cos;\n");*/

    while(1) {
       
        while(queue.getNumberOfElements()){
            String dataFrame = queue.pop() + "\n";
            
            switch (dataFrame[2]){
                case 'M':
                    SD_write("/R4_data.txt", dataFrame);
                    break;
                case 'T':
                    SD_write("/R4_tanwa.txt", dataFrame);
                    break;
                case 'G':
                    SD_write("/R4_gps.txt", dataFrame);
                    break;
                default:
                    Serial.println("Nie ma takiego znacznika!"); // Tu się wstawi dodawanie liczby błędów SD o 1.
            }
        }
        vTaskDelay(2 / portTICK_PERIOD_MS);
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

    // Pullup dla krańcówek:
    pinMode(GPIO_NUM_27, INPUT_PULLUP);
    pinMode(GPIO_NUM_14, INPUT_PULLUP);

    while(1) {

        // Ciśnienie butli:
        mainDataFrame.tankPressure = analogRead(GPIO_NUM_34) / 1.0; // Dorobić dzielnik lub mnożnik lub funkcję map!

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