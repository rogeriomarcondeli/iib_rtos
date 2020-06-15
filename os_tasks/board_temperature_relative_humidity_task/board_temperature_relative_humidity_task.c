
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/board_temp_hum.h"
#include "iib_devices/priorities_task_iib.h"
#include "peripheral_drivers/utils/uartstdio.h"
#include "system/system.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/board_temperature_relative_humidity_task/board_temperature_relative_humidity_task.h"

/////////////////////////////////////////////////////////////////////////////////////////////

xTaskHandle g_xBoardTempRHHandle = NULL;

//UBaseType_t uxBoardTempRHTaskHighWaterMark;

/////////////////////////////////////////////////////////////////////////////////////////////

static void BoardTempRHTask(void *pvParameters)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        BoardTemperatureStartConversion();

        vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_RATE_MS);

        BoardTemperatureRead();

        vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_RATE_MS);

        //uxBoardTempRHTaskHighWaterMark = uxTaskGetStackHighWaterMark( NULL );

        //vPrintStringAndNumber("TASK SIZE :", uxBoardTempRHTaskHighWaterMark);

        //vTaskDelayUntil(&xLastWakeTime, 1000 / portTICK_RATE_MS);

        RelativeHumidityStartConversion();

        vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_RATE_MS);

        RelativeHumidityRead();

        vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_RATE_MS);

        vPrintStringAndNumber("TempBoard :", BoardTempRead());

        vTaskDelayUntil(&xLastWakeTime, 1000 / portTICK_RATE_MS);

        vPrintStringAndNumber("Umidade :", RhRead());

        vTaskDelayUntil(&xLastWakeTime, 1000 / portTICK_RATE_MS);
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t BoardTempRHTaskInit(void)
{
    if(xTaskCreate(BoardTempRHTask, (const portCHAR *)"BOARD_TEMP_RH",
                   BOARD_TEMP_RH_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_BOARD_TEMP_RH_TASK, &g_xBoardTempRHHandle) != pdTRUE)
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





