#include "stm32f10x.h"                  // Device header


void MySPI_W_SS(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)BitValue);		//根据BitValue，设置SCL引脚的电平
}

void MySPI_W_SCK(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_5, (BitAction)BitValue);		//根据BitValue，设置SCL引脚的电平
}

void MySPI_W_MOSI(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_7, (BitAction)BitValue);		//根据BitValue，设置SCL引脚的电平
}


u8 MySPI_R_MISO(void)
{
	uint8_t temp;
	temp = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);		//读取SDA电平											
	return temp;											//返回SDA电平
}



void MySPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOB的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PB10和PB11引脚初始化为开漏输出
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PB10和PB11引脚初始化为开漏输出
	/*设置默认电平*/
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