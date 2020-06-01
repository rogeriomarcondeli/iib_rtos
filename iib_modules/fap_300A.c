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
 * @file fap_300A.c
 * @brief Brief description of module
 * 
 * Detailed description
 *
 * @author allef.silva
 * @date 20 de out de 2018
 *
 */

#include "iib_modules/fap_300A.h"

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
#define FAP_INPUT_OVERVOLTAGE_ALM_LIM       55.0
#define FAP_INPUT_OVERVOLTAGE_ITLK_LIM      60.0
#define FAP_OUTPUT_OVERVOLTAGE_ALM_LIM      17.0
#define FAP_OUTPUT_OVERVOLTAGE_ITLK_LIM     20.0
#define FAP_OUTPUT_OVERCURRENT_1_ALM_LIM    151.0
#define FAP_OUTPUT_OVERCURRENT_1_ITLK_LIM   152.0
#define FAP_OUTPUT_OVERCURRENT_2_ALM_LIM    151.5
#define FAP_OUTPUT_OVERCURRENT_2_ITLK_LIM   152.0
#define FAP_GROUND_LEAKAGE_ITLK_LIM         50.0
#define FAP_GROUND_LEAKAGE_ALM_LIM          45.0
#define FAP_IGBT1_OVERTEMP_ALM_LIM          00.0    // Not in use
#define FAP_IGBT1_OVERTEMP_ITLK_LIM         00.0    // Not in use
#define FAP_IGBT2_OVERTEMP_ALM_LIM          00.0    // Not in use
#define FAP_IGBT2_OVERTEMP_ITLK_LIM         00.0    // Not in use
#define FAP_DRIVER_OVERVOLTAGE_ALM_LIM      00.0    // Not in use
#define FAP_DRIVER_OVERVOLTAGE_ITLK_LIM     00.0    // Not in use
#define FAP_DRIVER1_OVERCURRENT_ALM_LIM     00.0    // Not in use
#define FAP_DRIVER1_OVERCURRENT_ITLK_LIM    00.0    // Not in use
#define FAP_DRIVER2_OVERCURRENT_ALM_LIM     00.0    // Not in use
#define FAP_DRIVER2_OVERCURRENT_ITLK_LIM    00.0    // Not in use
#define FAP_INDUC_OVERTEMP_ALM_LIM          50.0
#define FAP_INDUC_OVERTEMP_ITLK_LIM         60.0
#define FAP_HS_OVERTEMP_ALM_LIM             80
#define FAP_HS_OVERTEMP_ITLK_LIM            60
#define FAP_RH_ALM_LIM                      80
#define FAP_RH_ITLK_LIM                     90
#define FAP_BOARD_TEMP_ALM_LIM              80
#define FAP_BOARD_TEMP_ITLK_LIM             90

typedef struct
{
    union {
        float   f;
        uint8_t u8[4];
    } Vin;

    bool VinAlarmSts;
    bool VinItlkSts;

    union {
        float   f;
        uint8_t u8[4];
    } Vout;

    bool VoutAlarmSts;
    bool VoutItlkSts;

    union {
        float   f;
        uint8_t u8[4];
    } IoutA1;

    bool IoutA1AlarmSts;
    bool IoutA1ItlkSts;

    union {
        float   f;
        uint8_t u8[4];
    } IoutA2;

    bool IoutA2AlarmSts;
    bool IoutA2ItlkSts;

    union {
        float f;
        uint8_t u8[4];
    } GroundLeakage;

    bool GroundLeakageItlkSts;
    bool GroundLeakageAlarmSts;

    union {
        float   f;
        uint8_t u8[4];
    } TempIGBT1;

    bool TempIGBT1AlarmSts;
    bool TempIGBT1ItlkSts;
    bool TempIGBT1HwrItlk;
    bool TempIGBT1HwrItlkSts;

    union {
        float   f;
        uint8_t u8[4];
    } TempIGBT2;

    bool TempIGBT2AlarmSts;
    bool TempIGBT2ItlkSts;
    bool TempIGBT2HwrItlk;
    bool TempIGBT2HwrItlkSts;

    union {
        float   f;
        uint8_t u[4];
    } DriveVoltage;

    union {
        float   f;
        uint8_t u[4];
    } Drive1Current;

    union {
        float   f;
        uint8_t u[4];
    } Drive2Current;

    bool Driver1Error;
    bool Driver1ErrorItlk;
    bool Driver2Error;
    bool Driver2ErrorItlk;

    union {
        float   f;
        uint8_t u[4];
    } TempL;

    bool TempLAlarmSts;
    bool TempLItlkSts;

    union {
        float   f;
        uint8_t u[4];
    } TempHeatSink;

    bool TempHeatSinkAlarmSts;
    bool TempHeatSinkItlkSts;
    bool Relay;
    bool ExternalItlk;
    bool ExternalItlkSts;
    bool LeakageCurrent;
    bool LeakageCurrentSts;
    bool Rack;
    bool RackSts;
} fap_300A_t;

