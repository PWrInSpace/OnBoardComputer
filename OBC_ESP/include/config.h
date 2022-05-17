#ifndef CONFIG_HH
#define CONFIG_HH

#include "Arduino.h"
#include "FreeRTOS.h"
#include <stdint.h>
#include "../include/structs/stateMachine.h"

#define SD_FRAME_ARRAY_SIZE 512
#define LORA_FRAME_ARRAY_SIZE 256
#define LORA_TX_DATA_PREFIX "R4D;"
#define LORA_TX_OPTIONS_PREFIX "R4C;"

//VALVE
#define VALVE_OPEN 0x00
#define VALVE_CLOSE 0x01
#define VALVE_UNKNOWN 0x02

//COMMANDS
#define VALVE_CLOSE_COMMAND 0x10
#define VALVE_OPEN_COMMAND  0x11
#define IGNITION_COMMAND 0x32

//SENSORS ADRESS
#define PRESSURE_SENSOR_ADRESS  0x5C
#define GPS_ADRESS  0x42


//I2C RECOVERY COMMANDS
#define RECOVERY_ADDRES 0x03
#define RECOVERY_ARM            1
#define RECOVERY_DISARM         2
#define RECOVERY_TELEMETRUM_ON  3
#define RECOVERY_TELEMETRUM_OFF 4
#define RECOVERY_FORCE_1        165
#define RECOVERY_FORCE_2        90


//OPTIONS DEFAULT VALUE (SETUP)
#define LoRa_FREQUENCY_MHZ 868
#define COUNTDOWN_TIME 15 * 1000
#define IGNITION_TIME -3000
#define TANK_MIN_PRESSURE 35 //bar
#define FLASH_WRITE false
#define FORCE_LAUNCH true

//PERIODS
#define ESP_NOW_SLEEP_TIME 30 * 1000
#define ESP_NOW_LONG_PERIOD 500
#define ESP_NOW_SHORT_PERIOD 100
#define LORA_PERIOD 500
//#define DATA_PERIOD 100
#define FLASH_SHORT_PERIOD 100
#define FLASH_LONG_PERIOD 500
#define SD_SHORT_PERIOD 100
#define SD_LONG_PERIOD 500
#define IDLE_PERIOD 10 * 1000

#define DATA_PERIOD 100 / portTICK_PERIOD_MS
#define PERIOD_ARRAY_SIZE 12

static const uint16_t loraPeriod[PERIOD_ARRAY_SIZE] = {  
  1000 / portTICK_PERIOD_MS, //INIT                  0
  1000 / portTICK_PERIOD_MS, //IDLE                  1
  1000 / portTICK_PERIOD_MS, //ARM                   2
  1000 / portTICK_PERIOD_MS,  //FILLING               3
  2000 / portTICK_PERIOD_MS,  //RTL                   4
  500 / portTICK_PERIOD_MS,   //COUNTDOWN             5
  1000 / portTICK_PERIOD_MS,  //flight                6
  1000 / portTICK_PERIOD_MS,  //first_stage_recov     7
  1000 / portTICK_PERIOD_MS,  //second_stage_recov    8
  1000 / portTICK_PERIOD_MS,  //on ground             9
  10000 / portTICK_PERIOD_MS, //HOLD                  10
  10000 / portTICK_PERIOD_MS  //ABORT                 11
};

static const uint16_t sdPeriod[ESP_NOW_SLEEP_TIME] = {  
  10000 / portTICK_PERIOD_MS, //INIT
  10000 / portTICK_PERIOD_MS, //IDLE
  10000 / portTICK_PERIOD_MS, //ARM
  1000 / portTICK_PERIOD_MS,  //FILLING
  2000 / portTICK_PERIOD_MS, //RTL
  500 / portTICK_PERIOD_MS, //COUNTDOWN
  100 / portTICK_PERIOD_MS, //flight
  500 / portTICK_PERIOD_MS, //first_stage_recov
  500 / portTICK_PERIOD_MS, //second_stage_recov
  1000 / portTICK_PERIOD_MS, //on ground
  10000 / portTICK_PERIOD_MS, //HOLD
  10000 / portTICK_PERIOD_MS //ABORT
};

static const uint16_t flashPeriod[ESP_NOW_SLEEP_TIME] = {  
  10000 / portTICK_PERIOD_MS , //INIT
  10000 / portTICK_PERIOD_MS, //IDLE
  10000 / portTICK_PERIOD_MS, //ARM
  1000 / portTICK_PERIOD_MS, //FILLING
  2000 / portTICK_PERIOD_MS, //RTL
  500 / portTICK_PERIOD_MS, //COUNTDOWN
  500 / portTICK_PERIOD_MS, //flight
  500 / portTICK_PERIOD_MS, //first_stage_recov
  500 / portTICK_PERIOD_MS, //second_stage_recov
  1000 / portTICK_PERIOD_MS, //on ground
  10000 / portTICK_PERIOD_MS, //HOLD
  10000 / portTICK_PERIOD_MS //ABORT
};

