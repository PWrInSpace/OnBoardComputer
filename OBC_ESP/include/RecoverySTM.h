#ifndef RECOVERY_STM
#define RECOVERY_STM

#include <Wire.h>

class RecoverySTM {

  TwoWire *stmWire;
  uint8_t address;

public:

  RecoverySTM(TwoWire &wirePort, uint8_t i2cAddress);

  void arm(bool on) { sendCommand(on ? 1 : 2, 0); }

  void setTelemetrum(bool on) { sendCommand(on ? 3 : 4, 0); }

  void forceFirstStageSeparation() { sendCommand(165, 0); }

  void forceSecondStageSeparation() { sendCommand(90, 0); }

  bool getRecoveryData(uint8_t* buffer);

  void sendCommand(uint8_t command, uint16_t arg);
};

#endif