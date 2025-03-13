/*
 * motor_control.h
 *
 *  Created on: Mar 13, 2025
 *      Author: Joel
 */
// motor_control.h

#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "stm32l4xx_hal.h"

typedef enum {
	MOTOR_CLOCKWISE,
	MOTOR_COUNTER_CLOCKWISE,
	MOTOR_STOPPED
} MotorDirection;

/**
 * @brief Initialize the motor control.
 *
 * @param htim Pointer to the TIM_HandleTypeDef structure for PWM configuration.
 * @param channel1 PWM channel used for motor control (e.g., TIM_CHANNEL_1).
 * @param channel2 PWM channel used for motor control (e.g., TIM_CHANNEL_2).
 */
void Motor_Init(TIM_HandleTypeDef *htim, uint32_t channel1, uint32_t channel2);

/**
 * @brief Set the speed and direction of the motor.
 *
 * @param speed Desired speed for the motor (0 to 100).
 * @param direction Desired direction for the motor.
 */
void Motor_SetSpeed(uint8_t speed, MotorDirection direction);

/**
 * @brief Run the motor in the clockwise direction.
 */
void Motor_RunClockwise(void);

/**
 * @brief Run the motor in the counter-clockwise direction.
 */
void Motor_RunCounterClockwise(void);

/**
 * @brief Stop the motor.
 */
void Motor_Stop(void);

void Motor_ApplyBrake(void);

#endif /* MOTOR_CONTROL_H */
