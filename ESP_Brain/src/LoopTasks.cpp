#include "LoopTasks.h"

extern MainDataFrame mainDataFrame;
extern Queue queue;
extern bool forceStateAction;
extern Timer_ms frameTimer;

/* Zadanie zajmujące się wszystkim, co łączy się przez i2c:
 *   1. Atmega odzyskowa, [ALMOST_DONE]
 *   2. BME280.           [TODO]
 */

void i2cTask(void *arg) {
    
    Adafruit_BME280 bme;

    Wire.begin();

    vTaskDelay(500 / portTICK_PERIOD_MS);

    if(!bme.begin(0x76))
        mainDataFrame.sdErrorCounter++;

    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Pomiar początkowego ciśnienia:

    mainDataFrame.pressure = bme.readPressure() / 100.0F;
    mainDataFrame.initialPressure = mainDataFrame.pressure;
    mainDataFrame.altitude = bme.readAltitude(mainDataFrame.initialPressure);

    while (mainDataFrame.rocketState < FUELING) {

        if (frameTimer.flag) {

            frameTimer.flag = false;
        
            Wire.requestFrom(3, 1);
            vTaskDelay(10 / portTICK_PERIOD_MS);
            if (Wire.available()) {
                mainDataFrame.separationData = Wire.read();
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);

            mainDataFrame.pressure = bme.readPressure() / 100.0F;
            mainDataFrame.altitude = bme.readAltitude(mainDataFrame.initialPressure);

            Serial.println(mainDataFrame.pressure);
            Serial.println(mainDataFrame.altitude);
        }
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }

    Wire.beginTransmission(3);
    Wire.write((uint8_t) 8);
    Wire.endTransmission();
    vTaskDelay(40 / portTICK_PERIOD_MS);

    while (1) {
        
        if (frameTimer.flag) {

            frameTimer.flag = false;

            Wire.requestFrom(3, 1);
            vTaskDelay(10 / portTICK_PERIOD_MS);
            if (Wire.available()) {
                mainDataFrame.separationData = Wire.read();
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);

            mainDataFrame.pressure = bme.readPressure() / 100.0F;
            mainDataFrame.altitude = bme.readAltitude(mainDataFrame.initialPressure);
        }

        if (mainDataFrame.rocketState >= FLIGHT && mainDataFrame.rocketState < GROUND) {

            // Tutaj lecą pomiary z BME i obliczenia wszystkie (wysokość, prędkość, przyspieszenie)
            vTaskDelay(2 / portTICK_PERIOD_MS);

            if (forceStateAction && mainDataFrame.rocketState == FIRST_SEPAR) {

                forceStateAction = false;

                // Rozkaz separacji 1 st:
                Wire.beginTransmission(3);
                Wire.write((uint8_t) 24);
                Wire.endTransmission();
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }

            else if (forceStateAction && mainDataFrame.rocketState == SECOND_SEPAR) {

                forceStateAction = false;

                // Rozkaz separacji 2 st:
                Wire.beginTransmission(3);
                Wire.write((uint8_t) 56);
                Wire.endTransmission();
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }

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