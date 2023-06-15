#ifndef GLOBAL_STRUCT_HH
#define GLOBAL_STRUCT_HH

#include <SPI.h>

struct HardwareManagment{
  /**RTOS**/
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
  QueueHandle_t espNowQueue; //best solution 
	//mutexes
	SemaphoreHandle_t spiMutex;
	SemaphoreHandle_t i2c1Mutex;

	//spinlock
	portMUX_TYPE stateLock = portMUX_INITIALIZER_UNLOCKED;

	//software timers
	TimerHandle_t watchdogTimer;
	TimerHandle_t disconnectTimer;
  TimerHandle_t espNowConnectionTimer;

  /** SERIAL COMMUNICATION **/
  SPIClass mySPI;
  TwoWire i2c1;
  TwoWire i2c2;

  HardwareManagment():
    loraTask(NULL),
    rxHandlingTask(NULL),
    stateTask(NULL),
    dataTask(NULL),
    sdTask(NULL),
    flashTask(NULL),
    loraRxQueue(NULL),
    loraTxQueue(NULL),
    sdQueue(NULL),
    flashQueue(NULL),
    espNowQueue(NULL),
    spiMutex(NULL),
    watchdogTimer(NULL),
    disconnectTimer(NULL),
    mySPI(SPIClass(VSPI)),
    i2c1(TwoWire(0)),
    i2c2(TwoWire(1))
    {}
};



#endif