/**
 * TODO: Put here your constants and variables. Always use static for 
 * private members.
 */

/**
 * TODO: Put here your function prototypes for private functions. Use
 * static in declaration.
 */
fap_300A_t fap_300A;
uint32_t fap_300A_interlocks_indication   = 0;
uint32_t fap_300A_alarms_indication       = 0;

static uint32_t itlk_id;
static uint32_t alarm_id;

static void get_itlks_id();
static void get_alarms_id();
static void map_vars();
static void config_module();

/**
 * TODO: Put here the implementation for your public functions.
 */

void init_fap_300A()
{
    config_module();
}

void clear_fap_300A_interlocks()
{
    fap_300A.VinItlkSts             = 0;
    fap_300A.VoutItlkSts            = 0;
    fap_300A.IoutA1ItlkSts          = 0;
    fap_300A.IoutA2ItlkSts          = 0;
    fap_300A.TempIGBT1ItlkSts       = 0;
    fap_300A.TempIGBT1HwrItlkSts    = 0;
    fap_300A.TempIGBT2ItlkSts       = 0;
    fap_300A.TempIGBT2HwrItlkSts    = 0;
    fap_300A.Driver1ErrorItlk       = 0;
    fap_300A.Driver2ErrorItlk       = 0;
    fap_300A.TempLItlkSts           = 0;
    fap_300A.TempHeatSinkItlkSts    = 0;
    fap_300A.ExternalItlkSts        = 0;
    fap_300A.LeakageCurrentSts      = 0;
    fap_300A.RackSts                = 0;
    fap_300A.GroundLeakageItlkSts   = 0;

    itlk_id = 0;
}

uint8_t check_fap_300A_interlocks()
{
    uint8_t test = 0;

    test |= fap_300A.VinItlkSts;
    test |= fap_300A.VoutItlkSts;
    test |= fap_300A.IoutA1ItlkSts;
    test |= fap_300A.IoutA2ItlkSts;
    test |= fap_300A.TempIGBT1ItlkSts;
    test |= fap_300A.TempIGBT1HwrItlkSts;
    test |= fap_300A.TempIGBT2ItlkSts;
    test |= fap_300A.TempIGBT2HwrItlkSts;
    test |= fap_300A.Driver1ErrorItlk;
    test |= fap_300A.Driver2ErrorItlk;
    test |= fap_300A.TempLItlkSts;
    test |= fap_300A.TempHeatSinkItlkSts;
    test |= fap_300A.ExternalItlkSts;
    test |= fap_300A.LeakageCurrentSts;
    test |= fap_300A.RackSts;
    test |= fap_300A.GroundLeakageItlkSts;

    return test;
}

