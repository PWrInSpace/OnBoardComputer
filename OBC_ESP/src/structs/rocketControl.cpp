#include "../include/structs/rocketControl.h"

void RocketControl::updateCurrentState(){
  dataFrame.mcb.state = StateMachine::getCurrentState();
}

void RocketControl::sendLog(char * message){
  static char log[SD_FRAME_ARRAY_SIZE] = {};
  char temp[40] = {};
  strcpy(log, "LOG ");
  snprintf(temp, 40, " [ %d , %lu ]\n", dataFrame.mcb.state, millis());
  strcat(log, message);
  strcat(log, temp);

  //Serial.println(log); //DEBUG
  xQueueSend(hardware.sdQueue, (void*)&log, 0);
}

uint32_t RocketControl::getDisconnectRemainingTime(){
  if(hardware.disconnectTimer == NULL){
    return disconnectDelay;
  }else if(xTimerIsTimerActive(hardware.disconnectTimer) == pdFALSE){
    return 0;
  }
  
  return (xTimerGetExpiryTime(hardware.disconnectTimer) - xTaskGetTickCount()) * portTICK_PERIOD_MS;
}

bool RocketControl::allDevicesWokenUp(){
  return (dataFrame.pitot.wakeUp && dataFrame.mainValve.wakeUp && dataFrame.upustValve.wakeUp && dataFrame.payLoad.wakeUp && dataFrame.blackBox.wakeUp);
}

void RocketControl::createOptionsFrame(char* data){
  size_t optionsSize;

  optionsSize = snprintf(NULL, 0, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d\n",
    options.LoRaFrequencyMHz, options.countdownTime, options.ignitionTime,
    options.tankMinPressure, options.flashWrite, options.forceLaunch,
    options.espnowSleepTime, options.espnowShortPeriod, options.espnowLongPeriod, 
    options.flashShortPeriod, options.flashLongPeriod, 
    options.sdShortPeriod, options.sdLongPeriod, options.idlePeriod, options.dataFramePeriod, 
    options.loraPeriod, options.flashDataCurrentPeriod, options.sdDataCurrentPeriod);


  char opt[optionsSize];

  snprintf(opt, optionsSize, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d\n",
    options.LoRaFrequencyMHz, options.countdownTime, options.ignitionTime,
    options.tankMinPressure, options.flashWrite, options.forceLaunch,
    options.espnowSleepTime, options.espnowShortPeriod, options.espnowLongPeriod, 
    options.flashShortPeriod, options.flashLongPeriod, 
    options.sdShortPeriod, options.sdLongPeriod, options.idlePeriod, options.dataFramePeriod, 
    options.loraPeriod, options.flashDataCurrentPeriod, options.sdDataCurrentPeriod);


  strcpy(data, LORA_TX_OPTIONS_PREFIX);

  
  strcat(data, opt);
  strcat(data, "\n");
}

