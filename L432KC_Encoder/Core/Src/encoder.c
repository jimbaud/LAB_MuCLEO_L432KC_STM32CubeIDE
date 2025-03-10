/*
 * encoder.c
 *
 *  Created on: Mar 10, 2025
 *      Author: Joel
 */


#include "encoder.h"

static volatile uint16_t encoder_count = 0; /**< Compteur de l'encodeur */
static volatile int encoder_direction = 1;	/**< Direction de l'encodeur */
int current_option = 0;			/**< Option actuelle sélectionnée dans le menu */
uint8_t encoder_flag;				/**< Drapeau d'interruption de l'encodeur */
uint32_t previousMillis = 0;			/**< Variable pour antirebond du bouton poussoir */
uint8_t SW_Button_flag = 0;			/**< État du bouton poussoir */

void ENC_Init(void) {

	HAL_TIM_Encoder_Start_IT(&htim1, TIM_CHANNEL_2); 	/**< Démarrage de l'encodeur */
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);		 /**< Démarrage du PWM */

	/* Initialisation du LCD */
	LCD_Init(LCD_ADDR);
	LCD_clear(LCD_ADDR);
	LCD_put_cur(0, 0, LCD_ADDR);
	LCD_SendString(LCD_ADDR, "  Encoder Menu  ");
	LCD_put_cur(1,0, LCD_ADDR);
	LCD_SendString(LCD_ADDR, "> Turn to enter ");

}

int16_t ENC_GetCount(void) {
	return encoder_count;
}

int ENC_GetDirection(void) {
	return encoder_direction;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)  {
	if (htim->Instance == TIM1) {
		encoder_direction = __HAL_TIM_IS_TIM_COUNTING_DOWN(htim) ? -1 : 1;
		encoder_count += encoder_direction; /**< Mise à jour du compteur */
		encoder_flag = 1;
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	if (GPIO_Pin == SW_Pin && (HAL_GetTick() - previousMillis > 100)) {
		previousMillis = HAL_GetTick();
		SW_Button_flag = (SW_Button_flag == 0) ? 1 : 0;
		if (SW_Button_flag == 0) {
			encoder_flag = 1;
		}
	}
}

void update_screen(uint16_t count) {
	current_option = count % 4;	/**< Sélection de l'option en fonction du compteur */

	LCD_clear(LCD_ADDR);
	LCD_put_cur(0, 1, LCD_ADDR);

	switch (current_option) {
	case 0: LCD_SendString(LCD_ADDR, "1. PWM Adjust"); break;
	case 1: LCD_SendString(LCD_ADDR, "2. Led Full"); break;
	case 2: LCD_SendString(LCD_ADDR, "3. Led Half"); break;
	case 3: LCD_SendString(LCD_ADDR, "4. Led OFF"); break;
	}

	LCD_put_cur(1, 0, LCD_ADDR);
	LCD_SendString(LCD_ADDR, "> Push to apply ");
}

void Execute_Option(void) {

	switch (current_option) {
	case 0: /**< Ajustement du PWM via l'encodeur */
	{
		int pwm_value = encoder_count % 1000;
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pwm_value);
		LCD_put_cur(0, 0, LCD_ADDR);
		LCD_SendString(LCD_ADDR, " Turn to adjust ");
		LCD_put_cur(1,0, LCD_ADDR);
		LCD_SendString(LCD_ADDR, "> Push to exit  ");
	}
	break;
	case 1: /**< LED à pleine intensité */
	{
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,1000);
		LCD_put_cur(1, 0, LCD_ADDR);
		LCD_SendString(LCD_ADDR, "> Push to exit  ");
	}
	break;
	case 2: /**< LED à 50 % d'intensité */
	{
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 500);
		LCD_put_cur(1, 0, LCD_ADDR);
		LCD_SendString(LCD_ADDR, "> Push to exit  ");
	}
	break;
	case 3: /**< LED éteinte */
	{
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
		LCD_put_cur(1, 0, LCD_ADDR);
		LCD_SendString(LCD_ADDR, "> Push to exit  ");
	}
	break;
	}
}

void User_Process(void) {

	if (encoder_flag == 1 && SW_Button_flag == 0) {
		encoder_flag = 0;
		update_screen(ENC_GetCount()); /**< Mise à jour de l'écran */
	}
	if(SW_Button_flag)
	{
		Execute_Option();	/**< Exécution de l'option sélectionnée */
		HAL_Delay(10);		/**< Délai rafraichissement affichage */
	}
}
