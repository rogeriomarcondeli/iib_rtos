
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/application.h"
#include "iib_devices/priorities_task_iib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/send_data_schedule_task/send_data_schedule_task.h"

/////////////////////////////////////////////////////////////////////////////////////////////

xTaskHandle g_xSendDataScheduleHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void SendDataScheduleTask(void *pvParameters)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        send_data_schedule();

        vTaskDelayUntil(&xLastWakeTime, 50 / portTICK_RATE_MS);
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t SendDataScheduleTaskInit(void)
{
    if(xTaskCreate(SendDataScheduleTask, (const portCHAR *)"SEND_DATA_SCHEDULE",
                   SEND_DATA_SCHEDULE_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_SEND_DATA_SCHEDULE_TASK, &g_xSendDataScheduleHandle) != pdTRUE)
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





