/********************************************************
* @file     :graphics_api.c
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

#include "graphics_api.h"
#include "oled.h"
#include <math.h>

#ifndef Max
#define Max(a,b)      ((a) > (b) ? (a) : (b))
#endif

#ifndef Min
#define Min(a,b)      ((a) < (b) ? (a) : (b))
#endif

#define DATA_LAYER        OLED_DATA_LAYER       //输出数据层
#define IMAGE_LAYER       OLED_IMAGE_LAYER      //图像层
#define GRATING_LAYER     OLED_GRATING_LAYER    //二元光栅层
#define ALL_LAYER         OLED_ALL_LAYER        //所有层

uint8_t Graphics_GRAM[SCREEN_GRAM_SIZE];      //定义显存
Transition_PID PID_Parameter;         //PID参数, 为全局变量

uint8_t Operate_Layer = DATA_LAYER;   //要操作的层

//基础函数, 不同屏幕需要更改
void Graphics_Init(void) {
  OLED_Init();
  OLED_Clear(ALL_LAYER);
}

void Graphics_Clear(void) {
  OLED_Clear(Operate_Layer);
}

void Graphics_Display(void) {
  OLED_GRAM_Transmit();
  OLED_Clear(ALL_LAYER);
}

//获取显存
void Graphics_GetGRAM(void) {
  OLED_GetGRAM(Graphics_GRAM, Operate_Layer);
}

//更新显存(与原数据叠加)
void Graphics_UpDateToGRAM(void) {
  OLED_UpDateToGRAM(Graphics_GRAM, Operate_Layer);
}

//二元光栅运算
void Graphics_GratingCount(void) {
  OLED_Grating_Count();
}

//基础绘图函数, 不同屏幕需要更改
void Graphics_DrawPoint(int x, int y, uint32_t color) {
  OLED_DrawPoint(x, y, color, Operate_Layer);
}

void Graphics_ShowChar(int x, int y, char* Char, int size, uint32_t color) {
  OLED_ShowChar(x, y, Char, size, color, Operate_Layer);
}

void Graphics_ShowBMP(int x, int y, char* BMP, int BMP_Width, int BMP_Heigth, uint32_t color) {
  OLED_ShowBMP(x, y, (uint8_t*)BMP, BMP_Heigth, BMP_Width, color, Operate_Layer);
}

//矩形选择的函数, 我这里用OLED反色实现
void Graphics_Check(int ixs, int iys, int ixe, int iye, int mode) {
  OLED_Negation(ixs, iys, ixe, iye, mode);
}

//背景虚化
void Graphics_Bokeh(void) {
  for(int i = 0; i < SCREEN_GRAM_SIZE; i += 2) {
    Graphics_GRAM[i] = 0x55;
    Graphics_GRAM[i + 1] = 0xAA;
  }

  Operate_Layer = GRATING_LAYER;
  Graphics_UpDateToGRAM();
  Graphics_GratingCount();
  Graphics_Clear();
  Operate_Layer = DATA_LAYER;
}

//获取时间戳，不同平台需要更改
uint32_t Graphics_GetTick(void) {
  return HAL_GetTick();
}


/*在上述函数按要求重写的情况下，以下函数无需更改*/

//线性映射算法
uint32_t Graphics_Mapping(uint32_t input, uint32_t as, uint32_t ae, uint32_t bs, uint32_t be) {
  return input * (be - bs) / (ae - as) + bs;
}

//显示字符串
void Graphics_ShowString(int x, int y, char* String, int size, uint32_t color) {
  int Start = x;

  for(int i = 0; String[i] != '\0';) {
    Graphics_ShowChar(Start, y, String + i, size, color);

    if(String[i] > 127) {
      Start += size;
      i += sizeof("中") - 1; //中文的偏移值, 删去 \0
    } else {
      Start += size / 2;
      i++;
    }

    if((String[i] < 127 && Start + size / 2 > SCREEN_WIDTH) ||
        (String[i] > 127 && Start + size > SCREEN_WIDTH)) {
      Start = x;
      y += size;
    }     //判断下一个字符是否超出屏幕边缘, 如果是, 则换行
  }
}

//获取相对长度, 不用 strlen() 是因为不同编码模式下汉字所占字节数不同
int GetStrLen(char* String) {
  int len = 0;
  for(int i = 0; String[i] != '\0';) {
    if(String[i] > 127) {
      i += sizeof("中") - 1; //中文的偏移值, 删去 \0
      len += 2;
    } else {
      i++;
      len++;
    }
  }
  return len;
}

