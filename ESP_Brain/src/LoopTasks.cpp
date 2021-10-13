#include "LoopTasks.h"

extern MainDataFrame mainDataFrame;
extern Queue queue;
extern Timer_ms frameTimer;
extern MaximumData maxData;

/* Zadanie zajmujące się wszystkim, co łączy się przez i2c:
 *   1. Atmega odzyskowa, [DONE]
 *   2. BME280.           [DONE]
 */

void i2cTask(void *arg) {
    
    // Inicjalizacja wszystkiego do i2c:
    Adafruit_BME280 bme;
    i2cTaskInit(bme);

    /*------------------------------------*/

    // Oczekiwanie:
    while (mainDataFrame.rocketState < FUELING) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    // Uzbrojenie odzysku:
    i2cSendByte(3, 8, 2);

    uint32_t lastMeasuredTime = millis(); // Do wyliczania szybkości i przyspieszenia.
    
    // Powtórny pomiar początkowego ciśnienia - jakby od początkowego minęło za dużo czasu:
    mainDataFrame.initialPressure = bme.readPressure() / 100.0F;
    
    Timer_ms separCommTimer;
    separCommTimer.setVal(WAIT_DATA_PERIOD*2);

    while (1) {
        
        /*------------------------------------*/
        // Cykliczne pomiary i obliczenia:

        if (frameTimer.flag) {

            frameTimer.flag = false;

            // Komunikacja z odzyskiem:
            if (separCommTimer.check()) {
                Wire.requestFrom(3, 2);
                vTaskDelay(1 / portTICK_PERIOD_MS);
                if (Wire.available()) {
                    Wire.readBytes((uint8_t*) &mainDataFrame.separationData, sizeof(mainDataFrame.separationData));
                }
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);

            // Pomiary ciśnienia i obliczenia:

            int oldAlt = mainDataFrame.altitude;
            float oldSpeed = mainDataFrame.speed;

            float tmpPress = bme.readPressure() / 100.0F;
            if (tmpPress > 200)
                mainDataFrame.pressure = tmpPress;
            mainDataFrame.altitude = 44330.0 * (1.0 - pow(mainDataFrame.pressure / mainDataFrame.initialPressure, 0.1903));

            float deltaT_s = (millis() - lastMeasuredTime) / 1000.0F;
            mainDataFrame.speed = (mainDataFrame.altitude - oldAlt) / deltaT_s;
            mainDataFrame.gForce = (mainDataFrame.speed - oldSpeed) / deltaT_s;

            // Uzupełnienie danych maxymalnych:
            if (mainDataFrame.altitude > maxData.apogee)    maxData.apogee    = mainDataFrame.altitude;
            if (mainDataFrame.speed > maxData.maxSpeed)     maxData.maxSpeed  = mainDataFrame.speed;
            if (mainDataFrame.gForce > maxData.maxAcc)      maxData.maxAcc    = mainDataFrame.gForce;

            lastMeasuredTime = millis();
        }

        /*------------------------------------*/
        // Rozkazy awaryjnej separacji:

        if (mainDataFrame.rocketState >= FLIGHT && mainDataFrame.rocketState < GROUND) {            

            vTaskDelay(2 / portTICK_PERIOD_MS);

            // Rozkaz separacji 1 st:
            if (mainDataFrame.forceSeparation && mainDataFrame.rocketState == FIRST_SEPAR) {

                mainDataFrame.forceSeparation = false;
                i2cSendByte(3, 24, 2);
            }

            // Rozkaz separacji 2 st:
            else if (mainDataFrame.forceSeparation && mainDataFrame.rocketState == SECOND_SEPAR) {

                mainDataFrame.forceSeparation = false;
                i2cSendByte(3, 56, 2);
            }

        }

    vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/**********************************************************************************************/

void i2cTaskInit(Adafruit_BME280 &bme) {

    Wire.begin(GPIO_NUM_21, GPIO_NUM_22, 50000);

    vTaskDelay(500 / portTICK_PERIOD_MS);

    if(!bme.begin(0x76))
        mainDataFrame.sdErrorCounter++;

    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Pomiar początkowego ciśnienia:
    mainDataFrame.pressure = bme.readPressure() / 100.0F;
    mainDataFrame.initialPressure = mainDataFrame.pressure;
    mainDataFrame.altitude = 44330.0 * (1.0 - pow(mainDataFrame.pressure / mainDataFrame.initialPressure, 0.1903));

    // Sprawdzenie komunikacji z odzyskiem:
    Wire.requestFrom(3, 2);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    if (Wire.available()) {
        Wire.readBytes((uint8_t*) &mainDataFrame.separationData, sizeof(mainDataFrame.separationData));
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

/**********************************************************************************************/
/**********************************************************************************************/
/**********************************************************************************************/

/* Zadanie odpowiedzialne za logowanie wszystkiego na SD -> pobierając dane z kolejki. [DONE]
 */

void sdTask(void *arg) {

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    SPIClass SPISD(HSPI);
    SPISD.begin(GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_15);
    SPI.setClockDivider(SPI_CLOCK_DIV2);

    while(1) {

        while(queue.getNumberOfElements()){

            String dataFrame = queue.pop();

            Serial.print(dataFrame); // Dla debugu

            switch (dataFrame[2]) {
            case 'M':
                SD_write("/R4_data.txt", dataFrame, SPISD);
                break;

            case 'T':
                SD_write("/R4_tanwa.txt", dataFrame, SPISD);
                break;

            case 'G':
                SD_write("/R4_gps.txt", dataFrame, SPISD);
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
 *   3. Napięcie zasilania,                         [DONE]
 *   4. Krańcówki (I/O),                            [TESTING]
 *   5. Potencjometr zaworu upustowego.             [TESTING]
 */

void adcTask(void *arg) {

    while(1) {

        // Ciśnienie butli:
        mainDataFrame.tankPressure = analogRead(GPIO_NUM_34) * 0.0201465 * 1.11; // * 82.5 / 4095
        /* 
         * Mnożymy przez 82.5, bo przy takim ciśnieniu będzie maksymalne napięcie na adc - 3.3v
         * (Czujnik daje 10v przy 250 barach).
         * dzielimy przez 4095 bo to max wartość z przetwornika
         */

        // Akumulator:
        mainDataFrame.battery = analogRead(GPIO_NUM_36) / 254.0 * 0.893;

        // Krańcówki:
        if (!digitalRead(GPIO_NUM_14))
            mainDataFrame.upust.valveState = CLOSED;
        else if (!digitalRead(GPIO_NUM_27))
            mainDataFrame.upust.valveState = OPEN;
        else mainDataFrame.upust.valveState = MOVING;

        // Potencjometr zaworu upustowego:
        mainDataFrame.upust.potentiometer = analogRead(GPIO_NUM_39);        

        vTaskDelay(WAIT_DATA_PERIOD / portTICK_PERIOD_MS);
    }
}

/**********************************************************************************************/

void i2cSendByte(uint8_t adress, uint8_t val, int8_t times) {

    for (; times > 0; times--) {
        Wire.beginTransmission(adress);
        Wire.write(val);
        Wire.endTransmission();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}