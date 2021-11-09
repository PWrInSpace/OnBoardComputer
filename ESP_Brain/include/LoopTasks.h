#ifndef LOOPTASKS_H
#define LOOPTASKS_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>

#include "Adafruit_BME280.h"
#include "now.h"
#include "queue.h"
#include "SDcard.h"
#include "Timer_ms.h"
#include "ThrustController.h"

void i2cTask(void *arg);
void i2cTaskInit(Adafruit_BME280 &bme);

void i2cSendByte(uint8_t adress, uint8_t val, int8_t times);

void sdTask(void *arg);

void adcTask(void *arg);

#endif