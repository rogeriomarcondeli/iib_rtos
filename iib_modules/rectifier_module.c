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
 * @file rectifier_module.c
 * @brief Brief description of module
 * 
 * Detailed description
 *
 * @author allef.silva
 * @date 20 de out de 2018
 *
 */

#include "iib_modules/rectifier_module.h"

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

#define RM_OUTPUT_OVERCURRENT_RECT1_ITLK_LIM     235.0
#define RM_OUTPUT_OVERCURRENT_RECT1_ALM_LIM      230.0
#define RM_OUTPUT_OVERCURRENT_RECT2_ITLK_LIM     235.0
#define RM_OUTPUT_OVERCURRENT_RECT2_ALM_LIM      230.0
#define RM_OUTPUT_OVERVOLTAGE_RECT1_ITLK_LIM     58.0
#define RM_OUTPUT_OVERVOLTAGE_RECT1_ALM_LIM      55.0
#define RM_OUTPUT_OVERVOLTAGE_RECT2_ITLK_LIM     58.0
#define RM_OUTPUT_OVERVOLTAGE_RECT2_ALM_LIM      55.0
#define RM_LEAKAGE_OVERCURRENT_ITLK_LIM          0.1
#define RM_LEAKAGE_OVERCURRENT_ALM_LIM           0.09
#define RM_HS_OVERTEMP_ITLK_LIM                  80.0
#define RM_HS_OVERTEMP_ALM_LIM                   70.0
#define RM_WATER_OVERTEMP_ITLK_LIM               60.0
#define RM_WATER_OVERTEMP_ALM_LIM                55.0
#define RM_MODULE1_OVERTEMP_ITLK_LIM             115.0
#define RM_MODULE1_OVERTEMP_ALM_LIM              125.0
#define RM_MODULE2_OVERTEMP_ITLK_LIM             125.0
#define RM_MODULE2_OVERTEMP_ALM_LIM              115.0
#define RM_INDUCTOR1_OVERTEMP_ITLK_LIM           60.0
#define RM_INDUCTOR1_OVERTEMP_ALM_LIM            55.0
#define RM_INDUCTOR2_OVERTEMP_ITLK_LIM           60.0
#define RM_INDUCTOR2_OVERTEMP_ALM_LIM            55.0
#define RM_AC_OVERCURRENT_ITLK_LIM               100.0
#define RM_AC_OVERCURRENT_ALM_LIM                100.0
#define RM_RH_ITLK_LIM                           90.0
#define RM_RH_ALM_LIM                            80.0
#define RM_BOARD_TEMP_ITLK_LIM                   90.0
#define RM_BOARD_TEMP_ALM_LIM                    80.0

typedef struct
{
    union {
        float   f;
        uint8_t u8[4];
    } IoutRectf1;

    bool IoutRectf1AlarmSts;
    bool IoutRectf1ItlkSts;

    union {
        float   f;
        uint8_t u8[4];
    } IoutRectf2;

    bool IoutRectf2AlarmSts;
    bool IoutRectf2ItlkSts;

    union {
        float   f;
        uint8_t u8[4];
    } VoutRectf1;

    bool VoutRectf1AlarmSts;
    bool VoutRectf1ItlkSts;

    union {
        float   f;
        uint8_t u8[4];
    } VoutRectf2;

    bool VoutRectf2AlarmSts;
    bool VoutRectf2ItlkSts;

    union {
        float   f;
        uint8_t u8[4];
    } LeakageCurrent;

    bool LeakageCurrentAlarmSts;
    bool LeakageCurrentItlkSts;

    union {
        float   f;
        uint8_t u8[4];
    } TempHeatSink;

    bool TempHeatSinkAlarmSts;
    bool TempHeatSinkItlkSts;

    union {
        float   f;
        uint8_t u8[4];
    } TempWater;

    bool TempWaterAlarmSts;
    bool TempWaterItlkSts;

    union {
        float   f;
        uint8_t u8[4];
    } TempModule1;

    bool TempModule1AlarmSts;
    bool TempModule1ItlkSts;

    union {
        float   f;
        uint8_t u8[4];
    } TempModule2;

    bool TempModule2AlarmSts;
    bool TempModule2ItlkSts;

    union {
        float   f;
        uint8_t u8[4];
    } TempL1;

    bool TempL1AlarmSts;
    bool TempL1ItlkSts;

    union {
        float   f;
        uint8_t u8[4];
    } TempL2;

    bool TempL2AlarmSts;
    bool TempL2ItlkSts;
    bool AcPhaseFault;
    bool AcPhaseFaultSts;
    bool AcOverCurrent;
    bool AcOverCurrentSts;
    bool AcTransformerOverTemp;
    bool AcTransformerOverTempSts;
    bool WaterFluxInterlock;
    bool WaterFluxInterlockSts;

} rectifier_module_t;

