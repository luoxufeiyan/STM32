#include <stdio.h>
#include <string.h>
#include "mp3dec.h"
#include "ff.h"
#include "diskio.h"
#include "bsp_iis.h"
#include "bsp_pcm1770.h"
#include "bsp_led.h"
#include "mp3.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_bmp.h"
#include "bsp_touch.h"

#define FILE_NAME_LEN 	50			//文件名长度，如果检测到文件名超过50 则丢弃这个文件 
#define MUSIC_NAME_LEN 	24			//LCD显示的文件名的最大长度
#define _DF1S	0x81


/* 标志位 */
uint8_t select_index = 0;				//界面歌曲选择标志
uint8_t speaker_flag = 0;				//选择使用扬声器的标志（野火logo）
uint16_t play_index = 0;				//存储正在或准备播放的歌曲索引

PLAYER_STATE player_state;			//播放器状态标志位，指示切歌，正在播放，暂停等状态
TOUCH_EVEN touch_even;					//触摸状态标志位，指示触摸到播放器的哪一个按钮，如音量+-等

/* 播放器全局变量 */
uint32_t  file_num = 0;					//存储扫描到的MP3+WAV音频文件数量 
char current_page = 1;					//当前播放器页面
char all_page = 1;							//总共的播放器页面数量
extern uint8_t voice;

/* 文件系统全局变量 */
static FATFS fs;								//文件系统工作空间
static FIL	file;								//文件句柄		
static FRESULT fres;						//文件系统API函数返回值
static unsigned int rw_num;			//已读或已写的字节数

char path[500]="0:";						//扫描SD卡的初始目录，扫描是递归函数，递归越深，此数组取值也要越大(由于RAM限制 此处取少点)	

/* MP3解码时用到的变量 */
uint8_t bufflag=0;										//用于切换buffer的标志
static MP3FrameInfo		Mp3FrameInfo;		//mP3帧信息  
static uint8_t  buffer[1024*4]; 			// 文件缓冲区
static short   outBuf[2][2500];		    // PCM流缓冲，使用两个缓冲区 

/* WAV解码用到的变量 */
								

/* 内部函数声明 */
static void file_scan(char* path);							//扫描文件，处理非递归过程
static FRESULT scan_files (char* path);					//扫描文件，处理递归过程
static void mp3_player(const char *filename);		//mp3播放器
static void wav_player(const char *filename);		//wav播放器	
static void lcd_list(char page);								//显示歌单到LCD
static void even_process(void);									//处理触摸标志事件


/**
  * @brief  player_run 运行mp3播放器过程，内部死循环
  * @param  none
  * @retval none
  */
