/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char clearScreen[] = "\e[1;1H\e[2J"; // Commande ANSI pour effacer l'écran du terminal
char output[100]; // Tampon de sortie pour l'UART
uint8_t Nbr_Channel = 4; // Nombre total de canaux ADC à convertir

/* Sélection d'un canal ADC spécifique (par défaut : ADC_CHANNEL_5) */
uint32_t selectedChannel = ADC_CHANNEL_5;

/* Décommenter la ligne suivante pour activer le mode "un seul canal" */
//#define SINGLE_CHANNEL_MODE

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_ADC1_Init();
	/* USER CODE BEGIN 2 */

	// Envoi de la commande pour effacer l'écran du terminal via l'UART
	HAL_UART_Transmit(&huart2, (uint8_t*)clearScreen, strlen(clearScreen), HAL_MAX_DELAY);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{

#ifdef SINGLE_CHANNEL_MODE		/* MODE : CONVERSION D'UN SEUL CANAL ADC */

		/* Configuration du canal ADC sélectionné */
		ADC_ChannelConfTypeDef sConfig = {0};
		sConfig.Channel = selectedChannel;
		sConfig.Rank = ADC_REGULAR_RANK_1;
		sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
		sConfig.SingleDiff = ADC_SINGLE_ENDED;
		sConfig.OffsetNumber = ADC_OFFSET_NONE;
		sConfig.Offset = 0;

		if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
			Error_Handler();
		}

		/* Démarrage de la conversion ADC */
		HAL_ADC_Start(&hadc1);

		/* Attente de la fin de la conversion */
		if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
			uint32_t resultatADC = HAL_ADC_GetValue(&hadc1);
			float tensionADC = (resultatADC * 3.3) / 4096.0;

			/* Affichage du résultat sur le terminal série */
			snprintf(output, sizeof(output), "ADC Single Channel Mode: Value = %ld (%.2f V)\r\n", resultatADC, tensionADC);
			HAL_UART_Transmit(&huart2, (uint8_t*)output, strlen(output), HAL_MAX_DELAY);
		}

		/* Arrêt de l'ADC */
		HAL_ADC_Stop(&hadc1);

#else		/* MODE : CONVERSION DE TOUS LES CANAUX ADC */

		for (uint8_t i = 0; i < Nbr_Channel; i++) {
			/* Démarrage de la conversion ADC */
			HAL_ADC_Start(&hadc1);

			/* Attente de la fin de la conversion */
			if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
				/* Récupération de la valeur convertie */
				uint32_t resultatADC = HAL_ADC_GetValue(&hadc1);

				/* Conversion de la valeur ADC en tension */
				float tensionADC = (resultatADC * 3.3) / 4096.0;

				/* Mise en forme et affichage du résultat via UART */
				snprintf(output, sizeof(output), "ADC Multi-Channel Mode - IN%d: %ld (%.2f V)\r\n", i, resultatADC, tensionADC);
				HAL_UART_Transmit(&huart2, (uint8_t*)output, strlen(output), HAL_MAX_DELAY);
			}
		}

		/* Arrêt de l'ADC après conversion */
		HAL_ADC_Stop(&hadc1);

#endif

		/* Pause de 500 ms avant la prochaine série de mesures */
		HAL_Delay(500);

		/* Effacement de l'écran du terminal pour afficher les nouveaux résultats */
		HAL_UART_Transmit(&huart2, (uint8_t*)clearScreen, strlen(clearScreen), HAL_MAX_DELAY);

		/* Indicateur de fonctionnement : bascule de la LED LD3 */
		HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 8;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
