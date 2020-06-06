
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/application.h"
#include "iib_devices/priorities_task_iib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/interlocks_app_check_task/interlocks_app_check_task.h"

/////////////////////////////////////////////////////////////////////////////////////////////

xTaskHandle g_xInterlocksAppCheckHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void InterlocksAppCheckTask(void *pvParameters)
{

    while(1)
    {
        InterlockAppCheck();
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t InterlocksAppCheckTaskInit(void)
{
    if(xTaskCreate(InterlocksAppCheckTask, (const portCHAR *)"INTERLOCKS_APP_CHECK",
                   INTERLOCKS_APP_CHECK_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_INTERLOCKS_APP_CHECK_TASK, &g_xInterlocksAppCheckHandle) != pdTRUE)
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





