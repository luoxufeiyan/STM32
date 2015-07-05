/**
  ******************************************************************************
  * @file    main.c
  * @author  LXFY
  * @version V1.0
  * @date    2013-07-02
  * @brief   屏幕显示 HelloWorld
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_usart1.h"

extern void Lcd_Delay(__IO uint32_t nCount);

uint32_t n;

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{  
  LCD_Init();	
  
	LCD_Clear(0, 0, 240, 320, BACKGROUND);	
  
	LCD_DispStr(0, 0, (uint8_t *)"Hello, World", BLUE);	
    LCD_DispStr(100, 100, (uint8_t *)"Hello,World!", BLUE);	
	
	//LCD_Point(320,240);
	
	while( 1 ){}
}


