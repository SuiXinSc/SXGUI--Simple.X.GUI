#ifndef __LCD_H
#define __LCD_H

#include "main.h"

#define DRIVER_VERSION    "LCD v1.0"

#define FSMC_BASE   ((uint32_t)(0x60000000))

#define LCD_REG    (*(uint8_t*)((uint32_t)(0x60000000)))
#define LCD_DAT    (*(uint8_t*)((uint32_t)(0x60010000)))

#define LCD_RST_SET   HAL_GPIO_WritePin(GPIOA, LCD_RES_Pin, GPIO_PIN_SET);
#define LCD_RST_CLR   HAL_GPIO_WritePin(GPIOA, LCD_RES_Pin, GPIO_PIN_RESET);

//一些颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	   0x001F  
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40
#define BRRED 			 0XFC07
#define GRAY  			 0X8430

#define DARKBLUE      	 0X01CF
#define LIGHTBLUE      	 0X7D7C
#define GRAYBLUE       	 0X5458
 
#define LIGHTGREEN     	 0X841F
#define LGRAY 					 0XC618

#define LGRAYBLUE        0XA651
#define LBBLUE           0X2B12

extern uint16_t POINT_COLOR;

typedef struct  
{										    
	uint16_t width;
	uint16_t height;
	uint16_t id;
	uint8_t  dir;
	uint16_t	wramcmd;
	uint16_t setxcmd;
	uint16_t  setycmd;
}_lcd_dev; 	

#define MAX(a,b)  ((a)>(b)?(a):(b))
#define MIN(a,b)  ((a)<(b)?(a):(b))

int LCD_Init(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Clear(uint16_t Color);
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);

void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue);
void LCD_WriteRAM_Prepare(void);
void LCD_SetWindow(int sx,int sy,uint16_t width,uint16_t height);
void LCD_Display_Dir(uint8_t dir);

void LCD_Flash(void);
void LCD_DrawPoint(int x,int y,uint16_t color);
void LCD_ShowImage(int x,int y,int width,int height,uint8_t* Image);
void LCD_ShowChar(int x,int y,char* num,uint8_t size,uint16_t color);

#endif
