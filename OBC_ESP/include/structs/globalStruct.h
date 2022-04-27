#ifndef GLOBAL_STRUCT_HH
#define GLOBAL_STRUCT_HH

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
  QueueHandle_t espNowQueue; //best solution XD
	//mutexes
	SemaphoreHandle_t spiMutex;
	SemaphoreHandle_t i2c1Mutex;

	//spinlock
	portMUX_TYPE stateLock = portMUX_INITIALIZER_UNLOCKED;

	//software timers
	TimerHandle_t watchdogTimer;
	TimerHandle_t disconnectTimer;

  /** SERIAL COMMUNICATION **/
  SPIClass mySPI = SPIClass(VSPI);
  TwoWire i2c1 = TwoWire(0);
  TwoWire i2c2 = TwoWire(1);
}

struct RocketControl{
  Options options;
  Timer missionTimer;
  bool allDevicesConnected;
  bool allDevicesWokenUp;

  RecoverySTM recoveryStm = RecoverySTM(i2c1, RECOVERY_ADDRES);

  void sendLog(char *message);
}

#endif