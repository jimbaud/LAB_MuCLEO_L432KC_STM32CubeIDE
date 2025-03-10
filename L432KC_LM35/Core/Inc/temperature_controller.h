/*
 * temperature_controller.h
 *
 *  Created on: Mar 8, 2025
 *      Author: Joel
 */

#ifndef TEMPERATURE_CONTROLLER_H
#define TEMPERATURE_CONTROLLER_H

#include "stm32l4xx_hal.h"

void initTemperatureController(UART_HandleTypeDef *huart, ADC_HandleTypeDef *hadc, GPIO_TypeDef *gpioPort, uint16_t gpioPin);

void startTemperatureController(void);

#endif /* TEMPERATURE_CONTROLLER_H */

