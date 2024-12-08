#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "PWM.h"
#include "IC.h"
#include "ADC.h"
#include "Myuart.h"
#include "W25Q64.h"



u8* ptr;
int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	PWM_Init();
	MG996R_Set_Angle(-90);

	//主循环
	while (1)
	{
		
	}
}
