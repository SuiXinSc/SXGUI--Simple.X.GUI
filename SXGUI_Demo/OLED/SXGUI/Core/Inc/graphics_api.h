#ifndef __GRAPHICS_API_
#define __GRAPHICS_API_

#include "main.h"
#include "sxgui.h"

#define GRAPHICS_VERSION    "v1.2"

/*宏定义*/
#define SCREEN_WIDTH        128     //屏幕宽度
#define SCREEN_HEIGHT       64     //屏幕高度

#define POINTER_STYLE   0x10
#define FRAME_STYLE     0x00

#define DASHED_LINE     0xAF
#define SOLID_LINE      0xBF

#ifndef SOLID
#define SOLID       0xF0
#endif

#ifndef HOLLOW
#define HOLLOW      0x0F
#endif

/*定义结构体*/
struct Transition_PID;
struct Transition_DATA;

//PID参数结构体
typedef struct Transition_PID {
  double Kp;
  double Ki;
  double Kd;

  int MinTime;
  double Factor;
} Transition_PID;

//数据结构体
typedef struct Transition_DATA {
  int Start;
  int length;
} Transition_DATA;

/*定义全局变量*/
extern Transition_PID PID_Parameter;

/*函数声明*/
uint32_t Graphics_GetTick(void);
uint32_t Graphics_Mapping(uint32_t input, uint32_t as, uint32_t ae, uint32_t bs, uint32_t be);
void Graphics_PID(Transition_DATA INPUT, Transition_DATA* OUTPUT);
int GetStrLen(char* String);

void Graphics_Init(void);
void Graphics_Clear(void);
void Graphics_Display(void);
void Graphics_DrawPoint(int x, int y, uint32_t color);
void Graphics_SetWindows(int x, int y, int width, int height);
void Graphics_ShowChar(int x, int y, char* Char, int size, uint32_t color);
void Graphics_ShowBMP(int x, int y, char* BMP, int BMP_Width, int BMP_Heigth, uint32_t color);
void Graphics_Check(int ixs, int iys, int ixe, int iye, int mode);
void Graphics_Bokeh(void);

void Graphics_RegBack(void (*Callback)(void));
void Graphics_DrawBackground(void);
void Graphics_ShowString(int x, int y, char* String, int size, uint32_t color);
void Graphics_DrawLine(int xs, int ys, int xe, int ye, uint32_t color, int mode);
void Graphics_DrawQuarterRound(int xo, int yo, int r, int width, int quarter, uint32_t color, int mode);
void Graphics_DrawRound(int xo, int yo, int r, int width, uint32_t color, int mode);
void Graphics_DrawRect(int ixs, int iys, int ixe, int iye, int color, int mode);
void Graphics_DrawRoundRect(int ixs, int iys, int ixe, int iye, int r, int color, int mode);
void Graphics_ShowList(int x, int y, int length, int width, int size, void* LinkList, int Start, uint32_t color, float x_zoom, float y_zoom);
int Graphics_ListChoose(int x, int y, int length, int height, int size, void* LinkList,
                        int MenuListLong, int AppListLong, int option, int Style, uint32_t color, int runtick);

#endif
