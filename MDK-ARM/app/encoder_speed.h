#ifndef ENCODER_SPEED_H
#define ENCODER_SPEED_H

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	TIM_HandleTypeDef *htim;
	uint32_t last_count;
	float radps;
} EncoderSpeedState;

void EncoderSpeed_Init(EncoderSpeedState *state, TIM_HandleTypeDef *htim);
void EncoderSpeed_Update(EncoderSpeedState *state, float dt, float counts_per_rev);
float EncoderSpeed_GetRadps(const EncoderSpeedState *state);

#ifdef __cplusplus
}
#endif

#endif
