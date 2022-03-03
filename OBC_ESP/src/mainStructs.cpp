#include "mainStructs.h"

  //notify that changing state event occure
bool RocketControl::changeStateEvent(StateMachineEvent newEvent){
		//portENTER_CRITICAL(&stateLock);
  if((newEvent - 1) != stateEvent && newEvent != ABORT_EVENT){
    return false;
  }
    
  this->stateEvent = newEvent;
  //portEXIT_CRITICAL(&stateLock);
  xTaskNotifyGive(this->stateTask);
  return true;
}

//Use only in stateTask
void RocketControl::changeState(StateMachine newState){
  if((newState - 1) != state && newState != ABORT){
    return;
  }
    
  //portENTER_CRITICAL(&stateLock);
  this->state = newState;
  //portEXIT_CRITICAL(&stateLock);
}

void RocketControl::unsuccessfulEvent(){
  this->stateEvent = (StateMachineEvent)((uint8_t)stateEvent - 1);
}