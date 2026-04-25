#include "PID.h"

#define PID_EPSILON_F (1e-6f)

/* 取绝对值，用于统一处理正负限幅输入 */
static float pid_absf(float x)
{
	return (x >= 0.0f) ? x : -x;
}

/* 双向限幅：将 value 约束到 [-|limit|, |limit|] */
static float pid_clampf(float value, float limit)
{
	float lim = pid_absf(limit);

	if (value > lim) {
		return lim;
	}

	if (value < -lim) {
		return -lim;
	}

	return value;
}

static int16_t pid_clamp_i16(int32_t value, int16_t limit)
{
	int32_t lim = (limit >= 0) ? limit : -((int32_t)limit);

	if (value > lim) {
		return (int16_t)lim;
	}

	if (value < -lim) {
		return (int16_t)(-lim);
	}

	return (int16_t)value;
}

/*
 * 初始化PID参数和运行状态。
 * dt <= 0 时使用默认值 1ms，避免后续微分计算除零。
 */
void PID_Init(PID_Controller *pid,
			  float kp,
			  float ki,
			  float kd,
			  float dt,
			  float max_out,
			  float max_i_out)
{
	if (pid == 0) {
		return;
	}

	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;

	pid->dt = (dt > 0.0f) ? dt : 0.001f;

	pid->max_out = pid_absf(max_out);
	pid->max_i_out = pid_absf(max_i_out);

	PID_Reset(pid);
}

/* 清空历史状态，通常用于上电或切换控制模式后 */
void PID_Reset(PID_Controller *pid)
{
	if (pid == 0) {
		return;
	}

	pid->set = 0.0f;
	pid->fdb = 0.0f;

	pid->err = 0.0f;
	pid->err_last = 0.0f;

	pid->p_out = 0.0f;
	pid->i_out = 0.0f;
	pid->d_out = 0.0f;
	pid->out = 0.0f;
}

/* 统一入口：当前仅保留位置式PID */
float PID_Calc(PID_Controller *pid, float set, float fdb)
{
	if (pid == 0) {
		return 0.0f;
	}

	return PID_CalcPosition(pid, set, fdb);
}

/*
 * 速度环封装（m/s）：
 * set 与 fdb 都使用米每秒，内部直接调用通用 PID_Calc。
 */
float PID_CalcSpeedMps(PID_Controller *pid, float set_mps, float fdb_mps)
{
	return PID_Calc(pid, set_mps, fdb_mps);
}

/* 线速度(m/s) -> 轮角速度(rad/s): omega = v / r */
float PID_LinearMpsToWheelRadps(float linear_mps, float wheel_radius_m)
{
	if (pid_absf(wheel_radius_m) < PID_EPSILON_F) {
		return 0.0f;
	}

	return linear_mps / wheel_radius_m;
}

/* 轮角速度(rad/s) -> 线速度(m/s): v = omega * r */
float PID_WheelRadpsToLinearMps(float wheel_radps, float wheel_radius_m)
{
	return wheel_radps * wheel_radius_m;
}

/*
 * 基于标定参数的速度->PWM映射：
 * pwm = gain * speed + sign(speed) * bias
 */
int16_t PID_SpeedMpsToPwm(float speed_mps, const PID_SpeedMap *map)
{
	float abs_speed;
	float pwm_f;
	int32_t pwm_i;

	if (map == 0) {
		return 0;
	}

	abs_speed = pid_absf(speed_mps);
	pwm_f = map->speed_to_pwm_gain * speed_mps;

	if (abs_speed > PID_EPSILON_F) {
		if (speed_mps > 0.0f) {
			pwm_f += map->speed_to_pwm_bias;
		} else {
			pwm_f -= map->speed_to_pwm_bias;
		}
	}

	pwm_i = (int32_t)pwm_f;

	if ((map->pwm_deadzone > 0) && (pwm_i != 0)) {
		int32_t deadzone = map->pwm_deadzone;
		if ((pwm_i > 0) && (pwm_i < deadzone)) {
			pwm_i = deadzone;
		} else if ((pwm_i < 0) && (pwm_i > -deadzone)) {
			pwm_i = -deadzone;
		}
	}

	return pid_clamp_i16(pwm_i, map->pwm_max);
}

/*
 * 位置式PID：
 * out = Kp*e + Ki*Σe*dt + Kd*(e[k]-e[k-1])/dt
 */
float PID_CalcPosition(PID_Controller *pid, float set, float fdb)
{
	if (pid == 0) {
		return 0.0f;
	}

	pid->set = set;
	pid->fdb = fdb;

	pid->err = pid->set - pid->fdb;

	pid->p_out = pid->kp * pid->err;

	/* 积分累加并做抗饱和限幅 */
	pid->i_out += pid->ki * pid->err * pid->dt;
	pid->i_out = pid_clampf(pid->i_out, pid->max_i_out);

	/* 微分项用误差差分近似 */
	pid->d_out = pid->kd * (pid->err - pid->err_last) / pid->dt;

	pid->out = pid->p_out + pid->i_out + pid->d_out;
	/* 总输出限幅，匹配执行器能力 */
	pid->out = pid_clampf(pid->out, pid->max_out);

	pid->err_last = pid->err;

	return pid->out;
}

/* 在线调参接口 */
void PID_SetTunings(PID_Controller *pid, float kp, float ki, float kd)
{
	if (pid == 0) {
		return;
	}

	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
}

/* 在线调整限幅，并把当前状态裁剪到新限幅内 */
void PID_SetLimits(PID_Controller *pid, float max_out, float max_i_out)
{
	if (pid == 0) {
		return;
	}

	pid->max_out = pid_absf(max_out);
	pid->max_i_out = pid_absf(max_i_out);
	pid->i_out = pid_clampf(pid->i_out, pid->max_i_out);
	pid->out = pid_clampf(pid->out, pid->max_out);
}

