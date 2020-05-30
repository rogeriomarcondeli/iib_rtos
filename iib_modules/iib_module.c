/*
 * iib_module.c
 *
 *  Created on: 26 de fev de 2019
 *      Author: allef.silva
 */

/////////////////////////////////////////////////////////////////////////////////////////////

#include "iib_modules/iib_module.h"

/////////////////////////////////////////////////////////////////////////////////////////////

iib_module_t g_iib_module;

/////////////////////////////////////////////////////////////////////////////////////////////

void init_iib_module(iib_module_t *iib_module,
                     void (*clear_interlocks) (void),
                     void (*check_interlocks) (void),
                     void (*clear_alarms) (void),
                     void (*check_alarms) (void),
                     void (*check_indication_leds) (void),
                     void (*application_readings) (void),
                     void (*power_on_check) (void),
                     void (*send_data) (void))
{
    iib_module->clear_interlocks        = clear_interlocks;
    iib_module->check_interlocks        = check_interlocks;
    iib_module->clear_alarms            = clear_alarms;
    iib_module->check_alarms            = check_alarms;
    iib_module->check_indication_leds   = check_indication_leds;
    iib_module->application_readings    = application_readings;
    iib_module->power_on_check          = power_on_check;
    iib_module->send_data               = send_data;
}

/////////////////////////////////////////////////////////////////////////////////////////////



