/******************************************************************************
 * Copyright (C) 2017 by LNLS - Brazilian Synchrotron Light Laboratory
 *
 * Redistribution, modification or use of this software in source or binary
 * forms is permitted as long as the files maintain this copyright. LNLS and
 * the Brazilian Center for Research in Energy and Materials (CNPEM) are not
 * liable for any misuse of this material.
 *
 *****************************************************************************/

/**
 * @file fac_os.c
 * @brief Brief description of module
 * 
 * Detailed description
 *
 * @author allef.silva
 * @date 28 de ago de 2018
 *
 */

#include "iib_modules/fac_os.h"

#include "iib_devices/iib_data.h"
#include "iib_devices/adc_internal.h"
#include "iib_devices/application.h"
#include "iib_devices/board_temp_hum.h"
#include "iib_devices/pt100.h"
#include "iib_devices/output.h"
#include "iib_devices/leds.h"
#include "iib_devices/can_bus.h"
#include "iib_devices/input.h"

#include <stdbool.h>
#include <stdint.h>

/**
 * TODO: Put here your defines. Just what is local. If you don't
 * need to access it from other module, consider use a constant (const)
 */

#define FAC_OS_INPUT_CURRENT_ALM_LIM        430.0
#define FAC_OS_INPUT_CURRENT_ITLK_LIM       440.0
#define FAC_OS_OUTPUT_CURRENT_ALM_LIM       570.0
#define FAC_OS_OUTPUT_CURRENT_ITLK_LIM      590.0
#define FAC_OS_INPUT_VOLTAGE_ALM_LIM        290.0
#define FAC_OS_INPUT_VOLTAGE_ITLK_LIM       295.0
#define FAC_OS_HS_TEMP_ALM_LIM              40
#define FAC_OS_HS_TEMP_ITLK_LIM             45
#define FAC_OS_INDUC_TEMP_ALM_LIM           40
#define FAC_OS_INDUC_TEMP_ITLK_LIM          45
#define FAC_OS_RH_TEMP_ALM_LIM              80
#define FAC_OS_RH_TEMP_ITLK_LIM             90

typedef struct
{
    union {
       float    f;
       uint8_t u[4];
    } Iin;

    bool IinAlarmSts;
    bool IinItlkSts;

    union {
       float    f;
       uint8_t u[4];
    } Iout;

    bool IoutAlarmSts;
    bool IoutItlkSts;

    union {
       float    f;
       uint8_t u[4];
    } VdcLink;

    bool VdcLinkAlarmSts;
    bool VdcLinkItlkSts;

    union {
       float    f;
       uint8_t u[4];
    } TempIGBT1;

    bool TempIGBT1AlarmSts;
    bool TempIGBT1ItlkSts;
    bool TempIGBT1HwrItlk;
    bool TempIGBT1HwrItlkSts;

    union {
       float    f;
       uint8_t u[4];
    } TempIGBT2;

    bool TempIGBT2AlarmSts;
    bool TempIGBT2ItlkSts;
    bool TempIGBT2HwrItlk;
    bool TempIGBT2HwrItlkSts;

    union {
       float    f;
       uint8_t u[4];
    } TempL;

    bool TempLAlarmSts;
    bool TempLItlkSts;

    union {
       float    f;
       uint8_t u[4];
    } TempHeatSink;

    bool TempHeatSinkAlarmSts;
    bool TempHeatSinkItlkSts;
    bool Driver1Error;
    bool Driver1ErrorItlk;
    bool Driver2Error;
    bool Driver2ErrorItlk;

} fac_os_t;


/**
 * TODO: Put here your constants and variables. Always use static for 
 * private members.
 */


/**
 * TODO: Put here your function prototypes for private functions. Use
 * static in declaration.
 */
fac_os_t fac_os;
uint32_t fac_os_interlocks_indication = 0;
uint32_t fac_os_alarms_indication = 0;

static uint32_t itlk_id;
static uint32_t alarm_id;

static void get_itlks_id();
static void get_alarms_id();
static void map_vars();
static void config_module();

/**
 * TODO: Put here the implementation for your public functions.
 */

void init_fac_os()
{
    config_module();
}