rectifier_module_t rectf_module;
uint32_t rectf_module_interlocks_indication = 0;
uint32_t rectf_module_alarms_indication     = 0;

static uint32_t itlk_id;
static uint32_t alarm_id;

static void get_itlks_id();
static void get_alarms_id();
static void map_vars();
static void config_module();

void init_rectifier_module()
{
    config_module();
}

void clear_rectifier_interlocks()
{
    rectf_module.IoutRectf1ItlkSts           = 0;
    rectf_module.IoutRectf2ItlkSts           = 0;
    rectf_module.VoutRectf1ItlkSts           = 0;
    rectf_module.VoutRectf2ItlkSts           = 0;
    rectf_module.LeakageCurrentItlkSts       = 0;
    rectf_module.TempHeatSinkItlkSts         = 0;
    rectf_module.TempWaterItlkSts            = 0;
    rectf_module.TempModule1ItlkSts          = 0;
    rectf_module.TempModule2ItlkSts          = 0;
    rectf_module.TempL1ItlkSts               = 0;
    rectf_module.TempL2ItlkSts               = 0;
    rectf_module.AcPhaseFaultSts             = 0;
    rectf_module.AcOverCurrentSts            = 0;
    rectf_module.AcTransformerOverTempSts    = 0;
    rectf_module.WaterFluxInterlockSts       = 0;

    itlk_id = 0;
}

uint8_t check_rectifier_interlocks()
{
    uint8_t test = 0;

    test |= rectf_module.IoutRectf1ItlkSts;
    test |= rectf_module.IoutRectf2ItlkSts;
    test |= rectf_module.VoutRectf1ItlkSts;
    test |= rectf_module.VoutRectf2ItlkSts;
    test |= rectf_module.LeakageCurrentItlkSts;
    test |= rectf_module.TempHeatSinkItlkSts;
    test |= rectf_module.TempWaterItlkSts;
    test |= rectf_module.TempModule1ItlkSts;
    test |= rectf_module.TempModule2ItlkSts;
    test |= rectf_module.TempL1ItlkSts;
    test |= rectf_module.TempL2ItlkSts;
    test |= rectf_module.AcPhaseFaultSts;
    test |= rectf_module.AcOverCurrentSts;
    test |= rectf_module.AcTransformerOverTempSts;
    test |= rectf_module.WaterFluxInterlockSts;

    return test;
}

void clear_rectifier_alarms()
{
    rectf_module.IoutRectf1AlarmSts          = 0;
    rectf_module.IoutRectf2AlarmSts          = 0;
    rectf_module.VoutRectf1AlarmSts          = 0;
    rectf_module.VoutRectf2AlarmSts          = 0;
    rectf_module.LeakageCurrentAlarmSts      = 0;
    rectf_module.TempHeatSinkAlarmSts        = 0;
    rectf_module.TempWaterAlarmSts           = 0;
    rectf_module.TempModule1AlarmSts         = 0;
    rectf_module.TempModule2AlarmSts         = 0;
    rectf_module.TempL1AlarmSts              = 0;
    rectf_module.TempL2AlarmSts              = 0;

    alarm_id = 0;
}

