#include "../include/com/txEspNow.h"

TxDataEspNow::TxDataEspNow(uint8_t _command, uint16_t _commandTime):

  command(_command),
  commandTime(_commandTime) {}

void TxDataEspNow::setVal(uint8_t _command, uint16_t _commandTime){

  command = _command;
  commandTime = _commandTime;
}