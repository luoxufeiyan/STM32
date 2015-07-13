#ifndef __MP3_H
#define	__MP3_H

#include "stm32f10x.h"


/*
三个坐标系统的转换：


触摸屏：
				column 240
			 
			 x
        _ _ _ _ _ _
       A           |
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |  320        
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
	(0,0)- - - - - ->  y       
				
液晶显示字体及清屏等函数：
				
				column 240
			 
										x
  (0,0) ----------->
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |  320        
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
			 V- - - - - - 

       y
			 
液晶显示图像：
				
				column 240		 
									 X	
				-----------A
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |  320        
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
		 Y <- - - - - -(0,0) 

       

转换关系：
Y字 = 320 - X触摸
X字 = Y触摸

X图像 = X触摸
Y图像 = 240-Y触摸

Y图像 = 240 - X字
X图像 = 320 - Y字




*/



/* WAV文件头部格式 */
typedef __packed struct
{ 
	uint8_t		riff[4];					/* = "RIFF"							       				*/
	uint32_t	size_8;						/* 从下个地址开始到文件尾的总字节数		*/
	uint8_t		wave[4];					/* = "WAVE" 							   					*/
	uint8_t		fmt[4];						/* = "fmt " 							   					*/
	uint32_t	fmtSize;					/* 下一个结构体的大小(一般为16)			  */
	 
	uint16_t	wFormatTag;				/* 编码方式,一般为1						   			*/
	uint16_t	wChannels;				/* 通道数，单声道为1，立体声为2			  */
	uint32_t	dwSamplesPerSec;	/* 采样率								   						*/
	uint32_t	dwAvgBytesPerSec;	/* 每秒字节数(= 采样率 × 每个采样点字节数) */
	uint16_t	wBlockAlign;			/* 每个采样点字节数(=量化比特数/8*通道数)  	*/
	uint16_t	wBitsPerSample;		/* 量化比特数(每个采样需要的bit数) 		   		*/
																			   
	uint8_t		data[4];					/* = "data"; 							   					*/
	uint32_t	datasize;					/* 纯数据长度 							   				*/
} WavHead;


/* 播放器状态标志   */
typedef enum {S_READY,S_PLAY,S_STOP,S_SWITCH}PLAYER_STATE;
//准备、播放、暂停、切歌


/* 触摸事件标志 */
typedef enum {E_NULL,E_PLAY_STOP,E_PREV,E_NEXT,E_UP,E_DOWN,E_SELECT,E_SELT_UP,E_SELT_DOWN,E_SELT_PREV,E_SELT_NEXT,E_SELT_OK,E_LOUD_SPEAK}TOUCH_EVEN;
//空事件、播放暂停事件、上一首、下一首、音量+、音量减、直接选择歌曲、向上选择、向下选择、上一页、下一页、选择OK、选择扬声器、



void player_run(void);
void touch_process (void);	/* 中断服务函数 */							

#endif


