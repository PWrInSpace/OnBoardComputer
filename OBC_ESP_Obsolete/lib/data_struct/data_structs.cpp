#include "data_structs.h"
#include <Arduino.h>


static struct {
    PitotData       pitot;
    MainValveData   main_valve;
    TanWaData       tanwa;
    UpustValveData  upust_valve;
    RecoveryData    recovery;
    SlaveData       black_box;
    PayloadData     payload;
    MCB             mcb;
    SemaphoreHandle_t mutex;
}data;

bool DF_init(void) {
    memset(&data.pitot, 0, sizeof(data.pitot));
    memset(&data.main_valve, 0, sizeof(data.main_valve));
    memset(&data.tanwa, 0, sizeof(data.tanwa));
    memset(&data.upust_valve, 0, sizeof(data.upust_valve));
    memset(&data.recovery, 0, sizeof(data.recovery));
    memset(&data.black_box, 0, sizeof(data.black_box));
    memset(&data.payload, 0, sizeof(data.payload));
    memset(&data.mcb, 0, sizeof(data.mcb));

    data.mutex = NULL;
    data.mutex = xSemaphoreCreateMutex();
    assert(data.mutex != NULL);
    if (data.mutex == NULL) {
        return false;
    }

    return true;
}

void DF_set_connection_status(uint8_t connection_status) {
    xSemaphoreTake(data.mutex, portMAX_DELAY);
    data.mcb.connection_status = connection_status;
    xSemaphoreGive(data.mutex);
}

void DF_update_data_on_action(uint8_t state, uint32_t uptime, int32_t mission_timer) {
    xSemaphoreTake(data.mutex, portMAX_DELAY);
    data.mcb.state = state;
    data.mcb.uptime = uptime;
    data.mcb.mission_timer = mission_timer;
    xSemaphoreGive(data.mutex);
}

void DF_fill_pysd_struct(pysdmain_DataFrame* frame) {
    xSemaphoreTake(data.mutex, portMAX_DELAY);
    frame->pitot = data.pitot;
    frame->mainValve = data.main_valve;
    frame->tanWa = data.tanwa;
    frame->upustValve = data.upust_valve;
    frame->recovery = data.recovery.data;
    frame->blackBox = data.black_box;
    frame->payload = data.payload;
    frame->mcb = data.mcb;
    xSemaphoreGive(data.mutex);
}

void DF_set_mcb_data(MCB mcb) {
    xSemaphoreTake(data.mutex, portMAX_DELAY);
    memcpy(&data.mcb, &mcb, sizeof(mcb));
    xSemaphoreGive(data.mutex);
}

void DF_set_pitot_data(PitotData pitot) {
    xSemaphoreTake(data.mutex, portMAX_DELAY);
    memcpy(&data.pitot, &pitot, sizeof(pitot));
    Serial.print("Pitot data: ");
    Serial.print(pitot.vBat);
    Serial.print("\t");
    Serial.print(pitot.altitude);
    Serial.print("\t");
    Serial.println(pitot.speed);
    xSemaphoreGive(data.mutex);
}

void DF_set_main_valve_data(MainValveData main_valve) {
    xSemaphoreTake(data.mutex, portMAX_DELAY);
    memcpy(&data.main_valve, &main_valve, sizeof(main_valve));
    xSemaphoreGive(data.mutex);
}

uint8_t DF_get_main_valve_state(void) {
    uint8_t state;
    xSemaphoreTake(data.mutex, portMAX_DELAY);
    state = data.main_valve.valveState;
    xSemaphoreGive(data.mutex);

    return state;
}

uint8_t DF_get_upust_valve_state(void) {
    uint8_t state;
    xSemaphoreTake(data.mutex, portMAX_DELAY);
    state = data.upust_valve.valveState;
    xSemaphoreGive(data.mutex);

    return state;
}

