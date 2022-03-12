#include "dataStructs.h"


/****** DATAFRAME ******/
bool DataFrame::allDevicesWokeUp(){
    return (pitot.wakeUp && mainValve.wakeUp && upustValve.wakeUp);
}

String DataFrame::createLoRaFrame(StateMachine state, uint32_t disconnectTime){
  uint8_t byte_data = 0x00;
  String frame;
  
  frame = "R4D;";
  frame += String(state) + ';'; //State
  frame += String(millis()) + ';'; //Work time
  frame += String(missionTimer.getTime()) + ';';//Mission time
  frame += String(disconnectTime) + ';';
/*  frame += String(batteryVoltage, 2) + ';';
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
*/
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

String DataFrame::createSDFrame(StateMachine state, uint32_t disconnectTime, Options options){
  String frame;
  //Serial.print("Start: "); Serial.println(millis()); //DEBUG
  //MCB
  frame = String(state) + ';'; //State
  frame += String(millis()) + ';'; //Work time
  frame += String(missionTimer.getTime()) + ';';//Mission time
  frame += String(disconnectTime) + ';';
  /*
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

  */

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

void DataFrame::createSDFrame(StateMachine state, uint32_t disconnectTime, Options options, char* data){
  snprintf(data, FRAME_ARRAY_SIZE, "%d; %lu; %d; %d\n",
     state, millis(), missionTimer.getTime(), disconnectTime);
}


/****** TXDATAESPNOW *******/
TxDataEspNow::TxDataEspNow(uint16_t _sleepTime, uint8_t _command, uint16_t _commandTime):
  sleepTime(_sleepTime),
  command(_command),
  commandTime(_commandTime) {}

void TxDataEspNow::setVal(uint16_t _sleepTime, uint8_t _command, uint16_t _commandTime){
  sleepTime = _sleepTime;
  command = _command;
  commandTime = _commandTime;
}