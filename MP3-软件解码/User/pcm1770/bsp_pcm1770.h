#ifndef __PCM1770_H
#define __PCM1770_H

#include "stm32f10x.h"


#define PCM_PD               (1 <<  7)									//PC7	  
#define PCM_PD_SET_L         GPIOC->ODR&=~(PCM_PD)     	//GPIOB->ODR = (GPIOB->ODR & ~PCM_PD) | (x ? PCM_PD : 0);   
#define PCM_PD_SET_H		     GPIOC->ODR|=(PCM_PD)

#define PCM_CS               (1 <<  14)									//PB14
#define PCM_CS_SET_L         GPIOB->ODR&=~(PCM_CS)      //GPIOB->ODR = (GPIOB->ODR & ~PCM_CS) | (x ? PCM_CS : 0);   
#define PCM_CS_SET_H         GPIOB->ODR|=(PCM_CS)

	
#define PCM_CLK              (1 <<  8)									//PB8
#define PCM_CLK_SET_L        GPIOB->ODR&=~(PCM_CLK)    	//     GPIOB->ODR = (GPIOB->ODR & ~PCM_CLK) | (x ? PCM_CLK : 0);   
#define PCM_CLK_SET_H		     GPIOB->ODR|=(PCM_CLK)

#define PCM_DAT              (1 <<  9)									//PB9
#define PCM_DAT_SET_L        GPIOB->ODR&=~(PCM_DAT)    	//GPIOB->ODR = (GPIOB->ODR & ~PCM_DAT) | (x ? PCM_DAT : 0);   
#define PCM_DAT_SET_H		     GPIOB->ODR|=(PCM_DAT)


#define C_VOLUME_MAX	0x3F			//◊Ó¥Û“Ù¡ø	


void PCM1770Init(void);
void PCM_WriteData(const uint8_t Reg, const uint8_t Data);
void Volume_Dec(void);
void Volume_Add(void);
void PCM1770_Mute(void);
void PCM1770_VolumeSet(uint8_t vol);
void Loud_Speaker_ON(void);
void Loud_Speaker_OFF(void);



#endif /* __PCM1770_H */


/******************* end of file****************************/
