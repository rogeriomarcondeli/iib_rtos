
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

TaskHandle_t g_xRelativeHumidityHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void RelativeHumidityTask(void *pvParameters)
{

    while(1)
    {
        RelativeHumidityRead();
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





