
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/adc_internal.h"
#include "iib_devices/priorities_task_iib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/drives_current_tasks/drives_current_tasks.h"

/////////////////////////////////////////////////////////////////////////////////////////////

TaskHandle_t g_xDrivesCurrentHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void DrivesCurrentTask(void *pvParameters)
{

    while(1)
    {
        Driver1CurrentSample();
        Driver2CurrentSample();
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t DrivesCurrentTaskInit(void)
{
    if(xTaskCreate(DrivesCurrentTask, (const portCHAR *)"DRIVES_CURRENT",
                   DRIVES_CURRENT_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_DRIVES_CURRENT_TASK, &g_xDrivesCurrentHandle) != pdTRUE)
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





