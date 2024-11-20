#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "Myuart.h"


//USART2_MAX_TX_LEN��USART2_MAX_RX_LEN��ͷ�ļ������˺궨�壬�ֱ�ָUSART2����ͳ��Ⱥ������ճ���
u8 USART2_TX_BUF[USART2_MAX_TX_LEN]; 	//���ͻ���,���USART2_MAX_TX_LEN�ֽ�
u8 u1rxbuf[USART2_MAX_RX_LEN];				//�������ݻ�����1
u8 u2rxbuf[USART2_MAX_RX_LEN];				//�������ݻ�����2
u8 witchbuf=0;                  			//��ǵ�ǰʹ�õ����ĸ�������,0��ʹ��u1rxbuf��1��ʹ��u2rxbuf
u8 USART2_TX_FLAG=0;									//USART2���ͱ�־����������ʱ��1
u8 USART2_RX_FLAG=0;									//USART2���ձ�־����������ʱ��1
extern u8* ptr;

/**
  * ��    �������ڳ�ʼ��
  * ��    ������
  * �� �� ֵ����
  */
void Serial_Init(unsigned long baudrate)
{
	/*����ʱ��*/
	//GPIO�˿�����
 	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);		//ʹ��USART2��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//USART2_TX   GPIOA.2��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;																				//PA.2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;																	//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																//GPIO����50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);																					//��ʼ��GPIOA.2
	
	//USART2_RX	  GPIOA.3��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;																				//PA.3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;														//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);																					//��ʼ��GPIOA.3
	 
	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = baudrate;																	//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;											//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;													//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No ;														//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;									//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); 																			//��ʼ������2
	
	//�жϿ�������
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);																	//������⴮�ڿ���״̬�ж�
	USART_ClearFlag(USART2,USART_FLAG_TC);																					//���USART2��־λ
		
	USART_Cmd(USART2, ENABLE);																											//ʹ�ܴ���2
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;																//NVICͨ������
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;												//��ռ���ȼ�8
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;															//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;																	//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);																									//����ָ���Ĳ�����ʼ��NVIC�Ĵ���
	
	DMA1_USART2_Init();																			
}


void DMA1_USART2_Init(void)
{
	DMA_InitTypeDef DMA1_Init;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);								//ʹ��DMA1ʱ��

	//DMA_USART2_RX  USART2->RAM�����ݴ���
	DMA_DeInit(DMA1_Channel6);																			//��DMA��ͨ��6�Ĵ�������Ϊȱʡֵ 
	DMA1_Init.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);					//��������ǰװ��ʵ��RAM��ַ
	DMA1_Init.DMA_MemoryBaseAddr = (u32)u1rxbuf;            				//���ý��ջ������׵�ַ
	DMA1_Init.DMA_DIR = DMA_DIR_PeripheralSRC;											//���ݴ��䷽�򣬴������ȡ���ڴ�
	DMA1_Init.DMA_BufferSize = USART2_MAX_RX_LEN;										//DMAͨ����DMA����Ĵ�С
	DMA1_Init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//�����ַ�Ĵ�������
	DMA1_Init.DMA_MemoryInc = DMA_MemoryInc_Enable;									//�ڴ��ַ�Ĵ�������
	DMA1_Init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//���ݿ��Ϊ8λ
	DMA1_Init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					//���ݿ��Ϊ8λ
	DMA1_Init.DMA_Mode = DMA_Mode_Normal;														//����������ģʽ
	DMA1_Init.DMA_Priority = DMA_Priority_High; 										//DMAͨ�� xӵ�и����ȼ� 
	DMA1_Init.DMA_M2M = DMA_M2M_Disable;														//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	 
	DMA_Init(DMA1_Channel6,&DMA1_Init); 														//��DMAͨ��6���г�ʼ��
	
	//DMA_USART2_TX  RAM->USART2�����ݴ���
	DMA_DeInit(DMA1_Channel7);																			//��DMA��ͨ��7�Ĵ�������Ϊȱʡֵ 
	DMA1_Init.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);					//��������ǰװ��ʵ��RAM��ַ
	DMA1_Init.DMA_MemoryBaseAddr = (u32)USART2_TX_BUF;              //���÷��ͻ������׵�ַ
	DMA1_Init.DMA_DIR = DMA_DIR_PeripheralDST; 											//���ݴ��䷽�򣬴��ڴ淢�͵�����
	DMA1_Init.DMA_BufferSize = USART2_MAX_TX_LEN;										//DMAͨ����DMA����Ĵ�С
	DMA1_Init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//�����ַ�Ĵ�������
	DMA1_Init.DMA_MemoryInc = DMA_MemoryInc_Enable;									//�ڴ��ַ�Ĵ�������
	DMA1_Init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//���ݿ��Ϊ8λ
	DMA1_Init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					//���ݿ��Ϊ8λ
	DMA1_Init.DMA_Mode = DMA_Mode_Normal;														//����������ģʽ
	DMA1_Init.DMA_Priority = DMA_Priority_High; 										//DMAͨ�� xӵ�и����ȼ� 
	DMA1_Init.DMA_M2M = DMA_M2M_Disable;														//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��

	DMA_Init(DMA1_Channel7,&DMA1_Init); 														//��DMAͨ��7���г�ʼ��
	
	//DMA1ͨ��6 NVIC ����
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;				//NVICͨ������
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;			//��ռ���ȼ�
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;							//�����ȼ�
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//IRQͨ��ʹ��
//	NVIC_Init(&NVIC_InitStructure);																	//����ָ���Ĳ�����ʼ��NVIC�Ĵ���
 
	//DMA1ͨ��7 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;				//NVICͨ������
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;			//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;							//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);																	//����ָ���Ĳ�����ʼ��NVIC�Ĵ���

	DMA_ITConfig(DMA1_Channel6,DMA_IT_TC,ENABLE);										//��USART2 Rx DMA�ж�
	DMA_ITConfig(DMA1_Channel7,DMA_IT_TC,ENABLE);										//��USART2 Tx DMA�ж�

	DMA_Cmd(DMA1_Channel6,ENABLE);           												//ʹDMAͨ��6ֹͣ����
	DMA_Cmd(DMA1_Channel7,DISABLE);           											//ʹDMAͨ��7ֹͣ����
	 
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);        					//��������DMA����
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);        					//��������DMA����
}
/**
  * ��    �������ڷ�������
  * 
  * �� �� ֵ����
  */
