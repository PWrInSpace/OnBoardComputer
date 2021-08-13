#include "SingleTasks.h"

extern MainDataFrame mainDataFrame;
extern Queue queue;

String countStructData() {

    String frame = "R4MC;" + String(millis()) + ";";
    
    frame += String(mainDataFrame.rocketState) + ";";
    frame += String(mainDataFrame.battery) + ";";
    frame += String(mainDataFrame.pitotData.staticPressure) + ";";
    frame += String(mainDataFrame.pitotData.dynamicPressure) + ";";
    frame += String(mainDataFrame.pitotData.altitude) + ";";
    frame += String(mainDataFrame.pitotData.speed) + ";";
    frame += String(mainDataFrame.pitotData.temperature) + ";";
    frame += String(mainDataFrame.upust.endStop1) + ";";
    frame += String(mainDataFrame.upust.endStop2) + ";";
    frame += String(mainDataFrame.upust.potentiometer) + ";";
    frame += String(mainDataFrame.tankPressure) + ";";
    frame += String(mainDataFrame.pressure) + ";";
    frame += String(mainDataFrame.altitude) + ";";
    frame += String(mainDataFrame.speed) + ";";
    frame += String(mainDataFrame.gForce) + ";";
    frame += String(mainDataFrame.espNowErrorCounter) + ";";
    frame += String(mainDataFrame.sdErrorCounter) + ";";
    frame += String((int) mainDataFrame.separationData) + "\n";

    return frame;
}

/**********************************************************************************************/

void uart2Handler() {

    // Dane przychodzące z uartu:
    if (Serial2.available()) {

        mainDataFrame.pressure = 222;

        vTaskDelay(2 / portTICK_PERIOD_MS);
        String dataFrom3Ant = Serial2.readString();
        
        if (dataFrom3Ant.indexOf("R4TN") >= 0 || dataFrom3Ant.indexOf("R4GP") >= 0) {
            queue.push(dataFrom3Ant);
        }

        //else if () TODO polecenia zaworu upustowego

        else if (dataFrom3Ant.indexOf("MNCP;STAT") >= 0) {

            uint8_t oldState, newState;
            sscanf(dataFrom3Ant.c_str(), "MNCP;STAT;%d;%d", (int*) &oldState, (int*) &newState);

            if (oldState == mainDataFrame.rocketState)
                mainDataFrame.rocketState = newState;
        }

        else if (dataFrom3Ant.indexOf("MNCP;ABRT") == 0) {
            
            mainDataFrame.rocketState = ABORT;
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