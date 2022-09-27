#ifndef DATA_STRUCTS_HH
#define DATA_STRUCTS_HH

#include <stdio.h>
#include <cstdint>
#include "gen_pysd.h"

typedef struct {
    float latitude;
    float longitude;
    float altitude;
    uint32_t satellites;
    bool is_time_valid;
} gps_data_t;

typedef struct {
    float pressure;
    float temperature;
} lps25hb_data_t;

typedef float mcp9808_data_t;

bool DF_init(void);
void DF_update_data_on_action(uint8_t state, uint32_t uptime, uint32_t mission_timer);
void DF_set_connection_status(uint8_t connection_status);
void DF_fill_pysd_struct(pysdmain_DataFrame* frame);
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