void DMA_USART2_Tx_Data(u8 *buffer, u32 size)
{
	while(USART2_TX_FLAG);										//�ȴ���һ�η�����ɣ�USART2_TX_FLAGΪ1�����ڷ������ݣ�
	USART2_TX_FLAG=1;													//USART2���ͱ�־���������ͣ�
	DMA1_Channel7->CMAR  = (uint32_t)buffer;	//����Ҫ���͵����ݵ�ַ
	DMA1_Channel7->CNDTR = size;    					//����Ҫ���͵��ֽ���Ŀ
	DMA_Cmd(DMA1_Channel7, ENABLE);						//��ʼDMA����
}

void Serial_SendByte(uint8_t Byte)
{
	while(USART2_TX_FLAG);										//�ȴ���һ�η�����ɣ�USART2_TX_FLAGΪ1�����ڷ������ݣ�
	USART2_TX_FLAG=1;													//USART2���ͱ�־���������ͣ�
	DMA1_Channel7->CMAR  = (uint32_t)(&Byte);	//����Ҫ���͵����ݵ�ַ
	DMA1_Channel7->CNDTR = 1;    					//����Ҫ���͵��ֽ���Ŀ
	DMA_Cmd(DMA1_Channel7, ENABLE);						//��ʼDMA����
}

/**
  * ��    �������ڷ���һ���ַ���
  * ��    ����String Ҫ�����ַ������׵�ַ
  * �� �� ֵ����
  */
void Serial_SendString(char *String,u32 size)
{
	while(USART2_TX_FLAG);										//�ȴ���һ�η�����ɣ�USART2_TX_FLAGΪ1�����ڷ������ݣ�
	USART2_TX_FLAG=1;													//USART2���ͱ�־���������ͣ�
	DMA1_Channel7->CMAR  = (uint32_t)String;	//����Ҫ���͵����ݵ�ַ
	DMA1_Channel7->CNDTR = size;    					//����Ҫ���͵��ֽ���Ŀ
	DMA_Cmd(DMA1_Channel7, ENABLE);						//��ʼDMA����
}


/**
	�������ݰ�
  */
