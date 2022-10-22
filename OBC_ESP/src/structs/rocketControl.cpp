#include "../include/structs/rocketControl.h"

/**
 * @brief update state 
 * 
 */
void RocketControl::updateCurrentState(){
  dataFrame.mcb.state = SM_getCurrentState();
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
  snprintf(temp, 40, " [ %d , %lu ]\n", SM_getCurrentState(), millis());
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
  return (dataFrame.pitot.wakenUp && dataFrame.mainValve.wakeUp && dataFrame.upustValve.wakeUp && dataFrame.blackBox.wakeUp);
}

/**
 * @brief create options frame for LoRa communication
 * 
 * @param data data
 */
void RocketControl::createOptionsFrame(char* data, Options options){
  size_t optionsSize;

  optionsSize = snprintf(NULL, 0, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",
    options.lora_freq_khz, options.countdown_begin_time, options.ignition_time,
    options.tank_min_pressure, options.flash_write, options.force_launch, options.data_current_period,
    options.lora_current_period, options.flash_write_current_period, options.sd_write_current_period) + 1;


  char opt[optionsSize];

  snprintf(opt, optionsSize, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",
    options.lora_freq_khz, options.countdown_begin_time, options.ignition_time,
    options.tank_min_pressure, options.flash_write, options.force_launch, options.data_current_period,
    options.lora_current_period, options.flash_write_current_period, options.sd_write_current_period);


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

  char mcbFrame[100] = {};
  char pitotFrame[50] = {};
  char mvFrame[50] = {};
  char uvFrame[50] = {};
  char tanwaFrame[100] = {};
  //char otherSlaves[30];
  char plFrame[50] = {};
  char bbFrame[50] = {};
  char wakenUpFrame[10] = {};
  char connectionFrame[10] = {};
  char valveStateFrame[20] = {};
  char recoveryFrame[10] = {};
  char errorsFrame[20] = {};


  //MCB
  sprintf(mcbFrame, "%d;%d;%d;%0.1f;%0.4f;%0.4f;%d;%d;%d;%0.1f;",
    dataFrame.mcb.state, missionTimer.getTime()/1000, getDisconnectRemainingTime()/1000,
    dataFrame.mcb.batteryVoltage, dataFrame.mcb.gps.latitude, dataFrame.mcb.gps.longitude,
    dataFrame.mcb.gps.satellites, dataFrame.mcb.gps.is_time_valid,
    (int)dataFrame.mcb.altitude, dataFrame.mcb.temp_mcp); //11

  sprintf(pitotFrame, "%0.1f;%d;%d;",
    dataFrame.pitot.vBat, dataFrame.pitot.altitude, (int)dataFrame.pitot.speed); //8

  sprintf(mvFrame, "%0.1f;", dataFrame.mainValve.batteryVoltage); //5

  sprintf(uvFrame, "%0.1f;%0.1f;%d;",
    dataFrame.upustValve.batteryVoltage, dataFrame.upustValve.tankPressure,
    dataFrame.upustValve.thermistor); //9
  
  // sprintf(tanwaFrame, "%d;%0.1f;%d;%d;%0.2f;%0.2f;%d;%d;",
  //   dataFrame.tanWa.tanWaState, dataFrame.tanWa.vbat, dataFrame.tanWa.igniterContinouity[0],
  //   dataFrame.tanWa.igniterContinouity[1], dataFrame.tanWa.rocketWeight, dataFrame.tanWa.tankWeight,
  //   dataFrame.tanWa.armButton, dataFrame.tanWa.abortButton);//19
  
  sprintf(plFrame, "%d;%d;%0.1f;",
    dataFrame.payload.isRecording,
    dataFrame.payload.isRpiOn, dataFrame.payload.vBat);

  // Slaves waken up (from top of the rocket):
  memset(byteData, 0, 4);
  byteData[0] |= (dataFrame.pitot.wakenUp      << 0);
  byteData[0] |= (dataFrame.payload.wakenUp         << 1);
  byteData[0] |= (dataFrame.blackBox.wakeUp   << 2);
  byteData[0] |= (dataFrame.upustValve.wakeUp << 3);
  byteData[0] |= (dataFrame.mainValve.wakeUp  << 4);

  sprintf(wakenUpFrame, "%d;", byteData[0]);

  sprintf(connectionFrame, "%d;", connectedStatus);

  // Valve states:
  memset(byteData, 0, 4);
  // byteData[0] |= (dataFrame.mainValve.valveState  << 0);
  // byteData[0] |= (dataFrame.upustValve.valveState << 2);
  // byteData[0] |= (dataFrame.tanWa.motorState[0]   << 4);

  // byteData[1] |= (dataFrame.tanWa.motorState[1]   << 0);  
  // byteData[1] |= (dataFrame.tanWa.motorState[2]   << 3);

  // byteData[2] |= (dataFrame.tanWa.motorState[3]   << 0);
  // byteData[2] |= (dataFrame.tanWa.motorState[4]   << 3);

  sprintf(valveStateFrame, "%d;%d;%d;", byteData[0], byteData[1], byteData[2]);

  //recovery first byte
  memset(byteData, 0, 4);
  byteData[0] |= (dataFrame.recovery.isArmed                << 6);
  byteData[0] |= (dataFrame.recovery.firstStageContinouity  << 5);
  byteData[0] |= (dataFrame.recovery.secondStageContinouity << 4);
  byteData[0] |= (dataFrame.recovery.separationSwitch1      << 3);
  byteData[0] |= (dataFrame.recovery.separationSwitch2      << 2);
  byteData[0] |= (dataFrame.recovery.telemetrumFirstStage   << 1);
  byteData[0] |= (dataFrame.recovery.telemetrumSecondStage  << 0);
  
  //recovery second byte  
  byteData[1] |= (dataFrame.recovery.altimaxFirstStage    << 5);
  byteData[1] |= (dataFrame.recovery.altimaxSecondStage   << 4);
  byteData[1] |= (dataFrame.recovery.apogemixFirstStage   << 3);
  byteData[1] |= (dataFrame.recovery.apogemixSecondStage  << 2);
  byteData[1] |= (dataFrame.recovery.firstStageDone       << 1);
  byteData[1] |= (dataFrame.recovery.secondStageDone      << 0);

  sprintf(recoveryFrame, "%d;%d;", byteData[0], byteData[1]);

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

  sprintf(errorsFrame, "%d;%d;%d", byteData[0], byteData[1], byteData[2]);

  strcpy(data, LORA_TX_DATA_PREFIX);
  strcat(data, mcbFrame);
  strcat(data, pitotFrame);
  strcat(data, mvFrame);
  strcat(data, uvFrame);
  strcat(data, tanwaFrame);
  strcat(data, plFrame);
  strcat(data, bbFrame);
  strcat(data, wakenUpFrame);
  strcat(data, connectionFrame);
  strcat(data, valveStateFrame);
  strcat(data, recoveryFrame); //2
  strcat(data, errorsFrame); //2
  strcat(data, "\n");
}

