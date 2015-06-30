/**
  ******************************************************************************
  * @file    main.c
  * @author  LXFY
  * @version V1.0 BUILD 150629
  * @brief   °´¼üÊµÑé
  ******************************************************************************
  * @attention
  * GitHub  :https://github.com/luoxufeiyan/STM32
  * Blog    :http://www.luoxufeiyan.com
  *
  ******************************************************************************
  */
 
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_key.h"

/**
  * @brief  Ö÷º¯Êý
  * @param  Î
  * @retval Þ
  */
int main(void)
{
	//ÅäÖÃLED
	LED_GPIO_Config();
	Key_GPIO_Config();
	LED2(ON);
	while(1)
	{
		if(Key_Scan(GPIOC,GPIO_Pin_13)== KEY_ON)
			//GPIO_WriteBit(GPIOC,GPIO_Pin_3,(BitAction)((1-GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_3))));
			LED2_TOGGLE;
	}
}

/*********************************************END OF FILE**********************/

