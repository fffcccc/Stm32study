#ifndef __PWM_H
#define __PWM_H

void PWM_Init(void);
void Set_Plus(uint16_t num);
void PWM_SetCompare1(uint16_t Compare);
void PWM_SetPrescaler(uint16_t Prescaler);
void MG996R_Set_Angle(int16_t angle);

#endif
