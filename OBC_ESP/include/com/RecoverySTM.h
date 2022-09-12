#ifndef RECOVERY_STM
#define RECOVERY_STM

#include <Wire.h>
#include "config.h"
#include "txEspNow.h"

class RecoverySTM {

  TwoWire *stmWire;
  uint8_t address;

public:

  RecoverySTM(TwoWire &wirePort, uint8_t i2cAddress);

  void arm(bool on) { sendCommand(on ? RECOVERY_ARM_COMMAND : RECOVERY_DISARM, 0); }

  void setTelemetrum(bool on) { sendCommand(on ? RECOVERY_TELEMETRUM_ON : RECOVERY_TELEMETRUM_OFF, 0); }

  void forceFirstStageSeparation() { sendCommand(RECOVERY_FORCE_1, 0); }

  void forceSecondStageSeparation() { sendCommand(RECOVERY_FORCE_2, 0); }

  bool getRecoveryData(uint8_t* buffer);

  void sendCommand(uint8_t command, uint16_t arg);
};

#endif