#ifndef ERRORS_HH
#define ERRORS_HH

//ERROR RESET TRIGERS
#define ERROR_RESET_LORA 0x21
#define ERROR_RESET_SD 0x22

#include <stdint.h>
#include <Arduino.h> //DEBUG
//loop error
enum SDError{ //GIT
  SD_NO_ERROR = 0,
  SD_INIT_ERROR,
  SD_WRITE_ERROR,
};

//loop error
enum FlashError{ //+-
  FLASH_NO_ERROR = 0,
  FLASH_WRITE_ERROR,
  FLASH_NO_SPACE_ERROR,
};

//loop error
enum RTOSError{
  RTOS_NO_ERROR = 0,
  RTOS_SD_QUEUE_ADD_ERROR,
  RTOS_LORA_QUEUE_ADD_ERROR,
  RTOS_FLASH_QUEUE_ADD_ERROR,
};

enum EspNowError{
  ESPNOW_NO_ERROR = 0,
  ESPNOW_INIT_ERROR,
  ESPNOW_ADD_PEER_ERROR,
  ESPNOW_SEND_ERROR,
  ESPNOW_DELIVERY_ERROR,
};

//loop erorr
enum WatchDogError{
  WATCHDOG_NO_ERROR = 0,
  WATCHDOG_LORA_ERROR,
  WATCHDOG_RX_HANDLING_ERROR,
  WATCHDOG_SD_ERROR,
  WATCHDOG_FLASH_ERROR,
};

//loop error
enum SensorsError{
  SENSORS_NO_ERROR = 0,
  IMU_INIT_ERROR,
  GPS_INIT_ERROR,
};

enum LastException{
  NO_EXCEPTION = 0,
  WAKE_UP_EXCEPTION,
  MISSION_TIMER_EXCEPTION,
  INVALID_PREFIX_EXCEPTION, //FRAME
  DEVICE_NOT_ARMED_EXCEPTION,
  INVALID_STATE_CHANGE_EXCEPTION,
  INVALID_OPTION_NUMBER,
  INVALID_OPTION_VALUE,
};

enum RecoveryError{
  COM_NO_ERROR = 0,
  I2C_INIT_ERROR,
};

struct Errors{
  SDError sd;
  FlashError flash;
  RTOSError rtos;
  EspNowError espnow;
  WatchDogError watchdog;
  SensorsError sensors;
  LastException exceptions;
  RecoveryError recovery;
  bool newError;

  Errors() = default;
  void setSDError(SDError error);
  void setFlashError(FlashError error);
  void setRTOSError(RTOSError error);
  void setEspNowError(EspNowError error);
  void setWatchDogError(WatchDogError error);
  void setSensorError(SensorsError error);
  void setLastException(LastException error); 
  void setRecoveryError(RecoveryError error); 
  void reset(uint8_t triger);
};


#endif