/********************************************/
/*               ESP_NOW                    */
/********************************************/

//ADRESSES
const uint8_t adressOBC[]       = {0xB4, 0xE6, 0x2D, 0x85, 0x49, 0x4D}; // TODO poprawny adres!!!
const uint8_t adressPitot[]     = {0x94, 0xB9, 0x7E, 0xC2, 0xE6, 0x28}; // TODO poprawny adres!!!
const uint8_t adressMValve[]    = {0x34, 0x94, 0x54, 0xD9, 0x2D, 0x20};
const uint8_t adressUpust[]     = {0xE8, 0xDB, 0x84, 0xA5, 0x93, 0x5D}; // TODO poprawny adres!!!
const uint8_t adressBlackBox[]  = {0xE8, 0xDB, 0x84, 0xA5, 0x93, 0x5D}; // TODO poprawny adres!!!
const uint8_t adressTanWa[]     = {0xE8, 0xDB, 0x84, 0xA5, 0x93, 0x5D}; // TODO poprawny adres!!!
const uint8_t adressPayLoad[]   = {0xE8, 0xDB, 0x84, 0xA5, 0x93, 0x5D}; // TODO poprawny adres!!!
const uint8_t adressACS[]       = {0xE8, 0xDB, 0x84, 0xA5, 0x93, 0x5D};
const uint8_t cameraACS[]       = {0xE8, 0xDB, 0x84, 0xA5, 0x93, 0x5D};
const uint8_t cameraPitot[]       = {0xE8, 0xDB, 0x84, 0xA5, 0x93, 0x5D};
const uint8_t cameraRecovery[]       = {0xE8, 0xDB, 0x84, 0xA5, 0x93, 0x5D};


//ESP NOW NOTIFICATION 
#define TANWA 1
#define PITOT 2
#define MAIN_VALVE 3
#define UPUST_VALVE 4
#define BLACK_BOX 5
#define PAYLOAD 6
#define ACS 7
#define CAMERA_ACS 8
#define CAMERA_PITOT 9
#define CAMERA_RECOVERY 10

#define ESP_NOW_SLEEP_TIME 30 * 1000

//ESP NOW PERIODS
static const uint16_t mainValvePeriod[ESP_NOW_SLEEP_TIME] = {  
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //INIT
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //IDLE
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //ARM
  500 / portTICK_PERIOD_MS, //FILLING
  500 / portTICK_PERIOD_MS, //RTL
  100 / portTICK_PERIOD_MS, //COUNTDOWN
  500 / portTICK_PERIOD_MS, //flight
  500 / portTICK_PERIOD_MS, //first_stage_recov
  500 / portTICK_PERIOD_MS, //second_stage_recov
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //on ground
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //HOLD
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS //ABORT
};

static const uint16_t upustValvePeriod[ESP_NOW_SLEEP_TIME] = {  
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //INIT
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //IDLE
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //ARM
  500 / portTICK_PERIOD_MS, //FILLING
  500 / portTICK_PERIOD_MS, //RTL
  500 / portTICK_PERIOD_MS, //COUNTDOWN
  100 / portTICK_PERIOD_MS, //flight
  500 / portTICK_PERIOD_MS, //first_stage_recov
  500 / portTICK_PERIOD_MS, //second_stage_recov
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //on ground
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //HOLD
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS //ABORT
};

static const uint16_t pitotPeriod[ESP_NOW_SLEEP_TIME] = {  
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //INIT
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //IDLE
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //ARM
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //FILLING
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //RTL
  500 / portTICK_PERIOD_MS, //COUNTDOWN
  100 / portTICK_PERIOD_MS, //flight
  500 / portTICK_PERIOD_MS, //first_stage_recov
  500 / portTICK_PERIOD_MS, //second_stage_recov
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //on ground
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //HOLD
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS //ABORT
};


static const uint16_t espNowDefaultPeriod[ESP_NOW_SLEEP_TIME] = {  
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //INIT
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //IDLE
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //ARM
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //FILLING
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //RTL
  500 / portTICK_PERIOD_MS, //COUNTDOWN
  500 / portTICK_PERIOD_MS, //flight
  500 / portTICK_PERIOD_MS, //first_stage_recov
  500 / portTICK_PERIOD_MS, //second_stage_recov
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //on ground
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS, //HOLD
  ESP_NOW_SLEEP_TIME / portTICK_PERIOD_MS //ABORT
};



//TIMERS
static const TickType_t watchdogDelay = 1000 / portTICK_PERIOD_MS; //250ms
static const TickType_t disconnectDelay = 15 * 60 * 1000 / portTICK_PERIOD_MS; //15min

static String dataFileName= "/data_";
static String logFileName = "/log_";


#endif