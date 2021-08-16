#include "LoopTasks.h"

extern MainDataFrame mainDataFrame;
extern Queue queue;
extern bool forceStateAction;
extern Timer_ms frameTimer;

/* Zadanie zajmujące się wszystkim, co łączy się przez i2c:
 *   1. Atmega odzyskowa, [ALMOST_DONE]
 *   2. BME280.           [TODO]
 */

void i2cTask(void *arg) { // Trochę jest bałagan w tej funkcji. Będzie tego mniej docelowo.

    // TODO pomiar początkowego ciśnienia.

    Wire.begin();

    while (mainDataFrame.rocketState < FUELING) {

        if (frameTimer.flag) {

            frameTimer.flag = false;
        
            Wire.requestFrom(3, 1);
            vTaskDelay(10 / portTICK_PERIOD_MS);
            if (Wire.available()) {
                mainDataFrame.separationData = Wire.read();
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

    }

    Wire.beginTransmission(3);
    Wire.write(8);
    Wire.endTransmission();
    vTaskDelay(10 / portTICK_PERIOD_MS);

    while (1) {
        
        if (frameTimer.flag) {

            frameTimer.flag = false;

            Wire.requestFrom(3, 1);
            vTaskDelay(10 / portTICK_PERIOD_MS);
            if (Wire.available()) {
                mainDataFrame.separationData = Wire.read();
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);

        }

        while (mainDataFrame.rocketState == FLIGHT) {

            // Tutaj lecą pomiary z BME i obliczenia wszystkie (wysokość, prędkość, przyspieszenie)
            vTaskDelay(2 / portTICK_PERIOD_MS);
        }

        if (forceStateAction && mainDataFrame.rocketState == FIRST_SEPAR) {

            forceStateAction = false;

            // Rozkaz separacji 1 st:
            Wire.beginTransmission(3);
            Wire.write(24);
            Wire.endTransmission();
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

        else if (forceStateAction && mainDataFrame.rocketState == SECOND_SEPAR) {

            forceStateAction = false;

            // Rozkaz separacji 2 st.
            // Rozkaz separacji 1 st:
            Wire.beginTransmission(3);
            Wire.write(56);
            Wire.endTransmission();
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

    vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/**********************************************************************************************/

/* Zadanie odpowiedzialne za logowanie wszystkiego na SD -> pobierając dane z kolejki. [TODO]
 */

void sdTask(void *arg) {

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    SPIClass SPISD(HSPI);
    SPISD.begin(GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_15);
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    
    if(!SD.begin(SD_CS, SPISD)){

        mainDataFrame.sdErrorCounter = 1000;
    }

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

/* Zadanie odpowiedzialne za pomiary analogowe:
 *   1. Czujnik ciśnienia butli,                    [ALMOST_DONE]
 *   2. 3 czuniki halla (2 przejdą na moduł GPS),   [TODO]
 *   3. Napięcie zasilania,                         [ALMOST_DONE]
 *   4. Krańcówki (I/O),                            [ALMOST_DONE]
 *   5. Potencjometr zaworu upustowego.             [ALMOST_DONE]
 */

void adcTask(void *arg) {
    
    while(1) {

        // Ciśnienie butli:
        mainDataFrame.tankPressure = analogRead(GPIO_NUM_34) / 1.0; // Dorobić dzielnik lub mnożnik lub funkcję map!

        // Akumulator:
        mainDataFrame.battery = analogRead(GPIO_NUM_36) / 254.0;

        // Krańcówki:
        if (!digitalRead(GPIO_NUM_14))
            mainDataFrame.upust.valveState = CLOSED;
        else if (!digitalRead(GPIO_NUM_17))
            mainDataFrame.upust.valveState = OPEN;
        else mainDataFrame.upust.valveState = MOVING;

        // Potencjometr zaworu upustowego:
        mainDataFrame.upust.potentiometer = analogRead(GPIO_NUM_39);        

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}