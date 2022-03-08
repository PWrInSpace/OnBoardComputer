#include "dataStructs.h"

#include "mainStructs.h"
extern RocketControl rc;

/****** MAINSTRUCT *******/



/****** DATAFRAME ******/
bool DataFrame::allDevicesWokeUp(){
    return (pitot.wakeUp && mainValve.wakeUp && upustValve.wakeUp);
}

String DataFrame::createLoRaFrame(){
  uint8_t byte_data = 0x00;
  String frame;
  
  frame = "R4D;";
  frame += String(rc.state) + ';'; //State
  frame += String(millis()) + ';'; //Work time
  frame += String(missionTimer.getTime()) + ';';//Mission time
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
  byte_data |= (errors.exceptions << 1);
  byte_data |= (errors.espnow << 0);
  frame += String((int)byte_data); //DEBUG

  return frame;
}

String DataFrame::createSDFrame(){
  String frame;
  //Serial.print("Start: "); Serial.println(millis()); //DEBUG
  //MCB
  frame = String(rc.state) + ';'; //State
  frame += String(millis()) + ';'; //Work time
  frame += String(missionTimer.getTime()) + ';';//Mission time
  frame += String((xTimerGetExpiryTime(rc.disconnectTimer) - xTaskGetTickCount()) * portTICK_PERIOD_MS) + ';';
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
  /*
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
  */
  //OPTIONS
  //mutex ??
  frame += String(rc.options.flashWrite) + ';';
  frame += String(rc.options.forceLaunch) + ';';
  frame += String(rc.options.LoRaFrequencyMHz) + ';';
  frame += String(rc.options.countdownTime) + ';';
  frame += String(rc.options.ignitionTime) + ';';
  frame += String(rc.options.tankMinPressure) + ';';
  frame += String(rc.options.mainValveTime) + ';';
  frame += String(rc.options.upustValveTime) + ';';
  frame += String(rc.options.mainValveRequestState) + ';';
  frame += String(rc.options.upustValveRequestState) + ';';
  frame += String((uint16_t)rc.options.dataFramePeriod) + ';';
  frame += String((uint16_t)rc.options.loraDataPeriod) + ';';
  frame += String((uint16_t)rc.options.flashDataPeriod) + ';';
  frame += String((uint16_t)rc.options.sdDataPeriod) + ';';


  //ERRORS
  frame += String(errors.sd) + ';';
  frame += String(errors.flash) + ';';
  frame += String(errors.rtos) + ';';
  frame += String(errors.espnow) + ';';
  frame += String(errors.watchdog) + ';';
  frame += String(errors.sensors) + ';';
  frame += String(errors.exceptions);
  
  
  //Serial.print("Stop: "); Serial.println(millis());  //DEBUG
  //Serial.println(frame.length()); //DEBUG
  return frame;
}


/****** ERRORSTRUCT *******/
