//*****************************************************************************
//
// priorities.h - Priorities for the various FreeRTOS tasks.
//
// Copyright (c) 2012-2013 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 1.0 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#ifndef __PRIORITIES_TASK_IIB_H__
#define __PRIORITIES_TASK_IIB_H__

//*****************************************************************************
//
// The priorities of the various tasks.
//
//*****************************************************************************

#define PRIORITY_APPLICATION_TASK               1
#define PRIORITY_VOLTAGES_TASK                  1
#define PRIORITY_CURRENTS_TASK                  1
#define PRIORITY_LV_CURRENTS_TASK               1
#define PRIORITY_DRIVES_VOLTAGE_TASK            1
#define PRIORITY_DRIVES_CURRENT_TASK            1
#define PRIORITY_INTERLOCKS_APP_CHECK_TASK      1
#define PRIORITY_SEND_DATA_SCHEDULE_TASK        1
#define PRIORITY_PT100_TASK                     1
#define PRIORITY_LEDS_INDICATIONS_STATUS_TASK   1
#define PRIORITY_RELATIVE_HUMIDITY_TASK         1
#define PRIORITY_BOARD_TEMPERATURE_TASK         1
#define PRIORITY_NTC_TASK                       1
#define PRIORITY_ERROR_CHECK_HANDLE_TASK        1
#define PRIORITY_ALARMS_APP_CHECK_TASK          1
#define PRIORITY_POWER_ON_CHECK_TASK            1
#define PRIORITY_RUN_TOGGLE_TASK                1
#define PRIORITY_SAMPLE_ADC_TASK                1



#endif // __PRIORITIES_H__