void clear_fap_300A_alarms()
{
    fap_300A.VinAlarmSts             = 0;
    fap_300A.VoutAlarmSts            = 0;
    fap_300A.IoutA1AlarmSts          = 0;
    fap_300A.IoutA2AlarmSts          = 0;
    fap_300A.TempLAlarmSts           = 0;
    fap_300A.TempHeatSinkAlarmSts    = 0;
    fap_300A.GroundLeakageAlarmSts  = 0;

    alarm_id = 0;
}

uint8_t check_fap_300A_alarms()
{
    uint8_t test = 0;

    test |= fap_300A.VinAlarmSts;
    test |= fap_300A.VoutAlarmSts;
    test |= fap_300A.IoutA1AlarmSts;
    test |= fap_300A.IoutA2AlarmSts;
    test |= fap_300A.TempIGBT1AlarmSts;
    test |= fap_300A.TempIGBT2AlarmSts;
    test |= fap_300A.TempLAlarmSts;
    test |= fap_300A.TempHeatSinkAlarmSts;
    test |= fap_300A.GroundLeakageAlarmSts;

    return test;
}

void check_fap_300A_indication_leds()
{
    // Output over voltage
    if(fap_300A.VoutItlkSts) Led2TurnOff();
    else if(fap_300A.VoutAlarmSts) Led2Toggle();
    else Led2TurnOn();

    // Input over voltage
    if(fap_300A.VinItlkSts) Led3TurnOff();
    else if(fap_300A.VinAlarmSts) Led3Toggle();
    else Led3TurnOn();

    // Output over current
    if (fap_300A.IoutA1ItlkSts || fap_300A.IoutA2ItlkSts) Led4TurnOff();
    else if(fap_300A.IoutA1AlarmSts || fap_300A.IoutA2AlarmSts) Led4Toggle();
    else Led4TurnOn();

    // Over temperature
    if(fap_300A.TempIGBT1ItlkSts || fap_300A.TempIGBT2ItlkSts ||  fap_300A.TempLItlkSts || fap_300A.TempHeatSinkItlkSts || fap_300A.TempIGBT1HwrItlkSts || fap_300A.TempIGBT2HwrItlkSts) Led5TurnOff();
    else if(fap_300A.TempIGBT1AlarmSts || fap_300A.TempIGBT2AlarmSts ||  fap_300A.TempLAlarmSts || fap_300A.TempHeatSinkAlarmSts) Led5Toggle();
    else Led5TurnOn();

    if(fap_300A.ExternalItlkSts) Led6TurnOff();
    else Led6TurnOn();

    if(fap_300A.LeakageCurrentSts) Led7TurnOff();
    else Led7TurnOn();

    if(fap_300A.RackSts) Led8TurnOff();
    else Led8TurnOn();

    //if(fap_300A.Driver1ErrorItlk || fap_300A.Driver2ErrorItlk) Led9TurnOff();
    //else if(!InterlockRead()) Led9TurnOn();

    if(fap_300A.GroundLeakageItlkSts) Led9TurnOff();
    else if(fap_300A.GroundLeakageAlarmSts) Led9Toggle();
    else Led9TurnOn();

    if(InterlockRead()) Led10TurnOff();
    else Led10TurnOn();
}