//画线; mode: 实线/虚线
void Graphics_DrawLine(int xs, int ys, int xe, int ye, uint32_t color, int mode) {
  int Step = 1;
  if(mode == DASHED_LINE) {
    Step = 3;
  }

  if(xs == xe) {
    for(int i = ys; i <= ye; i += Step) {
      Graphics_DrawPoint(xs, i, color);
    }
    return;
  } else if(ys == ye) {
    for(int i = xs; i <= xe; i += Step) {
      Graphics_DrawPoint(i, ys, color);
    }
    return;
  }

  double k = (double)(ye - ys) / (xe - xs);

  for(double i = 0; i <= xe - xs; i += Step) {
    Graphics_DrawPoint(i + xs, k * i + ys, color);
  }
  for(double i = 0; i <= ye - ys; i += Step) {
    Graphics_DrawPoint(i / k + xs, i + ys, color);
  }
}

//画四分之一圆; mode:填充/不填充
void Graphics_DrawQuarterRound(int xo, int yo, int r, int width, int quarter, uint32_t color, int mode) {
  int xs = Max(xo, 0);
  int xe = Min(xo + r + 1, SCREEN_WIDTH);
  int ys = Max(yo, 0);
  int ye = Min(yo + r + 1, SCREEN_HEIGHT);

  if(mode == SOLID) {
    for(int i = ys; i < ye; i++) {
      for(int t = xs - ys + i; t < xe; t++) {
        if(fabs(powf(xo - t, 2) + powf(yo - i, 2)) <= powf(r, 2)) {
          switch(quarter) {
          case 1:
            Graphics_DrawPoint(t, 2 * ys - i, color);
            Graphics_DrawPoint(xs + i - ys, ys + xs - t, color);
            break;
          case 2:
            Graphics_DrawPoint(2 * xs - t, 2 * ys - i, color);
            Graphics_DrawPoint(xs + ys - i, ys + xs - t, color);
            break;
          case 3:
            Graphics_DrawPoint(2 * xs - t, i, color);
            Graphics_DrawPoint(xs + ys - i, ys + t - xs, color);
            break;
          case 4:
            Graphics_DrawPoint(t, i, color);
            Graphics_DrawPoint(xs + i - ys, ys + t - xs, color);
            break;
          default:
            break;
          }
        }
      }
    }
  } else {
    for(int i = ys; i < ye; i++) {
      for(int t = xs - ys + i; t < xe; t++) {
        if(fabs(powf(xo - t, 2) + powf(yo - i, 2)) <= powf(r, 2) &&
            fabs(powf(xo - t, 2) + powf(yo - i, 2)) >= powf(r - width, 2)) {
          switch(quarter) {
          case 1:
            Graphics_DrawPoint(t, 2 * ys - i, color);
            Graphics_DrawPoint(xs + i - ys, ys + xs - t, color);
            break;
          case 2:
            Graphics_DrawPoint(2 * xs - t, 2 * ys - i, color);
            Graphics_DrawPoint(xs + ys - i, ys + xs - t, color);
            break;
          case 3:
            Graphics_DrawPoint(2 * xs - t, i, color);
            Graphics_DrawPoint(xs + ys - i, ys + t - xs, color);
            break;
          case 4:
            Graphics_DrawPoint(t, i, color);
            Graphics_DrawPoint(xs + i - ys, ys + t - xs, color);
            break;
          default:
            break;
          }
        }
      }
    }
  }
}

//画整圆; mode:填充/不填充
void Graphics_DrawRound(int xo, int yo, int r, int width, uint32_t color, int mode) {
  int xs = Max(xo, 0);
  int xe = Min(xo + r + 1, SCREEN_WIDTH);
  int ys = Max(yo, 0);
  int ye = Min(yo + r + 1, SCREEN_HEIGHT);

  if(mode == SOLID) {
    for(int i = ys; i < ye; i++) {
      for(int t = xs - ys + i; t < xe; t++) {
        if(fabs(powf(xo - t, 2) + powf(yo - i, 2)) <= powf(r, 2)) {
          Graphics_DrawPoint(t, i, color);
          Graphics_DrawPoint(xs + i - ys, ys + t - xs, color);

          Graphics_DrawPoint(t, 2 * ys - i, color);
          Graphics_DrawPoint(xs + i - ys, ys + xs - t, color);

          Graphics_DrawPoint(2 * xs - t, 2 * ys - i, color);
          Graphics_DrawPoint(xs + ys - i, ys + xs - t, color);

          Graphics_DrawPoint(2 * xs - t, i, color);
          Graphics_DrawPoint(xs + ys - i, ys + t - xs, color);
        }
      }
    }
  } else {
    for(int i = ys; i < ye; i++) {
      for(int t = xs - ys + i; t < xe; t++) {
        if(fabs(powf(xo - t, 2) + powf(yo - i, 2)) <= powf(r, 2) &&
            fabs(powf(xo - t, 2) + powf(yo - i, 2)) >= powf(r - width, 2)) {
          Graphics_DrawPoint(t, i, color);
          Graphics_DrawPoint(xs + i - ys, ys + t - xs, color);

          Graphics_DrawPoint(t, 2 * ys - i, color);
          Graphics_DrawPoint(xs + i - ys, ys + xs - t, color);

          Graphics_DrawPoint(2 * xs - t, 2 * ys - i, color);
          Graphics_DrawPoint(xs + ys - i, ys + xs - t, color);

          Graphics_DrawPoint(2 * xs - t, i, color);
          Graphics_DrawPoint(xs + ys - i, ys + t - xs, color);
        }
      }
    }
  }
}

