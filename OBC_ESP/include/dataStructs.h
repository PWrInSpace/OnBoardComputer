#ifndef DATA_STRUCTS_HH
#define DATA_STRUCTS_HH

#include <cstdint>
#include <Arduino.h>
#include "mainStructs.h"
#include "errors.h"
#include "missionTimer.h"

/**   RX    **/

struct PitotData {
  bool wakeUp : 1;
  float staticPressure;
  float dynamicPressure;
  float temperature;
  float altitude;
  float velocity;
  float predictedApogee;

  float batteryVoltage;
};

struct MainValveData {
  bool wakeUp : 1;
  uint8_t valveState : 2;
  float thermocouple[2];
  float batteryVoltage;
};

struct TanWaData {
  uint8_t fillValveState : 2;
  uint8_t deprValveState : 2;
  uint8_t pullValveState : 2;
  uint8_t tanWaState : 4;
  bool igniterContinouity : 1;
  float rocketWeight;
  float butlaWeight; 
  uint32_t rocketWeightRaw;
  uint32_t butlaWeightRaw;
  float batteryVoltage;
};

struct UpustValveData {
  bool wakeUp : 1;
  uint8_t valveState : 2;
  uint16_t hall[5];
  float tankPressure;
  float batteryVoltage;
};

struct RecoveryData {
  bool isArmed : 1;
  bool firstStageContinouity : 1;
  bool secondStageContinouity : 1;
  bool separationSwitch1 : 1;
  bool separationSwitch2 : 1;
  bool telemetrumFirstStage : 1;
  bool telemetrumSecondStage : 1;
  bool altimaxFirstStage : 1;
  bool altimaxSecondStage : 1;
  bool apogemixFirstStage : 1;
  bool apogemixSecondStage : 1;
  bool firstStageDone : 1;
  bool secondStageDone : 1;
};

struct DataFrame {
  PitotData       pitot;
  MainValveData   mainValve;
  TanWaData       tanWa;
  UpustValveData  upustValve;
  RecoveryData    recovery;
  Errors          errors;
  Timer missionTimer;
  //float imuData[12]; //TODO
  float pressure;
  uint8_t temp;
  float altitude;
  float velocity;
  float batteryVoltage;
  float GPSlal;
  float GPSlong;
  float GPSalt;
  uint8_t GPSSat;
  uint8_t GPSsec;
  //uint16_t errors;
  bool ignition;

  DataFrame() = default;
  bool allDevicesWokeUp();
  String createLoRaFrame(StateMachine state, uint32_t disconnectTime);
  String createSDFrame(StateMachine state, uint32_t disconnectTime, Options options);
};


/**   TX    **/

struct TxDataEspNow{
  uint16_t sleepTime;
  uint8_t command;
  uint16_t commandTime;

  TxDataEspNow() = default;
  TxDataEspNow(uint16_t _sleepTime, uint8_t _command, uint16_t _commandTime);
  void setVal(uint16_t _sleepTime, uint8_t _command, uint16_t _commandTime);
};

#endif