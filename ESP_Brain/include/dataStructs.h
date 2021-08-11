#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <cstdint>

// Struktury pomocnicze:

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

struct UpustData {

    bool endStop1;
    bool endStop2;
    uint16_t potentiometer;
};

// Główna struktura na przechowywanie danych całej rakiety:

struct MainDataFrame {

    uint32_t timer_ms;
    float battery;

    PitotData pitotData = {};
    GPSData gps = {};
    UpustData upust = {};

    uint16_t halSensor[3];
    float tankPressure;

    float initialPressure = 1013;
    float pressure;
    float speed;
    float gForce;

    uint16_t espNowErrorCounter;
    uint16_t sdErrorCounter;
};

// Stany maszyny stanów:

enum States {

    INIT = 0,
    IDLE,
    FUELING,
    COUNTDOWN,
    ABORT,
    FLIGHT,
    FIRST_SEPAR,
    SECOND_SEPAR,
    GROUND
};

#endif