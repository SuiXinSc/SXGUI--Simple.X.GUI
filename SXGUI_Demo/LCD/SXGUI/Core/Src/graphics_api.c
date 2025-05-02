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
#include "lcd.h"

#ifndef Max
#define Max(a,b)      ((a) > (b) ? (a) : (b))
#endif

#ifndef Min
#define Min(a,b)      ((a) < (b) ? (a) : (b))
#endif

Transition_PID PID_Parameter;         //PID参数, 为全局变量
bool Autoln = true;

void (*BackCallBack)(void);     //绘制菜单背景的函数指针

//注册背景绘制函数回调
void Graphics_RegBack(void (*Callback)(void)) {
  BackCallBack = Callback;
}

//调用背景绘制
void Graphics_DrawBackground(void) {
  BackCallBack();
}

//基础函数, 不同屏幕需要更改
void Graphics_Init(void) {
  BackCallBack = Graphics_Clear; //赋给回调函数初始值以防止空指针死机，不要改！！！

  LCD_Init();
  LCD_Display_Dir(3);
  LCD_Clear(WHITE);
}

void Graphics_Clear(void) {
  LCD_Clear(WHITE);
}

void Graphics_Display(void) {
  LCD_Flash();
  HAL_Delay(2);
  //这一段延时主要是因为DMA传输，不能改动传输区域内存，裸机会性能浪费，但是用RTOS换成RTOS的延时就没有关系了
  LCD_Clear(WHITE);
}

//设置显示窗口，要求是该范围外的点不显示
void Graphics_SetWindows(int x, int y, int width, int height) {
  LCD_SetWindow(x, y, width, height);
}

//基础绘图函数, 不同屏幕需要更改
void Graphics_DrawPoint(int x, int y, uint32_t color) {
  LCD_DrawPoint(x, y, color);
}

void Graphics_ShowChar(int x, int y, char* Char, int size, uint32_t color) {
  LCD_ShowChar(x, y, Char, size, color);
}

void Graphics_ShowBMP(int x, int y, char* BMP, int BMP_Width, int BMP_Heigth, uint32_t color) {
  LCD_ShowImage(x, y, BMP_Width, BMP_Heigth, (uint8_t*)BMP);
}

//矩形选择的函数
void Graphics_Check(int ixs, int iys, int ixe, int iye, int mode) {
  Graphics_DrawRoundRect(ixs, iys, ixe, iye, 3, BLUE, HOLLOW);
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

    if(((String[i] < 127 && Start + size / 2 > SCREEN_WIDTH) ||
        (String[i] > 127 && Start + size > SCREEN_WIDTH)) && Autoln) {
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
      (PID_Parameter.Kp * Error + PID_Parameter.Ki * integral + PID_Parameter.Kd * derivative) * PID_Parameter.Factor;

    OUTPUT->length =
      (PID_Parameter.Kp * L_Error + PID_Parameter.Ki * L_integral + PID_Parameter.Kd * L_derivative) * PID_Parameter.Factor;

    LastError = Error;
    L_LastError = L_Error;

    pid_tick = Graphics_GetTick();
  }
}

//列表显示
void Graphics_ShowList(int x, int y, int length, int height, int size, void* LinkList,
                       int Start, uint32_t color, float x_zoom, float y_zoom) {
  SXGUI_MenuItem* Ptr = (SXGUI_MenuItem*)LinkList;
  SXGUI_APPItem* AppPtr = Ptr->App;
  Graphics_SetWindows(x, y, length + 1, height + 1);
  Autoln = false;

  int LinkListLong = 0;

  //菜单列表
  if(Ptr->Sub != NULL) {
    Ptr = Ptr->Sub;
    for(; Ptr != NULL; Ptr = Ptr->Next, LinkListLong++) {
      Graphics_ShowString(length * (1.0f - x_zoom), (float)((LinkListLong * size - Start)*y_zoom), Ptr->name, size, color);
    }
  }

  //APP列表
  for(; AppPtr != NULL; AppPtr = AppPtr->Next, LinkListLong++) {
    Graphics_ShowString(length * (1.0f - x_zoom), (float)((LinkListLong * size - Start)*y_zoom), AppPtr->name, size, color);
  }

  //进度条
  if(height / (LinkListLong * size) < 1) {
    Graphics_DrawLine(length, Graphics_Mapping(Start, 0, LinkListLong * size, 0, height),
                      length, height * height / (LinkListLong * size) + Graphics_Mapping(Start, 0, LinkListLong * size, 0, height),
                      1, SOLID_LINE);
    Graphics_DrawLine(length, 0, length, height, 1, DASHED_LINE);
  }

  Graphics_SetWindows(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  Autoln = true;

}

//列表显示+选择
int Graphics_ListChoose(int x, int y, int length, int height, int size, void* LinkList,
                        int MenuListLong, int AppListLong, int option, int Style, uint32_t color, int runtick) {
  if(LinkList == NULL || option >= MenuListLong + AppListLong) {
    return SXGUI_ERROR;
  }

  static Transition_DATA OUTPUT;
  Transition_DATA INPUT;
  INPUT.Start = option * size;

  int End_Edge = (MenuListLong + AppListLong) * size - height; //列表底(最大起始值)
  int ListStart = 0, CursorStart;   //列表起始值, 光标坐标

  SXGUI_MenuItem* Ptr = (SXGUI_MenuItem*)LinkList;
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


  static float zoom;
  zoom = (runtick < TRANSITION_TIMES ? (log(runtick) / log(TRANSITION_TIMES)) : 1.0f);

  if(Style == POINTER_STYLE) {
    Graphics_ShowList(x + size, y, length - size, height, size, LinkList, ListStart, color, zoom, zoom);
    Graphics_ShowString(x, y + CursorStart, "->", size, 1); //指针风格
  } else if(Style == FRAME_STYLE) {
    Graphics_ShowList(x, y, length, height, size, LinkList, ListStart, color, zoom, zoom);
    Graphics_Check(x, y + CursorStart, x + OUTPUT.length, y + size + CursorStart, 1); //框选风格
  }

  return SXGUI_OK;
}