void player_run(void)
{
		char music_name[FILE_NAME_LEN];	
		
		/* 注册工作区，原sd卡字库也使用了f_mount而且mount(0)但文件系统指针不同,
		继续mount在0盘符的话会出错，我把sd卡GetGBKCode_from_sd函数中的f_mount注册到了1	*/		
		f_mount(0, &fs);	
	
		file_scan(path);							//扫描文件
	
		if(file_num == 0)
		{
			printf("\r\n no mp3 file ! ");
			return; 										//跳出本函数
		}			
			
		player_state = S_READY;				//初始化状态
		touch_even = E_NULL;					//初始化事件标志
		
		
		all_page = (file_num+7)/8 ;		//每页显示8个文件,+7是因为使用进一法
		current_page = 1;
		printf("\r\n file_num =%d,all_page=%d",file_num,all_page);
		
		
		PCM1770_VolumeSet(28);				//不知道为什么初始音量设置无效，第一次上电的时候音乐会很大声，在播放歌曲后调节音量才能正常
																	//在播放歌曲前调节音量有时还会导致后面播放没有声音~	
		
		
		lcd_list(current_page);				//显示歌曲列表,第一页
	
		while(1)											//进入死循环，根据状态切换				
		{		
			if(play_index >= file_num-1)//检查play_index	
				play_index = file_num-1;	//index指向最后一个文件
			else if(play_index<=0)				
				play_index =0;				
			
			even_process();							//处理事件
				
			switch(player_state)
			{
				case S_PLAY:							//播放状态	
												
					//开始play流程						
					//读取音频文件流程
					//打开playlist，读取音频文件名
					fres = f_open (&file, "0:mp3player/playlist.txt", FA_READ);
					fres = f_lseek (&file, play_index*FILE_NAME_LEN);
					fres = f_read(&file, music_name, FILE_NAME_LEN, &rw_num);
					fres = f_close (&file);
			
					//获取文件名，准备解码					
					printf(" ready for:%s ",music_name);
				
					if(strstr(music_name,".mp3")||strstr(music_name,".MP3"))	//MP3格式
					{				
						//开始mp3解码
						mp3_player(music_name);
					}
					else																											//wav格式
					{			
						//开始wav文件播放
						wav_player(music_name);
					}	
					
				break;
						
				case S_SWITCH:				 //切歌状态
						player_state = S_PLAY;																	//更新标志位
						
						/* 检测要切换的歌曲是否在播放列表的上一页 */
						if((play_index+8)/8 < current_page) //play_index从零开始计数所以+1，+7是使用进一法。 +1+7 =+8
						{
							current_page--;										//更新当前页码
							lcd_list(current_page);						//刷新LCD列表	
						}
				
						/* 检测要切换的歌曲是否在播放列表的下一页 */
						if((play_index+8)/8 >current_page)  //play_index从零开始计数所以+1，+7是使用进一法。 +1+7 =+8
						{
							current_page++;										//更新当前页码
							lcd_list(current_page);						//刷新LCD列表
						}
						
					break;				

				
				default:break;

				}
		}

}



/**
  * @brief  file_scan处理非递归过程，
	*					删除旧的playlist（用于存储歌曲路径，用于文件定位）和lcdlist（用于存储歌曲列表，用于显示）
  * @param  path:初始扫描路径
  * @retval none
  */
static void file_scan(char* path)
{ 
	
  fres = f_unlink("0:mp3player/playlist.txt");//删除旧的playlist
	fres = f_unlink("0:mp3player/lcdlist.txt");	//删除旧的playlist
	fres = scan_files(path);				//递归扫描歌曲文件
}

/**
  * @brief  scan_files 递归扫描sd卡内的歌曲文件
  * @param  path:初始扫描路径
  * @retval result:文件系统的返回值
  */
static FRESULT scan_files (char* path) 
{ 
    FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量	
    FILINFO fno; 
    DIR dir; 
    int i; 
    char *fn; 
		char file_name[FILE_NAME_LEN];	
	
#if _USE_LFN 
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1]; 	//长文件名支持
    fno.lfname = lfn; 
    fno.lfsize = sizeof(lfn); 
