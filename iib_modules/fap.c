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
 * @file fap.c
 * @brief Brief description of module
 * 
 * Detailed description
 *
 * @author allef.silva
 * @date 20/10/2018
 *
 */

/////////////////////////////////////////////////////////////////////////////////////////////

#include "iib_modules/fap.h"

#include "iib_devices/iib_data.h"
#include "iib_devices/adc_internal.h"
#include "iib_devices/application.h"
#include "iib_devices/BoardTempHum.h"
#include "iib_devices/ntc_isolated_i2c.h"
#include "iib_devices/pt100.h"
#include "iib_devices/output.h"
#include "iib_devices/leds.h"
#include "iib_devices/can_bus.h"
#include "iib_devices/input.h"
#include "system/system.h"
#include "inc/hw_ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/ssi.h"

#include <stdbool.h>
#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////////////////////

/**
 * TODO: Put here your defines. Just what is local. If you don't
 * need to access it from other module, consider use a constant (const)
 */

// Linhas de transporte
//#define FAP_INPUT_OVERVOLTAGE_ALM_LIM           50.0
//#define FAP_INPUT_OVERVOLTAGE_ITLK_LIM          55.0

//#define FAP_OUTPUT_OVERVOLTAGE_ALM_LIM          35.0
//#define FAP_OUTPUT_OVERVOLTAGE_ITLK_LIM         40.0

//#define FAP_OUTPUT_OVERCURRENT_1_ALM_LIM        105.0
//#define FAP_OUTPUT_OVERCURRENT_1_ITLK_LIM       115.0

//#define FAP_OUTPUT_OVERCURRENT_2_ALM_LIM        105.0
//#define FAP_OUTPUT_OVERCURRENT_2_ITLK_LIM       115.0

// Potencia nominal
#define FAP_INPUT_OVERVOLTAGE_ALM_LIM         555.0
#define FAP_INPUT_OVERVOLTAGE_ITLK_LIM        560.0

#define FAP_OUTPUT_OVERVOLTAGE_ALM_LIM        260.0
#define FAP_OUTPUT_OVERVOLTAGE_ITLK_LIM       270.0

#define FAP_OUTPUT_OVERCURRENT_1_ALM_LIM      115.0
#define FAP_OUTPUT_OVERCURRENT_1_ITLK_LIM     120.0

#define FAP_OUTPUT_OVERCURRENT_2_ALM_LIM      115.0
#define FAP_OUTPUT_OVERCURRENT_2_ITLK_LIM     120.0

// Fonte 750A
//#define FAP_INPUT_OVERVOLTAGE_ALM_LIM                    55.0
//#define FAP_INPUT_OVERVOLTAGE_ITLK_LIM                   60.0

//#define FAP_OUTPUT_OVERVOLTAGE_ALM_LIM                   40.0
//#define FAP_OUTPUT_OVERVOLTAGE_ITLK_LIM                  45.0

//#define FAP_OUTPUT_OVERCURRENT_1_ALM_LIM                 100.0
//#define FAP_OUTPUT_OVERCURRENT_1_ITLK_LIM                105.0

//#define FAP_OUTPUT_OVERCURRENT_2_ALM_LIM                 100.0
//#define FAP_OUTPUT_OVERCURRENT_2_ITLK_LIM                105.0

#define FAP_GROUND_LEAKAGE_ALM_LIM              40.0
#define FAP_GROUND_LEAKAGE_ITLK_LIM             45.0

#define FAP_IGBT1_OVERTEMP_ALM_LIM              60
#define FAP_IGBT1_OVERTEMP_ITLK_LIM             80

#define FAP_IGBT2_OVERTEMP_ALM_LIM              60
#define FAP_IGBT2_OVERTEMP_ITLK_LIM             80

#define FAP_DRIVER_OVERVOLTAGE_ALM_LIM_LIM      16.0
#define FAP_DRIVER_OVERVOLTAGE_ITLK_LIM         17.0

#define FAP_DRIVER1_OVERCURRENT_ALM_LIM         1.15
#define FAP_DRIVER1_OVERCURRENT_ITLK_LIM        2.4

#define FAP_DRIVER2_OVERCURRENT_ALM_LIM         1.15
#define FAP_DRIVER2_OVERCURRENT_ITLK_LIM        2.4

#define FAP_INDUC_OVERTEMP_ALM_LIM              30
#define FAP_INDUC_OVERTEMP_ITLK_LIM             60

#define FAP_HS_OVERTEMP_ALM_LIM                 30
#define FAP_HS_OVERTEMP_ITLK_LIM                60

#define FAP_RH_ALM_LIM                          50
#define FAP_RH_ITLK_LIM                         90

#define FAP_BOARD_TEMP_ALM_LIM                  80
#define FAP_BOARD_TEMP_ITLK_LIM                 90