//画矩形; mode:填充/不填充
void Graphics_DrawRect(int ixs, int iys, int ixe, int iye, int color, int mode) {
  int xs = Min(ixs, ixe), ys = Min(iys, iye),
      xe = Max(ixs, ixe), ye = Max(iys, iye); //重新整理起始点和结束点
  if(mode == HOLLOW) {
    Graphics_DrawLine(xs, ys, xe, ys, color, SOLID_LINE);
    Graphics_DrawLine(xs, ye, xe, ye, color, SOLID_LINE);
    Graphics_DrawLine(xs, ys, xs, ye, color, SOLID_LINE);
    Graphics_DrawLine(xe, ys, xe, ye, color, SOLID_LINE);
  } else {
    for(int i = ys; i <= ye; i++) {
      for(int t = xs; t <= xe; t++) {
        Graphics_DrawPoint(t, i, color);
      }
    }
  }
}

//画圆角矩形; mode:填充/不填充
void Graphics_DrawRoundRect(int ixs, int iys, int ixe, int iye, int r, int color, int mode) {
  int xs = Min(ixs, ixe), ys = Min(iys, iye),
      xe = Max(ixs, ixe), ye = Max(iys, iye); //重新整理起始点和结束点

  if(xe - xs < 2 * r) {
    xe = xs + 2 * r;
  }
  if(ye - ys < 2 * r) {
    ye = ys + 2 * r;
  }

  Graphics_DrawQuarterRound(xs + r, ys + r, r, 1, 2, color, mode);
  Graphics_DrawQuarterRound(xe - r, ys + r, r, 1, 1, color, mode);
  Graphics_DrawQuarterRound(xs + r, ye - r, r, 1, 3, color, mode);
  Graphics_DrawQuarterRound(xe - r, ye - r, r, 1, 4, color, mode);

  if(mode == HOLLOW) {
    Graphics_DrawLine(xs + r, ys, xe - r, ys, color, SOLID_LINE);
    Graphics_DrawLine(xs + r, ye, xe - r, ye, color, SOLID_LINE);
    Graphics_DrawLine(xs, ys + r, xs, ye - r, color, SOLID_LINE);
    Graphics_DrawLine(xe, ys + r, xe, ye - r, color, SOLID_LINE);
  } else {
    Graphics_DrawRect(xs, ys + r, xe, ye - r, color, SOLID);
    Graphics_DrawRect(xs + r, ys, xe - r, ys + r, color, SOLID);
    Graphics_DrawRect(xs + r, ye - r, xe - r, ye, color, SOLID);
  }
}

//PID算法
void Graphics_PID(Transition_DATA INPUT, Transition_DATA* OUTPUT) {
  static double integral = 0.0;   // 积分项，历史误差
  static double LastError = 0.0;  //上次误差

  static double L_integral = 0.0;   // 积分项，历史误差
  static double L_LastError = 0.0;  //上次误差

  static int pid_tick = 0;

  double derivative = 0.0; // 微分项，变化趋势
  double L_derivative = 0.0; // 微分项，变化趋势
  double Error = 0.0;      //当前误差
  double L_Error = 0.0;      //当前误差

  if((Graphics_GetTick() - pid_tick) > PID_Parameter.MinTime) {
    /*PID算法*/
    Error = INPUT.Start - OUTPUT->Start;
    integral += Error;
    derivative = Error - LastError;

    L_Error = INPUT.length - OUTPUT->length;
    L_integral += L_Error;
    L_derivative = L_Error - L_LastError;

    OUTPUT->Start =
      (double)(PID_Parameter.Kp * Error + PID_Parameter.Ki * integral + PID_Parameter.Kd * derivative) * PID_Parameter.Factor;

    OUTPUT->length =
      (double)(PID_Parameter.Kp * L_Error + PID_Parameter.Ki * L_integral + PID_Parameter.Kd * L_derivative) * PID_Parameter.Factor;

    LastError = Error;
    L_LastError = L_Error;

    pid_tick = Graphics_GetTick();
  }
}