#endif 

    res = f_opendir(&dir, path); //打开目录
    if (res == FR_OK) 
			{ 
        i = strlen(path); 
        for (;;) 
				{ 
            res = f_readdir(&dir, &fno); 										//读取目录下的内容
            if (res != FR_OK || fno.fname[0] == 0) break; 	//为空时表示所有项目读取完毕，跳出
#if _USE_LFN 
            fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
            fn = fno.fname; 
#endif 
            if (*fn == '.') continue; 											//点表示当前目录，跳过			
            if (fno.fattrib & AM_DIR) 
						{ 																							//目录，递归读取
                sprintf(&path[i], "/%s", fn); 							//合成完整目录名
                res = scan_files(path);											//递归遍历 
                if (res != FR_OK) 
									break; 																		//打开失败，跳出循环
                path[i] = 0; 
            } 
						else 
						{ 
              printf("%s/%s\r\n", path, fn);								//输出文件名

								if(strstr(fn,".mp3")||strstr(fn,".MP3")||strstr(fn,".wav")||strstr(fn,".WAV"))//判断是否mp3或wav文件
								{
									if (strlen(path)+strlen(fn)<FILE_NAME_LEN)
									{
										sprintf(file_name, "%s/%s", path, fn); 
									
										//存储音频文件名到playlist(含路径)
										res = f_open (&file, "0:mp3player/playlist.txt", FA_READ|FA_WRITE|FA_CREATE_ALWAYS );
										res = f_lseek (&file, file_num*FILE_NAME_LEN);  
										res = f_write (&file, file_name, FILE_NAME_LEN, &rw_num);	
										res = f_close (&file);
										
										//存储音频文件名及路径到lcdlist（仅文件名）
										res = f_open (&file, "0:mp3player/lcdlist.txt", FA_READ|FA_WRITE|FA_CREATE_ALWAYS );
										res = f_lseek (&file, file_num*FILE_NAME_LEN);  
										res = f_write (&file, fn, FILE_NAME_LEN, &rw_num);	
										res = f_close (&file);

										
										file_num++;//记录文件个数									
							
									}
								}//if mp3||wav
            }//else
        } //for
    } 

    return res; 
} 




/**
  * @brief  mp3_player 进行mp3文件解码、播放
  * @param  filename:要播放的文件路径
  * @retval none
  */
