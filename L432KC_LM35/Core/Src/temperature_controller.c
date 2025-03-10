/*
 * temperature_controller.c
 *
 *  Created on: Mar 8, 2025
 *      Author: Joel
 */


#include "temperature_controller.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

// Déclaration des variables statiques utilisées dans tout le fichier
static UART_HandleTypeDef *huart2;
static ADC_HandleTypeDef *hadc1;
static GPIO_TypeDef *ld2GpioPort;
static uint16_t ld2GpioPin;
static char clearScreen[] = "\e[1;1H\e[2J";
static char output[100];
static char inputBuffer[5];
static double T;
static float MaxTemp, MinTemp;

/**
 * @brief Initialise le contrôleur de température.
 *
 * @param huart Pointeur vers la structure UART_HandleTypeDef pour la communication UART.
 * @param hadc Pointeur vers la structure ADC_HandleTypeDef pour la conversion analogique-numérique.
 * @param gpioPort Pointeur vers la structure GPIO_TypeDef pour le port du voyant LED.
 * @param gpioPin Broche du voyant LED.
 */
void initTemperatureController(UART_HandleTypeDef *huart, ADC_HandleTypeDef *hadc, GPIO_TypeDef *gpioPort, uint16_t gpioPin) {
	huart2 = huart;
	hadc1 = hadc;
	ld2GpioPort = gpioPort;
	ld2GpioPin = gpioPin;
}

/**
 * @brief Démarre le contrôleur de température.
 *
 * Cette fonction initialise le contrôleur de température en demandant à l'utilisateur
 * d'entrer les valeurs maximales et minimales de température. Ensuite, elle effectue
 * une boucle infinie où elle mesure la température, compare avec les valeurs maximales
 * et minimales, allume la LED en conséquence, et affiche les informations sur la console UART.
 */
void startTemperatureController(void) {
	// Initialisation du contrôleur de température
	HAL_UART_Transmit(huart2, (uint8_t*)clearScreen, strlen(clearScreen), HAL_MAX_DELAY);

	// Positionne le curseur en haut à gauche de l'écran
	char homeCursor[] = {0x1B, '[', 'H', 0};
	HAL_UART_Transmit(huart2, (uint8_t*)homeCursor, sizeof(homeCursor), HAL_MAX_DELAY);

	// Demande à l'utilisateur d'entrer la température maximale
	char maxTempMsg[] = "\r\nEntrer Max temperature XX.XX : ";
	HAL_UART_Transmit(huart2, (uint8_t*)maxTempMsg, strlen(maxTempMsg), HAL_MAX_DELAY);
	HAL_UART_Receive(huart2, (uint8_t*)inputBuffer, sizeof(inputBuffer), HAL_MAX_DELAY);
	MaxTemp = strtof(inputBuffer, NULL);

	// Demande à l'utilisateur d'entrer la température minimale
	char minTempMsg[] = "\r\nEntrer Min temperature XX.XX : ";
	HAL_UART_Transmit(huart2, (uint8_t*)minTempMsg, strlen(minTempMsg), HAL_MAX_DELAY);
	HAL_UART_Receive(huart2, (uint8_t*)inputBuffer, sizeof(inputBuffer), HAL_MAX_DELAY);
	MinTemp = strtof(inputBuffer, NULL);

	while (1) {
		HAL_ADC_Start(hadc1);
		HAL_StatusTypeDef conv = HAL_ADC_PollForConversion(hadc1, 100);

		if (conv != HAL_OK) {
			// Gestion d'une éventuelle erreur de conversion ADC
			char errorMsg[30];
			snprintf(errorMsg, sizeof(errorMsg), "ADC ERROR!\r\nError code: %i", conv);
			HAL_UART_Transmit(huart2, (uint8_t*)errorMsg, strlen(errorMsg), HAL_MAX_DELAY);
		}

		// Calcul de la température en degrés Celsius
		T = (HAL_ADC_GetValue(hadc1) * 330.0) / 4096.0;

		// Positionne le curseur pour afficher les informations de température
		char gotoScr[] = {0x1B, '[', '7', ';', '0', 'H', 0};
		HAL_UART_Transmit(huart2, (uint8_t*)gotoScr, sizeof(gotoScr), HAL_MAX_DELAY);

		// Compare la température avec les seuils et prend des actions appropriées
		if (T < MaxTemp && T > MinTemp) {
			char normalMsg[] = "Normal - Temperature     = ";
			HAL_UART_Transmit(huart2, (uint8_t*)normalMsg, strlen(normalMsg), HAL_MAX_DELAY);
			HAL_GPIO_WritePin(ld2GpioPort, ld2GpioPin, GPIO_PIN_RESET);  // Éteint la LED
		} else if (T >= MaxTemp) {
			char highTempMsg[] = "Alarm High - Temperature = ";
			HAL_UART_Transmit(huart2, (uint8_t*)highTempMsg, strlen(highTempMsg), HAL_MAX_DELAY);
			HAL_GPIO_WritePin(ld2GpioPort, ld2GpioPin, GPIO_PIN_SET);  // Allume la LED
		} else if (T <= MinTemp) {
			char lowTempMsg[] = "Alarm low - Temperature  = ";
			HAL_UART_Transmit(huart2, (uint8_t*)lowTempMsg, strlen(lowTempMsg), HAL_MAX_DELAY);
			HAL_GPIO_WritePin(ld2GpioPort, ld2GpioPin, GPIO_PIN_SET);  // Allume la LED
		}

		// Affiche la température sur la console
		snprintf(output, sizeof(output), "%5.2f", T);
		HAL_UART_Transmit(huart2, (uint8_t*)output, strlen(output), HAL_MAX_DELAY);
		HAL_Delay(1000);  // Pause d'une seconde
	}
}
