#include "../include/structs/rocketControl.h"

/**
 * @brief update state 
 * 
 */
void RocketControl::updateCurrentState(){
  dataFrame.mcb.state = StateMachine::getCurrentState();
}

/**
 * @brief create log and add to sd queue
 * 
 * @param message log message
 */
void RocketControl::sendLog(const char * message){
  static char log[SD_FRAME_ARRAY_SIZE] = {};
  char temp[40] = {};
  strcpy(log, "LOG ");
  snprintf(temp, 40, " [ %d , %lu ]\n", dataFrame.mcb.state, millis());
  strcat(log, message);
  strcat(log, temp);

  //Serial.println(log); //DEBUG
  xQueueSend(hardware.sdQueue, (void*)&log, 0);
}

/**
 * @brief disconnect timer 
 * 
 * @return uint32_t remaining time
 */
uint32_t RocketControl::getDisconnectRemainingTime(){
  if(xTimerIsTimerActive(hardware.disconnectTimer) == pdFALSE){
    return disconnectDelay;
  }
  
  return (xTimerGetExpiryTime(hardware.disconnectTimer) - xTaskGetTickCount()) * portTICK_PERIOD_MS;
}

/**
 * @brief restar disconnect timer or active timer
 * 
 */
void RocketControl::restartDisconnectTimer(bool _force){
  //if timer is active or is in hold state -> restart timer
  if(xTimerIsTimerActive(hardware.disconnectTimer) == pdTRUE || _force){
    xTimerReset(hardware.disconnectTimer, 0);
  }
}

bool RocketControl::deactiveDisconnectTimer(){
  return xTimerStop(hardware.disconnectTimer, 0) == pdTRUE ? true : false; 
}


/**
 * @brief add data to rtosQueue, if queue is full, first element is taken and new element is added to the end
 * 
 * @param _handle handler to queue
 * @param data data
 * @return true queue isn't full
 * @return false queue is full
 */
bool RocketControl::queueSend(xQueueHandle _handle, char *data){
  char rubbishData[LORA_FRAME_ARRAY_SIZE];
  if(xQueueSend(_handle, (void*)&data, 0) != pdTRUE){
    xQueueReceive(_handle, (void*)&rubbishData, 0);
    xQueueSend(_handle, (void*)&data, 0);
    return false;
  }

  return true;
}

/**
 * @brief check status of slaves
 * 
 * @return true slaves are working
 * @return false slaves are sleeping zzzz
 */
bool RocketControl::allDevicesWokenUp(){
  return (dataFrame.pitot.wakeUp && dataFrame.mainValve.wakeUp && dataFrame.upustValve.wakeUp);
}

/**
 * @brief create options frame for LoRa communication
 * 
 * @param data data
 */
void RocketControl::createOptionsFrame(char* data){
  size_t optionsSize;

  optionsSize = snprintf(NULL, 0, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;",
    options.LoRaFrequencyMHz, options.countdownTime, options.ignitionTime,
    options.tankMinPressure, options.flashWrite, options.forceLaunch, options.dataCurrentPeriod,
    options.loraCurrentPeriod, options.flashDataCurrentPeriod, options.sdDataCurrentPeriod) + 1;


  char opt[optionsSize];

  snprintf(opt, optionsSize, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;",
    options.LoRaFrequencyMHz, options.countdownTime, options.ignitionTime,
    options.tankMinPressure, options.flashWrite, options.forceLaunch, options.dataCurrentPeriod,
    options.loraCurrentPeriod, options.flashDataCurrentPeriod, options.sdDataCurrentPeriod);


  strcpy(data, LORA_TX_OPTIONS_PREFIX);

  
  strcat(data, opt);
  strcat(data, "\n");
}

/**
 * @brief create data frame for LoRa communication
 * 
 * @param data 
 */
