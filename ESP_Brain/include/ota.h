#ifndef OTA_H
#define OTA_H

#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

#define HOST_NAME   "esp32maincomp"
#define SSID        "AirboxKowal"
#define PASS        "gta34567"

extern bool useOta;
extern WebServer serverOta;

void initOtaSerwer();

#endif