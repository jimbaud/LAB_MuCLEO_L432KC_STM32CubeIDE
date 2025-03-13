/*
 * DRV8833_Stepper.c
 *
 *  Created on: Mar 13, 2025
 *      Author: Joel
 */


#include "DRV8833_Stepper.h"

void Stepper_Init(StepperMotor *motor) {
	motor->motorPort[0] = MOTOR_COIL1_PORT_1;
	motor->motorPort[1] = MOTOR_COIL1_PORT_2;
	motor->motorPort[2] = MOTOR_COIL2_PORT_3;
	motor->motorPort[3] = MOTOR_COIL2_PORT_4;

	motor->motorPin[0] = MOTOR_COIL1_PIN_1;
	motor->motorPin[1] = MOTOR_COIL1_PIN_2;
	motor->motorPin[2] = MOTOR_COIL2_PIN_3;
	motor->motorPin[3] = MOTOR_COIL2_PIN_4;

	motor->currentStep = 1;
	motor->step_delay = 0;  // Initialiser à 0, ajusté par la vitesse plus tard
}

static void Stepper_StepGeneric(StepperMotor *motor, int direction, uint16_t speed) {
	motor->step_delay = speed;
	unsigned long now = HAL_GetTick();

	// Tableau de séquence pour une rotation complète en mode pas complet
	const uint8_t step_sequence[4][4] = {
			{1, 0, 1, 0},  // Étape 1
			{0, 1, 1, 0},  // Étape 2
			{0, 1, 0, 1},  // Étape 3
			{1, 0, 0, 1}   // Étape 4
	};

	// Calculer le prochain pas en fonction de la direction
	if (direction == 1) {
		motor->currentStep = (motor->currentStep + 1) % 4;  // CW
	} else {
		motor->currentStep = (motor->currentStep - 1 + 4) % 4;  // CCW
	}

	// Appliquer la séquence de pas
	for (int i = 0; i < 4; i++) {
		if (step_sequence[motor->currentStep][i] == 1) {
			HAL_GPIO_WritePin(motor->motorPort[i], motor->motorPin[i], GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(motor->motorPort[i], motor->motorPin[i], GPIO_PIN_RESET);
		}
	}

	// Délai avant le prochain pas
	while (HAL_GetTick() - now < motor->step_delay) {
	}
}


void Stepper_StepCW_Steps(StepperMotor *motor, uint16_t steps, uint16_t speed) {
	for (uint16_t i = 0; i < steps; i++) {
		Stepper_StepGeneric(motor, 1, speed);  // 1 pour CW
	}
}

void Stepper_StepCCW_Steps(StepperMotor *motor, uint16_t steps, uint16_t speed) {
	for (uint16_t i = 0; i < steps; i++) {
		Stepper_StepGeneric(motor, -1, speed);  // -1 pour CCW
	}
}

static void Stepper_StepGeneric_Half(StepperMotor *motor, int direction, uint16_t speed) {
	motor->step_delay = speed;
	unsigned long now = HAL_GetTick();

	// Séquence de demi-pas (8 étapes)
	const uint8_t half_step_sequence[8][4] = {
			{1, 0, 1, 0},  // Étape 1
			{0, 0, 1, 0},  // Étape 2
			{0, 1, 1, 0},  // Étape 3
			{0, 1, 0, 0},  // Étape 4
			{0, 1, 0, 1},  // Étape 5
			{0, 0, 0, 1},  // Étape 6
			{1, 0, 0, 1},  // Étape 7
			{1, 0, 0, 0}   // Étape 8

	};

	// Calculer le prochain demi-pas en fonction de la direction
	if (direction == 1) {
		motor->currentStep = (motor->currentStep + 1) % 8;  // CW
	} else {
		motor->currentStep = (motor->currentStep - 1 + 8) % 8;  // CCW
	}

	// Appliquer la séquence de demi-pas
	for (int i = 0; i < 4; i++) {
		if (half_step_sequence[motor->currentStep][i] == 1) {
			HAL_GPIO_WritePin(motor->motorPort[i], motor->motorPin[i], GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(motor->motorPort[i], motor->motorPin[i], GPIO_PIN_RESET);
		}
	}

	// Délai avant le prochain demi-pas
	while (HAL_GetTick() - now < motor->step_delay) {
	}
}


void Stepper_StepCW_Half_Steps(StepperMotor *motor, uint16_t steps, uint16_t speed) {
	for (uint16_t i = 0; i < steps; i++) {
		Stepper_StepGeneric_Half(motor, 1, speed);  // CW demi-pas
	}
}

void Stepper_StepCCW_Half_Steps(StepperMotor *motor, uint16_t steps, uint16_t speed) {
	for (uint16_t i = 0; i < steps; i++) {
		Stepper_StepGeneric_Half(motor, -1, speed);  // CCW demi-pas
	}
}

