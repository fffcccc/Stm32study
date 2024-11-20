#ifndef __MYUART_H
#define __MYUART_H

#include <stdio.h>

#define    USART2_MAX_TX_LEN		500			     	//����ͻ����ֽ���
#define    USART2_MAX_RX_LEN		500			     	//�����ջ����ֽ���
extern u8  USART2_TX_BUF[USART2_MAX_TX_LEN]; 	//���ͻ�����
extern u8  u1rxbuf[USART2_MAX_RX_LEN];				//�������ݻ�����1
extern u8	 u2rxbuf[USART2_MAX_RX_LEN];				//�������ݻ�����2
extern u8  witchbuf;                  				//��ǵ�ǰʹ�õ����ĸ�������,0,ʹ��u1rxbuf��1,ʹ��u2rxbuf��
extern u8 USART2_TX_FLAG;
extern u8 USART2_RX_FLAG;




void Serial_Init(unsigned long baudrate);
void Serial_SendByte(uint8_t Byte);
void Serial_SendString(char *String,u32 size);
//void Serial_SendArray(uint8_t *Array, uint16_t Length);
//void Serial_SendString(char *String);
//void Serial_SendNumber(uint32_t Number, uint8_t Length);
//void Serial_Printf(char *format, ...);



void DMA1_USART2_Init(void);

void DMA_USART2_Tx_Data(u8 *buffer, u32 size);

#endif
