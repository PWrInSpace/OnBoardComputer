#include "../include/timers/watchdog.h"
#include "../lib/data_struct/data_structs.h"
#include "../include/structs/rocketControl.h"
#include "../include/tasks/tasks.h"
#include "../include/com/now.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <esp_wifi.h>
#include "../include/sensors/imuAPI.h"
#include "../include/components/runcam.h"
#include "../include/pysd/gen_pysd.h"
#include "../include/components/buzzer.h"

WatchdogTimer wt;
RocketControl rc;

void setup() {
  Serial.begin(115200); //DEBUG

  BUZZ_init(BUZZER);
  BUZZ_on();
  vTaskDelay(pdMS_TO_TICKS(250));
  BUZZ_off();

  SM_init(&rc.hardware.stateTask);
  DF_init();
  OPT_init();
  if (ERR_init() == false) {
    ESP.restart();
  }
  Serial.print("Setup state: "); //DEBUG
  Serial.println(SM_getCurrentState()); //DEBUG
  //BROWNOUT DETECTOT DISABLING
  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  WiFi.mode(WIFI_STA);
  esp_wifi_set_mac(WIFI_IF_STA, adressOBC);
  //set mission timer
  rc.missionTimer.setDisableValue(OPT_get_countdown_begin_time());
  Serial.println("Run cam init");
  RUNCAM_init(CAMERA);

  //set esp now
  Serial.println("Now init");
  if(nowInit() == false) ESP.restart();
  if(nowAddPeer(adressPitot, 0) == false) ERR_set_esp_now_error(ESPNOW_ADD_PEER_ERROR);
  if(nowAddPeer(adressMValve, 0) == false) ERR_set_esp_now_error(ESPNOW_ADD_PEER_ERROR);
  if(nowAddPeer(adressUpust, 0) == false) ERR_set_esp_now_error(ESPNOW_ADD_PEER_ERROR);
  if(nowAddPeer(adressBlackBox, 0) == false) ERR_set_esp_now_error(ESPNOW_ADD_PEER_ERROR);
  if(nowAddPeer(adressTanWa, 0) == false) ERR_set_esp_now_error(ESPNOW_ADD_PEER_ERROR);
  if(nowAddPeer(adressPayLoad, 0) == false) ERR_set_esp_now_error(ESPNOW_ADD_PEER_ERROR);


  //init all components
  Serial.println("I2C init");
  rc.hardware.i2c1.begin(I2C1_SDA, I2C1_SCL, 100E3);
  rc.hardware.i2c2.begin(I2C2_SDA, I2C2_SCL, 100E3);
  rc.hardware.i2c1.setTimeOut(20);
  rc.hardware.i2c2.setTimeOut(20);

  Serial.println("SPI init");
  rc.hardware.mySPI.begin();

  //create Queues and Mutex //TODO
  Serial.println("Queue init");
  rc.hardware.loraRxQueue = xQueueCreate(LORA_RX_QUEUE_LENGTH, sizeof(char[LORA_FRAME_ARRAY_SIZE]));
  rc.hardware.loraTxQueue = xQueueCreate(LORA_TX_QUEUE_LENGTH, sizeof(char[LORA_FRAME_ARRAY_SIZE]));
  rc.hardware.sdQueue = xQueueCreate(SD_QUEUE_LENGTH, sizeof(char[SD_FRAME_ARRAY_SIZE]));
  rc.hardware.flashQueue = xQueueCreate(FLASH_QUEUE_LENGTH, sizeof(DataFrame));
  rc.hardware.espNowQueue = xQueueCreate(ESP_NOW_QUEUE_LENGTH, sizeof(uint8_t));

  Serial.println("Mutex init");
  rc.hardware.spiMutex = xSemaphoreCreateMutex();
  rc.hardware.i2c1Mutex = xSemaphoreCreateMutex();

  //create Tasks
  //pro cpu
  Serial.println("Pro cpu tasks init");
  xTaskCreatePinnedToCore(loraTask,       "LoRa task",        8192, NULL, 5, &rc.hardware.loraTask,       PRO_CPU_NUM);
  xTaskCreatePinnedToCore(rxHandlingTask, "RX handling task", 8192, NULL, 2, &rc.hardware.rxHandlingTask, PRO_CPU_NUM);

  //app cpu
  Serial.println("App cpu tasks init");
  xTaskCreatePinnedToCore(stateTask, "State task", 8192, NULL, 5, &rc.hardware.stateTask, APP_CPU_NUM);
  xTaskCreatePinnedToCore(dataTask,  "Data task",  45000, NULL, 5, &rc.hardware.dataTask,  APP_CPU_NUM);
  xTaskCreatePinnedToCore(sdTask,    "SD task",    30000, NULL, 1, &rc.hardware.sdTask,    APP_CPU_NUM);
  xTaskCreatePinnedToCore(flashTask, "Flash task", 8192, NULL, 1, &rc.hardware.flashTask, APP_CPU_NUM);

  //create Timers
  Serial.println("Timers init");
  rc.hardware.disconnectTimer = xTimerCreate("disconnect timer",
                                              disconnectDelay,
                                              pdFALSE,
                                              NULL,
                                              disconnectTimerCallback);

  rc.hardware.watchdogTimer = xTimerCreate("watchdog timer",
                                            watchdogDelay,
                                            pdTRUE,
                                            NULL,
                                            watchdogTimerCallback);

  rc.hardware.espNowConnectionTimer = xTimerCreate("espnow timer",
                                                    espNowConnectionCheckPeriod,
                                                    pdTRUE,
                                                    NULL,
                                                    espNowConnectionCallback);

  //check created elements
  if (rc.hardware.loraRxQueue == NULL ||
      rc.hardware.loraTxQueue == NULL ||
      rc.hardware.sdQueue == NULL ||
      rc.hardware.flashQueue == NULL ||
      rc.hardware.espNowQueue == NULL){
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

  // if (rc.hardware.stateTask == NULL ||
  //     rc.hardware.dataTask == NULL ||
  //     rc.hardware.sdTask == NULL ||
  //     rc.hardware.flashTask == NULL){
  //   //error handling
  //   Serial.println("ProCPU task create error!"); //DEBUG
  //   ESP.restart();
  // }

  if(rc.hardware.watchdogTimer == NULL || rc.hardware.disconnectTimer == NULL){
    //error handling
    Serial.println("timer create error!"); //DEBUG
    ESP.restart();
  }

  //watchdogtimer
  /*
  wt.begin();
  wt.EEPROMread();
  Serial.println(wt.previousState); //DEBUG
  Serial.println((uint8_t) wt.resetCounter); //DEBUG
  if(wt.previousState != INIT && wt.previousState != COUNTDOWN){
    SM_changeStateRequest((States) wt.previousState);
  }else{
    SM_changeStateRequest(States::IDLE);
  }

  */
  //start timers
  Serial.println("Timers start");
  if (SM_changeStateRequest(States::IDLE) == false) {
    Serial.println("Failed to change state to IDLE");
    ESP.restart();
  }
  xTimerStart(rc.hardware.disconnectTimer, portMAX_DELAY);
  xTimerStart(rc.hardware.espNowConnectionTimer, portMAX_DELAY);
  //xTimerStart(rc.hardware.watchdogTimer, portMAX_DELAY);

  Serial.print("State after init: ");
  Serial.println(SM_getCurrentState());
  Serial.println("END INIT");
  vTaskDelete(NULL); //delete main task (loop())
}

void loop() {

}
