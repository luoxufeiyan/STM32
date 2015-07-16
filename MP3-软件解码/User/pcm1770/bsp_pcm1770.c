#include "stm32f10x.h"
#include "bsp_pcm1770.h"
#include "bsp_ili9341_lcd.h"
#include <stdio.h>


uint8_t voice=28;						//音量大小


/**
  * @brief  Delay 短暂延时
  * @param  Delay Num
  * @retval None
  */
static void Delay(u32 Num)
{
	vu32 Count = Num*4;
	
	while (--Count);	
}



/**
  * @brief  PCM1770 Init初始化PCM1770用到的引脚，并复位芯片
  * @param  None
  * @retval None
  */
void PCM1770Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//使能控制信号的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	Delay(50);
	
	//硬件复位
	PCM_PD_SET_L;
	Delay(1000);
	PCM_PD_SET_H;  	
	Delay(100);

	PCM_CS_SET_H ;
	PCM_CLK_SET_H;
	PCM_DAT_SET_H; 	
}



/**
  * @brief  向PCM1770写入控制数据
  * @param  Reg:寄存器索引
	*					Data:要写入的数据
  * @retval None
  */
  
void PCM_WriteData(const u8 Reg, const u8 Data)
{
	vu16 TrasferData, i; 
	
	TrasferData = Data;
	TrasferData |= (Reg<<8)&0xff00;

	PCM_CS_SET_L;			//片选
	Delay(10);
	for (i = 0; i < 16; i++)
	{
		//传输时MSB first
		PCM_CLK_SET_L;
		
		if (TrasferData&(0x8000>>i))
		{
		   PCM_DAT_SET_H;	
		}
		else
		{
			 PCM_DAT_SET_L;	
		}
		Delay(10);			//等数据稳定
		PCM_CLK_SET_H;	//上升沿写入
		Delay(10);			//等待从机读数据		
	}		
	
	PCM_CLK_SET_H;
  PCM_DAT_SET_H;	

	PCM_CS_SET_H;			//释放片选
	Delay(10);
}
  
/**
  * @brief  Volume_Add
  * @param  None
  * @retval None
  */
void PCM1770_VolumeSet(vu8 vol)
{
		char lcd_char[10] ;
		voice = vol;

		PCM_WriteData(0x01, vol);//调整左耳机整音量
		PCM_WriteData(0x02, vol);//调整右耳机音量  

		Lcd_GramScan( 1 );
		sprintf(lcd_char,"音量:%2d",voice);		//显示音量到LCD上
		LCD_DispEnCh(155,235,lcd_char,BLUE);	

}

/**
  * @brief  Volume_Dec
  * @param  None
  * @retval None
  */
void Volume_Dec(void)
{
	char lcd_char[10] ;
	
	if (voice > 0)
	{
		voice--;
		PCM_WriteData(0x01, voice);//调整左耳机整音量
		PCM_WriteData(0x02, voice);//调整右耳机音量  	
	
		Lcd_GramScan( 1 );
		sprintf(lcd_char,"音量:%2d",voice);	//显示音量到LCD上
		LCD_DispEnCh(155,235,lcd_char,BLUE);

	}	
	
}

/**
  * @brief  Volume_Add
  * @param  None
  * @retval None
  */
void Volume_Add(void)
{
	char lcd_char[10];
	if (voice < C_VOLUME_MAX)
	{
		voice++;
		PCM_WriteData(0x01, voice);//调整左耳机整音量
		PCM_WriteData(0x02, voice);//调整右耳机音量  
	
		Lcd_GramScan( 1 );
		sprintf(lcd_char,"音量:%2d",voice);	//显示音量到LCD上
		LCD_DispEnCh(155,235,lcd_char,BLUE);
		
	}
}



/**
  * @brief  Volume_Add
  * @param  None
  * @retval None
  */
void PCM1770_Mute(void)
{
		PCM_WriteData(0x01, 0xc0);//左右耳机静音 
		PCM_WriteData(0x02, 0x00);//右耳机静音		
}



/**
  * @brief  Loud_Speaker_ON 开启扬声器
  * @param  none
  * @retval None
  */
void Loud_Speaker_ON(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		//使能控制信号的时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);

	}

/**
  * @brief  Loud_Speaker_OFF 关闭扬声器
  * @param  none
  * @retval None
  */
void Loud_Speaker_OFF(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		//使能控制信号的时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		GPIO_SetBits(GPIOA,GPIO_Pin_8);

	}


/******************* end of file**************************/
