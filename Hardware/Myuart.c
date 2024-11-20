#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "Myuart.h"


//USART2_MAX_TX_LEN和USART2_MAX_RX_LEN在头文件进行了宏定义，分别指USART2最大发送长度和最大接收长度
u8 USART2_TX_BUF[USART2_MAX_TX_LEN]; 	//发送缓冲,最大USART2_MAX_TX_LEN字节
u8 u1rxbuf[USART2_MAX_RX_LEN];				//发送数据缓冲区1
u8 u2rxbuf[USART2_MAX_RX_LEN];				//发送数据缓冲区2
u8 witchbuf=0;                  			//标记当前使用的是哪个缓冲区,0：使用u1rxbuf；1：使用u2rxbuf
u8 USART2_TX_FLAG=0;									//USART2发送标志，启动发送时置1
u8 USART2_RX_FLAG=0;									//USART2接收标志，启动接收时置1
extern u8* ptr;

/**
  * 函    数：串口初始化
  * 参    数：无
  * 返 回 值：无
  */
void Serial_Init(unsigned long baudrate)
{
	/*开启时钟*/
	//GPIO端口设置
 	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);		//使能USART2，GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//USART2_TX   GPIOA.2初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;																				//PA.2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;																	//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																//GPIO速率50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);																					//初始化GPIOA.2
	
	//USART2_RX	  GPIOA.3初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;																				//PA.3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;														//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);																					//初始化GPIOA.3
	 
	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = baudrate;																	//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;											//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;													//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No ;														//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;									//收发模式
	USART_Init(USART2, &USART_InitStructure); 																			//初始化串口2
	
	//中断开启设置
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);																	//开启检测串口空闲状态中断
	USART_ClearFlag(USART2,USART_FLAG_TC);																					//清除USART2标志位
		
	USART_Cmd(USART2, ENABLE);																											//使能串口2
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;																//NVIC通道设置
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;												//抢占优先级8
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;															//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;																	//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);																									//根据指定的参数初始化NVIC寄存器
	
	DMA1_USART2_Init();																			
}


void DMA1_USART2_Init(void)
{
	DMA_InitTypeDef DMA1_Init;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);								//使能DMA1时钟

	//DMA_USART2_RX  USART2->RAM的数据传输
	DMA_DeInit(DMA1_Channel6);																			//将DMA的通道6寄存器重设为缺省值 
	DMA1_Init.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);					//启动传输前装入实际RAM地址
	DMA1_Init.DMA_MemoryBaseAddr = (u32)u1rxbuf;            				//设置接收缓冲区首地址
	DMA1_Init.DMA_DIR = DMA_DIR_PeripheralSRC;											//数据传输方向，从外设读取到内存
	DMA1_Init.DMA_BufferSize = USART2_MAX_RX_LEN;										//DMA通道的DMA缓存的大小
	DMA1_Init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//外设地址寄存器不变
	DMA1_Init.DMA_MemoryInc = DMA_MemoryInc_Enable;									//内存地址寄存器递增
	DMA1_Init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//数据宽度为8位
	DMA1_Init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					//数据宽度为8位
	DMA1_Init.DMA_Mode = DMA_Mode_Normal;														//工作在正常模式
	DMA1_Init.DMA_Priority = DMA_Priority_High; 										//DMA通道 x拥有高优先级 
	DMA1_Init.DMA_M2M = DMA_M2M_Disable;														//DMA通道x没有设置为内存到内存传输
	 
	DMA_Init(DMA1_Channel6,&DMA1_Init); 														//对DMA通道6进行初始化
	
	//DMA_USART2_TX  RAM->USART2的数据传输
	DMA_DeInit(DMA1_Channel7);																			//将DMA的通道7寄存器重设为缺省值 
	DMA1_Init.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);					//启动传输前装入实际RAM地址
	DMA1_Init.DMA_MemoryBaseAddr = (u32)USART2_TX_BUF;              //设置发送缓冲区首地址
	DMA1_Init.DMA_DIR = DMA_DIR_PeripheralDST; 											//数据传输方向，从内存发送到外设
	DMA1_Init.DMA_BufferSize = USART2_MAX_TX_LEN;										//DMA通道的DMA缓存的大小
	DMA1_Init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//外设地址寄存器不变
	DMA1_Init.DMA_MemoryInc = DMA_MemoryInc_Enable;									//内存地址寄存器递增
	DMA1_Init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//数据宽度为8位
	DMA1_Init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					//数据宽度为8位
	DMA1_Init.DMA_Mode = DMA_Mode_Normal;														//工作在正常模式
	DMA1_Init.DMA_Priority = DMA_Priority_High; 										//DMA通道 x拥有高优先级 
	DMA1_Init.DMA_M2M = DMA_M2M_Disable;														//DMA通道x没有设置为内存到内存传输

	DMA_Init(DMA1_Channel7,&DMA1_Init); 														//对DMA通道7进行初始化
	
	//DMA1通道6 NVIC 配置
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;				//NVIC通道设置
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;			//抢占优先级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;							//子优先级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);																	//根据指定的参数初始化NVIC寄存器
 
	//DMA1通道7 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;				//NVIC通道设置
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;			//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;							//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);																	//根据指定的参数初始化NVIC寄存器

	DMA_ITConfig(DMA1_Channel6,DMA_IT_TC,ENABLE);										//开USART2 Rx DMA中断
	DMA_ITConfig(DMA1_Channel7,DMA_IT_TC,ENABLE);										//开USART2 Tx DMA中断

	DMA_Cmd(DMA1_Channel6,ENABLE);           												//使DMA通道6停止工作
	DMA_Cmd(DMA1_Channel7,DISABLE);           											//使DMA通道7停止工作
	 
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);        					//开启串口DMA发送
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);        					//开启串口DMA接收
}
/**
  * 函    数：串口发送数组
  * 
  * 返 回 值：无
  */
