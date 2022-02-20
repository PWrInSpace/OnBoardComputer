#include "LoopTasks.h"

extern MainDataFrame mainDataFrame;
extern Queue queue;
extern Timer_ms frameTimer;
extern MaximumData maxData;

/* Zadanie odpowiedzialne za logowanie wszystkiego na SD -> pobierając dane z kolejki.
 */
// TODO Flash zamiast SD
void sdTask(void *arg) {

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    SPIClass SPISD(HSPI);
    SPISD.begin(GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_15);
    SPI.setClockDivider(SPI_CLOCK_DIV2);

    if(!SD.begin(SD_CS, SPISD))
        mainDataFrame.sdErrorCounter = 2137; // Fatalny błąd.

    while(1) {

        while(queue.getNumberOfElements()){

            String dataFrame = queue.pop();

            Serial.print(dataFrame); // Dla debugu

            switch (dataFrame[2]) {
            case 'M':
                SD_write("/R4_data.txt", dataFrame);
                break;

            case 'T':
                SD_write("/R4_tanwa.txt", dataFrame);
                break;

            case 'G':
                SD_write("/R4_gps.txt", dataFrame);
                break;
            }

            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }
}

/**********************************************************************************************/

/* Funkcja odpowiedzialna za pomiary analogowe.
 */

void adcMeasure() {

    // Ciśnienie butli:
    mainDataFrame.tankPressure = analogRead(GPIO_NUM_34) * 0.0201465 * 1.11; // * 82.5 / 4095
    /* 
        * Mnożymy przez 82.5, bo przy takim ciśnieniu będzie maksymalne napięcie na adc - 3.3v
        * (Czujnik daje 10v przy 250 barach).
        * dzielimy przez 4095 bo to max wartość z przetwornika
        */

    // Akumulator:
    mainDataFrame.battery = analogRead(GPIO_NUM_36) / 254.0 * 0.893;  
}