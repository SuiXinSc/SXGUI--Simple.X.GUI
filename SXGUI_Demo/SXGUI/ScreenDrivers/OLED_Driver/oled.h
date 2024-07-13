#ifndef __OLED_H__
#define __OLED_H__

#include "spi.h"
#include "main.h"

#define OLED_RES_Clr HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_RESET);
#define OLED_RES_Set HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_SET);

#define OLED_DC_Clr HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET);
#define OLED_DC_Set HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET);

#define OLED_CS_Clr HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
#define OLED_CS_Set HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);

#define Hollow 0
#define Solid 1

#define Max(a,b)      ((a) > (b) ? (a) : (b))
#define Min(a,b)      ((a) < (b) ? (a) : (b))
#define OLED_Abs(a)   ((a) < 0 ? (-a) : (a))

#define OLED_IMAGE_LAYER      0
#define OLED_GRATING_LAYER    1
#define OLED_DATA_LAYER       2
#define OLED_ALL_LAYER        0xAD

//在下面写函数声明
void OLED_WR_CMD(unsigned char cmd);
void OLED_WR_Data(unsigned char* data);
void OLED_Init(void);
void OLED_Clear(int layer);
void OLED_GRAM_Transmit(void);
void OLED_DrawPoint(int x,int y,int mode,int layer);
void OLED_ShowChar(int x,int y,char* chr,int Size,int mode,int layer);
void OLED_ShowBMP(int x,int y,uint8_t *BMP,int BMP_Heigth,int BMP_Width,int mode,int layer);
void OLED_Negation(int ixs,int iys,int ixe,int iye,int mode);

void OLED_Grating_Count(void);
void OLED_UpDateToGRAM(uint8_t Data[1024],int layer);
void OLED_GetGRAM(uint8_t *Data,int layer);

#endif
