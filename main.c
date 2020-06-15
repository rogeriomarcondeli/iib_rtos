
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>

#include "system/system.h"
#include "iib_devices/application.h"
#include "iib_devices/task_iib.h"

/////////////////////////////////////////////////////////////////////////////////////////////

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "os_tasks/application_task/application_task.h"
#include "os_tasks/voltages_tasks/voltages_tasks.h"
#include "os_tasks/currents_tasks/currents_tasks.h"
#include "os_tasks/lv_currents_tasks/lv_currents_tasks.h"
#include "os_tasks/drives_voltage_task/drives_voltage_task.h"
#include "os_tasks/drives_current_tasks/drives_current_tasks.h"
#include "os_tasks/interlocks_app_check_task/interlocks_app_check_task.h"
#include "os_tasks/send_data_schedule_task/send_data_schedule_task.h"
#include "os_tasks/pt100_tasks/pt100_tasks.h"
#include "os_tasks/leds_indications_status_task/leds_indications_status_task.h"
#include "os_tasks/board_temperature_relative_humidity_task/board_temperature_relative_humidity_task.h"
#include "os_tasks/ntc_tasks/ntc_tasks.h"
#include "os_tasks/error_check_handle_task/error_check_handle_task.h"
#include "os_tasks/alarms_app_check_task/alarms_app_check_task.h"
#include "os_tasks/power_on_check_task/power_on_check_task.h"
#include "os_tasks/run_toggle_task/run_toggle_task.h"
#include "os_tasks/sample_adc_task/sample_adc_task.h"

/////////////////////////////////////////////////////////////////////////////////////////////

//main.c

int main(void)
{
    init_system();

    AppConfiguration();

/////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Create the application task.
    //
    if(ApplicationTaskInit() != 0)
    {
        while(1)
        {
            //
            // Do Nothing.
            //
        }
    }

/////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Create the sample adc task.
    //
    /*if(SampleAdcTaskInit() != 0)
    {
        while(1)
        {
            //
            // Do Nothing.
            //
        }
    }*/

/////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Create the voltages tasks.
    //
    /*if(VoltagesTaskInit() != 0)
    {
        while(1)
        {
            //
            // Do Nothing.
            //
        }
    }*/

/////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Create the currents tasks.
    //
    /*if(CurrentsTaskInit() != 0)
    {
        while(1)
        {
            //
            // Do Nothing.
            //
        }
    }*/

/////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Create the lv_currents tasks.
    //
    /*if(LvCurrentsTaskInit() != 0)
    {
        while(1)
        {
            //
            // Do Nothing.
            //
        }
    }*/

/////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Create the drives voltage task.
    //
    /*if(DrivesVoltageTaskInit() != 0)
    {
        while(1)
        {
            //
            // Do Nothing.
            //
        }
    }*/

/////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Create the drives current task.
    //
    /*if(DrivesCurrentTaskInit() != 0)
    {
        while(1)
        {
            //
            // Do Nothing.
            //
        }
    }*/

/////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Create the interlocks app check task.
    //
    if(InterlocksAppCheckTaskInit() != 0)
    {
        while(1)
        {
            //
            // Do Nothing.
            //
        }
    }

/////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Create the leds indications status task.
    //
    if(LedsIndicationsStatusTaskInit() != 0)
    {
        while(1)
        {
            //
            // Do Nothing.
            //
        }
    }

/////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Create the send data schedule task.
    //
    /*if(SendDataScheduleTaskInit() != 0)
    {
        while(1)
        {
            //
            // Do Nothing.
            //
        }
    }*/

/////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Create the pt100 tasks.
    //
    /*if(Pt100TaskInit() != 0)
    {
        while(1)
        {
            //
            // Do Nothing.
            //
        }
    }

/////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Create the error check handle task.
    //
    if(ErrorCheckHandleTaskInit() != 0)
    {
        while(1)
        {
            //
            // Do Nothing.
            //
        }
    }*/

/////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Create the board temperature and relative humidity task.
    //
    if(BoardTempRHTaskInit() != 0)
    {
        while(1)
        {
            //
            // Do Nothing.
            //
        }
    }

/////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Create the ntc tasks.
    //
    /*if(NtcTaskInit() != 0)
    {
        while(1)
        {
            //
            // Do Nothing.
            //
        }
    }*/

/////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Create the alarms app check task.
    //
    if(AlarmsAppCheckTaskInit() != 0)
    {
        while(1)
        {
            //
            // Do Nothing.
            //
        }
    }
/////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Create the power on check task.
    //
    if(PowerOnCheckTaskInit() != 0)
    {
        while(1)
        {
            //
            // Do Nothing.
            //
        }
    }

/////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Create the run toggle task.
    //
    if(RunToggleTaskInit() != 0)
    {
        while(1)
        {
            //
            // Do Nothing.
            //
        }
    }

/////////////////////////////////////////////////////////////////////////////////////////////

    //
    // Start the scheduler.  This should not return.
    //
    vTaskStartScheduler();

    while(1)
    {
        //
        // Do Nothing.
        //
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////





