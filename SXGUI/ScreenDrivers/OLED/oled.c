/********************************************************
* @file     :oled.c
* @writer   :岁心(SuiXinSc)
*
* @bilibili :https://space.bilibili.com/3494359452354953
* @Gtihub   :https://github.com/SuiXinSc
* @CSDN     :https://blog.csdn.net/m0_73677866
*
* @attention:
*   Follow MIT License
*   Copyright (c) 2024 SuiXinSc
*********************************************************/

#include "oled.h"
#include "oledfont.h"
#include "stdio.h"
#include "gbk_font.h"
#include <math.h>
#include "stdlib.h"
#include "string.h"

uint8_t Layer=OLED_DATA_LAYER;

unsigned char GRAM[2][1024]; //定义显存，适用于SRAM较大的单片机

struct Windows{
  uint16_t x,y;
  uint16_t width,height;
} Windows = {0};

//写命令
void OLED_WR_CMD(unsigned char cmd) {
  /*SPI:*/
  OLED_CS_Clr; //拉低片选表示选中OLED
  OLED_DC_Clr; //把 D/C 引脚拉低以表示命令
  HAL_SPI_Transmit(&hspi1, &cmd, 1, 1);
  OLED_CS_Set;
  /*IIC:
  HAL_I2C_Mem_Write(&hi2c1 ,0x78,0x00,I2C_MEMADD_SIZE_8BIT,&cmd,1,1000);
  */
}

//写数据
void OLED_WR_Data(unsigned char* data) {
  /*SPI:*/
  OLED_CS_Clr; //拉低片选表示选中OLED
  OLED_DC_Set; //把 D/C 引脚拉高以表示数据
  HAL_SPI_Transmit(&hspi1, data, 1024, 1);
  OLED_CS_Set;
  /*IIC:
  HAL_I2C_Mem_Write(&hi2c1 ,0x78,0x40,I2C_MEMADD_SIZE_8BIT,data,1024,1000);
  */
}


