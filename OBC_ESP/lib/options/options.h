#ifndef OPTIONS_HH
#define OPTIONS_HH

#include <stdint.h>
#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "pysd_data_structs.h"

//options are change only in stateTasks, and in handlingTask obviously
/*** 
 * IMPORTANT, be sure that program will not use set and get in this same time 
 * otherwise mutex is necessery
 * ***/

bool OPT_init(void);
//  MUTEX ??????
Options OPT_get_options_struct(void);
void OPT_fill_pysd_struct(pysdmain_DataFrame *data);

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

bool OPT_set_data_current_period(uint32_t data_period);
uint32_t OPT_get_data_current_period(void);

bool OPT_set_lora_current_period(uint32_t lora_period);
uint32_t OPT_get_lora_current_period(void);

bool OPT_set_flash_write_current_period(uint32_t flash_period);
uint32_t OPT_get_flash_write_current_period(void);

bool OPT_set_sd_write_current_period(uint32_t sd_write_period);
uint32_t OPT_get_sd_write_current_period(void);

bool OPT_create_lora_frame(char *buffer, size_t size);

#endif