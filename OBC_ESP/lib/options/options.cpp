#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include "options.h"
#include "esp_log.h" 
#include "config.h"

#define ABS(x) ((x > 0) ? x : -x)

static struct{
    Options options;
    SemaphoreHandle_t options_mutex;
}opt = {
    .options = {
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
    },
    .options_mutex = NULL,
};

bool OPT_init(void) {
    opt.options.lora_freq_khz = LoRa_FREQUENCY_KHZ;
    opt.options.countdown_begin_time = -ABS(COUNTDOWN_TIME);
    opt.options.ignition_time = -ABS(IGNITION_TIME);
    opt.options.tank_min_pressure = TANK_MIN_PRESSURE;
    opt.options.flash_write = FLASH_WRITE;
    opt.options.force_launch = FORCE_LAUNCH;
    opt.options.data_current_period = DATA_PERIOD;
    opt.options.lora_current_period = IDLE_PERIOD;
    opt.options.flash_write_current_period = FLASH_LONG_PERIOD;
    opt.options.sd_write_current_period = IDLE_PERIOD;

    opt.options_mutex = xSemaphoreCreateMutex();
    assert(opt.options_mutex != NULL);
    if (opt.options_mutex == NULL) {
        return false;
    }

    return true;
}

Options OPT_get_options_struct(void) {
    return opt.options;
}

bool OPT_set_lora_freq(uint32_t lora_freq_khz) {
    if (lora_freq_khz < 1E5 || lora_freq_khz > 1E6) {
        return false;
    }
    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    opt.options.lora_freq_khz = lora_freq_khz;
    xSemaphoreGive(opt.options_mutex);
    return true;
}

uint32_t OPT_get_lora_freq(void) {
    uint32_t lora_freq_khz;
    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    lora_freq_khz = opt.options.lora_freq_khz;
    xSemaphoreGive(opt.options_mutex);
    return lora_freq_khz;
}

#define MAX_COUTDOWN_TIME -10000    // ms

bool OPT_set_countdown_begin_time(int32_t time_ms) {
    time_ms = ABS(time_ms);
    time_ms = -time_ms;

    assert(MAX_COUTDOWN_TIME < 0);
    assert(opt.options.ignition_time < 0);
    assert(time_ms < 0);

    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    if (time_ms > MAX_COUTDOWN_TIME && time_ms > opt.options.ignition_time) {
        xSemaphoreGive(opt.options_mutex);
        return false;
    }

    opt.options.countdown_begin_time = time_ms;
    xSemaphoreGive(opt.options_mutex);
    return true;
}

int32_t OPT_get_countdown_begin_time(void) {
    int32_t countdown_begin_time;
    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    countdown_begin_time = opt.options.countdown_begin_time;
    xSemaphoreGive(opt.options_mutex);

    return countdown_begin_time;
}

#define MAX_IGNIITION_TIME -500

bool OPT_set_ignition_time(int32_t ignition_time) {
    ignition_time = ABS(ignition_time);
    ignition_time = -ignition_time;

    assert(MAX_COUTDOWN_TIME < 0);
    assert(ignition_time < 0);

    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    if (ignition_time > MAX_IGNIITION_TIME || ignition_time <= opt.options.countdown_begin_time) {
        xSemaphoreGive(opt.options_mutex);
        return false;
    }

    opt.options.ignition_time = ignition_time;
    xSemaphoreGive(opt.options_mutex);
    return true;
}

int32_t OPT_get_ignition_time(void) {
    int32_t ignition_time;
    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    ignition_time = opt.options.ignition_time;
    xSemaphoreGive(opt.options_mutex);

    return ignition_time;
}

bool OPT_set_tank_min_pressure(uint8_t bar) {
    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    opt.options.tank_min_pressure = bar;
    xSemaphoreGive(opt.options_mutex);

    return true;
}

uint8_t OPT_get_tank_min_pressure(void) {
    uint8_t tank_min_pressure;
    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    tank_min_pressure = opt.options.tank_min_pressure;
    xSemaphoreGive(opt.options_mutex);

    return tank_min_pressure;
}

bool OPT_set_flash_write(bool flash_write) {
    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    opt.options.flash_write = flash_write;
    xSemaphoreGive(opt.options_mutex);

    return true;
}

bool OPT_get_flash_write(void) {
    bool flash_write;
    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    flash_write = opt.options.flash_write;
    xSemaphoreGive(opt.options_mutex);

    return flash_write;
}

bool OPT_set_force_launch(bool force_launch) {
    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    opt.options.force_launch = force_launch;
    xSemaphoreGive(opt.options_mutex);
    return true;
}

bool OPT_get_force_launch(void) {
    bool force_launch;
    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    force_launch = opt.options.force_launch;
    xSemaphoreGive(opt.options_mutex);
    return force_launch;
}

#define MIN_PERIOD 75

bool OPT_set_data_current_period(TickType_t data_period) {
    Serial.print("setting data period");
    Serial.println(data_period);
    if (data_period < MIN_PERIOD) {
        return false;
    }

    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    opt.options.data_current_period = data_period;
    xSemaphoreGive(opt.options_mutex);
    return true;
}


TickType_t OPT_get_data_current_period(void) {
    TickType_t data_current_period;
    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    data_current_period = opt.options.data_current_period;
    xSemaphoreGive(opt.options_mutex);
    return data_current_period;
}

bool OPT_set_lora_current_period(TickType_t lora_period) {
    if (lora_period < MIN_PERIOD) {
        return false;
    }

    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    opt.options.lora_current_period = lora_period;
    xSemaphoreGive(opt.options_mutex);
    return true;
}

TickType_t OPT_get_lora_current_period(void) {
    TickType_t lora_current_period;
    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    lora_current_period = opt.options.lora_current_period;
    xSemaphoreGive(opt.options_mutex);

    return lora_current_period;
}

bool OPT_set_flash_write_current_period(TickType_t flash_period) {
    if (flash_period < MIN_PERIOD) {
        return false;
    }

    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    opt.options.flash_write_current_period = flash_period;
    xSemaphoreGive(opt.options_mutex);
    return true;
}

TickType_t OPT_get_flash_write_current_period(void) {
    TickType_t flash_write_current_period;

    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    flash_write_current_period = opt.options.flash_write_current_period;
    xSemaphoreGive(opt.options_mutex);

    return flash_write_current_period;
}

bool OPT_set_sd_write_current_period(TickType_t sd_write_period) {
    if (sd_write_period < MIN_PERIOD)  {
        return false;
    }

    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    opt.options.sd_write_current_period = sd_write_period;
    xSemaphoreGive(opt.options_mutex);
    return true;
}

TickType_t OPT_get_sd_write_current_period(void) {
    TickType_t sd_write_current_period;
    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    sd_write_current_period = opt.options.sd_write_current_period;
    xSemaphoreGive(opt.options_mutex);

    return sd_write_current_period;
}

bool OPT_create_lora_frame(char *buffer, size_t size) {
    assert(buffer != NULL);
    assert(size > 0);
    if (buffer == NULL || size == 0) {
        return false;
    }

    Options options;
    xSemaphoreTake(opt.options_mutex, portMAX_DELAY);
    options = opt.options;
    xSemaphoreGive(opt.options_mutex);

    snprintf(buffer, size, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;",
        options.lora_freq_khz, options.countdown_begin_time, options.ignition_time,
        options.tank_min_pressure, options.flash_write, options.force_launch, options.data_current_period,
        options.lora_current_period, options.flash_write_current_period, options.sd_write_current_period);

    return true;
}