void DMA_USART2_Tx_Data(u8 *buffer, u32 size)
{
	while(USART2_TX_FLAG);										//等待上一次发送完成（USART2_TX_FLAG为1即还在发送数据）
	USART2_TX_FLAG=1;													//USART2发送标志（启动发送）
	DMA1_Channel7->CMAR  = (uint32_t)buffer;	//设置要发送的数据地址
	DMA1_Channel7->CNDTR = size;    					//设置要发送的字节数目
	DMA_Cmd(DMA1_Channel7, ENABLE);						//开始DMA发送
}

void Serial_SendByte(uint8_t Byte)
{
	while(USART2_TX_FLAG);										//等待上一次发送完成（USART2_TX_FLAG为1即还在发送数据）
	USART2_TX_FLAG=1;													//USART2发送标志（启动发送）
	DMA1_Channel7->CMAR  = (uint32_t)(&Byte);	//设置要发送的数据地址
	DMA1_Channel7->CNDTR = 1;    					//设置要发送的字节数目
	DMA_Cmd(DMA1_Channel7, ENABLE);						//开始DMA发送
}

/**
  * 函    数：串口发送一个字符串
  * 参    数：String 要发送字符串的首地址
  * 返 回 值：无
  */
void Serial_SendString(char *String,u32 size)
{
	while(USART2_TX_FLAG);										//等待上一次发送完成（USART2_TX_FLAG为1即还在发送数据）
	USART2_TX_FLAG=1;													//USART2发送标志（启动发送）
	DMA1_Channel7->CMAR  = (uint32_t)String;	//设置要发送的数据地址
	DMA1_Channel7->CNDTR = size;    					//设置要发送的字节数目
	DMA_Cmd(DMA1_Channel7, ENABLE);						//开始DMA发送
}


/**
	定长数据包
  */
