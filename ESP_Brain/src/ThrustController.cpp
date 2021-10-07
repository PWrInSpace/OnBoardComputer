#include "ThrustController.h"

extern MainDataFrame mainDataFrame;
extern Queue queue;

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
    return mainDataFrame.initialPressure * pow((1 - L * height / T0), GM_OVER_RL);
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
    int i;

    /*std::ifstream flight;
    flight.open("lot.txt");*/
    float flightData[2][400];
    /*
    for (i = 0; i < 400; ++i)
    {
        flight >> flightData[0][i];
        flight >> flightData[1][i];
        std::cout << flightData[0][i] << " " << flightData[1][i] << std::endl;
    }*/

    while(mainDataFrame.rocketState != FLIGHT) {
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }

    // MAIN PROGRAM STARTS HERE //
    bool running;   // for the while loop
    /* 
     * velocity, height, dragForce - self explanatory
     * simTime - time when data was taken and simulation began
     * thrustEndTime - duration of engine working
     * simHeight - height in timestamp n-1 and n respectively
     * rocketMass - mass with motors, without propelant
     * propellantMass - mass of propellant at the time of launch
     * allMass - rocketMass + mass of propellant in time t
     */
    float velocity, height, dragForce, simTime, thrustEndTime = 3.423, simHeight[2], rocketMass = 13.141, propellantMass = 2.500, allMass; // mass in kg
    for (i = 1; i < 400; ++i)
    {
        height = flightData[1][i];
        velocity = (flightData[1][i] - flightData[1][i - 1]) / 0.1; // v = (h1 - h0)/dt

        running = 1;

        simTime = flightData[0][i];
        simHeight[1] = flightData[1][i];
        simHeight[0] = flightData[1][i - 1];
        if (simTime < thrustEndTime)
            allMass = rocketMass + propellantMass * ((thrustEndTime - simTime) / thrustEndTime);
        else allMass = rocketMass;
        while (running)
        {
            velocity = (simHeight[1] - simHeight[0]) / TIMESTEP;
            if (velocity < 0)   
            {
                running = 0;
                if (simHeight[1] > 3000.0)
                    queue.push(String("R4MC;ThrustControllerCloseRequest\n"));
            }

            if (mainDataFrame.rocketState != FLIGHT)
                running = 0;

            dragForce = thrustController.calculateDragForce(simHeight[1], velocity);
            simHeight[0] = simHeight[1];    // height in t(n) prepare for next step
            simHeight[1] = simHeight[1] + velocity * TIMESTEP - 4.9 * TIMESTEPSQ - dragForce / allMass * TIMESTEPSQ * 0.5; // height in t(n+1)
            simTime += TIMESTEP;   // increase simTime

            vTaskDelay(1 / portTICK_PERIOD_MS);
        }

    }
}