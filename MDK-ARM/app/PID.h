#ifndef __PID_H
#define __PID_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	float kp;  /* 比例系数 */
	float ki;  /* 积分系数 */
	float kd;  /* 微分系数 */

	float dt;  /* 控制周期，单位：秒 */

	float max_out;    /* 总输出限幅 */
	float max_i_out;  /* 积分项限幅 */

	float set;  /* 目标值 */
	float fdb;  /* 反馈值 */

	float err;       /* 本次误差 e[k] */
	float err_last;  /* 上次误差 e[k-1] */

	float p_out;  /* 比例输出 */
	float i_out;  /* 积分输出 */
	float d_out;  /* 微分输出 */
	float out;    /* 最终输出 */
} PID_Controller;

/*
 * 速度到PWM的标定参数（需要实测）
 * pwm ~= speed_to_pwm_gain * speed_mps +/- speed_to_pwm_bias
 */
typedef struct {
	float wheel_radius_m;    /* 轮半径（米），用于线速度/角速度转换 */
	float speed_to_pwm_gain; /* 速度到PWM斜率（实测得到） */
	float speed_to_pwm_bias; /* 克服静摩擦的偏置（实测得到） */
	int16_t pwm_max;         /* PWM绝对值上限 */
	int16_t pwm_deadzone;    /* 输出死区补偿阈值（可为0） */
} PID_SpeedMap;

/*
 * @brief 初始化PID控制器
 * @param pid       PID控制器实例
 * @param kp,ki,kd  PID参数
 * @param dt        控制周期（秒）
 * @param max_out   输出限幅
 * @param max_i_out 积分限幅
 */
void PID_Init(PID_Controller *pid,
			  float kp,
			  float ki,
			  float kd,
			  float dt,
			  float max_out,
			  float max_i_out);

/* 清空历史误差和输出状态 */
void PID_Reset(PID_Controller *pid);

/* 根据当前模式计算PID输出 */
float PID_Calc(PID_Controller *pid, float set, float fdb);

/* 速度环封装：形参单位为米每秒（m/s），要求set/fdb都已是线速度 */
float PID_CalcSpeedMps(PID_Controller *pid, float set_mps, float fdb_mps);

/* 线速度(m/s) -> 轮角速度(rad/s)，需提供轮半径 */
float PID_LinearMpsToWheelRadps(float linear_mps, float wheel_radius_m);

/* 轮角速度(rad/s) -> 线速度(m/s)，需提供轮半径 */
float PID_WheelRadpsToLinearMps(float wheel_radps, float wheel_radius_m);

/*
 * 将“线速度指令(m/s)”映射为PWM（需要实测标定参数）。
 * 返回值范围：[-pwm_max, pwm_max]
 */
int16_t PID_SpeedMpsToPwm(float speed_mps, const PID_SpeedMap *map);

/* 按位置式PID计算输出 */
float PID_CalcPosition(PID_Controller *pid, float set, float fdb);

/* 运行时更新PID参数 */
void PID_SetTunings(PID_Controller *pid, float kp, float ki, float kd);

/* 运行时更新输出/积分限幅 */
void PID_SetLimits(PID_Controller *pid, float max_out, float max_i_out);

#ifdef __cplusplus
}
#endif

#endif