void DF_set_tanwa_data(TanWaData tanwa) {
    xSemaphoreTake(data.mutex, portMAX_DELAY);
    memcpy(&data.tanwa, &tanwa, sizeof(tanwa));
    xSemaphoreGive(data.mutex);
}

void DF_set_upust_valve_data(UpustValveData upust) {
    xSemaphoreTake(data.mutex, portMAX_DELAY);
    memcpy(&data.upust_valve, &upust, sizeof(upust));
    xSemaphoreGive(data.mutex);
}

void DF_set_recovery_data(RecoveryData recovery) {
    xSemaphoreTake(data.mutex, portMAX_DELAY);
    memcpy(&data.recovery, &recovery, sizeof(recovery));
    xSemaphoreGive(data.mutex);
}

bool DF_get_first_stage_recovery_done(void) {
    bool first_stage_done = false;
    xSemaphoreTake(data.mutex, portMAX_DELAY);
    first_stage_done = data.recovery.data.firstStageDone;
    xSemaphoreGive(data.mutex);

    return first_stage_done;
}

bool DF_get_second_stage_recovery_done(void) {
    bool second_stage_done = false;
    xSemaphoreTake(data.mutex, portMAX_DELAY);
    second_stage_done = data.recovery.data.secondStageDone;
    xSemaphoreGive(data.mutex);

    return second_stage_done;
}

void DF_set_blackbox_data(SlaveData black_box) {
    xSemaphoreTake(data.mutex, portMAX_DELAY);
    memcpy(&data.black_box, &black_box, sizeof(black_box));
    xSemaphoreGive(data.mutex);
}

void DF_set_payload_data(PayloadData payload) {
    xSemaphoreTake(data.mutex, portMAX_DELAY);
    memcpy(&data.payload, &payload, sizeof(payload));
    xSemaphoreGive(data.mutex);
}