void fap_300A_application_readings()
{
    fap_300A.TempHeatSink.f = (float) Pt100ReadCh1();//PT100 CH1
    fap_300A.TempHeatSinkAlarmSts = Pt100ReadCh1AlarmSts();
    if(!fap_300A.TempHeatSinkItlkSts)fap_300A.TempHeatSinkItlkSts        = Pt100ReadCh1TripSts();

    fap_300A.TempL.f = (float) Pt100ReadCh2();//PT100 CH2
    fap_300A.TempLAlarmSts = Pt100ReadCh2AlarmSts();
    if(!fap_300A.TempLItlkSts)fap_300A.TempLItlkSts                      = Pt100ReadCh2TripSts();

    fap_300A.TempIGBT1.f = 0.0;
    fap_300A.TempIGBT1AlarmSts = 0;
    fap_300A.TempIGBT1ItlkSts = 0;

    if(!fap_300A.TempIGBT1HwrItlkSts) fap_300A.TempIGBT1HwrItlkSts       = Driver1OverTempRead();

    fap_300A.TempIGBT2.f = 0.0;
    fap_300A.TempIGBT2AlarmSts = 0;
    fap_300A.TempIGBT2ItlkSts = 0;

    if(!fap_300A.TempIGBT2HwrItlkSts) fap_300A.TempIGBT2HwrItlkSts       = Driver2OverTempRead();

    fap_300A.IoutA1.f = CurrentCh1Read();//HALL CH1
    fap_300A.IoutA1AlarmSts = CurrentCh1AlarmStatusRead();
    if(!fap_300A.IoutA1ItlkSts)fap_300A.IoutA1ItlkSts                    = CurrentCh1TripStatusRead();

    fap_300A.IoutA2.f = CurrentCh2Read();//HALL CH2
    fap_300A.IoutA2AlarmSts = CurrentCh2AlarmStatusRead();
    if(!fap_300A.IoutA2ItlkSts)fap_300A.IoutA2ItlkSts                    = CurrentCh2TripStatusRead();

    fap_300A.Vin.f = LvCurrentCh1Read();
    fap_300A.VinAlarmSts = LvCurrentCh1AlarmStatusRead();
    if(!fap_300A.VinItlkSts)fap_300A.VinItlkSts                          = LvCurrentCh1TripStatusRead();

    fap_300A.Vout.f = LvCurrentCh2Read();
    fap_300A.VoutAlarmSts = LvCurrentCh2AlarmStatusRead();
    if(!fap_300A.VoutItlkSts)fap_300A.VoutItlkSts                        = LvCurrentCh2TripStatusRead();

    fap_300A.GroundLeakage.f = LvCurrentCh3Read();
    fap_300A.GroundLeakageAlarmSts = LvCurrentCh3AlarmStatusRead();
    if(!fap_300A.GroundLeakageItlkSts) fap_300A.GroundLeakageItlkSts     = LvCurrentCh3TripStatusRead();

    fap_300A.ExternalItlk = Gpdi1Read();
    if(!fap_300A.ExternalItlkSts) fap_300A.ExternalItlkSts               = Gpdi1Read();

    fap_300A.LeakageCurrent = Gpdi2Read();
    if(!fap_300A.LeakageCurrentSts) fap_300A.LeakageCurrentSts           = Gpdi2Read();

    fap_300A.Rack = Gpdi3Read();
    if(!fap_300A.RackSts) fap_300A.RackSts                               = Gpdi3Read();

    fap_300A.Relay = Gpdi4Read();

    fap_300A.Driver1Error = Driver1TopErrorRead();
    if(!fap_300A.Driver1ErrorItlk) fap_300A.Driver1ErrorItlk             = Driver1TopErrorRead();

    fap_300A.Driver2Error = Driver2TopErrorRead();
    if(!fap_300A.Driver2ErrorItlk) fap_300A.Driver2ErrorItlk             = Driver2TopErrorRead();

    if(fap_300A.ExternalItlkSts || fap_300A.Driver2ErrorItlk || fap_300A.Driver2ErrorItlk) InterlockSet(); // If no signal over the port, then set Interlock action

    map_vars();
    get_itlks_id();
    get_alarms_id();
}

void fap_300A_power_on_check()
{
    if (Gpdi4Read()) {
        Led1TurnOff();
        ReleExtItlkTurnOff();
    }
    else {
        Led1TurnOn();
        ReleExtItlkTurnOn();
    }
}

