#ifndef _BSP_IIS_H
#define _BSP_IIS_H


#include "stm32f10x.h"


void I2S_Bus_Init(void);
void I2S_Stop(void);
void I2S_Freq_Config(uint16_t SampleFreq);
void DMA_I2S_Configuration(uint32_t addr, uint32_t size);

#endif
