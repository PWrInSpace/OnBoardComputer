#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "options.h"
#include "esp_log.h" 

#define ABS(x) ((x > 0) ? x : -x)

static Options opt = {
    .lora_freq_khz = LoRa_FREQUENCY_KHZ,
    .countdown_begin_time = -ABS(COUNTDOWN_TIME),
    .ignition_time = -ABS(IGNITION_TIME),
    .tank_min_pressure = TANK_MIN_PRESSURE,
    .flash_write = FLASH_WRITE,
    .force_launch = FORCE_LAUNCH,
    .data_current_period = DATA_PERIOD,
    .lora_current_period = IDLE_PERIOD,
    .flash_write_current_period = FLASH_LONG_PERIOD,
    .sd_write_current_period = IDLE_PERIOD,
};

//add init to check if options are correct

Options OPT_get_options_struct(void) {
    return opt;
}

bool OPT_set_lora_freq(uint32_t lora_freq_khz) {
    if (lora_freq_khz < 1E5 || lora_freq_khz > 1E6) {
        return false;
    }

    opt.lora_freq_khz = lora_freq_khz;
    return true;
}

uint32_t OPT_get_lora_freq(void) {
    return opt.lora_freq_khz;
}

#define MAX_COUTDOWN_TIME -10000    // ms

bool OPT_set_countdown_begin_time(int32_t time_ms) {
    time_ms = ABS(time_ms);
    time_ms = -time_ms;
    
    assert(MAX_COUTDOWN_TIME < 0);
    assert(opt.ignition_time < 0);
    assert(time_ms < 0);
    if (time_ms > MAX_COUTDOWN_TIME && time_ms > opt.ignition_time) {
        return false;
    }

    opt.countdown_begin_time = time_ms;
    return true;
}

int32_t OPT_get_countdown_begin_time(void) {
    return opt.countdown_begin_time;
}

#define MAX_IGNIITION_TIME -500

bool OPT_set_ignition_time(int32_t ignition_time) {
    ignition_time = ABS(ignition_time);
    ignition_time = -ignition_time;

    assert(MAX_COUTDOWN_TIME < 0);
    assert(ignition_time < 0);
    if (ignition_time > MAX_IGNIITION_TIME || ignition_time <= opt.countdown_begin_time) {
        return false;
    }

    opt.ignition_time = ignition_time;
    return true;
}

int32_t OPT_get_ignition_time(void) {
    return opt.ignition_time;
}

bool OPT_set_tank_min_pressure(uint8_t bar) {
    opt.tank_min_pressure = bar;
    return true;
}

uint8_t OPT_get_tank_min_pressure(void) {
    return opt.tank_min_pressure;
}

bool OPT_set_flash_write(bool flash_write) {
    opt.flash_write = flash_write;
    return true;
}

bool OPT_get_flash_write(void) {
    return opt.flash_write;
}

bool OPT_set_force_launch(bool force_launch) {
    opt.force_launch = force_launch;
    return true;
}

bool OPT_get_force_launch(void) {
    return opt.force_launch;
}

#define MIN_PERIOD 75

bool OPT_set_data_current_period(TickType_t data_period) {
    if (data_period < MIN_PERIOD) {
        return false;
    }
    opt.data_current_period = data_period;
    return true;
}

TickType_t OPT_get_data_current_period(void) {
    return opt.data_current_period;
}

bool OPT_set_lora_current_period(TickType_t lora_period) {
    if (lora_period < MIN_PERIOD) {
        return false;
    }
    opt.lora_current_period = lora_period;
    return true;
}

TickType_t OPT_get_lora_current_period(void) {
    return opt.lora_current_period;
}

bool OPT_set_flash_write_current_period(TickType_t flash_period) {
    if (flash_period < MIN_PERIOD) {
        return false;
    }
    opt.flash_write_current_period = flash_period;
    return true;
}

TickType_t OPT_get_flash_write_current_period(void) {
    return opt.flash_write_current_period;
}

bool OPT_set_sd_write_current_period(TickType_t sd_write_period) {
    if (sd_write_period < MIN_PERIOD)  {
        return false;
    }

    opt.sd_write_current_period = sd_write_period;
    return true;
}

TickType_t OPT_get_sd_write_current_period(void) {
    return opt.sd_write_current_period;
}
