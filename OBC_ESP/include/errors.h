#ifndef ERRORS_HH
#define ERRORS_HH

//ERROR RESET TRIGERS
#define ERROR_RESET_LORA 0x21
#define ERROR_RESET_SD 0x22

//loop error
enum SDError{
  SD_NO_ERROR = 0,
  SD_INIT_ERROR,
  SD_WRITE_ERROR,
};

//loop error
enum FlashError{
  FLASH_NO_ERROR = 0,
  FLASH_WRITE_ERROR,
  FLASH_NO_SPACE_ERROR,
};

//loop error
enum RTOSError{
  RTOS_NO_ERROR = 0,
  RTOS_QUEUE_ADD_ERROR,
  RTOS_SPI_MUTEX_ERROR,
};

//idk 
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
};

enum LastException{
  NO_EXCEPTION = 0,
  WAKE_UP_EXCEPTION,
  MISSION_TIMER_EXCEPTION,
  INVALID_PREFIX_EXCEPTION, //FRAME
  DEVICE_NOT_ARMED_EXCEPTION,
  INVALID_STATE_CHANGE_EXCEPTION,
};

struct Errors{
  SDError sd;
  FlashError flash;
  RTOSError rtos;
  EspNowError espnow;
  WatchDogError watchdog;
  SensorsError sensors;
  LastException exceptions;
  bool newError;


  void setSDError(SDError error){
    if(sd != error){
      newError = true;
      sd = error;
    }
  }
  void reset(uint8_t triger){
    static bool loraTriger = false;
    static bool sdTriger = false; 

    //new error reset previous trigers, to provide save error to sd and lora before reset error
    if(newError){ 
      loraTriger = false;
      sdTriger = false;
    }

    //error reset double triger
    if(triger == ERROR_RESET_LORA){
      loraTriger = true;
    }else if(triger == ERROR_RESET_SD){
      sdTriger = true;
    }

    if(loraTriger && sdTriger){
      sd = SD_NO_ERROR;
      flash = FLASH_NO_ERROR;
      rtos = RTOS_NO_ERROR;
      espnow = ESPNOW_NO_ERROR;
      watchdog = WATCHDOG_NO_ERROR;
      sensors = SENSORS_NO_ERROR;

      loraTriger = false;
      sdTriger = false;
      Serial.print("ERRORS reset: "); Serial.println((int)triger);
    }
  }
};


#endif