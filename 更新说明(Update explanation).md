# CH：

## 更新：

v1.0 —— v1.1

## 改动：

### 一：

        在菜单项的结构体中添加了界面绘制的函数指针，使得每个菜单项都有一个独立的界面绘制函数，增强了灵活性；同时保留了 SXGUI_Interface函数，不想单独写可以直接调用；

### 二：

        添加 INTERFACE_PARAMETERS宏定义，该宏定义为界面绘制函数的输入参数，避免输入参数输错导致一系列问题；



# EN:

## Update:

v1.0 —— v1.1

## Change:

### First:

        Added function pointers for interface drawing in the structure of menu items, so that each menu item has an independent interface drawing function, enhancing flexibility; At the same time, the SXGUI_interface function is retained, and if you don't want to write it separately, you can call it directly;

### Second:

        Add the definition of the INTERFaced Parameters macro, which is defined as the input parameter of the interface drawing function to avoid a series of problems caused by input parameter errors;
