
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/pt100.h"
#include "iib_devices/priorities_task_iib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/error_check_handle_task/error_check_handle_task.h"

/////////////////////////////////////////////////////////////////////////////////////////////

xTaskHandle g_xErrorCheckHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void ErrorCheckHandleTask(void *pvParameters)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        ErrorCheckHandle();

        vTaskDelayUntil(&xLastWakeTime, 10 / portTICK_RATE_US);
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t ErrorCheckHandleTaskInit(void)
{
    if(xTaskCreate(ErrorCheckHandleTask, (const portCHAR *)"ERROR_CHECK_HANDLE",
                   ERROR_CHECK_HANDLE_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_ERROR_CHECK_HANDLE_TASK, &g_xErrorCheckHandle) != pdTRUE)
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





