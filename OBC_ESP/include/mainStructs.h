#ifndef ROCKET_MAIN_STRUCTS
#define ROCKET_MAIN_STRUCTS

#include "FreeRTOS.h"

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
	StateMachine state;
	Options options;

	//tasks
	TaskHandle_t LoRaTask;
	TaskHandle_t rxHandlingTask;
	TaskHandle_t stateTask;
	TaskHandle_t dataTask;
	TaskHandle_t sdTask;
	TaskHandle_t flashTask;
	//queues
	QueueHandle_t LoRaRxQueue;
	QueueHandle_t LoRaTxQueue;
	QueueHandle_t sdQueue;
	QueueHandle_t flashQueue;
	//mutex
	//SemaphoreHandle_t spiMutex = NULL;
	//SemaphoreHandle_t i2cMutex_1 = NULL;
	//SemaphoreHandle_t i2cMutex_2 = NULL;

	//spinlock
	//portMUX_TYPE stateLock = portMUX_INITIALIZER_UNLOCKED;

	//software timers
	TimerHandle_t watchdogTimer;
	TimerHandle_t disconnectTimer;

	//TODO
	//constructor 
	//RocketControl();

	void changeState(StateMachine newState){
		//portENTER_CRITICAL(&stateLock);
    this->state = newState;
    //portEXIT_CRITICAL(&stateLock);
    //xTaskNotifyGive(this->stateTask);
	}
};


//Tasks

//Timers
void watchdogTimerCallback(TimerHandle_t xTimer);

void disconnectTimerCallback(TimerHandle_t xTimer);

#endif