uint8_t check_rectifier_alarms()
{
    uint8_t test = 0;

    test |= rectf_module.IoutRectf1AlarmSts;
    test |= rectf_module.IoutRectf2AlarmSts;
    test |= rectf_module.VoutRectf1AlarmSts;
    test |= rectf_module.VoutRectf2AlarmSts;
    test |= rectf_module.LeakageCurrentAlarmSts;
    test |= rectf_module.TempHeatSinkAlarmSts;
    test |= rectf_module.TempWaterAlarmSts;
    test |= rectf_module.TempModule1AlarmSts;
    test |= rectf_module.TempModule2AlarmSts;
    test |= rectf_module.TempL1AlarmSts;
    test |= rectf_module.TempL2AlarmSts;

    return test;
}

void check_rectifier_indication_leds()
{
    // Rectifier Output Over Voltage
    if(rectf_module.VoutRectf1ItlkSts || rectf_module.VoutRectf2ItlkSts) Led2TurnOff();
    else if(rectf_module.VoutRectf1AlarmSts || rectf_module.VoutRectf2AlarmSts) Led2Toggle();
    else Led2TurnOn();

    // Rectifier Output Over Current
    if(rectf_module.IoutRectf1ItlkSts || rectf_module.IoutRectf1ItlkSts) Led3TurnOff();
    else if(rectf_module.IoutRectf1AlarmSts || rectf_module.IoutRectf1AlarmSts) Led3Toggle();
    else Led3TurnOn();

    // Rectifier Over Temperature
    if(rectf_module.TempHeatSinkItlkSts || rectf_module.TempWaterItlkSts || rectf_module.TempModule1ItlkSts || rectf_module.TempModule2ItlkSts || rectf_module.TempL1ItlkSts || rectf_module.TempL2ItlkSts) Led4TurnOff();
    else if(rectf_module.TempHeatSinkAlarmSts || rectf_module.TempWaterAlarmSts || rectf_module.TempModule1AlarmSts || rectf_module.TempModule2AlarmSts || rectf_module.TempL1AlarmSts || rectf_module.TempL2AlarmSts) Led4Toggle();
    else Led4TurnOn();

    // External interlock or Driver error
    if(rectf_module.AcPhaseFaultSts || rectf_module.AcOverCurrentSts || rectf_module.AcTransformerOverTempSts || rectf_module.WaterFluxInterlockSts) Led5TurnOff();
    else if(!InterlockRead())Led5TurnOn();
}

