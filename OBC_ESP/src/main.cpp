#include <Arduino.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "mainStructs.h"

WatchdogTimer wt;
RocketControl rc;

void setup() {
    Serial.begin(115200); //DEBUG
    //rc.state = INIT;

    //set esp now

    //init all components

    //create Queues and Mutex

    //create Tasks

    //create Timers
    rc.watchdogTimer = xTimerCreate("watchdog timer", watchdogDelay, pdTRUE, NULL, watchdogTimerCallback);
    rc.disconnectTimer = xTimerCreate("disconnect timer", disconnectDelay, pdFALSE, NULL, disconnectTimerCallback);

    //check created elements
    if(rc.watchdogTimer == NULL || rc.disconnectTimer == NULL){
        //error handling
        Serial.println("timer create error!"); //DEBUG
        //ESP.restart();
    }


    //check wachdog timer previous state
    wt.begin();
    wt.EEPROMread();
    Serial.println(wt.previousState); //DEBUG
    Serial.println((uint8_t) wt.resetCounter); //DEBUG
    
    if(wt.previousState != INIT){
        rc.state = (StateMachine) wt.previousState;
    }else{
        rc.state = IDLE;
    }

    //start watchdog timer
    xTimerStart(rc.watchdogTimer, portMAX_DELAY);
    xTimerStart(rc.disconnectTimer, portMAX_DELAY);
    
    vTaskDelete(NULL); //delete main task (loop())
}

void loop() {

}