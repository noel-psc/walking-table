#ifndef SPEED_LOOP_H
#define SPEED_LOOP_H

#define __SPEED_LOOP_ON

#include "PID.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	PID_Controller pid;
} SpeedLoop;

void SpeedLoop_Init(SpeedLoop *loop,
				   float kp,
				   float ki,
				   float kd,
				   float dt,
				   float max_out,
				   float max_i_out);

void SpeedLoop_Reset(SpeedLoop *loop);
float SpeedLoop_Update(SpeedLoop *loop, float target_radps, float feedback_radps);

#ifdef __cplusplus
}
#endif

#endif
