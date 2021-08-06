#include <Arduino.h>

#include "now.h"

//Test

PitotData pitotdata;

float initialPressure = 1013;

void setup(){
  
  Serial.begin(115200);

  nowInit();

  Serial.println();
  Serial.println("Mac adress:" + WiFi.macAddress());

  nowAddPeer(adressPitot, 0);
  nowAddPeer(adressMValve, 0);
}

void loop() {

  /*char message[] = "wazna wiadomosc do przeslania\n";
  esp_err_t result = esp_now_send(adressPitot, (uint8_t *) message, strlen(message));
   
  if (!result) int i =5; //Serial.println("Wyslano");
  else Serial.println("Blad wysylania");

  delay(10);*/

  // Kod tylko dla pitota:
  pitotdata.staticPressure += 1;
  pitotdata.dynamicPressure += 1;
  pitotdata.altitude = 44330.0 * (1.0 - pow(pitotdata.staticPressure / (initialPressure * 100), 0.1903));
  pitotdata.speed += 1;

  esp_err_t result = esp_now_send(adressMain, (uint8_t *) &pitotdata, sizeof(pitotdata));
   
  if (!result) Serial.println("Wyslano dane pitota");
  else Serial.println("Blad wysylania danych pitota");
  
  //delay(pitotDelayTime);

  // Kod tylko dla maina:
  /*if (Serial.available() > 0) {
    String uartData = Serial.readString();

    Serial.println(uartData);
    if (uartData == String("Napier..")) {

      pitotDelayTime = 50;
      esp_err_t result = esp_now_send(adressPitot, (uint8_t *) &pitotDelayTime, sizeof(pitotDelayTime));
   
      if (!result) Serial.println("Wyslano zmiane szybkosci");
      else Serial.println("Blad wysylania zmiany szybkosci");
    }

    else if (uartData == String("Stop")) {

      pitotDelayTime = 2500;
      esp_err_t result = esp_now_send(adressPitot, (uint8_t *) &pitotDelayTime, sizeof(pitotDelayTime));
   
      if (!result) Serial.println("Wyslano zmiane szybkosci");
      else Serial.println("Blad wysylania zmiany szybkosci");
    }

    else Serial.println("Nieznany napis");
  }*/
}