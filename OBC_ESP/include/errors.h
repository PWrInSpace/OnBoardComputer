#ifndef ERRORS_HH
#define ERRORS_HH

enum SDError{
  SD_NO_ERROR = 0,
  SD_INIT_ERROR,
  SD_WRITE_ERROR,
};

enum FlashError{
  FLASH_NO_ERROR = 0,
  FLASH_WRITE_ERROR,
  FLASH_NO_SPACE_ERROR,
};

enum RTOSError{
  RTOS_NO_ERROR = 0,
  RTOS_QUEUE_ADD_ERROR,
};

enum EspNowError{
  ESPNOW_NO_ERROR = 0,
  ESPNOW_INIT_ERROR,
  ESPNOW_ADD_PEER_ERROR,
  ESPNOW_SEND_ERROR,
  ESPNOW_DELIVERY_ERROR,
};

enum WatchDogError{
  WATCHDOG_NO_ERROR = 0,
  WATCHDOG_LORA_ERROR,
  WATCHDOG_RX_HANDLING_ERROR,
  WATCHDOG_SD_ERROR,
  WATCHDOG_FLASH_ERROR,
};

enum SensorsError{
  SENSORS_NO_ERROR = 0,
  IMU_INIT_ERROR,
};

enum LastException{
  NO_EXCEPTION = 0,
  WAKE_UP_EXCEPTION,
  INVALID_PREFIX, //FRAME
}

struct Errors{
  SDError sd;
  FlashError flash;
  RTOSError rtos;
  EspNowError espnow;
  WatchDogError watchdog;
  SensorsError sensors;

  void reset(){
    sd = SD_NO_ERROR;
    flash = FLASH_NO_ERROR;
    rtos = RTOS_NO_ERROR;
    espnow = ESPNOW_NO_ERROR;
    watchdog = WATCHDOG_NO_ERROR;
    sensors = SENSORS_NO_ERROR;
  }
};


#endif