//void DMA1_Channel6_IRQHandler(void)
//{
//	u8 *p;
//	if(DMA_GetITStatus(DMA1_IT_TC6)!= RESET)		//DMA������ɱ�־
//	{
//		DMA_ClearITPendingBit(DMA1_IT_TC6); 			//����жϱ�־ 
//		USART_ClearFlag(USART2,USART_FLAG_TC);		//���USART2��־λ
//		DMA_Cmd(DMA1_Channel6, DISABLE );   			//�ر�USART2 TX DMA1 ��ָʾ��ͨ��
//		if(witchbuf)                        			//֮ǰ�õ�u2rxbuf���л�Ϊu1rxbuf
//		{
//			p=u2rxbuf;															//�ȱ���ǰһ�����ݵ�ַ���л�������
//			DMA1_Channel6->CMAR=(u32)u1rxbuf;				//�л�Ϊu1rxbuf��������ַ
//			witchbuf=0;                     				//��һ���л�Ϊu2rxbuf
//		}else                               			//֮ǰ�õ�u1rxbuf���л�Ϊu2rxbuf
//		{
//			p=u1rxbuf;															//�ȱ���ǰһ�����ݵ�ַ���л�������
//			DMA1_Channel6->CMAR=(u32)u2rxbuf;				//�л�Ϊu2rxbuf��������ַ
//			witchbuf=1;                     				//��һ���л�Ϊu1rxbuf
//		}
//		DMA1_Channel6->CNDTR = USART2_MAX_RX_LEN;	//DMAͨ����DMA����Ĵ�С
//		DMA_Cmd(DMA1_Channel6, ENABLE);     			//ʹ��USART2 TX DMA1 ��ָʾ��ͨ��
//		
//		//******************�������������������ݴ������������******************//
//		
//		DMA_USART2_Tx_Data(p,USART2_MAX_RX_LEN);
//		
//		//******************�������������������ݴ������������******************//
//		
//	}
//}

//DMA1ͨ��7�ж�
void DMA1_Channel7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC7)!= RESET)	//DMA������ɱ�־
	{
		DMA_ClearITPendingBit(DMA1_IT_TC7); 		//����жϱ�־ 
		USART_ClearFlag(USART2,USART_FLAG_TC);	//�������2�ı�־λ
		DMA_Cmd(DMA1_Channel7, DISABLE );   		//�ر�USART2 TX DMA1 ��ָʾ��ͨ��
		USART2_TX_FLAG=0;												//USART2���ͱ�־(�ر�)
	}
}

//����2�жϺ���
void USART2_IRQHandler(void)                	
{
	u8 *p;
	u8 USART2_RX_LEN = 0;																				//�������ݳ���
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)				//����2�����ж�
	{
		USART_ReceiveData(USART2); 																//�������2�����ж�IDLE��־λ
		USART_ClearFlag(USART2,USART_FLAG_TC);										//���USART2��־λ
		DMA_Cmd(DMA1_Channel6, DISABLE );   											//�ر�USART2 TX DMA1 ��ָʾ��ͨ��
		USART2_RX_LEN = USART2_MAX_RX_LEN - DMA1_Channel6->CNDTR;	//��ý��յ����ֽ���
		if(witchbuf)                        											//֮ǰ�õ�u2rxbuf���л�Ϊu1rxbuf
		{
			p=u2rxbuf;																							//�ȱ���ǰһ�����ݵ�ַ���л�������
			DMA1_Channel6->CMAR=(u32)u1rxbuf;												//�л�Ϊu1rxbuf��������ַ
			witchbuf=0;                     												//��һ���л�Ϊu2rxbuf
		}else                               											//֮ǰ�õ�u1rxbuf���л�Ϊu2rxbuf
		{
			p=u1rxbuf;																							//�ȱ���ǰһ�����ݵ�ַ���л�������
			DMA1_Channel6->CMAR=(u32)u2rxbuf;												//�л�Ϊu2rxbuf��������ַ
			witchbuf=1;                     												//��һ���л�Ϊu1rxbuf
		}
		DMA1_Channel6->CNDTR = USART2_MAX_RX_LEN;									//DMAͨ����DMA����Ĵ�С
		DMA_Cmd(DMA1_Channel6, ENABLE);     											//ʹ��USART2 TX DMA1 ��ָʾ��ͨ��
		
		//******************�������������������ݴ������������******************//
		
		DMA_USART2_Tx_Data(p,USART2_RX_LEN);
		ptr=p;
		
		//******************�������������������ݴ������������******************//
		
  }
	USART_ClearITPendingBit(USART2,USART_IT_ORE);								//���USART2_ORE��־λ
}
