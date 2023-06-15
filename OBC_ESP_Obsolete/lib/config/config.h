#ifndef CONFIG_HH
#define CONFIG_HH

#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include <stdint.h>
// #include "../include/structs/stateMachine.h"

#define SD_FRAME_ARRAY_SIZE     1024
#define LORA_FRAME_ARRAY_SIZE   256
#define LORA_TX_DATA_PREFIX     "R4D;"
#define LORA_TX_OPTIONS_PREFIX  "R4C;"

//VALVE
#define VALVE_CLOSE     0x00
#define VALVE_OPEN      0x01
#define VALVE_UNKNOWN   0x02

//COMMANDS
#define VALVE_CLOSE_COMMAND 0x10
#define VALVE_OPEN_COMMAND  0x11
#define IGNITION_COMMAND    60
#define PAYLOAD_RECORD_OFF  69
#define PAYLOAD_RESET       21
#define PAYLOAD_RECORCD_ON  37

#define PAYLOAD_SWITCH_ON_AFTER_STATE_TIME 30000 / portTICK_PERIOD_MS
#define CAMERA_TURN_OFF_TIME 30000 / portTICK_PERIOD_MS

//TIMERS
#define TIMER_PAYLOAD_SWITCH_ON_NUM     (void*)0
#define TIMER_TURN_OFF_RECORDING_NUM    (void*)1
#define TIMER_IGNITION_REQUEST_NUM      (void*)2


//SENSORS ADRESS
#define PRESSURE_SENSOR_ADRESS  0x5C
#define GPS_ADRESS              0x42
#define MCP_ADDRESS             0x18


//I2C RECOVERY COMMANDS
#define RECOVERY_ADDRES         0x03
#define RECOVERY_ARM_COMMAND    1
#define RECOVERY_DISARM         2
#define RECOVERY_TELEMETRUM_ON  3
#define RECOVERY_TELEMETRUM_OFF 4
#define RECOVERY_FORCE_1        165
#define RECOVERY_FORCE_2        90


//OPTIONS DEFAULT VALUE (SETUP)
#define LoRa_FREQUENCY_KHZ  867000
#define COUNTDOWN_TIME      -30000
#define IGNITION_TIME       -13500
#define TANK_MIN_PRESSURE   35 //barpayload
#define FLASH_WRITE         false
#define FORCE_LAUNCH        false

//PERIODS
#define ESP_NOW_SLEEP_TIME      30 * 1000
#define ESP_NOW_LONG_PERIOD     500
#define ESP_NOW_SHORT_PERIOD    100
#define LORA_PERIOD             500
//#define DATA_PERIOD 100
#define FLASH_SHORT_PERIOD      100
#define FLASH_LONG_PERIOD       500
#define SD_SHORT_PERIOD         100
#define SD_LONG_PERIOD          500
#define IDLE_PERIOD             10 * 1000

#define DATA_PERIOD             100
#define PERIOD_ARRAY_SIZE       13

#define STATE_TASK_LOOP_INTERVAL 200

extern const TickType_t loraPeriod[PERIOD_ARRAY_SIZE];
extern const TickType_t sdPeriod[PERIOD_ARRAY_SIZE];
extern const TickType_t flashPeriod[PERIOD_ARRAY_SIZE];

/********************************************/
/*               ESP_NOW                    */
/********************************************/

//ADRESSES
extern const uint8_t adressOBC[];      
extern const uint8_t adressPitot[];
extern const uint8_t adressMValve[];  
extern const uint8_t adressUpust[];    
extern const uint8_t adressBlackBox[];
extern const uint8_t adressTanWa[]; 
extern const uint8_t adressPayLoad[];
extern const uint8_t adressACS[];
extern const uint8_t cameraACS[];
extern const uint8_t cameraPitot[];
extern const uint8_t cameraRecovery[];


//ESP NOW NOTIFICATION 
// #define TANWA           0
// #define PITOT           1
// #define MAIN_VALVE      2
// #define UPUST_VALVE     3
// #define BLACK_BOX       4
// #define PAYLOAD         5
// #define ACS             6
// #define CAMERA_ACS      7
// #define CAMERA_PITOT    8
// #define CAMERA_RECOVERY 9

typedef enum{
    TANWA = 0,
    PITOT,
    MAIN_VALVE,
    UPUST_VALVE,
    BLACK_BOX,
    PAYLOAD,
    ACS,
    CAMERA_ACS,
    CAMERA_PITOT,
    CAMERA_RECOVERY,
}EspNowDevice;

#define ESP_NOW_SLEEP_TIME 30 * 1000

//ESP NOW PERIODS
extern uint16_t valvePeriod[PERIOD_ARRAY_SIZE];

extern uint16_t pitotPeriod[PERIOD_ARRAY_SIZE];

extern uint16_t espNowDefaultPeriod[PERIOD_ARRAY_SIZE];

extern uint16_t payloadPeriod[PERIOD_ARRAY_SIZE];

//esp now connection array
//static bool isConnectedFlags[6];
//#define CONNECTION_CHECK_DEVICE_NUMBER 6

//TIMERS
extern const TickType_t watchdogDelay;
extern const TickType_t disconnectDelay;
extern const TickType_t espNowConnectionCheckPeriod; 

extern const String dataFileName;
extern const String logFileName;


#endif