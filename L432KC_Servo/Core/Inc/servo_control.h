/*
 * servo_control.h
 *
 *  Created on: Mar 13, 2025
 *      Author: Joel
 */

#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include "stm32l4xx_hal.h"
#include "main.h"
#include "math.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

extern unsigned char MSG1[4]; // Variable globale du buffer de réception
extern UART_HandleTypeDef huart2;
void Servo_Init(TIM_HandleTypeDef *htim, uint32_t channel);
void Servo_SetPosition(uint8_t angle);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void Servo_Test_TimeSet(int start, int stop);
#endif /* SERVO_CONTROL_H */