void clear_fac_os_interlocks()
{
    fac_os.IinItlkSts              = 0;
    fac_os.IoutItlkSts             = 0;
    fac_os.VdcLinkItlkSts          = 0;
    fac_os.TempIGBT1ItlkSts        = 0;
    fac_os.TempIGBT1HwrItlkSts     = 0;
    fac_os.TempIGBT2ItlkSts        = 0;
    fac_os.TempIGBT2HwrItlkSts     = 0;
    fac_os.TempLItlkSts            = 0;
    fac_os.TempHeatSinkItlkSts     = 0;
    fac_os.Driver1ErrorItlk        = 0;
    fac_os.Driver2ErrorItlk        = 0;

    itlk_id = 0;
}

uint8_t check_fac_os_interlocks()
{
    uint8_t test = 0;

    test |= fac_os.IinItlkSts;
    test |= fac_os.IoutItlkSts;
    test |= fac_os.VdcLinkItlkSts;
    test |= fac_os.TempIGBT1ItlkSts;
    test |= fac_os.TempIGBT1HwrItlkSts;
    test |= fac_os.TempIGBT2ItlkSts;
    test |= fac_os.TempIGBT2HwrItlkSts;
    test |= fac_os.TempLItlkSts;
    test |= fac_os.TempHeatSinkItlkSts;
    test |= fac_os.Driver1ErrorItlk;
    test |= fac_os.Driver2ErrorItlk;

    return test;
}

void clear_fac_os_alarms(void)
{
    fac_os.IinAlarmSts           = 0;
    fac_os.IoutAlarmSts          = 0;
    fac_os.VdcLinkAlarmSts       = 0;
    fac_os.TempIGBT1AlarmSts     = 0;
    fac_os.TempIGBT2AlarmSts     = 0;
    fac_os.TempLAlarmSts         = 0;
    fac_os.TempHeatSinkAlarmSts  = 0;

    alarm_id = 0;
}

uint8_t check_fac_os_alarms(void)
{
    uint8_t test = 0;

    test |= fac_os.IinAlarmSts;
    test |= fac_os.IoutAlarmSts;
    test |= fac_os.VdcLinkAlarmSts;
    test |= fac_os.TempIGBT1AlarmSts;
    test |= fac_os.TempIGBT2AlarmSts;
    test |= fac_os.TempLAlarmSts;
    test |= fac_os.TempHeatSinkAlarmSts;

    return test;
}

void check_fac_os_indication_leds()
{
    // Input over voltage
    if(fac_os.VdcLinkItlkSts) Led2TurnOff();
    else if(fac_os.VdcLinkAlarmSts) Led2Toggle();
    else Led2TurnOn();

    // Input over current
    if(fac_os.IinItlkSts) Led3TurnOff();
    else if(fac_os.IinAlarmSts) Led3Toggle();
    else Led3TurnOn();

    // Output over current
    if(fac_os.IoutItlkSts) Led4TurnOff();
    else if(fac_os.IoutAlarmSts) Led4Toggle();
    else Led4TurnOn();

    // Output over current
    if(fac_os.Driver1ErrorItlk || fac_os.Driver2ErrorItlk) Led5TurnOff();
    else Led5TurnOn();

    // Heatsink Over temperature
    if(fac_os.TempHeatSinkItlkSts) Led6TurnOff();
    else if(fac_os.TempHeatSinkAlarmSts) Led6Toggle();
    else Led6TurnOn();

    // Inductor Over temperature
    if(fac_os.TempLItlkSts) Led7TurnOff();
    else if(fac_os.TempLAlarmSts) Led7Toggle();
    else Led7TurnOn();
}

void fac_os_application_readings()
{
    fac_os.Iin.f = CurrentCh1Read();
    fac_os.IinAlarmSts = CurrentCh1AlarmStatusRead();
    if(!fac_os.IinItlkSts) fac_os.IinItlkSts = CurrentCh1TripStatusRead();

    fac_os.Iout.f = CurrentCh2Read();
    fac_os.IoutAlarmSts = CurrentCh2AlarmStatusRead();
    if(!fac_os.IoutItlkSts) fac_os.IoutItlkSts = CurrentCh2TripStatusRead();

    fac_os.VdcLink.f = LvCurrentCh1Read();
    fac_os.VdcLinkAlarmSts = LvCurrentCh1AlarmStatusRead();
    if(!fac_os.VdcLinkItlkSts) fac_os.VdcLinkItlkSts = LvCurrentCh1TripStatusRead();

    fac_os.TempHeatSink.f = (float) Pt100ReadCh1();//PT100 CH1
    fac_os.TempHeatSinkAlarmSts = Pt100ReadCh1AlarmSts();
    if(!fac_os.TempHeatSinkItlkSts)fac_os.TempHeatSinkItlkSts = Pt100ReadCh1TripSts();

    fac_os.TempL.f = (float) Pt100ReadCh2();//PT100 CH2
    fac_os.TempLAlarmSts = Pt100ReadCh2AlarmSts();
    if(!fac_os.TempLItlkSts)fac_os.TempLItlkSts = Pt100ReadCh2TripSts();

    map_vars();
    get_itlks_id();
    get_alarms_id();
}

