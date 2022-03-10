#ifndef ROCKET_MAIN_STRUCTS
#define ROCKET_MAIN_STRUCTS

#include <Arduino.h>
#include "FreeRTOS.h"
#include "config.h"

#define LORA_RX_QUEUE_LENGTH 10
#define LORA_TX_QUEUE_LENGTH 10
#define SD_QUEUE_LENGTH 10
#define FLASH_QUEUE_LENGTH 10
#define ESP_NOW_QUEUE_LENGTH 5

enum StateMachine{
  INIT = 0,
  IDLE,
  ARMED,
  FUELING,
  RDY_TO_LAUNCH,
  COUNTDOWN,
  FLIGHT,
  FIRST_STAGE_RECOVERY,
  SECOND_STAGE_RECOVERY,
  ON_GROUND,
  ABORT
};

enum StateMachineEvent{
  INIT_EVENT = 0,
  IDLE_EVENT,
  ARMED_EVENT,
  FUELING_EVENT,
  RDY_TO_LAUNCH_EVENT,
  COUNTDOWN_EVENT,
  FLIGHT_EVENT,
  FIRST_STAGE_RECOVERY_EVENT,
  SECOND_STAGE_RECOVERY_EVENT,
  ON_GROUND_EVENT,
  ABORT_EVENT
};


//options are change only in stateTasks, and in handlingTask obviously
struct Options{
	uint16_t LoRaFrequencyMHz; //IDK uint32_t
  uint32_t countdownTime;
	int16_t ignitionTime; //ignition time
  uint8_t tankMinPressure; //bar
  uint16_t espnowSleepTime;
  uint16_t espnowFastPeriod;
  uint16_t espnowSlowPeriod;

	bool flashWrite : 1;
	bool forceLaunch : 1; 
  //change in statTask
	uint8_t mainValveRequestState : 2;
  uint8_t upustValveRequestState : 2;
  uint16_t mainValveCommandTime;
  uint16_t upustValveCommandTime;


  TickType_t loraFastPeriod;
  TickType_t loraSlowPeriod;
  TickType_t dataFastPeriod;
  TickType_t flashFastPeriod;
  TickType_t flashSlowPeriod;
  TickType_t sdFastPeriod;
  TickType_t sdSlowPeriod;
  TickType_t sharedPeriod;
	TickType_t dataFramePeriod;
  TickType_t loraDataPeriod;
  TickType_t flashDataPeriod;
  TickType_t sdDataPeriod;
  
  Options();
};

struct RocketControl{
  StateMachineEvent stateEvent;
  StateMachine state;
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
	//mutex
	SemaphoreHandle_t spiMutex;
	//SemaphoreHandle_t i2cMutex_1 = NULL;
	//SemaphoreHandle_t i2cMutex_2 = NULL;

	//spinlock
	//portMUX_TYPE stateLock = portMUX_INITIALIZER_UNLOCKED;

	//software timers
	TimerHandle_t watchdogTimer;
	TimerHandle_t disconnectTimer;

	RocketControl();
  bool changeStateEvent(StateMachineEvent newEvent);
  //Use only in stateTask
  void changeState(StateMachine newState);
  void unsuccessfulEvent();
  void sendLog(const String & message);
  uint32_t getDisconnectRemainingTime();
};

#endif