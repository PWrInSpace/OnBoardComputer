#ifndef DATA_STRUCTS_HH
#define DATA_STRUCTS_HH

#include <stdio.h>
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
  bool wakenUp : 1;
  float vBat;
  float statPress;
  float dynamicPress;
  float temp;
  uint16_t altitude;
  uint8_t speed;
  bool apogee;
  bool isRecording : 1; // if rpi says it is
  bool data_collected : 1; // if data is collected
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

struct PayloadData {
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
  float batteryVoltage;
  float GPSlal;
  float GPSlong;
  float GPSalt;
  uint8_t GPSsat;
  uint8_t GPSsec;
  float imuData[11];
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
  PayloadData     payload;
  MCB mcb;
  uint32_t missionTimer; //DRUT
};

bool DF_init(void);
void DF_set_mcb_data(MCB *data);
void DF_set_pitot_data(PitotData *pitot);
void DF_set_main_valve_data(MainValveData *main_valve);
void DF_set_tanwa_data(TanWaData *tanwa);
void DF_set_upust_valve_data(UpustValveData *upust_valve);
void DF_set_recovery_data(RecoveryData *recovery);
void DF_set_blackbox_data(SlaveData *blackbox);
void DF_set_payload_data(PayloadData *payload);
// void DF_set_imu_data(ImuData imu_data);
// void DF_set_gps_data();
void DF_set_mission_timer(uint32_t mission_time);
void DF_create_lora_frame(char *buffer, size_t size);
void DF_create_sd_frame(char *buffer, size_t size);

void DF_create_lora_frame(char* buffer, size_t size);
void DF_create_sd_frame(char* buffer, size_t size);

// THREAD UNSAFE FUNCTIONS
bool DF_create_mcb_frame(char *buffer, size_t size);
bool DF_create_pitot_frame(char *buffer, size_t size);
bool DF_create_main_valve_frame(char *buffer, size_t size);
bool DF_create_tanwa_frame(char *buffer, size_t size);
bool DF_create_upust_valve_frame(char *buffer, size_t size);
bool DF_create_recovery_frame(char *buffer, size_t size);
bool DF_create_blackbox_frame(char *buffer, size_t size);
bool DF_create_payload_frame(char *buffer, size_t size);
#endif