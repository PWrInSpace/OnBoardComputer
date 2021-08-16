#include "SingleTasks.h"

extern MainDataFrame mainDataFrame;
extern Queue queue;

String countStructData() {

    String frame = "R4MC;" + String(millis()) + ";";
    
    frame += String((int) mainDataFrame.rocketState) + ";";
    frame += String(mainDataFrame.battery) + ";";
    frame += String(mainDataFrame.pitotData.staticPressure) + ";";
    frame += String(mainDataFrame.pitotData.dynamicPressure) + ";";
    frame += String(mainDataFrame.pitotData.altitude) + ";";
    frame += String(mainDataFrame.pitotData.speed) + ";";
    frame += String(mainDataFrame.pitotData.temperature) + ";";
    frame += String(mainDataFrame.upust.endStop1) + ";";
    frame += String(mainDataFrame.upust.endStop2) + ";";
    frame += String(mainDataFrame.upust.potentiometer) + ";";
    frame += String(mainDataFrame.mValve.endStop1) + ";";
    frame += String(mainDataFrame.mValve.endStop2) + ";";
    frame += String(mainDataFrame.mValve.potentiometer) + ";";
    frame += String(mainDataFrame.tankPressure) + ";";
    frame += String(mainDataFrame.pressure) + ";";
    frame += String(mainDataFrame.altitude) + ";";
    frame += String(mainDataFrame.speed) + ";";
    frame += String(mainDataFrame.gForce) + ";";
    frame += String(mainDataFrame.espNowErrorCounter) + ";";
    frame += String(mainDataFrame.sdErrorCounter) + ";";
    frame += String((int) mainDataFrame.separationData) + ";";
    frame += String((int) mainDataFrame.countdown) + "\n";

    return frame;
}

/**********************************************************************************************/

void uart2Handler() {

    // Dane przychodzące z uartu:
    if (Serial2.available()) {

        String dataFrom3Ant = "";

        while(Serial2.available()) {

            dataFrom3Ant += Serial2.readString();
            vTaskDelay(2 / portTICK_PERIOD_MS);
        }

        if (strstr(dataFrom3Ant.c_str(), "MNCP;STAT") != NULL) {

            mainDataFrame.countdown++;

            int oldState, newState;
            sscanf(dataFrom3Ant.c_str(), "MNCP;STAT;%d;%d", &oldState, &newState);

            if (oldState == mainDataFrame.rocketState)
                mainDataFrame.rocketState = newState;
        }

        else if (strstr(dataFrom3Ant.c_str(), "MNCP;ABRT") != NULL) {
            
            mainDataFrame.rocketState = ABORT;
        }

        //else if () TODO polecenia zaworu upustowego

        else if (strstr(dataFrom3Ant.c_str(), "R4TN") != NULL || strstr(dataFrom3Ant.c_str(), "R4GP") != NULL) {
            
            queue.push(dataFrom3Ant);
        }

    }
}

/**********************************************************************************************/

uint32_t lastSendTime = 0;

void sendData(String txData) {

    if (millis() - lastSendTime > 1000) {

        Serial2.print(txData);
        lastSendTime = millis();
    }
}