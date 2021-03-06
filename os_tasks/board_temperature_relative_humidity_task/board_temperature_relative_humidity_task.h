
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __BOARD_TEMPERATURE_RELATIVE_HUMIDITY_TASK_H__
#define __BOARD_TEMPERATURE_RELATIVE_HUMIDITY_TASK_H__

/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#define BOARD_TEMP_RH_TASK_STACK_SIZE        128         // Stack size in words
#define BOARD_TEMP_RH_TASK_PERIOD_US         10000       // periodic rate of the task

/////////////////////////////////////////////////////////////////////////////////////////////

extern TaskHandle_t g_xBoardTempRHHandle;

/////////////////////////////////////////////////////////////////////////////////////////////

extern uint32_t BoardTempRHTaskInit(void);

/////////////////////////////////////////////////////////////////////////////////////////////

#endif

/////////////////////////////////////////////////////////////////////////////////////////////





