
/** Put this in the src folder **/

#include <LCD_I2C.h>

extern I2C_HandleTypeDef hi2c1;  // change your handler here accordingly
extern UART_HandleTypeDef huart2;
uint8_t backlight_status = 1 ;

#define PIN_RS    (1 << 0)
#define PIN_EN    (1 << 2)
#define BACKLIGHT_ON (1 << 3)
#define BACKLIGHT_OFF (1 << 2)
#define LCD_DELAY_MS 5
/* LCD BITS */
#define LCD_RW                          1 << 1

#define LCD_D(n)                        1 << n
#define LCD_D_HIGHER(n)                 (n & 0xF0)
#define LCD_D_LOWER(n)                  ((n & 0x0F) << 4)

/* LCD REGISTERS */
#define LCD_INSTRUCTION_R               0
#define LCD_DATA_R                      LCD_RS

/* LCD_INSTRUCTIONS */
#define LCD_CLEAR_DISPLAY               0x01

#define LCD_RETURN_HOME                 0x02

#define LCD_EMS                         0x04
#define LCD_EMS_S                       0x01
#define LCD_EMS_ID                      0x02

#define LCD_DC                          0x08
#define LCD_DC_B                        0x01
#define LCD_DC_C                        0x02
#define LCD_DC_D                        0x04

#define LCD_CDS                         0x10
#define LCD_CDS_RL                      0x04
#define LCD_CDS_SC                      0x08

#define LCD_FS                          0x20
#define LCD_FS_F                        0x04
#define LCD_FS_N                        0x08
#define LCD_FS_DL                       0x10

#define LCD_SET_CGRAM_ADDRESS           0x40
#define LCD_SET_CGRAM_ADDRESS_MASK      0X3F

#define LCD_SET_DDRAM_ADDRESS           0x80
#define LCD_SET_DDRAM_ADDRESS_MASK      0X7F

#define LCD_BUSY_FLAG                   0X80


void I2C_Scan() {
	char info[] = "Scanning I2C bus...\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t*)info, strlen(info), HAL_MAX_DELAY);

	HAL_StatusTypeDef res;
	for(uint16_t i = 0; i < 128; i++) {
		res = HAL_I2C_IsDeviceReady(&hi2c1, i << 1, 1, 10);
		if(res == HAL_OK) {
			char msg[64];
			snprintf(msg, sizeof(msg), "0x%02X", i);
			HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
		} else {
			HAL_UART_Transmit(&huart2, (uint8_t*)".", 1, HAL_MAX_DELAY);
		}
	}

	HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
}

HAL_StatusTypeDef LCD_SendInternal(uint8_t lcd_addr, uint8_t data, uint8_t flags) {
	HAL_StatusTypeDef res;
	for(;;) {
		res = HAL_I2C_IsDeviceReady(&hi2c1, lcd_addr, 1, HAL_MAX_DELAY);
		if(res == HAL_OK)
			break;
	}

	uint8_t up = data & 0xF0;
	uint8_t lo = (data << 4) & 0xF0;

	uint8_t data_arr[4];
	if(backlight_status)
	{
		data_arr[0] = up|flags|BACKLIGHT_ON|PIN_EN;
		data_arr[1] = up|flags|BACKLIGHT_ON;
		data_arr[2] = lo|flags|BACKLIGHT_ON|PIN_EN;
		data_arr[3] = lo|flags|BACKLIGHT_ON;
	}
	else {
		data_arr[0] = up|flags|BACKLIGHT_OFF|PIN_EN;
		data_arr[1] = up|flags|BACKLIGHT_OFF;
		data_arr[2] = lo|flags|BACKLIGHT_OFF|PIN_EN;
		data_arr[3] = lo|flags|BACKLIGHT_OFF;
	}

	res = HAL_I2C_Master_Transmit(&hi2c1, lcd_addr, data_arr, sizeof(data_arr), HAL_MAX_DELAY);
	HAL_Delay(LCD_DELAY_MS);
	return res;
}

void LCD_SendCommand(uint8_t lcd_addr, uint8_t cmd) {
	LCD_SendInternal(lcd_addr, cmd, 0);
}

void LCD_SendData(uint8_t lcd_addr, uint8_t data) {
	LCD_SendInternal(lcd_addr, data, PIN_RS);
}

void LCD_Init(uint8_t lcd_addr) {
	// 4-bit mode, 2 lines, 5x7 format
	LCD_SendCommand(lcd_addr, 0b00110000);
	// display & cursor home (keep this!)
	LCD_SendCommand(lcd_addr, LCD_RETURN_HOME);
	// display on, right shift, underline off, blink off
	LCD_SendCommand(lcd_addr, 0b00001100);
	// clear display (optional here)
	LCD_SendCommand(lcd_addr, LCD_CLEAR_DISPLAY);
}

void LCD_SendString(uint8_t lcd_addr, char *str) {
	while(*str) {
		LCD_SendData(lcd_addr, (uint8_t)(*str));
		str++;
	}
}

void LCD_clear (uint8_t lcd_addr)
{
	LCD_SendCommand(lcd_addr, LCD_CLEAR_DISPLAY);
}

void LCD_put_cur(int row, int col, uint8_t lcd_addr)
{
	switch (row)
	{
	case 0:
		col |= LCD_SET_DDRAM_ADDRESS;
		break;
	case 1:
		col |= 0xC0;
		break;
	}

	LCD_SendCommand (lcd_addr, col);
}


void LCD_return_home(uint8_t lcd_addr)
{
	LCD_SendCommand (lcd_addr, LCD_RETURN_HOME);

}

void LCD_SetAddress(uint8_t lcd_addr, uint8_t add)
{

	if(add > LCD_SET_DDRAM_ADDRESS_MASK)
		return;
	LCD_SendCommand(lcd_addr, LCD_SET_DDRAM_ADDRESS | add);
}

void LCD_Write_String(uint8_t lcd_addr, char* string, uint8_t pos)
{

	int32_t iteration;

	iteration = LCD_SET_DDRAM_ADDRESS_MASK - pos + 1;
	if(iteration > 0){
		LCD_SetAddress(lcd_addr, pos);
		while((*string != '\0') && (iteration > 0)){
			LCD_SendData(lcd_addr, *string);
			string++;
			iteration--;
		}
	}
	else
		return;
}

void LCD_Do_Display_Shift(uint8_t lcd_addr, uint8_t dir)
{

	LCD_SendCommand(lcd_addr, LCD_CDS | LCD_CDS_SC | dir);
}

void LCD_Backlight_On(uint8_t lcd_addr) {

	backlight_status = 1;
	LCD_SendCommand(lcd_addr, 0);
}

void LCD_Backlight_Off(uint8_t lcd_addr) {

	backlight_status = 0;
	LCD_SendCommand(lcd_addr, 0);
}

