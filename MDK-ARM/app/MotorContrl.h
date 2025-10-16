#ifndef __PWM_H
#define	__PWM_H
#include "stm32f4xx_hal.h"       // 主HAL库头文件
#include "stm32f4xx_hal_tim.h"   // 定时器HAL库头文件
#include "main.h"
#include "cmsis_os.h"

extern TIM_HandleTypeDef htim3;
void Set_PWM(int motor_left,int motor_right);

#endif

