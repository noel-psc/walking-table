#ifndef __AX_PS2_H
#define __AX_PS2_H

#include "main.h"
#include "cmsis_os.h" // 提供FreeRTOS类型和API
#include "usart.h"
#include "MotorContrl.h"
#include <string.h>  // 为 strncpy, strstr, strlen
#include <stdlib.h>  // 为 strtol
extern  JOYSTICK_TypeDef table_state;
uint8_t parse_joystick_data(const uint8_t* data, JOYSTICK_TypeDef* joy);
typedef struct {
    uint8_t mode;        // MODE: 工作模式
    uint8_t btn1;        // BTN1: 按钮1状态
    uint8_t btn2;        // BTN2: 按钮2状态
    uint8_t RJoy_LR;     // RJOY_LR: 右摇杆左右
    uint8_t RJoy_UD;     // RJOY_UD: 右摇杆上下
    uint8_t LJoy_LR;     // LJOY_LR: 左摇杆左右
    uint8_t LJoy_UD;     // LJOY_UD: 左摇杆上下

} uart_TypeDef;
extern uart_TypeDef uart;

#endif
