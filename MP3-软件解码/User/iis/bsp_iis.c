#include "stm32f10x.h"
#include "bsp_iis.h"

static void I2S_GPIO_Config(void);
static void I2S_Mode_Config(void);


/**
  * @brief  I2S_Bus_Init 初始化iis总线
  * @param  none
  * @retval none
  */
void I2S_Bus_Init(void)
{
	
	/* 配置I2S GPIO用到的引脚 */
	I2S_GPIO_Config(); 

	/* 配置I2S工作模式 */
	I2S_Mode_Config();

	I2S_Cmd(SPI2, DISABLE);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}


/**
  * @brief  I2S_Stop 停止iis总线
  * @param  none
  * @retval none
  */
void I2S_Stop(void)
{		
	/* 禁能 SPI2/I2S2 外设 */
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, DISABLE);
	I2S_Cmd(SPI2, DISABLE);	

	DMA_Cmd(DMA1_Channel5, DISABLE);

}


/**
  * @brief  I2S_Freq_Config 根据采样频率配置iis总线，在播放音频文件时可从文件中解码获取
  * @param  SampleFreq 采样频率
  * @retval none
  */
void I2S_Freq_Config(uint16_t SampleFreq)
{
	I2S_InitTypeDef I2S_InitStructure;

	I2S_Stop();
	I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;						// 配置I2S工作模式
	I2S_InitStructure.I2S_Standard = I2S_Standard_LSB;				// 接口标准 
	I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;		// 数据格式，16bit 
	I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;	// 主时钟模式 
	I2S_InitStructure.I2S_AudioFreq = SampleFreq;							// 音频采样频率 
	I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;  			
	I2S_Init(SPI2, &I2S_InitStructure);
	
	I2S_Cmd(SPI2, ENABLE);																		//使能iis总线
}


/**
  * @brief  DMA_I2S_Configuration 配置DMA总线
  * @param  addr:数据源地址
	*	@param	size:要传输的数据大小
  * @retval none
  */
void DMA_I2S_Configuration(uint32_t addr, uint32_t size)
{
    DMA_InitTypeDef DMA_InitStructure;				  

    /* DMA 通道配置*/
    DMA_Cmd(DMA1_Channel5, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SPI2->DR));								//目的地址，iis的数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) addr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);															//使能DMA请求

    DMA_Cmd(DMA1_Channel5, ENABLE);
}

/**
  * @brief  I2S_GPIO_Config 配置IIS总线用到的GPIO引脚
  * @param  none
  * @retval none
  */
static void I2S_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable GPIOB, GPIOC */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB , ENABLE);
	
	/* 打开 I2S2 APB1 时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	/* I2S2 SD, CK  WS 配置 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* I2S2 MCK  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
  * @brief  I2S_Mode_Config 配置IIS总线的工作模式(默认采样频率)
  * @param  none
  * @retval none
  */
static void I2S_Mode_Config(void)
{
	I2S_InitTypeDef I2S_InitStructure; 
			
	/* I2S2 外设配置 */
	I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;							// 配置I2S工作模式 
	I2S_InitStructure.I2S_Standard = I2S_Standard_LSB;					// 接口标准 
	I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;			// 数据格式，16bit
	I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;		// 主时钟模式 
	I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_44k;				// 音频采样频率
	I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;  								// 默认为低	
	I2S_Init(SPI2, &I2S_InitStructure);

}