static void map_vars()
{
    g_controller_iib.iib_signals[0].u32     = fap_300A_interlocks_indication;
    g_controller_iib.iib_signals[1].u32     = fap_300A_alarms_indication;
    g_controller_iib.iib_signals[2].f       = fap_300A.Vin.f;
    g_controller_iib.iib_signals[3].f       = fap_300A.Vout.f;
    g_controller_iib.iib_signals[4].f       = fap_300A.IoutA1.f;
    g_controller_iib.iib_signals[5].f       = fap_300A.IoutA2.f;
    g_controller_iib.iib_signals[6].f       = fap_300A.TempIGBT1.f;
    g_controller_iib.iib_signals[7].f       = fap_300A.TempIGBT2.f;
    g_controller_iib.iib_signals[8].f       = fap_300A.DriveVoltage.f;
    g_controller_iib.iib_signals[9].f       = fap_300A.Drive1Current.f;
    g_controller_iib.iib_signals[10].f      = fap_300A.Drive2Current.f;
    g_controller_iib.iib_signals[11].f      = fap_300A.TempL.f;
    g_controller_iib.iib_signals[12].f      = fap_300A.TempHeatSink.f;
    g_controller_iib.iib_signals[13].f      = fap_300A.GroundLeakage.f;
}

void send_fap_300A_data()
{
    //uint8_t i;
    //for (i = 2; i < 13; i++) send_data_message(i);
    static uint8_t i = 2;

    send_data_message(i);

    i++;

    if (i > 13) i = 2;
}

static void get_itlks_id()
{
    if (fap_300A.VinItlkSts)            itlk_id |= FAP_300A_INPUT_OVERVOLTAGE_ITLK;
    if (fap_300A.VoutItlkSts)           itlk_id |= FAP_300A_OUTPUT_OVERVOLTAGE_ITLK;
    if (fap_300A.IoutA1ItlkSts)         itlk_id |= FAP_300A_OUTPUT_OVERCURRENT_1_ITLK;
    if (fap_300A.IoutA2ItlkSts)         itlk_id |= FAP_300A_OUTPUT_OVERCURRENT_2_ITLK;
    if (fap_300A.TempIGBT1ItlkSts)      itlk_id |= FAP_300A_IGBT1_OVERTEMP_ITLK;
    if (fap_300A.TempIGBT2ItlkSts)      itlk_id |= FAP_300A_IGBT2_OVERTEMP_ITLK;
    if (fap_300A.Driver1ErrorItlk)      itlk_id |= FAP_300A_DRIVER1_ERROR_ITLK;
    if (fap_300A.Driver2ErrorItlk)      itlk_id |= FAP_300A_DRIVER2_ERROR_ITLK;
    if (fap_300A.TempLItlkSts)          itlk_id |= FAP_300A_INDUC_OVERTEMP_ITLK;
    if (fap_300A.TempHeatSinkItlkSts)   itlk_id |= FAP_300A_HS_OVERTEMP_ITLK;
    if (fap_300A.Relay)                 itlk_id |= FAP_300A_RELAY_ITLK;
    if (fap_300A.ExternalItlkSts)       itlk_id |= FAP_300A_EXTERNAL_ITLK;
    if (fap_300A.LeakageCurrentSts)     itlk_id |= FAP_300A_LEAKAGE_CURRENT_ITLK;
    if (fap_300A.RackSts)               itlk_id |= FAP_300A_RACK_ITLK;
    if (fap_300A.GroundLeakageItlkSts)  itlk_id |= FAP_300A_GROUND_LKG_ITLK;
}

