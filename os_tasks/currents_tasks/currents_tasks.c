
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/adc_internal.h"
#include "iib_devices/priorities_task_iib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/currents_tasks/currents_tasks.h"

/////////////////////////////////////////////////////////////////////////////////////////////

xTaskHandle g_xCurrentsHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void CurrentsTask(void *pvParameters)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        CurrentCh1Sample();

        vTaskDelayUntil(&xLastWakeTime, 1 / portTICK_RATE_US);

        CurrentCh2Sample();

        vTaskDelayUntil(&xLastWakeTime, 1 / portTICK_RATE_US);

        CurrentCh3Sample();

        vTaskDelayUntil(&xLastWakeTime, 1 / portTICK_RATE_US);

        CurrentCh4Sample();

        vTaskDelayUntil(&xLastWakeTime, 1 / portTICK_RATE_US);
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t CurrentsTaskInit(void)
{
    if(xTaskCreate(CurrentsTask, (const portCHAR *)"CURRENTS",
                   CURRENTS_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_CURRENTS_TASK, &g_xCurrentsHandle) != pdTRUE)
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





