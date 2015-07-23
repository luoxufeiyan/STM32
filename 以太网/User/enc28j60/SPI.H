#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x.h"

#define  ENC28J60_CSL()    (GPIOB->ODR &= ~(1<<12))
#define  ENC28J60_CSH()    (GPIOB->ODR |= 1<<12)

#define   ENC28J60_RSTL()  // (GPIOE->ODR &= ~(1<<1))
#define   ENC28J60_RSTH()  // (GPIOE->ODR |= 1<<1)

#define  ENC_SPI_ReadWrite  SPI2_ReadWrite
#define  ENC_SPI_Init   SPI2_Init

void SPI2_Init(void);
uint8_t SPI2_ReadWrite(uint8_t writedat);
   

#endif
