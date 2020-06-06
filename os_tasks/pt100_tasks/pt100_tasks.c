
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/pt100.h"
#include "iib_devices/priorities_task_iib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/pt100_tasks/pt100_tasks.h"

/////////////////////////////////////////////////////////////////////////////////////////////

xTaskHandle g_xPt100Handle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void Pt100Task(void *pvParameters)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        Pt100Ch1Sample();

        vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_RATE_MS);

        Pt100Ch2Sample();

        vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_RATE_MS);

        Pt100Ch3Sample();

        vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_RATE_MS);

        Pt100Ch4Sample();

        vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_RATE_MS);
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t Pt100TaskInit(void)
{
    if(xTaskCreate(Pt100Task, (const portCHAR *)"PT100",
                   PT100_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_PT100_TASK, &g_xPt100Handle) != pdTRUE)
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





