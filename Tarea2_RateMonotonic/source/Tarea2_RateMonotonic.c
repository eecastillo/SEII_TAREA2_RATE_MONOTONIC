/*
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    1.2-contextSwitch.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK66F18.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */
#define RTOS_STACK_SIZE 500

#define RTOS_MAX_NUMBER_OF_TASKS 3

#define STACK_FRAME_SIZE 8

#define STACK_PC_OFFSET 2

#define STACK_PSR_OFFSET 1

#define STACK_PSR_DEFAULT 0x01000000

typedef enum
{
  stateReady,
  stateRunning,
  stateWaiting
} rtosTaskState_t;

typedef enum
{
  priority_0,       //Highest priority
  priority_1,
  priority_2,
  priority_3,
  priority_4       //Lowest priority
}priority_t;

typedef enum
{
  from_execution,
  from_isr
} rtosContextSwitchFrom_t;

typedef struct
{
  uint32_t *sp;
  void (*task_body)();
  uint32_t stack[RTOS_STACK_SIZE];
  uint64_t local_tick;
  rtosTaskState_t state;
  priority_t priority;
  uint8_t ID;
} task_t;

struct
{
  uint8_t nTask;
  int8_t current_task;
  int8_t next_task;
  task_t tasks[RTOS_MAX_NUMBER_OF_TASKS + 1];
  uint64_t global_tick;
} task_list =
{ 0 };

void task0(void);

void task1(void);

void task2(void);

void taskIdel(void);

void rtosStart(void);

void rtosReloadSysTick(void);

void rtosDelay(uint64_t ticks);

void rtosActivateWaitingTask(void);

void rtosKernel(rtosContextSwitchFrom_t from);



int main(void)
{
  gpio_pin_config_t led_config =
  { kGPIO_DigitalOutput, 1 };

  /* Init board hardware. */
  BOARD_InitBootPins();
  BOARD_InitBootClocks();
  BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
  /* Init FSL debug console. */
  BOARD_InitDebugConsole();
#endif
  CLOCK_EnableClock(kCLOCK_PortC);
  PORT_SetPinMux(BOARD_LED_RED_GPIO_PORT, BOARD_LED_RED_PIN, kPORT_MuxAsGpio);
  GPIO_PinInit(BOARD_LED_RED_GPIO, BOARD_LED_RED_PIN, &led_config);

  CLOCK_EnableClock(kCLOCK_PortE);
  PORT_SetPinMux(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN, kPORT_MuxAsGpio);
  GPIO_PinInit(BOARD_LED_GREEN_GPIO, BOARD_LED_GREEN_PIN, &led_config);

  CLOCK_EnableClock(kCLOCK_PortA);
  PORT_SetPinMux(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PIN, kPORT_MuxAsGpio);
  GPIO_PinInit(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_PIN, &led_config);

  task_list.tasks[0].task_body = task0;
  task_list.tasks[0].ID = 0;
  task_list.tasks[0].priority = priority_0;
  task_list.tasks[0].sp = &(task_list.tasks[0].stack[RTOS_STACK_SIZE - 1])
    - STACK_FRAME_SIZE;
  task_list.tasks[0].stack[RTOS_STACK_SIZE - STACK_PC_OFFSET] =
    (uint32_t) task0;
  task_list.tasks[0].stack[RTOS_STACK_SIZE - STACK_PSR_OFFSET] =
    (STACK_PSR_DEFAULT);
  task_list.tasks[0].state = stateReady;
  task_list.nTask++;

  task_list.tasks[1].task_body = task1;
  task_list.tasks[1].ID = 1;
  task_list.tasks[1].priority = priority_2;
  task_list.tasks[1].sp = &(task_list.tasks[1].stack[RTOS_STACK_SIZE - 1])
    - STACK_FRAME_SIZE;
  task_list.tasks[1].stack[RTOS_STACK_SIZE - STACK_PC_OFFSET] =
    (uint32_t) task1;
  task_list.tasks[1].stack[RTOS_STACK_SIZE - STACK_PSR_OFFSET] =
    (STACK_PSR_DEFAULT);
  task_list.tasks[1].state = stateReady;
  task_list.nTask++;

  task_list.tasks[2].task_body = task2;
  task_list.tasks[2].ID = 2;

  task_list.tasks[2].priority = priority_1;
  task_list.tasks[2].sp = &(task_list.tasks[2].stack[RTOS_STACK_SIZE - 1])
    - STACK_FRAME_SIZE;
  task_list.tasks[2].stack[RTOS_STACK_SIZE - STACK_PC_OFFSET] =
    (uint32_t) task2;
  task_list.tasks[2].stack[RTOS_STACK_SIZE - STACK_PSR_OFFSET] =
    (STACK_PSR_DEFAULT);
  task_list.tasks[2].state = stateReady;
  task_list.nTask++;

  /* idle task*/
  task_list.tasks[task_list.nTask].task_body = taskIdel;
  task_list.tasks[task_list.nTask].ID = task_list.nTask;

  task_list.tasks[task_list.nTask].priority = priority_4;
  task_list.tasks[task_list.nTask].sp =
    &(task_list.tasks[task_list.nTask].stack[RTOS_STACK_SIZE - 1])
      - STACK_FRAME_SIZE;
  task_list.tasks[task_list.nTask].stack[RTOS_STACK_SIZE - STACK_PC_OFFSET] =
    (uint32_t) taskIdel;
  task_list.tasks[task_list.nTask].stack[RTOS_STACK_SIZE - STACK_PSR_OFFSET] =
    (STACK_PSR_DEFAULT);
  task_list.tasks[task_list.nTask].state = stateReady;


  NVIC_SetPriority(PendSV_IRQn, 0xFF);

  PRINTF("RTOS Init\n\r");

  while (1)
  {
    rtosStart();

    __asm volatile ("nop");
  }

  return 0;
}

