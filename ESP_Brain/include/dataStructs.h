#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <cstdint>

// Stany zaworów:

enum ValveStates {

    CLOSED = 0,
    OPEN,
    MOVING
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

struct MaximumData {

    int apogee = 1;
    float maxSpeed = 1;
    float maxAcc = 1;
};

// Główna struktura na przechowywanie danych całej rakiety:

struct MainDataFrame {

    uint8_t rocketState;
    float battery = -1;

    PitotData pitotData = {};
    ValveData upust = {};
    ValveData mValve = {};
    
    float tankPressure = -1;

    float initialPressure = 2137;
    float pressure = -1;
    int altitude;
    float speed;
    float gForce;

    uint16_t espNowErrorCounter = 0;
    uint16_t sdErrorCounter = 0;

    uint16_t separationData = 65535;
    int8_t countdown = 8;

    bool forceSeparation;
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

#define FLIGHT_DATA_PERIOD  (50)
#define WAIT_DATA_PERIOD    (500)
#define END_DATA_PERIOD     (10000)

#endif