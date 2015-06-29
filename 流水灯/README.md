流水灯
=====

1,获知GPIO的地址映射，知道它挂载的总线APB2.
2,了解ST官方库对寄存器的封装
3,了解时钟树，知道GPIOC对应的时钟来源PCLK2
4,添加用到的头文件
5,添加对应的bsp_led.c和bsp_led.h用户文件
6,考虑初始化函数LED_GPIO_Config()
7,开启外设时钟，分析由SystemInit()配置的Sysclk=72MHz的时钟频率是否符合要求
8,初始化结构体GPIO_InitStructure()
9,用GPIO_Init()初始化GPIOC
10,编写对应的bsp_led.c和bsp_led.h用户文件
11,编写main程序
12,编译调试