void fac_os_power_on_check()
{
    Led1TurnOn();
}

static void map_vars()
{
    g_controller_iib.iib_signals[0].u32 = fac_os_interlocks_indication;
    g_controller_iib.iib_signals[1].u32 = fac_os_alarms_indication;
    g_controller_iib.iib_signals[2].f   = fac_os.Iin.f;
    g_controller_iib.iib_signals[3].f   = fac_os.Iout.f;
    g_controller_iib.iib_signals[4].f   = fac_os.VdcLink.f;
    g_controller_iib.iib_signals[5].f   = fac_os.TempIGBT1.f;
    g_controller_iib.iib_signals[6].f   = fac_os.TempIGBT2.f;
    g_controller_iib.iib_signals[7].f   = fac_os.TempL.f;
    g_controller_iib.iib_signals[8].f   = fac_os.TempHeatSink.f;
}

void send_fac_os_data()
{
    //uint8_t i;
    //for (i = 2; i < 9; i++) send_data_message(i);
    static uint8_t i = 2;

    send_data_message(i);

    i++;

    if (i > 8) i = 2;
}

static void get_itlks_id()
{
    if (fac_os.IinItlkSts)           itlk_id |= FAC_OS_INPUT_OVERCURRENT_ITLK;
    if (fac_os.IoutItlkSts)          itlk_id |= FAC_OS_OUTPUT_OVERCURRENT_ITLK;
    if (fac_os.VdcLinkItlkSts)       itlk_id |= FAC_OS_INPUT_OVERVOLTAGE_ITLK;
    if (fac_os.TempIGBT1ItlkSts)     itlk_id |= FAC_OS_IGBT1_OVERTEMP_ITLK;
    if (fac_os.TempIGBT1HwrItlkSts)  itlk_id |= FAC_OS_IGBT1_HWR_OVERTEMP_ITLK;
    if (fac_os.TempIGBT2ItlkSts)     itlk_id |= FAC_OS_IGBT2_OVERTEMP_ITLK;
    if (fac_os.TempIGBT2HwrItlkSts)  itlk_id |= FAC_OS_IGBT2_HWR_OVERTEMP_ITLK;
    if (fac_os.Driver1ErrorItlk)     itlk_id |= FAC_OS_DRIVER1_ERROR_ITLK;
    if (fac_os.Driver2ErrorItlk)     itlk_id |= FAC_OS_DRIVER2_ERROR_ITLK;
    if (fac_os.TempLItlkSts)         itlk_id |= FAC_OS_INDUC_OVERTEMP_ITLK;
    if (fac_os.TempHeatSinkItlkSts)  itlk_id |= FAC_OS_HS_OVERTEMP_ITLK;

}

static void get_alarms_id()
{
    if (fac_os.IinAlarmSts)          alarm_id |= FAC_OS_INPUT_OVERCURRENT_ALM;
    if (fac_os.IoutAlarmSts)         alarm_id |= FAC_OS_OUTPUT_OVERCURRENT_ALM;
    if (fac_os.VdcLinkAlarmSts)      alarm_id |= FAC_OS_INPUT_OVERVOLTAGE_ALM;
    if (fac_os.TempIGBT1AlarmSts)    alarm_id |= FAC_OS_IGBT1_OVERTEMP_ALM;
    if (fac_os.TempIGBT2AlarmSts)    alarm_id |= FAC_OS_IGBT2_OVERTEMP_ALM;
    if (fac_os.TempLAlarmSts)        alarm_id |= FAC_OS_INDUC_OVERTEMP_ALM;
    if (fac_os.TempHeatSinkAlarmSts) alarm_id |= FAC_OS_HS_OVERTEMP_ALM;
}