static void mp3_player(const char *filename)
{
	int err, i, outputSamps, current_sample_rate = 0;	

	int						read_offset = 0;				/* 读偏移指针				*/
	int						bytes_left = 0;					/* 剩余字节数				*/	
	unsigned long	Frames = 0;							/* mP3帧计数				*/
	unsigned char	*read_ptr = buffer;			/* 缓冲区指针				*/
	HMP3Decoder		Mp3Decoder;						  /* mp3解码器指针		*/
	

	
	//打开音频文件
	fres = f_open (&file, filename, FA_READ );
	
	//打开失败
	if (fres!=FR_OK)
	{
		printf("read file %s error  ! open another file\r\n",filename);
		fres = f_close (&file);
		
		if (++play_index>=file_num)	//索引值加1
		{
				play_index=0;						//归0，所以如果所有文件都打开失败会一直循环
		}				
		return ;										//文件无法打开，终止解码。进入下一次循环，读取下一个文件
	}
		
	//打开成功
	//初始化MP3解码器
	Mp3Decoder = MP3InitDecoder();	
	
	//获取输入数据流，调用helix库解码，输出PCM数据，约20ms完成一次循环
	//开始进入播放状态,期间中断会修改touch_even状态
	while(player_state != S_SWITCH)	//循环1， 如果touch_even不是切歌状态则继续呆在循环体里 
	{
		//有时出现解码错误，错误后继续在本循环体内，继续播放
		
		//显示播放图标
		Lcd_GramScan(1);
		LCD_Clear(12,88,8,145,BACKGROUND);
		Lcd_show_bmp(320-(103+((play_index-((current_page-1)*8))*18)),240-20,"/mp3player/ui_playing.bmp");
		
		//读取mp3文件
		fres = f_read(&file, buffer, sizeof(buffer), &rw_num);
		if(fres != FR_OK)
		{
			printf("read %s failed! %d\r\n",filename,fres);
			break;
			//return;
		}
		read_ptr = buffer;									//指向mp3输入流
		bytes_left = rw_num;								//实际读到的输入流大小大小

		//按帧处理	
		while(player_state != S_SWITCH)			//循环2，循环本过程播放音频，直到按了下一首、上一首	
		{
			if (player_state == S_STOP)
			{								
				even_process();									//检查是否有事件需要处理
				continue;												//暂停的时候结束本次循环
			}
				
			player_state = S_PLAY;						//状态更新为正在播放
			
			read_offset = MP3FindSyncWord(read_ptr, bytes_left);	//寻找帧同步，返回第一个同步字的位置
			if(read_offset < 0)																		//没有找到同步字
			{
				break;																							//跳出循环2，回到循环1	
			}
			
			read_ptr += read_offset;					//偏移至同步字的位置
			bytes_left -= read_offset;				//同步字之后的数据大小	
			if(bytes_left < 1024)							//补充数据
			{
				/* 注意这个地方因为采用的是DMA读取，所以一定要4字节对齐  */
				i=(uint32_t)(bytes_left)&3;									//判断多余的字节
				if(i) i=4-i;														//需要补充的字节
				memcpy(buffer+i, read_ptr, bytes_left);	//从对齐位置开始复制
				read_ptr = buffer+i;										//指向数据对齐位置
				fres = f_read(&file, buffer+bytes_left+i, sizeof(buffer)-bytes_left-i, &rw_num);//补充数据
				bytes_left += rw_num;										//有效数据流大小
			}
			err = MP3Decode(Mp3Decoder, &read_ptr, &bytes_left, outBuf[bufflag], 0);					//开始解码 参数：mp3解码结构体、输入流指针、输入流大小、输出流指针、数据格式
			Frames++;			
			
			if (err != ERR_MP3_NONE)									//错误处理
			{
				switch (err)
				{
					case ERR_MP3_INDATA_UNDERFLOW:
						printf("ERR_MP3_INDATA_UNDERFLOW\r\n");
						read_ptr = buffer;
						fres = f_read(&file, read_ptr, sizeof(buffer), &rw_num);
						bytes_left = rw_num;
						break;
			
					case ERR_MP3_MAINDATA_UNDERFLOW:
						/* do nothing - next call to decode will provide more mainData */
						printf("ERR_MP3_MAINDATA_UNDERFLOW\r\n");
						break;
			
					default:
						printf("UNKNOWN ERROR:%d\r\n", err);
			
						// 跳过此帧
						if (bytes_left > 0)
						{
							bytes_left --;
							read_ptr ++;
						}	
						break;
				}
			}
			else		//解码无错误，准备把数据输出到PCM
			{
				MP3GetLastFrameInfo(Mp3Decoder, &Mp3FrameInfo);		//获取解码信息				

		    /* 根据解码信息设置采样率 */
				if (Mp3FrameInfo.samprate != current_sample_rate)	//采样率 
				{
					current_sample_rate = Mp3FrameInfo.samprate;
					printf("Frame info of this song \n");
					printf(" \r\n Bitrate       %dKbps", Mp3FrameInfo.bitrate/1000);
				  printf(" \r\n Samprate      %dHz", current_sample_rate);
					printf(" \r\n BitsPerSample %db", Mp3FrameInfo.bitsPerSample);
					printf(" \r\n nChans        %d", Mp3FrameInfo.nChans);
					printf(" \r\n Layer         %d", Mp3FrameInfo.layer);
					printf(" \r\n Version       %d", Mp3FrameInfo.version);
					printf(" \r\n OutputSamps   %d", Mp3FrameInfo.outputSamps);

					if(current_sample_rate >= I2S_AudioFreq_Default)	//I2S_AudioFreq_Default = 2，正常的帧，每次都要改速率
					{
						I2S_Freq_Config(current_sample_rate);						//根据采样率修改iis速率
					}
				}
				
				/* 输出到DAC */
				outputSamps = Mp3FrameInfo.outputSamps;							//PCM数据个数
				
				if (outputSamps > 0)
				{
					if (Mp3FrameInfo.nChans == 1)	//单声道
					{
						//单声道数据需要复制一份到另一个声道
						for (i = outputSamps - 1; i >= 0; i--)
						{
							outBuf[bufflag][i * 2] = outBuf[bufflag][i];
							outBuf[bufflag][i * 2 + 1] = outBuf[bufflag][i];
						}
						outputSamps *= 2;
					}
				
					//非单声道数据可直接由DMA传输到IIS交给DAC
					/* 等待DMA播放完，这段时间我们可以干其他的事，扫描事件进行处理 */
					while((DMA1_Channel5->CCR&DMA_CCR1_EN) && !(DMA1->ISR&DMA1_IT_TC5))
					{
							even_process();							
						}
						
					/*DMA传输完毕*/
			    DMA_ClearFlag(DMA1_FLAG_TC5 | DMA1_FLAG_TE5);
			    DMA_I2S_Configuration((uint32_t)outBuf[bufflag], outputSamps);
					bufflag = 1 -bufflag;																			//切换buffer

				}//if (outputSamps > 0)
			}//else 解码正常
			
		
		if(file.fptr==file.fsize) 		//如果指针指向了文件尾，表示数据全部读完
		{
			printf("END\r\n");
			if(play_index<file_num-1)		//自动开始下一首歌曲
			{
				play_index++;
				player_state = S_SWITCH;	//进入切歌状态，跳出
			}
			else
			{
				play_index = 0;
				player_state = S_SWITCH;
			}
					
			break; 										//跳出这首歌的播放状态	while break; 
		}
  	
	}//循环2		内 while(player_state != S_SWITCH)	
	
 }//循环1  	外 while(player_state != S_SWITCH)

	f_close(&file);							//结束播放本歌曲，关闭文件
	MP3FreeDecoder(Mp3Decoder);
	I2S_Stop();
	
}


