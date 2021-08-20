#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <cstdint>

// Stany zaworów:

enum ValveStates {

    CLOSED = 0,
    MOVING,
    OPEN
};

// Struktury pomocnicze:

struct PitotData {

    float staticPressure = -1;
    float dynamicPressure = -1;
    float temperature = -1;
    float altitude = -1;
    float speed = -1;
};

struct ValveData {

    uint8_t valveState = MOVING;
    uint16_t potentiometer = 2137;
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
    int8_t countdown = 32;
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