void DF_create_lora_frame(char* buffer, size_t size) {
    assert(buffer != NULL);
    assert(size != 0);
    char byte_data[4] = {};
    char data_buffer[150] = {};
    xSemaphoreTake(data.mutex, portMAX_DELAY);
    PitotData       pitot = data.pitot;
    MainValveData   main_valve = data.main_valve;
    TanWaData       tanwa = data.tanwa;
    UpustValveData  upust_valve = data.upust_valve;
    RecoveryData    recovery = data.recovery;
    SlaveData       black_box = data.black_box;
    PayloadData     payload = data.payload;
    MCB             mcb = data.mcb;
    xSemaphoreGive(data.mutex);
    //MCB
    snprintf(data_buffer, sizeof(data_buffer), "%d;%d;%d;%0.1f;%0.4f;%0.4f;%d;%d;%d;%0.1f;",
        mcb.state, mcb.mission_timer / 1000, mcb.disconnect_remaining_time / 1000,
        mcb.batteryVoltage, mcb.gps.latitude, mcb.gps.longitude, mcb.gps.satellites,
        mcb.gps.is_time_valid, (int)mcb.imu.altitude, mcb.temp_mcp); //11
    strcat(buffer, data_buffer);
    memset(data_buffer, 0 , sizeof(data_buffer));

    snprintf(data_buffer, sizeof(data_buffer), "%0.1f;%d;%d;",
        pitot.vBat, (int)pitot.altitude, (int)pitot.speed); //8
    strcat(buffer, data_buffer);
    memset(data_buffer, 0 , sizeof(data_buffer));

    snprintf(data_buffer, sizeof(data_buffer), "%0.1f;", main_valve.batteryVoltage); //5
    strcat(buffer, data_buffer);
    memset(data_buffer, 0 , sizeof(data_buffer));

    snprintf(data_buffer, sizeof(data_buffer), "%0.1f;%0.1f;%d;",
      upust_valve.batteryVoltage, upust_valve.tankPressure,
      upust_valve.thermistor); //9
    strcat(buffer, data_buffer);
    memset(data_buffer, 0 , sizeof(data_buffer));

    snprintf(data_buffer, sizeof(data_buffer), "%d;%0.1f;%d;%d;%d;%0.2f;%0.2f;%d;%d;",
      tanwa.tanWaState, tanwa.vbat, tanwa.ground_pressure, tanwa.igniterContinouityOne,
      tanwa.igniterContinouityTwo, tanwa.rocketWeight, tanwa.tankWeight,
      tanwa.armButton, tanwa.abortButton);//19
    strcat(buffer, data_buffer);
    memset(data_buffer, 0 , sizeof(data_buffer));

    snprintf(data_buffer, sizeof(data_buffer), "%d;%d;%0.1f;",
      payload.isRecording, payload.isRpiOn, payload.vBat);
    strcat(buffer, data_buffer);
    memset(data_buffer, 0 , sizeof(data_buffer));

    // Slaves waken up (from top of the rocket):
    memset(byte_data, 0, 4);
    byte_data[0] |= (pitot.wakenUp     << 0);
    byte_data[0] |= (payload.wakenUp   << 1);
    byte_data[0] |= (black_box.wakeUp   << 2);
    byte_data[0] |= (upust_valve.wakeUp << 3);
    byte_data[0] |= (main_valve.wakeUp  << 4);

    snprintf(data_buffer, sizeof(data_buffer), "%d;", byte_data[0]);
    strcat(buffer, data_buffer);
    memset(data_buffer, 0 , sizeof(data_buffer));


    snprintf(data_buffer, sizeof(data_buffer), "%d;", mcb.connection_status);
    strcat(buffer, data_buffer);
    memset(data_buffer, 0 , sizeof(data_buffer));

    memset(byte_data, 0, 4);
    // Valve states:
    byte_data[0] |= (main_valve.valveState  << 0);
    byte_data[0] |= (upust_valve.valveState << 2);
    byte_data[0] |= (tanwa.motor_one   << 4);

    byte_data[1] |= (tanwa.motor_two   << 0);
    byte_data[1] |= (tanwa.motor_three << 3);

    byte_data[2] |= (tanwa.motor_four   << 0);
    byte_data[2] |= (tanwa.motor_five   << 3);

    snprintf(data_buffer, sizeof(data_buffer), "%d;%d;%d;", byte_data[0], byte_data[1], byte_data[2]);
    strcat(buffer, data_buffer);
    memset(data_buffer, 0 , sizeof(data_buffer));

    //recovery first byte
    memset(byte_data, 0, 4);
    byte_data[0] |= (recovery.data.isArmed                << 6);
    byte_data[0] |= (recovery.data.firstStageContinouity  << 5);
    byte_data[0] |= (recovery.data.secondStageContinouity << 4);
    byte_data[0] |= (recovery.data.separationSwitch1      << 3);
    byte_data[0] |= (recovery.data.separationSwitch2      << 2);
    byte_data[0] |= (recovery.data.telemetrumFirstStage   << 1);
    byte_data[0] |= (recovery.data.telemetrumSecondStage  << 0);

    //recovery second byte  
    byte_data[1] |= (recovery.data.isTeleActive         << 6);
    byte_data[1] |= (recovery.data.altimaxFirstStage    << 5);
    byte_data[1] |= (recovery.data.altimaxSecondStage   << 4);
    byte_data[1] |= (recovery.data.apogemixFirstStage   << 3);
    byte_data[1] |= (recovery.data.apogemixSecondStage  << 2);
    byte_data[1] |= (recovery.data.firstStageDone       << 1);
    byte_data[1] |= (recovery.data.secondStageDone      << 0);

    snprintf(data_buffer, sizeof(data_buffer), "%d;%d;", byte_data[0], byte_data[1]);
    strcat(buffer, data_buffer);
    memset(data_buffer, 0 , sizeof(data_buffer));

}

