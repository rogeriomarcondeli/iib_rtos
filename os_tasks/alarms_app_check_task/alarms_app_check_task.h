
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __ALARMS_APP_CHECK_TASK_H__
#define __ALARMS_APP_CHECK_TASK_H__

/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#define ALARMS_APP_CHECK_TASK_STACK_SIZE        512         // Stack size in words
#define ALARMS_APP_CHECK_TASK_PERIOD_US         10000       // periodic rate of the task

/////////////////////////////////////////////////////////////////////////////////////////////

extern TaskHandle_t g_xAlarmsAppCheckHandle;

/////////////////////////////////////////////////////////////////////////////////////////////

extern uint32_t AlarmsAppCheckTaskInit(void);

/////////////////////////////////////////////////////////////////////////////////////////////

#endif

/////////////////////////////////////////////////////////////////////////////////////////////




