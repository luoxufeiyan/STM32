
#include "SPI.h"

/*
 * 函数名：SPI2_Init
 * 描述  ：初始化SPI1端口及基模式
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void SPI2_Init(void)
{
   SPI_InitTypeDef SPI_InitStructure;
   GPIO_InitTypeDef GPIO_InitStructure;

   /* Enable SPI2 and GPIOB clocks */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); //注意挂载在APB1总线上,说明只能插API2
   //RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
   
  /* 片选 */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   // 推免输出
   GPIO_Init(GPIOB, &GPIO_InitStructure); 

   /* Configure SPI2 pins: NSS, SCK, MISO and MOSI */
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		   // 复用输出
	 GPIO_Init(GPIOB, &GPIO_InitStructure);
  
   /* PC6:ENC28J60_INT */						// 中断引脚没用到

   /* PC7:ENC28J60_RST*/				    // 复位引脚没用到

   /* SPI2 configuration */ 
   SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
   SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
   SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
   SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
   SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
   SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
   SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
   SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
   SPI_InitStructure.SPI_CRCPolynomial = 7;
   SPI_Init(SPI2, &SPI_InitStructure);

   /* Enable SPI2  */
   SPI_Cmd(SPI2, ENABLE);
}

/*
 * 函数名：SPI2_ReadWrite
 * 描述  ：最底层的使用SPI1收发一节字数据
 * 输入  ：要写入的数据
 * 输出  : 接收到的数据
 * 调用  ：外部调用
 */
uint8_t SPI2_ReadWrite(uint8_t dat)
{
   /* Loop while DR register in not emplty */
   while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

   /* Send byte through the SPI2 peripheral */
   SPI_I2S_SendData(SPI2, dat);

   /* Wait to receive a byte */
   while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

   /* Return the byte read from the SPI bus */
   return SPI_I2S_ReceiveData(SPI2);
}


