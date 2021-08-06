#include "now.h"

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

  if (status) Serial.println("Blad Dost");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? /*"Dostarczono"*/"" : "Blad dostarczania");
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  
  /*memcpy(&pitotdata, (PitotData*) incomingData, sizeof(pitotdata));

  Serial.println("Cisnienie stat: " + String(pitotdata.staticPressure));
  Serial.println("Cisnienie dyn: " + String(pitotdata.dynamicPressure));
  Serial.println("Wysokosc: " + String(pitotdata.altitude));
  Serial.println("Predkosc: " + String(pitotdata.speed));*/

  //memcpy(&pitotDelayTime, (uint16_t*) incomingData, sizeof(pitotDelayTime));

  //Serial.println(pitotDelayTime);
}