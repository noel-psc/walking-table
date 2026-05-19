#include "ax_ps2.h"

typedef union {
    uint8_t bytes[4];
    float value;
} FloatConverter;

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
  /* 首先判断是哪个串口触发的中断，这在有多个串口时非常必要 */
  if (huart->Instance == USART3)
  {
    /* 这里是您的应用逻辑实现区 */
    // 1. 此时，DMA已经将数据填充至接收缓冲区的前半部分
    // 2. 您可以安全地处理这前半部分数据
    // 例如：Process_Received_Data(rx_buffer, RX_BUFFER_SIZE / 2);
	uart_rx_complete = 1;
	
	 
    // 示例：点亮一个LED指示半传输发生
    // HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  }

  /* 如果还有其他串口（如USART2）也使用了半传输中断，可以继续添加判断 */
  // else if (huart->Instance == USART2)
  // {
  //   // 处理USART2的数据
  // }
}
// 解析摇杆数据函数
uint8_t parse_joystick_data(const uint8_t* data, JOYSTICK_TypeDef* joy) {
//    const char* tokens[] = {"RJOY_LR:", "RJOY_UD:", "LJOY_LR:", "LJOY_UD:"};
//    uint8_t* values[] = {&joy->RJoy_LR, &joy->RJoy_UD, &joy->LJoy_LR, &joy->LJoy_UD};
//    int token_count = 4;
//    int found_count = 0;
//    
//    // 确保字符串以空字符结尾[5](@ref)
    uint8_t working_data[RX_BUFFER_SIZE];
    memcpy(working_data, data, RX_BUFFER_SIZE);
    
    // 在缓冲区中搜索数据包头（假设0xAA 0x55是包头）
    uint16_t i = 0;
    for (i = 0; i < RX_BUFFER_SIZE - 1; i++) {
        if ((uint8_t)data[i] == 0xAA && (uint8_t)data[i + 1] == 0x55) {
            break; // 找到包头
        }
    }
    if (i >= RX_BUFFER_SIZE - 1) {
        return 1; // 没有找到包头，返回错误
    }
    // 解析数据（跳过包头2字节）
    const uint8_t* payload_start = &data[i + 2];
    joy->mode = (uint8_t)payload_start[0];     // MODE
    joy->btn1 = (uint8_t)payload_start[1];     // BTN1
    joy->btn2 = (uint8_t)payload_start[2];     // BTN2
    joy->RJoy_LR = (uint8_t)payload_start[3];  // RJOY_LR
    joy->RJoy_UD = (uint8_t)payload_start[4];  // RJOY_UD
    joy->LJoy_LR = (uint8_t)payload_start[5];  // LJOY_LR
    joy->LJoy_UD = (uint8_t)payload_start[6];  // LJOY_UD
    memset(working_data, 0, RX_BUFFER_SIZE);
    // 如果成功找到并解析了所有4个值，返回成功[3]
    return 0;
}

uint8_t parse_cmd_vel_data(const uint8_t* data, Velocity_Input* input) {
    uint16_t i = 0;

    for (i = 0; i < RX_BUFFER_SIZE - 1; i++) {
        if ((uint8_t)data[i] == 0xAA && (uint8_t)data[i + 1] == 0x55) {
            break;
        }
    }
    if (i >= RX_BUFFER_SIZE - 1) {
        return 1;
    }

    const uint8_t* payload_start = &data[i + 2];

    if (payload_start[12] != 0x0D || payload_start[13] != 0x0A) {
        return 1;
    }

    FloatConverter converter;
    memcpy(converter.bytes, &payload_start[0], 4);
    input->vx = converter.value;
    memcpy(converter.bytes, &payload_start[4], 4);
    input->vy = converter.value;
    memcpy(converter.bytes, &payload_start[8], 4);
    input->omega = converter.value;
    input->source = INPUT_SOURCE_CMD_VEL;

    return 0;
}

