/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MotorContrl.h"
#include "usart.h"
#include <string.h>    // Ϊ memset, strncpy, strlen �Ⱥ���
#include <stdlib.h>    // Ϊ strtol ����
#include "ax_ps2.h"    // Ϊ parse_joystick_data ����������
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for motor_move */
osThreadId_t motor_moveHandle;
const osThreadAttr_t motor_move_attributes = {
  .name = "motor_move",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for remote_control */
osThreadId_t remote_controlHandle;
const osThreadAttr_t remote_control_attributes = {
  .name = "remote_control",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityRealtime1,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void motor_move_entry(void *argument);
void remote_control_entry(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of motor_move */
  motor_moveHandle = osThreadNew(motor_move_entry, NULL, &motor_move_attributes);

  /* creation of remote_control */
  remote_controlHandle = osThreadNew(remote_control_entry, NULL, &remote_control_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_motor_move_entry */
/**
* @brief Function implementing the motor_move thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_motor_move_entry */
void motor_move_entry(void *argument)
{
  /* USER CODE BEGIN motor_move_entry */
  /* Infinite loop */
  for(;;)
  {
	Motor_contrl(table_state);
    osDelay(1);
  }
  /* USER CODE END motor_move_entry */
}

/* USER CODE BEGIN Header_remote_control_entry */
/**
* @brief Function implementing the remote_control thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_remote_control_entry */
void remote_control_entry(void *argument)
{
  /* USER CODE BEGIN remote_control_entry */
  /* Infinite loop */
  for(;;)
  {
  
	if (uart_rx_complete) {

    
        uart_rx_buffer[uart_rx_length] = '\0';

		parse_joystick_data((uint8_t*)uart_rx_buffer, &table_state);

        memset(uart_rx_buffer, 0, RX_BUFFER_SIZE);
        uart_rx_length = 0;
		uart_rx_complete = 0;
    }
    osDelay(1);
  }
  /* USER CODE END remote_control_entry */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

