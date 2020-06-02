
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __RUN_TOGGLE_TASK_H__
#define __RUN_TOGGLE_TASK_H__

/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#define RUN_TOGGLE_TASK_STACK_SIZE        512         // Stack size in words
#define RUN_TOGGLE_TASK_PERIOD_US         10000       // periodic rate of the task

/////////////////////////////////////////////////////////////////////////////////////////////

extern TaskHandle_t g_xRunToggleHandle;

/////////////////////////////////////////////////////////////////////////////////////////////

extern uint32_t RunToggleTaskInit(void);

/////////////////////////////////////////////////////////////////////////////////////////////

#endif

/////////////////////////////////////////////////////////////////////////////////////////////





