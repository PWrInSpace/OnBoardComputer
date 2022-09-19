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
  PRESSURE_SENSOR_INIT_ERROR,
  TEMP_SENSOR_INIT_ERROR
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
  RECOVERY_NO_ERROR = 0,
  RECOVERY_I2C_INIT_ERROR,
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
};

bool ERR_init(void);
void ERR_set_sd_error(SDError error);
void ERR_set_flash_error(FlashError error);
void ERR_set_rtos_error(RTOSError error);
void ERR_set_esp_now_error(EspNowError error);
void ERR_set_watchdog_error(WatchDogError error);
void ERR_set_sensors_error(SensorsError error);
void ERR_set_last_exception(LastException error); 
void ERR_set_recovery_error(RecoveryError error); 
void ERR_reset(uint8_t triger);
bool ERR_create_lora_frame(char* buffer, size_t size);
bool ERR_create_sd_frame(char* buffer, size_t size);
Errors ERR_get_struct(void);
// Next errors struct
// union {
//   struct {
//     uint8_t b0:1;
//     uint8_t b1:1;
//     uint8_t b2:1;
//     uint8_t b3:1;
//     uint8_t b4:1;
//     uint8_t b5:1;
//     uint8_t b6:1;
//     uint8_t b7:1;
//   } bits;

//   uint8_t byte;
// }error_t;


#endif