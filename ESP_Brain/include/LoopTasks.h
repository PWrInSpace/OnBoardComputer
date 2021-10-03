#ifndef LOOPTASKS_H
#define LOOPTASKS_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "now.h"
#include "queue.h"
#include "SDcard.h"
#include "Timer_ms.h"

void i2cTask(void *arg);
void i2cTaskInit(Adafruit_BME280 &bme);

void sdTask(void *arg);

void adcTask(void *arg);

#endif