/**********************************************************************************************/

/**
 * @brief create data frame for sd card
 * 
 * @param data 
 */
void RocketControl::createSDFrame(char* data){
  size_t mcbSize, pitotSize, mvSize, uvSize, tanwaSize, bbSize, plSize, optionsSize, recoverySize, errorsSize;

  // mcbSize = snprintf(NULL, 0, "%d;%lu;%d;%d;%0.2f;%d;%0.4f;%0.4f;%0.2f;%d;%d;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;",
  //   dataFrame.mcb.state, millis(), missionTimer.getTime(), getDisconnectRemainingTime(),
  //   dataFrame.mcb.batteryVoltage, dataFrame.mcb.watchdogResets, dataFrame.mcb.latitude, 
  //   dataFrame.mcb.longitude, dataFrame.mcb.altitude, dataFrame.mcb.satellites, dataFrame.mcb.is_time_valid,
  //   dataFrame.mcb.temp_lp25, dataFrame.mcb.pressure, dataFrame.mcb.altitude, dataFrame.mcb.velocity,
  //   dataFrame.mcb.imuData[0], dataFrame.mcb.imuData[1], dataFrame.mcb.imuData[2], dataFrame.mcb.imuData[3],
  //   dataFrame.mcb.imuData[4], dataFrame.mcb.imuData[5], dataFrame.mcb.imuData[6], dataFrame.mcb.imuData[7],
  //   dataFrame.mcb.imuData[8], dataFrame.mcb.imuData[9], dataFrame.mcb.imuData[10], dataFrame.mcb.apogee);

  // pitotSize = snprintf(NULL, 0, "%d;%0.2f;%0.2f;%0.2f;%0.2f;%d;%d;%d;",
  //   dataFrame.pitot.wakenUp, dataFrame.pitot.vBat, dataFrame.pitot.statPress, 
  //   dataFrame.pitot.dynamicPress, dataFrame.pitot.temp, dataFrame.pitot.altitude,
  //   dataFrame.pitot.speed, dataFrame.pitot.estimated_apogee) + 1; //8

  
  // mvSize = snprintf(NULL, 0, "%d;%0.2f;%d;%0.2f;%0.2f;",
  //   dataFrame.mainValve.wakeUp, dataFrame.mainValve.batteryVoltage, dataFrame.mainValve.valveState,
  //   dataFrame.mainValve.thermocouple[0], dataFrame.mainValve.thermocouple[1]) + 1; //5

  // uvSize = snprintf(NULL, 0, "%d;%0.2f;%d;%0.2f;%d;",
  //   dataFrame.upustValve.wakeUp, dataFrame.upustValve.batteryVoltage, dataFrame.upustValve.valveState,
  //   dataFrame.upustValve.tankPressure, dataFrame.upustValve.thermistor) + 1;
  
  // tanwaSize = snprintf(NULL, 0, "%d;%0.2f;%d;%d;%d;%d;%d;%d;%d;%0.2f;%0.2f;%d;%d;%f;%f;%f;%d;%d;%d;",
  //   dataFrame.tanWa.tanWaState, dataFrame.tanWa.vbat, dataFrame.tanWa.igniterContinouity[0],
  //   dataFrame.tanWa.igniterContinouity[1], dataFrame.tanWa.motorState[0], dataFrame.tanWa.motorState[1],
  //   dataFrame.tanWa.motorState[2], dataFrame.tanWa.motorState[3], dataFrame.tanWa.motorState[4],
  //   dataFrame.tanWa.rocketWeight, dataFrame.tanWa.tankWeight, dataFrame.tanWa.rocketWeightRaw, 
  //   dataFrame.tanWa.tankWeightRaw, dataFrame.tanWa.thermocouple[0], dataFrame.tanWa.thermocouple[1], 
  //   dataFrame.tanWa.thermocouple[2], dataFrame.tanWa.armButton, 
  //   dataFrame.tanWa.abortButton, dataFrame.tanWa.tankHeating) + 1; //19
  
  plSize = snprintf(NULL, 0, "%f;%d;%d;%d;%d;",
    dataFrame.payload.vBat, dataFrame.payload.wakenUp, dataFrame.payload.isRecording,
    dataFrame.payload.data, dataFrame.payload.isRpiOn) + 1;

  bbSize = snprintf(NULL, 0, "%d;", dataFrame.blackBox.wakeUp) + 1;

  optionsSize = snprintf(NULL, 0, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",
    options.lora_freq_khz, options.countdown_begin_time, options.ignition_time,
    options.tank_min_pressure, options.flash_write, options.force_launch, options.data_current_period,
    options.lora_current_period, options.flash_write_current_period, options.sd_write_current_period) + 1;

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
  char plFrame[plSize] = {};
  char bbFrame[bbSize] = {};
  char optionsFrame[optionsSize] = {};
  char recoveryFrame[recoverySize] = {};
  char errorsFrame[errorsSize] = {};


  // //MCB
  // snprintf(mcbFrame, mcbSize, "%d;%lu;%d;%d;%0.2f;%d;%0.4f;%0.4f;%0.2f;%d;%d;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;",
  //   dataFrame.mcb.state, millis(), missionTimer.getTime(), getDisconnectRemainingTime(),
  //   dataFrame.mcb.batteryVoltage, dataFrame.mcb.watchdogResets, dataFrame.mcb.latitude, 
  //   dataFrame.mcb.longitude, dataFrame.mcb.altitude, dataFrame.mcb.satellites, dataFrame.mcb.is_time_valid,
  //   dataFrame.mcb.temp_lp25, dataFrame.mcb.pressure, dataFrame.mcb.altitude, dataFrame.mcb.velocity,
  //    dataFrame.mcb.imuData[0], dataFrame.mcb.imuData[1], dataFrame.mcb.imuData[2], dataFrame.mcb.imuData[3],
  //   dataFrame.mcb.imuData[4], dataFrame.mcb.imuData[5], dataFrame.mcb.imuData[6], dataFrame.mcb.imuData[7],
  //   dataFrame.mcb.imuData[8], dataFrame.mcb.imuData[9], dataFrame.mcb.imuData[10], dataFrame.mcb.imuData[11]);


  // snprintf(pitotFrame, pitotSize, "%d;%0.2f;%0.2f;%0.2f;%0.2f;%d;%d;%d;",
  //   dataFrame.pitot.wakenUp, dataFrame.pitot.vBat, dataFrame.pitot.statPress,
  //   dataFrame.pitot.dynamicPress, dataFrame.pitot.temp, dataFrame.pitot.altitude,
  //   dataFrame.pitot.speed, dataFrame.pitot.estimated_apogee);//8


  // snprintf(mvFrame, mvSize, "%d;%0.2f;%d;%0.2f;%0.2f;",
  //   dataFrame.mainValve.wakeUp, dataFrame.mainValve.batteryVoltage, dataFrame.mainValve.valveState,
  //   dataFrame.mainValve.thermocouple[0], dataFrame.mainValve.thermocouple[1]); //5

  snprintf(uvFrame, uvSize, "%d;%0.2f;%d;%0.2f;%d;",
    dataFrame.upustValve.wakeUp, dataFrame.upustValve.batteryVoltage, dataFrame.upustValve.valveState,
    dataFrame.upustValve.tankPressure, dataFrame.upustValve.thermistor);
  
  // snprintf(tanwaFrame, tanwaSize, "%d;%0.2f;%d;%d;%d;%d;%d;%d;%d;%0.2f;%0.2f;%d;%d;%f;%f;%f;%d;%d;%d;",
  //   dataFrame.tanWa.tanWaState, dataFrame.tanWa.vbat, dataFrame.tanWa.igniterContinouity[0],
  //   dataFrame.tanWa.igniterContinouity[1], dataFrame.tanWa.motorState[0], dataFrame.tanWa.motorState[1],
  //   dataFrame.tanWa.motorState[2], dataFrame.tanWa.motorState[3], dataFrame.tanWa.motorState[4],
  //   dataFrame.tanWa.rocketWeight, dataFrame.tanWa.tankWeight, dataFrame.tanWa.rocketWeightRaw, 
  //   dataFrame.tanWa.tankWeightRaw, dataFrame.tanWa.thermocouple[0], dataFrame.tanWa.thermocouple[1], 
  //   dataFrame.tanWa.thermocouple[2], dataFrame.tanWa.armButton, 
  //   dataFrame.tanWa.abortButton, dataFrame.tanWa.tankHeating); //19

  snprintf(plFrame, plSize, "%f;%d;%d;%d;%d;",
    dataFrame.payload.vBat, dataFrame.payload.wakenUp, dataFrame.payload.isRecording,
    dataFrame.payload.data, dataFrame.payload.isRpiOn);

  snprintf(bbFrame, bbSize, "%d;", dataFrame.blackBox.wakeUp);

  snprintf(optionsFrame, optionsSize, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",
    options.lora_freq_khz, options.countdown_begin_time, options.ignition_time,
    options.tank_min_pressure, options.flash_write, options.force_launch, options.data_current_period,
    options.lora_current_period, options.flash_write_current_period, options.sd_write_current_period);
 
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
  strcat(data, plFrame);
  strcat(data, bbFrame);
  strcat(data, optionsFrame);
  strcat(data, recoveryFrame);
  strcat(data, errorsFrame); 
  strcat(data, "\n");
}