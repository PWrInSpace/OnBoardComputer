#include "dataStructs.h"


/****** DATAFRAME ******/
bool DataFrame::allDevicesWokeUp(){
  return (pitot.wakeUp && mainValve.wakeUp && upustValve.wakeUp);
}

void DataFrame::createLoRaFrame(StateMachine state, uint32_t disconnectTime, char* data){
  uint8_t byteData[4] = {};
  char mcbFrame[100] = {};
  char pitotFrame[60] = {};
  char mvFrame[40] = {};
  char uvFrame[40] = {};
  char tanwaFrame[60] = {};
  char recoveryFrame[20] = {};
  char errorsFrame[20] = {};

  strcpy(data, LORA_TX_PREFIX);

  //MCB
  snprintf(mcbFrame, 100, "%d;%lu;%d;%d;%0.2f;%0.4f;%0.4f;%0.4f;%d;%d;",
    state, millis(), missionTimer.getTime(), disconnectTime,
    batteryVoltage, GPSlal, GPSlong, GPSalt, GPSsat, GPSsec);

  strcat(data, mcbFrame);

  //PITOT
  snprintf(pitotFrame, 60, "%d;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;",
    pitot.wakeUp, pitot.batteryVoltage, pitot.staticPressure, pitot.dynamicPressure,
    pitot.temperature, pitot.altitude, pitot.velocity, pitot.predictedApogee);
  
  strcat(data, pitotFrame);

  //MAIN VALVE
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
  snprintf(tanwaFrame, 60, "%d;%0.2f;%d;%d;%d;%d;%d;%d;%0.2f;%0.2f",
    tanWa.tanWaState, tanWa.batteryVoltage, tanWa.igniterContinouity,
    tanWa.fillValveState, tanWa.deprValveState, tanWa.pullState,
    tanWa.rocketWeightRaw, tanWa.butlaWeightRaw, tanWa.rocketWeight,
    tanWa.butlaWeight);
  
  strcat(data, tanwaFrame);

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
  snprintf(recoveryFrame, 20, "%d;%d;", byteData[0], byteData[1]);
  strcat(data, recoveryFrame);

  
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
  snprintf(errorsFrame, 20, "%d;%d\n", byteData[0], byteData[1]);
  strcat(data, errorsFrame);

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
}

/**********************************************************************************************/

