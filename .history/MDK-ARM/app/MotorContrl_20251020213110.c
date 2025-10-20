#include "MotorContrl.h"


/*************************************************************
函数功能：PWM赋值
入口参数：motor_left:左电机PWM值，motor_right:右电机PWM值
返回  值：无
*************************************************************/
void Motor_contrl()
{
   Start_PWM();

   Set_PWM(2000,2000);
}
void Set_PWM(TIM_HandleTypeDef *htim,int motor_left,int motor_right)
{	motor_left = (motor_left > 7200) ? 7200 : motor_left;
   motor_left = (motor_left < -7200) ? -7200 : motor_left;
   motor_right = (motor_right > 7200) ? 7200 : motor_right;
   motor_right = (motor_right < -7200) ? -7200 : motor_right;

   if(motor_left>0)   
   {  
	   __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1,7200);
	   __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2,7200-motor_left);
   }
   else
   {
	   __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1,7200+motor_left);
	   __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2,7200);
   }
   if(motor_right>0)
   {
	   __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3,7200);
	   __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4,7200-motor_right);
   }
   else
   {
	   __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3,7200+motor_right);
	   __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4,7200);
   }
	
	
}
void Stop_PWM(TIM_HandleTypeDef *htim)
{
   HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_1);
   HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_2);
   HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_3);
   HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_4);
}
void Start_PWM(TIM_HandleTypeDef *htim)
{
   HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
   HAL_TIM_PWM_Start(htim, TIM_CHANNEL_2);
   HAL_TIM_PWM_Start(htim, TIM_CHANNEL_3);
   HAL_TIM_PWM_Start(htim, TIM_CHANNEL_4);
} 

