#ifndef __OLED_H__
#define __OLED_H__

#include "spi.h"
#include "main.h"

#define DRIVER_VERSION    "OLED v1.4.M"

#define OLED_RES_Clr HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_RESET);
#define OLED_RES_Set HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_SET);

#define OLED_DC_Clr HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET);
#define OLED_DC_Set HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET);

#define OLED_CS_Clr HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
#define OLED_CS_Set HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);

#define Hollow 0
#define Solid 1

#define WHITE 1
#define BLACK 0

#define Max(a,b)      ((a) > (b) ? (a) : (b))
#define Min(a,b)      ((a) < (b) ? (a) : (b))
#define OLED_Abs(a)   ((a) < 0 ? (-a) : (a))

#define OLED_XOR_LAYER        1
#define OLED_DATA_LAYER       0

//在下面写函数声明
void OLED_WR_CMD(unsigned char cmd);
void OLED_WR_Data(unsigned char* data);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_SetWindows(int sx, int sy, uint16_t width, uint16_t height);
void OLED_GRAM_Transmit(void);
void OLED_DrawPoint(int x,int y,int mode);
void OLED_ShowChar(int x,int y,char* chr,int Size,int mode);
void OLED_ShowBMP(int x,int y,uint8_t *BMP,int BMP_Heigth,int BMP_Width,int mode);
void OLED_Negation(int ixs,int iys,int ixe,int iye,int mode);

#endif
