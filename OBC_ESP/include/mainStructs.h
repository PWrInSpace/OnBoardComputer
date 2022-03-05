#ifndef ROCKET_MAIN_STRUCTS
#define ROCKET_MAIN_STRUCTS

#include <Arduino.h>
#include "FreeRTOS.h"
#include "missionTimer.h"

#define LORA_RX_QUEUE_LENGTH 10
#define LORA_TX_QUEUE_LENGTH 10
#define SD_QUEUE_LENGTH 10
#define FLASH_QUEUE_LENGTH 10
#define ESP_NOW_QUEUE_LENGTH 3

#define TANWA 1
#define PITOT 2
#define MAIN_VALVE 3
#define UPUST_VALVE 4
#define BLACK_BOX 5

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
  IDLE_EVENT = 1,
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
	bool flashWrite = true;
	bool forceLaunch : 1; 
	
  //change in statTask
  uint8_t upustValveRequestState : 2;
	uint8_t mainValveRequestState : 2;
	TickType_t dataFramePeriod = 100; 
  TickType_t loraDataPeriod = 500;
  TickType_t flashDataPeriod = 500;
  TickType_t sdFramePeriod = 1000;
  //

	uint16_t LoRaFrequencyMHz;
  uint16_t countdownTime = 5000;
	int32_t ignitionTime = -3000; //ignition time
  uint8_t tankMinPressure = 35; //bar
  uint16_t upustValveTime;
	uint16_t mainValveTime;
};

struct RocketControl{
  StateMachineEvent stateEvent;
  StateMachine state;
	Options options;  //TODO set options value
  Timer missionTimer;

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
	SemaphoreHandle_t spiMutex = NULL;
	//SemaphoreHandle_t i2cMutex_1 = NULL;
	//SemaphoreHandle_t i2cMutex_2 = NULL;

	//spinlock
	portMUX_TYPE stateLock = portMUX_INITIALIZER_UNLOCKED;

	//software timers
	TimerHandle_t watchdogTimer;
	TimerHandle_t disconnectTimer;

	//TODO new constructor with options or begin method
	RocketControl() = default;
  bool changeStateEvent(StateMachineEvent newEvent);
  //Use only in stateTask
  void changeState(StateMachine newState);
  void unsuccessfulEvent();
};

#endif