/**
  ******************************************************************************
  * @file    main.c
  * @author  fire & LXFY
  * @version V1.0
  * @date    2015-07-14
  * @brief   液晶画板
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_SysTick.h"
#include "bsp_touch.h"
#include "bsp_sdfs_app.h"
#include "bsp_spi_flash.h"
#include "bsp_usart1.h"
#include "bsp_led.h"
#include "bsp_key.h"

#define CLI() __set_PRIMASK(1)
#define SEI() __set_PRIMASK(0)

extern volatile unsigned char touch_flag;

/**
  * @brief  主函数
  * @param  取消了触摸屏修正  
  * @retval 无
  */
int main(void)
{	
  //char name=0;
  uint8_t k;
	
  /* 系统定时器 1us 定时初始化 */
  SysTick_Init();
  LCD_Init();	
  /* GRAM扫描方向为左下脚->右上角 */
  Lcd_GramScan(2);
  LCD_Clear(0, 0, 320, 240, BACKGROUND);
  /*------------------------------------------------------------------------------------------------------*/
  		/* 初始化sd卡*/
		Sd_fs_init();	
		/* 初始化串口 */
        //USART1_Config();
        /* 触摸屏IO和中断初始化 */
        Touch_Init();
        /* 初始化外部FLASH */
        SPI_FLASH_Init();
/*==========================================初始化完毕=================================================*/
//开始校对

		while(Touch_Calibrate() !=0);
		//#endif
        
  /* 触摸取色板初始化 */
  Palette_Init();
  
	while( 1 )
  {
    if(touch_flag == 1)			/*如果触笔按下了*/
    {
      /*获取点的坐标*/
      if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)      
      {	
        Palette_draw_point(display.x,display.y);	
      }
    }
  }
}

/* ------------------------------------------end of file---------------------------------------- */

