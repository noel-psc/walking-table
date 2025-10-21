#ifndef __PWM_H
#define	__PWM_H
#include "stm32f4xx_hal.h"       // ��HAL��ͷ�ļ�
#include "stm32f4xx_hal_tim.h"   // ��ʱ��HAL��ͷ�ļ�
#include "main.h"
#include "cmsis_os.h"
#include "ax_ps2.h"
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim5;
extern JOYSTICK_TypeDef my_joystick;
void Set_PWM(TIM_HandleTypeDef *htim,int motor_left,int motor_right);
void Motor_contrl(JOYSTICK_TypeDef JOYSTICK);
void Stop_PWM(TIM_HandleTypeDef *htim);
void Start_PWM(TIM_HandleTypeDef *htim);
#endif

