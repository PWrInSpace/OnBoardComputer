#include "mainStructs.h"

  //notify that changing state event occure
bool RocketControl::changeStateEvent(StateMachineEvent newEvent){
	//portENTER_CRITICAL(&stateLock);
  //out of range
  if(newEvent < IDLE_EVENT || newEvent > ABORT_EVENT){
    return false;
  }

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

void RocketControl::sendLog(const String & message){
  String log = "LOG: " + message + "[ " + String(state) + " , " + String(millis()) + " ]";
  Serial.println(log); //DEBUG
  xQueueSend(sdQueue, (void*)&log, 0);
}