void RocketControl::createLoRaFrame(char* data){
  size_t mcbSize, pitotSize, mvSizem, uvSize, tanwaSize, otherSize, recoverySize, errorsSize;

  mcbSize = snprintf(NULL, 0, "%d;%lu;%d;%d;%0.2f;%d;%0.4f;%0.4f;%0.2f;%d;%d;",
    dataFrame.mcb.state, millis(), missionTimer.getTime(), getDisconnectRemainingTime(),
    dataFrame.mcb.batteryVoltage, dataFrame.mcb.watchdogResets, dataFrame.mcb.GPSlal, 
    dataFrame.mcb.GPSlong, dataFrame.mcb.GPSalt, dataFrame.mcb.GPSsat, dataFrame.mcb.GPSsec); //11



  uint8_t byteData[4] = {};
  char mcbFrame[mcbSize] = {};
  //char pitotFrame[80] = {};
  //char mvFrame[60] = {};
  //char uvFrame[60] = {};
  //char tanwaFrame[60] = {};
  //char otherSlaves[30] = {};
  //char recoveryFrame[10] = {};
  //char errorsFrame[10] = {};

  strcpy(data, LORA_TX_DATA_PREFIX);

  //MCB
  snprintf(mcbFrame, mcbSize, "%d;%lu;%d;%d;%0.2f;%d;%0.4f;%0.4f;%0.2f;%d;%d;",
    dataFrame.mcb.state, millis(), missionTimer.getTime(), getDisconnectRemainingTime(),
    dataFrame.mcb.batteryVoltage, dataFrame.mcb.watchdogResets, dataFrame.mcb.GPSlal, 
    dataFrame.mcb.GPSlong, dataFrame.mcb.GPSalt, dataFrame.mcb.GPSsat, dataFrame.mcb.GPSsec); //11

  strcat(data, mcbFrame);
  strcat(data, "\n");

  /*
  //PITOT
  snprintf(pitotFrame, 80, "%d;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;",
    pitot.wakeUp, pitot.batteryVoltage, pitot.staticPressure, pitot.dynamicPressure,
    pitot.temperature, pitot.altitude, pitot.velocity, pitot.predictedApogee); //8
  
  strcat(data, pitotFrame);

  //MAIN VALVE
  snprintf(mvFrame, 60, "%d;%0.2f;%d;%0.2f;%0.2f;",
    mainValve.wakeUp, mainValve.batteryVoltage, mainValve.valveState,
    mainValve.thermocouple[0], mainValve.thermocouple[1]); //5
  
  strcat(data, mvFrame);


  //UPUST VALVE
  snprintf(uvFrame, 60, "%d;%0.2f;%d;%0.2f;%d;%d;%d;%d;%d;",
    upustValve.wakeUp, upustValve.batteryVoltage, upustValve.valveState,
    upustValve.tankPressure, upustValve.hall[0], upustValve.hall[1],
    upustValve.hall[2], upustValve.hall[3], upustValve.hall[4]); //9
  
  strcat(data, uvFrame);   

  //TANWA
  snprintf(tanwaFrame, 60, "%d;%0.2f;%d;%d;%d;%d;%d;%d;%0.2f;%0.2f",
    tanWa.tanWaState, tanWa.batteryVoltage, tanWa.igniterContinouity,
    tanWa.fillValveState, tanWa.deprValveState, tanWa.pullState,
    tanWa.rocketWeightRaw, tanWa.butlaWeightRaw, tanWa.rocketWeight,
    tanWa.butlaWeight);//10
  
  strcat(data, tanwaFrame);

  snprintf(otherSlaves, 30 , "%d;%0.2f;%d;%0.2f;",
    blackBox.wakeUp, blackBox.batteryVoltage, 
    payLoad.wakeUp, payLoad.batteryVoltage); //4

  strcat(data, otherSlaves);
  
  //recovery first byte
  memset(byteData, 0, 4);
  byteData[0] |= (recovery.isArmed << 6);
  byteData[0] |= (recovery.firstStageContinouity << 5);
  byteData[0] |= (recovery.secondStageContinouity << 4);
  byteData[0] |= (recovery.separationSwitch1 << 3);
  byteData[0] |= (recovery.separationSwitch2 << 2);
  byteData[0] |= (recovery.telemetrumFirstStage << 1);
  byteData[0] |= (recovery.telemetrumSecondStage << 0);
  
  //recovery second byte  
  byteData[1] |= (recovery.altimaxFirstStage << 5);
  byteData[1] |= (recovery.altimaxSecondStage << 4);
  byteData[1] |= (recovery.apogemixFirstStage << 3);
  byteData[1] |= (recovery.apogemixSecondStage << 2);
  byteData[1] |= (recovery.firstStageDone << 1);
  byteData[1] |= (recovery.secondStageDone << 0);
  
  //int cast for //DEBUG
  snprintf(recoveryFrame, 10, "%d;%d;", byteData[0], byteData[1]);
  strcat(data, recoveryFrame); //2

  
  //error first byte  
  memset(byteData, 0, 4);
  byteData[0] |= (errors.sd << 6);
  byteData[0] |= (errors.flash << 4);
  byteData[0] |= (errors.watchdog << 1);
  byteData[0] |= (errors.rtos << 0);

  //error second byte  
  byteData[1] |= (errors.exceptions << 3);
  byteData[1] |= (errors.espnow << 0);
  
  //int cast for //DEBUG
  snprintf(errorsFrame, 10, "%d;%d;0", byteData[0], byteData[1]);
  strcat(data, errorsFrame); //2
  

  strcat(data, "\n");
  /*
  Serial.print("MCB SIZE: "); //DEBUG
  Serial.print(strlen(mcbFrame));
  Serial.print("\tPITOT SIZE: "); //DEBUG
  Serial.print(strlen(pitotFrame));
  Serial.print("\tMAIN VALVE SIZE: "); //DEBUG
  Serial.print(strlen(mvFrame));
  Serial.print("\tPupst valve SIZE: "); //DEBUG
  Serial.print(strlen(uvFrame));
  Serial.print("\tTanWa SIZE: "); //DEBUG
  Serial.print(strlen(tanwaFrame));
  Serial.print("\tRecovery SIZE: "); //DEBUG
  Serial.print(strlen(recoveryFrame));
  Serial.print("\tErrors SIZE: "); //DEBUG
  Serial.print(strlen(errorsFrame));
  Serial.print("\tTOTAL SIZE: "); //DEBUG
  Serial.println(strlen(data));
  */
}

/**********************************************************************************************/

