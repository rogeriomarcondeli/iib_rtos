
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/board_temp_hum.h"
#include "iib_devices/priorities_task_iib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/board_temperature_task/board_temperature_task.h"

/////////////////////////////////////////////////////////////////////////////////////////////

xTaskHandle g_xBoardTemperatureHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void BoardTemperatureTask(void *pvParameters)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
       BoardTemperatureStartConversion();

       vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_RATE_MS);

       BoardTemperatureRead();

       vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_RATE_MS);

       RelativeHumidityStartConversion();

       vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_RATE_MS);

       RelativeHumidityRead();

       vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_RATE_MS);
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t BoardTemperatureTaskInit(void)
{
    if(xTaskCreate(BoardTemperatureTask, (const portCHAR *)"BOARD_TEMPERATURE",
                   BOARD_TEMPERATURE_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_BOARD_TEMPERATURE_TASK, &g_xBoardTemperatureHandle) != pdTRUE)
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