void OLED_Init(void) {
  Windows.x = 0;
  Windows.y = 0;
  Windows.width = 128;
  Windows.height = 64;
  
  HAL_Delay(200); //延时防止卡死
  /*使用水平寻址模式*/
  OLED_RES_Clr;
  HAL_Delay(80);
  OLED_RES_Set; //复位OLED
  OLED_WR_CMD(0xAE); //display off
  OLED_WR_CMD(0x20);	//Set Memory Addressing Mode
  OLED_WR_CMD(0x00);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
  OLED_WR_CMD(0xB0);	//Set Page Start Address for Page Addressing Mode,0-7
  OLED_WR_CMD(0xC8);	//Set COM Output Scan Direction
  OLED_WR_CMD(0x00); //---set low column address
  OLED_WR_CMD(0x10); //---set high column address
  OLED_WR_CMD(0x40); //--set start line address
  OLED_WR_CMD(0x81); //--set contrast control register
  OLED_WR_CMD(0x80); //亮度调节 0x00~0xff
  OLED_WR_CMD(0xA1); //--set segment re-map 0 to 127
  OLED_WR_CMD(0xA6); //--set normal display
  OLED_WR_CMD(0xA8); //--set multiplex ratio(1 to 64)
  OLED_WR_CMD(0x3F); //
  OLED_WR_CMD(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
  OLED_WR_CMD(0xD3); //-set display offset
  OLED_WR_CMD(0x00); //-not offset
  OLED_WR_CMD(0xD5); //--set display clock divide ratio/oscillator frequency
  OLED_WR_CMD(0xF0); //--set divide ratio
  OLED_WR_CMD(0xD9); //--set pre-charge period
  OLED_WR_CMD(0x22); //
  OLED_WR_CMD(0xDA); //--set com pins hardware configuration
  OLED_WR_CMD(0x12);
  OLED_WR_CMD(0xDB); //--set vcomh
  OLED_WR_CMD(0x20); //0x20,0.77xVcc
  OLED_WR_CMD(0x8D); //--set DC-DC enable
  OLED_WR_CMD(0x14); //
  OLED_WR_CMD(0xAF); //--turn on oled panel
}

/*
功能描述：清屏
参数：layer: 要操作的层
*/
void OLED_Clear(void) {
  memset(GRAM[Layer], 0, 1024);
}

void OLED_SetWindows(int sx, int sy, uint16_t width, uint16_t height){
  Windows.x=Max(sx,0),Windows.y=Max(sy,0);
  Windows.x=Min(Windows.x,127);
  Windows.y=Min(Windows.y,63);
  Windows.width=(Windows.x+width<128?width:128-Windows.x);
  Windows.height=(Windows.y+height<64?height:64-Windows.y);
}

/*
功能描述：在OLED中画点
参数：x：x坐标；y：y坐标；mode:0,反白显示;1,正常显示;
*/
void OLED_DrawPoint(int x, int y, int mode) {
  if(x<0||x>=Windows.width||y<0||y>=Windows.height)return;
  //判断数据合法性
  
  y += Windows.y;
  x += Windows.x;
  //跳过缓冲区并将坐标移到窗口
  int line_y, pixel_y, temp; //定义临时变量
  line_y = y / 8;  //计算对应显存行数
  pixel_y = y % 8; //计算对应行的像素
  temp = 0x01 << pixel_y; //通过移位得到数据
  if(mode == 1) {
    GRAM[Layer][line_y * 128 + x] |= temp; //通过或运算来更新显存
  } else {
    GRAM[Layer][line_y * 128 + x] ^= 0xFF;
    GRAM[Layer][line_y * 128 + x] |= temp;
    GRAM[Layer][line_y * 128 + x] ^= 0xFF;
  }
}

/*
功能描述：显示一个字符(包含字库中有的中文)
参数：x(0~127)，y(0~63)：坐标；chr：字符；Size：尺寸12（6*8）、16（8*16）；mode:0,反白显示;1,正常显示;layer: 要操作的层
*/
void OLED_ShowChar(int x, int y, char* chr, int Size, int mode) {
  if((int)chr[0] < 128) {
    uint8_t temp, t1, t;
    int y0 = y;
    uint8_t csize = (Size / 8 + ((Size % 8) ? 1 : 0)) * (Size / 2);		//得到字体一个字符对应点阵集所占的字节数
    unsigned char num = chr[0] - ' '; //得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
    for(t = 0; t < csize; t++) {
      if(Size == 12)temp = asc2_1206[num][t]; 	 	//调用1206字体
      else if(Size == 16)temp = asc2_1608[num][t];	//调用1608字体
      else return;								//没有的字库
      for(t1 = 0; t1 < 8; t1++) {
        if(temp & 0x80)OLED_DrawPoint(x, y, mode);
        temp <<= 1;
        y++;
        if((y - y0) == Size) {
          y = y0;
          x++;
          break;
        }
      }
    }
    return;
  }
  for(int i = 0; i < sizeof(Index) / sizeof("中"); i++) {
    if(memcmp(chr, Index[i], sizeof("中") - 1) == 0) {
      OLED_ShowBMP(x, y, (uint8_t*)Font16[i], 16, 16, mode);
      break;
    }
  }
}

/*
功能描述：显示显示BMP图片
参数: x(0~127), y(0~63): 起点坐标; *BMP:图片数组指针,BMP_Heigth: 图片高度;BMP_Width: 图片宽度;
      mode:0,反白显示;1,正常显示
*/
void OLED_ShowBMP(int x, int y, uint8_t* BMP, int BMP_Heigth, int BMP_Width, int mode) {
  uint8_t tmp;
  for(int n = 0; n < BMP_Heigth / 8; n++) {
    if(mode == 1) {
      for(int i = 0; i < BMP_Width; i++) {
        tmp = BMP[n*BMP_Width+i];
        for(int t=0;t<8;t++,tmp<<=1)if(tmp&0x80)OLED_DrawPoint(x+i, y+8-t, mode);
      }
    } else {
      for(int i = 0; i < BMP_Width; i++) {
        tmp = !BMP[n*BMP_Width+i];
        for(int t=0;t<8;t++,tmp<<=1)if(tmp&0x80)OLED_DrawPoint(x, y, mode);
      }
    }
    y += 8;
  }
}

//画四分之一圆
void OLED_DrawQuarterRound(int xo, int yo, int r, int width, int quarter, uint32_t color) {
  int xs = Max(xo, 0);
  int xe = Min(xo + r + 1, 127);
  int ys = Max(yo, 0);
  int ye = Min(yo + r + 1, 63);
  
  for(int i = ys; i < ye; i++) {
    for(int t = xs - ys + i; t < xe; t++) {
      if(fabs(powf(xo - t, 2) + powf(yo - i, 2)) <= powf(r, 2)) {
        switch(quarter) {
        case 1:
          OLED_DrawPoint(t, 2 * ys - i, color);
          OLED_DrawPoint(xs + i - ys, ys + xs - t, color);
          break;
        case 2:
          OLED_DrawPoint(2 * xs - t, 2 * ys - i, color);
          OLED_DrawPoint(xs + ys - i, ys + xs - t, color);
          break;
        case 3:
          OLED_DrawPoint(2 * xs - t, i, color);
          OLED_DrawPoint(xs + ys - i, ys + t - xs, color);
          break;
        case 4:
          OLED_DrawPoint(t, i, color);
          OLED_DrawPoint(xs + i - ys, ys + t - xs, color);
          break;
        default:
          break;
        }
      }
    }
  }
}

//反色显示
void OLED_Negation(int ixs, int iys, int ixe, int iye, int mode) {
  int xs = Min(ixs, ixe), ys = Min(iys, iye),
      xe = Max(ixs, ixe), ye = Max(iys, iye); //重新整理起始点和结束点

  Layer = OLED_XOR_LAYER;
  OLED_Clear();
  
  if(mode) {      //圆角
    OLED_DrawQuarterRound(xs + 2, ys + 2, 2, 1, 2, 1);
    OLED_DrawQuarterRound(xe - 2, ys + 2, 2, 1, 1, 1);
    OLED_DrawQuarterRound(xs + 2, ye - 2, 2, 1, 3, 1);
    OLED_DrawQuarterRound(xe - 2, ye - 2, 2, 1, 4, 1);

    for(int i = ys + 2; i <= ye - 2; i++) {
      for(int t = xs; t <= xe; t++) {
        OLED_DrawPoint(t, i, 1);
      }
    }
    for(int i = ys; i <= ys + 2; i++) {
      for(int t = xs + 2; t <= xe - 2; t++) {
        OLED_DrawPoint(t, i, 1);
      }
    }
    for(int i = ye - 2; i <= ye; i++) {
      for(int t = xs + 2; t <= xe - 2; t++) {
        OLED_DrawPoint(t, i, 1);
      }
    }

  } else {
    for(int i = ys; i <= ye; i++) {
      for(int t = xs; t <= xe; t++) {
        OLED_DrawPoint(t, i, 1);
      }
    }
  }

  for(int i = 0; i < 1024; i++) {
    GRAM[OLED_DATA_LAYER][i] ^= GRAM[OLED_XOR_LAYER][i];
  }
  OLED_Clear();
  Layer = OLED_DATA_LAYER;
}

//发送显存
void OLED_GRAM_Transmit(void) {
  OLED_WR_Data(GRAM[0]);
}
