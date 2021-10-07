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

    const float CdOverMach[102] = {
        0.0,
        0.7104690365906882,
        0.6965515159858551,
        0.6806878622581635,
        0.6639080411335089,
        0.6472420183377858,
        0.6317197595968889,
        0.6183712306367131,
        0.6078983596203444,
        0.5996909244596332,
        0.5928106655036209,
        0.5864844955146807,
        0.5806000169085104,
        0.57521000451414,
        0.5703417429804254,
        0.5659205562355301,
        0.5618462780274438,
        0.5580330687869629,
        0.5544523956761068,
        0.551090052539701,
        0.5479287792254202,
        0.5449390995923338,
        0.5420884835023612,
        0.5393562318878992,
        0.5367689699632597,
        0.534365154013233,
        0.5321722351606956,
        0.5301736438808686,
        0.5283418054870592,
        0.5266718065003031,
        0.5252493782725495,
        0.5241829133634759,
        0.52354072638934,
        0.523230820192721,
        0.5231211196727781,
        0.5230964317209174,
        0.5231090911975342,
        0.523128314955271,
        0.5231298575391382,
        0.5231156242636199,
        0.5230940581355681,
        0.5230718384004436,
        0.5230485892581423,
        0.5230221711471692,
        0.5229909376201348,
        0.5229552146860754,
        0.5229158214681338,
        0.5228733438656572,
        0.5228274348828109,
        0.5227775142999651,
        0.5227234175710527,
        0.5226666428442559,
        0.52260910394132,
        0.5225512622527955,
        0.5224877694444511,
        0.522411824750861,
        0.5223220003349747,
        0.5222383600732424,
        0.5221863407704901,
        0.5221713212739099,
        0.5221184486001611,
        0.521932811808269,
        0.5215511914350528,
        0.5210371339285036,
        0.5204858772144061,
        0.5199725327280476,
        0.5194917059427238,
        0.5190178758412332,
        0.51853089985288,
        0.5180321491929922,
        0.5175283735234041,
        0.5170249286987483,
        0.516521595344851,
        0.5160167602803372,
        0.515509003923699,
        0.5149976810928986,
        0.5144823402057649,
        0.5139631489486725,
        0.513442752082174,
        0.5129244136353672,
        0.5124080118150672,
        0.5118798815389588,
        0.5113229719024441,
        0.5107246163706476,
        0.510089685887582,
        0.5094274357669821,
        0.5088006604092067,
        0.508486310561107,
        0.508814876056158,
        0.5100560934715839,
        0.512236686359603,
        0.5153226250161831,
        0.5192806376265574,
        0.5240804839330198,
        0.5296926815671292,
        0.5361313529768853,
        0.5435850398760504,
        0.5522858887948271,
        0.5623123780564381,
        0.5731283131561858,
        0.584043831382392,
        0.5950154592003485,
    };
    // array for Cd values,
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