/*
 * Gestion_IT_UART.h
 *
 *  Created on: Nov 15, 2023
 *      Author: joel.imbaud
 */

#ifndef INC_GESTION_IT_UART_H_
#define INC_GESTION_IT_UART_H_

#include "main.h"
#include "math.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

extern int resultat_int; // Variable globale contenant le résultat
extern unsigned char MSG1[32]; // Variable globale du buffer de réception
extern UART_HandleTypeDef huart2;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* INC_GESTION_IT_UART_H_ */
