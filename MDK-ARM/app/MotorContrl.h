#ifndef __PWM_H
#define	__PWM_H
#include "stm32f4xx_hal.h"       
#include "stm32f4xx_hal_tim.h"   
#include "main.h"
#include "cmsis_os.h"
#include "iwdg.h"
#define MOTORNUMBER 4

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim8;
extern const float k_;
typedef struct			 				
{
  uint8_t mode;		    /* 手柄的工作模式 */

  uint8_t btn1;         /* B0:SLCT B1:JR  B0:JL B3:STRT B4:UP B5:R B6:DOWN  B7:L   */

  uint8_t btn2;         /* B0:L2   B1:R2  B2:L1 B3:R1   B4:Y  B5:B B6:A     B7:X */

  uint8_t RJoy_LR;      /* 右边摇杆  0x00 = 左    0xff = 右   */

  uint8_t RJoy_UD;      /* 右边摇杆  0x00 = 上    0xff = 下   */

  uint8_t LJoy_LR;      /* 左边摇杆  0x00 = 左    0xff = 右   */

  uint8_t LJoy_UD;      /* 左边摇杆  0x00 = 上    0xff = 下   */
	
}JOYSTICK_TypeDef;

extern JOYSTICK_TypeDef joystick_state;

// 单个电机结构体定义
typedef struct {
    float lastspeed; 
    float speed;                
    float targetspeed;               
} Single_Motor;

typedef struct {
    Single_Motor motors[MOTORNUMBER]; // 多个电机
} Chassic_State;
extern Chassic_State chassis;

// 底盘参数结构体
typedef struct {
    float length;      // 车长（前后轮距，单位：米）
    float width;       // 车宽（左右轮距，单位：米）
    float wheel_radius;// 轮子半径（单位：米）
} Chassis_Params;

// 输入来源
typedef enum {
    INPUT_SOURCE_NONE = 0,
    INPUT_SOURCE_JOYSTICK = 1,
    INPUT_SOURCE_CMD_VEL = 2,
} Input_Source;

// 速度输入结构体
typedef struct {
    Input_Source source;
    float vx;          // 前后速度（m/s，前进为正）
    float vy;          // 左右速度（m/s，左移为正）
    float omega;       // 旋转速度（rad/s，逆时针为正）
} Velocity_Input;

extern Velocity_Input v_input;

// 初始化底盘参数
//void kinematics_init(float length, float width, float wheel_radius);

// 全向轮运动学逆解
void omni_wheel_inverse_kinematics(Velocity_Input input, Chassic_State* chassis_);

extern const float k_;      // Smoothing factor (0 < k < 1)
float fof_update(Single_Motor* input);


void Set_PWM(TIM_HandleTypeDef *htim,int motor_left,int motor_right);
void Motor_apply_input(Velocity_Input input);
void Motor_contrl(JOYSTICK_TypeDef joystick);
void Joystick_to_input(const JOYSTICK_TypeDef* joystick, Velocity_Input* input);
void Stop_PWM(TIM_HandleTypeDef *htim);
void Start_PWM(TIM_HandleTypeDef *htim);
void Motor_init(void);
#endif