static void get_alarms_id()
{
    if (fap_300A.VinAlarmSts)           alarm_id |= FAP_300A_INPUT_OVERVOLTAGE_ALM;
    if (fap_300A.VoutAlarmSts)          alarm_id |= FAP_300A_OUTPUT_OVERVOLTAGE_ALM;
    if (fap_300A.IoutA1AlarmSts)        alarm_id |= FAP_300A_OUTPUT_OVERCURRENT_1_ALM;
    if (fap_300A.IoutA2AlarmSts)        alarm_id |= FAP_300A_OUTPUT_OVERCURRENT_2_ALM;
    if (fap_300A.TempIGBT1AlarmSts)     alarm_id |= FAP_300A_IGBT1_OVERTEMP_ALM;
    if (fap_300A.TempIGBT2AlarmSts)     alarm_id |= FAP_300A_IGBT2_OVERTEMP_ALM;
    if (fap_300A.TempLAlarmSts)         alarm_id |= FAP_300A_INDUC_OVERTEMP_ALM;
    if (fap_300A.TempHeatSinkAlarmSts)  alarm_id |= FAP_300A_HS_OVERTEMP_ALM;
    if (fap_300A.GroundLeakageAlarmSts) alarm_id |= FAP_300A_GROUND_LKG_ALM;
}

void send_fap_300A_itlk_msg()
{
    send_data_message(0);
}

