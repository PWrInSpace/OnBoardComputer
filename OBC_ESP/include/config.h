#ifndef CONFIG_HH
#define CONFIG_HH

#define SD_FRAME_ARRAY_SIZE 512
#define LORA_FRAME_ARRAY_SIZE 256
#define LORA_TX_PREFIX "R4D;"
//VALVE
#define VALVE_OPEN 0x00
#define VALVE_CLOSE 0x01
#define VALVE_UNKNOWN 0x02

//COMMANDS
#define VALVE_CLOSE_COMMAND 0x10
#define VALVE_OPEN_COMMAND  0x11
#define IGNITION_COMMAND 0x32

//OPTIONS DEFAULT VALUE (SETUP)
#define LoRa_FREQUENCY_MHZ 868
#define COUNTDOWN_TIME 15 * 1000
#define IGNITION_TIME -3000
#define TANK_MIN_PRESSURE 35 //bar
#define ESP_NOW_SLEEP_TIME 30 * 1000
#define ESP_NOW_SLOW_PERIOD 500
#define ESP_NOW_FAST_PERIOD 100
#define FLASH_WRITE false
#define FORCE_LAUNCH true
#define LORA_FAST_PERIOD 500
#define LORA_SLOW_PERIOD 1000
#define DATA_FAST_PERIOD 100
#define FLASH_FAST_PERIOD 100
#define FLASH_SLOW_PERIOD 500
#define SD_SLOW_PERIOD 500
#define SD_FAST_PERIOD 100
#define SHARED_PERIOD 10 * 1000

//ESP NOW NOTIFICATION 
#define TANWA 1
#define PITOT 2
#define MAIN_VALVE 3
#define UPUST_VALVE 4
#define BLACK_BOX 5

static String dataFileName= "/data_";
static String logFileName = "/log_";

//TIMERS
static const TickType_t watchdogDelay = 1000 / portTICK_PERIOD_MS; //250ms
static const TickType_t disconnectDelay = 0.5 * 60 * 1000 / portTICK_PERIOD_MS; //15min

#endif