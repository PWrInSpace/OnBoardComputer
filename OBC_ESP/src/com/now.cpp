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
    ERR_set_esp_now_error(ESPNOW_DELIVERY_ERROR);
  }
}

/**********************************************************************************************/

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {

  EspNowDevice adressToQueue;

  //Serial.println("Odpiur");

  if(adressCompare(mac, adressPitot)) {
    PitotData data;
    memcpy(&data, (PitotData*) incomingData, sizeof(data));
    DF_set_pitot_data(data);
    adressToQueue = PITOT;
  }

  else if(adressCompare(mac, adressTanWa)) {
    TanWaData data;
    memcpy(&data, (TanWaData*) incomingData, sizeof(data));
    DF_set_tanwa_data(data);
    adressToQueue = TANWA;
  }

  else if(adressCompare(mac, adressMValve)) {
    Serial.println("MainValve Data");
    MainValveData data;
    memcpy(&data, (MainValveData*) incomingData, sizeof(data));
    DF_set_main_valve_data(data);
    adressToQueue = MAIN_VALVE;
  }

  else if(adressCompare(mac, adressUpust)) {
    Serial.println("Upust Data");
    UpustValveData data;
    memcpy(&data, (UpustValveData*) incomingData, sizeof(data));
    DF_set_upust_valve_data(data);
    adressToQueue = UPUST_VALVE;
  }

  else if(adressCompare(mac, adressBlackBox)) {
    Serial.println("Blackboc Data");
    SlaveData data;
    memcpy(&data, (SlaveData*) incomingData, sizeof(data));
    DF_set_blackbox_data(data);
    adressToQueue = BLACK_BOX;
  }

  else if(adressCompare(mac, adressPayLoad)) {
    Serial.println("Payload Data");
    PayloadData data;
    memcpy(&data, (PayloadData*) incomingData, sizeof(data));
    DF_set_payload_data(data);
    adressToQueue = PAYLOAD;
  } else {
    return;
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