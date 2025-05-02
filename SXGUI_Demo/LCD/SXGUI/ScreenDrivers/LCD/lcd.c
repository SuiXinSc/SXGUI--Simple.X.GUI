#include "lcd.h"
#include "font.h"
#include "string.h"
#include <math.h>
#include "dma.h"

uint8_t GRAM[128 * 160 * 2] = {0};

struct Windows {
  uint16_t x, y;
  uint16_t width, height;
} Windows = {0};

_lcd_dev lcddev;

void LCD_WR_REG(uint8_t reg) {
  LCD_REG = reg;
}

void LCD_WR_DATA(uint8_t dat) {
  LCD_DAT = dat;
}

void LCD_WR_DATA16(uint16_t dat) {
  LCD_DAT = dat >> 8;
  LCD_DAT = dat & 0xFF;
}

void LCD_WriteRAM_Prepare(void) {
  LCD_WR_REG(lcddev.wramcmd);
}

void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue) {
  LCD_WR_REG(LCD_Reg);
  LCD_WR_DATA16(LCD_RegValue);
}

//LCD开启显示
void LCD_DisplayOn(void) {
  LCD_WR_REG(0X29);
}
//LCD关闭显示
void LCD_DisplayOff(void) {
  LCD_WR_REG(0X28);
}

//设置光标位置
//Xpos:x坐标
//Ypos:y坐标
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos) {
  LCD_WR_REG(lcddev.setxcmd);
  LCD_WR_DATA(Xpos >> 8);
  LCD_WR_DATA(Xpos & 0XFF);
  LCD_WR_REG(lcddev.setycmd);
  LCD_WR_DATA(Ypos >> 8);
  LCD_WR_DATA(Ypos & 0XFF);
}

//dir:显示方向 	0-0旋转, 1-180旋转, 2-270旋转, 3-90旋转
void LCD_Display_Dir(uint8_t dir) {
  if(dir == 0 || dir == 1) {
    lcddev.dir = 0;
    lcddev.width = 128;
    lcddev.height = 160;

    lcddev.wramcmd = 0X2C;
    lcddev.setxcmd = 0X2A;
    lcddev.setycmd = 0X2B;

    if(dir == 0) {
      LCD_WR_REG(0x36);
      LCD_WR_DATA((0 << 3) | (1 << 7) | (1 << 6) | (0 << 5));
    } else {
      LCD_WR_REG(0x36);
      LCD_WR_DATA((0 << 3) | (0 << 7) | (0 << 6) | (0 << 5));
    }

  } else if(dir == 2 || dir == 3) {

    lcddev.dir = 1;
    lcddev.width = 160;
    lcddev.height = 128;

    lcddev.wramcmd = 0X2C;
    lcddev.setxcmd = 0X2A;
    lcddev.setycmd = 0X2B;

    if(dir == 2) {
      LCD_WR_REG(0x36);
      LCD_WR_DATA((0 << 3) | (0 << 7) | (1 << 6) | (1 << 5));

    } else {
      LCD_WR_REG(0x36);
      LCD_WR_DATA((0 << 3) | (1 << 7) | (0 << 6) | (1 << 5));
    }
  }

  Windows.x = 0, Windows.y = 0;
  Windows.width = lcddev.width, Windows.height = lcddev.height;

  LCD_WR_REG(lcddev.setxcmd);
  LCD_WR_DATA(0);
  LCD_WR_DATA(0);
  LCD_WR_DATA((lcddev.width - 1) >> 8);
  LCD_WR_DATA((lcddev.width - 1) & 0XFF);
  LCD_WR_REG(lcddev.setycmd);
  LCD_WR_DATA(0);
  LCD_WR_DATA(0);
  LCD_WR_DATA((lcddev.height - 1) >> 8);
  LCD_WR_DATA((lcddev.height - 1) & 0XFF);
}

//设置窗口，注意设置完之后调用显示的坐标是以该窗口起点为原点的
//sx,sy:窗口左上起点
//width,height:窗口宽高
void LCD_SetWindow(int sx, int sy, uint16_t width, uint16_t height) {
  Windows.x = MAX(sx, 0), Windows.y = MAX(sy, 0);
  Windows.x = MIN(Windows.x, lcddev.width - 1);
  Windows.y = MIN(Windows.y, lcddev.height - 1);
  Windows.width = (Windows.x + width < lcddev.width ? width : lcddev.width - Windows.x);
  Windows.height = (Windows.y + height < lcddev.height ? height : lcddev.height - Windows.y);
}

//LCD清屏
//color:清屏的颜色
void LCD_Clear(uint16_t color) {
  for(int index = 0; index < lcddev.width * lcddev.height; index++) {
    GRAM[index * 2] = color >> 8;
    GRAM[index * 2 + 1] = color & 0xFF;
  }

}

//DMA发送显存
void LCD_Flash(void) {
  LCD_WriteRAM_Prepare();     		//准备发送显存
  HAL_DMA_Start_IT(&hdma_memtomem_dma2_stream3, (uint32_t)GRAM, (uint32_t)&LCD_DAT, 128 * 160 * 2);
}

//LCD画点
//x,y:点的坐标
//color:点的颜色
void LCD_DrawPoint(int x, int y, uint16_t color) {
  if(x < 0 || x >= Windows.width || y < 0 || y >= Windows.height)return;
  x += Windows.x;
  y += Windows.y;
  GRAM[(y * lcddev.width + x) * 2] = color >> 8;
  GRAM[(y * lcddev.width + x) * 2 + 1] = color & 0xFF;
}

