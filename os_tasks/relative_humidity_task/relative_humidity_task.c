
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/board_temp_hum.h"
#include "iib_devices/priorities_task_iib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/relative_humidity_task/relative_humidity_task.h"

/////////////////////////////////////////////////////////////////////////////////////////////

xTaskHandle g_xRelativeHumidityHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void RelativeHumidityTask(void *pvParameters)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        RelativeHumidityRead();

        vTaskDelayUntil(&xLastWakeTime, 1 / portTICK_RATE_MS);
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t RelativeHumidityTaskInit(void)
{
    if(xTaskCreate(RelativeHumidityTask, (const portCHAR *)"RELATIVE_HUMIDITY",
                   RELATIVE_HUMIDITY_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_RELATIVE_HUMIDITY_TASK, &g_xRelativeHumidityHandle) != pdTRUE)
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





