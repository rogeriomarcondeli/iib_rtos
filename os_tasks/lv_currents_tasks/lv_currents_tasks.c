
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/adc_internal.h"
#include "iib_devices/priorities_task_iib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/lv_currents_tasks/lv_currents_tasks.h"

/////////////////////////////////////////////////////////////////////////////////////////////

xTaskHandle g_xLvCurrentsHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void LvCurrentsTask(void *pvParameters)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        LvCurrentCh1Sample();

        vTaskDelayUntil(&xLastWakeTime, 1 / portTICK_RATE_MS);

        LvCurrentCh2Sample();

        vTaskDelayUntil(&xLastWakeTime, 1 / portTICK_RATE_MS);

        LvCurrentCh3Sample();

        vTaskDelayUntil(&xLastWakeTime, 1 / portTICK_RATE_MS);
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t LvCurrentsTaskInit(void)
{
    if(xTaskCreate(LvCurrentsTask, (const portCHAR *)"LV_CURRENTS",
                   LV_CURRENTS_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_LV_CURRENTS_TASK, &g_xLvCurrentsHandle) != pdTRUE)
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






