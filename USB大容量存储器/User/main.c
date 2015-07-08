/**
  ******************************************************************************
  * @file    main.c
  * @author  LXFY
  * @version V1.0
  * @date    2013-07-08
  * @brief   usb mass storage USB大容量存储实现 
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "bsp_sdio_sdcard.h"
#include "bsp_usart1.h"	
#include "hw_config.h" 
#include "usb_lib.h"
#include "usb_pwr.h"
#include "bsp_led.h"  

void USB_Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

int main(void)
{
	 /* USART1 config */
	USART1_Config();
	
  LED_GPIO_Config();
  
	/*初始化SD卡*/
	Set_System();
  	printf("\n initializing sd card \n");
	/*设置USB时钟为48M*/
	Set_USBClock();
 	
	/*配置USB中断(包括SDIO中断)*/
	USB_Interrupts_Config();
 
	/*USB初始化*/
 	USB_Init();
    printf("\n initializing USB, please wait.\n");
 	while (bDeviceState != CONFIGURED);	 //等待配置完成
	   
	printf("\r\n USB storage mounted. Excited!\r\n");
	 
  while (1)
  {
    LED1_TOGGLE;
    USB_Delay(0x0FFFFF);
  }
}

/* -------------------------------------end of file -------------------------------------------- */
