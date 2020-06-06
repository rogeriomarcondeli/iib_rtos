
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/application.h"
#include "iib_devices/priorities_task_iib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/power_on_check_task/power_on_check_task.h"

/////////////////////////////////////////////////////////////////////////////////////////////

xTaskHandle g_xPowerOnCheckHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void PowerOnCheckTask(void *pvParameters)
{

    while(1)
    {
        power_on_check();
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t PowerOnCheckTaskInit(void)
{
    if(xTaskCreate(PowerOnCheckTask, (const portCHAR *)"POWER_ON_CHECK",
                   POWER_ON_CHECK_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_POWER_ON_CHECK_TASK, &g_xPowerOnCheckHandle) != pdTRUE)
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





