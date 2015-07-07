/**
  ******************************************************************************
  * @file    main.c
  * @author  LXFY
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   液晶显示BMP图片
  ******************************************************************************
  */
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_sdfs_app.h"
#include "bsp_bmp.h"

/*
 * 要想图片显示成功
 * 需把字库拷贝到sd卡上，然后把sd卡插到开发板的卡槽上
 */
int main(void)
{
   /* USART1 config */
		//USART1_Config();
		//printf("\r\n USART testing...... \r\n");
	
		LCD_Init();	
		LCD_Clear(0, 0, 240, 320, BACKGROUND);		
		
		/* 初始化sd卡文件系统，因为汉字的字库放在了sd卡里面 */
		Sd_fs_init();	
	
		/* 设置Lcd Gram 扫描方向为: 左上角 -> 右下角*/
		//Lcd_GramScan( 1 );
		LCD_DispStr(5, 5, (uint8_t *)"fatfs init success", RED);
		LCD_DispStr(5, 25, (uint8_t *)"ensure sdcard has been inserted", RED);
	
	  LCD_DispStrCH(50, 50, (uint8_t *)"位图显示", RED);
		/* 显示BMP图片 */
		Lcd_show_bmp(0, 0,"/lxfy.bmp");  
		while(1);
}