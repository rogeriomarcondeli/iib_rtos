
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/ntc_isolated_i2c.h"
#include "iib_devices/priorities_task_iib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/ntc_tasks/ntc_tasks.h"

/////////////////////////////////////////////////////////////////////////////////////////////

xTaskHandle g_xNtcHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void NtcTask(void *pvParameters)
{

    while(1)
    {
        NtcRead();
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t NtcTaskInit(void)
{
    if(xTaskCreate(NtcTask, (const portCHAR *)"NTC",
                   NTC_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_NTC_TASK, &g_xNtcHandle) != pdTRUE)
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





