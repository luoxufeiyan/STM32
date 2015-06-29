/**
  ******************************************************************************
  * @file    main.c
  * @author  LXFY
  * @version V1.0 BUILD 150629
  * @brief   LEDÁ÷Ë®µÆ
  ******************************************************************************
  * @attention
  * GitHub  :https://github.com/luoxufeiyan/STM32
  * Blog    :http://www.luoxufeiyan.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "bsp_led.h"

void Delay(__IO u32 nCount); 

/**
  * @brief  Ö÷º¯Êý
  * @param  Î
  * @retval Þ
  */
int main(void)
{	
	/* LED ¶Ë¿Ú³õÊ¼»¯ */
	LED_GPIO_Config();	 
 
	/* ·½·¨2£¬Ê¹ÓÃ¹Ì¼þ¿â¿ØÖÆIO */
	while (1)
	{
		LED1( ON );			  // ÁÁ
		Delay(0x0FFFFF);
		LED1( OFF );		  // Ãð

		LED2( ON );			  // ÁÁ
		Delay(0x0FFFFF);
		LED2( OFF );		  // Ãð

		LED3( ON );			  // ÁÁ
		Delay(0x0FFFFF);
		LED3( OFF );		  // Ãð	   
	}

}

void Delay(__IO uint32_t nCount)	 //¼òµ¥µÄÑÓÊ±º¯Êý
{
	for(; nCount != 0; nCount--);
}