/////////////////////////////////////////////////////////////////////////////////////////////

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

    union {
        float   f;
        uint8_t u8[4];
    } TempIGBT2;

    bool TempIGBT2AlarmSts;
    bool TempIGBT2ItlkSts;

    union {
        float   f;
        uint8_t u[4];
    } DriveVoltage;

    bool DriveVoltageAlarmSts;
    bool DriveVoltageItlkSts;

    union {
        float   f;
        uint8_t u[4];
    } Drive1Current;

    bool Drive1CurrentAlarmSts;
    bool Drive1CurrentItlkSts;

    union {
        float   f;
        uint8_t u[4];
    } Drive2Current;

    bool Drive2CurrentAlarmSts;
    bool Drive2CurrentItlkSts;

    bool Driver1Error;
    bool Driver1ErrorItlkSts;
    bool Driver2Error;
    bool Driver2ErrorItlkSts;

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

    union {
        float   f;
        uint8_t u[4];
    } BoardTemperature;

    bool BoardTemperatureAlarmSts;
    bool BoardTemperatureItlkSts;

    union {
        float   f;
        uint8_t u[4];
    } RelativeHumidity;

    bool RelativeHumidityAlarmSts;
    bool RelativeHumidityItlkSts;

    bool Relay;
    bool ExternalItlk;
    bool ExternalItlkSts;
    bool Rack;
    bool RackItlkSts;

    bool ReleAuxItlkSts;
    bool ReleExtItlkSts;
    bool RelayOpenItlkSts;
    bool RelayContactStickingItlkSts;
    bool FlagAux;

} fap_t;

/////////////////////////////////////////////////////////////////////////////////////////////

fap_t fap;

static uint32_t fap_interlocks_indication;
static uint32_t fap_alarms_indication;

/////////////////////////////////////////////////////////////////////////////////////////////

static uint32_t itlk_id;
static uint32_t alarm_id;

/////////////////////////////////////////////////////////////////////////////////////////////

static void get_itlks_id();
static void get_alarms_id();
static void map_vars();
static void config_module();

/////////////////////////////////////////////////////////////////////////////////////////////

void init_fap()
{
    config_module();
}

/////////////////////////////////////////////////////////////////////////////////////////////

