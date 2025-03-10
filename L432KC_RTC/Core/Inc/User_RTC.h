/*
 * User_RTC.h
 *
 *  Created on: 15 nov. 2023
 *      Author: Joel
 */

/* User_RTC.h */

#ifndef User_RTC_H_
#define User_RTC_H_

#include "stm32l4xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "main.h"

extern RTC_HandleTypeDef hrtc;
extern UART_HandleTypeDef huart2;

// Fonction pour initialiser le module RTC
void RTC_Init(void);

// Fonction pour configurer l'heure du RTC
void RTC_SetTime(uint8_t format12H);

// Fonction pour configurer la date du RTC
void RTC_SetDate(void);

// Fonction pour configurer l'alarme du RTC
void RTC_SetAlarm(void);

// Fonction pour activer l'interruption d'alarme RTC
void RTC_EnableAlarmInterrupt(void);

// Fonction pour désactiver l'interruption d'alarme RTC
void RTC_DisableAlarmInterrupt(void);

// Gestionnaire de l'interruption d'événement d'alarme RTC
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);

// Actions à effectuer lorsqu'une alarme est déclenchée
void RTC_OnAlarmTrigger(void);

// Fonction pour obtenir l'heure actuelle du RTC
void RTC_GetTime(void);

// Fonction pour afficher l'heure actuelle du RTC
void RTC_DisplayTime(void);

#endif /* User_RTC_H_ */
