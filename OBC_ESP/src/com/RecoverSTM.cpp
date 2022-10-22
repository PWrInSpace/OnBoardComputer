#include "../include/com/RecoverySTM.h"
#include "data_structs.h"

RecoverySTM::RecoverySTM(TwoWire &wirePort, uint8_t i2cAddress) {

  stmWire = &wirePort;
  address = i2cAddress;
}

/**********************************************************************************************/

void RecoverySTM::sendCommand(uint8_t command, uint16_t arg) {

  TxDataEspNow txData(command, arg);

  stmWire->beginTransmission(address);
  stmWire->write((uint8_t*) &txData, sizeof(txData));
  stmWire->endTransmission();
}

/**********************************************************************************************/

bool RecoverySTM::getRecoveryData(uint8_t* buffer) {

  stmWire->requestFrom(RECOVERY_ADDRES, sizeof(RecoveryData));
  if (stmWire->available()) {
    if (!stmWire->readBytes(buffer, sizeof(RecoveryData))) {
      return false;
    }
  }

  return true;
}