void task0(void) // LED Red
{
  while (1)
  {
    GPIO_PortClear(BOARD_LED_RED_GPIO, 1u << BOARD_LED_RED_GPIO_PIN);
    GPIO_PortSet(BOARD_LED_GREEN_GPIO, 1u << BOARD_LED_GREEN_GPIO_PIN);
    GPIO_PortSet(BOARD_LED_BLUE_GPIO, 1u << BOARD_LED_BLUE_GPIO_PIN);

    PRINTF("Task_0\n\r");

    rtosDelay(10);
  }
}

void task1(void) // LED Green
{
  while (1)
  {
    GPIO_PortClear(BOARD_LED_GREEN_GPIO, 1u << BOARD_LED_GREEN_GPIO_PIN);
    GPIO_PortSet(BOARD_LED_RED_GPIO, 1u << BOARD_LED_RED_GPIO_PIN);
    GPIO_PortSet(BOARD_LED_BLUE_GPIO, 1u << BOARD_LED_BLUE_GPIO_PIN);

    PRINTF("Task_1\n\r");
    rtosDelay(30);
  }
}

void task2(void) // LED Blue
{
  while (1)
  {
    GPIO_PortClear(BOARD_LED_BLUE_GPIO, 1u << BOARD_LED_BLUE_GPIO_PIN);
    GPIO_PortSet(BOARD_LED_RED_GPIO, 1u << BOARD_LED_RED_GPIO_PIN);
    GPIO_PortSet(BOARD_LED_GREEN_GPIO, 1u << BOARD_LED_GREEN_GPIO_PIN);

    PRINTF("Task_2\n\r");
    rtosDelay(20);
  }
}

void taskIdel(void)
{
  for (;;);
}

void rtosStart(void)
{
  task_list.global_tick = 0;
  task_list.current_task = -1;

  SysTick->CTRL =
  SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk
    | SysTick_CTRL_ENABLE_Msk;
  rtosReloadSysTick();
  for (;;)
    ;
}

void rtosReloadSysTick(void)
{
  SysTick->LOAD = (SystemCoreClock / 1e3);
  SysTick->VAL = 0;
}

void rtosDelay(uint64_t ticks)
{
  task_list.tasks[task_list.current_task].state = stateWaiting;
  task_list.tasks[task_list.current_task].local_tick = ticks;
  rtosKernel(from_execution);
}

void rtosActivateWaitingTask(void)
{
  uint8_t idx;
  for (idx = 0; idx < task_list.nTask; idx++)
  {
    if (stateWaiting == task_list.tasks[idx].state)
    {
      task_list.tasks[idx].local_tick--;
      if (0 == task_list.tasks[idx].local_tick)
      {
        task_list.tasks[idx].state = stateReady;
      }
    }
  }
}

void rtosKernel(rtosContextSwitchFrom_t from)
{
  uint8_t nextTask = task_list.nTask;
  uint8_t findNextTask = 0;//task_list.current_task + 1;
  uint8_t foundNextTask = 0;
  priority_t current_priority = priority_0;
  static uint8_t first = 1;
  register uint32_t r0 asm("r0");

  (void) r0;

  /* calendarizador */
  for(findNextTask = 0; foundNextTask == 0; findNextTask++)
  {
    if(findNextTask > task_list.nTask)
    {
      findNextTask = 0;
      current_priority++;
    }
    if(current_priority >  task_list.tasks[task_list.nTask].priority)
    {
      current_priority = priority_0;
    }
    if(current_priority == task_list.tasks[findNextTask].priority)
    {
      if (stateReady == task_list.tasks[findNextTask].state
        || stateRunning == task_list.tasks[findNextTask].state)
      {
        nextTask = findNextTask;

        foundNextTask = 1;
      }
      else if (findNextTask == task_list.current_task)
      {
        foundNextTask = 1;
      }
    }
  }


  task_list.next_task = nextTask;

  /* context swtich */
  if (task_list.current_task != task_list.next_task)
  { // Context switching needed
    if (!first)
    {
      asm("mov r0, r7");
      task_list.tasks[task_list.current_task].sp = (uint32_t*) r0;
      if (from)
      {
        task_list.tasks[task_list.current_task].sp -= (-7);
        task_list.tasks[task_list.current_task].state = stateReady;
      }
      else
      {
        task_list.tasks[task_list.current_task].sp -= (9);
      }
    }
    else
    {
      first = 0;
    }

    task_list.current_task = task_list.next_task;
    task_list.tasks[task_list.current_task].state = stateRunning;
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; // Set PendSV to pending
  }
}

void SysTick_Handler(void) // 1KHz
{
// Increment systick counter for LED blinking
  task_list.global_tick++;

  rtosActivateWaitingTask();

  rtosReloadSysTick();

  rtosKernel(from_isr);
}

void PendSV_Handler(void) // Context switching code
{
  register int32_t r0 asm("r0");
  (void) r0;
  SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;
  r0 = (int32_t) task_list.tasks[task_list.current_task].sp;
  asm("mov r7,r0");
}
