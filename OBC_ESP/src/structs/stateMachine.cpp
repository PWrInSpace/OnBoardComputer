#include "../include/structs/stateMachine.h"

StateMachine::StateMachine(xTaskHandle _stateTask){
  stateTask = _stateTask;
  currentState = States::INIT;
  requestState = States::NO_CHANGE;
  holdedState = States::HOLD;
}

//notify that changing state event occure
bool StateMachine::changeStateRequest(States _newState){
 //task handler wasn't set
  if(stateTask == NULL){
    return false;
  }

  //out of range
  if(_newState < States::IDLE || _newState > States::ABORT){
    return false;
  }

  //ABORT CASAE
  if(currentState == States::ABORT){ 
    return false;
  }

  //Flight case, prevent rocket block in flight
  if((currentState > COUNTDOWN && currentState < HOLD) && (_newState == States::ABORT || _newState == States::HOLD)){
    return false;
  }

  //check new state correctness
  if((_newState - 1) != currentState && (_newState != States::ABORT && _newState != States::HOLD)){
    return false;
  }
  
  if(currentState == States::HOLD){ //current state is hold
    if(_newState == States::ABORT){ //abort in hold case
      requestState = _newState;
    }else if(holdedState == States::COUNTDOWN){ //hold in countdown case
      requestState = (States)((uint8_t)holdedState - 1);
    }else{
      requestState = holdedState;
    }
    holdedState = States::HOLD;
  }else if(_newState == States::HOLD){ //new state is hold
    holdedState = currentState;
    requestState = _newState;
  }else{ //normal states or abort
    requestState = _newState;
  }

  xTaskNotifyGive(stateTask);
  return true;
}

//Use only in stateTask
void StateMachine::changeStateConfirmation(){
  if(requestState != States::NO_CHANGE){
    currentState = requestState;
  }

  requestState = NO_CHANGE;
}

void StateMachine::changeStateRejection(){
  requestState = NO_CHANGE;
}

States StateMachine::getRequestedState(){
  return requestState;
}

States StateMachine::getCurrentState(){
  return currentState;
}

States StateMachine::currentState = States::INIT;
States StateMachine::requestState = States::NO_CHANGE;
States StateMachine::holdedState = States::HOLD;
xTaskHandle StateMachine::stateTask = NULL;