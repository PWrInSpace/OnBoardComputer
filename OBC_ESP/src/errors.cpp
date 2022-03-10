#include "errors.h"

void Errors::setSDError(SDError error){
  if(sd != error){
      newError = true;
      sd = error;
  }
}

void Errors::setFlashError(FlashError error){
  if(flash != error){
    newError = true;
    flash = error;
  }
}

void Errors::setRTOSError(RTOSError error){
  if(rtos != error){
    newError = true;
    rtos = error;
  }
}

void Errors::setEspNowError(EspNowError error){
  if(espnow != error){
    newError = true;
    espnow = error;
  }
}

void Errors::setWatchDogError(WatchDogError error){
  if(watchdog != error){
    newError = true;
    watchdog = error;
  }
}

void Errors::setSensorError(SensorsError error){
  if(sensors != error){
    newError = true;
    sensors = error;
  }
}

void Errors::setLastException(LastException error){
  if(exceptions != error){
    newError = true;
    exceptions = error;
  }
}

void Errors::reset(uint8_t triger){
  static bool loraTriger = false;
  static bool sdTriger = false; 

  //new error reset previous trigers, to provide save error to sd and lora before reset error
  if(newError){ 
    loraTriger = false;
    sdTriger = false;
    newError = false; //reset new error flag
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
  } 
}