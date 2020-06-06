
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/application.h"
#include "iib_devices/priorities_task_iib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/application_task/application_task.h"

/////////////////////////////////////////////////////////////////////////////////////////////

xTaskHandle g_xApplicationHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void ApplicationTask(void *pvParameters)
{

    while(1)
    {
        Application();
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t ApplicationTaskInit(void)
{
    if(xTaskCreate(ApplicationTask, (const portCHAR *)"APPLICATION",
                   APPLICATION_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_APPLICATION_TASK, &g_xApplicationHandle) != pdTRUE)
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





