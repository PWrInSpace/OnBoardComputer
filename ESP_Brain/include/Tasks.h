#include <Arduino.h>
#include <Wire.h>

#include "now.h"

void i2cTask(void *arg);

void uart3AntsTask(void *arg);

void sdTask(void *arg);

void espNowTask(void *arg);