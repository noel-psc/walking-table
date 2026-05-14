#ifndef TELEMETRY_UART4_H
#define TELEMETRY_UART4_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void TelemetryUart4_Init(void);
void TelemetryUart4_Tick1ms(void);
void TelemetryUart4_SendDouble(double value);
void TelemetryUart4_SendInt(uint16_t value);
void TelemetryUart4_SendString(const char *str);

#ifdef __cplusplus
}
#endif

#endif
