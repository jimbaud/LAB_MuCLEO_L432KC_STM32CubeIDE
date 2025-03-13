/*
 * servo_control.c
 *
 *  Created on: Mar 13, 2025
 *      Author: Joel
 */


#include "servo_control.h"
#include <stdio.h>
#include <string.h>
#include "ctype.h"

// Variables globales statiques
static TIM_HandleTypeDef *servoTimHandle;
static uint32_t servoChannel;

// Durée du temps haut pour 0 et 180 degrés (en microsecondes)
static uint16_t pulse_0_degree = 500;
static uint16_t pulse_180_degree = 2500;

int angle; // Variable globale contenant l'angle
unsigned char MSG1[4]; // Variable globale du buffer de réception

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	static int i = 0;
	static int trame_en_cours = 0;
	static char angleBuffer[4]; // Buffer pour stocker les chiffres de l'angle

	if (trame_en_cours == 1) {
		if (MSG1[i] != 0x0D) {
			if (isdigit(MSG1[i]) && i < 3) { // Vérifie si le caractère est un chiffre et limite à 3 chiffres
				angleBuffer[i] = MSG1[i];
				i++;
			} else {
				// Affiche un message d'erreur sur la console en cas d'entrée incorrecte
				char errorResp[50];
				snprintf(errorResp, sizeof(errorResp), "\n\rErreur: Entrée invalide.\r\n");
				HAL_UART_Transmit(&huart2, (uint8_t*)errorResp, strlen(errorResp), HAL_MAX_DELAY);
				i = 0;
				trame_en_cours = 0;
			}
		} else {
			angleBuffer[i] = '\0'; // Ajoute le caractère de fin de chaîne
			angle = atoi(angleBuffer);
			Servo_SetPosition(angle);
			char resp[50];
			snprintf(resp, sizeof(resp), "\n\rVous avez entré la valeur de : %d degrés.\r\n", angle);
			HAL_UART_Transmit(&huart2, (uint8_t*)resp, strlen(resp), HAL_MAX_DELAY);
			i = 0;
			trame_en_cours = 0;
		}
	} else {
		if (isdigit(MSG1[0])) { // Vérifie si le premier caractère est un chiffre
			angleBuffer[i] = MSG1[0];
			i = 1;
			trame_en_cours = 1;
		} else {
			i = 0;
			trame_en_cours = 0;
		}
	}
	HAL_UART_Receive_IT(&huart2, &MSG1[i], 1);
}

void Servo_Init(TIM_HandleTypeDef *htim, uint32_t channel) {
	servoTimHandle = htim;
	servoChannel = channel;
	// Add PWM initialization code here
}

void Servo_SetPosition(uint8_t angle) {
	// Assurez-vous que l'angle est dans la plage valide (0 à 180 degrés)
	angle = (angle > 180) ? 180 : angle;
	// Calculez le rapport de service PWM en fonction de l'angle désiré
	uint16_t pulse = pulse_0_degree + (angle * (pulse_180_degree - pulse_0_degree)) / 180;
	// Définissez le rapport de service PWM
	HAL_TIM_PWM_Start(servoTimHandle, servoChannel);
	__HAL_TIM_SET_COMPARE(servoTimHandle, servoChannel, pulse);
}

void Servo_Test_TimeSet(int start, int stop) {
	for (int i = start; i <= stop; i++) {
		HAL_TIM_PWM_Start(servoTimHandle, servoChannel);
		__HAL_TIM_SET_COMPARE(servoTimHandle, servoChannel, i);
		// Afficher la valeur du timer sur la console
		char resp[50];
		snprintf(resp, sizeof(resp), "\e[1;1H\e[2JValeur du timer : %d", i);
		HAL_UART_Transmit(&huart2, (uint8_t*)resp, strlen(resp), HAL_MAX_DELAY);
		HAL_Delay(25);
	}
}
