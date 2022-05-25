#include "../include/timers/watchdog.h"
#include "../include/structs/rocketControl.h"
#include "../include/structs/dataStructs.h"
#include "../include/tasks/tasks.h"
#include "../include/com/now.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <esp_wifi.h>
#include "../include/sensors/imuAPI.h"

WatchdogTimer wt;
RocketControl rc;

ImuAPI imu(&rc.hardware.i2c2);
ImuData imuData;

void setup() {
  Serial.begin(115200); //DEBUG
  
  
  /*
  Serial.print("Setup state: "); //DEBUG
  Serial.println(StateMachine::getCurrentState()); //DEBUG
  //BROWNOUT DETECTOT DISABLING
  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  WiFi.mode(WIFI_STA);
  esp_wifi_set_mac(ESP_IF_WIFI_STA, adressOBC);
  //set mission timer
  rc.missionTimer.setDisableValue(rc.options.countdownTime * -1);

  //set esp now
  if(nowInit() == false) ESP.restart();
  if(nowAddPeer(adressPitot, 0) == false) rc.errors.setEspNowError(ESPNOW_ADD_PEER_ERROR);
  if(nowAddPeer(adressMValve, 0) == false){
     rc.errors.setEspNowError(ESPNOW_ADD_PEER_ERROR);
    Serial.println("MVal add error");
  } 
  if(nowAddPeer(adressUpust, 0) == false) rc.errors.setEspNowError(ESPNOW_ADD_PEER_ERROR);
  if(nowAddPeer(adressBlackBox, 0) == false) rc.errors.setEspNowError(ESPNOW_ADD_PEER_ERROR);
  if(nowAddPeer(adressTanWa, 0) == false) rc.errors.setEspNowError(ESPNOW_ADD_PEER_ERROR);

  //init all components
  rc.hardware.i2c1.begin(I2C1_SDA, I2C1_SCL, 100E3);
  rc.hardware.i2c2.begin(I2C2_SDA, I2C2_SCL, 100E3);
  rc.hardware.i2c1.setTimeOut(20);
  rc.hardware.i2c2.setTimeOut(20);

  rc.hardware.mySPI.begin();

  //create Queues and Mutex //TODO
  rc.hardware.loraRxQueue = xQueueCreate(LORA_RX_QUEUE_LENGTH, sizeof(char[LORA_FRAME_ARRAY_SIZE]));
  rc.hardware.loraTxQueue = xQueueCreate(LORA_TX_QUEUE_LENGTH, sizeof(char[LORA_FRAME_ARRAY_SIZE]));
  rc.hardware.sdQueue = xQueueCreate(SD_QUEUE_LENGTH, sizeof(char[SD_FRAME_ARRAY_SIZE]));
  rc.hardware.flashQueue = xQueueCreate(FLASH_QUEUE_LENGTH, sizeof(DataFrame));
  rc.hardware.espNowQueue = xQueueCreate(ESP_NOW_QUEUE_LENGTH, sizeof(uint8_t));

  rc.hardware.spiMutex = xSemaphoreCreateMutex();
  rc.hardware.i2c1Mutex = xSemaphoreCreateMutex();

  //create Tasks
  //pro cpu
  xTaskCreatePinnedToCore(loraTask,       "LoRa task",        8192, NULL, 2, &rc.hardware.loraTask,       PRO_CPU_NUM);
  xTaskCreatePinnedToCore(rxHandlingTask, "RX handling task", 8192, NULL, 2, &rc.hardware.rxHandlingTask, PRO_CPU_NUM);

  //app cpu
  xTaskCreatePinnedToCore(stateTask, "State task", 8192, NULL, 5, &rc.hardware.stateTask, APP_CPU_NUM);
  xTaskCreatePinnedToCore(dataTask,  "Data task",  30000, NULL, 2, &rc.hardware.dataTask,  APP_CPU_NUM);
  xTaskCreatePinnedToCore(sdTask,    "SD task",    30000, NULL, 1, &rc.hardware.sdTask,    APP_CPU_NUM);
  xTaskCreatePinnedToCore(flashTask, "Flash task", 8192, NULL, 1, &rc.hardware.flashTask, APP_CPU_NUM);

  //create Timers
  rc.hardware.disconnectTimer = xTimerCreate("disconnect timer", disconnectDelay, pdFALSE, NULL, disconnectTimerCallback);

  rc.hardware.watchdogTimer = xTimerCreate("watchdog timer", watchdogDelay, pdTRUE, NULL, watchdogTimerCallback);
  
  //check created elements
  
  if(rc.hardware.loraRxQueue == NULL || rc.hardware.loraTxQueue == NULL || rc.hardware.sdQueue == NULL || rc.hardware.flashQueue == NULL || rc.hardware.espNowQueue == NULL){
    //error handling
    Serial.println("Queue create error!"); //DEBUG
    ESP.restart();
  }

  if(rc.hardware.spiMutex == NULL){
    //error handling
    Serial.println("Mutex create error!"); //DEBUG
    ESP.restart();
  }

  if(rc.hardware.loraTask == NULL || rc.hardware.rxHandlingTask == NULL){
    //error handling
    Serial.println("ProCPU task create error!"); //DEBUG
    ESP.restart();  
  }

  if(rc.hardware.stateTask == NULL || rc.hardware.dataTask == NULL || rc.hardware.sdTask == NULL || rc.hardware.flashTask == NULL){
    //error handling
    Serial.println("ProCPU task create error!"); //DEBUG
    ESP.restart();  
  }

  if(rc.hardware.watchdogTimer == NULL || rc.hardware.disconnectTimer == NULL){
    //error handling
    Serial.println("timer create error!"); //DEBUG
    ESP.restart();
  }
  
  
  //watchdogtimer
  wt.begin();
  wt.EEPROMread();
  Serial.println(wt.previousState); //DEBUG
  Serial.println((uint8_t) wt.resetCounter); //DEBUG
  
  if(wt.previousState != INIT && wt.previousState != COUNTDOWN){
    StateMachine::changeStateRequest((States) wt.previousState);
  }else{
    StateMachine::changeStateRequest(States::IDLE);
  }

  //start timers
  xTimerStart(rc.hardware.watchdogTimer, portMAX_DELAY);
  xTimerStart(rc.hardware.disconnectTimer, portMAX_DELAY);
  vTaskDelete(NULL); //delete main task (loop())
  */

 rc.hardware.i2c2.begin(I2C2_SDA, I2C2_SCL, 100E3);

 while(!imu.begin()){
   Serial.println("Imu error!");
   vTaskDelay(1000 / portTICK_PERIOD_MS);
 }
 imu.setInitPressure();
}

