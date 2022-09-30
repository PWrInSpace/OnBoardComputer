#include <string.h>
#include "pysd/gen_pysd.h"

/**
 * @brief Get header size
 *
 * @param new_line_ending end string with new line character
 * @return size_t header size
 */
size_t pysd_get_header_size(bool new_line_ending) {
    size_t string_size;
    if (new_line_ending == true) {
        string_size = snprintf(NULL, 0, "mcb state;mcb uptime;mcb mission_timer;mcb connection_status;mcb imuData[11];mcb batteryVoltage;mcb latitude;mcb longitude;mcb gps_altitude;mcb satellites;mcb is_time_valid;mcb temp_mcp;mcb temp_lp25;mcb pressure;mcb altitude;mcb velocity;mcb watchdogResets;mcb ignition;mcb apogee;pitot wakenUp;pitot vBat;pitot statPress;pitot dynamicPress;pitot temp;pitot altitude;pitot speed;pitot apogee;pitot isRecording;pitot data_collected;mainValve wakeUp;mainValve valveState;mainValve thermocouple[2];mainValve batteryVoltage;tanWa tanWaState;tanWa tankHeating;tanWa abortButton;tanWa armButton;tanWa igniterContinouity[2];tanWa vbat;tanWa motorState[5];tanWa rocketWeight;tanWa tankWeight;tanWa rocketWeightRaw;tanWa tankWeightRaw;tanWa thermocouple[3];upustValve wakeUp;upustValve valveState;upustValve thermistor;upustValve tankPressure;upustValve batteryVoltage;recovery isArmed;recovery firstStageContinouity;recovery secondStageContinouity;recovery separationSwitch1;recovery separationSwitch2;recovery telemetrumFirstStage;recovery telemetrumSecondStage;recovery altimaxFirstStage;recovery altimaxSecondStage;recovery apogemixFirstStage;recovery apogemixSecondStage;recovery firstStageDone;recovery secondStageDone;recovery isTeleActive;blackBox wakeUp;blackBox batteryVoltage;payload wakenUp;payload isRecording;payload data;payload vBat;payload isRpiOn;\n");
    } else {
        string_size = snprintf(NULL, 0, "mcb state;mcb uptime;mcb mission_timer;mcb connection_status;mcb imuData[11];mcb batteryVoltage;mcb latitude;mcb longitude;mcb gps_altitude;mcb satellites;mcb is_time_valid;mcb temp_mcp;mcb temp_lp25;mcb pressure;mcb altitude;mcb velocity;mcb watchdogResets;mcb ignition;mcb apogee;pitot wakenUp;pitot vBat;pitot statPress;pitot dynamicPress;pitot temp;pitot altitude;pitot speed;pitot apogee;pitot isRecording;pitot data_collected;mainValve wakeUp;mainValve valveState;mainValve thermocouple[2];mainValve batteryVoltage;tanWa tanWaState;tanWa tankHeating;tanWa abortButton;tanWa armButton;tanWa igniterContinouity[2];tanWa vbat;tanWa motorState[5];tanWa rocketWeight;tanWa tankWeight;tanWa rocketWeightRaw;tanWa tankWeightRaw;tanWa thermocouple[3];upustValve wakeUp;upustValve valveState;upustValve thermistor;upustValve tankPressure;upustValve batteryVoltage;recovery isArmed;recovery firstStageContinouity;recovery secondStageContinouity;recovery separationSwitch1;recovery separationSwitch2;recovery telemetrumFirstStage;recovery telemetrumSecondStage;recovery altimaxFirstStage;recovery altimaxSecondStage;recovery apogemixFirstStage;recovery apogemixSecondStage;recovery firstStageDone;recovery secondStageDone;recovery isTeleActive;blackBox wakeUp;blackBox batteryVoltage;payload wakenUp;payload isRecording;payload data;payload vBat;payload isRpiOn;");
    }

    return string_size;
}

/**
 * @brief Get size of sd data frame
 *
 * @param pysd_main
 * @param new_line_ending end string with new line character
 * @return size_t frame size
 */
