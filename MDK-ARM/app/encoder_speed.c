#include "encoder_speed.h"

#define ENCODER_TWO_PI (6.283185307179586f)

static int32_t encoder_delta(TIM_HandleTypeDef *htim, uint32_t now, uint32_t last)
{
	uint32_t max = __HAL_TIM_GET_AUTORELOAD(htim);
	int32_t period = (int32_t)(max + 1U);
	int32_t diff = (int32_t)now - (int32_t)last;
	int32_t half = period / 2;

	if (diff > half) {
		diff -= period;
	} else if (diff < -half) {
		diff += period;
	}

	return diff;
}

void EncoderSpeed_Init(EncoderSpeedState *state, TIM_HandleTypeDef *htim)
{
	if ((state == 0) || (htim == 0)) {
		return;
	}

	state->htim = htim;
	state->last_count = __HAL_TIM_GET_COUNTER(htim);
	state->radps = 0.0f;
}

void EncoderSpeed_Update(EncoderSpeedState *state, float dt, float counts_per_rev)
{
	uint32_t now;
	int32_t delta;
	float revs;

	if ((state == 0) || (state->htim == 0)) {
		return;
	}

	if ((dt <= 0.0f) || (counts_per_rev <= 0.0f)) {
		state->radps = 0.0f;
		return;
	}

	now = __HAL_TIM_GET_COUNTER(state->htim);
	delta = encoder_delta(state->htim, now, state->last_count);
	state->last_count = now;

	revs = (float)delta / counts_per_rev;
	state->radps = (revs * ENCODER_TWO_PI) / dt;
}

float EncoderSpeed_GetRadps(const EncoderSpeedState *state)
{
	if (state == 0) {
		return 0.0f;
	}

	return state->radps;
}
