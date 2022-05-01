#ifndef ROCKET_MAIN_STRUCTS
#define ROCKET_MAIN_STRUCTS

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <string>
#include "FreeRTOS.h"
#include "../include/com/RecoverySTM.h"
#include "options.h"
#include "hardwareManagment.h"
#include "dataStructs.h"
#include "config.h"
#include "pinout.h"
#include "../include/timers/missionTimer.h"
#include "errors.h"
#include "../com/txEspNow.h"
#include "stateMachine.h"

#define LORA_RX_QUEUE_LENGTH 5
#define LORA_TX_QUEUE_LENGTH 5
#define SD_QUEUE_LENGTH 10
#define FLASH_QUEUE_LENGTH 20
#define ESP_NOW_QUEUE_LENGTH 5

struct RocketControl{
	Options options;  
  HardwareManagment hardware;
  Timer missionTimer;
  Errors errors;
  DataFrame dataFrame;

  RecoverySTM recoveryStm = RecoverySTM(hardware.i2c1, RECOVERY_ADDRES);

	RocketControl() = default;
  void updateCurrentState();
  void sendLog(const char *message);
  //void sendLog(String message);
  //void sendLog(std::string message);
  uint32_t getDisconnectRemainingTime();
  void restartDisconnectTimer(bool _force = false);
  bool deactiveDisconnectTimer();
  void createSDFrame(char *_array);
  void createLoRaFrame(char *_array);
  void createOptionsFrame(char *_array);
  bool allDevicesWokenUp();
  bool queueSend(xQueueHandle _handle, char *data);
  //void loraFrameForce();
};

#endif