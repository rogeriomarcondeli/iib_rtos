
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/application.h"
#include "iib_devices/priorities_task_iib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/leds_indications_status_task/leds_indications_status_task.h"

/////////////////////////////////////////////////////////////////////////////////////////////

TaskHandle_t g_xLedsIndicationsStatusHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void LedsIndicationsStatusTask(void *pvParameters)
{

    while(1)
    {
        LedIndicationStatus();
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t LedsIndicationsStatusTaskInit(void)
{
    if(xTaskCreate(LedsIndicationsStatusTask, (const portCHAR *)"LEDS_INDICATIONS_STATUS",
                   LEDS_INDICATIONS_STATUS_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_LEDS_INDICATIONS_STATUS_TASK, &g_xLedsIndicationsStatusHandle) != pdTRUE)
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




