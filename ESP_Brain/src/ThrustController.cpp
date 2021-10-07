#include "ThrustController.h"


float ThrustController::calculateTemperature(float height)
{
    return T0 - L * height;
}

/*! P = P0 * (1 - L*height/T0) ^ (GM/RL)
 *  https://en.wikipedia.org/wiki/Density_of_air#Variation_with_altitude\
 *  !!!! Pressure can also be taken from our measurements !!!!
 */
float ThrustController::calculatePressure(float height)
{
    /* read pressure from main comp,
       return pressure
    */
    return P0 * pow((1 - L * height / T0), GM_OVER_RL);
}

/*!
 *  https://en.wikipedia.org/wiki/Density_of_air#Variation_with_altitude
 */
float ThrustController::calculateAirDensity(float height)
{
    return calculatePressure(height) * M / (R * calculateTemperature(height));
}

/*!
 *  https://en.wikipedia.org/wiki/Mach_number#Calculation
 */
float ThrustController::calculateSpeedOfSound(float height)
{
    return pow(GAMMA * RSTAR * calculateTemperature(height), 0.5);
}

/*!
 *  https://en.wikipedia.org/wiki/Mach_number#Calculation
 */
float ThrustController::calculateMachNumber(float height, float velocity)
{
    return velocity / calculateSpeedOfSound(height);
}

float ThrustController::getCd(float machNumber)
{
    float machTimes100 = machNumber * 100.0;
    return CdOverMach[(int)machTimes100]; //conversion from float to int*100
}

float ThrustController::calculateDragForce(float height, float velocity)
{
    return calculateAirDensity(height) * pow(velocity, 2) * AREF * getCd(calculateMachNumber(height, velocity)) * 0.5;
}

/**********************************************************************************************/

void thrustControllerTask(void *arg) {

    ThrustController thrustController;

    // TODO!!!
}