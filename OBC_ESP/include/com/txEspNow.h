#ifndef TX_ESP_NOW_HH
#define TX_ESP_NOW_HH

#include <stdint.h>

struct TxDataEspNow {
  uint8_t command;
  uint16_t commandTime;

  TxDataEspNow() = default;
  TxDataEspNow(uint8_t _command, uint16_t _commandTime);
  void setVal(uint8_t _command, uint16_t _commandTime);
};

#endif