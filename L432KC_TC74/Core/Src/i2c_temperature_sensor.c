/*
 * i2c_temperature_sensor.c
 *
 *  Created on: Mar 8, 2025
 *      Author: Joel
 */

#include "i2c_temperature_sensor.h"

uint8_t temperature_raw; /**< Raw temperature data. */
float temperature_celsius; /**< Temperature in Celsius. */
char clearScreen[] = "\e[1;1H\e[2J"; /**< Command to clear the screen. */
UART_HandleTypeDef *UART_handle; /**< Global variable for UART communication. */
I2C_HandleTypeDef *I2C_handle; /**< Global variable for I2C communication. */

/**
 * @brief Initializes the temperature sensor.
 *
 * @param huart Pointer to the UART_HandleTypeDef structure for UART communication.
 * @param hi2c Pointer to the I2C_HandleTypeDef structure for I2C communication.
 */
void initTemperatureSensor(UART_HandleTypeDef *huart, I2C_HandleTypeDef *hi2c)
{
	UART_handle = huart;
	I2C_handle = hi2c;
	// Add other initializations if necessary
}

/**
 * @brief Reads and prints the temperature from the TC74 sensor.
 *
 * This function sends a read temperature command to the TC74 sensor, reads the response,
 * converts the raw temperature to degrees Celsius, displays the temperature on the
 * UART console, and then waits before reading the temperature again.
 */
void readAndPrintTemperature(void)
{
	uint8_t read_command[1] = {TC74_READ_TEMP_COMMAND};

	// Send the temperature read command to the TC74 sensor
	if (HAL_I2C_Master_Transmit(I2C_handle, TC74_ADDRESS << 1, read_command, sizeof(read_command), HAL_MAX_DELAY) != HAL_OK)
	{
		Error();
	}

	// Wait for a short moment before reading the response
	HAL_Delay(100);

	// Read the raw temperature from the TC74 sensor
	if (HAL_I2C_Master_Receive(I2C_handle, TC74_ADDRESS << 1, &temperature_raw, 1, HAL_MAX_DELAY) != HAL_OK)
	{
		Error();
	}

	// Convert the raw temperature to degrees Celsius
	temperature_celsius = (float)temperature_raw;

	// Display the temperature on the console
	char buffer[50];
	snprintf(buffer, sizeof(buffer), "Temperature : %.2f degC\r\n", temperature_celsius);

	// Clear the screen
	HAL_UART_Transmit(UART_handle, (uint8_t *)clearScreen, strlen(clearScreen), HAL_MAX_DELAY);

	// Transmit the buffer via UART
	for (int i = 0; buffer[i] != '\0'; i++)
	{
		HAL_UART_Transmit(UART_handle, (uint8_t *)&buffer[i], 1, HAL_MAX_DELAY);
	}

	// Wait before reading the temperature again
	HAL_Delay(1000);
}

/**
 * @brief Performs an I2C bus scan and displays the addresses of available devices.
 *
 * This function displays the addresses of available I2C devices on the UART console.
 */
void I2C_Scan(void)
{
	char info[] = "Scanning I2C bus...\r\n";
	HAL_UART_Transmit(UART_handle, (uint8_t *)info, strlen(info), HAL_MAX_DELAY);

	HAL_StatusTypeDef res;
	for (uint16_t i = 0; i < 128; i++)
	{
		res = HAL_I2C_IsDeviceReady(I2C_handle, i << 1, 1, 10);
		if (res == HAL_OK)
		{
			char msg[64];
			snprintf(msg, sizeof(msg), "0x%02X", i);
			HAL_UART_Transmit(UART_handle, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
		}
		else
		{
			HAL_UART_Transmit(UART_handle, (uint8_t *)".", 1, HAL_MAX_DELAY);
		}
	}

	HAL_UART_Transmit(UART_handle, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);
}

/**
 * @brief Error handling function.
 *
 * This function is called in case of an error in the code. Ideally, you should
 * add error handling actions specific to your application, such as logging the error,
 * disabling peripherals, displaying an error message, and/or implementing a recovery strategy.
 *
 * For now, this function does nothing more than being called.
 * It's up to you to add code according to the needs of your application.
 */
void Error(void)
{
	// TODO: Add specific error handling actions here
	// For example, you could log the error, disable peripherals,
	// display an error message, and/or implement a recovery strategy.

	// For now, this function does nothing more than being called.
	// It's up to you to add code according to the needs of your application.
}