void RocketControl::createLoRaFrame(char* data){
  uint8_t byteData[4] = {};
  size_t mcbSize, pitotSize, mvSize, uvSize, tanwaSize, bbSize, plSize, recoverySize = 10, errorsSize = 10;

  mcbSize = snprintf(NULL, 0, "%d;%lu;%d;%d;%0.2f;%d;%0.4f;%0.4f;%0.2f;%d;%d;",
    dataFrame.mcb.state, millis(), missionTimer.getTime(), getDisconnectRemainingTime(),
    dataFrame.mcb.batteryVoltage, dataFrame.mcb.watchdogResets, dataFrame.mcb.GPSlal, 
    dataFrame.mcb.GPSlong, dataFrame.mcb.GPSalt, dataFrame.mcb.GPSsat, dataFrame.mcb.GPSsec) + 1; //11

  pitotSize = snprintf(NULL, 0, "%d;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;",
    dataFrame.pitot.wakeUp, dataFrame.pitot.batteryVoltage, dataFrame.pitot.staticPressure, 
    dataFrame.pitot.dynamicPressure, dataFrame.pitot.temperature, dataFrame.pitot.altitude,
    dataFrame.pitot.velocity, dataFrame.pitot.predictedApogee) + 1; //8
  
  mvSize = snprintf(NULL, 0, "%d;%0.2f;%d;%0.2f;%0.2f;",
    dataFrame.mainValve.wakeUp, dataFrame.mainValve.batteryVoltage, dataFrame.mainValve.valveState,
    dataFrame.mainValve.thermocouple[0], dataFrame.mainValve.thermocouple[1]) + 1; //5

  uvSize = snprintf(NULL, 0, "%d;%0.2f;%d;%0.2f;%d;%d;%d;%d;%d;",
    dataFrame.upustValve.wakeUp, dataFrame.upustValve.batteryVoltage, dataFrame.upustValve.valveState,
    dataFrame.upustValve.tankPressure, dataFrame.upustValve.hall[0], dataFrame.upustValve.hall[1],
    dataFrame.upustValve.hall[2], dataFrame.upustValve.hall[3], dataFrame.upustValve.hall[4]) + 1; //9
  
  tanwaSize = snprintf(NULL, 0, "%d;%0.2f;%d;%d;%d;%d;%d;%d;%d;%0.2f;%0.2f;%d;%d;%f;%f;%f;%d;%d;%d;",
    dataFrame.tanWa.tanWaState, dataFrame.tanWa.vbat, dataFrame.tanWa.igniterContinouity[0],
    dataFrame.tanWa.igniterContinouity[1], dataFrame.tanWa.motorState[0], dataFrame.tanWa.motorState[1],
    dataFrame.tanWa.motorState[2], dataFrame.tanWa.motorState[3], dataFrame.tanWa.motorState[4],
    dataFrame.tanWa.rocketWeight, dataFrame.tanWa.butlaWeight, dataFrame.tanWa.rocketWeightRaw, 
    dataFrame.tanWa.butlaWeightRaw, dataFrame.tanWa.thermocouple[0], dataFrame.tanWa.thermocouple[1], 
    dataFrame.tanWa.thermocouple[2], dataFrame.tanWa.armButton, 
    dataFrame.tanWa.abortButton, dataFrame.tanWa.tankHeating) + 1; //19

  //recoverySize = snprintf(NULL, 0, "%d;%d;", byteData[0], byteData[1]) + 1;

  //errorsSize = snprintf(NULL, 0, "%d;%d", byteData[0], byteData[1]) + 1;

  char mcbFrame[mcbSize] = {};
  char pitotFrame[pitotSize] = {};
  char mvFrame[mvSize] = {};
  char uvFrame[uvSize] = {};
  char tanwaFrame[tanwaSize] = {};
  //char otherSlaves[30] = {};
  char recoveryFrame[recoverySize] = {};
  char errorsFrame[errorsSize] = {};


  //MCB
  snprintf(mcbFrame, mcbSize, "%d;%lu;%d;%d;%0.2f;%d;%0.4f;%0.4f;%0.2f;%d;%d;",
    dataFrame.mcb.state, millis(), missionTimer.getTime(), getDisconnectRemainingTime(),
    dataFrame.mcb.batteryVoltage, dataFrame.mcb.watchdogResets, dataFrame.mcb.GPSlal, 
    dataFrame.mcb.GPSlong, dataFrame.mcb.GPSalt, dataFrame.mcb.GPSsat, dataFrame.mcb.GPSsec); //11

  snprintf(pitotFrame, pitotSize, "%d;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;",
    dataFrame.pitot.wakeUp, dataFrame.pitot.batteryVoltage, dataFrame.pitot.staticPressure, 
    dataFrame.pitot.dynamicPressure, dataFrame.pitot.temperature, dataFrame.pitot.altitude,
    dataFrame.pitot.velocity, dataFrame.pitot.predictedApogee); //8

  snprintf(mvFrame, mvSize, "%d;%0.2f;%d;%0.2f;%0.2f;",
    dataFrame.mainValve.wakeUp, dataFrame.mainValve.batteryVoltage, dataFrame.mainValve.valveState,
    dataFrame.mainValve.thermocouple[0], dataFrame.mainValve.thermocouple[1]); //5

  snprintf(uvFrame, uvSize, "%d;%0.2f;%d;%0.2f;%d;%d;%d;%d;%d;",
    dataFrame.upustValve.wakeUp, dataFrame.upustValve.batteryVoltage, dataFrame.upustValve.valveState,
    dataFrame.upustValve.tankPressure, dataFrame.upustValve.hall[0], dataFrame.upustValve.hall[1],
    dataFrame.upustValve.hall[2], dataFrame.upustValve.hall[3], dataFrame.upustValve.hall[4]); //9
  
  snprintf(tanwaFrame, tanwaSize, "%d;%0.2f;%d;%d;%d;%d;%d;%d;%d;%0.2f;%0.2f;%d;%d;%f;%f;%f;%d;%d;%d;",
    dataFrame.tanWa.tanWaState, dataFrame.tanWa.vbat, dataFrame.tanWa.igniterContinouity[0],
    dataFrame.tanWa.igniterContinouity[1], dataFrame.tanWa.motorState[0], dataFrame.tanWa.motorState[1],
    dataFrame.tanWa.motorState[2], dataFrame.tanWa.motorState[3], dataFrame.tanWa.motorState[4],
    dataFrame.tanWa.rocketWeight, dataFrame.tanWa.butlaWeight, dataFrame.tanWa.rocketWeightRaw, 
    dataFrame.tanWa.butlaWeightRaw, dataFrame.tanWa.thermocouple[0], dataFrame.tanWa.thermocouple[1], 
    dataFrame.tanWa.thermocouple[2], dataFrame.tanWa.armButton, 
    dataFrame.tanWa.abortButton, dataFrame.tanWa.tankHeating);//19

  //recovery first byte
  memset(byteData, 0, 4);
  byteData[0] |= (dataFrame.recovery.isArmed << 6);
  byteData[0] |= (dataFrame.recovery.firstStageContinouity << 5);
  byteData[0] |= (dataFrame.recovery.secondStageContinouity << 4);
  byteData[0] |= (dataFrame.recovery.separationSwitch1 << 3);
  byteData[0] |= (dataFrame.recovery.separationSwitch2 << 2);
  byteData[0] |= (dataFrame.recovery.telemetrumFirstStage << 1);
  byteData[0] |= (dataFrame.recovery.telemetrumSecondStage << 0);
  
  //recovery second byte  
  byteData[1] |= (dataFrame.recovery.altimaxFirstStage << 5);
  byteData[1] |= (dataFrame.recovery.altimaxSecondStage << 4);
  byteData[1] |= (dataFrame.recovery.apogemixFirstStage << 3);
  byteData[1] |= (dataFrame.recovery.apogemixSecondStage << 2);
  byteData[1] |= (dataFrame.recovery.firstStageDone << 1);
  byteData[1] |= (dataFrame.recovery.secondStageDone << 0);

  snprintf(recoveryFrame, recoverySize, "%d;%d;", byteData[0], byteData[1]);

  //error first byte  
  memset(byteData, 0, 4);
  byteData[0] |= (errors.sd << 5);
  byteData[0] |= (errors.flash << 3);
  byteData[0] |= (errors.watchdog);

  //error second byte  
  byteData[1] |= (errors.recovery << 6);
  byteData[1] |= (errors.exceptions << 3);
  byteData[1] |= (errors.espnow << 0);

  //error third byte  
  byteData[2] |= (errors.sensors << 2);
  byteData[2] |= (errors.rtos << 0);

  snprintf(errorsFrame, errorsSize, "%d;%d;%d", byteData[0], byteData[1], byteData[2]);

  strcpy(data, LORA_TX_DATA_PREFIX);
  strcat(data, mcbFrame);
  strcat(data, pitotFrame);
  strcat(data, mvFrame);
  strcat(data, uvFrame);
  strcat(data, tanwaFrame);
  strcat(data, recoveryFrame); //2
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

/**
 * @brief create data frame for sd card
 * 
 * @param data 
 */
void RocketControl::createSDFrame(char* data){
  size_t mcbSize, pitotSize, mvSize, uvSize, tanwaSize, bbSize, plSize, optionsSize, recoverySize, errorsSize;

  mcbSize = snprintf(NULL, 0, "%d;%lu;%d;%d;%0.2f;%d;%0.4f;%0.4f;%0.2f;%d;%d;%0.2f;%0.2f;%0.2f;%0.2f;",
    dataFrame.mcb.state, millis(), missionTimer.getTime(), getDisconnectRemainingTime(),
    dataFrame.mcb.batteryVoltage, dataFrame.mcb.watchdogResets, dataFrame.mcb.GPSlal, 
    dataFrame.mcb.GPSlong, dataFrame.mcb.GPSalt, dataFrame.mcb.GPSsat, dataFrame.mcb.GPSsec,
    dataFrame.mcb.temp_lp25, dataFrame.mcb.pressure, dataFrame.mcb.altitude, dataFrame.mcb.velocity) + 1;

  pitotSize = snprintf(NULL, 0, "%d;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;",
    dataFrame.pitot.wakeUp, dataFrame.pitot.batteryVoltage, dataFrame.pitot.staticPressure, 
    dataFrame.pitot.dynamicPressure, dataFrame.pitot.temperature, dataFrame.pitot.altitude,
    dataFrame.pitot.velocity, dataFrame.pitot.predictedApogee) + 1; //8
  
  mvSize = snprintf(NULL, 0, "%d;%0.2f;%d;%0.2f;%0.2f;",
    dataFrame.mainValve.wakeUp, dataFrame.mainValve.batteryVoltage, dataFrame.mainValve.valveState,
    dataFrame.mainValve.thermocouple[0], dataFrame.mainValve.thermocouple[1]) + 1; //5

  uvSize = snprintf(NULL, 0, "%d;%0.2f;%d;%0.2f;%d;%d;%d;%d;%d;",
    dataFrame.upustValve.wakeUp, dataFrame.upustValve.batteryVoltage, dataFrame.upustValve.valveState,
    dataFrame.upustValve.tankPressure, dataFrame.upustValve.hall[0], dataFrame.upustValve.hall[1],
    dataFrame.upustValve.hall[2], dataFrame.upustValve.hall[3], dataFrame.upustValve.hall[4]) + 1; //9
  
  tanwaSize = snprintf(NULL, 0, "%d;%0.2f;%d;%d;%d;%d;%d;%d;%d;%0.2f;%0.2f;%d;%d;%f;%f;%f;%d;%d;%d;",
    dataFrame.tanWa.tanWaState, dataFrame.tanWa.vbat, dataFrame.tanWa.igniterContinouity[0],
    dataFrame.tanWa.igniterContinouity[1], dataFrame.tanWa.motorState[0], dataFrame.tanWa.motorState[1],
    dataFrame.tanWa.motorState[2], dataFrame.tanWa.motorState[3], dataFrame.tanWa.motorState[4],
    dataFrame.tanWa.rocketWeight, dataFrame.tanWa.butlaWeight, dataFrame.tanWa.rocketWeightRaw, 
    dataFrame.tanWa.butlaWeightRaw, dataFrame.tanWa.thermocouple[0], dataFrame.tanWa.thermocouple[1], 
    dataFrame.tanWa.thermocouple[2], dataFrame.tanWa.armButton, 
    dataFrame.tanWa.abortButton, dataFrame.tanWa.tankHeating) + 1; //19
  
  optionsSize = snprintf(NULL, 0, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;",
    options.LoRaFrequencyMHz, options.countdownTime, options.ignitionTime,
    options.tankMinPressure, options.flashWrite, options.forceLaunch, options.dataCurrentPeriod,
    options.loraCurrentPeriod, options.flashDataCurrentPeriod, options.sdDataCurrentPeriod) + 1;

  recoverySize = snprintf(NULL, 0, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;",
    dataFrame.recovery.isArmed, dataFrame.recovery.firstStageContinouity, dataFrame.recovery.secondStageContinouity,
    dataFrame.recovery.separationSwitch1, dataFrame.recovery.separationSwitch2,
    dataFrame.recovery.telemetrumFirstStage, dataFrame.recovery.telemetrumSecondStage,
    dataFrame.recovery.altimaxFirstStage, dataFrame.recovery.altimaxSecondStage,
    dataFrame.recovery.apogemixFirstStage, dataFrame.recovery.apogemixSecondStage,
    dataFrame.recovery.firstStageDone, dataFrame.recovery.secondStageDone) + 1;

  errorsSize = snprintf(NULL, 0, "%d;%d;%d;%d;%d;%d;%d;%d",
    errors.sd, errors.flash, errors.rtos, errors.espnow,
    errors.watchdog, errors.sensors, errors.exceptions, errors.recovery) + 1;

  char mcbFrame[mcbSize] = {};
  char pitotFrame[pitotSize] = {};
  char mvFrame[mvSize] = {};
  char uvFrame[uvSize] = {};
  char tanwaFrame[tanwaSize] = {};
  //char payloadFrame[plSize] = {};
  //char blackboxFrame[bbSize] = {};
  char optionsFrame[optionsSize] = {};
  char recoveryFrame[recoverySize] = {};
  char errorsFrame[errorsSize] = {};


  //MCB
  snprintf(mcbFrame, mcbSize, "%d;%lu;%d;%d;%0.2f;%d;%0.4f;%0.4f;%0.2f;%d;%d;%0.2f;%0.2f;%0.2f;%0.2f;",
    dataFrame.mcb.state, millis(), missionTimer.getTime(), getDisconnectRemainingTime(),
    dataFrame.mcb.batteryVoltage, dataFrame.mcb.watchdogResets, dataFrame.mcb.GPSlal, 
    dataFrame.mcb.GPSlong, dataFrame.mcb.GPSalt, dataFrame.mcb.GPSsat, dataFrame.mcb.GPSsec,
    dataFrame.mcb.temp_lp25, dataFrame.mcb.pressure, dataFrame.mcb.altitude, dataFrame.mcb.velocity);

  snprintf(pitotFrame, pitotSize, "%d;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;",
    dataFrame.pitot.wakeUp, dataFrame.pitot.batteryVoltage, dataFrame.pitot.staticPressure, 
    dataFrame.pitot.dynamicPressure, dataFrame.pitot.temperature, dataFrame.pitot.altitude,
    dataFrame.pitot.velocity, dataFrame.pitot.predictedApogee); //8

  snprintf(mvFrame, mvSize, "%d;%0.2f;%d;%0.2f;%0.2f;",
    dataFrame.mainValve.wakeUp, dataFrame.mainValve.batteryVoltage, dataFrame.mainValve.valveState,
    dataFrame.mainValve.thermocouple[0], dataFrame.mainValve.thermocouple[1]); //5

  snprintf(uvFrame, uvSize, "%d;%0.2f;%d;%0.2f;%d;%d;%d;%d;%d;",
    dataFrame.upustValve.wakeUp, dataFrame.upustValve.batteryVoltage, dataFrame.upustValve.valveState,
    dataFrame.upustValve.tankPressure, dataFrame.upustValve.hall[0], dataFrame.upustValve.hall[1],
    dataFrame.upustValve.hall[2], dataFrame.upustValve.hall[3], dataFrame.upustValve.hall[4]); //9
  
  snprintf(tanwaFrame, tanwaSize, "%d;%0.2f;%d;%d;%d;%d;%d;%d;%d;%0.2f;%0.2f;%d;%d;%f;%f;%f;%d;%d;%d;",
    dataFrame.tanWa.tanWaState, dataFrame.tanWa.vbat, dataFrame.tanWa.igniterContinouity[0],
    dataFrame.tanWa.igniterContinouity[1], dataFrame.tanWa.motorState[0], dataFrame.tanWa.motorState[1],
    dataFrame.tanWa.motorState[2], dataFrame.tanWa.motorState[3], dataFrame.tanWa.motorState[4],
    dataFrame.tanWa.rocketWeight, dataFrame.tanWa.butlaWeight, dataFrame.tanWa.rocketWeightRaw, 
    dataFrame.tanWa.butlaWeightRaw, dataFrame.tanWa.thermocouple[0], dataFrame.tanWa.thermocouple[1], 
    dataFrame.tanWa.thermocouple[2], dataFrame.tanWa.armButton, 
    dataFrame.tanWa.abortButton, dataFrame.tanWa.tankHeating); //19

  snprintf(optionsFrame, optionsSize, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;",
    options.LoRaFrequencyMHz, options.countdownTime, options.ignitionTime,
    options.tankMinPressure, options.flashWrite, options.forceLaunch, options.dataCurrentPeriod,
    options.loraCurrentPeriod, options.flashDataCurrentPeriod, options.sdDataCurrentPeriod);

  snprintf(recoveryFrame, recoverySize, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;",
    dataFrame.recovery.isArmed, dataFrame.recovery.firstStageContinouity, dataFrame.recovery.secondStageContinouity,
    dataFrame.recovery.separationSwitch1, dataFrame.recovery.separationSwitch2,
    dataFrame.recovery.telemetrumFirstStage, dataFrame.recovery.telemetrumSecondStage,
    dataFrame.recovery.altimaxFirstStage, dataFrame.recovery.altimaxSecondStage,
    dataFrame.recovery.apogemixFirstStage, dataFrame.recovery.apogemixSecondStage,
    dataFrame.recovery.firstStageDone, dataFrame.recovery.secondStageDone);

  snprintf(errorsFrame, errorsSize, "%d;%d;%d;%d;%d;%d;%d;%d",
    errors.sd, errors.flash, errors.rtos, errors.espnow,
    errors.watchdog, errors.sensors, errors.exceptions, errors.recovery);

  strcpy(data, LORA_TX_DATA_PREFIX);
  strcat(data, mcbFrame);
  strcat(data, pitotFrame);
  strcat(data, mvFrame);
  strcat(data, uvFrame);
  strcat(data, tanwaFrame);
  strcat(data, optionsFrame);
  strcat(data, recoveryFrame);
  strcat(data, errorsFrame); 
  strcat(data, "\n");
}