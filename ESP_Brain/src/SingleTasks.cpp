#include "SingleTasks.h"

extern MainDataFrame mainDataFrame;
extern Queue queue;
extern bool forceStateAction;

String countStructData() {

    String frame = "R4MC;" + String((int) mainDataFrame.rocketState) + ";";
    
    frame += String(millis()) + ";";
    frame += String(mainDataFrame.battery) + ";";
    frame += String(mainDataFrame.pitotData.staticPressure) + ";";
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

            int oldState, newState;
            sscanf(dataFrom3Ant.c_str(), "MNCP;STAT;%d;%d", &oldState, &newState);

            if (oldState == mainDataFrame.rocketState) {
                mainDataFrame.rocketState = newState;
                forceStateAction = true;
            }
        }

        else if (strstr(dataFrom3Ant.c_str(), "MNCP;ABRT") != NULL) {
            
            mainDataFrame.rocketState = ABORT;
            forceStateAction = true;
        }

        else if (strstr(dataFrom3Ant.c_str(), "R4TN") != NULL || strstr(dataFrom3Ant.c_str(), "R4GP") != NULL) {
            
            queue.push(dataFrom3Ant);
        }

        else if (mainDataFrame.rocketState == FUELING) { 
            // TODO polecenia zaworu upustowego
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

/*********************************************************************************************/
//                                  ZAWÓR UPUSTOWY                                           //


void valveInit(){
    pinMode(VALVE1, OUTPUT);
    pinMode(VALVE2, OUTPUT);
    pinMode(VALVE_PWM, OUTPUT);
    pinMode(GPIO_NUM_14, INPUT_PULLUP);
    pinMode(GPIO_NUM_27, INPUT_PULLUP);

    ledcSetup(valvePWMChanel, valveFrequency, valveResolution);
    ledcAttachPin(VALVE_PWM, valvePWMChanel);
    Serial.println("Valve init complete");
    
    digitalWrite(VALVE1, LOW);
    digitalWrite(VALVE2, LOW);
    ledcWrite(valvePWMChanel, 0);
}

void valveMove(const uint8_t & limitSwitchPIN, const uint8_t & highValvePIN, const uint8_t & valveSpeed){
    if(!digitalRead(limitSwitchPIN)){ // gdy krancówka jest zwarta
        vTaskDelete(NULL);
        return;
    }

    //wlaczenie silnika
    digitalWrite(highValvePIN, HIGH);
    ledcWrite(valvePWMChanel, valveSpeed);

    //oczekiwanie az sie obroci
    while(digitalRead(limitSwitchPIN)){};

    //wylaczenie silnika
    digitalWrite(highValvePIN, LOW);
    ledcWrite(valvePWMChanel, 0);
}

void valveOpen(void *arg){
    valveMove(GPIO_NUM_27, VALVE1);
    
    vTaskDelete(NULL);
}

void valveClose(void *arg){
    valveMove(GPIO_NUM_14, VALVE2);
    
    vTaskDelete(NULL);
}

void valveTimeOpen(void *arg){
    unsigned int openTime = 5000; //(ms)
    unsigned long int valveTimer;

    valveMove(GPIO_NUM_27, VALVE1);

    valveTimer = millis();
    while(millis() - valveTimer < openTime){
        Serial.println(String(millis() - valveTimer));
    }
    
    valveMove(GPIO_NUM_14, VALVE2);
    vTaskDelete(NULL);
}


