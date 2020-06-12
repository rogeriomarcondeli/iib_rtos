
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/adc_internal.h"
#include "iib_devices/priorities_task_iib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/voltages_tasks/voltages_tasks.h"

/////////////////////////////////////////////////////////////////////////////////////////////

xTaskHandle g_xVoltagesHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void VoltagesTask(void *pvParameters)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        VoltageCh1Sample();

        vTaskDelayUntil(&xLastWakeTime, 1 / portTICK_RATE_MS);

        VoltageCh2Sample();

        vTaskDelayUntil(&xLastWakeTime, 1 / portTICK_RATE_MS);

        VoltageCh3Sample();

        vTaskDelayUntil(&xLastWakeTime, 1 / portTICK_RATE_MS);

        VoltageCh4Sample();

        vTaskDelayUntil(&xLastWakeTime, 1 / portTICK_RATE_MS);
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t VoltagesTaskInit(void)
{
    if(xTaskCreate(VoltagesTask, (const portCHAR *)"VOLTAGES",
                   VOLTAGES_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_VOLTAGES_TASK, &g_xVoltagesHandle) != pdTRUE)
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