void rectifier_application_readings()
{
    rectf_module.IoutRectf1.f = CurrentCh1Read();
    rectf_module.IoutRectf1AlarmSts = CurrentCh1AlarmStatusRead();
    if(!rectf_module.IoutRectf1ItlkSts) rectf_module.IoutRectf1ItlkSts        = CurrentCh1TripStatusRead();

    rectf_module.IoutRectf2.f = CurrentCh2Read();
    rectf_module.IoutRectf2AlarmSts = CurrentCh2AlarmStatusRead();
    if(!rectf_module.IoutRectf2ItlkSts) rectf_module.IoutRectf2ItlkSts        = CurrentCh2TripStatusRead();

    rectf_module.VoutRectf1.f = VoltageCh1Read();
    rectf_module.VoutRectf1AlarmSts = VoltageCh1AlarmStatusRead();
    if(!rectf_module.VoutRectf1ItlkSts) rectf_module.VoutRectf1ItlkSts        = VoltageCh1TripStatusRead();

    rectf_module.VoutRectf2.f = VoltageCh2Read();
    rectf_module.VoutRectf2AlarmSts = VoltageCh2AlarmStatusRead();
    if(!rectf_module.VoutRectf2ItlkSts) rectf_module.VoutRectf2ItlkSts        = VoltageCh2TripStatusRead();

    rectf_module.LeakageCurrent.f = CurrentCh3Read();
    rectf_module.LeakageCurrentAlarmSts = CurrentCh3AlarmStatusRead();
    if(!rectf_module.LeakageCurrentItlkSts) rectf_module.LeakageCurrentItlkSts = CurrentCh3TripStatusRead();

    rectf_module.TempHeatSink.f = (float) Pt100ReadCh1();
    rectf_module.TempHeatSinkAlarmSts = Pt100ReadCh1AlarmSts();
    if(!rectf_module.TempHeatSinkItlkSts) rectf_module.TempHeatSinkItlkSts    = Pt100ReadCh1TripSts();

    rectf_module.TempWater.f = (float) Pt100ReadCh4();
    rectf_module.TempWaterAlarmSts = Pt100ReadCh4AlarmSts();
    if(!rectf_module.TempWaterItlkSts) rectf_module.TempWaterItlkSts          = Pt100ReadCh4TripSts();

    rectf_module.TempModule1.f = VoltageCh3Read();
    rectf_module.TempModule1AlarmSts = VoltageCh3AlarmStatusRead();
    if(!rectf_module.TempModule1ItlkSts) rectf_module.TempModule1ItlkSts      = VoltageCh3TripStatusRead();

    rectf_module.TempModule2.f = VoltageCh4Read();
    rectf_module.TempModule2AlarmSts = VoltageCh4AlarmStatusRead();
    if(!rectf_module.TempModule2ItlkSts) rectf_module.TempModule2ItlkSts      = VoltageCh4TripStatusRead();

    rectf_module.TempL1.f = (float) Pt100ReadCh2();
    rectf_module.TempL1AlarmSts = Pt100ReadCh2AlarmSts();
    if(!rectf_module.TempL1ItlkSts) rectf_module.TempL1ItlkSts                = Pt100ReadCh2TripSts();

    rectf_module.TempL2.f = (float) Pt100ReadCh3();
    rectf_module.TempL2AlarmSts = Pt100ReadCh3AlarmSts();
    if(!rectf_module.TempL2ItlkSts) rectf_module.TempL2ItlkSts                = Pt100ReadCh3TripSts();

    rectf_module.AcPhaseFault = !Gpdi1Read();
    if(!rectf_module.AcPhaseFaultSts) rectf_module.AcPhaseFaultSts            = !Gpdi1Read();

    rectf_module.AcOverCurrent = !Gpdi2Read();
    if(!rectf_module.AcOverCurrentSts) rectf_module.AcOverCurrentSts          = !Gpdi2Read();

    rectf_module.AcTransformerOverTemp = !Gpdi3Read();
    if(!rectf_module.AcTransformerOverTempSts) rectf_module.AcTransformerOverTempSts = !Gpdi3Read();

    rectf_module.WaterFluxInterlock = !Gpdi4Read();
    if(!rectf_module.WaterFluxInterlockSts) rectf_module.WaterFluxInterlockSts = !Gpdi4Read();

    if(rectf_module.AcPhaseFault || rectf_module.AcOverCurrent || rectf_module.AcTransformerOverTemp || rectf_module.WaterFluxInterlock) InterlockSet();

    map_vars();
    get_itlks_id();
    get_alarms_id();
}

static void map_vars()
{
    g_controller_iib.iib_signals[0].u32     = rectf_module_interlocks_indication;
    g_controller_iib.iib_signals[1].u32     = rectf_module_alarms_indication;
    g_controller_iib.iib_signals[2].f       = rectf_module.IoutRectf1.f;
    g_controller_iib.iib_signals[3].f       = rectf_module.IoutRectf2.f;
    g_controller_iib.iib_signals[4].f       = rectf_module.VoutRectf1.f;
    g_controller_iib.iib_signals[5].f       = rectf_module.VoutRectf2.f;
    g_controller_iib.iib_signals[6].f       = rectf_module.LeakageCurrent.f;
    g_controller_iib.iib_signals[7].f       = rectf_module.TempHeatSink.f;
    g_controller_iib.iib_signals[8].f       = rectf_module.TempWater.f;
    g_controller_iib.iib_signals[9].f       = rectf_module.TempModule1.f;
    g_controller_iib.iib_signals[10].f      = rectf_module.TempModule2.f;
    g_controller_iib.iib_signals[11].f      = rectf_module.TempL1.f;
    g_controller_iib.iib_signals[12].f      = rectf_module.TempL2.f;
}

void send_rm_data()
{
    //uint8_t i;
    //for (i = 2; i < 13; i++) send_data_message(i);
    static uint8_t i = 2;

    send_data_message(i);

    i++;

    if (i > 12) i = 2;
}

