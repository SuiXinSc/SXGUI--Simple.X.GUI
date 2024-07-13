#ifndef __SXGUI_
#define __SXGUI_

#include "main.h"
#include "graphics_api.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*宏定义*/
#define SXGUI_OK          0x00
#define SXGUI_ERROR       0x01
#define SXGUI_EXIT        0x02
#define SXGUI_RUNNING     0x03

/*定义结构体*/
struct SXGUI_MenuItem;
struct SXGUI_APPItem;
struct SXGUI_KeyItem;

typedef struct SXGUI_MenuItem{
  char *name;
  struct SXGUI_MenuItem *Pre;
  struct SXGUI_MenuItem *Next;
  struct SXGUI_MenuItem *Parent;
  struct SXGUI_MenuItem *Sub;
  struct SXGUI_APPItem *App;
  const uint8_t *Icon;    //图标的指针
} SXGUI_MenuItem;

typedef struct SXGUI_APPItem{
  char *name;
  void (*Function)(struct SXGUI_KeyItem *Key);
  struct SXGUI_APPItem *Pre;
  struct SXGUI_APPItem *Next;
  struct SXGUI_MenuItem *Parent;
  const uint8_t *Icon;    //图标的指针
} SXGUI_APPItem;

typedef struct SXGUI_KeyItem{
  bool Back;
  bool Pre;
  bool Next;
  bool OK;
  int *MoreKey;     //更多按键
} SXGUI_KeyItem;

/*函数声明*/
void SXGUI_Interface(int Fontsize,SXGUI_MenuItem *Menu,int option,int MenuNum,
                    int AppNum,uint32_t BackColor,uint32_t FontColor,int style);

int SXGUI_CreateRoot(char *Name);
SXGUI_MenuItem *SXGUI_CreateMenu(char *Name);
SXGUI_APPItem *SXGUI_CreateApp(char *Name,void (*AppFunction)(struct SXGUI_KeyItem *Key));
void SXGUI_Free(void);
void SXGUI_DeleteMenu(SXGUI_MenuItem *Menu);
void SXGUI_DeleteApp(SXGUI_APPItem *APP);

int SXGUI_RootAddSubMenu(SXGUI_MenuItem *SubMenu);
int SXGUI_RootAddApp(SXGUI_APPItem *APP);
int SXGUI_AddSubMenu(SXGUI_MenuItem *ParentMenu, SXGUI_MenuItem *SubMenu);
int SXGUI_AddApp(SXGUI_MenuItem *ParentMenu, SXGUI_APPItem *APP);

void SXGUI_Init(char *HubName);
void SXGUI_Main(int Fontsize,uint32_t BackColor,uint32_t FontColor,int Sytle,SXGUI_KeyItem *Key);



#endif