/*
 * motor_control.c
 *
 *  Created on: Mar 13, 2025
 *      Author: Joel
 */


// motor_control.c

#include "motor_control.h"

static TIM_HandleTypeDef *motorTimHandle;
static uint32_t motorChannel1;
static uint32_t motorChannel2;
static MotorDirection motorDirection = MOTOR_STOPPED;

void Motor_Init(TIM_HandleTypeDef *htim, uint32_t channel1, uint32_t channel2) {
	motorTimHandle = htim;
	motorChannel1 = channel1;
	motorChannel2 = channel2;

	// Add motor control initialization code here

	Motor_Stop();
}

void Motor_SetSpeed(uint8_t speed, MotorDirection direction) {
	// Ensure that the speed is in the valid range (0 to 100)
	speed = (speed > 100) ? 100 : speed;

	// Calculate the PWM duty cycle based on the speed
	uint16_t dutyCycle = (speed * __HAL_TIM_GET_AUTORELOAD(motorTimHandle)) / 100;

	// Set the PWM duty cycle based on the specified direction
	if (direction == MOTOR_CLOCKWISE) {
		HAL_TIM_PWM_Start(motorTimHandle, motorChannel1);
		HAL_TIM_PWM_Stop(motorTimHandle, motorChannel2);
		__HAL_TIM_SET_COMPARE(motorTimHandle, motorChannel1, dutyCycle);
	} else if (direction == MOTOR_COUNTER_CLOCKWISE) {
		HAL_TIM_PWM_Start(motorTimHandle, motorChannel2);
		HAL_TIM_PWM_Stop(motorTimHandle, motorChannel1);
		__HAL_TIM_SET_COMPARE(motorTimHandle, motorChannel2, dutyCycle);
	} else {
		// Stop the motor if direction is stopped
		Motor_Stop();
	}

	// Update the motor direction
	motorDirection = direction;
}

void Motor_RunClockwise(void) {
	motorDirection = MOTOR_CLOCKWISE;
	HAL_TIM_PWM_Start(motorTimHandle, motorChannel1);
	HAL_TIM_PWM_Stop(motorTimHandle, motorChannel2);
}

void Motor_RunCounterClockwise(void) {
	motorDirection = MOTOR_COUNTER_CLOCKWISE;
	HAL_TIM_PWM_Start(motorTimHandle, motorChannel2);
	HAL_TIM_PWM_Stop(motorTimHandle, motorChannel1);
}

void Motor_Stop(void) {
	motorDirection = MOTOR_STOPPED;
	HAL_TIM_PWM_Stop(motorTimHandle, motorChannel1);
	HAL_TIM_PWM_Stop(motorTimHandle, motorChannel2);
}

void Motor_ApplyBrake(void) {
	// Set both PWM channels to 100% duty cycle for active braking
	__HAL_TIM_SET_COMPARE(motorTimHandle, motorChannel1, __HAL_TIM_GET_AUTORELOAD(motorTimHandle));
	__HAL_TIM_SET_COMPARE(motorTimHandle, motorChannel2, __HAL_TIM_GET_AUTORELOAD(motorTimHandle));

	// Update the motor direction
	motorDirection = MOTOR_STOPPED;
}