void DataFrame::createSDFrame(StateMachine state, uint32_t disconnectTime, Options options, char* data){
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
  snprintf(optionsFrame, 120, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d:%d;%d;",
    options.LoRaFrequencyMHz, options.countdownTime, options.ignitionTime, options.tankMinPressure,
    options.flashWrite, options.forceLaunch, options.espnowSleepTime, options.espnowSlowPeriod,
    options.espnowFastPeriod, options.loraFastPeriod, options.loraSlowPeriod, options.dataFastPeriod, 
    options.flashFastPeriod, options.flashSlowPeriod, options.sdFastPeriod, options.sdSlowPeriod,
    options.sharedPeriod, options.dataFramePeriod, options.loraDataPeriod, options.flashDataPeriod,
    options.sdDataPeriod);
  
  strcat(data, optionsFrame);

  //ERRORS
  snprintf(errorsFrame, 20, "%d;%d;%d;%d;%d;%d;%d\n",
    errors.sd, errors.flash, errors.rtos, errors.espnow,
    errors.watchdog, errors.sensors, errors.exceptions);
  
  strcat(data, errorsFrame);
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

/**********************************************************************************************/

/****** TXDATAESPNOW *******/
TxDataEspNow::TxDataEspNow(uint8_t _command, uint16_t _commandTime):

  command(_command),
  commandTime(_commandTime) {}

void TxDataEspNow::setVal(uint8_t _command, uint16_t _commandTime){

  command = _command;
  commandTime = _commandTime;
}

/**********************************************************************************************/

/*
String DataFrame::createLoRaFrame(StateMachine state, uint32_t disconnectTime){
  uint8_t byte_data = 0x00;
  String frame;
  
  frame = "R4D;";
  frame += String(state) + ';'; //State
  frame += String(millis()) + ';'; //Work time
  frame += String(missionTimer.getTime()) + ';';//Mission time
  frame += String(disconnectTime) + ';';
 frame += String(batteryVoltage, 2) + ';';
  frame += String(GPSlal, 4) + ';';
  frame += String(GPSlong, 4) + ';';
  frame += String(GPSalt, 4) + ';';
  frame += String(GPSSat) + ';';
  frame += String(GPSsec) + ';';
   //PITOT
  frame += String(pitot.wakeUp) + ';';
  frame += String(pitot.batteryVoltage, 2) + ';';
  frame += String(pitot.staticPressure, 2) + ';';
  frame += String(pitot.dynamicPressure, 2) + ';';
  frame += String(pitot.temperature, 2) + ';';
  frame += String(pitot.altitude, 2) + ';';
  frame += String(pitot.velocity, 2) + ';';
  frame += String(pitot.predictedApogee, 2) + ';';

  //MAIN VALVE
  frame += String(mainValve.wakeUp) + ';';
  frame += String(mainValve.batteryVoltage, 2) + ';';
  frame += String(mainValve.valveState) + ';';
  frame += String(mainValve.thermocouple[0]) + ';';
  frame += String(mainValve.thermocouple[1]) + ';';

  //UPUST VALVE
  frame += String(upustValve.wakeUp) + ';';
  frame += String(upustValve.batteryVoltage, 2) + ';';
  frame += String(upustValve.valveState) + ';';
  frame += String(upustValve.tankPressure) + ';';
  frame += String(upustValve.hall[0]) + ';';
  frame += String(upustValve.hall[1]) + ';';
  frame += String(upustValve.hall[2]) + ';';
  frame += String(upustValve.hall[3]) + ';';
  frame += String(upustValve.hall[4]) + ';';
  
  //TANWA
  frame += String(tanWa.tanWaState) + ';';
  frame += String(tanWa.batteryVoltage) + ';';
  frame += String(tanWa.igniterContinouity) + ';';
  frame += String(tanWa.fillValveState) + ';';
  frame += String(tanWa.deprValveState) + ';';
  frame += String(tanWa.pullValveState) + ';';
  frame += String(tanWa.rocketWeightRaw) + ';';
  frame += String(tanWa.butlaWeightRaw) + ';';
  frame += String(tanWa.rocketWeight) + ';';
  frame += String(tanWa.butlaWeight) + ';';

  //recovery first byte
  byte_data = 0x00;
  byte_data |= (recovery.isArmed << 6);
  byte_data |= (recovery.firstStageContinouity << 5);
  byte_data |= (recovery.secondStageContinouity << 4);
  byte_data |= (recovery.separationSwitch1 << 3);
  byte_data |= (recovery.separationSwitch2 << 2);
  byte_data |= (recovery.telemetrumFirstStage << 1);
  byte_data |= (recovery.telemetrumSecondStage << 0);
  frame += String((int)byte_data) + ';'; //DEBUG

  //recovery second byte  
  byte_data = 0x00;
  byte_data |= (recovery.altimaxFirstStage << 5);
  byte_data |= (recovery.altimaxSecondStage << 4);
  byte_data |= (recovery.apogemixFirstStage << 3);
  byte_data |= (recovery.apogemixSecondStage << 2);
  byte_data |= (recovery.firstStageDone << 1);
  byte_data |= (recovery.secondStageDone << 0);
  frame += String((int)byte_data) + ';'; //DEBUG

  //error first byte  
  byte_data = 0x00;
  byte_data |= (errors.sd << 6);
  byte_data |= (errors.flash << 4);
  byte_data |= (errors.watchdog << 1);
  byte_data |= (errors.rtos << 0);
  frame += String((int)byte_data) + ';'; //DEBUG

  //error first byte  
  byte_data = 0x00;
  byte_data |= (errors.exceptions << 3);
  byte_data |= (errors.espnow << 0);
  frame += String((int)byte_data); //DEBUG

  return frame;
}
*/
/*
String DataFrame::createSDFrame(StateMachine state, uint32_t disconnectTime, Options options){
  String frame;
  //Serial.print("Start: "); Serial.println(millis()); //DEBUG
  //MCB
  frame = String(state) + ';'; //State
  frame += String(millis()) + ';'; //Work time
  frame += String(missionTimer.getTime()) + ';';//Mission time
  frame += String(disconnectTime) + ';';

  frame += String(batteryVoltage, 2) + ';';
  frame += String(GPSlal, 4) + ';';
  frame += String(GPSlong, 4) + ';';
  frame += String(GPSalt, 4) + ';';
  frame += String(GPSSat) + ';';
  frame += String(GPSsec) + ';';
  frame += String(temp) + ';';
  frame += String(pressure, 2) + ';';
  frame += String(altitude, 2) + ';';
  frame += String(velocity, 2) + ';';
  frame += String(ignition) + ';';
  //IMU DATA
  //PITOT
  frame += String(pitot.wakeUp) + ';';
  frame += String(pitot.batteryVoltage, 2) + ';';
  frame += String(pitot.staticPressure, 2) + ';';
  frame += String(pitot.dynamicPressure, 2) + ';';
  frame += String(pitot.temperature, 2) + ';';
  frame += String(pitot.altitude, 2) + ';';
  frame += String(pitot.velocity, 2) + ';';
  frame += String(pitot.predictedApogee, 2) + ';';

  //MAIN VALVE
  frame += String(mainValve.wakeUp) + ';';
  frame += String(mainValve.batteryVoltage, 2) + ';';
  frame += String(mainValve.valveState) + ';';
  frame += String(mainValve.thermocouple[0]) + ';';
  frame += String(mainValve.thermocouple[1]) + ';';

  //UPUST VALVE
  frame += String(upustValve.wakeUp) + ';';
  frame += String(upustValve.batteryVoltage, 2) + ';';
  frame += String(upustValve.valveState) + ';';
  frame += String(upustValve.tankPressure) + ';';
  frame += String(upustValve.hall[0]) + ';';
  frame += String(upustValve.hall[1]) + ';';
  frame += String(upustValve.hall[2]) + ';';
  frame += String(upustValve.hall[3]) + ';';
  frame += String(upustValve.hall[4]) + ';';
  
  //TANWA
  frame += String(tanWa.tanWaState) + ';';
  frame += String(tanWa.batteryVoltage) + ';';
  frame += String(tanWa.igniterContinouity) + ';';
  frame += String(tanWa.fillValveState) + ';';
  frame += String(tanWa.deprValveState) + ';';
  frame += String(tanWa.pullValveState) + ';';
  frame += String(tanWa.rocketWeightRaw) + ';';
  frame += String(tanWa.butlaWeightRaw) + ';';
  frame += String(tanWa.rocketWeight) + ';';
  frame += String(tanWa.butlaWeight) + ';';

  //RECOVERY
  frame += String(recovery.isArmed) + ';';
  frame += String(recovery.firstStageContinouity) + ';';
  frame += String(recovery.secondStageContinouity) + ';';
  frame += String(recovery.separationSwitch1) + ';';
  frame += String(recovery.separationSwitch2) + ';';
  frame += String(recovery.telemetrumFirstStage) + ';';
  frame += String(recovery.telemetrumSecondStage) + ';';
  frame += String(recovery.altimaxFirstStage) + ';';
  frame += String(recovery.altimaxSecondStage) + ';';
  frame += String(recovery.apogemixFirstStage) + ';';
  frame += String(recovery.apogemixSecondStage) + ';';
  frame += String(recovery.firstStageDone) + ';';
  frame += String(recovery.secondStageDone) + ';';
  //OPTIONS
  //mutex ??
  frame += String(options.LoRaFrequencyMHz) + ';';
  frame += String(options.countdownTime) + ';';
  frame += String(options.ignitionTime) + ';';
  frame += String(options.tankMinPressure) + ';';
  frame += String(options.espnowSleepTime) + ';';
  frame += String(options.espnowSlowPeriod) + ';';
  frame += String(options.espnowFastPeriod) + ';';
  frame += String(options.flashWrite) + ';';
  frame += String(options.forceLaunch) + ';';
  frame += String(options.mainValveRequestState) + ';';
  frame += String(options.upustValveRequestState) + ';';
  frame += String(options.mainValveCommandTime) + ';';
  frame += String(options.upustValveCommandTime) + ';';
  frame += String((uint16_t)options.loraFastPeriod) + ';';
  frame += String((uint16_t)options.loraSlowPeriod) + ';';
  frame += String((uint16_t)options.dataFastPeriod) + ';';
  frame += String((uint16_t)options.flashFastPeriod) + ';';
  frame += String((uint16_t)options.flashSlowPeriod) + ';';
  frame += String((uint16_t)options.sdFastPeriod) + ';';
  frame += String((uint16_t)options.sdSlowPeriod) + ';';
  frame += String((uint16_t)options.sharedPeriod) + ';';
  frame += String((uint16_t)options.dataFramePeriod) + ';';
  frame += String((uint16_t)options.loraDataPeriod) + ';';
  frame += String((uint16_t)options.flashDataPeriod) + ';';

  frame += String((uint16_t)options.sdDataPeriod) + ';';



  //ERRORS
  frame += String(errors.sd) + ';';
  frame += String(errors.flash) + ';';
  frame += String(errors.rtos) + ';';
  frame += String(errors.espnow) + ';';
  frame += String(errors.watchdog) + ';';
  frame += String(errors.sensors) + ';';
  frame += String(errors.exceptions) + '\n';
  
  
  //Serial.print("Stop: "); Serial.println(millis());  //DEBUG
  //Serial.println(frame.length()); //DEBUG
  return frame;
}
*/