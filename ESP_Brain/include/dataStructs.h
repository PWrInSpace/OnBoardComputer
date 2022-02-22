#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <cstdint>
#include <Arduino.h>

// Struktura do zapisu:

struct DataFrame {

	uint32_t time_ms;
	float batteryV;
	float tankPressure;
	float gpsLatitude;
	float gpsLongitude;
	float gpsAltitude;
	uint8_t gpsSatNum;
	uint8_t gpsTimeS;
	uint16_t hallSensors[5];

	float tanWaVoltage;
	uint8_t tanWaState : 4;
	bool tanWaIgniter : 1;
	uint8_t tanWaFill : 2;
	uint8_t tanWaDepr : 2;
	uint8_t tanWaUpust : 2;
	float rocketWeightKg;
	float tankWeightKg;
	uint32_t rocketWRaw;
	uint32_t tankWRaw;

};

struct EspBinData {

	float batteryV;
	float tankPressure;
};

#endif