/**
  ******************************************************************************
  * @file    main.c
  * @author  LXFY
  * @version V1.0 BUILD 150630
  * @brief   EXTIµÄÍâ²¿ÖÐ¶Ï
  ******************************************************************************
  * @attention
  * GitHub  :https://github.com/luoxufeiyan/STM32
  * Blog    :http://www.luoxufeiyan.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_exti.h"
				
#define CLI() __set_PRIMASK(1)
#define SEI() __set_PRIMASK(0)

/**
  * @brief  Ö÷º¯Êý
  * @param  Î
  * @retval Þ
  */
int main(void)
{
			LED_GPIO_Config();
			LED1_ON;
			//LED2_ON;
	
			CLI();
			SEI();
			
			EXTI_PA0_Config();
			//EXTI_P13_Config();
	
  	  //while(1);
}

/*********************************************END OF FILE**********************/

