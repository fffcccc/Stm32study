#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "PWM.h"
#include "IC.h"
#include "ADC.h"
#include "Myuart.h"

uint8_t send = 44;
u8* ptr;
char send1[6]="yuygg";
extern u8 u1rxbuf[USART2_MAX_RX_LEN];				//发送数据缓冲区1
extern u8 u2rxbuf[USART2_MAX_RX_LEN];				//发送数据缓冲区2
extern u8 witchbuf;  
int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	Serial_Init(115200);
	Serial_SendString(send1,6);
	
	while (1)
	{
			OLED_ShowChar(1,1,(char)ptr[0]);
	}
}
