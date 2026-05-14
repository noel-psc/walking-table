#include "telemetry_uart4.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

void TelemetryUart4_Init(void)
{
	(void)huart4;
}

void TelemetryUart4_SendDouble(double value)
{
	char buf[32];
	uint32_t int_part;
	uint32_t frac_part;
	uint32_t idx = 0;
	uint32_t tmp;
	uint32_t digits = 0;
	double abs_val = value;

	if (abs_val < 0.0) {
		buf[idx++] = '-';
		abs_val = -abs_val;
	}

	int_part = (uint32_t)abs_val;
	frac_part = (uint32_t)((abs_val - (double)int_part) * 1000.0 + 0.5);

	tmp = int_part;
	do {
		digits++;
		tmp /= 10U;
	} while (tmp > 0U);

	while (digits > 0U) {
		digits--;
		buf[idx + digits] = (char)('0' + (int_part % 10U));
		int_part /= 10U;
	}
	idx += (idx == 1U && buf[0] == '-') ? 1U : 0U;
	idx += (idx == 0U) ? 1U : 0U;

	buf[idx++] = '.';
	buf[idx++] = (char)('0' + ((frac_part / 100U) % 10U));
	buf[idx++] = (char)('0' + ((frac_part / 10U) % 10U));
	buf[idx++] = (char)('0' + (frac_part % 10U));
	buf[idx++] = '\r';
	buf[idx++] = '\n';

	HAL_UART_Transmit(&huart4, (uint8_t *)buf, (uint16_t)idx, 100);
}

void TelemetryUart4_SendInt(uint16_t value)
{
	char buf[16] = {0};
	snprintf(buf, sizeof(buf), "%u", value);
	HAL_UART_Transmit(&huart4, (uint8_t *)buf, (uint16_t)strlen(buf), HAL_MAX_DELAY);
}

void TelemetryUart4_SendString(const char *str)
{
	if (str == NULL) {
		return;
	}

	HAL_UART_Transmit(&huart4, (uint8_t *)str, (uint16_t)strlen(str), HAL_MAX_DELAY);
}
