#ifndef __PWM_H
#define	__PWM_H
#include "stm32f4xx_hal.h"       // ��HAL��ͷ�ļ�
#include "stm32f4xx_hal_tim.h"   // ��ʱ��HAL��ͷ�ļ�
#include "main.h"
#include "cmsis_os.h"

extern TIM_HandleTypeDef htim3;
void Set_PWM(int motor_left,int motor_right);
void Motor_contrl();
void Stop_PWM(void);
void Start_PWM(void);
#endif