size_t pysd_get_sd_frame_size(pysdmain_DataFrame pysd_main, bool new_line_ending) {
    size_t string_size;
    if (new_line_ending == true) {
        string_size = snprintf(NULL, 0, "%d;%lu;%d;%d;%f;%f;%f;%f;%f;%d;%d;%f;%f;%f;%f;%f;%d;%d;%f;%d;%f;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%f;%f;%d;%d;%d;%d;%d;%f;%d;%f;%f;%lu;%lu;%f;%d;%d;%d;%f;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%f;%d;%d;%d;%f;%d;\n", pysd_main.mcb.state, pysd_main.mcb.uptime, pysd_main.mcb.mission_timer, pysd_main.mcb.connection_status, pysd_main.mcb.imuData[11], pysd_main.mcb.batteryVoltage, pysd_main.mcb.latitude, pysd_main.mcb.longitude, pysd_main.mcb.gps_altitude, pysd_main.mcb.satellites, pysd_main.mcb.is_time_valid, pysd_main.mcb.temp_mcp, pysd_main.mcb.temp_lp25, pysd_main.mcb.pressure, pysd_main.mcb.altitude, pysd_main.mcb.velocity, pysd_main.mcb.watchdogResets, pysd_main.mcb.ignition, pysd_main.mcb.apogee, pysd_main.pitot.wakenUp, pysd_main.pitot.vBat, pysd_main.pitot.statPress, pysd_main.pitot.dynamicPress, pysd_main.pitot.temp, pysd_main.pitot.altitude, pysd_main.pitot.speed, pysd_main.pitot.apogee, pysd_main.pitot.isRecording, pysd_main.pitot.data_collected, pysd_main.mainValve.wakeUp, pysd_main.mainValve.valveState, pysd_main.mainValve.thermocouple[2], pysd_main.mainValve.batteryVoltage, pysd_main.tanWa.tanWaState, pysd_main.tanWa.tankHeating, pysd_main.tanWa.abortButton, pysd_main.tanWa.armButton, pysd_main.tanWa.igniterContinouity[2], pysd_main.tanWa.vbat, pysd_main.tanWa.motorState[5], pysd_main.tanWa.rocketWeight, pysd_main.tanWa.tankWeight, pysd_main.tanWa.rocketWeightRaw, pysd_main.tanWa.tankWeightRaw, pysd_main.tanWa.thermocouple[3], pysd_main.upustValve.wakeUp, pysd_main.upustValve.valveState, pysd_main.upustValve.thermistor, pysd_main.upustValve.tankPressure, pysd_main.upustValve.batteryVoltage, pysd_main.recovery.isArmed, pysd_main.recovery.firstStageContinouity, pysd_main.recovery.secondStageContinouity, pysd_main.recovery.separationSwitch1, pysd_main.recovery.separationSwitch2, pysd_main.recovery.telemetrumFirstStage, pysd_main.recovery.telemetrumSecondStage, pysd_main.recovery.altimaxFirstStage, pysd_main.recovery.altimaxSecondStage, pysd_main.recovery.apogemixFirstStage, pysd_main.recovery.apogemixSecondStage, pysd_main.recovery.firstStageDone, pysd_main.recovery.secondStageDone, pysd_main.recovery.isTeleActive, pysd_main.blackBox.wakeUp, pysd_main.blackBox.batteryVoltage, pysd_main.payload.wakenUp, pysd_main.payload.isRecording, pysd_main.payload.data, pysd_main.payload.vBat, pysd_main.payload.isRpiOn);
    } else {
        string_size = snprintf(NULL, 0, "%d;%lu;%d;%d;%f;%f;%f;%f;%f;%d;%d;%f;%f;%f;%f;%f;%d;%d;%f;%d;%f;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%f;%f;%d;%d;%d;%d;%d;%f;%d;%f;%f;%lu;%lu;%f;%d;%d;%d;%f;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%f;%d;%d;%d;%f;%d;", pysd_main.mcb.state, pysd_main.mcb.uptime, pysd_main.mcb.mission_timer, pysd_main.mcb.connection_status, pysd_main.mcb.imuData[11], pysd_main.mcb.batteryVoltage, pysd_main.mcb.latitude, pysd_main.mcb.longitude, pysd_main.mcb.gps_altitude, pysd_main.mcb.satellites, pysd_main.mcb.is_time_valid, pysd_main.mcb.temp_mcp, pysd_main.mcb.temp_lp25, pysd_main.mcb.pressure, pysd_main.mcb.altitude, pysd_main.mcb.velocity, pysd_main.mcb.watchdogResets, pysd_main.mcb.ignition, pysd_main.mcb.apogee, pysd_main.pitot.wakenUp, pysd_main.pitot.vBat, pysd_main.pitot.statPress, pysd_main.pitot.dynamicPress, pysd_main.pitot.temp, pysd_main.pitot.altitude, pysd_main.pitot.speed, pysd_main.pitot.apogee, pysd_main.pitot.isRecording, pysd_main.pitot.data_collected, pysd_main.mainValve.wakeUp, pysd_main.mainValve.valveState, pysd_main.mainValve.thermocouple[2], pysd_main.mainValve.batteryVoltage, pysd_main.tanWa.tanWaState, pysd_main.tanWa.tankHeating, pysd_main.tanWa.abortButton, pysd_main.tanWa.armButton, pysd_main.tanWa.igniterContinouity[2], pysd_main.tanWa.vbat, pysd_main.tanWa.motorState[5], pysd_main.tanWa.rocketWeight, pysd_main.tanWa.tankWeight, pysd_main.tanWa.rocketWeightRaw, pysd_main.tanWa.tankWeightRaw, pysd_main.tanWa.thermocouple[3], pysd_main.upustValve.wakeUp, pysd_main.upustValve.valveState, pysd_main.upustValve.thermistor, pysd_main.upustValve.tankPressure, pysd_main.upustValve.batteryVoltage, pysd_main.recovery.isArmed, pysd_main.recovery.firstStageContinouity, pysd_main.recovery.secondStageContinouity, pysd_main.recovery.separationSwitch1, pysd_main.recovery.separationSwitch2, pysd_main.recovery.telemetrumFirstStage, pysd_main.recovery.telemetrumSecondStage, pysd_main.recovery.altimaxFirstStage, pysd_main.recovery.altimaxSecondStage, pysd_main.recovery.apogemixFirstStage, pysd_main.recovery.apogemixSecondStage, pysd_main.recovery.firstStageDone, pysd_main.recovery.secondStageDone, pysd_main.recovery.isTeleActive, pysd_main.blackBox.wakeUp, pysd_main.blackBox.batteryVoltage, pysd_main.payload.wakenUp, pysd_main.payload.isRecording, pysd_main.payload.data, pysd_main.payload.vBat, pysd_main.payload.isRpiOn);
    }

    return string_size;
}

