/**
  ******************************************************************************
  * @file    main.c
  * @author  LXFY
  * @version V1.0
  * @date    20150701
  * @brief   通过按键控制串口输出古诗<饮湖上初晴后雨>
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
  
//写入软件中断实现
int main(void)
{
	//int flag=0;
    USART1_Config();
	NVIC_Configuration();
    while(1){
        if((Key_Scan(GPIOA,GPIO_Pin_0)== KEY_ON)&&flag==0){
        printf("\r\n 水光潋滟晴方好, \r\n");
        //flag++;
        } 
        if((Key_Scan(GPIOA,GPIO_Pin_0)== KEY_ON)&&flag==1){
        printf("\r\n 山色空濛雨亦奇. \r\n");
        //flag++;
        }
        if((Key_Scan(GPIOA,GPIO_Pin_0)== KEY_ON)&&flag==2){
        printf("\r\n 欲把西湖比西子, \r\n");
        //flag++;
        }
        if((Key_Scan(GPIOA,GPIO_Pin_0)== KEY_ON)&&flag==3){
        printf("\r\n 淡妆浓抹总相宜. \r\n");
        //flag=0;
        }
        
    }
	for(;;)
	{
		
	}
}
/*********************************************END OF FILE**********************/























