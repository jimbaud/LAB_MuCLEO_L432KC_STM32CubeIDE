#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#include "stm32l4xx_hal.h"
#include "gpio.h"
#include "LCD_I2C.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "usart.h"
#include "tim.h"

#define LCD_ADDR (0x27 << 1)

void ENC_Init(void);
int16_t ENC_GetCount(void);
int ENC_GetDirection(void);
void update_screen(uint16_t count);
void User_Process(void);
void Execute_Option(void);
void Manage_LED_Blinking(void);

#endif /* INC_ENCODER_H_ */