static void get_itlks_id()
{
    if (rectf_module.IoutRectf1ItlkSts)        itlk_id |= RM_OUTPUT_OVERCURRENT_RECT1_ITLK;
    if (rectf_module.IoutRectf2ItlkSts)        itlk_id |= RM_OUTPUT_OVERCURRENT_RECT2_ITLK;
    if (rectf_module.VoutRectf1ItlkSts)        itlk_id |= RM_OUTPUT_OVERVOLTAGE_RECT1_ITLK;
    if (rectf_module.VoutRectf2ItlkSts)        itlk_id |= RM_OUTPUT_OVERVOLTAGE_RECT2_ITLK;
    if (rectf_module.LeakageCurrentItlkSts)    itlk_id |= RM_LEAKAGE_OVERCURRENT_ITLK;
    if (rectf_module.TempHeatSinkItlkSts)      itlk_id |= RM_HS_OVERTEMP_ITLK;
    if (rectf_module.TempWaterItlkSts)         itlk_id |= RM_WATER_OVERTEMP_ITLK;
    if (rectf_module.TempModule1ItlkSts)       itlk_id |= RM_MODULE1_OVERTEMP_ITLK;
    if (rectf_module.TempModule2ItlkSts)       itlk_id |= RM_MODULE2_OVERTEMP_ITLK;
    if (rectf_module.TempL1ItlkSts)            itlk_id |= RM_INDUCTOR1_OVERTEMP_ITLK;
    if (rectf_module.TempL2ItlkSts)            itlk_id |= RM_INDUCTOR2_OVERTEMP_ITLK;
    if (rectf_module.AcPhaseFaultSts)          itlk_id |= RM_PHASE_FAULT_ITLK;
    if (rectf_module.AcOverCurrentSts)         itlk_id |= RM_AC_OVERCURRENT_ITLK;
    if (rectf_module.AcTransformerOverTempSts) itlk_id |= RM_AC_TRANSF_OVERTEMP_ITLK;
    if (rectf_module.WaterFluxInterlockSts)    itlk_id |= RM_WATER_FLOW_ITLK;
}

static void get_alarms_id()
{
    if (rectf_module.IoutRectf1AlarmSts)      alarm_id |= RM_OUTPUT_OVERCURRENT_RECT1_ALM;
    if (rectf_module.IoutRectf2AlarmSts)      alarm_id |= RM_OUTPUT_OVERCURRENT_RECT2_ALM;
    if (rectf_module.VoutRectf1AlarmSts)      alarm_id |= RM_OUTPUT_OVERVOLTAGE_RECT1_ALM;
    if (rectf_module.VoutRectf2AlarmSts)      alarm_id |= RM_OUTPUT_OVERVOLTAGE_RECT2_ALM;
    if (rectf_module.LeakageCurrentAlarmSts)  alarm_id |= RM_LEAKAGE_OVERCURRENT_ALM;
    if (rectf_module.TempHeatSinkAlarmSts)    alarm_id |= RM_HS_OVERTEMP_ALM;
    if (rectf_module.TempWaterAlarmSts)       alarm_id |= RM_WATER_OVERTEMP_ALM;
    if (rectf_module.TempModule1AlarmSts)     alarm_id |= RM_MODULE1_OVERTEMP_ALM;
    if (rectf_module.TempModule2AlarmSts)     alarm_id |= RM_MODULE2_OVERTEMP_ALM;
    if (rectf_module.TempL1AlarmSts)          alarm_id |= RM_INDUCTOR1_OVERTEMP_ALM;
    if (rectf_module.TempL2AlarmSts)          alarm_id |= RM_INDUCTOR2_OVERTEMP_ALM;
}

void send_rectf_itlk_msg()
{
    send_data_message(0);
}

