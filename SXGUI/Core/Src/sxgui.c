/********************************************************
* @file     :sxgui.c
* @version  :v1.2
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

#include "sxgui.h"

#define TRANSITION_TIMES      120     //虚化过渡时间, ms

//提供的一个标准界面绘制函数
void SXGUI_Interface(INTERFACE_PARAMETERS) {
  int List_x = Fontsize / 2,
      List_y = Fontsize + 1;

  Graphics_ListChoose(List_x, List_y, 128 - List_x - 1, 64 - Fontsize - 1,
                      Fontsize, Menu, MenuNum, AppNum, option, style, FontColor);

  Graphics_ShowString(0, 0, Menu->name, Fontsize, FontColor);
}

//根目录
SXGUI_MenuItem* ROOT;

//创建根
int SXGUI_CreateRoot(char* Name, void (*Interface)(INTERFACE_PARAMETERS)){

  if(ROOT != NULL) {
    return SXGUI_ERROR;     //如果已创建
  }
  ROOT = malloc(sizeof(SXGUI_MenuItem));
  ROOT->name = Name;
  ROOT->Interface = Interface;
  return SXGUI_OK;
}

//创建菜单项
SXGUI_MenuItem* SXGUI_CreateMenu(char* Name, void (*Interface)(INTERFACE_PARAMETERS)){
  SXGUI_MenuItem* Menu = malloc(sizeof(SXGUI_MenuItem));
  Menu->name = Name;
  Menu->Interface = Interface;
  return Menu;
}

//创建APP项
SXGUI_APPItem* SXGUI_CreateApp(char* Name, void (*AppFunction)(APP_PARAMETERS)) {
  SXGUI_APPItem* APP = malloc(sizeof(SXGUI_APPItem));
  APP->name = Name;
  APP->Function = AppFunction;
  return APP;
}

//释放GUI
void SXGUI_Free(void) {
  free(ROOT);
}

//删除菜单项
void SXGUI_DeleteMenu(SXGUI_MenuItem* Menu) {
  if(Menu->Pre == NULL) {
    Menu->Parent->Sub = Menu->Next;
    Menu->Next->Pre = NULL;
  } else {
    Menu->Pre->Next = Menu->Next;
    if(Menu->Next != NULL) {
      Menu->Next->Pre = Menu->Pre;
    }
  }
  free(Menu);
}

//删除APP项
void SXGUI_DeleteApp(SXGUI_APPItem* APP) {
  if(APP->Pre == NULL) {
    APP->Parent->App = APP->Next;
    APP->Next->Pre = NULL;
  } else {
    APP->Pre->Next = APP->Next;
    if(APP->Next != NULL) {
      APP->Next->Pre = APP->Pre;
    }
  }
  free(APP);
}

//在根目录下添加菜单项
int SXGUI_RootAddSubMenu(SXGUI_MenuItem* SubMenu) {
  if(SubMenu->Parent != NULL) { //如果已被添加
    return SXGUI_ERROR;
  }

  SubMenu->Parent = ROOT;
  SXGUI_MenuItem* Ptr = ROOT->Sub;

  if(Ptr != NULL) {
    for(; Ptr->Next != NULL; Ptr = Ptr->Next);
    Ptr->Next = SubMenu;
  } else {
    ROOT->Sub = SubMenu;
  }

  SubMenu->Pre = Ptr;

  return SXGUI_OK;
}

//根目录下添加APP项
int SXGUI_RootAddApp(SXGUI_APPItem* APP) {
  if(APP->Parent != NULL) {   //如果已被添加
    return SXGUI_ERROR;
  }
  APP->Parent = ROOT;

  SXGUI_APPItem* Ptr = ROOT->App;

  if(Ptr != NULL) {
    for(; Ptr->Next != NULL; Ptr = Ptr->Next);
    Ptr->Next = APP;
  } else {
    ROOT->App = APP;
  }

  APP->Pre = Ptr;

  return SXGUI_OK;
}

//菜单下添加子菜单
int SXGUI_AddSubMenu(SXGUI_MenuItem* ParentMenu, SXGUI_MenuItem* SubMenu) {
  if(SubMenu->Parent != NULL) {   //如果已被添加
    return SXGUI_ERROR;
  }
  SubMenu->Parent = ParentMenu;

  SXGUI_MenuItem* Ptr = ParentMenu->Sub;

  if(Ptr != NULL) {
    for(; Ptr->Next != NULL; Ptr = Ptr->Next);
    Ptr->Next = SubMenu;
  } else {
    ParentMenu->Sub = SubMenu;
  }

  SubMenu->Pre = Ptr;

  return SXGUI_OK;
}

//菜单下添加APP
int SXGUI_AddApp(SXGUI_MenuItem* ParentMenu, SXGUI_APPItem* APP) {
  if(APP->Parent != NULL) {   //如果已被添加
    return SXGUI_ERROR;
  }
  APP->Parent = ParentMenu;

  SXGUI_APPItem* Ptr = ParentMenu->App;

  if(Ptr != NULL) {
    for(; Ptr->Next != NULL; Ptr = Ptr->Next);
    Ptr->Next = APP;
  } else {
    ParentMenu->App = APP;
  }

  APP->Pre = Ptr;

  return SXGUI_OK;
}

//控件

//消息框
void Message_Box(char* Text,int Fontsize,uint32_t BackColor, uint32_t FontColor){
  int StrLen = GetStrLen(Text);
  
  if(StrLen > (SCREEN_WIDTH*3/2/Fontsize)){
    StrLen = (SCREEN_WIDTH*3/2/Fontsize);
  }
  
  Graphics_DrawRoundRect((SCREEN_WIDTH-(StrLen*Fontsize/2))/2-SCREEN_HEIGHT/32,0,
    (SCREEN_WIDTH+(StrLen*Fontsize/2))/2+SCREEN_HEIGHT/32,
    Fontsize+SCREEN_HEIGHT/32,SCREEN_HEIGHT/16,BackColor,SOLID);
  
  int Start = (SCREEN_WIDTH-(StrLen*Fontsize/2))/2;

  for(int n=0,i=0; i<StrLen;) {
    Graphics_ShowChar(Start, SCREEN_HEIGHT/64, Text + n, Fontsize, FontColor);

    if(Text[n] > 127) {
      Start += Fontsize;
      n += sizeof("中") - 1; //中文的偏移值, 删去 \0
      i+=2;
    } else {
      Start += Fontsize / 2;
      n++;
      i++;
    }
  }
}

//进度条
void Progress_Bar(int x,int y,int len,int height,uint32_t EdgeColor,uint32_t inColor,double Percent){
  Graphics_DrawRoundRect(x,y,x+len,y+height,height/2,EdgeColor,HOLLOW);
  Graphics_DrawRoundRect(x,y,x+(len*Percent),y+height,height/2,inColor,SOLID);
}

//指针变量
SXGUI_MenuItem* NowMenu;
SXGUI_MenuItem* MenuSelect;
SXGUI_APPItem* AppSelect;

//GUI初始化
void SXGUI_Init(char* HubName, void (*Interface)(INTERFACE_PARAMETERS)){
  SXGUI_CreateRoot(HubName,Interface);    //创建根目录
  NowMenu = ROOT;     //初始化指针变量
}

void SXGUI_Main(int Fontsize, uint32_t BackColor, uint32_t FontColor, int style, SXGUI_KeyItem* Key) {
  static int option = 0;
  static int tick = 0;
  static bool Success_flag = true;
  static bool RunApp = false;
  static bool Exit = false;
  static bool init_flag = true;

  if(init_flag) {
    if(NowMenu->Sub != NULL) {
      MenuSelect = NowMenu->Sub;
      AppSelect = NULL;
    } else {
      MenuSelect = NULL;
      AppSelect = NowMenu->App;
    }
    init_flag = false;
  }

  int MenuLinkListNum;
  int AppLinkListNum;

  SXGUI_MenuItem* Ptr = NowMenu->Sub;
  SXGUI_APPItem* AppPtr = NowMenu->App;

  //计算子菜单项数量和APP数量
  for(MenuLinkListNum = 0; Ptr != NULL; Ptr = Ptr->Next, MenuLinkListNum++);
  for(AppLinkListNum = 0; AppPtr != NULL; AppPtr = AppPtr->Next, AppLinkListNum++);

  /*按键处理*/

  //确定
  if(Key->OK && Success_flag) {
    if(MenuSelect != NULL) {
      NowMenu = MenuSelect;
      MenuSelect = MenuSelect->Sub;
      if(MenuSelect != NULL) {
        tick = Graphics_GetTick();
        Success_flag = false;
        AppSelect = NULL;
      } else {
        AppSelect = NowMenu->App;
        tick = Graphics_GetTick();
        Success_flag = false;
      }
      option = 0;
    } else if(AppSelect != NULL && RunApp != true) {
      tick = Graphics_GetTick();
      Success_flag = false;
      RunApp = true;
      Exit = false;
    }
  }

  //回退
  if(Key->Back && Success_flag) {
    if(RunApp) {
      Exit = true;
    } else if(NowMenu->Parent != NULL) {
      tick = Graphics_GetTick();
      Success_flag = false;
      NowMenu = NowMenu->Parent;
      MenuSelect = NowMenu->Sub;
      AppSelect = NULL;
      option = 0;
    }
  }

  //下一个
  if(Key->Next && RunApp == false && Success_flag) {
    if(MenuSelect != NULL) {
      if(MenuSelect->Next != NULL) {
        AppSelect = NULL;
        MenuSelect = MenuSelect->Next;
        option++;
      } else if(NowMenu->App != NULL) {
        AppSelect = NowMenu->App;
        MenuSelect = NULL;
        option++;
      } else {
        AppSelect = NULL;
        MenuSelect = NowMenu->Sub;
        option = 0;
      }
    } else if(AppSelect != NULL) {
      if(AppSelect->Next != NULL) {
        AppSelect = AppSelect->Next;
        MenuSelect = NULL;
        option++;
      } else if(NowMenu->Sub != NULL) {
        AppSelect = NULL;
        MenuSelect = NowMenu->Sub;
        option = 0;
      } else {
        AppSelect = NowMenu->App;
        MenuSelect = NULL;
        option = 0;
      }
    }
  }

  //上一个
  if(Key->Pre && RunApp == false && Success_flag) {
    if(MenuSelect != NULL) {
      if(MenuSelect->Pre != NULL) {
        AppSelect = NULL;
        MenuSelect = MenuSelect->Pre;
        option--;
      } else if(NowMenu->App != NULL) {
        AppPtr = NowMenu->App;
        for(; AppPtr->Next != NULL; AppPtr = AppPtr->Next);
        AppSelect = AppPtr;
        MenuSelect = NULL;
        option = AppLinkListNum + MenuLinkListNum - 1;
      } else {
        AppSelect = NULL;
        Ptr = NowMenu->Sub;
        for(; Ptr->Next != NULL; Ptr = Ptr->Next);
        MenuSelect = Ptr;
        option = AppLinkListNum + MenuLinkListNum - 1;
      }
    } else if(AppSelect != NULL) {
      if(AppSelect->Pre != NULL) {
        AppSelect = AppSelect->Pre;
        MenuSelect = NULL;
        option--;
      } else if(NowMenu->Sub != NULL) {
        AppSelect = NULL;
        Ptr = NowMenu->Sub;
        for(; Ptr->Next != NULL; Ptr = Ptr->Next);
        MenuSelect = Ptr;
        option--;
      } else {
        AppPtr = NowMenu->App;
        for(; AppPtr->Next != NULL; AppPtr = AppPtr->Next);
        AppSelect = AppPtr;
        MenuSelect = NULL;
        option = AppLinkListNum + MenuLinkListNum - 1;
      }
    }
  }

  //执行APP/绘制UI界面
  if(RunApp && AppSelect->Function != NULL) {
    AppSelect->Function(Key,&Exit);
    if(Exit){
      RunApp=false;
      tick = Graphics_GetTick();
      Success_flag = false;
      Exit = false;
    }
  } else {
    NowMenu->Interface(Fontsize, NowMenu, option, MenuLinkListNum, AppLinkListNum, BackColor, FontColor, style);
  }

  //虚化效果
  if(Graphics_GetTick() - tick < TRANSITION_TIMES / 3) {
    Graphics_Clear();
  } else if(Graphics_GetTick() - tick < TRANSITION_TIMES) {
    Graphics_Bokeh();
  } else {
    Success_flag = true;
  }

  //在屏幕上显示
  Graphics_Display();
}

