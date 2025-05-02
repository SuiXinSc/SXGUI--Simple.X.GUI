# CH：

## 更新：

v1.2 —— v1.3

## 改动（相较于 v1.2）：

### 一，重磅更新：

​	彩屏专门适配，同时优化了图形库处理逻辑，开发者可通过重写Graphics中的图形接口实现任意屏幕的适配

### 二，页面过渡：

​	取消了单独的固定页面过渡，而是集成到了菜单中的Interface接口中，同时在该函数的参数中添加了一个 tick参数，表示的是进入该界面的时间，可以用于页面过渡动画的时间戳

### 三，其余改动：

​	删除图形库二元光栅，用SetWindows代替实现部分功能，方便移植到各种显示屏并降低占用

​	添加了版本的宏定义，为一串字符串

### 另外：

​	彩屏Demo使用的是 STM32F407ZGT6，采用20pin 8位并口1.8寸LCD，驱动芯片为ST7735，使用FSMC+DMA驱动。默认graphics_api中是LCD的，需要OLED版本只需要换成OLED的驱动并且把graphics_api中的对应函数改成OLED的就可以了（换完驱动哪个报错改哪个）

# EN:

## Update:

v1.2 —— v1.3

## Change (Compare with v1.2):

### First, Heavy update:

​	The color screen is specially adapted and the graphics library processing logic is optimized. Developers can adapt to any screen by rewriting the graphics interface in Graphics

### Second, Page transition:

​	The separate fixed page transition has been eliminated and integrated into the Interface interface in the menu. At the same time, a "tick" parameter has been added to the function's parameters, indicating the time of entering the interface, which can be used as timestamps for page transitions.

### Third, Other changes:

​	Delete the binary raster in the graphics library and replace some functions with SetWindows to facilitate porting to various displays and reduce usage

### By the way:

​	The color screen demo uses STM32F407ZGT6, a 20 pin 8-bit parallel port 1.8-inch LCD, with ST7735 driver chip and FSMC+DMA driver. The default graphics_api.c is for LCD. If you need the OLED version, simply switch to the OLED driver and change the corresponding function in the graphics_api.c to OLED (after changing the driver, which one will report an error and change which one)

