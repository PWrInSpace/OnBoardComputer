#ifndef DATA_STRUCTS_PYSD_HH
#define DATA_STRUCTS_PYSD_HH

#include <stdint.h>

typedef struct {
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
} pysd_PitotData;

typedef struct {
  bool wakeUp : 1;
  uint8_t valveState : 2;
  float thermocouple[2];
  float batteryVoltage;
} pysd_MainValveData;



typedef struct {
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
} pysd_TanWaData;

typedef struct {
  bool wakeUp : 1;
  uint8_t valveState : 2;
  uint16_t thermistor;
  float tankPressure;
  float batteryVoltage;
} pysd_UpustValveData;

typedef struct {
    bool wakenUp : 1; //if waken up
    bool isRecording; // if rpi says it is
    bool data : 1; // if data is collected
    float vBat; //battery voltage
    bool isRpiOn;
} pysd_PlData;

typedef struct {
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
} pysd_RecoveryData;

typedef struct {
  bool wakeUp : 1;
  float batteryVoltage;
} pysd_SlaveData;

typedef struct {
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
} pysd_MCB;

typedef struct{
  pysd_PitotData pitot;
  pysd_MainValveData mainValve;
  pysd_TanWaData tanWa;
  pysd_UpustValveData upustValve;
  pysd_RecoveryData recovery;
  pysd_SlaveData blackBox;
  pysd_PlData pl;
  pysd_MCB mcb;
  uint32_t missionTimer; //DRUT
} pysdmain_DataFrame;

#endif