#ifndef LOOPTASKS_H
#define LOOPTASKS_H

#include <Arduino.h>

#include "dataStructs.h"
#include "SDcard.h"

void sdTask(void *arg);

void flashTask(void *arg);

void adcMeasure();

#endif