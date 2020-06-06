
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/adc_internal.h"
#include "iib_devices/priorities_task_iib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/drives_voltage_task/drives_voltage_task.h"

/////////////////////////////////////////////////////////////////////////////////////////////

xTaskHandle g_xDrivesVoltageHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void DrivesVoltageTask(void *pvParameters)
{

    while(1)
    {
        DriverVoltageSample();
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t DrivesVoltageTaskInit(void)
{
    if(xTaskCreate(DrivesVoltageTask, (const portCHAR *)"DRIVES_VOLTAGE",
                   DRIVES_VOLTAGE_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_DRIVES_VOLTAGE_TASK, &g_xDrivesVoltageHandle) != pdTRUE)
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





