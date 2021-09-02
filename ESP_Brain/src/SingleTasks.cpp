#include "SingleTasks.h"

extern MainDataFrame mainDataFrame;
extern Queue queue;
extern bool forceStateAction;

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

void saveFrameHeaders() {

    String espFrame = "R4MC;State;Time_ms;Voltage;";

    espFrame += "Pt_stat_press;Pt_dyna_press;Pt_alt;Pt_speed;Pt_temp;";
    espFrame += "Upust_state;Upust_analog;MValve_state;MValve_analog;Tank_press;";
    espFrame += "Press;Altitude;Speed;Acceleration;";
    espFrame += "ESP_errors;SD_errors;Separation_frame;Countdown\n";

    queue.push(espFrame);

    String gpsFrame = "R4GP;Lal;Long;Alt;Satelites;Time_s;Hal1;Hal2;Hal3;HAl4;Hal5\n";

    queue.push(gpsFrame);
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

        /*------------------------------------*/
        // Sterowanie zaworem upustowym:

        else if (mainDataFrame.rocketState == FUELING) { 

            if (strstr(dataFrom3Ant.c_str(), "MNCP;UPST") != NULL) {

                int upustMode = -1; // mode - 0 - zamykanie, 1 - otwieranie, upustTime - czas, gdy równy zero, to otwarcie na stałe.
                sscanf(dataFrom3Ant.c_str(), "MNCP;UPST;%d;%d", &upustMode, &upustTime_ms);

                if (upustMode == 0)
                    xTaskCreate(valveClose, "Task close valve", 4096, NULL, 2, NULL);

                else if (upustMode == 1) {
                    if (!upustTime_ms)
                        xTaskCreate(valveOpen, "Task open valve", 4096, NULL, 2, NULL);
                    else
                        xTaskCreate(valveTimeOpen, "Task open valve", 4096, NULL, 2, NULL);
                    
                    vTaskDelay(2 / portTICK_PERIOD_MS);
                }

            }
        }
    }
}

/**********************************************************************************************/

uint32_t lastSendTime = 0;

void sendData(String txData) {

    if (millis() - lastSendTime > 950) {

        Serial2.print(txData);
        lastSendTime = millis();
    }
}

/*********************************************************************************************/
/*********************************************************************************************/
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

/*********************************************************************************************/

void valveMove(const uint8_t & limitSwitchPIN, const uint8_t & highValvePIN, const uint8_t & valveSpeed){
    if(!digitalRead(limitSwitchPIN)){ // gdy krancówka jest zwarta
        return;
    }

    //wlaczenie silnika
    digitalWrite(highValvePIN, HIGH);
    ledcWrite(valvePWMChanel, valveSpeed);

    //oczekiwanie az sie obroci
    while(digitalRead(limitSwitchPIN)){
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }

    //wylaczenie silnika
    digitalWrite(highValvePIN, LOW);
    ledcWrite(valvePWMChanel, 0);
}

/*********************************************************************************************/

void valveOpen(void *arg){
    valveMove(GPIO_NUM_27, VALVE1);
    
    vTaskDelete(NULL);
}

/*********************************************************************************************/

void valveClose(void *arg){
    valveMove(GPIO_NUM_14, VALVE2);
    
    vTaskDelete(NULL);
}

/*********************************************************************************************/

void valveTimeOpen(void *arg){

    uint32_t openTime = upustTime_ms;
    unsigned long int valveTimer;

    valveMove(GPIO_NUM_27, VALVE1);

    valveTimer = millis();
    while(millis() - valveTimer < openTime){
        Serial.println(String(millis() - valveTimer));
    }
    
    valveMove(GPIO_NUM_14, VALVE2);
    vTaskDelete(NULL);
}


