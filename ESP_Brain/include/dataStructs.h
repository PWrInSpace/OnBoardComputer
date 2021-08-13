#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <cstdint>

// Struktury pomocnicze:

struct PitotData {

    float staticPressure;
    float dynamicPressure;
    float altitude;
    float speed;
    float temperature;
};

struct ValveData {

    bool endStop1;
    bool endStop2;
    uint16_t potentiometer;
};

// Główna struktura na przechowywanie danych całej rakiety:

struct MainDataFrame {

    uint8_t rocketState;
    float battery;

    PitotData pitotData = {};
    ValveData upust = {};
    ValveData mValve = {};
    
    float tankPressure;

    float initialPressure = 1013;
    float pressure;
    float altitude;
    float speed;
    float gForce;

    uint16_t espNowErrorCounter = 0;
    uint16_t sdErrorCounter = 0;

    uint8_t separationData = 0;
    int8_t countdown = 45;
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