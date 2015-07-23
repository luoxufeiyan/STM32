/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   enc28j60+lwip测试.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f10x.h"

#include "err.h"
#include "etharp.h"

#include "netconfig.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h" 
#include "spi.h"
#include "bsp_led.h"

#include "cmd.h"
#include "httpd.h"

/*****************************************************************************************************************************/
    
__IO uint32_t LocalTime = 0; 
/* this variable is used to create a time reference incremented by 10ms */ 
	
int main(void)
{
	/*初始化串口*/
  USART1_Config();

	/*初始化 以太网SPI接口*/
	ENC_SPI_Init();

	/*初始化systick，用于定时轮询输入或给LWIP提供定时*/
	SysTick_Init();		

	/*初始化LED使用的端口*/
	LED_GPIO_Config();
	 
	printf("\r\n*************STM32_enc8j60+lwip移植实验*************\r\n");
	  
  	/* 初始化LWIP协议栈*/
	LwIP_Init();
    
    printf("LWIP mounted.IP addr is\n");
    //printf("My IP:&s",ipaddr.addr);
	/*初始化web server 显示网页程序*/
	
    httpd_init();
  
  /* 初始化telnet   远程控制 程序 */   
  CMD_init();

  /* Infinite loop */
  while ( 1 )
	{	
		/*轮询*/  
		LwIP_Periodic_Handle(LocalTime);		          	  
  }
}