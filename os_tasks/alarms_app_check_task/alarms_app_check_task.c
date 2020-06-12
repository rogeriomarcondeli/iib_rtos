
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/application.h"
#include "iib_devices/priorities_task_iib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/alarms_app_check_task/alarms_app_check_task.h"

/////////////////////////////////////////////////////////////////////////////////////////////

xTaskHandle g_xAlarmsAppCheckHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void AlarmsAppCheckTask(void *pvParameters)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        AlarmAppCheck();

        vTaskDelayUntil(&xLastWakeTime, 1 / portTICK_RATE_MS);
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t AlarmsAppCheckTaskInit(void)
{
    if(xTaskCreate(AlarmsAppCheckTask, (const portCHAR *)"ALARMS_APP_CHECK",
                   ALARMS_APP_CHECK_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_ALARMS_APP_CHECK_TASK, &g_xAlarmsAppCheckHandle) != pdTRUE)
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





