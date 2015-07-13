#ifndef __MP3PLAY_H
#define	__MP3PLAY_H

#include "stm32f10x.h"
#include "usart1.h"
#include "SysTick.h"
#include "vs1003.h"
#include "sdio_sdcard.h"
#include "ff.h"
#include "diskio.h"
#include <string.h>
#include <stdio.h>	 

#define MIN(x, y)			((x) < (y)? (x) : (y))
#define READBUF_SIZE  ( 5*1024 )/*5*1024*/

#define TRUE 1
#define FALSE 0

/* music tag information structure */
struct tag_info
{
	char title[40];        // music title               
	char artist[40];        // music artist
	char album[40];         // album        
	char year[8];						// year

	int duration;           // music total duration (second)   
	int position;           // music current position (second) 

	uint8_t  length;        // bits PerSample			       
	uint32_t bit_rate;      // bit rate                       
	uint32_t sampling;      // sampling                     
	uint32_t data_start;    // start position of data        
};



/*
 * MP3文件大体分为3部分:TAG_V2(ID3V2),Frame,TAG_V1(ID3V1)。要注意
 * 有些MP3文件尾是没有ID3V1 tag的，有的是MP3文件头部的ID3V2 tag。
 * 其中ID3V1 存放在MP3文件的末尾 ，用16进制的编辑器打开一个MP3文件，
 * 查看其末尾的128个顺序存放字节，数据结构定义如下：
 */

 /*
  *1、每帧的播放时间：无论帧长是多少，每帧的播放时间是26ms。
	*2、FrameSize = ( (( MPEGVersion == MPEG1 ? 144 : 72 ) * Bitrate) / SampleRate ) + PaddingBit 
	*/
typedef struct tagID3V1
{
	char Header[3];	     // 标签头必须是"TAG"，否则被认为没有标签头
	char Title[30];	     // 标题
	char Artist[30];		 // 作者
	char Album[30];			 // 专辑
	char Year[4];				 // 出品年代
	char Comment[28];		 // 备注
	char reserve;				 // 保留
	char track;					 // 音轨
	char Genre;					 // 风格
}ID3V1, *pID3V1;
										   
int Read_ID3V1(FIL *FileObject, struct tag_info *info);
void Read_ID3V2(FIL *FileObject, struct tag_info *info);
void MP3_Play(void);

#endif /* __MP3PLAY_H */

