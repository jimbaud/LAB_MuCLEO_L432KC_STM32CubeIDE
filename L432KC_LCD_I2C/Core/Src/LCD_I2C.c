/*
 * LCD_I2C.c
 *
 *  Created on: Mar 8, 2025
 *      Author: Joel
 */


#include "LCD_I2C.h"

uint8_t backlight_status = 1;

/* Fonction pour scanner le bus I2C */
void I2C_Scan()
{
	char info[] = "Scanning I2C bus...\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t *)info, strlen(info), HAL_MAX_DELAY);

	HAL_StatusTypeDef res;
	for (uint16_t i = 0; i < 128; i++)
	{
		res = HAL_I2C_IsDeviceReady(&hi2c1, i << 1, 1, 10);
		if (res == HAL_OK)
		{
			char msg[64];
			snprintf(msg, sizeof(msg), "0x%02X", i);
			HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
		}
		else
		{
			HAL_UART_Transmit(&huart2, (uint8_t *)".", 1, HAL_MAX_DELAY);
		}
	}

	HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);
}

/* Fonction interne pour envoyer des données au LCD */
HAL_StatusTypeDef LCD_SendInternal(uint8_t lcd_addr, uint8_t data, uint8_t flags)
{
	HAL_StatusTypeDef res;
	for (;;)
	{
		res = HAL_I2C_IsDeviceReady(&hi2c1, lcd_addr, 1, HAL_MAX_DELAY);
		if (res == HAL_OK)
			break;
	}

	uint8_t up = data & 0xF0;
	uint8_t lo = (data << 4) & 0xF0;

	uint8_t data_arr[4];
	if (backlight_status)
	{
		data_arr[0] = up | flags | BACKLIGHT_ON | PIN_EN;
		data_arr[1] = up | flags | BACKLIGHT_ON;
		data_arr[2] = lo | flags | BACKLIGHT_ON | PIN_EN;
		data_arr[3] = lo | flags | BACKLIGHT_ON;
	}
	else
	{
		data_arr[0] = up | flags | BACKLIGHT_OFF | PIN_EN;
		data_arr[1] = up | flags | BACKLIGHT_OFF;
		data_arr[2] = lo | flags | BACKLIGHT_OFF | PIN_EN;
		data_arr[3] = lo | flags | BACKLIGHT_OFF;
	}

	res = HAL_I2C_Master_Transmit(&hi2c1, lcd_addr, data_arr, sizeof(data_arr), HAL_MAX_DELAY);
	HAL_Delay(LCD_DELAY_MS);
	return res;
}

/* Fonction pour envoyer une commande au LCD */
void LCD_SendCommand(uint8_t lcd_addr, uint8_t cmd)
{
	LCD_SendInternal(lcd_addr, cmd, 0);
}

/* Fonction pour envoyer des données au LCD */
void LCD_SendData(uint8_t lcd_addr, uint8_t data)
{
	LCD_SendInternal(lcd_addr, data, PIN_RS);
}

/* Fonction pour initialiser le LCD */
void LCD_Init(uint8_t lcd_addr)
{
	// Mode 4 bits, 2 lignes, format 5x7
	LCD_SendCommand(lcd_addr, 0b00110000);
	// Retour à la maison de l'affichage et du curseur (à conserver)
	LCD_SendCommand(lcd_addr, LCD_RETURN_HOME);
	// Affichage activé, décalage à droite, soulignement désactivé, clignotement désactivé
	LCD_SendCommand(lcd_addr, 0b00001100);
	// Effacer l'affichage (optionnel ici)
	LCD_SendCommand(lcd_addr, LCD_CLEAR_DISPLAY);
}

/* Fonction pour envoyer une chaîne de caractères au LCD */
void LCD_SendString(uint8_t lcd_addr, char *str)
{
	while (*str)
	{
		LCD_SendData(lcd_addr, (uint8_t)(*str));
		str++;
	}
}

/* Fonction pour effacer l'affichage */
void LCD_Clear(uint8_t lcd_addr)
{
	LCD_SendCommand(lcd_addr, LCD_CLEAR_DISPLAY);
}

/* Fonction pour positionner le curseur */
void LCD_SetCursor(int row, int col, uint8_t lcd_addr)
{
	uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54}; // Adresses de début de chaque ligne
	if (row >= 4)
	{
		row = 0; // Assurer que nous n'allons pas au-delà de 4 lignes
	}
	LCD_SendCommand(lcd_addr, LCD_SET_DDRAM_ADDRESS | (col + row_offsets[row]));
}

/* Fonction pour retourner le curseur à la position de départ */
void LCD_ReturnHome(uint8_t lcd_addr)
{
	LCD_SendCommand(lcd_addr, LCD_RETURN_HOME);
}

/* Fonction pour définir l'adresse du curseur */
void LCD_SetAddress(uint8_t lcd_addr, uint8_t add)
{
	if (add > LCD_SET_DDRAM_ADDRESS_MASK)
		return;
	LCD_SendCommand(lcd_addr, LCD_SET_DDRAM_ADDRESS | add);
}

/* Fonction pour écrire une chaîne de caractères à une position donnée */
void LCD_WriteStringAt(uint8_t lcd_addr, char *string, uint8_t pos)
{
	int32_t iteration = LCD_SET_DDRAM_ADDRESS_MASK - pos + 1;
	if (iteration > 0)
	{
		LCD_SetAddress(lcd_addr, pos);
		while ((*string != '\0') && (iteration > 0))
		{
			LCD_SendData(lcd_addr, *string);
			string++;
			iteration--;
		}
	}
	else
		return;
}

/* Fonction pour effectuer un décalage d'affichage */
void LCD_DisplayShift(uint8_t lcd_addr, uint8_t direction)
{
	uint8_t command;
	if (direction == LCD_SHIFT_RIGHT)
	{
		command = 0x1C; // Commande pour décalage à droite
	}
	else if (direction == LCD_SHIFT_LEFT)
	{
		command = 0x18; // Commande pour décalage à gauche
	}
	LCD_SendCommand(lcd_addr, command);
}


/* Fonction pour activer le rétroéclairage */
void LCD_BacklightOn(uint8_t lcd_addr)
{
	backlight_status = 1;
	LCD_SendCommand(lcd_addr, 0);
}

/* Fonction pour désactiver le rétroéclairage */
void LCD_BacklightOff(uint8_t lcd_addr)
{
	backlight_status = 0;
	LCD_SendCommand(lcd_addr, 0);
}
