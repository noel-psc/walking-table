#include "speed_loop.h"

void SpeedLoop_Init(SpeedLoop *loop,
				   float kp,
				   float ki,
				   float kd,
				   float dt,
				   float max_out,
				   float max_i_out)
{
	if (loop == 0) {
		return;
	}

	PID_Init(&loop->pid, kp, ki, kd, dt, max_out, max_i_out);
}

void SpeedLoop_Reset(SpeedLoop *loop)
{
	if (loop == 0) {
		return;
	}

	PID_Reset(&loop->pid);
}

float SpeedLoop_Update(SpeedLoop *loop, float target_radps, float feedback_radps)
{
	if (loop == 0) {
		return 0.0f;
	}

	return PID_CalcIncremental(&loop->pid, target_radps, feedback_radps);
}
