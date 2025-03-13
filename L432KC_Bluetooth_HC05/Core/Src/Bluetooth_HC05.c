#include "Bluetooth_HC05.h"
#include <ctype.h>  // Pour la fonction tolower

// Pointeur vers la structure de gestion de l'UART Bluetooth
UART_HandleTypeDef *bt_huart;

// Buffer de réception pour les données UART
char rxBuffer[BT_BUFFER_SIZE];
uint8_t rxIndex = 0;
uint8_t dataReceived = 0;
char rx[1];

/**
 * @brief Initialise le module Bluetooth avec l'UART spécifié.
 * @param huart Pointeur vers la structure de gestion de l'UART.
 */
void BT_Init(UART_HandleTypeDef *huart) {
    bt_huart = huart;
    HAL_UART_Receive_IT(bt_huart, (uint8_t*)&rxBuffer[rxIndex], 1);
}

/**
 * @brief Envoie des données via l'UART Bluetooth.
 * @param data Chaîne de caractères à envoyer.
 */
void BT_SendData(char *data) {
    if (HAL_UART_Transmit(bt_huart, (uint8_t*)data, strlen(data), HAL_MAX_DELAY) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief Envoie une commande AT au module Bluetooth.
 * @param command Commande à envoyer.
 */
void BT_SendCommand(char *command) {
    char cmd[BT_BUFFER_SIZE];
    snprintf(cmd, BT_BUFFER_SIZE, "%s\r\n", command);
    if (HAL_UART_Transmit(bt_huart, (uint8_t*)cmd, strlen(cmd), HAL_MAX_DELAY) != HAL_OK) {
        Error_Handler();
    }
    HAL_Delay(100);
}

/**
 * @brief Vérifie si des données sont disponibles dans le buffer de réception.
 * @return 1 si des données sont disponibles, 0 sinon.
 */
uint8_t BT_Available() {
    return dataReceived;
}

/**
 * @brief Lit les données disponibles dans le buffer de réception.
 * @return Pointeur vers les données lues, ou NULL si aucune donnée n'est disponible.
 */
char* BT_ReadData() {
    if (dataReceived) {
        dataReceived = 0;
        return rxBuffer;
    }
    return NULL;
}

/**
 * @brief Callback appelé lors de la réception d'un caractère sur l'UART.
 * @param huart Pointeur vers la structure de gestion de l'UART.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == bt_huart->Instance) {
        if (rxIndex < BT_BUFFER_SIZE - 1) {
            rxIndex++;
        } else {
            rxIndex = 0;
        }
        if (rxBuffer[rxIndex-1] == '\r' || rxBuffer[rxIndex-1] == '\n') {
            rxBuffer[rxIndex] = '\0';
            dataReceived = 1;
            rxIndex = 0;
            HAL_UART_Transmit(&huart2, (uint8_t*)rxBuffer, strlen(rxBuffer), HAL_MAX_DELAY);
        }
        HAL_UART_Receive_IT(bt_huart, (uint8_t*)&rxBuffer[rxIndex], 1);
    }

    if (huart->Instance == USART2) {
        HAL_UART_Transmit(bt_huart, (uint8_t*)&rx[0], 1, HAL_MAX_DELAY);
        HAL_UART_Receive_IT(&huart2, (uint8_t*)&rx[0], 1);
    }
}

/**
 * @brief Traite une commande reçue via l'UART.
 * @param command Commande à traiter.
 */
void processCommand(char *command) {
    // Convertir la commande en minuscules pour une comparaison insensible à la casse
    for (int i = 0; command[i]; i++) {
        command[i] = tolower(command[i]);
    }
    if (strcmp(command, "on") == 0) {
        HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
    } else if (strcmp(command, "off") == 0) {
        HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
    }
}
