#include "errors.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <freertos/semphr.h>
#include <Arduino.h>

static struct {
  Errors errors; 
  SemaphoreHandle_t errors_mutex;
}se = {
  .errors ={
    .sd = SD_NO_ERROR,
    .flash = FLASH_NO_ERROR,
    .rtos = RTOS_NO_ERROR,
    .espnow = ESPNOW_NO_ERROR,
    .watchdog = WATCHDOG_NO_ERROR,
    .sensors = SENSORS_NO_ERROR,
    .exceptions = NO_EXCEPTION,
    .recovery = RECOVERY_NO_ERROR,
  },
  .errors_mutex = NULL,
};

void ERR_fill_pysd_struct(pysdmain_DataFrame *data) {
  xSemaphoreTake(se.errors_mutex, portMAX_DELAY);
  data->errors.sd = se.errors.sd;
  data->errors.flash = se.errors.flash;
  data->errors.rtos = se.errors.rtos;
  data->errors.espnow = se.errors.espnow;
  data->errors.watchdog = se.errors.watchdog;
  data->errors.sensors = se.errors.sensors;
  data->errors.exceptions = se.errors.exceptions;
  data->errors.recovery = se.errors.recovery;
  xSemaphoreGive(se.errors_mutex);
}

bool ERR_init(void) {
  memset(&se.errors, 0, sizeof(se.errors));
  se.errors_mutex = xSemaphoreCreateMutex();

  assert(se.errors_mutex != NULL);
  if (se.errors_mutex == NULL) {
    return false;
  }

  return true;
}

void ERR_set_sd_error(SDError error){
  assert(se.errors_mutex != NULL);
  xSemaphoreTake(se.errors_mutex, portMAX_DELAY);
  if(se.errors.sd != error){
      se.errors.newError = true;
      se.errors.sd = error;
  }
  xSemaphoreGive(se.errors_mutex);
}

void ERR_set_flash_error(FlashError error){
  assert(se.errors_mutex != NULL);
  xSemaphoreTake(se.errors_mutex, portMAX_DELAY);
  if(se.errors.flash != error){
    se.errors.newError = true;
    se.errors.flash = error;
  }
  xSemaphoreGive(se.errors_mutex);
}

void ERR_set_rtos_error(RTOSError error){
  assert(se.errors_mutex != NULL);
  xSemaphoreTake(se.errors_mutex, portMAX_DELAY);
  if(se.errors.rtos != error){
    se.errors.newError = true;
    se.errors.rtos = error;
  }
  xSemaphoreGive(se.errors_mutex);
}

void ERR_set_esp_now_error(EspNowError error){
  assert(se.errors_mutex != NULL);
  xSemaphoreTake(se.errors_mutex, portMAX_DELAY);
  if(se.errors.espnow != error){
    se.errors.newError = true;
    se.errors.espnow = error;
  }
  xSemaphoreGive(se.errors_mutex);

}

void ERR_set_watchdog_error(WatchDogError error){
  assert(se.errors_mutex != NULL);
  xSemaphoreTake(se.errors_mutex, portMAX_DELAY);
  if(se.errors.watchdog != error){
    se.errors.newError = true;
    se.errors.watchdog = error;
  }
  xSemaphoreGive(se.errors_mutex);
}

void ERR_set_sensors_error(SensorsError error){
  assert(se.errors_mutex != NULL);
  xSemaphoreTake(se.errors_mutex, portMAX_DELAY);
  if(se.errors.sensors != error){
    se.errors.newError = true;
    se.errors.sensors = error;
  }
  xSemaphoreGive(se.errors_mutex);
}

void ERR_set_last_exception(LastException error){
  assert(se.errors_mutex != NULL);
  xSemaphoreTake(se.errors_mutex, portMAX_DELAY);
  if(se.errors.exceptions != error){
    se.errors.newError = true;
    se.errors.exceptions = error;
  }
  xSemaphoreGive(se.errors_mutex);
}

void ERR_set_recovery_error(RecoveryError error){
  assert(se.errors_mutex != NULL);
  xSemaphoreTake(se.errors_mutex, portMAX_DELAY);
  if(se.errors.recovery != error){
    se.errors.newError = true;
    se.errors.recovery = error;
  }
  xSemaphoreGive(se.errors_mutex);
} 

void ERR_reset(uint8_t triger){
  static bool loraTriger = false;
  static bool sdTriger = false; 

  //new error reset previous trigers, to provide save error to sd and lora before reset error
  if(se.errors.newError){ 
    loraTriger = false;
    sdTriger = false;
    se.errors.newError = false; //reset new error flag
  }

  //error reset double triger
  if(triger == ERROR_RESET_LORA){
    loraTriger = true;
  }else if(triger == ERROR_RESET_SD){
    sdTriger = true;
  }

  if(loraTriger && sdTriger){
    se.errors.sd = SD_NO_ERROR;
    se.errors.flash = FLASH_NO_ERROR;
    se.errors.rtos = RTOS_NO_ERROR;
    //prevent add peer error reset
    se.errors.espnow = (EspNowError)((int)se.errors.espnow & (int)ESPNOW_ADD_PEER_ERROR);
    se.errors.watchdog = WATCHDOG_NO_ERROR;
    se.errors.sensors = SENSORS_NO_ERROR;
    loraTriger = false;
    sdTriger = false;
  } 
}

bool ERR_create_lora_frame(char *buffer, size_t size) {
  assert(buffer != NULL);
  assert(size > 0);
  if (buffer == NULL || size == 0) {
    return false;
  }

  Errors err;
  xSemaphoreTake(se.errors_mutex, portMAX_DELAY);
  err = se.errors;
  xSemaphoreGive(se.errors_mutex);

  uint8_t byte_data[4] = {0};
  memset(byte_data, 0, sizeof(byte_data));
  byte_data[0] |= (se.errors.sd << 5);
  byte_data[0] |= (se.errors.flash << 3);
  byte_data[0] |= (se.errors.watchdog);
  //error second byte
  byte_data[1] |= (se.errors.recovery << 6);
  byte_data[1] |= (se.errors.exceptions << 3);
  byte_data[1] |= (se.errors.espnow << 0);
  //error third byte
  byte_data[2] |= (se.errors.sensors << 2);
  byte_data[2] |= (se.errors.rtos << 0);

  size_t wrote_data_size;
  wrote_data_size = snprintf(buffer, size, "%d;%d;%d;", byte_data[0], byte_data[1], byte_data[2]) + 1;
  assert(wrote_data_size < size);
  return true;
}

bool ERR_create_sd_frame(char *buffer, size_t size) {
  assert(buffer != NULL);
  assert(size > 0);
  if (buffer == NULL || size == 0) {
    return false;
  }
  
  Errors err;
  xSemaphoreTake(se.errors_mutex, portMAX_DELAY);
  err = se.errors;
  xSemaphoreGive(se.errors_mutex);

  size_t wrote_data_size;
  wrote_data_size = snprintf(buffer, size, "%d;%d;%d;%d;%d;%d;%d;%d;",
    se.errors.sd, se.errors.flash, se.errors.rtos, se.errors.espnow,
    se.errors.watchdog, se.errors.sensors, se.errors.exceptions, se.errors.recovery) + 1;
  assert(wrote_data_size < size);
  return true;
}

Errors ERR_get_struct(void){
  Errors err;
  xSemaphoreTake(se.errors_mutex, portMAX_DELAY);
  err = se.errors;
  xSemaphoreGive(se.errors_mutex);
  return err;
}
