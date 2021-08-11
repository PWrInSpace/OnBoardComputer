#ifndef LOOPTASKS_H
#define LOOPTASKS_H

#include <Arduino.h>
#include <Wire.h>

#include "now.h"
#include "queue.h"
#include "SDcard.h"

void i2cTask(void *arg);

void sdTask(void *arg);

void espNowTask(void *arg);

void adcTask(void *arg);

#endif