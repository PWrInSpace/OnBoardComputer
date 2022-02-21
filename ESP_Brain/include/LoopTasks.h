#ifndef LOOPTASKS_H
#define LOOPTASKS_H

#include <Arduino.h>
#include <LITTLEFS.h>

#include "Adafruit_BME280.h"
#include "Timer_ms.h"
#include "dataStructs.h"

void flashTask(void *arg);

void adcMeasure();

#endif