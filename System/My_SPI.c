#include "stm32f10x.h"                  // Device header


void MySPI_W_SS(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)BitValue);		//����BitValue������SCL���ŵĵ�ƽ
}

void MySPI_W_SCK(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_5, (BitAction)BitValue);		//����BitValue������SCL���ŵĵ�ƽ
}

void MySPI_W_MOSI(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_7, (BitAction)BitValue);		//����BitValue������SCL���ŵĵ�ƽ
}


u8 MySPI_R_MISO(void)
{
	uint8_t temp;
	temp = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);		//��ȡSDA��ƽ											
	return temp;											//����SDA��ƽ
}



void MySPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//����GPIOB��ʱ��
	
	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//��PB10��PB11���ų�ʼ��Ϊ��©���
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//��PB10��PB11���ų�ʼ��Ϊ��©���
	/*����Ĭ�ϵ�ƽ*/
	MySPI_W_SS(1);
	MySPI_W_SCK(0);
	
}


void MySPI_Start(void)
{
		MySPI_W_SS(0);
}

void MySPI_Stop(void)
{
	MySPI_W_SS(1);
}



uint8_t MySPI_Swap(u8 data)
{
	u8 i;
	for(i =0;i<8;i++)
	{
		MySPI_W_MOSI(data&(0x80>>1));
		data <<=1;
		MySPI_W_SCK(1);
		if(MySPI_R_MISO()==1){data |= 0x01;}
		MySPI_W_SCK(0);
	}
	return data;
}