#ifndef DATA_STRUCTS_PYSD_HH
#define DATA_STRUCTS_PYSD_HH

#include <stdint.h>
#include "freertos/FreeRTOS.h"

typedef struct {
    bool wakenUp : 1; //if waken up
    float vBat; //battery voltage
    float statPress;
    float dynamicPress;
    float temp;
    float altitude;
    float speed;
    bool estimated_apogee;
} pysd_PitotData;

typedef pysd_PitotData PitotData;

typedef struct {
    bool wakeUp : 1;
    uint8_t valveState : 2;
    float thermocoupleOne;
    float thermocoupleTwo;
    float batteryVoltage;
} pysd_MainValveData;

typedef pysd_MainValveData MainValveData;

typedef struct {
    uint8_t tanWaState;
    uint16_t ground_pressure;
    bool tankHeating : 1;
    bool abortButton : 1;
    bool armButton : 1;
    bool igniterContinouityOne;
    bool igniterContinouityTwo;
    float vbat;
    uint8_t motor_one;
    uint8_t motor_two;
    uint8_t motor_three;
    uint8_t motor_four;
    uint8_t motor_five;
    float rocketWeight;
    float tankWeight;
    uint32_t rocketWeightRaw;
    uint32_t tankWeightRaw;
    float thermocouple_one;
    float thermocouple_two;
    float thermocouple_three;
} pysd_TanWaData;

typedef pysd_TanWaData TanWaData;

typedef struct {
    bool wakeUp : 1;
    uint8_t valveState : 2;
    uint16_t thermistor;
    float tankPressure;
    float batteryVoltage;
} pysd_UpustValveData;

typedef pysd_UpustValveData UpustValveData;

typedef struct {
    bool wakenUp : 1; //if waken up
    bool isRecording; // if rpi says it is
    bool data : 1; // if data is collected
    float vBat; //battery voltage
    bool isRpiOn;
} pysd_PayloadData;

typedef pysd_PayloadData PayloadData;

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

// typedef pysd_RecoveryData RecoveryData;

typedef union {
    pysd_RecoveryData data;
    uint8_t raw[sizeof(pysd_RecoveryData)];
} RecoveryData;

typedef struct {
    bool wakeUp : 1;
    float batteryVoltage;
} pysd_SlaveData;

typedef pysd_SlaveData SlaveData;

typedef struct {
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
    float mx;
    float my;
    float mz;
    float temperature;
    float pressure;
    float altitude;
} pysd_imu_data;

typedef struct {
    float latitude;
    float longitude;
    float gps_altitude;
    uint8_t satellites;
    bool is_time_valid;
} pysd_gps_data;

typedef struct {
    uint8_t state;
    uint32_t uptime;
    int32_t mission_timer;
    uint32_t disconnect_remaining_time;
    uint8_t connection_status;
    float batteryVoltage;
    pysd_imu_data imu;
    pysd_gps_data gps;
    float temp_mcp;
    float temp_lp25;
    float pressure;
    float altitude;
    float velocity;
    uint8_t watchdogResets;
    bool ignition : 1;
    float apogee;
} pysd_MCB;

typedef pysd_MCB MCB;

typedef struct {
	uint32_t lora_freq_khz;
    int32_t countdown_begin_time;
	int32_t ignition_time; //ignition time
    uint8_t tank_min_pressure; //bar
	bool flash_write : 1;
	bool force_launch : 1;
    TickType_t data_current_period;
    TickType_t lora_current_period;
    TickType_t flash_write_current_period;
    TickType_t sd_write_current_period;
}pysd_Options;

typedef pysd_Options Options;

typedef struct {
    uint8_t sd;
    uint8_t flash;
    uint8_t rtos;
    uint8_t espnow;
    uint8_t watchdog;
    uint8_t sensors;
    uint8_t exceptions;
    uint8_t recovery;
}pysd_Errors;

typedef struct{
    pysd_MCB mcb;
    pysd_PitotData pitot;
    pysd_MainValveData mainValve;
    pysd_TanWaData tanWa;
    pysd_UpustValveData upustValve;
    pysd_RecoveryData recovery;
    pysd_SlaveData blackBox;
    pysd_PayloadData payload;
    pysd_Options options;
    pysd_Errors errors;
} pysdmain_DataFrame;

typedef pysdmain_DataFrame DataFrame;

#endif