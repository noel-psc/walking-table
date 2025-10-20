#include "MotorContrl.h"

/*************************************************************
�������ܣ�PWM��ֵ
��ڲ�����motor_left:����PWMֵ��motor_right:�ҵ��PWMֵ
����  ֵ����
*************************************************************/
void Set_PWM(int motor_left,int motor_right)
{	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
   if(motor_left>0)   
   {
	   __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,7200);
	   __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2,7200-motor_left);
   }
   else
   {
	   __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,7200+motor_left);
	   __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2,7200);
   }
   if(motor_right>0)
   {
	   __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,7200);
	   __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4,7200-motor_right);
   }
   else
   {
	   __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,7200+motor_right);
	   __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4,7200);
   }
	
	
}



motor_left = (motor_left > 7200) ? 7200 : motor_left;
motor_left = (motor_left < -7200) ? -7200 : motor_left;
// 对 motor_right 做同样处理