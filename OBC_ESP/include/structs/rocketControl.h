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

#define CONNECTION_CHECK_DEVICE_NUMBER 6

struct RocketControl{
	Options options;  
  HardwareManagment hardware;
  Timer missionTimer;
  Errors errors;
  DataFrame dataFrame;
  bool isConnectedFlags[CONNECTION_CHECK_DEVICE_NUMBER];
  uint8_t connectedStatus;

  RecoverySTM recoveryStm = RecoverySTM(hardware.i2c1, RECOVERY_ADDRES);

	RocketControl() = default;
  void updateCurrentState();
  void sendLog(const char *message);
  uint32_t getDisconnectRemainingTime();
  void restartDisconnectTimer(bool _force = false);
  bool deactiveDisconnectTimer();
  bool queueSend(xQueueHandle _handle, char *data);
  bool allDevicesWokenUp();

  void createOptionsFrame(char *_array);
  void createLoRaFrame(char *_array);
  void createSDFrame(char *_array);
  
};

#endif