/**
  * @brief  wav_player 进行wav文件播放
	*					wav格式存储的就是PCM数据，不需要解码
  * @param  filename:要播放的文件路径
  * @retval none
  */
static void wav_player(const char *filename)
{
	short *p;
	WavHead *wav;	

	//打开音频文件
	fres = f_open (&file, filename, FA_READ );
	
	//打开失败
	if (fres!=FR_OK)
	{
		printf("read file %s error  ! open another file\r\n",filename);
		fres = f_close (&file);
		
		if (++play_index>=file_num)	//索引值加1
		{
				play_index=0;						//归0，所以如果所有文件都打开失败会一直循环
		}				
		return ;										//文件无法打开，终止解码。进入下一次循环，读取下一个文件
	}
	
		//显示播放图标
	Lcd_GramScan(1);
	LCD_Clear(12,88,8,145,BACKGROUND);
	Lcd_show_bmp(320-(103+((play_index-((current_page-1)*8))*18)),240-20,"/mp3player/ui_playing.bmp");	
	
	
	fres = f_read(&file,buffer,512,&rw_num);							//读取文件头
	
	wav = (WavHead *)buffer;															//整理格式
		
	printf("\r\n samprate: %dHz", wav->dwSamplesPerSec);	//采样率

	if(wav->dwSamplesPerSec >= I2S_AudioFreq_Default)
		I2S_Freq_Config(wav->dwSamplesPerSec);							//设置采样率

		//播放循环	
		while(player_state != S_SWITCH)			//循环本过程播放音频，直到切歌
		{
			if (player_state == S_STOP)
				{			
					even_process();
					continue;											//暂停的时候跳出本循环
				}
				
			player_state = S_PLAY;						//状态更新为正在播放
		
				
			//读取wav文件
			p = (short *)(buffer+sizeof(buffer)/2*bufflag);		
			fres = f_read(&file, p, sizeof(buffer)/2, &rw_num);
			if(fres != FR_OK)
			{
				printf("failed to read %s, %d\r\n",filename,fres);
				break;
			}
			
			/* 等待DMA播放完，这段时间我们可以干其他的事，事件处理 */
			while((DMA1_Channel5->CCR & DMA_CCR1_EN) && !(DMA1->ISR&DMA1_IT_TC5))
			{
					even_process();
			}
        DMA_ClearFlag(DMA1_FLAG_TC5 | DMA1_FLAG_TE5);
        DMA_I2S_Configuration((uint32_t)p, rw_num/2);
				bufflag = 1 -bufflag;				//切换buffer																	
							
		
			if(file.fptr==file.fsize) 		//如果指针指向了文件尾，表示数据全部读完
			{
				printf("END\r\n");
				if(play_index<file_num-1)		//自动开始下一首歌曲
				{
					play_index++;
					player_state = S_SWITCH;	
				}
				else
				{
					play_index = 0;
					player_state = S_SWITCH;
				}
						
				break; 									//跳出这首歌的播放状态	while break; 
			}  	
	}
	
	f_close(&file);								//结束播放本歌曲，关闭文件
	I2S_Stop();
}


