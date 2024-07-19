/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sxgui.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//APP函数
void Draw_Line(SXGUI_KeyItem *Key){
  static int tick;
  static int i;
  if(Graphics_GetTick()-tick>15){
    i++;
    tick = Graphics_GetTick();
  }
  if(i>127){
    i=0;
  }
  Graphics_DrawLine(0,0,i,63,1,SOLID_LINE);
  Graphics_DrawLine(127,0,127-i,63,1,DASHED_LINE);
}

void Draw_Round(SXGUI_KeyItem *Key){
  static int tick;
  static int i;
  static bool flag = true;
  if(Graphics_GetTick()-tick>30){
    if(flag){
      i++;
    }else{
      i--;
    }
    tick = Graphics_GetTick();
  }
  if(i>31){
    flag = false;
  }else if(i<0){
    flag = true;
  }
  char tmp[8];
  sprintf(tmp,"r=%d",i);
  Graphics_DrawRound(64,31,i,1,1,SOLID);
  Graphics_ShowString(0,0,tmp,16,1);
}

void Round_Rect(SXGUI_KeyItem *Key){
  static int tick;
  static int width=0;
  static bool flag = true;
  if(Graphics_GetTick()-tick>15){
    if(flag){
      width++;
    }else{
      width--;
    }
    tick = Graphics_GetTick();
  }
  if(width>63){
    flag = false;
  }else if(width<0){
    flag = true;
  }
  Graphics_DrawRoundRect(0,0,width,width/2,4,1,HOLLOW);
  Graphics_DrawRoundRect(64,0,width+64,width/2,4,1,SOLID);
  Graphics_DrawRoundRect(0,32,width,width/2+32,8,1,SOLID);
  Graphics_DrawRoundRect(64,32,width+64,width/2+32,8,1,HOLLOW);
}

void Kernel_Infor(SXGUI_KeyItem *Key){
  Graphics_ShowString(0,24,"内核版本:v1.0",16,WHITE_COLOR);
}

void GUI_Infor(SXGUI_KeyItem *Key){
  Graphics_ShowString(0,24,"SXGUI v1.0",16,WHITE_COLOR);
}

void Graphics_Infor(SXGUI_KeyItem *Key){
  Graphics_ShowString(0,24,"图形库版本:v1.1",16,WHITE_COLOR);
}

void Driver_Infor(SXGUI_KeyItem *Key){
  Graphics_ShowString(0,24,"驱动版本:v1.3.M",16,WHITE_COLOR);
}

void Writer_Infor(SXGUI_KeyItem *Key){
  static int tick;
  static bool CB = true;
  Graphics_ShowString(0,0,"作者:岁心",16,WHITE_COLOR);
  if(Graphics_GetTick()-tick>5000){
    CB = !CB;
    tick = Graphics_GetTick();
  }
  if(CB){
    Graphics_ShowString(0,24,"CSDN: https://blog.csdn.net/m0_73677866",12,WHITE_COLOR);
    return;
  }
  Graphics_ShowString(0,24,"Bilibili: https://space.bilibili.com/3494359452354953",12,WHITE_COLOR);
}

void PID_Infor(SXGUI_KeyItem *Key){
  char tmp[16];
  sprintf(tmp,"Kp: %.3f",PID_Parameter.Kp);
  Graphics_ShowString(0,0,tmp,12,WHITE_COLOR);
  sprintf(tmp,"Ki: %.3f",PID_Parameter.Ki);
  Graphics_ShowString(0,12,tmp,12,WHITE_COLOR);
  sprintf(tmp,"Kd: %.3f",PID_Parameter.Kd);
  Graphics_ShowString(0,24,tmp,12,WHITE_COLOR);
  sprintf(tmp,"Factor: %.3f",PID_Parameter.Factor);
  Graphics_ShowString(0,36,tmp,12,WHITE_COLOR);
  sprintf(tmp,"MinTime: %dms",PID_Parameter.MinTime);
  Graphics_ShowString(0,48,tmp,12,WHITE_COLOR);
}

void Hub_Interface(INTERFACE_PARAMETERS){
  int List_x = Fontsize / 2,
      List_y = Fontsize + 1;

  Graphics_ListChoose(List_x, List_y, 128 - List_x - 1, 64 - Fontsize - 1,
                      Fontsize, Menu, MenuNum, AppNum, option, POINTER_STYLE, FontColor);

  Graphics_ShowString(0, 0, Menu->name, Fontsize, FontColor);
}