void send_output_fac_os_itlk_msg()
{
    //send_interlock_message(itlk_id);
    send_data_message(0);
}

static void config_module()
{
    /* Set current range */
    CurrentCh1Init(300.0, 0.150, 50.0, 0); /* Input */
    CurrentCh2Init(500.0, 0.100, 50.0, 0); /* Output */

    /* Protection Limits */
    CurrentCh1AlarmLevelSet(FAC_OS_INPUT_CURRENT_ALM_LIM);
    CurrentCh1TripLevelSet(FAC_OS_INPUT_CURRENT_ITLK_LIM);
    CurrentCh2AlarmLevelSet(FAC_OS_OUTPUT_CURRENT_ALM_LIM);
    CurrentCh2TripLevelSet(FAC_OS_OUTPUT_CURRENT_ITLK_LIM);

    /* Isolated Voltage */
    LvCurrentCh1Init(330.0, 0.025, 120.0, 3); /* Input Voltage */

    LvCurrentCh1AlarmLevelSet(FAC_OS_INPUT_VOLTAGE_ALM_LIM);
    LvCurrentCh1TripLevelSet(FAC_OS_INPUT_VOLTAGE_ITLK_LIM);

    /* Pt-100 Configuration Limits */
    Pt100SetCh1AlarmLevel(FAC_OS_HS_TEMP_ALM_LIM);
    Pt100SetCh1TripLevel(FAC_OS_HS_TEMP_ITLK_LIM);
    Pt100SetCh2AlarmLevel(FAC_OS_INDUC_TEMP_ALM_LIM);
    Pt100SetCh2TripLevel(FAC_OS_INDUC_TEMP_ITLK_LIM);

    /* Pt-100 channel enable */
    Pt100Ch1Enable();
    Pt100Ch2Enable();

    /* RH configuration limits */
    BoardTempAlarmLevelSet(80);
    BoardTempTripLevelSet(90);

    //Driver1 error configuration
    Driver1TopErrorDisable(); //Desabilitado driver error 1 Top
    Driver1BotErrorDisable(); //Desabilitado driver error 1 Bot
    Driver1OverTempDisable(); //Desabilitado Temperatura por Hardware do modulo 1

    //Driver2 error configuration
    Driver2TopErrorDisable(); //Desabilitado driver error 2 Top
    Driver2BotErrorDisable(); //Desabilitado driver error 2 Bot
    Driver2OverTempDisable(); //Desabilitado Temperatura por Hardware do modulo 2

    /* Variables */
    fac_os.Iin.f                 = 0;
    fac_os.IinAlarmSts           = 0;
    fac_os.IinItlkSts            = 0;

    fac_os.Iout.f                = 0;
    fac_os.IoutAlarmSts          = 0;
    fac_os.IoutItlkSts           = 0;

    fac_os.VdcLink.f             = 0;
    fac_os.VdcLinkAlarmSts       = 0;
    fac_os.VdcLinkItlkSts        = 0;

    fac_os.TempIGBT1.f           = 0;
    fac_os.TempIGBT1AlarmSts     = 0;
    fac_os.TempIGBT1ItlkSts      = 0;
    fac_os.TempIGBT1HwrItlk      = 0;
    fac_os.TempIGBT1HwrItlkSts   = 0;

    fac_os.TempIGBT2.f           = 0;
    fac_os.TempIGBT2AlarmSts     = 0;
    fac_os.TempIGBT2ItlkSts      = 0;
    fac_os.TempIGBT2HwrItlk      = 0;
    fac_os.TempIGBT2HwrItlkSts   = 0;

    fac_os.TempL.f               = 0;
    fac_os.TempLAlarmSts         = 0;
    fac_os.TempLItlkSts          = 0;

    fac_os.TempHeatSink.f        = 0;
    fac_os.TempHeatSinkAlarmSts  = 0;
    fac_os.TempHeatSinkItlkSts   = 0;

    fac_os.Driver1Error          = 0;
    fac_os.Driver1ErrorItlk      = 0;

    fac_os.Driver2Error          = 0;
    fac_os.Driver2ErrorItlk      = 0;
}
//
