/*
 * User_RTC.c
 *
 *  Created on: 15 nov. 2023
 *      Author: Joel
 */

#include "User_RTC.h"

char timeRTC[12];
char date[12];
int rtcHours, rtcMinutes, rtcSeconds, rtcYear, rtcMonth, rtcDay;
uint8_t alarmTriggered;

void RTC_SetTime(uint8_t format12H) {
    RTC_TimeTypeDef rtcTime;
    char message[3];

    // Demande à l'utilisateur d'entrer l'heure
    HAL_UART_Transmit(&huart2, (uint8_t *)"\r\nEntrez l'heure actuelle hh : ", strlen("\r\nEntrez l'heure actuelle hh : "), HAL_MAX_DELAY);
    HAL_UART_Receive(&huart2, (uint8_t *)message, 2, HAL_MAX_DELAY);
    message[2] = '\0'; // Ajout du caractère nul à la fin de la chaîne
    rtcTime.Hours = atoi(message);

    // Demande à l'utilisateur d'entrer les minutes
    HAL_UART_Transmit(&huart2, (uint8_t *)"\r\nEntrez les minutes actuelles mm : ", strlen("\r\nEntrez les minutes actuelles mm : "), HAL_MAX_DELAY);
    HAL_UART_Receive(&huart2, (uint8_t *)message, 2, HAL_MAX_DELAY);
    message[2] = '\0'; // Ajout du caractère nul à la fin de la chaîne
    rtcTime.Minutes = atoi(message);

    // Demande à l'utilisateur d'entrer les secondes
    HAL_UART_Transmit(&huart2, (uint8_t *)"\r\nEntrez les secondes actuelles ss : ", strlen("\r\nEntrez les secondes actuelles ss : "), HAL_MAX_DELAY);
    HAL_UART_Receive(&huart2, (uint8_t *)message, 2, HAL_MAX_DELAY);
    message[2] = '\0'; // Ajout du caractère nul à la fin de la chaîne
    rtcTime.Seconds = atoi(message);

    // Configure l'heure
    HAL_RTC_SetTime(&hrtc, &rtcTime, format12H ? RTC_HOURFORMAT_12 : RTC_HOURFORMAT_24);
}

void RTC_SetDate(void) {
    RTC_DateTypeDef rtcDate;
    char message[3];

    // Demande à l'utilisateur d'entrer l'année
    HAL_UART_Transmit(&huart2, (uint8_t *)"\r\nEntrez l'annee actuelle aa : ", strlen("\r\nEntrez l'année actuelle aa : "), HAL_MAX_DELAY);
    HAL_UART_Receive(&huart2, (uint8_t *)message, 2, HAL_MAX_DELAY);
    message[2] = '\0'; // Ajout du caractère nul à la fin de la chaîne
    rtcDate.Year = atoi(message);

    // Demande à l'utilisateur d'entrer le mois
    HAL_UART_Transmit(&huart2, (uint8_t *)"\r\nEntrez le mois actuel mm : ", strlen("\r\nEntrez le mois actuel mm : "), HAL_MAX_DELAY);
    HAL_UART_Receive(&huart2, (uint8_t *)message, 2, HAL_MAX_DELAY);
    message[2] = '\0'; // Ajout du caractère nul à la fin de la chaîne
    rtcDate.Month = atoi(message);

    // Demande à l'utilisateur d'entrer le jour
    HAL_UART_Transmit(&huart2, (uint8_t *)"\r\nEntrez le jour actuel jj : ", strlen("\r\nEntrez le jour actuel jj : "), HAL_MAX_DELAY);
    HAL_UART_Receive(&huart2, (uint8_t *)message, 2, HAL_MAX_DELAY);
    message[2] = '\0'; // Ajout du caractère nul à la fin de la chaîne
    rtcDate.Date = atoi(message);

    // Configure la date
    HAL_RTC_SetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN);
}

void RTC_SetAlarm(void) {
    RTC_AlarmTypeDef alarmConfig = {0};
    char message[3];

    // Demande à l'utilisateur d'entrer l'heure de l'alarme
    HAL_UART_Transmit(&huart2, (uint8_t *)"\r\nEntrez l'heure de l'alarme hh : ", strlen("\r\nEntrez l'heure de l'alarme hh : "), HAL_MAX_DELAY);
    HAL_UART_Receive(&huart2, (uint8_t *)message, 2, HAL_MAX_DELAY);
    message[2] = '\0'; // Ajout du caractère nul à la fin de la chaîne
    alarmConfig.AlarmTime.Hours = atoi(message);

    // Demande à l'utilisateur d'entrer les minutes de l'alarme
    HAL_UART_Transmit(&huart2, (uint8_t *)"\r\nEntrez les minutes de l'alarme mm : ", strlen("\r\nEntrez les minutes de l'alarme mm : "), HAL_MAX_DELAY);
    HAL_UART_Receive(&huart2, (uint8_t *)message, 2, HAL_MAX_DELAY);
    message[2] = '\0'; // Ajout du caractère nul à la fin de la chaîne
    alarmConfig.AlarmTime.Minutes = atoi(message);

    // Configuration d'alarme RTC
    alarmConfig.AlarmTime.Seconds = 0; // En supposant que les secondes sont toujours 0 pour simplifier
    alarmConfig.AlarmTime.SubSeconds = 0;
    alarmConfig.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    alarmConfig.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    alarmConfig.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY; // Pour activer la répétition hebdomadaire de l'alarme
    alarmConfig.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    alarmConfig.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
    alarmConfig.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;

    alarmConfig.Alarm = RTC_ALARM_A;

    // Configure l'alarme
    HAL_RTC_SetAlarm_IT(&hrtc, &alarmConfig, RTC_FORMAT_BIN);
}

void RTC_EnableAlarmInterrupt(void) {
    // Active l'interruption d'alarme RTC
    __HAL_RTC_ALARM_ENABLE_IT(&hrtc, RTC_IT_ALRA);
}

void RTC_DisableAlarmInterrupt(void) {
    // Désactive l'interruption d'alarme RTC
    __HAL_RTC_ALARM_DISABLE_IT(&hrtc, RTC_IT_ALRA);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
    alarmTriggered = 1;
}

void RTC_OnAlarmTrigger(void) {
    HAL_GPIO_WritePin(LDR_GPIO_Port, LDR_Pin, GPIO_PIN_SET);
    char alarmMessage[] = "\r\nAlarme !\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t *)alarmMessage, strlen(alarmMessage), HAL_MAX_DELAY);
}

void RTC_GetTime(void) {
    RTC_DateTypeDef rtcDate = {0};
    RTC_TimeTypeDef rtcTime = {0};

    HAL_RTC_GetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN);

    // Formate l'heure et la date
    snprintf((char *)timeRTC, sizeof(timeRTC), "%02d:%02d:%02d", rtcTime.Hours, rtcTime.Minutes, rtcTime.Seconds);
    snprintf((char *)date, sizeof(date), "%02d-%02d-%02d", rtcDate.Date, rtcDate.Month, 2000 + rtcDate.Year);
}

void RTC_DisplayTime(void) {
    char output[80];

    // Efface l'écran du terminal
    snprintf(output, sizeof(output), "\e[1;1H\e[2J");

    // Affiche l'heure et la date
    snprintf(output + strlen(output), sizeof(output) - strlen(output), "%s\r\n", timeRTC);
    snprintf(output + strlen(output), sizeof(output) - strlen(output), "%s\r\n", date);

    // Transmet le message via UART
    HAL_UART_Transmit(&huart2, (uint8_t *)output, strlen(output), HAL_MAX_DELAY);
}
