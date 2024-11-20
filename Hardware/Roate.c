#include "stm32f10x.h" 

uint16_t Roate_Count;



void Roate_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure1;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_13;
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure1);
	
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource15);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource13);
	
	
	EXTI_InitTypeDef EXTI_InitStructure1;
	EXTI_InitStructure1.EXTI_Line = EXTI_Line15|EXTI_Line13;
	EXTI_InitStructure1.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure1.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure1.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure1);

	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
}


uint16_t Roate_Get(void)
{
	uint16_t TEMP;
	TEMP =Roate_Count;
	return TEMP;
}


void EXTI15_10_IRQHandler(void)
{
		if (EXTI_GetITStatus(EXTI_Line13) == SET)
	{
	
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0 & GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 1 )
		{
			Roate_Count--;
		}
	}
	
	if (EXTI_GetITStatus(EXTI_Line15) == SET)
	{
	
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 1 & GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0 )
		{
			Roate_Count++;
		}
	}
	

	EXTI_ClearITPendingBit(EXTI_Line13|EXTI_Line15);
}
