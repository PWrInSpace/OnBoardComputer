#ifndef STATE_MACHINE_HH
#define STATE_MACHINE_HH

#include "FreeRTOS.h"
#include "Arduino.h"

enum States{
  INIT = 0,
  IDLE,
  ARMED,
  FUELING,
  RDY_TO_LAUNCH,
  COUNTDOWN,
  FLIGHT,
  FIRST_STAGE_RECOVERY,
  SECOND_STAGE_RECOVERY,
  ON_GROUND,
  HOLD,
  ABORT
};

class StateMachine{
  static States currentState;
  static States requestState;
  static xTaskHandle stateTask;
  static States holdedState; //keep holded state default is States::HOLD

  public:
  StateMachine(xTaskHandle _stateTask);
  static bool changeStateRequest(States _newState);
  void changeStateConfirmation();
  void changeStateRejection();
  States getRequestedState();
  static States getCurrentState();
};

//static init value



#endif