/**
 * @brief Fill buffer with struct variables names
 * 
 * @param buffer pointer to buffer
 * @param size buffer size
 * @param pysdmain_DataFrame struct with data
 * @param new_line_ending end string with new line character
 * @return size_t size of wrote data, return 0 in case of failure
 */
size_t pysd_create_header(char *buffer, size_t size, bool new_line_ending) {
    size_t header_size;
    header_size = pysd_get_header_size(new_line_ending);

    if (header_size > size) {
        return false;
    }

    if (new_line_ending == true) {
        snprintf(buffer, size, "mcb state;mcb uptime;mcb mission_timer;mcb connection_status;mcb imuData[11];mcb batteryVoltage;mcb latitude;mcb longitude;mcb gps_altitude;mcb satellites;mcb is_time_valid;mcb temp_mcp;mcb temp_lp25;mcb pressure;mcb altitude;mcb velocity;mcb watchdogResets;mcb ignition;mcb apogee;pitot wakenUp;pitot vBat;pitot statPress;pitot dynamicPress;pitot temp;pitot altitude;pitot speed;pitot apogee;pitot isRecording;pitot data_collected;mainValve wakeUp;mainValve valveState;mainValve thermocouple[2];mainValve batteryVoltage;tanWa tanWaState;tanWa tankHeating;tanWa abortButton;tanWa armButton;tanWa igniterContinouity[2];tanWa vbat;tanWa motorState[5];tanWa rocketWeight;tanWa tankWeight;tanWa rocketWeightRaw;tanWa tankWeightRaw;tanWa thermocouple[3];upustValve wakeUp;upustValve valveState;upustValve thermistor;upustValve tankPressure;upustValve batteryVoltage;recovery isArmed;recovery firstStageContinouity;recovery secondStageContinouity;recovery separationSwitch1;recovery separationSwitch2;recovery telemetrumFirstStage;recovery telemetrumSecondStage;recovery altimaxFirstStage;recovery altimaxSecondStage;recovery apogemixFirstStage;recovery apogemixSecondStage;recovery firstStageDone;recovery secondStageDone;recovery isTeleActive;blackBox wakeUp;blackBox batteryVoltage;payload wakenUp;payload isRecording;payload data;payload vBat;payload isRpiOn;\n");
    } else {
        snprintf(buffer, size, "mcb state;mcb uptime;mcb mission_timer;mcb connection_status;mcb imuData[11];mcb batteryVoltage;mcb latitude;mcb longitude;mcb gps_altitude;mcb satellites;mcb is_time_valid;mcb temp_mcp;mcb temp_lp25;mcb pressure;mcb altitude;mcb velocity;mcb watchdogResets;mcb ignition;mcb apogee;pitot wakenUp;pitot vBat;pitot statPress;pitot dynamicPress;pitot temp;pitot altitude;pitot speed;pitot apogee;pitot isRecording;pitot data_collected;mainValve wakeUp;mainValve valveState;mainValve thermocouple[2];mainValve batteryVoltage;tanWa tanWaState;tanWa tankHeating;tanWa abortButton;tanWa armButton;tanWa igniterContinouity[2];tanWa vbat;tanWa motorState[5];tanWa rocketWeight;tanWa tankWeight;tanWa rocketWeightRaw;tanWa tankWeightRaw;tanWa thermocouple[3];upustValve wakeUp;upustValve valveState;upustValve thermistor;upustValve tankPressure;upustValve batteryVoltage;recovery isArmed;recovery firstStageContinouity;recovery secondStageContinouity;recovery separationSwitch1;recovery separationSwitch2;recovery telemetrumFirstStage;recovery telemetrumSecondStage;recovery altimaxFirstStage;recovery altimaxSecondStage;recovery apogemixFirstStage;recovery apogemixSecondStage;recovery firstStageDone;recovery secondStageDone;recovery isTeleActive;blackBox wakeUp;blackBox batteryVoltage;payload wakenUp;payload isRecording;payload data;payload vBat;payload isRpiOn;");
    }

    return header_size;
}

