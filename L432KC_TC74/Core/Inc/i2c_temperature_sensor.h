/*
 * i2c_temperature_sensor.h
 *
 *  Created on: Mar 8, 2025
 *      Author: Joel
 */

#ifndef I2C_TEMPERATURE_SENSOR_H
#define I2C_TEMPERATURE_SENSOR_H

#include "stdint.h"
#include "i2c_temperature_sensor.h"
#include <stdio.h>
#include <string.h>
#include "stm32l4xx_hal.h"

// I2C address of the TC74 sensor
#define TC74_ADDRESS 0x48

// Command to read the temperature from the TC74 sensor
#define TC74_READ_TEMP_COMMAND 0x00

/**
 * @brief Initializes the temperature sensor.
 *
 * @param huart Pointer to the UART_HandleTypeDef structure for UART communication.
 * @param hi2c Pointer to the I2C_HandleTypeDef structure for I2C communication.
 */
void initTemperatureSensor(UART_HandleTypeDef *huart, I2C_HandleTypeDef *hi2c);

/**
 * @brief Reads and prints the temperature from the TC74 sensor.
 *
 * This function sends a read temperature command to the TC74 sensor, reads the response,
 * converts the raw temperature to degrees Celsius, displays the temperature on the
 * UART console, and then waits before reading the temperature again.
 */
void readAndPrintTemperature(void);

/**
 * @brief Performs an I2C bus scan and displays the addresses of available devices.
 *
 * This function displays the addresses of available I2C devices on the UART console.
 */
void I2C_Scan(void);

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
void Error(void);

#endif /* I2C_TEMPERATURE_SENSOR_H */
