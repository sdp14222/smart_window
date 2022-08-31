/*
 * Character_LCD.h
 *
 *  Created on: Mar 21, 2022
 *      Author: SangDon Park
 */

#ifndef INC_CHARACTER_LCD_H_
#define INC_CHARACTER_LCD_H_

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"


/********* OP_INTERFACE *********
 * 0 : 4 bits 	// Using D4 ~ D7 PIN
 * 1 : 8 bits	// Using D0 ~ D7 PIN
 */
#define OP_INTERFACE 0		// 0 : 4 bits, 1 : 8 bits


/********* CLCD_GPIO_PIN_CONFIG *********/
#define CLCD_PIN_RS_TYPE	GPIOC
#define CLCD_PIN_RS_NUM		GPIO_PIN_13
#define CLCD_PIN_RW_TYPE	GPIOC
#define CLCD_PIN_RW_NUM		GPIO_PIN_14
#define CLCD_PIN_E_TYPE		GPIOC
#define CLCD_PIN_E_NUM		GPIO_PIN_15

#define CLCD_PIN_D7_TYPE	GPIOB
#define CLCD_PIN_D7_NUM		GPIO_PIN_4
#define CLCD_PIN_D6_TYPE	GPIOB
#define CLCD_PIN_D6_NUM		GPIO_PIN_3
#define CLCD_PIN_D5_TYPE	GPIOB
#define CLCD_PIN_D5_NUM		GPIO_PIN_1
#define CLCD_PIN_D4_TYPE	GPIOB
#define CLCD_PIN_D4_NUM		GPIO_PIN_0

#if OP_INTERFACE == 1
#define CLCD_PIN_D3_TYPE	GPIOB
#define CLCD_PIN_D3_NUM		GPIO_PIN_8
#define CLCD_PIN_D2_TYPE	GPIOB
#define CLCD_PIN_D2_NUM		GPIO_PIN_7
#define CLCD_PIN_D1_TYPE	GPIOB
#define CLCD_PIN_D1_NUM		GPIO_PIN_6
#define CLCD_PIN_D0_TYPE	GPIOB
#define CLCD_PIN_D0_NUM		GPIO_PIN_5
#endif

/********* INSTRUCTIONS_INIT_CONFIG *********/

/********* ENTRY_MODE_SET_INIT_CONFIG *********/
#define CLCD_I_EMS_I_D	1		// 0 = DECREMENT,	1 = INCREMENT
#define CLCD_I_EMS_S	0		// 0 = No Shift,	1 = Accompanies display shift

/********* DISPLAY_ON_OFF_INIT_CONFIG *********/
#define CLCD_I_DOC_D	1		// 0 = Display off,		1 = Display on
#define CLCD_I_DOC_C	1		// 0 = Cursor off,		1 = Cursor on
#define CLCD_I_DOC_B	0		// 0 = Blinking off,	1 = Blinking on

/********* CURSOR_OR_DISPLAY_SHIFT_INIT_CONFIG *********/
#define CLCD_I_CODS_S_C	0		// 0 = Cursor move,			1 = Display shift
#define CLCD_I_CODS_R_L	0		// 0 = Shift to the left,	1 = Shift to the right

/********* FUNTION_SET_INIT_CONFIG *********/
#define CLCD_I_FS_D_L	OP_INTERFACE		// 0 = 4 bits,		1 = 8 bits
#define CLCD_I_FS_N		1					// 0 = 1 line,		1 = 2 lines
#define CLCD_I_FS_F		0					// 0 = 5x8 dots,	1 = 5x10 dots

typedef union
{
	struct
	{
		uint8_t i_d : 1;
		uint8_t s : 1;
	};
	uint8_t bits;
} CLCD_EMS;

typedef union
{
	struct
	{
		uint8_t d : 1;
		uint8_t c : 1;
		uint8_t b : 1;
	};
	uint8_t bits;
} CLCD_DOC;

typedef union
{
	struct
	{
		uint8_t s_c : 1;
		uint8_t r_l : 1;
	};
	uint8_t bits;
} CLCD_CODS;

typedef struct
{
	GPIO_TypeDef* lcd_gpio_type;
	uint16_t pin_num;
} CLCD_PIN;


/********* CLCD_PIN_SELECT *********/
typedef enum
{
	CLCD_PIN_S_RS            = ((uint16_t)0x400),
	CLCD_PIN_S_RW            = ((uint16_t)0x200),
	CLCD_PIN_S_DB7           = ((uint16_t)0x080),
	CLCD_PIN_S_DB6           = ((uint16_t)0x040),
	CLCD_PIN_S_DB5           = ((uint16_t)0x020),
	CLCD_PIN_S_DB4           = ((uint16_t)0x010),
	CLCD_PIN_S_DB3           = ((uint16_t)0x008),
	CLCD_PIN_S_DB2           = ((uint16_t)0x004),
	CLCD_PIN_S_DB1           = ((uint16_t)0x002),
	CLCD_PIN_S_DB0           = ((uint16_t)0x001),
	CLCD_PIN_S_ALL_RESET     = ((uint16_t)0x000)
} CLCD_PIN_S;

/********* CLCD_Entry_Mode_Set_SELECT *********/
typedef enum
{
	CLCD_EMS_E_I_D  = ((uint8_t)0x01),
	CLCD_EMS_E_S    = ((uint8_t)0x02),
	CLCD_EMS_E_ALL  = ((uint8_t)0x03),
	CLCD_EMS_E_NONE = ((uint8_t)0x00),
	CLCD_EMS_E_INIT = (\
			  ((CLCD_I_EMS_I_D << CLCD_EMS_E_I_D) \
			| (CLCD_I_EMS_S << CLCD_EMS_E_S)) \
			>> 1)
} CLCD_EMS_E;

typedef enum
{
	CLCD_DOC_E_D    = ((uint8_t)0x01),
	CLCD_DOC_E_C    = ((uint8_t)0x02),
	CLCD_DOC_E_B    = ((uint8_t)0x04),
	CLCD_DOC_E_ALL  = ((uint8_t)0x07),
	CLCD_DOC_E_NONE = ((uint8_t)0x00),
	CLCD_DOC_E_INIT = (\
			  ((CLCD_I_DOC_D << CLCD_DOC_E_D) \
			| (CLCD_I_DOC_C << CLCD_DOC_E_C) \
			| (CLCD_I_DOC_B << CLCD_DOC_E_B)) \
			>> 1)
} CLCD_DOC_E;

/********* CLCD_Cursor_Or_Display_Shift_SELECT *********/
typedef enum
{
	CLCD_CODS_E_S_C  = ((uint8_t)0x01),
	CLCD_CODS_E_R_L	 = ((uint8_t)0x02),
	CLCD_CODS_E_ALL  = ((uint8_t)0x03),
	CLCD_CODS_E_NONE = ((uint8_t)0x00)
} CLCD_CODS_E;

typedef enum
{
	CLCD_ADDR_NOT_SET = 0, // Set Current address
	CLCD_ADDR_SET     = 1, // Set row, col address
} CLCD_ADDR_SET_BOOL;

void CLCD_Init(void);
void CLCD_Clear_Display(void);
void CLCD_Return_Home(void);
void CLCD_Entry_Mode_Set(CLCD_EMS_E select);
void CLCD_Cursor_Or_Display_Shift(CLCD_CODS_E select);
void CLCD_Write(CLCD_ADDR_SET_BOOL addr_set, uint16_t row, uint16_t col, const char* str);


#endif /* INC_CHARACTER_LCD_H_ */