/**
 * @brief Fill buffer with struct data
 *
 * @param buffer pointer to buffer
 * @param size buffer size
 * @param ...pysd_main... struct with data
 * @param new_line_ending end string with new line character
 * @return size_t size of wrote data, return 0 in case of failure
 */
size_t pysd_create_sd_frame(char *buffer, size_t size, pysdmain_DataFrame pysd_main, bool new_line_ending) {
    size_t frame_size;
    frame_size = pysd_get_sd_frame_size(pysd_main, new_line_ending);

    if (frame_size > size) {
        return 0;
    }

    if (new_line_ending == true) {
        snprintf(buffer, size, "%d;%lu;%d;%d;%f;%f;%f;%f;%f;%d;%d;%f;%f;%f;%f;%f;%d;%d;%f;%d;%f;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%f;%f;%d;%d;%d;%d;%d;%f;%d;%f;%f;%lu;%lu;%f;%d;%d;%d;%f;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%f;%d;%d;%d;%f;%d;\n", pysd_main.mcb.state, pysd_main.mcb.uptime, pysd_main.mcb.mission_timer, pysd_main.mcb.connection_status, pysd_main.mcb.imuData[11], pysd_main.mcb.batteryVoltage, pysd_main.mcb.latitude, pysd_main.mcb.longitude, pysd_main.mcb.gps_altitude, pysd_main.mcb.satellites, pysd_main.mcb.is_time_valid, pysd_main.mcb.temp_mcp, pysd_main.mcb.temp_lp25, pysd_main.mcb.pressure, pysd_main.mcb.altitude, pysd_main.mcb.velocity, pysd_main.mcb.watchdogResets, pysd_main.mcb.ignition, pysd_main.mcb.apogee, pysd_main.pitot.wakenUp, pysd_main.pitot.vBat, pysd_main.pitot.statPress, pysd_main.pitot.dynamicPress, pysd_main.pitot.temp, pysd_main.pitot.altitude, pysd_main.pitot.speed, pysd_main.pitot.apogee, pysd_main.pitot.isRecording, pysd_main.pitot.data_collected, pysd_main.mainValve.wakeUp, pysd_main.mainValve.valveState, pysd_main.mainValve.thermocouple[2], pysd_main.mainValve.batteryVoltage, pysd_main.tanWa.tanWaState, pysd_main.tanWa.tankHeating, pysd_main.tanWa.abortButton, pysd_main.tanWa.armButton, pysd_main.tanWa.igniterContinouity[2], pysd_main.tanWa.vbat, pysd_main.tanWa.motorState[5], pysd_main.tanWa.rocketWeight, pysd_main.tanWa.tankWeight, pysd_main.tanWa.rocketWeightRaw, pysd_main.tanWa.tankWeightRaw, pysd_main.tanWa.thermocouple[3], pysd_main.upustValve.wakeUp, pysd_main.upustValve.valveState, pysd_main.upustValve.thermistor, pysd_main.upustValve.tankPressure, pysd_main.upustValve.batteryVoltage, pysd_main.recovery.isArmed, pysd_main.recovery.firstStageContinouity, pysd_main.recovery.secondStageContinouity, pysd_main.recovery.separationSwitch1, pysd_main.recovery.separationSwitch2, pysd_main.recovery.telemetrumFirstStage, pysd_main.recovery.telemetrumSecondStage, pysd_main.recovery.altimaxFirstStage, pysd_main.recovery.altimaxSecondStage, pysd_main.recovery.apogemixFirstStage, pysd_main.recovery.apogemixSecondStage, pysd_main.recovery.firstStageDone, pysd_main.recovery.secondStageDone, pysd_main.recovery.isTeleActive, pysd_main.blackBox.wakeUp, pysd_main.blackBox.batteryVoltage, pysd_main.payload.wakenUp, pysd_main.payload.isRecording, pysd_main.payload.data, pysd_main.payload.vBat, pysd_main.payload.isRpiOn);
    } else {
        snprintf(buffer, size, "%d;%lu;%d;%d;%f;%f;%f;%f;%f;%d;%d;%f;%f;%f;%f;%f;%d;%d;%f;%d;%f;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%f;%f;%d;%d;%d;%d;%d;%f;%d;%f;%f;%lu;%lu;%f;%d;%d;%d;%f;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%f;%d;%d;%d;%f;%d;\n", pysd_main.mcb.state, pysd_main.mcb.uptime, pysd_main.mcb.mission_timer, pysd_main.mcb.connection_status, pysd_main.mcb.imuData[11], pysd_main.mcb.batteryVoltage, pysd_main.mcb.latitude, pysd_main.mcb.longitude, pysd_main.mcb.gps_altitude, pysd_main.mcb.satellites, pysd_main.mcb.is_time_valid, pysd_main.mcb.temp_mcp, pysd_main.mcb.temp_lp25, pysd_main.mcb.pressure, pysd_main.mcb.altitude, pysd_main.mcb.velocity, pysd_main.mcb.watchdogResets, pysd_main.mcb.ignition, pysd_main.mcb.apogee, pysd_main.pitot.wakenUp, pysd_main.pitot.vBat, pysd_main.pitot.statPress, pysd_main.pitot.dynamicPress, pysd_main.pitot.temp, pysd_main.pitot.altitude, pysd_main.pitot.speed, pysd_main.pitot.apogee, pysd_main.pitot.isRecording, pysd_main.pitot.data_collected, pysd_main.mainValve.wakeUp, pysd_main.mainValve.valveState, pysd_main.mainValve.thermocouple[2], pysd_main.mainValve.batteryVoltage, pysd_main.tanWa.tanWaState, pysd_main.tanWa.tankHeating, pysd_main.tanWa.abortButton, pysd_main.tanWa.armButton, pysd_main.tanWa.igniterContinouity[2], pysd_main.tanWa.vbat, pysd_main.tanWa.motorState[5], pysd_main.tanWa.rocketWeight, pysd_main.tanWa.tankWeight, pysd_main.tanWa.rocketWeightRaw, pysd_main.tanWa.tankWeightRaw, pysd_main.tanWa.thermocouple[3], pysd_main.upustValve.wakeUp, pysd_main.upustValve.valveState, pysd_main.upustValve.thermistor, pysd_main.upustValve.tankPressure, pysd_main.upustValve.batteryVoltage, pysd_main.recovery.isArmed, pysd_main.recovery.firstStageContinouity, pysd_main.recovery.secondStageContinouity, pysd_main.recovery.separationSwitch1, pysd_main.recovery.separationSwitch2, pysd_main.recovery.telemetrumFirstStage, pysd_main.recovery.telemetrumSecondStage, pysd_main.recovery.altimaxFirstStage, pysd_main.recovery.altimaxSecondStage, pysd_main.recovery.apogemixFirstStage, pysd_main.recovery.apogemixSecondStage, pysd_main.recovery.firstStageDone, pysd_main.recovery.secondStageDone, pysd_main.recovery.isTeleActive, pysd_main.blackBox.wakeUp, pysd_main.blackBox.batteryVoltage, pysd_main.payload.wakenUp, pysd_main.payload.isRecording, pysd_main.payload.data, pysd_main.payload.vBat, pysd_main.payload.isRpiOn);
    }

    return frame_size;
}