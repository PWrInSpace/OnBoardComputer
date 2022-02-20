#include "SingleTasks.h"

extern MainDataFrame mainDataFrame;
extern Queue queue;
extern MaximumData maxData;

String countStructData() {

    String frame = "R4MC;" + String((int) mainDataFrame.rocketState) + ";";

    frame += String(millis()) + ";";
    frame += String(mainDataFrame.battery) + ";";
    frame += String(mainDataFrame.pitotData.staticPressure)  + ";";
    frame += String(mainDataFrame.pitotData.dynamicPressure) + ";";
    frame += String(mainDataFrame.pitotData.altitude) + ";";
    frame += String(mainDataFrame.pitotData.speed) + ";";
    frame += String(mainDataFrame.pitotData.temperature) + ";";
    frame += String(mainDataFrame.upust.valveState) + ";";
    frame += String(mainDataFrame.upust.potentiometer) + ";";
    frame += String(mainDataFrame.mValve.valveState) + ";";
    frame += String(mainDataFrame.mValve.potentiometer) + ";";
    frame += String(mainDataFrame.tankPressure) + ";";
    frame += String(mainDataFrame.pressure) + ";";

    if (mainDataFrame.rocketState == GROUND) {

        frame += String(maxData.apogee) + ";";
        frame += String(maxData.maxSpeed) + ";";
        frame += String(maxData.maxAcc) + ";";
    }

    else {

        frame += String(mainDataFrame.altitude) + ";";
        frame += String(mainDataFrame.speed) + ";";
        frame += String(mainDataFrame.gForce) + ";";
    }

    frame += String(mainDataFrame.espNowErrorCounter) + ";";
    frame += String(mainDataFrame.sdErrorCounter) + ";";


    uint8_t separation2Byte = mainDataFrame.separationData & 0xFF;
    uint8_t separation1Byte = mainDataFrame.separationData >> 8;
    frame += String((int) separation1Byte) + ";";
    frame += String((int) separation2Byte) + ";";
    frame += String((int) mainDataFrame.countdown) + ";";
    frame += String((int) mainDataFrame.abortTimerSec) + "\n";

    return frame;
}

/**********************************************************************************************/

int upustTime_ms;

void uart2Handler() {

    // Dane przychodzące z uartu:
    if (Serial2.available()) {

        String dataFrom3Ant = "";

        while(Serial2.available()) {

            dataFrom3Ant += Serial2.readString();
            vTaskDelay(2 / portTICK_PERIOD_MS);
        }
    // TODO Dodanie danych do kolejki do zapisu we flashu
    }
}