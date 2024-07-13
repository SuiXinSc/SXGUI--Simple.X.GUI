# SXGUI--Simple.X.GUI

#### 介绍
针对嵌入式开发的一个跨平台GUI库，内置PID过渡动画，旨在降低GUI开发的复杂度
例程使用STM32CubeMX生成初始化文件，在Keil5中开发，平台为 STM32F401RCT6，使用128*64 OLED显示屏，驱动芯片为SSD1306
另：我使用的是UTF8编码，如出现汉字乱码情况，请将编码格式换为 UTF8

#### 使用说明
1.基础：根据不同平台按照要求修改graphics_api.c内容即可
2.进阶：更改sxgui.c中的界面绘制函数（SXGUI_Interface）实现自己需求的界面

#### 参与贡献
岁心

Demo演示：http://t.csdnimg.cn/Q5gJz

有问题可以问我，有时间就答复
交流Q群：659512171