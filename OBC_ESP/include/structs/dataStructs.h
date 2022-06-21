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
  bool wakenUp : 1; //if waken up
  float vBat; //battery voltage
  float statPress;
  float dynamicPress;
  float temp;
  uint16_t altitude;
  uint8_t speed;
  bool apogee;
  bool isRecording : 1; // if rpi says it is
  bool data : 1; // if data is collected
};

struct MainValveData {
  bool wakeUp : 1;
  uint8_t valveState : 2;
  float thermocouple[2];
  //float termistor;
  //float pressure;
  float batteryVoltage;
};

struct TanWaData {
  uint8_t tanWaState;
  bool tankHeating : 1;
  bool abortButton : 1;
  bool armButton : 1;
  bool igniterContinouity[2];
  float vbat;
  uint8_t motorState[5];
  float rocketWeight;
  float tankWeight; 
  uint32_t rocketWeightRaw;
  uint32_t tankWeightRaw;
  float thermocouple[3];
};

struct UpustValveData {
  bool wakeUp : 1;
  uint8_t valveState : 2;
  uint16_t thermistor;
  float tankPressure;
  float batteryVoltage;
};

struct PlData {
    bool wakenUp : 1; //if waken up
    bool isRecording; // if rpi says it is
    bool data : 1; // if data is collected
    float vBat; //battery voltage
    bool isRpiOn;
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
  float imuData[11];
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
  float apogee;
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