void loop() {
  imu.readData();
  
  imuData = imu.getData();

  Serial.println("IMU DATA");
  Serial.print("a.x: ");
  Serial.print(imuData.ax);
  Serial.print("\t");
  Serial.print("a.y: ");
  Serial.print(imuData.ay);
  Serial.print("\t");
  Serial.print("a.z: ");
  Serial.print(imuData.az);
  Serial.println("");

  Serial.print("g.x: ");
  Serial.print(imuData.gx);
  Serial.print("\t");
  Serial.print("g.y: ");
  Serial.print(imuData.gy);
  Serial.print("\t");
  Serial.print("g.z: ");
  Serial.print(imuData.gz);
  Serial.println("");

  Serial.print("m.x: ");
  Serial.print(imuData.mx);
  Serial.print("\t");
  Serial.print("m.y: ");
  Serial.print(imuData.my);
  Serial.print("\t");
  Serial.print("m.z: ");
  Serial.print(imuData.mz);
  Serial.println("");

  Serial.print("tmp: ");
  Serial.print(imuData.temperature);
  Serial.print("\t");
  Serial.print("prs: ");
  Serial.print(imuData.pressure);
  Serial.print("\t");
  Serial.print("alt: ");
  Serial.print(imuData.altitude);
  Serial.println("");
  delay(100);
}
