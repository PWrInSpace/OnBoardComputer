#include "../include/structs/stateMachine.h"
#include "esp_log.h"

#define TAG "SM"

static struct {
  States currentState;
  States requestState;
  TaskHandle_t * stateTask;
  States holdedState; //keep holded state default is States::HOLD
  xSemaphoreHandle stateMutex;
}sm;

bool SM_init(TaskHandle_t *_stateTask){
  sm.stateTask = _stateTask;
  sm.currentState = States::INIT;
  sm.requestState = States::NO_CHANGE;
  sm.holdedState = States::HOLD;
  sm.stateMutex = xSemaphoreCreateMutex();
  if (sm.stateMutex == NULL) {
    ESP_LOGE(TAG, "State machine init error!");
    return false;
  }

  return true;
}

States SM_getCurrentState(){
  States currentState;
  assert(sm.stateMutex != NULL);
  xSemaphoreTake(sm.stateMutex, portMAX_DELAY);
  currentState = sm.currentState;
  xSemaphoreGive(sm.stateMutex);

  return currentState;
}

States SM_getRequestedState(void) {
  States requested;
  xSemaphoreTake(sm.stateMutex, portMAX_DELAY);
  requested = sm.requestState;
  xSemaphoreGive(sm.stateMutex);

  return requested;
}

static void SM_setCurrentState(States new_state) {
  xSemaphoreTake(sm.stateMutex, portMAX_DELAY);
  sm.currentState = new_state;
  xSemaphoreGive(sm.stateMutex);
}

static void SM_setRequestState(States requested) {
  xSemaphoreTake(sm.stateMutex, portMAX_DELAY);
  sm.requestState = requested;
  xSemaphoreGive(sm.stateMutex);
}

//notify that changing state event occure
bool SM_changeStateRequest(States _newState){
  States currentState;
  assert(sm.stateTask != NULL);
  if(sm.stateTask == NULL){
      return false;
  }

  currentState = SM_getCurrentState();
  //out of range
  if(_newState < States::IDLE || _newState > States::ABORT){
    return false;
  }

  //ABORT CASAE
  if(currentState == States::ABORT){
    return false;
  }

  //Flight case, prevent rocket block in flight
  if((currentState >= States::FLIGHT && currentState < States::HOLD) &&
      (_newState == States::ABORT || _newState == States::HOLD)){
    return false;
  }

  //check new state correctness
  if(((_newState - 1) != currentState) && (_newState != States::ABORT && _newState != States::HOLD)){
    return false;
  }

  if(currentState == States::HOLD){ //current state is hold
    if(_newState == States::ABORT){ //abort in hold case
      SM_setRequestState(_newState);
    }else if(sm.holdedState == States::COUNTDOWN){ //hold in countdown case
      SM_setRequestState((States)((uint8_t)sm.holdedState - 1));
    }else{
      SM_setRequestState(sm.holdedState);
    }
    sm.holdedState = States::HOLD;
  }else if(_newState == States::HOLD){ //new state is hold
    sm.holdedState = SM_getCurrentState();
    SM_setRequestState(States::HOLD);
  }else{ //normal states or abort
    SM_setRequestState(_newState);
  }

  xTaskNotifyGive(*sm.stateTask);
  return true;
}

void SM_forceStateChange(States new_state) {
  SM_setRequestState(new_state);
  xTaskNotifyGive(*sm.stateTask);
}

//Use only in stateTask
void SM_changeStateConfirmation(void){
  States requested;
  requested = SM_getRequestedState();
  if(requested != States::NO_CHANGE){
    SM_setCurrentState(requested);
    Serial.print("STATE CHANGE TO: ");
    Serial.println(requested);
  }

  SM_setRequestState(States::NO_CHANGE);
}

void SM_changeStateRejection(void){
  SM_setRequestState(States::NO_CHANGE);
}


