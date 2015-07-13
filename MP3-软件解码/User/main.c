/**
  ******************************************************************************
  * @file    main.c
  * @author  fire & LXFY
  * @version V1.0
  * @date    2015-07-13
  * @brief   MP3软件解码-helix
  ******************************************************************************
	*/
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_sdfs_app.h"
#include "bsp_bmp.h"
#include "bsp_SysTick.h"
#include "bsp_touch.h"
#include "bsp_spi_flash.h"
#include "bsp_led.h" 
#include "bsp_pcm1770.h"
#include "bsp_iis.h"
#include "mp3.h"




/**
  * @brief  mp3软解main函数，需要把SD备份的文件拷贝到SD卡上，然后把SD卡插到开发板的卡槽上
	*					本工程支持中文长文件名，但下载程序的时候比较慢。
	*					为节省时间：调试的时候可以把ffconf.h中的codepage936换成 437，并把cc936.c文件换成ccsbcs.c文件		
  * @param  无
  * @retval 无
 */
int main(void)
{
		uint8_t k;
	
		/* 初始化LCD */
		LCD_Init();	
		LCD_Clear(0, 0, 240, 320, BACKGROUND);		
		
		/* 初始化sd卡文件系统，因为汉字的字库放在了sd卡里面 */
		Sd_fs_init();	
	
		/* 初始化串口 */
	  USART1_Config();
	
		/* 系统定时器 1us 定时初始化 */
	 //	SysTick_Init();			//本工程把触摸屏函数的延时采用非精确延时，不需要systick了
	 
		/* 初始化LED */
		LED_GPIO_Config();
	
		/* 初始化外部FLASH */
		SPI_FLASH_Init();
		//SPI_FLASH_SectorErase(0);
  
		/* 触摸屏IO和中断初始化 */
		Touch_Init();		
		
		/* 初始化设置I2S */
		I2S_Bus_Init();
		
		/* 初始化PCM1770 */
		PCM1770Init();
		
		/*-------------------------------------------------------------------------------------------------------*/  
		#if 1
		SPI_FLASH_BufferRead(&cal_flag, 0, 1);
		if( cal_flag == 0x45 )
		{
			SPI_FLASH_BufferRead((void*)cal_p, 1, sizeof(cal_p));
			SPI_FLASH_CS_HIGH();
			for( k=0; k<6; k++ )
						printf("\r\n rx = %LF \r\n",cal_p[k]);
		}
		else
		{
			/* 等待触摸屏校正完毕 */
			while(Touch_Calibrate() !=0);
		}
		#elif 0
		/* 等待触摸屏校正完毕 */
		while(Touch_Calibrate() !=0);
		#endif
	
		/* 显示MP3界面图片 */
		Lcd_show_bmp(0, 0,"/mp3player/ui_window.bmp");

		/* 运行MP3播放器 */	
		player_run();
		
		while(1);

}


/******************* end of file**************************/

