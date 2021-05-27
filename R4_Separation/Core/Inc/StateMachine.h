#ifndef INC_STATEMACHINE_H_
#define INC_STATEMACHINE_H_


enum StateMachine {

	INIT,
	ARMED,
	FLIGHT,
	END
};

StateMachine currentState;

#endif /* INC_STATEMACHINE_H_ */
