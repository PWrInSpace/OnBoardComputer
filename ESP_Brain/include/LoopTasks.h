#include <Arduino.h>
#include <Wire.h>

#include "now.h"

#include <FS.h>
#include <SPI.h>
#include <SD.h>
#include "queue.h"

void i2cTask(void *arg);

void sdTask(void *arg);

void espNowTask(void *arg);

void adcTask(void *arg);