void clear_fap_interlocks()
{
    fap.RelayOpenItlkSts = 0;
    fap.RelayContactStickingItlkSts = 0;
    fap.ReleAuxItlkSts = 0;
    fap.ReleExtItlkSts = 0;
    fap.FlagAux = 0;

////////////////////////////////////////

    fap.VinItlkSts               = 0;
    fap.VoutItlkSts              = 0;
    fap.IoutA1ItlkSts            = 0;
    fap.IoutA2ItlkSts            = 0;
    fap.TempIGBT1ItlkSts         = 0;
    fap.TempIGBT2ItlkSts         = 0;
    fap.Driver1ErrorItlkSts      = 0;
    fap.Driver2ErrorItlkSts      = 0;
    fap.TempLItlkSts             = 0;
    fap.TempHeatSinkItlkSts      = 0;
    fap.ExternalItlkSts          = 0;
    fap.RackItlkSts              = 0;
    fap.GroundLeakageItlkSts     = 0;
    fap.DriveVoltageItlkSts      = 0;
    fap.Drive1CurrentItlkSts     = 0;
    fap.Drive2CurrentItlkSts     = 0;
    fap.BoardTemperatureItlkSts  = 0;
    fap.RelativeHumidityItlkSts  = 0;

    itlk_id = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

uint8_t check_fap_interlocks()
{
    uint8_t test = 0;

    test |= fap.VinItlkSts;
    test |= fap.VoutItlkSts;
    test |= fap.IoutA1ItlkSts;
    test |= fap.IoutA2ItlkSts;
    test |= fap.TempIGBT1ItlkSts;
    test |= fap.TempIGBT2ItlkSts;
    test |= fap.Driver1ErrorItlkSts;
    test |= fap.Driver2ErrorItlkSts;
    test |= fap.TempLItlkSts;
    test |= fap.TempHeatSinkItlkSts;
    test |= fap.ExternalItlkSts;
    test |= fap.RackItlkSts;
    test |= fap.GroundLeakageItlkSts;
    test |= fap.DriveVoltageItlkSts;
    test |= fap.Drive1CurrentItlkSts;
    test |= fap.Drive2CurrentItlkSts;
    test |= fap.BoardTemperatureItlkSts;
    test |= fap.RelativeHumidityItlkSts;

    return test;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void clear_fap_alarms()
{
    fap.VinAlarmSts               = 0;
    fap.VoutAlarmSts              = 0;
    fap.IoutA1AlarmSts            = 0;
    fap.IoutA2AlarmSts            = 0;
    fap.TempIGBT1AlarmSts         = 0;
    fap.TempIGBT2AlarmSts         = 0;
    fap.TempLAlarmSts             = 0;
    fap.TempHeatSinkAlarmSts      = 0;
    fap.GroundLeakageAlarmSts     = 0;
    fap.DriveVoltageAlarmSts      = 0;
    fap.Drive1CurrentAlarmSts     = 0;
    fap.Drive2CurrentAlarmSts     = 0;
    fap.BoardTemperatureAlarmSts  = 0;
    fap.RelativeHumidityAlarmSts  = 0;

    alarm_id = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

uint8_t check_fap_alarms()
{
    uint8_t test = 0;

    test |= fap.VinAlarmSts;
    test |= fap.VoutAlarmSts;
    test |= fap.IoutA1AlarmSts;
    test |= fap.IoutA2AlarmSts;
    test |= fap.TempIGBT1AlarmSts;
    test |= fap.TempIGBT2AlarmSts;
    test |= fap.TempLAlarmSts;
    test |= fap.TempHeatSinkAlarmSts;
    test |= fap.GroundLeakageAlarmSts;
    test |= fap.DriveVoltageAlarmSts;
    test |= fap.Drive1CurrentAlarmSts;
    test |= fap.Drive2CurrentAlarmSts;
    test |= fap.BoardTemperatureAlarmSts;
    test |= fap.RelativeHumidityAlarmSts;

    return test;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void check_fap_indication_leds()
{
    //Output over voltage
    if(fap.VoutItlkSts) Led2TurnOff();
    else if(fap.VoutAlarmSts) Led2Toggle();
    else Led2TurnOn();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Input over voltage
    if(fap.VinItlkSts) Led3TurnOff();
    else if(fap.VinAlarmSts) Led3Toggle();
    else Led3TurnOn();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Output over current
    if (fap.IoutA1ItlkSts || fap.IoutA2ItlkSts) Led4TurnOff();
    else if(fap.IoutA1AlarmSts || fap.IoutA2AlarmSts) Led4Toggle();
    else Led4TurnOn();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Over temperature
    if(fap.TempIGBT1ItlkSts || fap.TempIGBT2ItlkSts ||  fap.TempLItlkSts || fap.TempHeatSinkItlkSts) Led5TurnOff();
    else if(fap.TempIGBT1AlarmSts || fap.TempIGBT2AlarmSts ||  fap.TempLAlarmSts || fap.TempHeatSinkAlarmSts) Led5Toggle();
    else Led5TurnOn();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Interlock Externo
    if(fap.ExternalItlkSts)Led6TurnOff();
    else Led6TurnOff();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Fuga para o Terra
    if(fap.GroundLeakageItlkSts) Led7TurnOff();
    else if(fap.GroundLeakageAlarmSts) Led7Toggle();
    else Led7TurnOn();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Interlock do Rack
    if(fap.RackItlkSts) Led8TurnOff();
    else Led8TurnOn();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Interlocks dos Drivers
    if(fap.Driver1ErrorItlkSts || fap.Driver2ErrorItlkSts || fap.DriveVoltageItlkSts || fap.Drive1CurrentItlkSts || fap.Drive2CurrentItlkSts) Led9TurnOff();
    else if(fap.DriveVoltageAlarmSts || fap.Drive1CurrentAlarmSts || fap.Drive2CurrentAlarmSts) Led9Toggle();
    else Led9TurnOn();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Interlock Temperatura PCB e Umidade Relativa
    if(fap.BoardTemperatureItlkSts || fap.RelativeHumidityItlkSts) Led10TurnOff();
    else if(fap.BoardTemperatureAlarmSts || fap.RelativeHumidityAlarmSts) Led10Toggle();
    else Led10TurnOn();
}

/////////////////////////////////////////////////////////////////////////////////////////////

void fap_application_readings()
{
    //PT100 CH1 Dissipador
    fap.TempHeatSink.f = (float) Pt100ReadCh1();
    fap.TempHeatSinkAlarmSts = Pt100ReadCh1AlarmSts();
    if(!fap.TempHeatSinkItlkSts)fap.TempHeatSinkItlkSts = Pt100ReadCh1TripSts();

/////////////////////////////////////////////////////////////////////////////////////////////

    //PT100 CH2 Indutor
    fap.TempL.f = (float) Pt100ReadCh2();
    fap.TempLAlarmSts = Pt100ReadCh2AlarmSts();
    if(!fap.TempLItlkSts)fap.TempLItlkSts = Pt100ReadCh2TripSts();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Temperatura IGBT1
    fap.TempIGBT1.f = (float) TempIgbt1Read();
    fap.TempIGBT1AlarmSts = TempIgbt1AlarmStatusRead();
    if(!fap.TempIGBT1ItlkSts)fap.TempIGBT1ItlkSts = TempIgbt1TripStatusRead();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Temperatura IGBT2
    fap.TempIGBT2.f = (float) TempIgbt2Read();
    fap.TempIGBT2AlarmSts = TempIgbt2AlarmStatusRead();;
    if(!fap.TempIGBT2ItlkSts)fap.TempIGBT2ItlkSts = TempIgbt2TripStatusRead();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Temperatura PCB IIB
    fap.BoardTemperature.f = (float) BoardTempRead();
    fap.BoardTemperatureAlarmSts = BoardTempAlarmStatusRead();
    if(!fap.BoardTemperatureItlkSts)fap.BoardTemperatureItlkSts = BoardTempTripStatusRead();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Umidade Relativa
    fap.RelativeHumidity.f = (float) RhRead();
    fap.RelativeHumidityAlarmSts = RhAlarmStatusRead();
    if(!fap.RelativeHumidityItlkSts)fap.RelativeHumidityItlkSts = RhTripStatusRead();

/////////////////////////////////////////////////////////////////////////////////////////////

    //DriverVotage
    fap.DriveVoltage.f = DriverVoltageRead();
    fap.DriveVoltageAlarmSts = DriverVoltageAlarmStatusRead();
    if(!fap.DriveVoltageItlkSts)fap.DriveVoltageItlkSts = DriverVolatgeTripStatusRead();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Drive1Current
    fap.Drive1Current.f = Driver1CurrentRead();
    fap.Drive1CurrentAlarmSts = Driver1CurrentAlarmStatusRead();
    if(!fap.Drive1CurrentItlkSts)fap.Drive1CurrentItlkSts = Driver1CurrentTripStatusRead();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Drive2Current
    fap.Drive2Current.f = Driver2CurrentRead();
    fap.Drive2CurrentAlarmSts = Driver2CurrentAlarmStatusRead();
    if(!fap.Drive2CurrentItlkSts)fap.Drive2CurrentItlkSts = Driver2CurrentTripStatusRead();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Corrente de Saida IGBT1
    fap.IoutA1.f = CurrentCh1Read();//HALL CH1
    fap.IoutA1AlarmSts = CurrentCh1AlarmStatusRead();
    if(!fap.IoutA1ItlkSts)fap.IoutA1ItlkSts = CurrentCh1TripStatusRead();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Corrente de Saida IGBT2
    fap.IoutA2.f = CurrentCh2Read();//HALL CH2
    fap.IoutA2AlarmSts = CurrentCh2AlarmStatusRead();
    if(!fap.IoutA2ItlkSts)fap.IoutA2ItlkSts = CurrentCh2TripStatusRead();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Tensao de Entrada
    fap.Vin.f = LvCurrentCh1Read();
    fap.VinAlarmSts = LvCurrentCh1AlarmStatusRead();
    if(!fap.VinItlkSts)fap.VinItlkSts = LvCurrentCh1TripStatusRead();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Tensao de Saida
    fap.Vout.f = LvCurrentCh2Read();
    fap.VoutAlarmSts = LvCurrentCh2AlarmStatusRead();
    if(!fap.VoutItlkSts) fap.VoutItlkSts = LvCurrentCh2TripStatusRead();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Medida de Fuga para o Terra
    fap.GroundLeakage.f = LvCurrentCh3Read();
    fap.GroundLeakageAlarmSts = LvCurrentCh3AlarmStatusRead();
    if(!fap.GroundLeakageItlkSts) fap.GroundLeakageItlkSts = LvCurrentCh3TripStatusRead();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Interlock externo
    fap.ExternalItlk = Gpdi5Read(); //Variavel usada para debug
    if(!fap.ExternalItlkSts) fap.ExternalItlkSts = Gpdi5Read();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Interlock do Rack
    fap.Rack = Gpdi6Read(); //Variavel usada para debug
    if(!fap.RackItlkSts) fap.RackItlkSts = Gpdi6Read();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Status do Contato do Rele
    fap.Relay = Gpdi7Read();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Erro do Driver 1
    fap.Driver1Error = Driver1TopErrorRead(); //Variavel usada para debug
    if(!fap.Driver1ErrorItlkSts) fap.Driver1ErrorItlkSts = Driver1TopErrorRead();

/////////////////////////////////////////////////////////////////////////////////////////////

    //Erro do Driver 2
    fap.Driver2Error = Driver2TopErrorRead(); //Variavel usada para debug
    if(!fap.Driver2ErrorItlkSts) fap.Driver2ErrorItlkSts = Driver2TopErrorRead();

/////////////////////////////////////////////////////////////////////////////////////////////

    fap.ReleAuxItlkSts = ReleAuxSts();
    fap.ReleExtItlkSts = ReleExtItlkSts();

    if(fap.ReleAuxItlkSts == 1 && fap.ReleExtItlkSts == 0)
    {
       fap.FlagAux = 1;
       fap.RelayOpenItlkSts = 0;
       fap.RelayContactStickingItlkSts = 0;
    }

    delay_ms(2);

    if(fap.FlagAux == 1 && fap.ReleAuxItlkSts == 0 && fap.ReleExtItlkSts == 0)
    {
       fap.RelayOpenItlkSts = 1;
       fap.RelayContactStickingItlkSts = 0;
    }

    delay_ms(2);

    if(fap.FlagAux == 1 && fap.ReleAuxItlkSts == 0 && fap.ReleExtItlkSts == 1)
    {
       fap.RelayContactStickingItlkSts = 1;
       fap.RelayOpenItlkSts = 0;
    }

/////////////////////////////////////////////////////////////////////////////////////////////

    //Se nao houver sinal na entrada digital dos 4 sinais, defina a acao como Interlock.
    if(fap.ExternalItlkSts || fap.RackItlkSts || fap.Driver1ErrorItlkSts || fap.Driver2ErrorItlkSts) InterlockSet();

/////////////////////////////////////////////////////////////////////////////////////////////

    map_vars();
    get_itlks_id();
    get_alarms_id();
}

/////////////////////////////////////////////////////////////////////////////////////////////

void fap_power_on_check()
{
    if (fap.Relay) {
        Led1TurnOff();
        ReleExtItlkTurnOff();
    }
    else {
        Led1TurnOn();
        ReleExtItlkTurnOn();
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////

static void map_vars()
{
    fap_interlocks_indication = itlk_id;
    fap_alarms_indication = alarm_id;

    g_controller_iib.iib_signals[0].u32     = fap_interlocks_indication;
    g_controller_iib.iib_signals[1].u32     = fap_alarms_indication;
    g_controller_iib.iib_signals[2].f       = fap.Vin.f;
    g_controller_iib.iib_signals[3].f       = fap.Vout.f;
    g_controller_iib.iib_signals[4].f       = fap.IoutA1.f;
    g_controller_iib.iib_signals[5].f       = fap.IoutA2.f;
    g_controller_iib.iib_signals[6].f       = fap.TempIGBT1.f;
    g_controller_iib.iib_signals[7].f       = fap.TempIGBT2.f;
    g_controller_iib.iib_signals[8].f       = fap.DriveVoltage.f;
    g_controller_iib.iib_signals[9].f       = fap.Drive1Current.f;
    g_controller_iib.iib_signals[10].f      = fap.Drive2Current.f;
    g_controller_iib.iib_signals[11].f      = fap.TempL.f;
    g_controller_iib.iib_signals[12].f      = fap.TempHeatSink.f;
    g_controller_iib.iib_signals[13].f      = fap.GroundLeakage.f;
    g_controller_iib.iib_signals[14].f      = fap.BoardTemperature.f;
    g_controller_iib.iib_signals[15].f      = fap.RelativeHumidity.f;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void send_fap_data()
{
    static uint8_t i = 0;

    send_data_message(i);

    i++;

    if (i > 15) i = 0;

}

/////////////////////////////////////////////////////////////////////////////////////////////

static void get_itlks_id()
{
    if (fap.VinItlkSts)                     itlk_id |= FAP_INPUT_OVERVOLTAGE_ITLK;
    if (fap.VoutItlkSts)                    itlk_id |= FAP_OUTPUT_OVERVOLTAGE_ITLK;
    if (fap.IoutA1ItlkSts)                  itlk_id |= FAP_OUTPUT_OVERCURRENT_1_ITLK;
    if (fap.IoutA2ItlkSts)                  itlk_id |= FAP_OUTPUT_OVERCURRENT_2_ITLK;
    if (fap.TempIGBT1ItlkSts)               itlk_id |= FAP_IGBT1_OVERTEMP_ITLK;
    if (fap.TempIGBT2ItlkSts)               itlk_id |= FAP_IGBT2_OVERTEMP_ITLK;
    if (fap.DriveVoltageItlkSts)            itlk_id |= FAP_DRIVER_OVERVOLTAGE_ITLK;
    if (fap.Drive1CurrentItlkSts)           itlk_id |= FAP_DRIVER1_OVERCURRENT_ITLK;
    if (fap.Drive2CurrentItlkSts)           itlk_id |= FAP_DRIVER2_OVERCURRENT_ITLK;
    if (fap.Driver1ErrorItlkSts)            itlk_id |= FAP_DRIVER1_ERROR_ITLK;
    if (fap.Driver2ErrorItlkSts)            itlk_id |= FAP_DRIVER2_ERROR_ITLK;
    if (fap.TempLItlkSts)                   itlk_id |= FAP_INDUC_OVERTEMP_ITLK;
    if (fap.TempHeatSinkItlkSts)            itlk_id |= FAP_HS_OVERTEMP_ITLK;
    if (fap.RelayOpenItlkSts)               itlk_id |= FAP_RELAY_ITLK;
    if (fap.RelayContactStickingItlkSts)    itlk_id |= FAP_RELAY_CONTACT_STICKING_ITLK;
    if (fap.ExternalItlkSts)                itlk_id |= FAP_EXTERNAL_ITLK;
    if (fap.RackItlkSts)                    itlk_id |= FAP_RACK_ITLK;
    if (fap.GroundLeakageItlkSts)           itlk_id |= FAP_GROUND_LKG_ITLK;
    if (fap.BoardTemperatureItlkSts)        itlk_id |= FAP_BOARD_IIB_OVERTEMP_ITLK;
    if (fap.RelativeHumidityItlkSts)        itlk_id |= FAP_BOARD_IIB_OVERHUMIDITY_ITLK;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void send_fap_itlk_msg()
{
    send_data_message(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////

static void get_alarms_id()
{
    if (fap.VinAlarmSts)               alarm_id |= FAP_INPUT_OVERVOLTAGE_ALM;
    if (fap.VoutAlarmSts)              alarm_id |= FAP_OUTPUT_OVERVOLTAGE_ALM;
    if (fap.IoutA1AlarmSts)            alarm_id |= FAP_OUTPUT_OVERCURRENT_1_ALM;
    if (fap.IoutA2AlarmSts)            alarm_id |= FAP_OUTPUT_OVERCURRENT_2_ALM;
    if (fap.TempIGBT1AlarmSts)         alarm_id |= FAP_IGBT1_OVERTEMP_ALM;
    if (fap.TempIGBT2AlarmSts)         alarm_id |= FAP_IGBT2_OVERTEMP_ALM;
    if (fap.TempLAlarmSts)             alarm_id |= FAP_INDUC_OVERTEMP_ALM;
    if (fap.TempHeatSinkAlarmSts)      alarm_id |= FAP_HS_OVERTEMP_ALM;
    if (fap.GroundLeakageAlarmSts)     alarm_id |= FAP_GROUND_LKG_ALM;
    if (fap.DriveVoltageAlarmSts)      alarm_id |= FAP_DRIVER_OVERVOLTAGE_ALM;
    if (fap.Drive1CurrentAlarmSts)     alarm_id |= FAP_DRIVER1_OVERCURRENT_ALM;
    if (fap.Drive2CurrentAlarmSts)     alarm_id |= FAP_DRIVER2_OVERCURRENT_ALM;
    if (fap.BoardTemperatureAlarmSts)  alarm_id |= FAP_BOARD_IIB_OVERTEMP_ALM;
    if (fap.RelativeHumidityAlarmSts)  alarm_id |= FAP_BOARD_IIB_OVERHUMIDITY_ALM;
}

/////////////////////////////////////////////////////////////////////////////////////////////

static void config_module()
{
    //Set current range FAP 150 A
    CurrentCh1Init(130.0, 0.130, 50.0, 3); //Corrente braço1: Sensor Hall
    CurrentCh2Init(130.0, 0.130, 50.0, 3); //Corrente braço2: LEM LA 130-P

    CurrentCh1Enable();  //CurrentCh1 enable
    CurrentCh2Enable();  //CurrentCh2 enable
    CurrentCh3Disable(); //CurrentCh3 disable
    CurrentCh4Disable(); //CurrentCh4 disable

    //Set protection limits FAP 150 A
    //     These interlocks are bypassed due to the fact that their ADC's
    //     will most probably saturate during operation at 300 A. These
    //     measures are also performed by UDC, which guarantees these
    //     protections
    CurrentCh1AlarmLevelSet(FAP_OUTPUT_OVERCURRENT_1_ALM_LIM);  //Corrente braço1
    CurrentCh1TripLevelSet(FAP_OUTPUT_OVERCURRENT_1_ITLK_LIM);  //Corrente braço1
    CurrentCh2AlarmLevelSet(FAP_OUTPUT_OVERCURRENT_2_ALM_LIM);  //Corrente braço2
    CurrentCh2TripLevelSet(FAP_OUTPUT_OVERCURRENT_2_ITLK_LIM);  //Corrente braço2

/////////////////////////////////////////////////////////////////////////////////////////////

    //Leitura de tensão isolada
    LvCurrentCh1Init(720, 0.025, 120.0, 100); // Vin
    LvCurrentCh2Init(300, 0.025, 120.0, 100); // Vout
    LvCurrentCh3Init(50.0, 0.025, 120.0, 3); // Ground Leakage

    LvCurrentCh1Enable(); //LvCurrentCh1 enable
    LvCurrentCh2Enable(); //LvCurrentCh2 enable
    LvCurrentCh3Enable(); //LvCurrentCh3 enable

    LvCurrentCh1AlarmLevelSet(FAP_INPUT_OVERVOLTAGE_ALM_LIM);  //Tensão de entrada Alarme
    LvCurrentCh1TripLevelSet(FAP_INPUT_OVERVOLTAGE_ITLK_LIM);  //Tensão de entrada Interlock
    LvCurrentCh2AlarmLevelSet(FAP_OUTPUT_OVERVOLTAGE_ALM_LIM); //Tensão de saída Alarme
    LvCurrentCh2TripLevelSet(FAP_OUTPUT_OVERVOLTAGE_ITLK_LIM); // Tensão de saída Interlock
    LvCurrentCh3AlarmLevelSet(FAP_GROUND_LEAKAGE_ALM_LIM);     //Fuga para o terra alarme
    LvCurrentCh3TripLevelSet(FAP_GROUND_LEAKAGE_ITLK_LIM);     //Fuga para o terra interlock

/////////////////////////////////////////////////////////////////////////////////////////////

    //PT100 configuration
    //Delay 4 seconds
    Pt100SetCh1Delay(4);
    Pt100SetCh2Delay(4);

    //PT100 channel enable
    Pt100Ch1Enable(); //Temperatura Dissipador
    Pt100Ch2Enable(); //Temperatura L
    Pt100Ch3Disable();
    Pt100Ch4Disable();

    //PT100 configuration limits
    Pt100SetCh1AlarmLevel(FAP_HS_OVERTEMP_ALM_LIM);     //Temperatura Dissipador
    Pt100SetCh1TripLevel(FAP_HS_OVERTEMP_ITLK_LIM);     //Temperatura Dissipador
    Pt100SetCh2AlarmLevel(FAP_INDUC_OVERTEMP_ALM_LIM);  //Temperatura L
    Pt100SetCh2TripLevel(FAP_INDUC_OVERTEMP_ITLK_LIM);  //Temperatura L

/////////////////////////////////////////////////////////////////////////////////////////////

    //Temperature igbt1 configuration
    TempIgbt1Delay(3); //Inserir valor de delay

    TempIgbt1Enable(); //TempIgbt1 enable

    //Temp Igbt1 configuration limits
    TempIgbt1AlarmLevelSet(FAP_IGBT1_OVERTEMP_ALM_LIM);
    TempIgbt1TripLevelSet(FAP_IGBT1_OVERTEMP_ITLK_LIM);

/////////////////////////////////////////////////////////////////////////////////////////////

    //Temperature igbt2 configuration
    TempIgbt2Delay(3); //Inserir valor de delay

    TempIgbt2Enable(); //TempIgbt2 enable

    //Temp Igbt2 configuration limits
    TempIgbt2AlarmLevelSet(FAP_IGBT2_OVERTEMP_ALM_LIM);
    TempIgbt2TripLevelSet(FAP_IGBT2_OVERTEMP_ITLK_LIM);

/////////////////////////////////////////////////////////////////////////////////////////////

    //Temperature Board configuration
    BoardTempDelay(3); //Inserir valor de delay

    BoardTempEnable(); //BoardTemp enable

    //Temp board configuration limits
    BoardTempAlarmLevelSet(FAP_BOARD_TEMP_ALM_LIM);
    BoardTempTripLevelSet(FAP_BOARD_TEMP_ITLK_LIM);

/////////////////////////////////////////////////////////////////////////////////////////////

    //Humidity Board configuration
    RhDelay(3); //Inserir valor de delay

    RhEnable(); //Rh enable

    //Rh configuration limits
    RhAlarmLevelSet(FAP_RH_ALM_LIM);
    RhTripLevelSet(FAP_RH_ITLK_LIM);

/////////////////////////////////////////////////////////////////////////////////////////////

    //Driver1 error configuration
    Driver1TopErrorEnable(); //Habilitado driver error 1 Top
    Driver1BotErrorDisable(); //Desabilitado driver error 1 Bot
    Driver1OverTempDisable(); //Desabilitado Temperatura por Hardware do modulo 1

    //Driver2 error configuration
    Driver2TopErrorEnable(); //Habilitado driver error 2 Top
    Driver2BotErrorDisable(); //Desabilitado driver error 2 Bot
    Driver2OverTempDisable(); //Desabilitado Temperatura por Hardware do modulo 2

/////////////////////////////////////////////////////////////////////////////////////////////

    //Driver Voltage configuration
    DriverVoltageInit();

    DriverVoltageDelay(3); //Inserir valor de delay

    DriverVoltageEnable(); //DriverVoltage enable

    //Limite de alarme e interlock da tensao dos drivers
    DriverVoltageAlarmLevelSet(FAP_DRIVER_OVERVOLTAGE_ALM_LIM_LIM);
    DriverVoltageTripLevelSet(FAP_DRIVER_OVERVOLTAGE_ITLK_LIM);

/////////////////////////////////////////////////////////////////////////////////////////////

    //Driver Current configuration
    DriverCurrentInit();

    DriverCurrentDelay(3); //Inserir valor de delay

    Driver1CurrentEnable(); //Driver1Current enable
    Driver2CurrentEnable(); //Driver2Current enable

    //Limite de alarme e interlock da corrente do driver 1
    Driver1CurrentAlarmLevelSet(FAP_DRIVER1_OVERCURRENT_ALM_LIM );
    Driver1CurrentTripLevelSet(FAP_DRIVER1_OVERCURRENT_ITLK_LIM);

    //Limite de alarme e interlock da corrente do driver 2
    Driver2CurrentAlarmLevelSet(FAP_DRIVER2_OVERCURRENT_ALM_LIM);
    Driver2CurrentTripLevelSet(FAP_DRIVER2_OVERCURRENT_ITLK_LIM);

/////////////////////////////////////////////////////////////////////////////////////////////

    //Voltage configuration
    VoltageCh1Disable(); //VoltageCh1 disable
    VoltageCh2Disable(); //VoltageCh2 disable
    VoltageCh3Disable(); //VoltageCh3 disable
    VoltageCh4Disable(); //VoltageCh4 disable

/////////////////////////////////////////////////////////////////////////////////////////////

    //Gpdi configuration
    Gpdi1Disable();  //Gpdi1 disable
    Gpdi2Disable();  //Gpdi2 disable
    Gpdi3Disable();  //Gpdi3 disable
    Gpdi4Disable();  //Gpdi4 disable
    Gpdi5Enable();   //Gpdi5 enable ExternalITLK
    Gpdi6Enable();   //Gpdi6 enable RackITLK
    Gpdi7Enable();   //Gpdi7 enable RelayStatus
    Gpdi8Disable();  //Gpdi8 disable
    Gpdi9Disable();  //Gpdi9 disable
    Gpdi10Disable(); //Gpdi10 disable
    Gpdi11Disable(); //Gpdi11 disable
    Gpdi12Disable(); //Gpdi12 disable

    //Gpdo configuration
    Gpdo1Disable();  //Gpdo1 disable
    Gpdo2Disable();  //Gpdo2 disable
    Gpdo3Disable();  //Gpdo3 disable
    Gpdo4Disable();  //Gpdo4 disable

    //ReleAux and ReleExtItlk configuration
    ReleAuxEnable(); //ReleAux enable
    ReleExtItlkEnable(); //ReleExtItlk enable

/////////////////////////////////////////////////////////////////////////////////////////////

    //Init Variables
    fap.Vin.f                        = 0.0;
    fap.VinAlarmSts                  = 0;
    fap.VinItlkSts                   = 0;
    fap.Vout.f                       = 0.0;
    fap.VoutAlarmSts                 = 0;
    fap.VoutItlkSts                  = 0;
    fap.IoutA1.f                     = 0.0;
    fap.IoutA1AlarmSts               = 0;
    fap.IoutA1ItlkSts                = 0;
    fap.IoutA2.f                     = 0.0;
    fap.IoutA2AlarmSts               = 0;
    fap.IoutA2ItlkSts                = 0;
    fap.TempIGBT1.f                  = 0.0;
    fap.TempIGBT1AlarmSts            = 0;
    fap.TempIGBT1ItlkSts             = 0;
    fap.TempIGBT2.f                  = 0.0;
    fap.TempIGBT2AlarmSts            = 0;
    fap.TempIGBT2ItlkSts             = 0;
    fap.DriveVoltage.f               = 0.0;
    fap.DriveVoltageAlarmSts         = 0;
    fap.DriveVoltageItlkSts          = 0;
    fap.Drive1Current.f              = 0.0;
    fap.Drive1CurrentAlarmSts        = 0;
    fap.Drive1CurrentItlkSts         = 0;
    fap.Drive2Current.f              = 0.0;
    fap.Drive2CurrentAlarmSts        = 0;
    fap.Drive2CurrentItlkSts         = 0;
    fap.Driver1Error                 = 0;
    fap.Driver1ErrorItlkSts          = 0;
    fap.Driver2Error                 = 0;
    fap.Driver2ErrorItlkSts          = 0;
    fap.TempL.f                      = 0.0;
    fap.TempLAlarmSts                = 0;
    fap.TempLItlkSts                 = 0;
    fap.TempHeatSink.f               = 0.0;
    fap.TempHeatSinkAlarmSts         = 0;
    fap.TempHeatSinkItlkSts          = 0;
    fap.Relay                        = 0;
    fap.ExternalItlk                 = 0;
    fap.ExternalItlkSts              = 0;
    fap.Rack                         = 0;
    fap.RackItlkSts                  = 0;
    fap.GroundLeakage.f              = 0.0;
    fap.GroundLeakageAlarmSts        = 0;
    fap.GroundLeakageItlkSts         = 0;
    fap.BoardTemperature.f           = 0.0;
    fap.BoardTemperatureAlarmSts     = 0;
    fap.BoardTemperatureItlkSts      = 0;
    fap.RelativeHumidity.f           = 0.0;
    fap.RelativeHumidityAlarmSts     = 0;
    fap.RelativeHumidityItlkSts      = 0;
    fap.ReleAuxItlkSts               = 0;
    fap.ReleExtItlkSts               = 0;
    fap.RelayOpenItlkSts             = 0;
    fap.RelayContactStickingItlkSts  = 0;

}

/////////////////////////////////////////////////////////////////////////////////////////////

