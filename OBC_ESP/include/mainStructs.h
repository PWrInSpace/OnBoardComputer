#ifndef ROCKET_MAIN_STRUCTS
#define ROCKET_MAIN_STRUCTS

#include <Arduino.h>
#include "FreeRTOS.h"

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


struct Options{
	bool flashWrite : 1;
	bool forceLaunch : 1; 
	uint8_t upustValveRequestState : 2;
	uint8_t mainValveRequestState : 2;
	
	uint16_t LoRaFrequencyMHz;
	uint16_t launchDelay; //time between ignition and lift off
	uint16_t dataFramePeriod; //data frame creating period
	uint16_t upustValveTime;
	uint16_t mainValveTime;
};

struct RocketControl{
  StateMachineEvent stateEvent;
  StateMachine state;
	Options options;  //TODO set options value

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

  //notify that changing state event occure
	bool changeStateEvent(StateMachineEvent newEvent){
		//portENTER_CRITICAL(&stateLock);
    if((newEvent - 1) != stateEvent && newEvent != ABORT_EVENT){
      return false;
    }
    
    this->stateEvent = newEvent;
    //portEXIT_CRITICAL(&stateLock);
    //xTaskNotifyGive(this->stateTask);
    return true;
  }

  //Use only in stateTask
  void changeState(StateMachine newState){
    if((newState - 1) != state && newState != ABORT){
      return;
    }
    
    //portENTER_CRITICAL(&stateLock);
    this->state = newState;
    //portEXIT_CRITICAL(&stateLock);
  }
};

#endif