
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __LEDS_INDICATIONS_STATUS_TASK_H__
#define __LEDS_INDICATIONS_STATUS_TASK_H__

/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#define LEDS_INDICATIONS_STATUS_TASK_STACK_SIZE        128         // Stack size in words
#define LEDS_INDICATIONS_STATUS_TASK_PERIOD_US         10000       // periodic rate of the task

/////////////////////////////////////////////////////////////////////////////////////////////

extern TaskHandle_t g_xLedsIndicationsStatusHandle;

/////////////////////////////////////////////////////////////////////////////////////////////

extern uint32_t LedsIndicationsStatusTaskInit(void);

/////////////////////////////////////////////////////////////////////////////////////////////

#endif

/////////////////////////////////////////////////////////////////////////////////////////////





