#ifndef INC_LORARFM_H_
#define INC_LORARFM_H_

#include "gpio.h"
#include "spi.h"
#include "SX1278.h"

SX1278_hw_t SX1278_hw;
SX1278_t SX1278;

#define BUFFER_SIZE 512

int loraRet;
char loraBuffer[BUFFER_SIZE];

void loraInit(void);
void loraSendData(uint8_t* data, int len);
void loraLoop(void);


#endif /* INC_LORARFM_H_ */