void DF_create_sd_frame(char* buffer, size_t size) {
    char data_buffer[200];

    DF_create_mcb_frame(data_buffer, sizeof(data_buffer));
    strcat(buffer, data_buffer);
    memset(data_buffer, 0 , sizeof(data_buffer));

    DF_create_pitot_frame(data_buffer, sizeof(data_buffer));
    strcat(buffer, data_buffer);
    memset(data_buffer, 0 , sizeof(data_buffer));

    DF_create_main_valve_frame(data_buffer, sizeof(data_buffer));
    strcat(buffer, data_buffer);
    memset(data_buffer, 0 , sizeof(data_buffer));

    DF_create_tanwa_frame(data_buffer, sizeof(data_buffer));
    strcat(buffer, data_buffer);
    memset(data_buffer, 0 , sizeof(data_buffer));

    DF_create_upust_valve_frame(data_buffer, sizeof(data_buffer));
    strcat(buffer, data_buffer);
    memset(data_buffer, 0 , sizeof(data_buffer));

    DF_create_recovery_frame(data_buffer, sizeof(data_buffer));
    strcat(buffer, data_buffer);
    memset(data_buffer, 0 , sizeof(data_buffer));

    DF_create_blackbox_frame(data_buffer, sizeof(data_buffer));
    strcat(buffer, data_buffer);
    memset(data_buffer, 0 , sizeof(data_buffer));

    DF_create_payload_frame(data_buffer, sizeof(data_buffer));
    strcat(buffer, data_buffer);
    memset(data_buffer, 0 , sizeof(data_buffer));
}

// /***************************************/
// /*      THREAD UNSAFE FUNCTIONS        */
// /***************************************/

bool DF_create_mcb_frame(char *buffer, size_t size) {
    assert(buffer != NULL);
    assert(size > 0);
    if (buffer == NULL || size == 0) {
        return false;
    }

    // size_t wrote_data_size;
    // wrote_data_size = snprintf(buffer, size, "%d;%0.2f;%d;%0.4f;%0.4f;%0.2f;%d;%d;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;%0.2f;",
    //     data.mcb.state, data.mcb.batteryVoltage, data.mcb.watchdogResets,
    //     data.mcb.latitude, data.mcb.longitude, data.mcb.altitude, data.mcb.satellites, data.mcb.is_time_valid,
    //     data.mcb.temp_lp25, data.mcb.pressure, data.mcb.altitude, data.mcb.velocity,
    //     data.mcb.imu.[0], data.mcb.imu.[1], data.mcb.imu.[2], data.mcb.imu.[3],
    //     data.mcb.imu.[4], data.mcb.imu.[5], data.mcb.imu.[6], data.mcb.imu.[7],
    //     data.mcb.imu.[8], data.mcb.imu.[9], data.mcb.imu.[10], data.mcb.apogee);
    // assert(wrote_data_size < size);
    return true;
}

bool DF_create_pitot_frame(char *buffer, size_t size) {
    assert(buffer != NULL);
    assert(size > 0);
    if (buffer == NULL || size == 0) {
        return false;
    }

    // size_t wrote_data_size;
    // wrote_data_size = snprintf(buffer, size, "%d;%0.2f;%0.2f;%0.2f;%0.2f;%d;%d;%d;%d;%d;",
    //     data.pitot.wakenUp, data.pitot.vBat, data.pitot.statPress, 
    //     data.pitot.dynamicPress, data.pitot.temp, data.pitot.altitude,
    //     data.pitot.speed, data.pitot.apogee, data.pitot., 
    //     data.pitot.data_collected);//8
    // assert(wrote_data_size < size);
    return true;
}

bool DF_create_main_valve_frame(char *buffer, size_t size) {
    assert(buffer != NULL);
    assert(size > 0);
    if (buffer == NULL || size == 0) {
        return false;
    }

    size_t wrote_data_size;
    wrote_data_size = snprintf(buffer, size, "%d;%0.2f;%d;%0.2f;%0.2f;",
        data.main_valve.wakeUp, data.main_valve.batteryVoltage, data.main_valve.valveState,
        data.main_valve.thermocoupleOne, data.main_valve.thermocoupleTwo); //5
    assert(wrote_data_size < size);
    return true;
}

