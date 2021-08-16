#include "now.h"

extern MainDataFrame mainDataFrame;

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

  if (status) mainDataFrame.espNowErrorCounter++;
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  
  if(adressCompare(mac, adressPitot)) {

    memcpy(&mainDataFrame.pitotData, (PitotData*) incomingData, sizeof(mainDataFrame.pitotData));
  }

  else if(adressCompare(mac, adressMValve)) {

    Serial.println((char*) incomingData);

    int a, b, c;
    int wart = sscanf((char*) incomingData, "R4MV;%d;%d;%d", &a, &b, &c);
    Serial.println(wart);

    mainDataFrame.mValve.endStop1 = a;
    mainDataFrame.mValve.endStop2 = b;
    mainDataFrame.mValve.potentiometer = c;
  }
  

}

/**********************************************************************************************/

bool adressCompare(const uint8_t *addr1, const uint8_t *addr2) {

  for(int8_t i = 0; i < 6; i++) {

    if(addr1[i] != addr2[i]) return false;
  }

  return true;
}