/**
  * @brief  lcd_list 显示歌曲列表
  * @param  歌曲列表的页码
  * @retval none
  */
static void lcd_list(char page)
{	
	char i;
	char music_name[MUSIC_NAME_LEN];	
	char lcd_char[MUSIC_NAME_LEN+4];					//+4是为了预留空间的歌曲编号
	FIL name_file;
	
	
	select_index=0;														//重新显示新的一页时重置select_index
	Lcd_GramScan( 1 );												//设置扫描方向
	LCD_Clear(12,86,206,145,BACKGROUND);
	
	
	sprintf(lcd_char,"第 %0d/%0d 页",current_page,all_page);		//显示页码
	LCD_DispEnCh(30,235,(const uint8_t *)lcd_char,BLUE);
	

	sprintf(lcd_char,"音量:%2d",voice);													//显示音量
	LCD_DispEnCh(155,235,(const uint8_t *)lcd_char,BLUE);
	
	
	/* 打开扫描记录到的歌单 */ 
	fres = f_open (&name_file, "0:mp3player/lcdlist.txt", FA_READ);	
	
	/* 打开playlist，读取音频文件名 */
	for(i=0;(i+8*(page-1))<file_num && i< 8;i++)								//仅显示第一页
	{	
		fres = f_lseek (&name_file, (8*(page-1) + i)*FILE_NAME_LEN);			//文件中的歌曲名按FILE_NAME_LEN来偏移
		fres = f_read(&name_file, music_name, MUSIC_NAME_LEN, &rw_num);		//只读取MUSIC_NAME_LEN长度的字符，太长的话LCD不够空间显示
		
		music_name[MUSIC_NAME_LEN-1]='\0';												//把最后一个元素设置为'\0'防止没有结束符号而溢出
		
		sprintf(lcd_char,"%0d.%s",8*(page-1)+i+1,music_name);			//拼接出歌曲列表
		
		LCD_DispEnCh(22,87+i*18,(const uint8_t *)lcd_char,BLUE);										//显示

	}
	
	fres = f_close (&name_file);
}


/**
  * @brief  even_process 根据事件标志进行处理
  * @param  none
  * @retval none
  */
