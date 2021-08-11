#ifndef SDCARD_H
#define SDCARD_H

#include <FS.h>
#include <SPI.h>
#include <SD.h>
#include "queue.h"

#define SD_CS 5
//#define MISO GPIO_NUM_19
//#define MOSI GPIO_NUM_23
//#define SCK GPIO_NUM_18

//bool SD_init();

bool SD_write(const String & file, const String & data);

#endif