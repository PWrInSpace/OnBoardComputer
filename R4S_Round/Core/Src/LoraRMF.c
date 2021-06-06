#include "LoraRFM.h"

void loraInit(void) {

	//initialize LoRa module
	SX1278_hw.dio0.port = DO_RF_GPIO_Port;
	SX1278_hw.dio0.pin = DO_RF_Pin;
	SX1278_hw.nss.port = CS_RF_GPIO_Port;
	SX1278_hw.nss.pin = CS_RF_Pin;
	SX1278_hw.reset.port = RF_RST_GPIO_Port;
	SX1278_hw.reset.pin = RF_RST_Pin;
	SX1278_hw.spi = &hspi1;

	SX1278.hw = &SX1278_hw;

	SX1278_begin(&SX1278, 868000000, SX1278_POWER_20DBM, SX1278_LORA_SF_7, SX1278_LORA_BW_125KHZ, 10);

	SX1278_LoRaEntryRx(&SX1278, BUFFER_SIZE, 2000);

}

/****************************************************************************************/

void loraSendData(uint8_t *data, int len) {

	loraRet = SX1278_LoRaEntryTx(&SX1278, len, 2000);

	loraRet = SX1278_LoRaTxPacket(&SX1278, data, len, 2000);


	SX1278_LoRaEntryRx(&SX1278, BUFFER_SIZE, 2000);
}

/****************************************************************************************/

void loraLoop(void) {

	loraRet = SX1278_LoRaRxPacket(&SX1278);

	if (loraRet > 0) {
		SX1278_read(&SX1278, (uint8_t*) loraBuffer, loraRet);
	}
}
