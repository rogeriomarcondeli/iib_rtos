
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __POWER_ON_CHECK_TASK_H__
#define __POWER_ON_CHECK_TASK_H__

/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#define POWER_ON_CHECK_TASK_STACK_SIZE        128         // Stack size in words
#define POWER_ON_CHECK_TASK_PERIOD_US         10000       // periodic rate of the task

/////////////////////////////////////////////////////////////////////////////////////////////

extern TaskHandle_t g_xPowerOnCheckHandle;

/////////////////////////////////////////////////////////////////////////////////////////////

extern uint32_t PowerOnCheckTaskInit(void);

/////////////////////////////////////////////////////////////////////////////////////////////

#endif

/////////////////////////////////////////////////////////////////////////////////////////////