static void config_module()
{
    //Set current range FAP 300 A
    CurrentCh1Init(150.0, 0.150, 50.0, 3); // Corrente braço1: Sensor Hall
    CurrentCh2Init(150.0, 0.150, 50.0, 3); // Corrente braço2: LEM LA 130-


    //Set protection limits FAP 300 A
    //     These interlocks are bypassed due to the fact that their ADC's
    //     will most probably saturate during operation at 300 A. These
    //     measures are also performed by UDC, which guarantees these
    //     protections
    CurrentCh1AlarmLevelSet(FAP_OUTPUT_OVERCURRENT_1_ALM_LIM);  // Corrente braço1
    CurrentCh1TripLevelSet(FAP_OUTPUT_OVERCURRENT_1_ITLK_LIM);  // Corrente braço1
    CurrentCh2AlarmLevelSet(FAP_OUTPUT_OVERCURRENT_2_ALM_LIM);  // Corrente braço2
    CurrentCh2TripLevelSet(FAP_OUTPUT_OVERCURRENT_2_ITLK_LIM);  // Corrente braço2

    // NTC contiguration type
    //ConfigNtcType(SEMIX);

    //Leitura de tensão isolada
    LvCurrentCh1Init(450.0, 0.025, 120.0, 3); // Tensão de entrada
    LvCurrentCh2Init(250.0, 0.025, 120.0, 3); // Tensão de saída

    // TODO: Check this values
    LvCurrentCh3Init(50.0, 0.025, 120.0, 3); // Ground Leakage

    LvCurrentCh1AlarmLevelSet(FAP_INPUT_OVERVOLTAGE_ALM_LIM);   // Tensão de entrada Alarme
    LvCurrentCh1TripLevelSet(FAP_INPUT_OVERVOLTAGE_ITLK_LIM);   // Tensão de entrada Interlock
    LvCurrentCh2AlarmLevelSet(FAP_OUTPUT_OVERVOLTAGE_ALM_LIM);  // Tensão de saída Alarme
    LvCurrentCh2TripLevelSet(FAP_OUTPUT_OVERVOLTAGE_ITLK_LIM);  // Tensão de saída Interlock

    // TODO: Check this
    LvCurrentCh3AlarmLevelSet(FAP_GROUND_LEAKAGE_ALM_LIM);  // fuga para o terra alarme
    LvCurrentCh3TripLevelSet(FAP_GROUND_LEAKAGE_ITLK_LIM);  // fuga para o terra interlock

    // PT100 configuration limits
    Pt100SetCh1AlarmLevel(FAP_HS_OVERTEMP_ALM_LIM);     // Temperatura Dissipador
    Pt100SetCh1TripLevel(FAP_HS_OVERTEMP_ITLK_LIM);     // Temperatura Dissipador
    Pt100SetCh2AlarmLevel(FAP_INDUC_OVERTEMP_ALM_LIM);  // Temperatura L
    Pt100SetCh2TripLevel(FAP_INDUC_OVERTEMP_ITLK_LIM);  // Temperatura L

    // Delay 4 seconds
    Pt100SetCh1Delay(4);
    // Delay 4 seconds
    Pt100SetCh2Delay(4);

    // PT100 channel enable
    Pt100Ch1Enable(); // Temperatura Dissipador
    Pt100Ch2Enable(); // Temperatura L
    Pt100Ch3Disable();
    Pt100Ch4Disable();

    // Rh configuration limits
    RhAlarmLevelSet(FAP_RH_ALM_LIM);
    RhTripLevelSet(FAP_RH_ITLK_LIM);

    // Temp board configuration limits
    BoardTempAlarmLevelSet(FAP_BOARD_TEMP_ALM_LIM);
    BoardTempTripLevelSet(FAP_BOARD_TEMP_ITLK_LIM);

    //Driver1 error configuration
    Driver1TopErrorEnable(); //Habilitado driver error 1 Top
    Driver1BotErrorDisable(); //Desabilitado driver error 1 Bot
    Driver1OverTempDisable(); //Desabilitado Temperatura por Hardware do modulo 1

    //Driver2 error configuration
    Driver2TopErrorEnable(); //Habilitado driver error 2 Top
    Driver2BotErrorDisable(); //Desabilitado driver error 2 Bot
    Driver2OverTempDisable(); //Desabilitado Temperatura por Hardware do modulo 2

    // Init Variables
    fap_300A.Vin.f                  = 0.0;
    fap_300A.VinAlarmSts            = 0;
    fap_300A.VinItlkSts             = 0;
    fap_300A.Vout.f                 = 0.0;
    fap_300A.VoutAlarmSts           = 0;
    fap_300A.VoutItlkSts            = 0;
    fap_300A.IoutA1.f               = 0.0;
    fap_300A.IoutA1AlarmSts         = 0;
    fap_300A.IoutA1ItlkSts          = 0;
    fap_300A.IoutA2.f               = 0.0;
    fap_300A.IoutA2AlarmSts         = 0;
    fap_300A.IoutA2ItlkSts          = 0;
    fap_300A.TempIGBT1.f            = 0.0;
    fap_300A.TempIGBT1AlarmSts      = 0;
    fap_300A.TempIGBT1ItlkSts       = 0;
    fap_300A.TempIGBT1HwrItlk       = 0;
    fap_300A.TempIGBT1HwrItlkSts    = 0;
    fap_300A.TempIGBT2.f            = 0.0;
    fap_300A.TempIGBT2AlarmSts      = 0;
    fap_300A.TempIGBT2ItlkSts       = 0;
    fap_300A.TempIGBT2HwrItlk       = 0;
    fap_300A.TempIGBT2HwrItlkSts    = 0;
    fap_300A.DriveVoltage.f         = 0.0;
    fap_300A.Drive1Current.f        = 0.0;
    fap_300A.Drive2Current.f        = 0.0;
    fap_300A.Driver1Error           = 0;
    fap_300A.Driver1ErrorItlk       = 0;
    fap_300A.Driver2Error           = 0;
    fap_300A.Driver2ErrorItlk       = 0;
    fap_300A.TempL.f                = 0;
    fap_300A.TempLAlarmSts          = 0;
    fap_300A.TempLItlkSts           = 0;
    fap_300A.TempHeatSink.f         = 0;
    fap_300A.TempHeatSinkAlarmSts   = 0;
    fap_300A.TempHeatSinkItlkSts    = 0;
    fap_300A.Relay                  = 0;
    fap_300A.ExternalItlk           = 0;
    fap_300A.ExternalItlkSts        = 0;
    fap_300A.LeakageCurrent         = 0;
    fap_300A.LeakageCurrentSts      = 0;
    fap_300A.Rack                   = 0;
    fap_300A.RackSts                = 0;
    fap_300A.GroundLeakage.f        = 0;
    fap_300A.GroundLeakageAlarmSts  = 0;
    fap_300A.GroundLeakageItlkSts   = 0;
}
