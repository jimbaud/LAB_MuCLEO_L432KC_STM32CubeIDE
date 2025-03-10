
#include <string.h>
#include <stdio.h>
#include "stm32l4xx_hal.h"


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_I2C_H
#define __LCD_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

void I2C_Scan();

HAL_StatusTypeDef LCD_SendInternal(uint8_t lcd_addr, uint8_t data, uint8_t flags);

void LCD_SendCommand(uint8_t lcd_addr, uint8_t cmd);

void LCD_SendData(uint8_t lcd_addr, uint8_t data);

void LCD_Init(uint8_t lcd_addr);

void LCD_SendString(uint8_t lcd_addr, char *str);

void LCD_clear (uint8_t lcd_addr);

void LCD_put_cur(int row, int col, uint8_t lcd_addr);  // put cursor at the entered position row (0 or 1), col (0-15);

void LCD_return_home(uint8_t lcd_addr)  ;

void LCD_Set_Address(uint8_t lcd_addr, uint8_t add);

void LCD_Write_String(uint8_t lcd_addr, char* string, uint8_t pos);

void LCD_Do_Display_Shift(uint8_t lcd_addr, uint8_t dir);

void LCD_Backlight_On(uint8_t lcd_addr);

void LCD_Backlight_Off(uint8_t lcd_addr);

#ifdef __cplusplus
}
#endif

#endif
