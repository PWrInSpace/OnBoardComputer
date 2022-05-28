#ifndef DATA_STRUCTS_HH
#define DATA_STRUCTS_HH

#include <cstdint>
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
  float batteryVoltage;
};

struct TanWaData {
  uint8_t fillValveState : 2;
  uint8_t deprValveState : 2;
  uint8_t pullState : 2;
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
  bool isTeleActive :1;
};

struct SlaveData {
  bool wakeUp : 1;
  float batteryVoltage;
};

struct MCB{
  //float imuData[12]; //TODO
  float batteryVoltage;
  float GPSlal;
  float GPSlong;
  float GPSalt;
  uint8_t GPSsat;
  uint8_t GPSsec;
  float temp;
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
  SlaveData       payLoad;
  MCB mcb;

  DataFrame() = default;
};

#endif