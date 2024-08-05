# CH：

## 更新：

v1.1 —— v1.1（小改动）

## 改动（相较于 v1.0）：

### 一：

        在菜单项的结构体中添加了界面绘制的函数指针，使得每个菜单项都有一个独立的界面绘制函数，增强了灵活性；同时保留了 SXGUI_Interface函数，不想单独写可以直接调用；

        添加了APP主动退出的功能，只需要APP内调用 “ APP_EXIT; ” 即可完成主动退出。注意，“APP_EXIT” 是一个宏定义，具体为： “ *Exit = true; return ”

### 二：

        添加 INTERFACE_PARAMETERS宏定义，该宏定义为界面绘制函数的输入参数，避免输入参数输错导致一系列问题；

        添加 APP_PARAMETERS 宏定义，该宏定义为APP的输入参数，避免参数输入错误

        添加 APP_EXIT 宏定义，该宏定义只能在APP函数内部调用，用于APP的主动退出，会立即退出当前函数



# EN:

## Update:

v1.1 —— v1.1 (Minor changes)

## Change (Compare with v1.0):

### First:

        Added function pointers for interface drawing in the structure of menu items, so that each menu item has an independent interface drawing function, enhancing flexibility; At the same time, the SXGUI_Interface function is retained, and if you don't want to write it separately, you can call it directly;

        Added the function of app active exit, which only requires calling "APP_EXIT;" within the app to complete the active exit. Note that "APP_EXIT" is a macro definition, specifically:  " *Exit=true; return "

### Second:

        Add the definition of the INTERFaced Parameters macro, which is defined as the input parameter of the interface drawing function to avoid a series of problems caused by input parameter errors;

        Add the APP_PARAMETERS macro definition, which is defined as the input parameter of the APP to avoid parameter input errors

        Add the APP_EXIT macro definition, which can only be called within the APP function and is used for the APP's active exit. It will immediately exit the current function
