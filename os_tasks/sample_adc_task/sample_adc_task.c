
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "iib_devices/adc_internal.h"
#include "iib_devices/priorities_task_iib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "os_tasks/sample_adc_task/sample_adc_task.h"

/////////////////////////////////////////////////////////////////////////////////////////////

TaskHandle_t g_xSampleAdcHandle = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

static void SampleAdcTask(void *pvParameters)
{

    while(1)
    {
        sample_adc();
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t SampleAdcTaskInit(void)
{
    if(xTaskCreate(SampleAdcTask, (const portCHAR *)"SAMPLE_ADC",
                   SAMPLE_ADC_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_SAMPLE_ADC_TASK, &g_xSampleAdcHandle) != pdTRUE)
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





