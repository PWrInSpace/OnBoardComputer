#include "../include/com/now.h"

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

  if (status != ESP_NOW_SEND_SUCCESS){
    rc.errors.setEspNowError(ESPNOW_DELIVERY_ERROR);
  }
}

/**********************************************************************************************/

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {

  uint8_t adressToQueue = 0;

  if(adressCompare(mac, adressPitot)) {

    memcpy(&rc.dataFrame.pitot, (PitotData*) incomingData, sizeof(rc.dataFrame.pitot));
    adressToQueue = PITOT;
  }

  else if(adressCompare(mac, adressTanWa)) {

    memcpy(&rc.dataFrame.tanWa, (TanWaData*) incomingData, sizeof(rc.dataFrame.tanWa));
    adressToQueue = TANWA;
  }

  else if(adressCompare(mac, adressMValve)) {
    Serial.println("MainValve Data");
    memcpy(&rc.dataFrame.mainValve, (MainValveData*) incomingData, sizeof(rc.dataFrame.mainValve));
    adressToQueue = MAIN_VALVE;
  }

  else if(adressCompare(mac, adressUpust)) {

    memcpy(&rc.dataFrame.upustValve, (UpustValveData*) incomingData, sizeof(rc.dataFrame.upustValve));
    adressToQueue = UPUST_VALVE;
  }

  else if(adressCompare(mac, adressBlackBox)) {

    memcpy(&rc.dataFrame.blackBox, (SlaveData*) incomingData, sizeof(rc.dataFrame.blackBox));
    adressToQueue = BLACK_BOX;
  }

  else if(adressCompare(mac, adressPayLoad)) {

    memcpy(&rc.dataFrame.payLoad, (SlaveData*) incomingData, sizeof(rc.dataFrame.payLoad));
    adressToQueue = PAYLOAD;
  }

  xQueueSend(rc.hardware.espNowQueue, &adressToQueue, portMAX_DELAY);
}

/**********************************************************************************************/

bool adressCompare(const uint8_t *addr1, const uint8_t *addr2) {

  for(int8_t i = 0; i < 6; i++) {

    if(addr1[i] != addr2[i]) return false;
  }

  return true;
}