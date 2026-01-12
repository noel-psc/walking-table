#include "MotorContrl.h"
#include "utils.h"

const float k_=0.1;  // Smoothing factor (0 < k < 1)

    // 计算旋转半径（考虑长宽比影响）
const float R = 0.34;

// 45°角的正弦余弦值（sqrt(2)/2）
const float sin45 = 0.707f;  // sqrt(2)/2
const float cos45 = 0.707f;  // sqrt(2)/2
const float cos60 = 0.5f;    // cos(60°)=0.5
const float sin60 = 0.866f;  // sin(60°)=sqrt(3)/2
    

// 为4个轮子声明滤波器实例
Chassic_State chassis;

// 全局底盘参数
const Chassis_Params chassis_params={0.3f,0.15f,0.05f};//初始化底盘参数 车长0.3m 车宽0.3m 轮子半径0.05m

Velocity_Input v_input;// 速度输入结构体实例

/*************************************************************
函数功能：PWM赋值
入口参数：motor_left:左电机PWM值，motor_right:右电机PWM值
返回  值：无
*************************************************************/
void Motor_init(void)
{
    Stop_PWM(&htim3);
    Stop_PWM(&htim5);
	
    Start_PWM(&htim3);
    Start_PWM(&htim5);
    //Setup_Filters();

}

void Motor_contrl(JOYSTICK_TypeDef JOYSTICK)
{  
   v_input.vx = JOYSTICK.RJoy_LR-0x80;
   v_input.vy = JOYSTICK.RJoy_UD-0x7f;
   v_input.omega = JOYSTICK.LJoy_LR-0x80;//第一步处理
   omni_wheel_inverse_kinematics(v_input,&chassis);
   Set_PWM(&htim3,fof_update(&chassis.motors[0]),fof_update(&chassis.motors[1]));

    Set_PWM(&htim5,fof_update(&chassis.motors[2]),fof_update(&chassis.motors[3])); 
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

// 初始化滤波器部分

// void Setup_Filters() {
//     for(int i = 0; i < 4; i++) {
//          static float input_[MOTORNUMBER] = 0.0f;  // Current input value    
  
//          static float output_[MOTORNUMBER] = 0.0f; // Current output value
//     }
// }




float fof_update(Single_Motor* input) {

    input->lastspeed = input->lastspeed * (1 - k_) + input->targetspeed * k_;

    return input->lastspeed;
	
}



// int Velocity_A(int TargetVelocity, int CurrentVelocity)
// {  
//      int Bias;  //定义相关变量
// 		static int ControlVelocity, Last_bias; //静态变量，函数调用结束后其值依然存在
//         Bias=TargetVelocity-CurrentVelocity; //求速度偏差
		
// 		ControlVelocity+=Velcity_Kp*(Bias-Last_bias)+Velcity_Ki*Bias;  //增量式PI控制器
//                                                                    //Velcity_Kp*(Bias-Last_bias) 作用为限制加速度
// 	                                                                 //Velcity_Ki*Bias             速度控制值由Bias不断积分得到 偏差越大加速度越大
// 	    if(ControlVelocity>7200)ControlVelocity=7200;
// 		if(ControlVelocity<-7200)ControlVelocity=-7200;
// 		Last_bias=Bias;	
// 		return ControlVelocity; //返回速度控制值
// }






/**
 * @brief 初始化底盘参数
 * @param length 车长（前后轮距）
 * @param width 车宽（左右轮距）
 * @param wheel_radius 轮子半径
 */
// void kinematics_init(float length, float width, float wheel_radius) {
//     chassis_params.length = length;
//     chassis_params.width = width;
//     chassis_params.wheel_radius = wheel_radius;
// }

/**
 * @brief 全向轮运动学逆解计算
 * @param input 速度输入（vx, vy, omega）
 * @param motor_speeds 输出参数，四电机速度数组
 * 
 * 轮子编号约定：
 * 0: 前左轮  1: 前右轮
 * 2: 后左轮  3: 后右轮
 * 
 * 安装角度：45°（典型全向轮安装方式）
 */
void omni_wheel_inverse_kinematics(Velocity_Input input, Chassic_State* chassis_) {
    // 全向轮运动学逆解公式[4,6](@ref)输出 chassis_->motors[i].targetspeed 的单位是 1/s（即线速度/轮半径 → 角速度，rad/s），因为公式最后除以 wheel_radius。
    //速度=编码器读数（利用中断进行一个周期读数）*读取频率*/倍频数/减速比/编码器精度*轮子的周长。然后就可以根据输入的速度来反向计算出编码器的值（运动学逆运算）。
    // 考虑轮子安装角度为45°的情况
    chassis_->motors[0].targetspeed = ( input.vx * sin60 - input.vy * cos60 - input.omega * R) / chassis_params.wheel_radius;  // 前左轮
    chassis_->motors[1].targetspeed = (-input.vx * sin60 - input.vy * cos60 - input.omega * R) / chassis_params.wheel_radius;  // 前右轮
    chassis_->motors[2].targetspeed = ( input.vx * sin60 + input.vy * cos60 - input.omega * R) / chassis_params.wheel_radius;  // 后左轮
    chassis_->motors[3].targetspeed = (-input.vx * sin60 + input.vy * cos60 - input.omega * R) / chassis_params.wheel_radius;  // 后右轮

    // 注：如果轮子安装角度不同，需要调整上述公式中的角度参数[7](@ref)
    
}