//列表显示
void Graphics_ShowList(int x, int y, int length, int height, int size, void* LinkList, int Start, uint32_t color) {
  SXGUI_MenuItem* Ptr = LinkList;
  SXGUI_APPItem* AppPtr = Ptr->App;

  Operate_Layer = IMAGE_LAYER;

  int LinkListLong = 0;

  //菜单列表
  if(Ptr->Sub != NULL) {
    Ptr = Ptr->Sub;
    for(; Ptr != NULL; Ptr = Ptr->Next, LinkListLong++) {
      Graphics_ShowString(x, y + LinkListLong * size - Start, Ptr->name, size, color);
    }
  }

  //APP列表
  for(; AppPtr != NULL; AppPtr = AppPtr->Next, LinkListLong++) {
    Graphics_ShowString(x, y + LinkListLong * size - Start, AppPtr->name, size, color);
  }

  //进度条
  if(height / (LinkListLong * size) < 1) {
    Graphics_DrawLine(x + length, y + Graphics_Mapping(Start, 0, LinkListLong * size, 0, height),
                      x + length, y + height * height / (LinkListLong * size) + Graphics_Mapping(Start, 0, LinkListLong * size, 0, height),
                      1, SOLID_LINE);
    Graphics_DrawLine(x + length, y, x + length, y + height, 1, DASHED_LINE);
  }

  //光栅算法约束显示区域
  Operate_Layer = GRATING_LAYER;
  Graphics_DrawRect(x, y, x + length, y + height, color, SOLID);

  Graphics_GratingCount();
  Graphics_Clear();
  Operate_Layer = IMAGE_LAYER;
  Graphics_Clear();

  Operate_Layer = DATA_LAYER;
}

//列表显示+选择
int Graphics_ListChoose(int x, int y, int length, int height, int size, void* LinkList,
                        int MenuListLong, int AppListLong, int option, int Style, uint32_t color) {
  if(LinkList == NULL || option >= MenuListLong + AppListLong) {
    return SXGUI_ERROR;
  }

  static Transition_DATA OUTPUT;
  Transition_DATA INPUT;
  INPUT.Start = option * size;

  int End_Edge = (MenuListLong + AppListLong) * size - height; //列表底(最大起始值)
  int ListStart = 0, CursorStart;   //列表起始值, 光标坐标

  SXGUI_MenuItem* Ptr = LinkList;
  SXGUI_APPItem* AppPtr = Ptr->App;

  if(Ptr->Sub != NULL) {
    Ptr = Ptr->Sub;
  }

  //计算框选长度
  if(option < MenuListLong) {
    for(int i = 0; i < option; Ptr = Ptr->Next, i++);
    INPUT.length = GetStrLen(Ptr->name) * size / 2;
  } else if(option < AppListLong + MenuListLong) {
    for(int i = MenuListLong; i < option; AppPtr = AppPtr->Next, i++);
    INPUT.length = GetStrLen(AppPtr->name) * size / 2;
  }

  //动画过渡
  Graphics_PID(INPUT, &OUTPUT);

  //计算列表起始值和光标坐标
  if((MenuListLong + AppListLong)*size > height) {
    ListStart = OUTPUT.Start + size / 2 - height / 2;
    if(ListStart < 0) {
      ListStart = 0;
    } else if(ListStart > End_Edge) {
      ListStart = End_Edge;
    }
    CursorStart = OUTPUT.Start - ListStart;
  } else {
    CursorStart = OUTPUT.Start;
  }

  if(Style == POINTER_STYLE) {
    Graphics_ShowList(x + size, y, length - Style, height, size, LinkList, ListStart, color);
    Graphics_ShowString(x, y + CursorStart, "->", size, 1); //指针风格
  } else if(Style == FRAME_STYLE) {
    Graphics_ShowList(x, y, length - Style, height, size, LinkList, ListStart, color);
    Graphics_Check(x, y + CursorStart, x + OUTPUT.length, y + size + CursorStart, 1); //框选风格
  }

  return SXGUI_OK;
}