//void DMA1_Channel6_IRQHandler(void)
//{
//	u8 *p;
//	if(DMA_GetITStatus(DMA1_IT_TC6)!= RESET)		//DMA接收完成标志
//	{
//		DMA_ClearITPendingBit(DMA1_IT_TC6); 			//清除中断标志 
//		USART_ClearFlag(USART2,USART_FLAG_TC);		//清除USART2标志位
//		DMA_Cmd(DMA1_Channel6, DISABLE );   			//关闭USART2 TX DMA1 所指示的通道
//		if(witchbuf)                        			//之前用的u2rxbuf，切换为u1rxbuf
//		{
//			p=u2rxbuf;															//先保存前一次数据地址再切换缓冲区
//			DMA1_Channel6->CMAR=(u32)u1rxbuf;				//切换为u1rxbuf缓冲区地址
//			witchbuf=0;                     				//下一次切换为u2rxbuf
//		}else                               			//之前用的u1rxbuf，切换为u2rxbuf
//		{
//			p=u1rxbuf;															//先保存前一次数据地址再切换缓冲区
//			DMA1_Channel6->CMAR=(u32)u2rxbuf;				//切换为u2rxbuf缓冲区地址
//			witchbuf=1;                     				//下一次切换为u1rxbuf
//		}
//		DMA1_Channel6->CNDTR = USART2_MAX_RX_LEN;	//DMA通道的DMA缓存的大小
//		DMA_Cmd(DMA1_Channel6, ENABLE);     			//使能USART2 TX DMA1 所指示的通道
//		
//		//******************↓↓↓↓↓这里作数据处理↓↓↓↓↓******************//
//		
//		DMA_USART2_Tx_Data(p,USART2_MAX_RX_LEN);
//		
//		//******************↑↑↑↑↑这里作数据处理↑↑↑↑↑******************//
//		
//	}
//}

//DMA1通道7中断
void DMA1_Channel7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC7)!= RESET)	//DMA接收完成标志
	{
		DMA_ClearITPendingBit(DMA1_IT_TC7); 		//清除中断标志 
		USART_ClearFlag(USART2,USART_FLAG_TC);	//清除串口2的标志位
		DMA_Cmd(DMA1_Channel7, DISABLE );   		//关闭USART2 TX DMA1 所指示的通道
		USART2_TX_FLAG=0;												//USART2发送标志(关闭)
	}
}

//串口2中断函数
void USART2_IRQHandler(void)                	
{
	u8 *p;
	u8 USART2_RX_LEN = 0;																				//接收数据长度
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)				//串口2空闲中断
	{
		USART_ReceiveData(USART2); 																//清除串口2空闲中断IDLE标志位
		USART_ClearFlag(USART2,USART_FLAG_TC);										//清除USART2标志位
		DMA_Cmd(DMA1_Channel6, DISABLE );   											//关闭USART2 TX DMA1 所指示的通道
		USART2_RX_LEN = USART2_MAX_RX_LEN - DMA1_Channel6->CNDTR;	//获得接收到的字节数
		if(witchbuf)                        											//之前用的u2rxbuf，切换为u1rxbuf
		{
			p=u2rxbuf;																							//先保存前一次数据地址再切换缓冲区
			DMA1_Channel6->CMAR=(u32)u1rxbuf;												//切换为u1rxbuf缓冲区地址
			witchbuf=0;                     												//下一次切换为u2rxbuf
		}else                               											//之前用的u1rxbuf，切换为u2rxbuf
		{
			p=u1rxbuf;																							//先保存前一次数据地址再切换缓冲区
			DMA1_Channel6->CMAR=(u32)u2rxbuf;												//切换为u2rxbuf缓冲区地址
			witchbuf=1;                     												//下一次切换为u1rxbuf
		}
		DMA1_Channel6->CNDTR = USART2_MAX_RX_LEN;									//DMA通道的DMA缓存的大小
		DMA_Cmd(DMA1_Channel6, ENABLE);     											//使能USART2 TX DMA1 所指示的通道
		
		//******************↓↓↓↓↓这里作数据处理↓↓↓↓↓******************//
		
		DMA_USART2_Tx_Data(p,USART2_RX_LEN);
		ptr=p;
		
		//******************↑↑↑↑↑这里作数据处理↑↑↑↑↑******************//
		
  }
	USART_ClearITPendingBit(USART2,USART_IT_ORE);								//清除USART2_ORE标志位
}