//LCD显示图片
void LCD_ShowImage(int x, int y, int width, int height, uint8_t* Image) {
  for(int i = 0; i < height; i++) {
    for(int t = 0; t < width; t++) {
      GRAM[(i * 160 + t) * 2] = Image[(i * width + t) * 2];
      GRAM[(i * 160 + t) * 2 + 1] = Image[(i * width + t) * 2 + 1];
    }
  }
}

//LCD显示一个字符(懒得写中文支持了)
void LCD_ShowChar(int x, int y, char* chr, uint8_t size, uint16_t color) {
  if((int)chr[0] < 128) {
    uint8_t temp, t1, t;
    int y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
    unsigned char num = chr[0] - ' ';
    for(t = 0; t < csize; t++) {
      if(size == 12)temp = asc2_1206[num][t];
      else if(size == 16)temp = asc2_1608[num][t];
      else if(size == 24)temp = asc2_2412[num][t];
      else return;
      for(t1 = 0; t1 < 8; t1++) {
        if(temp & 0x80)LCD_DrawPoint(x, y, color);
        temp <<= 1;
        y++;
        if((y - y0) == size) {
          y = y0;
          x++;
          break;
        }
      }
    }
  }
}

int LCD_Init(void) {
  LCD_RST_SET;
  HAL_Delay(1);
  LCD_RST_CLR;
  HAL_Delay(10);
  LCD_RST_SET;
  HAL_Delay(120);


  //************* Start Initial Sequence **********//
  HAL_Delay(120);                //ms

  LCD_WR_REG(0x11);     //Sleep out

  HAL_Delay(120);                //delay_ms 120ms

  LCD_WR_REG(0xB1);     //In normal mode
  LCD_WR_DATA(0x00);   //frame rate=85.3Hz
  LCD_WR_DATA(0x2C);
  LCD_WR_DATA(0x2B);

  LCD_WR_REG(0xB2);     //In Idle mode
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x01);

  LCD_WR_REG(0xB3);     //In partial mode
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x01);

  LCD_WR_REG(0xB4);     //DOT inversion Display Inversion Control
  LCD_WR_DATA(0x03);

  LCD_WR_REG(0xB9);     //In normal mode
  LCD_WR_DATA(0xFF);
  LCD_WR_DATA(0x83);
  LCD_WR_DATA(0x47);

  LCD_WR_REG(0xC0);     //VRH: Set the GVDD
  LCD_WR_DATA(0xA2);
  LCD_WR_DATA(0x02);
  LCD_WR_DATA(0x84);

  LCD_WR_REG(0xC1);     //set VGH/ VGL
  LCD_WR_DATA(0x02);   //??02 VGH=16.6 VGL=-7.5  00 VGH=11.6 VGL=-7.5  06 VGH=16.6  VGL=-10

  LCD_WR_REG(0xC2);     //APA: adjust the operational amplifier DCA: adjust the booster Voltage
  LCD_WR_DATA(0x0A);
  LCD_WR_DATA(0x00);

  LCD_WR_REG(0xC3);     //In Idle mode (8-colors)
  LCD_WR_DATA(0x8A);
  LCD_WR_DATA(0x2A);

  LCD_WR_REG(0xC4);     //In partial mode + Full color
  LCD_WR_DATA(0x8A);
  LCD_WR_DATA(0xEE);

  LCD_WR_REG(0xC5);     //VCOM
  LCD_WR_DATA(0x09);

  LCD_WR_REG(0x20);     //Display inversion

  LCD_WR_REG(0xC7);
  LCD_WR_DATA(0x10);

  LCD_WR_REG(0x36);     //MX, MY, RGB mode
  LCD_WR_DATA(0xC0); //08

  LCD_WR_REG(0xE0);
  LCD_WR_DATA(0x0C);
  LCD_WR_DATA(0x1C);
  LCD_WR_DATA(0x1B);
  LCD_WR_DATA(0x1A);
  LCD_WR_DATA(0x2F);
  LCD_WR_DATA(0x28);
  LCD_WR_DATA(0x20);
  LCD_WR_DATA(0x24);
  LCD_WR_DATA(0x23);
  LCD_WR_DATA(0x22);
  LCD_WR_DATA(0x2A);
  LCD_WR_DATA(0x36);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x05);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x10);

  LCD_WR_REG(0xE1);
  LCD_WR_DATA(0x0C);
  LCD_WR_DATA(0x1A);
  LCD_WR_DATA(0x1A);
  LCD_WR_DATA(0x1A);
  LCD_WR_DATA(0x2E);
  LCD_WR_DATA(0x27);
  LCD_WR_DATA(0x21);
  LCD_WR_DATA(0x24);
  LCD_WR_DATA(0x24);
  LCD_WR_DATA(0x22);
  LCD_WR_DATA(0x2A);
  LCD_WR_DATA(0x35);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x05);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x10);

  LCD_WR_REG(0x35);     //65k mode
  LCD_WR_DATA(0x00);

  LCD_WR_REG(0x3A);     //65k mode
  LCD_WR_DATA(0x05);

  LCD_WR_REG(0x29);     //Display on
  return 0;
}
