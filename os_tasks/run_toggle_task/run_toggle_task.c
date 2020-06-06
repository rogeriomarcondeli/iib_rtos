
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/leds.h"
#include "iib_devices/priorities_task_iib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/run_toggle_task/run_toggle_task.h"

/////////////////////////////////////////////////////////////////////////////////////////////

xTaskHandle g_xRunToggleHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void RunToggleTask(void *pvParameters)
{

    while(1)
    {
        RunToggle();
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t RunToggleTaskInit(void)
{
    if(xTaskCreate(RunToggleTask, (const portCHAR *)"RUN_TOGGLE",
                   RUN_TOGGLE_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_RUN_TOGGLE_TASK, &g_xRunToggleHandle) != pdTRUE)
    {
        //
        // Task creation failed.
        //
        return(1);
    }

    //
    // Success.
    //
    return(0);

}

/////////////////////////////////////////////////////////////////////////////////////////////