static void even_process(void)
{
	switch(touch_even)
	{
		/* 音量加				 */
		case E_UP:														
			
			Volume_Add();		
			
			touch_even = E_NULL;
		
			break;
		
		/* 音量减 			*/
		case E_DOWN:													
			
			Volume_Dec();	
			
			touch_even = E_NULL;	
		
			break;
		
		/* 播放、暂停键	*/
		case E_PLAY_STOP:												
		
			if(player_state == S_PLAY)					//若当前状态为播放
			{
				player_state = S_STOP;						//切换为暂停状态
			}		
			else																//若当前状态为暂停
				player_state =S_PLAY;							//切换为播放暂停
				
			touch_even = E_NULL;
			break;	

		/* 上一首			*/	
		case E_PREV:														
			if(play_index <= 0)
			{
				play_index = 0;
			}
			else
			{
				play_index--;
				#if 0																//放在player_run 函数里统一处理了	
				/* 检测要切换的歌曲是否在播放列表的上一页 */
				if((play_index+8)/8 < current_page) //play_index从零开始计数所以+1，+7是使用进一法。 +1+7 =+8
				{
					current_page--;										//更新当前页码
					lcd_list(current_page);						//刷新LCD列表	
				}
				#endif

			}
			
			touch_even = E_NULL;						
			player_state = S_SWITCH;							//进入切歌状态
			
			break;
		
		/* 下一首	 	 */			
		case E_NEXT:														
			
			if(play_index < file_num-1)
			{
				play_index++;	
				#if 0															//放在player_run 函数里统一处理了
				/* 检测要切换的歌曲是否在播放列表的下一页 */
				if((play_index+8)/8 >current_page)  //play_index从零开始计数所以+1，+7是使用进一法。 +1+7 =+8
				{
					current_page++;										//更新当前页码
					lcd_list(current_page);						//刷新LCD列表
				}
				#endif
			}									
			else
			{
				play_index = file_num-1;						//play_index设置为最后一个歌曲文件
			}		
				touch_even = E_NULL;	
				player_state = S_SWITCH;						//进入切歌状态
		
			break;
		
		/* 向上			*/
		case E_SELT_UP:													
			if(select_index>0)
				select_index--;
			
			Lcd_GramScan(1);
			LCD_Clear(12,88,8,145,BACKGROUND);		//清除状态栏
			Lcd_show_bmp(320-(103+(select_index*18)),240-20,"/mp3player/ui_select.bmp");			//显示“打勾”标签
		
			touch_even = E_NULL;
			
			break;
		
		/* 向下 		*/ 
		case E_SELT_DOWN:												
			if(select_index<8-1 && (select_index+8*(current_page-1))<file_num-1)	//判断是否溢出，不大于8，不指向无文件名的位置
				select_index++;			
			
			Lcd_GramScan(1);
			LCD_Clear(12,88,8,145,BACKGROUND);	//清除状态栏
			Lcd_show_bmp(320-(103+(select_index*18)),240-20,"/mp3player/ui_select.bmp");			//显示“打勾”标签
		
			touch_even = E_NULL;	
					
			break;
		
		/* 直接点选歌曲	*/
		case E_SELECT:												
			
			play_index = select_index + ((current_page-1)*8);											//根据当前页和select_index确定play_index
			player_state = S_SWITCH;
		
			touch_even = E_NULL;	
		
			break;			
		
		/* 下一页				*/
		case E_SELT_NEXT:												
			
			if(current_page<all_page)
			{
				current_page++;										//更新当前页码
				lcd_list(current_page);						//刷新LCD列表
			}
			else
				current_page =1;
		
			touch_even = E_NULL;
			
			break;
		
		/* 上一页			*/
		case E_SELT_PREV:											
			if(current_page>1)					
			{
				current_page--;										//更新当前页码
				lcd_list(current_page);						//刷新LCD列表
			}
			else
				current_page =all_page;
			
			touch_even = E_NULL;
			break;
		
		/* OK					*/
		case E_SELT_OK:												
			play_index = select_index+8*(current_page-1);	//根据当前页和select_index确定play_index
			
			touch_even = E_NULL;	
			player_state = S_SWITCH;
			
			break;
		
		/* 扬声器 		*/
		case E_LOUD_SPEAK:										
			if(speaker_flag == 0)								//speaker关状态
			{
				Loud_Speaker_ON();								//开启speaker	
				speaker_flag =1;
				printf("\r\n loud on");
			}
			else																//speaker为开状态
			{
				Loud_Speaker_OFF();								//关闭speaker
				speaker_flag=0;
				printf("\r\n loud off");
			}
			
			touch_even = E_NULL;
			break;
		
		default:
			//touch_even = E_NULL;
			break;
	}

}

/**
  * @brief  touch_process 触摸事件处理，在触摸中断时被调用
  * @param  none
  * @retval none
  */
