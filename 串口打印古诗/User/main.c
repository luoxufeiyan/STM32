/**
  ******************************************************************************
  * @file    main.c
  * @author  LXFY
  * @version V1.0
  * @date    20150702
  * @brief   通过按键控制串口输出古诗<饮湖上初晴后雨>-延时法
  ******************************************************************************
  */ 
 
 
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_key.h"
#include "stm32f10x_it.h"

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
  
void Delay(__IO uint32_t nCount)	 
{
	for(; nCount != 0; nCount--);
}
int main(void)
{
	//int flag=0;
    USART1_Config();
	NVIC_Configuration();
    while(1){
        if((Key_Scan(GPIOA,GPIO_Pin_0)== KEY_ON)){
        printf("\r\n 水光潋滟晴方好, \r\n");
        }
        Delay(0xFFFFF);
        if((Key_Scan(GPIOA,GPIO_Pin_0)== KEY_ON)){
        printf("\r\n 山色空濛雨亦奇. \r\n");
        }
        Delay(0xFFFFF);
        if((Key_Scan(GPIOA,GPIO_Pin_0)== KEY_ON)){
        printf("\r\n 欲把西湖比西子, \r\n");
        }
        Delay(0xFFFFF);
        if((Key_Scan(GPIOA,GPIO_Pin_0)== KEY_ON)){
        printf("\r\n 淡妆浓抹总相宜. \r\n");
        Delay(0xFFFFF);
        }
        
    }
}
/*********************************************END OF FILE**********************/























