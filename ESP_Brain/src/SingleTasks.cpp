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

        vTaskDelay(2 / portTICK_PERIOD_MS);
        String dataFrom3Ant = Serial2.readString();

        if (strstr(dataFrom3Ant.c_str(), "MNCP;STAT") != NULL) {

            mainDataFrame.countdown++;

            uint8_t oldState, newState;
            sscanf(dataFrom3Ant.c_str(), "MNCP;STAT;%d;%d", (int*) &oldState, (int*) &newState);

            //if (oldState == mainDataFrame.rocketState)
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

void valveOpen(void *arg){
    int pwm = 140;
    if(!digitalRead(GPIO_NUM_27)){ // gdy druga krancówka jest zwarta
        Serial.println("Jazda");
        vTaskDelete(NULL);
        return;
    }

    digitalWrite(VALVE1, HIGH);
    digitalWrite(VALVE2, LOW);
    ledcWrite(valvePWMChanel, pwm);

    while(digitalRead(GPIO_NUM_27)){
        if(pwm < 255){
            pwm += 5;
            ledcWrite(valvePWMChanel, pwm);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
        Serial.println(String(pwm));
    }

    digitalWrite(VALVE1, LOW);
    ledcWrite(valvePWMChanel, 0);
    
    vTaskDelete(NULL);
}