void RocketControl::createSDFrame(char* data){
  /*
  char mcbFrame[100] = {};
  char pitotFrame[60] = {};
  char mvFrame[40] = {};
  char uvFrame[40] = {};
  char tanwaFrame[60] = {};
  char recoveryFrame[30] = {};
  char optionsFrame[120] = {};
  char errorsFrame[20] = {};
  
  //MCB
  snprintf(mcbFrame, 100, "%d;%lu;%d;%d;%0.2f;%0.4f;%0.4f;%0.2f;%d;%d;%d;%0.2f;%0.2f;%0.2f;%d;",
    state, millis(), missionTimer.getTime(), disconnectTime, batteryVoltage,
    GPSlal, GPSlong, GPSalt, GPSsat, GPSsec, temp,
    pressure, altitude, velocity, ignition);

  strcpy(data, mcbFrame);
  /*
  //PITOT
  snprintf(pitotFrame, 60, "%d;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;",
    pitot.wakeUp, pitot.batteryVoltage, pitot.staticPressure, pitot.dynamicPressure,
    pitot.temperature, pitot.altitude, pitot.velocity, pitot.predictedApogee);
  
  strcat(data, pitotFrame);

  //MAIN VALV
  snprintf(mvFrame, 40, "%d;%0.2f;%d;%0.2f;%0.2f;",
    mainValve.wakeUp, mainValve.batteryVoltage, mainValve.valveState,
    mainValve.thermocouple[0], mainValve.thermocouple[1]);
  strcat(data, mvFrame);

  //UPUST VALVE
  snprintf(uvFrame, 40, "%d;%0.2f;%d;%0.2f;%d;%d;%d;%d;%d;",
    upustValve.wakeUp, upustValve.batteryVoltage, upustValve.valveState,
    upustValve.tankPressure, upustValve.hall[0], upustValve.hall[1],
    upustValve.hall[2], upustValve.hall[3], upustValve.hall[4]);
  
  strcat(data, uvFrame);   

  //TANWA
  snprintf(tanwaFrame, 60, "%d;%0.2f;%d;%d;%d;%d;%d;%d;%0.2f;%0.2f;",
    tanWa.tanWaState, tanWa.batteryVoltage, tanWa.igniterContinouity,
    tanWa.fillValveState, tanWa.deprValveState, tanWa.pullState,
    tanWa.rocketWeightRaw, tanWa.butlaWeightRaw, tanWa.rocketWeight,
    tanWa.butlaWeight);
  
  strcat(data, tanwaFrame);
  
  //RECOVERY
  snprintf(recoveryFrame, 30, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;",
    recovery.isArmed, recovery.firstStageContinouity, recovery.secondStageContinouity,
    recovery.separationSwitch1, recovery.separationSwitch2,
    recovery.telemetrumFirstStage, recovery.telemetrumSecondStage,
    recovery.altimaxFirstStage, recovery.altimaxSecondStage,
    recovery.apogemixFirstStage, recovery.apogemixSecondStage,
    recovery.firstStageDone, recovery.secondStageDone);
  
  strcat(data, recoveryFrame);
  
  //OPTIONS
  snprintf(optionsFrame, 120, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d:%d;%d;",
    options.LoRaFrequencyMHz, options.countdownTime, options.ignitionTime, options.tankMinPressure,
    options.flashWrite, options.forceLaunch, options.espnowSleepTime, options.espnowLongPeriod,
    options.espnowShortPeriod, options.flashShortPeriod, 
    options.flashLongPeriod, options.sdShortPeriod, options.sdLongPeriod, options.idlePeriod, 
    options.dataFramePeriod, options.loraPeriod, options.flashDataCurrentPeriod, 
    options.sdDataCurrentPeriod);
  
  strcat(data, optionsFrame);

  //ERRORS
  snprintf(errorsFrame, 20, "%d;%d;%d;%d;%d;%d;%d",
    errors.sd, errors.flash, errors.rtos, errors.espnow,
    errors.watchdog, errors.sensors, errors.exceptions);
  
  strcat(data, errorsFrame);
  strcat(data, "\n");
  */
  /*
  Serial.print("MCB SIZE: "); //DEBUG
  Serial.print(strlen(mcbFrame));
  Serial.print("\tPITOT SIZE: "); //DEBUG
  Serial.print(strlen(pitotFrame));
  Serial.print("\tMAIN VALVE SIZE: "); //DEBUG
  Serial.print(strlen(mvFrame));
  Serial.print("\tPupst valve SIZE: "); //DEBUG
  Serial.print(strlen(uvFrame));
  Serial.print("\tTanWa SIZE: "); //DEBUG
  Serial.print(strlen(tanwaFrame));
  Serial.print("\tRecovery SIZE: "); //DEBUG
  Serial.print(strlen(recoveryFrame));
  Serial.print("\tOptions SIZE: "); //DEBUG
  Serial.print(strlen(optionsFrame));
  Serial.print("\tErrors SIZE: "); //DEBUG
  Serial.print(strlen(errorsFrame));
  Serial.print("\tTOTAL SIZE: "); //DEBUG
  Serial.println(strlen(data));
  */
}