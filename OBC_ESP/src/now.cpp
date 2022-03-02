#include "now.h"

extern DataFrame dataFrame;
extern RocketControl rc;

bool adressCompare(const uint8_t *addr1, const uint8_t *addr2);

/**********************************************************************************************/

bool nowInit() {

  WiFi.mode(WIFI_STA);
  if (esp_now_init()) return false;

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  return true;
}

/**********************************************************************************************/

bool nowAddPeer(const uint8_t* address, uint8_t channel) {

  esp_now_peer_info_t peerInfo = {};

  memcpy(peerInfo.peer_addr, address, 6);
  peerInfo.channel = channel;

  if (esp_now_add_peer(&peerInfo)) return false;
  return true;
}

/**********************************************************************************************/

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {

  // if (status) ... - Dodanie errora ESP-now.
}

/**********************************************************************************************/

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {

  uint8_t adressToQueue = 0;

  if(adressCompare(mac, adressPitot)) {

    memcpy(&dataFrame.pitotData, (PitotData*) incomingData, sizeof(dataFrame.pitotData));
    adressToQueue = 1;
  }

  else if(adressCompare(mac, adressTanWa)) {

    memcpy(&dataFrame.tanWaData, (TanWaData*) incomingData, sizeof(dataFrame.tanWaData));
    adressToQueue = 2;
  }

  else if(adressCompare(mac, adressMValve)) {

    memcpy(&dataFrame.mainValveData, (MainValveData*) incomingData, sizeof(dataFrame.mainValveData));
    adressToQueue = 3;
  }

  else if(adressCompare(mac, adressUpust)) {

    memcpy(&dataFrame.upustValveData, (UpustValveData*) incomingData, sizeof(dataFrame.upustValveData));
    adressToQueue = 4;
  }

  // TODO coś o black boxie.

  xQueueSend(rc.espNowQueue, &adressToQueue, portMAX_DELAY);
}

/**********************************************************************************************/

bool adressCompare(const uint8_t *addr1, const uint8_t *addr2) {

  for(int8_t i = 0; i < 6; i++) {

    if(addr1[i] != addr2[i]) return false;
  }

  return true;
}