bool DF_create_tanwa_frame(char *buffer, size_t size) {
    assert(buffer != NULL);
    assert(size > 0);
    if (buffer == NULL || size == 0) {
        return false;
    }

    size_t wrote_data_size;
    snprintf(buffer, size, "%d;%0.2f;%d;%d;%d;%d;%d;%d;%d;%0.2f;%0.2f;%d;%d;%0.2f;%0.2f;%0.2f;%d;%d;%d;",
        data.tanwa.tanWaState, data.tanwa.vbat, data.tanwa.igniterContinouityOne,
        data.tanwa.igniterContinouityTwo, data.tanwa.motor_one, data.tanwa.motor_two,
        data.tanwa.motor_three, data.tanwa.motor_four, data.tanwa.motor_five,
        data.tanwa.rocketWeight, data.tanwa.tankWeight, data.tanwa.rocketWeightRaw, 
        data.tanwa.tankWeightRaw, data.tanwa.thermocouple_one, data.tanwa.thermocouple_two, 
        data.tanwa.thermocouple_three, data.tanwa.armButton, 
        data.tanwa.abortButton, data.tanwa.tankHeating); //19
    assert(wrote_data_size < size);
    return true;
}

bool DF_create_upust_valve_frame(char *buffer, size_t size) {
    assert(buffer != NULL);
    assert(size > 0);
    if (buffer == NULL || size == 0) {
        return false;
    }

    size_t wrote_data_size;
    snprintf(buffer, size, "%d;%0.2f;%d;%0.2f;%d;",
        data.upust_valve.wakeUp, data.upust_valve.batteryVoltage, data.upust_valve.valveState,
        data.upust_valve.tankPressure, data.upust_valve.thermistor);
    assert(wrote_data_size < size);
    return true;
}

bool DF_create_recovery_frame(char *buffer, size_t size) {
    assert(buffer != NULL);
    assert(size > 0);
    if (buffer == NULL || size == 0) {
        return false;
    }

    size_t wrote_data_size;
    snprintf(buffer, size, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;",
        data.recovery.data.isArmed, data.recovery.data.firstStageContinouity, data.recovery.data.secondStageContinouity,
        data.recovery.data.separationSwitch1, data.recovery.data.separationSwitch2,
        data.recovery.data.telemetrumFirstStage, data.recovery.data.telemetrumSecondStage,
        data.recovery.data.altimaxFirstStage, data.recovery.data.altimaxSecondStage,
        data.recovery.data.apogemixFirstStage, data.recovery.data.apogemixSecondStage,
        data.recovery.data.firstStageDone, data.recovery.data.secondStageDone,
        data.recovery.data.isTeleActive);
    assert(wrote_data_size < size);
    return true;
}

bool DF_create_blackbox_frame(char *buffer, size_t size) {
    assert(buffer != NULL);
    assert(size > 0);
    if (buffer == NULL || size == 0) {
        return false;
    }

    size_t wrote_data_size;
    snprintf(buffer, size, "%d;", data.black_box.wakeUp);
    assert(wrote_data_size < size);
    return true;
}

bool DF_create_payload_frame(char *buffer, size_t size) {
    assert(buffer != NULL);
    assert(size > 0);
    if (buffer == NULL || size == 0) {
        return false;
    }

    size_t wrote_data_size;
    snprintf(buffer, size, "%0.2f;%d;%d;%d;%d;",
        data.payload.vBat, data.payload.wakenUp, data.payload.isRecording,
        data.payload.data, data.payload.isRpiOn);
    assert(wrote_data_size < size);
    return true;
}

bool DF_all_devices_woken_up(void) {
    return data.main_valve.wakeUp &&
            data.upust_valve.wakeUp &&
            data.pitot.wakenUp &&
            data.payload.wakenUp;
}