#include "../include/structs/stateMachine.h"

StateMachine::StateMachine(xTaskHandle _stateTask){
  stateTask = _stateTask;
  currentState = INIT;
  requestState = INIT;
}

//notify that changing state event occure
bool StateMachine::changeStateRequest(States _newState){
 //out of range
  if(stateTask == NULL){
    return false;
  }

  if(_newState < IDLE || _newState > ABORT){
    return false;
  }

  if((_newState - 1) != currentState && _newState != ABORT){
    return false;
  }
    
  requestState = _newState;

  xTaskNotifyGive(stateTask);
  return true;
}

//Use only in stateTask
void StateMachine::changeStateConfirmation(){
  if(requestState != INIT){
    currentState = requestState;
  }

  requestState = INIT;
}

void StateMachine::changeStateRejection(){
  requestState = INIT;
}

States StateMachine::getRequestedState(){
  return requestState;
}

States StateMachine::getCurrentState(){
  return currentState;
}

States StateMachine::currentState = INIT;
States StateMachine::requestState = INIT;
xTaskHandle StateMachine::stateTask = NULL;