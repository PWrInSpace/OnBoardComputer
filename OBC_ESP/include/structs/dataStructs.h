#ifndef DATA_STRUCTS_HH
#define DATA_STRUCTS_HH

#include <cstdint>
#include "stateMachine.h"

/**   RX    **/

enum ValveState{
  Close = 0, 
  Open = 1, 
  IDK = 2 , 
  Vent = 3, 
  AttemptToOpen = 4, 
  AttemptToClose = 5
};

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
  //sfloat termistor;
  float pressure;
  float batteryVoltage;
};

struct TanWaData {
  uint8_t tanWaState; //
  bool igniterContinouity[2]; //
  float vbat; //
  uint8_t motorState[5]; //
  float rocketWeight; //
  float butlaWeight; //
  uint32_t rocketWeightRaw; //
  uint32_t butlaWeightRaw; //
  float thermocouple[3]; //
  bool tankHeating : 1;//
  bool abortButton : 1;//
  bool armButton : 1; //
};

struct UpustValveData {
  bool wakeUp : 1;
  uint8_t valveState : 2;
  uint16_t thermistor;
  float tankPressure;
  float batteryVoltage;
};

struct PlData {
  bool wakeUp : 1;
  bool isRecording : 1;
  bool data : 1;
  float vbat;
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
  bool isTeleActive :1;
};

struct SlaveData {
  bool wakeUp : 1;
  float batteryVoltage;
};

struct MCB{
  float imuData[12];
  float batteryVoltage;
  float GPSlal;
  float GPSlong;
  float GPSalt;
  uint8_t GPSsat;
  uint8_t GPSsec;
  float temp_mcp;
  float temp_lp25;
  float pressure;
  float altitude;
  float velocity;
  uint8_t watchdogResets;
  uint8_t state;
  bool ignition : 1;

};

struct DataFrame {
  PitotData       pitot;
  MainValveData   mainValve;
  TanWaData       tanWa;
  UpustValveData  upustValve;
  RecoveryData    recovery;
  SlaveData       blackBox;
  PlData          pl;
  MCB mcb;
  uint32_t missionTimer; //DRUT

  DataFrame() = default;
};

#endif