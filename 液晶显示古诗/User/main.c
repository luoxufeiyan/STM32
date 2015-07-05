/**
  ******************************************************************************
  * @file    main.c
  * @author  LXFY
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   液晶显示中文
  ******************************************************************************
	*/
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_sdfs_app.h"
#include "bsp_key.h"
#include "stm32f10x_it.h"

void Delay(__IO uint32_t nCount)	 
{
	for(; nCount != 0; nCount--);
}

int main(void)
{
   /* USART1 config */
		//USART1_Config();

		LCD_Init();
        Key_GPIO_Config();	
		LCD_Clear(0, 0, 240, 320, BACKGROUND);		
		/* 初始化SD卡,读取汉语字库文件 */
		Sd_fs_init();		
		LCD_DispStr(50, 5, (uint8_t *)"Hello,World!", RED);   
        LCD_DispEnCh(50, 50, "你好,世界!", RED);
        LCD_DispEnCh(55, 70, (uint8_t *)"饮湖上初晴后雨", RED);
        if((Key_Scan(GPIOA,GPIO_Pin_0)== KEY_ON)){
        LCD_DispEnCh(55, 90, (uint8_t *)"水光潋滟晴方好", RED);
        }
        Delay(0xFFFFF);
        if((Key_Scan(GPIOA,GPIO_Pin_0)== KEY_ON)){
        LCD_DispEnCh(55, 110, (uint8_t *)"山色空濛雨亦奇", RED);
        }
        Delay(0xFFFFF);
        if((Key_Scan(GPIOA,GPIO_Pin_0)== KEY_ON)){
        LCD_DispEnCh(55, 130, (uint8_t *)"欲把西湖比西子", RED);
        }
        Delay(0xFFFFF);
        if((Key_Scan(GPIOA,GPIO_Pin_0)== KEY_ON)){
        LCD_DispEnCh(55,150, (uint8_t *)"淡妆浓抹总相宜", RED);
        Delay(0xFFFFF);
        }
}