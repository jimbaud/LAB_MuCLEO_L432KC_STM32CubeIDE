/*
 * LCD_I2C.h
 *
 *  Created on: Mar 8, 2025
 *      Author: Joel
 */

#ifndef LCD_I2C_H
#define LCD_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdio.h>
#include "stm32l4xx_hal.h"

/* Declaration aadresse LCD */
#define LCD_ADDR (0x27 << 1)

/* Déclaration des variables globales */
extern I2C_HandleTypeDef hi2c1; // Modifiez votre gestionnaire ici si nécessaire
extern UART_HandleTypeDef huart2;
extern uint8_t backlight_status;

/* Constantes pour les broches et les instructions LCD */
#define PIN_RS (1 << 0)
#define PIN_EN (1 << 2)
#define BACKLIGHT_ON (1 << 3)
#define BACKLIGHT_OFF (1 << 2)
#define LCD_DELAY_MS 5

/* Constantes pour les bits de données LCD */
#define LCD_RW (1 << 1)
#define LCD_D(n) (1 << n)
#define LCD_D_HIGHER(n) (n & 0xF0)
#define LCD_D_LOWER(n) ((n & 0x0F) << 4)

/* Constantes pour les décalages LCD */
#define LCD_SHIFT_RIGHT 0x1C
#define LCD_SHIFT_LEFT 0x18

/* Constantes pour les instructions LCD */
#define LCD_CLEAR_DISPLAY 0x01
#define LCD_RETURN_HOME 0x02
#define LCD_EMS 0x04
#define LCD_EMS_S 0x01
#define LCD_EMS_ID 0x02
#define LCD_DC 0x08
#define LCD_DC_B 0x01
#define LCD_DC_C 0x02
#define LCD_DC_D 0x04
#define LCD_CDS 0x10
#define LCD_CDS_RL 0x04
#define LCD_CDS_SC 0x08
#define LCD_FS 0x20
#define LCD_FS_F 0x04
#define LCD_FS_N 0x08
#define LCD_FS_DL 0x10
#define LCD_SET_CGRAM_ADDRESS 0x40
#define LCD_SET_CGRAM_ADDRESS_MASK 0X3F
#define LCD_SET_DDRAM_ADDRESS 0x80
#define LCD_SET_DDRAM_ADDRESS_MASK 0X7F
#define LCD_BUSY_FLAG 0X80

void I2C_Scan();

HAL_StatusTypeDef LCD_SendInternal(uint8_t lcd_addr, uint8_t data, uint8_t flags);

void LCD_SendCommand(uint8_t lcd_addr, uint8_t cmd);

void LCD_SendData(uint8_t lcd_addr, uint8_t data);

void LCD_Init(uint8_t lcd_addr);

void LCD_SendString(uint8_t lcd_addr, char *str);

void LCD_Clear(uint8_t lcd_addr);

void LCD_SetCursor(int row, int col, uint8_t lcd_addr);

void LCD_ReturnHome(uint8_t lcd_addr);

void LCD_SetAddress(uint8_t lcd_addr, uint8_t add);

void LCD_WriteStringAt(uint8_t lcd_addr, char *string, uint8_t pos);

void LCD_DisplayShift(uint8_t lcd_addr, uint8_t dir);

void LCD_BacklightOn(uint8_t lcd_addr);

void LCD_BacklightOff(uint8_t lcd_addr);

#ifdef __cplusplus
}
#endif

#endif