static void config_module()
{
    //Set current range
    CurrentCh1Init(300.0, 0.150, 50.0, 3);  // Rectifier1 Output Current Sensor Configuration: Hall Sensor
    CurrentCh2Init(300.0, 0.150, 50.0, 3);  // Rectifier2 Output Current Sensor Configuration: LEM LF 310-S
    CurrentCh3Init(1.0, 0.0125, 300.0, 0);  // Leakage Current
    CurrentCh4Init(125.0, 0.125, 50.0, 0);
    //Set protection limits
    CurrentCh1AlarmLevelSet(RM_OUTPUT_OVERCURRENT_RECT1_ALM_LIM); // Rectifier1 Output Current Alarm
    CurrentCh1TripLevelSet(RM_OUTPUT_OVERCURRENT_RECT1_ITLK_LIM); // Rectifier1 Output Current Trip
    CurrentCh2AlarmLevelSet(RM_OUTPUT_OVERCURRENT_RECT2_ALM_LIM); // Rectifier2 Output Current Alarm
    CurrentCh2TripLevelSet(RM_OUTPUT_OVERCURRENT_RECT2_ITLK_LIM); // Rectifier2 Output Current Trip
    CurrentCh3AlarmLevelSet(RM_LEAKAGE_OVERCURRENT_ALM_LIM); // Leakage Current Alarm Level
    CurrentCh3TripLevelSet(RM_LEAKAGE_OVERCURRENT_ITLK_LIM); // Leakage Current Trip Level
    CurrentCh4AlarmLevelSet(RM_AC_OVERCURRENT_ALM_LIM);
    CurrentCh4TripLevelSet(RM_AC_OVERCURRENT_ITLK_LIM);

    //Setar ranges de entrada
    VoltageCh1Init(61.21, 3);              // Rectifier1 Output Voltage Configuration.
    VoltageCh2Init(61.21, 3);              // Rectifier2 Output Voltage Configuration.
    VoltageCh3Init(10.0, 3);               // NTC SW1
    VoltageCh4Init(10.0, 3);               // NTC SW2

    ConfigPolVoltCh2(1);                   // Change the voltage polarity of the channel 2 (rectifier 2 voltage)

    //Setar limites
    VoltageCh1AlarmLevelSet(RM_OUTPUT_OVERVOLTAGE_RECT1_ALM_LIM);  // Rectifier1 Voltage Alarm
    VoltageCh1TripLevelSet(RM_OUTPUT_OVERVOLTAGE_RECT1_ITLK_LIM);  // Rectifier1 Voltage Trip
    VoltageCh2AlarmLevelSet(RM_OUTPUT_OVERVOLTAGE_RECT2_ALM_LIM);  // Rectifier2 Voltage Alarm
    VoltageCh2TripLevelSet(RM_OUTPUT_OVERVOLTAGE_RECT2_ITLK_LIM);  // Rectifier2 Voltage Trip
    VoltageCh3AlarmLevelSet(RM_MODULE1_OVERTEMP_ALM_LIM);          // Rectifier1 NTC Temperature Alarm
    VoltageCh3TripLevelSet(RM_MODULE1_OVERTEMP_ITLK_LIM);          // Rectifier1 NTC Temperature Trip
    VoltageCh4AlarmLevelSet(RM_MODULE2_OVERTEMP_ALM_LIM);          // Rectifier2 NTC Temperature Alarm
    VoltageCh4TripLevelSet(RM_MODULE2_OVERTEMP_ITLK_LIM);          // Rectifier2 NTC Temperature Tip

    // PT100 configuration limits
    Pt100SetCh1AlarmLevel(RM_HS_OVERTEMP_ALM_LIM); // Heat Sink Temperature Alarm
    Pt100SetCh1TripLevel(RM_HS_OVERTEMP_ITLK_LIM); // Heat Sink Temperature Trip
    Pt100SetCh2AlarmLevel(RM_INDUCTOR1_OVERTEMP_ALM_LIM); // L1 Temperature Alarm
    Pt100SetCh2TripLevel(RM_INDUCTOR1_OVERTEMP_ITLK_LIM); // L1 Temperature Trip
    Pt100SetCh3AlarmLevel(RM_INDUCTOR2_OVERTEMP_ALM_LIM); // L2 Temperature Alarm
    Pt100SetCh3TripLevel(RM_INDUCTOR2_OVERTEMP_ITLK_LIM); // L2 Temperature Trip
    Pt100SetCh4AlarmLevel(RM_WATER_OVERTEMP_ALM_LIM);     // Water Temperature Alarm
    Pt100SetCh4TripLevel(RM_WATER_OVERTEMP_ITLK_LIM);     // Water Temperature Trip

    // Delay 4 seconds
    Pt100SetCh1Delay(4);
    // Delay 4 seconds
    Pt100SetCh2Delay(4);
    // Delay 4 seconds
    Pt100SetCh3Delay(4);
    // Delay 4 seconds
    Pt100SetCh4Delay(4);

    // PT100 channel enable
    Pt100Ch1Enable();            // Enable PT100 channel 1
    Pt100Ch2Enable();            // Enable PT100 channel 2
    Pt100Ch3Enable();            // Enable PT100 channel 3
    Pt100Ch4Disable();           // Enable PT100 channel 4

    // Rh configuration limits
    RhAlarmLevelSet(RM_RH_ALM_LIM);
    RhTripLevelSet(RM_RH_ITLK_LIM);

    // Temp board configuration limits
    BoardTempAlarmLevelSet(RM_BOARD_TEMP_ALM_LIM);
    BoardTempTripLevelSet(RM_BOARD_TEMP_ITLK_LIM);

    //Driver1 error configuration
    Driver1TopErrorDisable(); //Desabilitado driver error 1 Top
    Driver1BotErrorDisable(); //Desabilitado driver error 1 Bot
    Driver1OverTempDisable(); //Desabilitado Temperatura por Hardware do modulo 1

    //Driver2 error configuration
    Driver2TopErrorDisable(); //Desabilitado driver error 2 Top
    Driver2BotErrorDisable(); //Desabilitado driver error 2 Bot
    Driver2OverTempDisable(); //Desabilitado Temperatura por Hardware do modulo 2

    // Init variables
    rectf_module.IoutRectf1.f             = 0;
    rectf_module.IoutRectf1AlarmSts       = 0;
    rectf_module.IoutRectf1ItlkSts        = 0;

    rectf_module.IoutRectf2.f             = 0;
    rectf_module.IoutRectf2AlarmSts       = 0;
    rectf_module.IoutRectf2ItlkSts        = 0;

    rectf_module.VoutRectf1.f             = 0;
    rectf_module.VoutRectf1AlarmSts       = 0;
    rectf_module.VoutRectf1ItlkSts        = 0;

    rectf_module.VoutRectf2.f             = 0;
    rectf_module.VoutRectf2AlarmSts       = 0;
    rectf_module.VoutRectf2ItlkSts        = 0;

    rectf_module.LeakageCurrent.f         = 0;
    rectf_module.LeakageCurrentAlarmSts   = 0;
    rectf_module.LeakageCurrentItlkSts    = 0;

    rectf_module.TempHeatSink.f           = 0;
    rectf_module.TempHeatSinkAlarmSts     = 0;
    rectf_module.TempHeatSinkItlkSts      = 0;

    rectf_module.TempWater.f              = 0;
    rectf_module.TempWaterAlarmSts        = 0;
    rectf_module.TempWaterItlkSts         = 0;

    rectf_module.TempModule1.f            = 0;
    rectf_module.TempModule1AlarmSts      = 0;
    rectf_module.TempModule1ItlkSts       = 0;

    rectf_module.TempModule2.f            = 0;
    rectf_module.TempModule2AlarmSts      = 0;
    rectf_module.TempModule2ItlkSts       = 0;

    rectf_module.TempL1.f                 = 0;
    rectf_module.TempL1AlarmSts           = 0;
    rectf_module.TempL1ItlkSts            = 0;

    rectf_module.TempL2.f                 = 0;
    rectf_module.TempL2AlarmSts           = 0;
    rectf_module.TempL2ItlkSts            = 0;

    rectf_module.AcPhaseFault             = 0;
    rectf_module.AcPhaseFaultSts          = 0;

    rectf_module.AcOverCurrent            = 0;
    rectf_module.AcOverCurrentSts         = 0;

    rectf_module.AcTransformerOverTemp    = 0;
    rectf_module.AcTransformerOverTempSts = 0;

    rectf_module.WaterFluxInterlock       = 0;
    rectf_module.WaterFluxInterlockSts    = 0;
}
