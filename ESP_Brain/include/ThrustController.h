#ifndef THRUST_CONTROLLER_H
#define THRUST_CONTROLLER_H

#include "dataStructs.h"
#include "queue.h"
#include <Arduino.h>

#include <cmath>
#define T0 288.15             // sea level standard temperature, can be changed to temperature on the launchpad, Kelvin
#define L 0.0065              // temperature lapse rate, kelvin/meter
#define G 9.80665             // gravitational acceleration, meter/second^2
#define R 8.31446             // ideal gas constant, Jule/(mol*Kelvin)
#define M 0.0289652           // molar mass of dry air, kilogram/mol
#define GM_OVER_RL 5.25593278 // GM/RL, constant and used in lots of derivations
#define GAMMA 1.4             // ratio of specific heat of a gas at a constant pressure to heat at a constant volume for air
#define RSTAR 287.058         // specific gas constant of air, = R/M, https://en.wikipedia.org/wiki/Gas_constant#Specific_gas_constant
#define AREF 0.028            // reference area of rocket
#define TIMESTEP 0.1
#define TIMESTEPSQ 0.01

class ThrustController{

public:

    float CdOverMach[101];  // array for Cd values,
                            // CdOverMach[1] contains Cd value for Mach 0.01, CdOverMach[2] for Mach 0.02 and so on

    float calculateTemperature(float height);
    float calculatePressure(float height);
    float calculateAirDensity(float height);
    float calculateSpeedOfSound(float height);
    float calculateMachNumber(float height, float velocity);
    float getCd(float machNumber);
    float calculateDragForce(float height, float velocity);
};

void thrustControllerTask(void* arg);

#endif