#ifndef BLUETOOTH_HC05_H
#define BLUETOOTH_HC05_H

#include "stm32l4xx_hal.h"  // Inclusion des bibliothèques nécessaires pour STM32
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BT_BUFFER_SIZE 128  // Taille du buffer de réception

// Déclaration des variables externes
extern UART_HandleTypeDef huart1;  // UART pour le module Bluetooth
extern UART_HandleTypeDef huart2;  // UART pour la communication avec le PC
extern uint8_t dataReceived;       // Indicateur de données reçues
extern char rx[1];                 // Buffer de réception pour un caractère

// Prototypes des fonctions
void BT_Init(UART_HandleTypeDef *huart);
void BT_SendData(char *data);
void BT_SendCommand(char *command);
uint8_t BT_Available();
char* BT_ReadData();
void processCommand(char *command);

#endif // BLUETOOTH_HC05_H
