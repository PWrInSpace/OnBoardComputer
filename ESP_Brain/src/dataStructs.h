#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <cstdint>

struct PitotData {

    float staticPressure;
    float dynamicPressure;
    float altitude;
    float speed;
};

struct GPSData {

    float longitude;
    float laltitude;
    float altitude;
    int satellitesNumber;

};

struct MainDataFrame {

    uint32_t timer_ms;
    float battery;

    PitotData pitotData = {};
    GPSData gps = {};

    uint16_t halSensor[5];
    float tankPressure;

    float initialPressure = 1013;
    float pressure;
    float speed;
    float gForce;

    uint16_t espNowErrorCounter;
    uint16_t sdErrorCounter;
};

#endif