void touch_process (void)
{
	
	  if(touch_flag == 1)			/*如果触笔按下了*/
    {
		
      /* 获取点的坐标 */
      if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)      
      {		
        LED1_TOGGLE;   																//LED反转	    
				
				//MP3控制条的x范围，再根据y坐标区分按键
				if(display.x>=13 && display.x<=31)							
				{
						if(display.y >=177 &&display.y<=194)			//音量 +
						{
							touch_even = E_UP;
						}
						else if(display.y >=199 &&display.y<=214)	//音量 -
						{
							touch_even = E_DOWN;
						}
						else if(display.y >=98 &&display.y<=131)	//播放暂停
						{
							touch_even = E_PLAY_STOP;
							printf("\r\n PAUSE");
						}
						else if(display.y >=65 &&display.y<=93)		//上一首
						{
							touch_even = E_PREV;									
							printf("\r\n PREV");
						}
						else if(display.y >=139 &&display.y<=167)	//下一首
						{										
							touch_even = E_NEXT;	
							printf("\r\n NEXT");
						}
						else if(display.y >=43 &&display.y<=57)		//下一页
						{
								touch_even = E_SELT_NEXT;
								printf("\r\n NEXT	PAGE");
						}
						else if(display.y >=24 &&display.y<=43)		//OK键
						{
							touch_even = E_SELT_OK;
							printf("\r\n OK");
						}
						else if(display.y >=10 &&display.y<=24)		//上一页
						{
							touch_even = E_SELT_PREV;					
							printf("\r\n PREV PAGE");
						}
							
				}
				/* 向上和向下的控制按钮y范围	*/		
				else if(display.y >=24 && display.y<=43)
				{
						if(display.x>=31 && display.x <=43)				//向上
						{
								touch_even = E_SELT_UP;
								printf("\r\n UP");
							}
						else if(display.x<=13 && display.x>0)		//向下
						{
								touch_even = E_SELT_DOWN;
								printf("\r\n DOWN ");
							}
				}		
				
				else if(display.x>260 && display.x< 300 &&display.y>47 && display.y<107)	//扬声器
				{
					touch_even = E_LOUD_SPEAK;
					printf("\r\n SPEAKER");
				}		
					
				//直接选择歌曲，第1.2.3.4.。。。8首~
					
				else if(display.x>208 && display.x<226)
				{
					touch_even = E_SELECT;	
					select_index = 0;
					printf("\r\n play_index=%d",select_index);
				}
				else if(display.x>190 && display.x<208)
				{
					touch_even = E_SELECT;	
					select_index = 1;
					printf("\r\n select_index=%d",select_index);
				}
				else if(display.x>172 && display.x<190)
				{
					touch_even = E_SELECT;	
					select_index = 2;
					printf("\r\n select_index=%d",select_index);
				}
				else if(display.x>154 && display.x<172)
				{
					touch_even = E_SELECT;	
					select_index = 3;
					printf("\r\n select_index=%d",select_index);
				}
				else if(display.x>136 && display.x<154)
				{
					touch_even = E_SELECT;
					select_index = 4;
					printf("\r\n select_index=%d",select_index);
				}
				else if(display.x>118 && display.x<136)
				{
					touch_even = E_SELECT;	
					select_index = 5;
					printf("\r\n select_index=%d",select_index);
				}
				else if(display.x>100 && display.x<118)
				{
					touch_even = E_SELECT;	
					select_index = 6;
					printf("\r\n select_index=%d",select_index);
				}
				else if(display.x>82 && display.x<100)
				{
					touch_even = E_SELECT;	
					select_index = 7;
					printf("\r\n select_index=%d",select_index);
				}
				else if(display.x>64 && display.x<82)
				{
					touch_even = E_SELECT;	
					select_index = 8;
					printf("\r\n select_index=%d",select_index);
				}						
						
      }//if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)      
			
    }// if(touch_flag == 1)			
	
}