//调用SXGUI的库函数, 初始化并创建GUI界面
void SX_MenuInit(void){
  SXGUI_Init("Hub",Hub_Interface);
  SXGUI_MenuItem *Sub1 = SXGUI_CreateMenu("信息",SXGUI_Interface);
  SXGUI_MenuItem *Sub2 = SXGUI_CreateMenu("GUI信息",SXGUI_Interface);
  SXGUI_APPItem *App1 = SXGUI_CreateApp("Draw Line",Draw_Line);
  SXGUI_APPItem *App2 = SXGUI_CreateApp("Draw Round",Draw_Round);
  SXGUI_APPItem *App3 = SXGUI_CreateApp("Round Rect",Round_Rect);
  SXGUI_APPItem *App4 = SXGUI_CreateApp("内核版本",Kernel_Infor);
  SXGUI_APPItem *App5 = SXGUI_CreateApp("GUI版本",GUI_Infor);
  SXGUI_APPItem *App6 = SXGUI_CreateApp("图形库版本",Graphics_Infor);
  SXGUI_APPItem *App7 = SXGUI_CreateApp("驱动信息",Driver_Infor);
  SXGUI_APPItem *App8 = SXGUI_CreateApp("作者信息",Writer_Infor);
  SXGUI_APPItem *App9 = SXGUI_CreateApp("PID Parameter",PID_Infor);
  SXGUI_RootAddSubMenu(Sub1);
  SXGUI_RootAddApp(App1);
  SXGUI_RootAddApp(App2);
  SXGUI_RootAddApp(App3);
  SXGUI_AddSubMenu(Sub1,Sub2);
  SXGUI_AddApp(Sub2,App4);
  SXGUI_AddApp(Sub2,App5);
  SXGUI_AddApp(Sub2,App6);
  SXGUI_AddApp(Sub2,App7);
  SXGUI_AddApp(Sub2,App8);
  SXGUI_AddApp(Sub2,App9);
}

//读取按键并消抖
#define KeyNum    4
void Scan_Key(SXGUI_KeyItem *Key){
  static int ShakeFree[KeyNum];
  static int tick;
  static SXGUI_KeyItem Last;
  if(HAL_GetTick()-tick>2){
    tick = HAL_GetTick();
  }else{
    return;
  }
  bool *KeyPtr;
  GPIO_PinState PINState;
  SXGUI_KeyItem tmp;
  for(int i=0;i<KeyNum;i++){
    switch(i){
      case 0:KeyPtr = &Key->Back;
        PINState = HAL_GPIO_ReadPin(GPIOC,BACK_KEY_Pin);
        break;
      case 1:KeyPtr = &Key->Pre;
        PINState = HAL_GPIO_ReadPin(GPIOC,PRE_KEY_Pin);
        break;
      case 2:KeyPtr = &Key->Next;
        PINState = HAL_GPIO_ReadPin(GPIOC,NEXT_KEY_Pin);
        break;
      case 3:KeyPtr = &Key->OK;
        PINState = HAL_GPIO_ReadPin(GPIOC,OK_KEY_Pin);
        break;
    }
    if(PINState==RESET){
      ShakeFree[i]++;
      if(ShakeFree[i]>5){
        *KeyPtr = true;
        ShakeFree[i] = 10;
      }else{
        *KeyPtr = false;
      }
    }else{
      ShakeFree[i]--;
      if(ShakeFree[i]<5){
        ShakeFree[i]=0;
        *KeyPtr = false;
      }else{
        *KeyPtr = true;
      }
    }
  }
  tmp.Back = Key->Back;
  tmp.Pre = Key->Pre;
  tmp.Next = Key->Next;
  tmp.OK = Key->OK;
  
  Key->Back ^= Last.Back;
  Key->Pre ^= Last.Pre;
  Key->Next ^= Last.Next;
  Key->OK ^= Last.OK;
  
  Key->Back &= tmp.Back;
  Key->Pre &= tmp.Pre;
  Key->Next &= tmp.Next;
  Key->OK &= tmp.OK;
  
  Last.Back = tmp.Back;
  Last.Pre = tmp.Pre;
  Last.Next = tmp.Next;
  Last.OK = tmp.OK;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  //设置PID参数
  PID_Parameter.Kp = -3.2;
  PID_Parameter.Ki = 4.032;
  PID_Parameter.Kd = 0.879;
  PID_Parameter.Factor = 0.1;
  PID_Parameter.MinTime = 10;
  
  //创建按键结构体
  SXGUI_KeyItem *Key = malloc(sizeof(SXGUI_KeyItem));
  
  //初始化图形库和GUI库
  Graphics_Init();
  SX_MenuInit();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    //主程序执行
    Scan_Key(Key);
    SXGUI_Main(16,BLACK_COLOR,WHITE_COLOR,FRAME_STYLE,Key);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
