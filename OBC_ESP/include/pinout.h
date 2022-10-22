#ifndef PINOUT_HH
#define PINOUT_HH

#include <Arduino.h>

#define MOSI GPIO_NUM_23
#define MISO GPIO_NUM_19
#define SCK GPIO_NUM_18
#define SD_CS GPIO_NUM_16

#define LORA_CS GPIO_NUM_4
#define LORA_RS GPIO_NUM_2
#define LORA_D0 GPIO_NUM_17

#define CAMERA GPIO_NUM_5
#define BUZZER GPIO_NUM_32

#define I2C1_SDA GPIO_NUM_21
#define I2C1_SCL GPIO_NUM_22
#define I2C2_SDA GPIO_NUM_27
#define I2C2_SCL GPIO_NUM_26

#define BATTERY_PIN GPIO_NUM_36

#endif