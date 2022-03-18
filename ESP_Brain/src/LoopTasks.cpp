#include "LoopTasks.h"

extern DataFrame dataFrame;
extern File file;
extern QueueHandle_t queue;
extern QueueHandle_t sdQueue;

void sdTask(void *arg) {

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    SPIClass SPISD(HSPI);
    SPISD.begin(GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_15);
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    
    SD.begin(SD_CS, SPISD);

    while(1) {
    
        if (uxQueueMessagesWaiting(queue) > 0) {

            DataFrame data;
            xQueueReceive(sdQueue, &data, portMAX_DELAY);

            sdWrite("/R4_data.txt", data2String(data));        
        }
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }
}

/**********************************************************************************************/

String data2String(DataFrame data) {

    char txString[250];

	sprintf(txString, "R4OR;%d;%f;%f;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%f;%d;%d;%d;%d;%d;%f;%f;%d;%d\n",
        (int)data.time_ms, 	    data.batteryV, 		    data.tankPressure,
        data.gpsLatitude, 		data.gpsLongitude, 	    data.gpsAltitude,
        data.gpsSatNum, 		data.gpsTimeS,			data.hallSensors[0],
        data.hallSensors[1], 	data.hallSensors[2],	data.hallSensors[3],
        data.hallSensors[4], 	data.tanWaVoltage,		data.tanWaState,
        data.tanWaIgniter,		data.tanWaFill,		    data.tanWaDepr,
        data.tanWaUpust, 		data.rocketWeightKg, 	data.tankWeightKg,
        (int)data.rocketWRaw, 	(int)data.tankWRaw);

    return String(txString);
}

/**********************************************************************************************/

/* Zadanie odpowiedzialne za logowanie wszystkiego na Flashu -> pobierając dane z kolejki.
 */

void flashTask(void *arg) {

    char writeMode[2] = "w";

    while (1) {

        if (uxQueueMessagesWaiting(queue) > 10) {

            DataFrame data;

            file = LITTLEFS.open("/file.txt", writeMode);
            writeMode[0] = 'a'; // zmiana na append po pierwszym zapisie.

            while (uxQueueMessagesWaiting(queue) > 0) {

                xQueueReceive(queue, &data, portMAX_DELAY);
                file.write((uint8_t*) &data, sizeof(data));
            }

            file.close();
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

/**********************************************************************************************/

/* Funkcja odpowiedzialna za pomiary analogowe.
 */

void adcMeasure() {

    EspBinData measureData;

    // Ciśnienie butli:
    measureData.tankPressure = analogRead(GPIO_NUM_34) * 0.0201465 * 1,18; // * 82.5 / 4095
    /* 
        * Mnożymy przez 82.5, bo przy takim ciśnieniu będzie maksymalne napięcie na adc - 3.3v
        * (Czujnik daje 10v przy 250 barach).
        * dzielimy przez 4095 bo to max wartość z przetwornika
        */

    // Akumulator:
    measureData.batteryV = analogRead(GPIO_NUM_36) / 254.0 * 0.893;

    Serial2.write((uint8_t*) &measureData, sizeof(measureData));
    Serial.printf("ESP;%f;%f\n", measureData.batteryV, measureData.tankPressure);
}