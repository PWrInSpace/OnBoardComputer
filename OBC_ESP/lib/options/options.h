#ifndef OPTIONS_HH
#define OPTIONS_HH

#include <stdint.h>
#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

//options are change only in stateTasks, and in handlingTask obviously
/*** 
 * IMPORTANT, be sure that program will not use set and get in this same time 
 * otherwise mutex is necessery
 * ***/
struct Options{
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
};

bool OPT_init(void);
//  MUTEX ??????
Options OPT_get_options_struct(void);

bool OPT_set_lora_freq(uint32_t lora_freq_khz);
uint32_t OPT_get_lora_freq(void);

bool OPT_set_countdown_begin_time(int32_t time_ms);
int32_t OPT_get_countdown_begin_time(void);

bool OPT_set_ignition_time(int32_t ignition_time);
int32_t OPT_get_ignition_time(void);

bool OPT_set_tank_min_pressure(uint8_t bar);
uint8_t OPT_get_tank_min_pressure(void);

bool OPT_set_flash_write(bool wite_to_flash);
bool OPT_get_flash_write(void);

bool OPT_set_force_launch(bool force_launch);
bool OPT_get_force_launch(void);

bool OPT_set_data_current_period(TickType_t data_period);
TickType_t OPT_get_data_current_period(void);

bool OPT_set_lora_current_period(TickType_t lora_period);
TickType_t OPT_get_lora_current_period(void);

bool OPT_set_flash_write_current_period(TickType_t flash_period);
TickType_t OPT_get_flash_write_current_period(void);

bool OPT_set_sd_write_current_period(TickType_t sd_write_period);
TickType_t OPT_get_sd_write_current_period(void);

bool OPT_create_lora_frame(char *buffer, size_t size);

#endif