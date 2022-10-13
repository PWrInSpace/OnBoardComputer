#include "config.h"
#include "../include/structs/stateMachine.h"

const TickType_t loraPeriod[PERIOD_ARRAY_SIZE] = {  
  1000, //INIT                  0
  1000, //IDLE                  1
  1000, //IDLE
  1000, //ARM                   2
  1000,  //FILLING               3
  2000,  //RTL                   4
  500,   //COUNTDOWN             5
  1000,  //flight                6
  1000,  //first_stage_recov     7
  1000,  //second_stage_recov    8
  10000,  //on ground             9
  10000, //HOLD                  10
  10000  //ABORT                 11
};

const TickType_t sdPeriod[PERIOD_ARRAY_SIZE] = {  
  10000, //INIT
  10000, //IDLE
  10000, //IDLE
  10000, //ARM
  1000,  //FILLING
  2000, //RTL
  500, //COUNTDOWN
  150, //flight
  500, //first_stage_recov
  500, //second_stage_recov
  10000, //on ground
  10000, //HOLD
  10000 //ABORT
};

const TickType_t flashPeriod[PERIOD_ARRAY_SIZE] = {  
  10000 , //INIT
  10000, //IDLE
  10000, //IDLE
  10000, //ARM
  1000, //FILLING
  2000, //RTL
  500, //COUNTDOWN
  500, //flight
  500, //first_stage_recov
  500, //second_stage_recov
  10000, //on ground
  10000, //HOLD
  10000 //ABORT
};

/********************************************/
/*               ESP_NOW                    */
/********************************************/

//ADRESSES
const uint8_t adressOBC[]       = {0x04, 0x20, 0x04, 0x20, 0x04, 0x20}; 
const uint8_t adressPitot[]     = {0xB8, 0xF0, 0x09, 0xA9, 0xCB, 0x14};
const uint8_t adressMValve[]    = {0x34, 0x94, 0x54, 0xD9, 0x2D, 0x20};
const uint8_t adressUpust[]     = {0xE8, 0x31, 0xCD, 0x85, 0xDA, 0x84}; 
const uint8_t adressBlackBox[]  = {0x78, 0x21, 0x84, 0x8D, 0x83, 0xFC};
const uint8_t adressTanWa[]     = {0x80, 0x08, 0x50, 0x80, 0x08, 0x50}; 
const uint8_t adressPayLoad[]   = {0x34, 0x94, 0x54, 0xD9, 0x5A, 0xE4}; // TODO poprawny adres!!!
const uint8_t adressACS[]       = {0xE8, 0xDB, 0x84, 0xA5, 0x93, 0x5D}; // TODO poprawny adres!!!
const uint8_t cameraACS[]       = {0xE8, 0xDB, 0x84, 0xA5, 0x93, 0x5D}; // TODO poprawny adres!!!
const uint8_t cameraPitot[]     = {0xE8, 0xDB, 0x84, 0xA5, 0x93, 0x5D}; // TODO poprawny adres!!!
const uint8_t cameraRecovery[]  = {0xE8, 0xDB, 0x84, 0xA5, 0x93, 0x5D}; // TODO poprawny adres!!!


typedef struct {
  States state;
  uint16_t period;
} period;

static struct {
  period dupa[12];
}test;

//ESP NOW PERIODS
uint16_t valvePeriod[PERIOD_ARRAY_SIZE] = {
  ESP_NOW_SLEEP_TIME, //INIT
  ESP_NOW_SLEEP_TIME, //IDLE
  ESP_NOW_SLEEP_TIME, //ARM
  1000, //FILLING
  1500,
  1500, //RTL
  250, //COUNTDOWN
  250, //flight
  500, //first_stage_recov
  500, //second_stage_recov
  1800, //on ground
  ESP_NOW_SLEEP_TIME, //HOLD
  10000,//ABORT
};

uint16_t pitotPeriod[PERIOD_ARRAY_SIZE] = {  
  ESP_NOW_SLEEP_TIME, //INIT
  ESP_NOW_SLEEP_TIME, //IDLE
  ESP_NOW_SLEEP_TIME, //ARM
  ESP_NOW_SLEEP_TIME, 
  1500, //FILLING
  1500, //RTL
  500, //COUNTDOWN
  500, //flight
  500, //first_stage_recov
  500, //second_stage_recov
  ESP_NOW_SLEEP_TIME, //on ground
  ESP_NOW_SLEEP_TIME, //HOLD
  ESP_NOW_SLEEP_TIME,//ABORT
};


uint16_t espNowDefaultPeriod[PERIOD_ARRAY_SIZE] = {  
  ESP_NOW_SLEEP_TIME, //INIT
  ESP_NOW_SLEEP_TIME, //IDLE
  ESP_NOW_SLEEP_TIME, 
  ESP_NOW_SLEEP_TIME, //ARM
  1500, //FILLING
  1500, //RTL
  500, //COUNTDOWN
  500, //flight
  500, //first_stage_recov
  500, //second_stage_recov
  ESP_NOW_SLEEP_TIME, //on ground
  ESP_NOW_SLEEP_TIME, //HOLD
  ESP_NOW_SLEEP_TIME,//ABORT
};

uint16_t payloadPeriod[PERIOD_ARRAY_SIZE]{
  ESP_NOW_SLEEP_TIME, //INIT
  ESP_NOW_SLEEP_TIME, //IDLE
  ESP_NOW_SLEEP_TIME, //ARM
  ESP_NOW_SLEEP_TIME, //FILLING
  1500, //ARMEDTOLAUNCH
  1500, //RTL
  1500, //COUNTDOWN
  1500, //flight
  1500, //first_stage_recov
  1500, //second_stage_recov
  ESP_NOW_SLEEP_TIME, //on ground
  ESP_NOW_SLEEP_TIME, //HOLD
  ESP_NOW_SLEEP_TIME,//ABORT
};

//esp now connection array
//static bool isConnectedFlags[6];
//#define CONNECTION_CHECK_DEVICE_NUMBER 6

//TIMERS
const TickType_t watchdogDelay = 1000 / portTICK_PERIOD_MS; //250ms
const TickType_t disconnectDelay = 15 * 60 * 1000 / portTICK_PERIOD_MS; //15min
const TickType_t espNowConnectionCheckPeriod =  35 * 1000 / portTICK_PERIOD_MS; //45sec

const String dataFileName = "/data_";
const String logFileName = "/log_";
