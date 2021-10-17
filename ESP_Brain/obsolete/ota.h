#ifndef OTA_H
#define OTA_H

#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

#define HOST_NAME   "e32mc"
#define SSID        "Redmi8T"
#define PASS        "gta34567"

extern bool useOta;
extern WebServer serverOta;

void initOtaSerwer();

#endif