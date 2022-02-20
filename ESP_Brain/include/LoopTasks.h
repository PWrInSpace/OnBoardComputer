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

void sdTask(void *arg);

void adcMeasure();

#endif