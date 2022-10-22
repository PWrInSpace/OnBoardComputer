#ifndef STATE_MACHINE_HH
#define STATE_MACHINE_HH


#include "freertos/FreeRTOS.h"
#include "Arduino.h"

enum States{
  INIT = 0,
  IDLE,
  RECOVERY_ARM,
  FUELING,
  ARMED_TO_LAUNCH,
  RDY_TO_LAUNCH,
  COUNTDOWN,
  FLIGHT,
  FIRST_STAGE_RECOVERY,
  SECOND_STAGE_RECOVERY,
  ON_GROUND,
  HOLD,
  ABORT,
  NO_CHANGE = 0xff //DO NOT USE, ONLY FOR REQUEST PURPOSE
};

// class StateMachine{
//   static States currentState;
//   static States requestState;
//   static xTaskHandle stateTask;
//   static States holdedState; //keep holded state default is States::HOLD

//   public:
//   StateMachine(xTaskHandle _stateTask);
//   static bool changeStateRequest(States _newState);
//   void changeStateConfirmation();
//   void changeStateRejection();
//   States getRequestedState();
//   static States getCurrentState();
// };

bool SM_init(TaskHandle_t* _stateTask);
States SM_getCurrentState(void);
States SM_getRequestedState(void);
bool SM_changeStateRequest(States _newState);
void SM_changeStateConfirmation(void);
void SM_changeStateRejection(void);
void SM_forceStateChange(States new_state);
#endif