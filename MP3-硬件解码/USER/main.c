/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * 文件名  ：main.c
 * 描述    ：MP3文件测试，支持中英文、短文件名和长文件名。
 *           测试信息显示在串口上而不是液晶上。         
 * 实验平台：野火STM32开发板
 * 库版本  ：ST3.5.0
 * 写在前面: 前两天改好了STM32的MP3例程.但是忘掉传到GitHub上来了.这个工程文件是<STM32库开发实战指南>的附录例程.用到的是VS1003的硬件解码芯片,与helix的软解方案不同.--LXFY
**********************************************************************************/
#include "stm32f10x.h"
#include "mp3play.h "

/*
 * 函数名：main
 * 描述  ：主函数
 * 输入  ：无
 * 输出  ：无
 */
int main(void)
{	 
	SysTick_Init();			/* 配置SysTick 为10us中断一次 */      
	USART1_Config(); 		/* 配置串口1 115200 8-N-1 */
	
	/* Interrupt Config,配置sdio的中断优先级， */
	NVIC_Configuration();
	
	printf(" \r\n 这是一个MP3测试例程 !\r\n " );  
	           
	VS1003_SPI_Init();		/* MP3硬件I/O初始化 */ 
	
	MP3_Start(); 			/* MP3就绪，准备播放，在vs1003.c实现 */ 
	
	MP3_Play();	  			/* 播放SD卡(FATFS)里面的音频文件 */ 		
	 
	/* Infinite loop */
	while (1)
	{
	}
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/

