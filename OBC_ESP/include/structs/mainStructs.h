/*
#ifndef ROCKET_MAIN_STRUCTS
#define ROCKET_MAIN_STRUCTS

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "FreeRTOS.h"
#include "../include/com/RecoverySTM.h"
#include "config.h"
#include "pinout.h"

#define LORA_RX_QUEUE_LENGTH 5
#define LORA_TX_QUEUE_LENGTH 5
#define SD_QUEUE_LENGTH 10
#define FLASH_QUEUE_LENGTH 20
#define ESP_NOW_QUEUE_LENGTH 5


//options are change only in stateTasks, and in handlingTask obviously
struct Options{
	uint16_t LoRaFrequencyMHz = 868;
  uint32_t countdownTime;
	int16_t ignitionTime; //ignition time
  uint8_t tankMinPressure; //bar
	bool flashWrite : 1;
	bool forceLaunch : 1; 
  //change in statTask

  uint16_t espnowSleepTime;
  uint16_t espnowShortPeriod;
  uint16_t espnowLongPeriod;
  TickType_t flashShortPeriod;
  TickType_t flashLongPeriod;
  TickType_t sdShortPeriod;
  TickType_t sdLongPeriod;
  TickType_t idlePeriod;
	TickType_t dataFramePeriod;
  TickType_t loraPeriod;
  TickType_t flashDataCurrentPeriod;
  TickType_t sdDataCurrentPeriod;
  
  Options();
};

struct RocketControl{
	Options options;  
 
	//tasks
	TaskHandle_t loraTask;
	TaskHandle_t rxHandlingTask;
	TaskHandle_t stateTask;
	TaskHandle_t dataTask;
	TaskHandle_t sdTask;
	TaskHandle_t flashTask;
	//queues
	QueueHandle_t loraRxQueue;
	QueueHandle_t loraTxQueue;
	QueueHandle_t sdQueue;
	QueueHandle_t flashQueue;
  QueueHandle_t espNowQueue; //best solution XD
	//mutexes
	SemaphoreHandle_t spiMutex;
	SemaphoreHandle_t i2c1Mutex;

	//spinlock
	portMUX_TYPE stateLock = portMUX_INITIALIZER_UNLOCKED;

	//software timers
	TimerHandle_t watchdogTimer;
	TimerHandle_t disconnectTimer;

  // Hardware handlers
  SPIClass mySPI = SPIClass(VSPI);
  TwoWire i2c1 = TwoWire(0);
  TwoWire i2c2 = TwoWire(1);

  RecoverySTM recoveryStm = RecoverySTM(i2c1, RECOVERY_ADDRES);

	RocketControl();
  void sendLog(char *message);
  uint32_t getDisconnectRemainingTime();
};
#endif
*/