
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "iib_devices/adc_internal.h"

/////////////////////////////////////////////////////////////////////////////////////////////

static int Adc_Value = 0;

/////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
 unsigned char Ch;
 unsigned char Enable;
 float Gain;
 float Value;
 unsigned int Offset;
 float AlarmLimit;
 float TripLimit;
 unsigned char Alarm;
 unsigned char Trip;
 unsigned char InvertPol;
 unsigned char Alarm_Delay_ms;  // milisecond
 unsigned char Alarm_DelayCount;
 unsigned char Itlk_Delay_ms;   // milisecond
 unsigned char Itlk_DelayCount;
}adc_t;

/////////////////////////////////////////////////////////////////////////////////////////////

static adc_t VoltageCh1;
static adc_t VoltageCh2;
static adc_t VoltageCh3;
static adc_t VoltageCh4;

static adc_t CurrentCh1;
static adc_t CurrentCh2;
static adc_t CurrentCh3;
static adc_t CurrentCh4;

static adc_t LvCurrentCh1;
static adc_t LvCurrentCh2;
static adc_t LvCurrentCh3;

static adc_t DriverVolt;
static adc_t Driver1Curr;
static adc_t Driver2Curr;

/////////////////////////////////////////////////////////////////////////////////////////////

static uint32_t adc_0_value[7];
static uint32_t adc_1_value[7];

/////////////////////////////////////////////////////////////////////////////////////////////

void AdcsInit(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);

    // Enable ADC0 and ADC1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC1));

    // Config ADC as a external voltage reference
    ADCReferenceSet(ADC0_BASE, ADC_REF_EXT_3V);
    ADCReferenceSet(ADC1_BASE, ADC_REF_EXT_3V);

    // Select the analog ADC function for these pins.

    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                                    GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2);
    GPIOPinTypeADC(GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2);

    // Enable sample sequence 0 (Max 8 samples) for ADC0 and
    // enable sample sequence 1 (Max 4 samples) for ADC1
    // Both with a processor trigger signal
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);

    // Configure steps on sequence 0 and 1. Here, we are using 7 channels
    // for ADC0 and 4 channels for ADC1
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH5); // VOLTAGE_1
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH4); // VOLTAGE_2
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH6); // VOLTAGE_3
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH7); // VOLTAGE_4
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH13); // LV_2X_SIGNAL1
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH14); // LV_2X_SIGNAL2
    ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH15 | ADC_CTL_IE |
                             ADC_CTL_END); // LV_2X_SIGNAL3
    ADCSequenceStepConfigure(ADC1_BASE, 0, 0, ADC_CTL_CH3); // CURRENT_1
    ADCSequenceStepConfigure(ADC1_BASE, 0, 1, ADC_CTL_CH2); // CURRENT_2
    ADCSequenceStepConfigure(ADC1_BASE, 0, 2, ADC_CTL_CH1); // CURRENT_3
    ADCSequenceStepConfigure(ADC1_BASE, 0, 3, ADC_CTL_CH12); // CURRENT_4
    ADCSequenceStepConfigure(ADC1_BASE, 0, 4, ADC_CTL_CH16); // DRIVER_VOLT
    ADCSequenceStepConfigure(ADC1_BASE, 0, 5, ADC_CTL_CH17); // DRIVER2_AMP
    ADCSequenceStepConfigure(ADC1_BASE, 0, 6, ADC_CTL_CH18 | ADC_CTL_IE |
                             ADC_CTL_END); // DRIVER1_AMP

    // Enable sample sequences.
    ADCSequenceEnable(ADC0_BASE, 0);
    ADCSequenceEnable(ADC1_BASE, 0);

    // Clear the interrupt status flag.  This is done to make sure the
    // interrupt flag is cleared before we sample.
    ADCIntClear(ADC0_BASE, 0);
    ADCIntClear(ADC1_BASE, 0);

    VoltageCh1.Enable = 0;
    VoltageCh2.Enable = 0;
    VoltageCh3.Enable = 0;
    VoltageCh4.Enable = 0;
    
    CurrentCh1.Enable = 0;
    CurrentCh2.Enable = 0;
    CurrentCh3.Enable = 0;
    CurrentCh4.Enable = 0;
    
    LvCurrentCh1.Enable = 0;
    LvCurrentCh2.Enable = 0;
    LvCurrentCh3.Enable = 0;

    DriverVolt.Enable = 0;
    Driver1Curr.Enable = 0;
    Driver2Curr.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void sample_adc(void)
{

    // Trigger the ADC0 conversion.
    ADCProcessorTrigger(ADC0_BASE, 0);

    // Wait for conversion to be completed.
    while(!ADCIntStatus(ADC0_BASE, 0, false)){}

    // Clear the ADC interrupt flag.
    ADCIntClear(ADC0_BASE, 0);

    // Read ADC Value.
    ADCSequenceDataGet(ADC0_BASE, 0, adc_0_value);

    // Trigger the ADC1 conversion.
    ADCProcessorTrigger(ADC1_BASE, 0);

    // Wait for conversion to be completed.
    while(!ADCIntStatus(ADC1_BASE, 0, false)){}

    // Clear the ADC interrupt flag.
    ADCIntClear(ADC1_BASE, 0);

    // Read ADC Value.
    ADCSequenceDataGet(ADC1_BASE, 0, adc_1_value);

}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh1Init(float nValue, unsigned char Delay)
{
    VoltageCh1.Ch = 1;
    VoltageCh1.Enable = 0;
    VoltageCh1.Gain = (nValue/2048.0);
    VoltageCh1.Value = 0.0;
    VoltageCh1.Offset = 0x0800; //OffsetRead(OFFSET_VOLT_CH1);
    VoltageCh1.AlarmLimit = 10; //VoltCh1AlarmLevelMemmoryRead();
    VoltageCh1.TripLimit = 10; //VoltCh1TripLevelMemmoryRead();
    VoltageCh1.Alarm = 0;
    VoltageCh1.Trip = 0;
    VoltageCh1.InvertPol = 0;
    VoltageCh1.Alarm_Delay_ms = Delay;
    VoltageCh1.Alarm_DelayCount = 0;
    VoltageCh1.Itlk_Delay_ms = Delay;
    VoltageCh1.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh2Init(float nValue, unsigned char Delay)
{
    VoltageCh2.Ch = 2;
    VoltageCh2.Enable = 0;
    VoltageCh2.Gain = (nValue/2048.0);
    VoltageCh2.Value = 0.0;
    VoltageCh2.Offset = 0x0800; //OffsetRead(OFFSET_VOLT_CH2);
    VoltageCh2.AlarmLimit = 10; //VoltCh2AlarmLevelMemmoryRead();
    VoltageCh2.TripLimit = 10; //VoltCh2TripLevelMemmoryRead();
    VoltageCh2.Alarm = 0;
    VoltageCh2.Trip = 0;
    VoltageCh2.InvertPol = 0;
    VoltageCh2.Alarm_Delay_ms = Delay;
    VoltageCh2.Alarm_DelayCount = 0;
    VoltageCh2.Itlk_Delay_ms = Delay;
    VoltageCh2.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh3Init(float nValue, unsigned char Delay)
{
    VoltageCh3.Ch = 3;
    VoltageCh3.Enable = 0;
    VoltageCh3.Gain = (nValue/2048.0);
    VoltageCh3.Value = 0.0;
    VoltageCh3.Offset = 0x0800; //OffsetRead(OFFSET_VOLT_CH3);
    VoltageCh3.AlarmLimit = 10; //VoltCh3AlarmLevelMemmoryRead();
    VoltageCh3.TripLimit = 10; //VoltCh3TripLevelMemmoryRead();
    VoltageCh3.Alarm = 0;
    VoltageCh3.Trip = 0;
    VoltageCh3.InvertPol = 0;
    VoltageCh3.Alarm_Delay_ms = Delay;
    VoltageCh3.Alarm_DelayCount = 0;
    VoltageCh3.Itlk_Delay_ms = Delay;
    VoltageCh3.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh4Init(float nValue, unsigned char Delay)
{
    VoltageCh4.Ch = 4;
    VoltageCh4.Enable = 0;
    VoltageCh4.Gain = (nValue/2048.0);
    VoltageCh4.Value = 0.0;
    VoltageCh4.Offset = 0x0800; //OffsetRead(OFFSET_VOLT_CH4);
    VoltageCh4.AlarmLimit = 10; //VoltCh4AlarmLevelMemmoryRead();
    VoltageCh4.TripLimit = 10; //VoltCh4TripLevelMemmoryRead();
    VoltageCh4.Alarm = 0;
    VoltageCh4.Trip = 0;
    VoltageCh4.InvertPol = 0;
    VoltageCh4.Alarm_Delay_ms = Delay;
    VoltageCh4.Alarm_DelayCount = 0;
    VoltageCh4.Itlk_Delay_ms = Delay;
    VoltageCh4.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

float CurrentRange(float nFstCurr, float nSecCurr, float nBurden, float MaxVoltInput)
{
    float Ix, Xv = 0.0;

    Xv = nSecCurr*nBurden;
    Ix = nFstCurr*MaxVoltInput;
    Ix = Ix/Xv;
    
    return Ix;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh1Init(float nFstCurr, float nSecCurr, float nBurden, unsigned char delay_ms)
{
    CurrentCh1.Ch = 1;
    CurrentCh1.Enable = 0;
    CurrentCh1.Gain = (CurrentRange(nFstCurr, nSecCurr, nBurden, 7.5)/2048.0);
    CurrentCh1.Value = 0.0;
    CurrentCh1.Offset = 0x0800; //OffsetRead(OFFSET_HALL_CH1);
    CurrentCh1.AlarmLimit = 10.0; //CurrCh1AlarmLevelMemmoryRead();
    CurrentCh1.TripLimit = 10.0; //CurrCh1TripLevelMemmoryRead();
    CurrentCh1.Alarm = 0;
    CurrentCh1.Trip = 0;
    CurrentCh1.InvertPol = 0;
    CurrentCh1.Alarm_Delay_ms = delay_ms;
    CurrentCh1.Alarm_DelayCount = 0;
    CurrentCh1.Itlk_Delay_ms = delay_ms;
    CurrentCh1.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh2Init(float nFstCurr, float nSecCurr, float nBurden, unsigned char delay_ms)
{
    CurrentCh2.Ch = 2;
    CurrentCh2.Enable = 0;
    CurrentCh2.Gain = (CurrentRange(nFstCurr, nSecCurr, nBurden, 7.5)/2048.0);
    CurrentCh2.Value = 0.0;
    CurrentCh2.Offset = 0x0800; //OffsetRead(OFFSET_HALL_CH2);
    CurrentCh2.AlarmLimit = 10.0; //CurrCh2AlarmLevelMemmoryRead();
    CurrentCh2.TripLimit = 10.0; //CurrCh2TripLevelMemmoryRead();
    CurrentCh2.Alarm = 0;
    CurrentCh2.Trip = 0;
    CurrentCh2.InvertPol = 0;
    CurrentCh2.Alarm_Delay_ms = delay_ms;
    CurrentCh2.Alarm_DelayCount = 0;
    CurrentCh2.Itlk_Delay_ms = delay_ms;
    CurrentCh2.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh3Init(float nFstCurr, float nSecCurr, float nBurden, unsigned char delay_ms)
{
    CurrentCh3.Ch = 3;
    CurrentCh3.Enable = 0;
    CurrentCh3.Gain = (CurrentRange(nFstCurr, nSecCurr, nBurden, 7.5)/2048.0);
    CurrentCh3.Value = 0.0;
    CurrentCh3.Offset = 0x0800; //OffsetRead(OFFSET_HALL_CH3);
    CurrentCh3.AlarmLimit = 10.0; //CurrCh3AlarmLevelMemmoryRead();
    CurrentCh3.TripLimit = 10.0; //CurrCh3TripLevelMemmoryRead();
    CurrentCh3.Alarm = 0;
    CurrentCh3.Trip = 0;
    CurrentCh3.InvertPol = 0;
    CurrentCh3.Alarm_Delay_ms = delay_ms;
    CurrentCh3.Alarm_DelayCount = 0;
    CurrentCh3.Itlk_Delay_ms = delay_ms;
    CurrentCh3.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh4Init(float nFstCurr, float nSecCurr, float nBurden, unsigned char delay_ms)
{
    CurrentCh4.Ch = 4;
    CurrentCh4.Enable = 0;
    CurrentCh4.Gain = (CurrentRange(nFstCurr, nSecCurr, nBurden, 7.5)/2048.0);
    CurrentCh4.Value = 0.0;
    CurrentCh4.Offset = 0x0800; //OffsetRead(OFFSET_HALL_CH4);
    CurrentCh4.AlarmLimit = 10.0; //CurrCh4AlarmLevelMemmoryRead();
    CurrentCh4.TripLimit = 10.0; //CurrCh4TripLevelMemmoryRead();
    CurrentCh4.Alarm = 0;
    CurrentCh4.Trip = 0;
    CurrentCh4.InvertPol = 0;
    CurrentCh4.Alarm_Delay_ms = delay_ms;
    CurrentCh4.Alarm_DelayCount = 0;
    CurrentCh4.Itlk_Delay_ms = delay_ms;
    CurrentCh4.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LvCurrentCh1Init(float nFstCurr, float nSecCurr, float nBurden, unsigned char delay_ms)
{
    LvCurrentCh1.Ch = 1;
    LvCurrentCh1.Enable = 0;
    LvCurrentCh1.Gain = (CurrentRange(nFstCurr, nSecCurr, nBurden, 3.0)/2048.0);
    LvCurrentCh1.Value = 0.0;
    LvCurrentCh1.Offset = 0x0800; //OffsetRead(OFFSET_HALL_CH1);
    LvCurrentCh1.AlarmLimit = 10.0; //LvCurrentCh1AlarmLevelMemmoryRead();
    LvCurrentCh1.TripLimit = 10.0; //LvCurrentCh1TripLevelMemmoryRead();
    LvCurrentCh1.Alarm = 0;
    LvCurrentCh1.Trip = 0;
    LvCurrentCh1.InvertPol = 0;
    LvCurrentCh1.Alarm_Delay_ms = delay_ms;
    LvCurrentCh1.Alarm_DelayCount = 0;
    LvCurrentCh1.Itlk_Delay_ms = delay_ms;
    LvCurrentCh1.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LvCurrentCh2Init(float nFstCurr, float nSecCurr, float nBurden, unsigned char delay_ms)
{
    LvCurrentCh2.Ch = 2;
    LvCurrentCh2.Enable = 0;
    LvCurrentCh2.Gain = (CurrentRange(nFstCurr, nSecCurr, nBurden, 3.0)/2048.0);
    LvCurrentCh2.Value = 0.0;
    LvCurrentCh2.Offset = 0x0800; //OffsetRead(OFFSET_HALL_CH2);
    LvCurrentCh2.AlarmLimit = 10.0; //LvCurrentCh2AlarmLevelMemmoryRead();
    LvCurrentCh2.TripLimit = 10.0; //LvCurrentCh2TripLevelMemmoryRead();
    LvCurrentCh2.Alarm = 0;
    LvCurrentCh2.Trip = 0;
    LvCurrentCh2.InvertPol = 0;
    LvCurrentCh2.Alarm_Delay_ms = delay_ms;
    LvCurrentCh2.Alarm_DelayCount = 0;
    LvCurrentCh2.Itlk_Delay_ms = delay_ms;
    LvCurrentCh2.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LvCurrentCh3Init(float nFstCurr, float nSecCurr, float nBurden, unsigned char delay_ms)
{
    LvCurrentCh3.Ch = 3;
    LvCurrentCh3.Enable = 0;
    LvCurrentCh3.Gain = (CurrentRange(nFstCurr, nSecCurr, nBurden, 3.0)/2048.0);
    LvCurrentCh3.Value = 0.0;
    LvCurrentCh3.Offset = 0x0800; //OffsetRead(OFFSET_HALL_CH3);
    LvCurrentCh3.AlarmLimit = 10.0; //LvCurrentCh3AlarmLevelMemmoryRead();
    LvCurrentCh3.TripLimit = 10.0; //LvCurrentCh3TripLevelMemmoryRead();
    LvCurrentCh3.Alarm = 0;
    LvCurrentCh3.Trip = 0;
    LvCurrentCh3.InvertPol = 0;
    LvCurrentCh3.Alarm_Delay_ms = delay_ms;
    LvCurrentCh3.Alarm_DelayCount = 0;
    LvCurrentCh3.Itlk_Delay_ms = delay_ms;
    LvCurrentCh3.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void DriverVoltageInit(void)
{
    DriverVolt.Ch = 1;
    DriverVolt.Enable = 0;
    DriverVolt.Gain = 0.00439453125; // 18V/4096
    DriverVolt.Value = 0.0;
    DriverVolt.Offset = 0x0000;
    DriverVolt.AlarmLimit = 16.0;
    DriverVolt.TripLimit = 17.0;
    DriverVolt.Alarm = 0;
    DriverVolt.Trip = 0;
    DriverVolt.InvertPol = 0;
    DriverVolt.Alarm_Delay_ms = 0;
    DriverVolt.Alarm_DelayCount = 0;
    DriverVolt.Itlk_Delay_ms = 0;
    DriverVolt.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void DriverCurrentInit(void)
{
    Driver1Curr.Ch = 1;
    Driver1Curr.Enable = 0;
    Driver1Curr.Gain = 0.003662109375; // 7,5A/2048
    Driver1Curr.Value = 0.0;
    Driver1Curr.Offset = 0x0800; //OffsetRead(OFFSET_DRIVE_CURRENT1);
    Driver1Curr.AlarmLimit = 2.0;
    Driver1Curr.TripLimit = 2.0;
    Driver1Curr.Alarm = 0;
    Driver1Curr.Trip = 0;
    Driver1Curr.InvertPol = 0;
    Driver1Curr.Alarm_Delay_ms = 0;
    Driver1Curr.Alarm_DelayCount = 0;
    Driver1Curr.Itlk_Delay_ms = 0;
    Driver1Curr.Itlk_DelayCount = 0;

    Driver2Curr.Ch = 1;
    Driver2Curr.Enable = 0;
    Driver2Curr.Gain = 0.003662109375; // 7,5A/2048
    Driver2Curr.Value = 0.0;
    Driver2Curr.Offset = 0x0800; //OffsetRead(OFFSET_DRIVE_CURRENT2);
    Driver2Curr.AlarmLimit = 2.0;
    Driver2Curr.TripLimit = 2.0;
    Driver2Curr.Alarm = 0;
    Driver2Curr.Trip = 0;
    Driver2Curr.InvertPol = 0;
    Driver2Curr.Alarm_Delay_ms = 0;
    Driver2Curr.Alarm_DelayCount = 0;
    Driver2Curr.Itlk_Delay_ms = 0;
    Driver2Curr.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh1Sample(void)
{
    Adc_Value = adc_0_value[0];
    Adc_Value = Adc_Value - VoltageCh1.Offset;
    VoltageCh1.Value = (float)Adc_Value * VoltageCh1.Gain;
    
    if(VoltageCh1.InvertPol) VoltageCh1.Value = VoltageCh1.Value * -1.0;
    
    if(VoltageCh1.Value > VoltageCh1.AlarmLimit || VoltageCh1.Value < -VoltageCh1.AlarmLimit)
    {
        if(VoltageCh1.Alarm_DelayCount < VoltageCh1.Alarm_Delay_ms) VoltageCh1.Alarm_DelayCount++;
        else
        {
           VoltageCh1.Alarm_DelayCount = 0;
           VoltageCh1.Alarm = 1;
        }
    }
    else VoltageCh1.Alarm_DelayCount = 0;

    if(VoltageCh1.Value > VoltageCh1.TripLimit || VoltageCh1.Value < -VoltageCh1.TripLimit)
    {
        if(VoltageCh1.Itlk_DelayCount < VoltageCh1.Itlk_Delay_ms) VoltageCh1.Itlk_DelayCount++;
        else
        {
           VoltageCh1.Itlk_DelayCount = 0;
           VoltageCh1.Trip = 1;
        }
    }
    else VoltageCh1.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh2Sample(void)
{
    Adc_Value = adc_0_value[1];
    Adc_Value = Adc_Value - VoltageCh2.Offset;
    VoltageCh2.Value = (float)Adc_Value * VoltageCh2.Gain;

    if(VoltageCh2.InvertPol) VoltageCh2.Value = VoltageCh2.Value * -1.0;

    if(VoltageCh2.Value > VoltageCh2.AlarmLimit || VoltageCh2.Value < -VoltageCh2.AlarmLimit)
    {
        if(VoltageCh2.Alarm_DelayCount < VoltageCh2.Alarm_Delay_ms) VoltageCh2.Alarm_DelayCount++;
        else
        {
           VoltageCh2.Alarm_DelayCount = 0;
           VoltageCh2.Alarm = 1;
        }
    }
    else VoltageCh2.Alarm_DelayCount = 0;

    if(VoltageCh2.Value > VoltageCh2.TripLimit || VoltageCh2.Value < -VoltageCh2.TripLimit)
    {
        if(VoltageCh2.Itlk_DelayCount < VoltageCh2.Itlk_Delay_ms) VoltageCh2.Itlk_DelayCount++;
        else
        {
           VoltageCh2.Itlk_DelayCount = 0;
           VoltageCh2.Trip = 1;
        }
    }
    else VoltageCh2.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh3Sample(void)
{
    Adc_Value = adc_0_value[2];
    Adc_Value = Adc_Value - VoltageCh3.Offset;
    VoltageCh3.Value = (float)Adc_Value * VoltageCh3.Gain;
    
    if(VoltageCh3.InvertPol) VoltageCh3.Value = VoltageCh3.Value * -1.0;

    if(VoltageCh3.Value > VoltageCh3.AlarmLimit || VoltageCh3.Value < -VoltageCh3.AlarmLimit)
    {
        if(VoltageCh3.Alarm_DelayCount < VoltageCh3.Alarm_Delay_ms) VoltageCh3.Alarm_DelayCount++;
        else
        {
           VoltageCh3.Alarm_DelayCount = 0;
           VoltageCh3.Alarm = 1;
        }
    }
    else VoltageCh3.Alarm_DelayCount = 0;

    if(VoltageCh3.Value > VoltageCh3.TripLimit || VoltageCh3.Value < -VoltageCh3.TripLimit)
    {
        if(VoltageCh3.Itlk_DelayCount < VoltageCh3.Itlk_Delay_ms) VoltageCh3.Itlk_DelayCount++;
        else
        {
           VoltageCh3.Itlk_DelayCount = 0;
           VoltageCh3.Trip = 1;
        }
    }
    else VoltageCh3.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh4Sample(void)
{
    Adc_Value = adc_0_value[3];
    Adc_Value = Adc_Value - VoltageCh4.Offset;
    VoltageCh4.Value = (float)Adc_Value * VoltageCh4.Gain;
    
    if(VoltageCh4.InvertPol) VoltageCh4.Value = VoltageCh4.Value * -1.0;

    if(VoltageCh4.Value > VoltageCh4.AlarmLimit || VoltageCh4.Value < -VoltageCh4.AlarmLimit)
    {
        if(VoltageCh4.Alarm_DelayCount < VoltageCh4.Alarm_Delay_ms) VoltageCh4.Alarm_DelayCount++;
        else
        {
           VoltageCh4.Alarm_DelayCount = 0;
           VoltageCh4.Alarm = 1;
        }
    }
    else VoltageCh4.Alarm_DelayCount = 0;

    if(VoltageCh4.Value > VoltageCh4.TripLimit || VoltageCh4.Value < -VoltageCh4.TripLimit)
    {
        if(VoltageCh4.Itlk_DelayCount < VoltageCh4.Itlk_Delay_ms) VoltageCh4.Itlk_DelayCount++;
        else
        {
           VoltageCh4.Itlk_DelayCount = 0;
           VoltageCh4.Trip = 1;
        }
    }
    else VoltageCh4.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh1Sample(void)
{
    Adc_Value = adc_1_value[0];
    Adc_Value = Adc_Value - CurrentCh1.Offset;
    CurrentCh1.Value = (float)Adc_Value * CurrentCh1.Gain;

    if(CurrentCh1.InvertPol) CurrentCh1.Value = CurrentCh1.Value * -1.0;

    if(CurrentCh1.Value > CurrentCh1.AlarmLimit || CurrentCh1.Value < -CurrentCh1.AlarmLimit)
    {
        if(CurrentCh1.Alarm_DelayCount < CurrentCh1.Alarm_Delay_ms) CurrentCh1.Alarm_DelayCount++;
        else
        {
           CurrentCh1.Alarm_DelayCount = 0;
           CurrentCh1.Alarm = 1;
        }
    }
    else CurrentCh1.Alarm_DelayCount = 0;

    if(CurrentCh1.Value > CurrentCh1.TripLimit || CurrentCh1.Value < -CurrentCh1.TripLimit)
    {
        if(CurrentCh1.Itlk_DelayCount < CurrentCh1.Itlk_Delay_ms) CurrentCh1.Itlk_DelayCount++;
        else
        {
           CurrentCh1.Itlk_DelayCount = 0;
           CurrentCh1.Trip = 1;
        }
    }
    else CurrentCh1.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh2Sample(void)
{
    Adc_Value = adc_1_value[1];
    Adc_Value = Adc_Value - CurrentCh2.Offset;
    CurrentCh2.Value = (float)Adc_Value * CurrentCh2.Gain;

    if(CurrentCh2.InvertPol) CurrentCh2.Value = CurrentCh2.Value * -1.0;

    if(CurrentCh2.Value > CurrentCh2.AlarmLimit || CurrentCh2.Value < -CurrentCh2.AlarmLimit)
    {
        if(CurrentCh2.Alarm_DelayCount < CurrentCh2.Alarm_Delay_ms) CurrentCh2.Alarm_DelayCount++;
        else
        {
           CurrentCh2.Alarm_DelayCount = 0;
           CurrentCh2.Alarm = 1;
        }
    }
    else CurrentCh2.Alarm_DelayCount = 0;

    if(CurrentCh2.Value > CurrentCh2.TripLimit || CurrentCh2.Value < -CurrentCh2.TripLimit)
    {
       if(CurrentCh2.Itlk_DelayCount < CurrentCh2.Itlk_Delay_ms) CurrentCh2.Itlk_DelayCount++;
       else
       {
          CurrentCh2.Itlk_DelayCount = 0;
          CurrentCh2.Trip = 1;
       }
    }
    else CurrentCh2.Itlk_DelayCount = 0;

}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh3Sample(void)
{
    Adc_Value = adc_1_value[2];
    Adc_Value = Adc_Value - CurrentCh3.Offset;
    CurrentCh3.Value = (float)Adc_Value * CurrentCh3.Gain;

    if(CurrentCh3.InvertPol) CurrentCh3.Value = CurrentCh3.Value * -1.0;

    if(CurrentCh3.Value > CurrentCh3.AlarmLimit || CurrentCh3.Value < -CurrentCh3.AlarmLimit)
    {
        if(CurrentCh3.Alarm_DelayCount < CurrentCh3.Alarm_Delay_ms) CurrentCh3.Alarm_DelayCount++;
        else
        {
           CurrentCh3.Alarm_DelayCount = 0;
           CurrentCh3.Alarm = 1;
        }
    }
    else CurrentCh3.Alarm_DelayCount = 0;

    if(CurrentCh3.Value > CurrentCh3.TripLimit || CurrentCh3.Value < -CurrentCh3.TripLimit)
    {
       if(CurrentCh3.Itlk_DelayCount < CurrentCh3.Itlk_Delay_ms) CurrentCh3.Itlk_DelayCount++;
       else
       {
          CurrentCh3.Itlk_DelayCount = 0;
          CurrentCh3.Trip = 1;
       }
    }
    else CurrentCh3.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh4Sample(void)
{
    Adc_Value = adc_1_value[3];
    Adc_Value = Adc_Value - CurrentCh4.Offset;
    CurrentCh4.Value = (float)Adc_Value * CurrentCh4.Gain;

    if(CurrentCh4.InvertPol) CurrentCh4.Value = CurrentCh4.Value * -1.0;

    if(CurrentCh4.Value > CurrentCh4.AlarmLimit || CurrentCh4.Value < -CurrentCh4.AlarmLimit)
    {
        if(CurrentCh4.Alarm_DelayCount < CurrentCh4.Alarm_Delay_ms) CurrentCh4.Alarm_DelayCount++;
        else
        {
           CurrentCh4.Alarm_DelayCount = 0;
           CurrentCh4.Alarm = 1;
        }
    }
    else CurrentCh4.Alarm_DelayCount = 0;

    if(CurrentCh4.Value > CurrentCh4.TripLimit || CurrentCh4.Value < -CurrentCh4.TripLimit)
    {
        if(CurrentCh4.Itlk_DelayCount < CurrentCh4.Itlk_Delay_ms) CurrentCh4.Itlk_DelayCount++;
        else
        {
           CurrentCh4.Itlk_DelayCount = 0;
           CurrentCh4.Trip = 1;
        }
    }
    else CurrentCh4.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LvCurrentCh1Sample(void)
{
    Adc_Value = adc_0_value[4];
    Adc_Value = Adc_Value - LvCurrentCh1.Offset;
    LvCurrentCh1.Value = (float)Adc_Value * LvCurrentCh1.Gain;

    if(LvCurrentCh1.InvertPol) LvCurrentCh1.Value = LvCurrentCh1.Value * -1.0;

    if(LvCurrentCh1.Value > LvCurrentCh1.AlarmLimit || LvCurrentCh1.Value < -LvCurrentCh1.AlarmLimit)
    {
        if(LvCurrentCh1.Alarm_DelayCount < LvCurrentCh1.Alarm_Delay_ms) LvCurrentCh1.Alarm_DelayCount++;
        else
        {
            LvCurrentCh1.Alarm_DelayCount = 0;
            LvCurrentCh1.Alarm = 1;
        }
    }
    else LvCurrentCh1.Alarm_DelayCount = 0;

    if(LvCurrentCh1.Value > LvCurrentCh1.TripLimit || LvCurrentCh1.Value < -LvCurrentCh1.TripLimit)
    {
        if(LvCurrentCh1.Itlk_DelayCount < LvCurrentCh1.Itlk_Delay_ms) LvCurrentCh1.Itlk_DelayCount++;
        else
        {
           LvCurrentCh1.Itlk_DelayCount = 0;
           LvCurrentCh1.Trip = 1;
        }
    }
    else LvCurrentCh1.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LvCurrentCh2Sample(void)
{
    Adc_Value = adc_0_value[5];
    Adc_Value = Adc_Value - LvCurrentCh2.Offset;
    LvCurrentCh2.Value = (float)Adc_Value * LvCurrentCh2.Gain;

    if(LvCurrentCh2.InvertPol) LvCurrentCh2.Value = LvCurrentCh2.Value * -1.0;

    if(LvCurrentCh2.Value > LvCurrentCh2.AlarmLimit || LvCurrentCh2.Value < -LvCurrentCh2.AlarmLimit)
    {
        if(LvCurrentCh2.Alarm_DelayCount < LvCurrentCh2.Alarm_Delay_ms) LvCurrentCh2.Alarm_DelayCount++;
        else
        {
            LvCurrentCh2.Alarm_DelayCount = 0;
            LvCurrentCh2.Alarm = 1;
        }
    }
    else LvCurrentCh2.Alarm_DelayCount = 0;

    if(LvCurrentCh2.Value > LvCurrentCh2.TripLimit || LvCurrentCh2.Value < -LvCurrentCh2.TripLimit)
    {
        if(LvCurrentCh2.Itlk_DelayCount < LvCurrentCh2.Itlk_Delay_ms) LvCurrentCh2.Itlk_DelayCount++;
        else
        {
           LvCurrentCh2.Itlk_DelayCount = 0;
           LvCurrentCh2.Trip = 1;
        }
    }
    else LvCurrentCh2.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LvCurrentCh3Sample(void)
{
    Adc_Value = adc_0_value[6];
    Adc_Value = Adc_Value - LvCurrentCh3.Offset;
    LvCurrentCh3.Value = (float)Adc_Value * LvCurrentCh3.Gain;

    if(LvCurrentCh3.InvertPol) LvCurrentCh3.Value = LvCurrentCh3.Value * -1.0;

    if(LvCurrentCh3.Value > LvCurrentCh3.AlarmLimit || LvCurrentCh3.Value < -LvCurrentCh3.AlarmLimit)
    {
        if(LvCurrentCh3.Alarm_DelayCount < LvCurrentCh3.Alarm_Delay_ms) LvCurrentCh3.Alarm_DelayCount++;
        else
        {
            LvCurrentCh3.Alarm_DelayCount = 0;
            LvCurrentCh3.Alarm = 1;
        }
    }
    else LvCurrentCh3.Alarm_DelayCount = 0;

    if(LvCurrentCh3.Value > LvCurrentCh3.TripLimit || LvCurrentCh3.Value < -LvCurrentCh3.TripLimit)
    {
        if(LvCurrentCh3.Itlk_DelayCount < LvCurrentCh3.Itlk_Delay_ms) LvCurrentCh3.Itlk_DelayCount++;
        else
        {
           LvCurrentCh3.Itlk_DelayCount = 0;
           LvCurrentCh3.Trip = 1;
        }
    }
    else LvCurrentCh3.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void DriverVoltageSample(void)
{
    Adc_Value = adc_1_value[4];
    DriverVolt.Value = (float)Adc_Value * DriverVolt.Gain;
    
    if(DriverVolt.Value > DriverVolt.AlarmLimit)
    {
        if(DriverVolt.Alarm_DelayCount < DriverVolt.Alarm_Delay_ms) DriverVolt.Alarm_DelayCount++;
        else
        {
           DriverVolt.Alarm_DelayCount = 0;
           DriverVolt.Alarm = 1;
        }
    }
    else DriverVolt.Alarm_DelayCount = 0;
    
    if(DriverVolt.Value > DriverVolt.TripLimit)
    {
        if(DriverVolt.Itlk_DelayCount < DriverVolt.Itlk_Delay_ms) DriverVolt.Itlk_DelayCount++;
        else
        {
           DriverVolt.Itlk_DelayCount = 0;
           DriverVolt.Trip = 1;
        }
    }
    else DriverVolt.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver1CurrentSample(void)
{
    Adc_Value = adc_1_value[6];
    Adc_Value = Adc_Value - Driver1Curr.Offset;
    Driver1Curr.Value = (float)Adc_Value * Driver1Curr.Gain;
    
    if(Driver1Curr.Value > Driver1Curr.AlarmLimit)
    {
        if(Driver1Curr.Alarm_DelayCount < Driver1Curr.Alarm_Delay_ms) Driver1Curr.Alarm_DelayCount++;
        else
        {
           Driver1Curr.Alarm_DelayCount = 0;
           Driver1Curr.Alarm = 1;
        }
    }
    else Driver1Curr.Alarm_DelayCount = 0;
    
    if(Driver1Curr.Value > Driver1Curr.TripLimit)
    {
        if(Driver1Curr.Itlk_DelayCount < Driver1Curr.Itlk_Delay_ms) Driver1Curr.Itlk_DelayCount++;
        else
        {
           Driver1Curr.Itlk_DelayCount = 0;
           Driver1Curr.Trip = 1;
        }
    }
    else Driver1Curr.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver2CurrentSample(void)
{
    Adc_Value = adc_1_value[5];
    Adc_Value = Adc_Value - Driver2Curr.Offset;
    Driver2Curr.Value = (float)Adc_Value * Driver2Curr.Gain;

    if(Driver2Curr.Value > Driver2Curr.AlarmLimit)
    {
        if(Driver2Curr.Alarm_DelayCount < Driver2Curr.Alarm_Delay_ms) Driver2Curr.Alarm_DelayCount++;
        else
        {
           Driver2Curr.Alarm_DelayCount = 0;
           Driver2Curr.Alarm = 1;
        }
    }
    else Driver2Curr.Alarm_DelayCount = 0;

    if(Driver2Curr.Value > Driver2Curr.TripLimit)
    {
        if(Driver2Curr.Itlk_DelayCount < Driver2Curr.Itlk_Delay_ms) Driver2Curr.Itlk_DelayCount++;
        else
        {
           Driver2Curr.Itlk_DelayCount = 0;
           Driver2Curr.Trip = 1;
        }
    }
    else Driver2Curr.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void ConfigPolVoltCh1(unsigned char sts)
{
    VoltageCh1.InvertPol = sts;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void ConfigPolVoltCh2(unsigned char sts)
{
    VoltageCh2.InvertPol = sts;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void ConfigPolVoltCh3(unsigned char sts)
{
    VoltageCh3.InvertPol = sts;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void ConfigPolVoltCh4(unsigned char sts)
{
    VoltageCh4.InvertPol = sts;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void ConfigPolCurrCh1(unsigned char sts)
{
    CurrentCh1.InvertPol = sts;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void ConfigPolCurrCh2(unsigned char sts)
{
    CurrentCh2.InvertPol = sts;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void ConfigPolCurrCh3(unsigned char sts)
{
    CurrentCh3.InvertPol = sts;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void ConfigPolCurrCh4(unsigned char sts)
{
    CurrentCh4.InvertPol = sts;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void ConfigPolLvCurrCh1(unsigned char sts)
{
    LvCurrentCh1.InvertPol = sts;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void ConfigPolLvCurrCh2(unsigned char sts)
{
    LvCurrentCh2.InvertPol = sts;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void ConfigPolLvCurrCh3(unsigned char sts)
{
    LvCurrentCh3.InvertPol = sts;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh1Enable(void)
{
    VoltageCh1.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh1Disable(void)
{
    VoltageCh1.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh2Enable(void)
{
    VoltageCh2.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh2Disable(void)
{
    VoltageCh2.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh3Enable(void)
{
    VoltageCh3.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh3Disable(void)
{
    VoltageCh3.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh4Enable(void)
{
    VoltageCh4.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh4Disable(void)
{
    VoltageCh4.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh1Enable(void)
{
    CurrentCh1.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh1Disable(void)
{
    CurrentCh1.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh2Enable(void)
{
    CurrentCh2.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh2Disable(void)
{
    CurrentCh2.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh3Enable(void)
{
    CurrentCh3.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh3Disable(void)
{
    CurrentCh3.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh4Enable(void)
{
    CurrentCh4.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh4Disable(void)
{
    CurrentCh4.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LvCurrentCh1Enable(void)
{
    LvCurrentCh1.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LvCurrentCh1Disable(void)
{
    LvCurrentCh1.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LvCurrentCh2Enable(void)
{
    LvCurrentCh2.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LvCurrentCh2Disable(void)
{
    LvCurrentCh2.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LvCurrentCh3Enable(void)
{
    LvCurrentCh3.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LvCurrentCh3Disable(void)
{
    LvCurrentCh3.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void DriverVoltageEnable(void)
{
    DriverVolt.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void DriverVoltageDisable(void)
{
    DriverVolt.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver1CurrentEnable(void)
{
    Driver1Curr.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver1CurrentDisable(void)
{
    Driver1Curr.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver2CurrentEnable(void)
{
    Driver2Curr.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver2CurrentDisable(void)
{
    Driver2Curr.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

float VoltageCh1Read(void)
{
    if(VoltageCh1.Enable)return VoltageCh1.Value;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

float VoltageCh2Read(void)
{
    if(VoltageCh2.Enable)return VoltageCh2.Value;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

float VoltageCh3Read(void)
{
    if(VoltageCh3.Enable)return VoltageCh3.Value;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

float VoltageCh4Read(void)
{
    if(VoltageCh4.Enable)return VoltageCh4.Value;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

float CurrentCh1Read(void)
{
    if(CurrentCh1.Enable)return CurrentCh1.Value;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

float CurrentCh2Read(void)
{
    if(CurrentCh2.Enable)return CurrentCh2.Value;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

float CurrentCh3Read(void)
{
    if(CurrentCh3.Enable)return CurrentCh3.Value;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

float CurrentCh4Read(void)
{
    if(CurrentCh4.Enable)return CurrentCh4.Value;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

float LvCurrentCh1Read(void)
{
    if(LvCurrentCh1.Enable)return LvCurrentCh1.Value;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

float LvCurrentCh2Read(void)
{
    if(LvCurrentCh2.Enable)return LvCurrentCh2.Value;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

float LvCurrentCh3Read(void)
{
    if(LvCurrentCh3.Enable)return LvCurrentCh3.Value;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

float DriverVoltageRead(void)
{
   if(DriverVolt.Enable)return (DriverVolt.Value);
   else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

float Driver1CurrentRead(void)
{
   if(Driver1Curr.Enable)return (Driver1Curr.Value);
   else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

float Driver2CurrentRead(void)
{
   if(Driver2Curr.Enable)return (Driver2Curr.Value);
   else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh1AlarmLevelSet(float nValue)
{
    VoltageCh1.AlarmLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh1TripLevelSet(float nValue)
{
    VoltageCh1.TripLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh2AlarmLevelSet(float nValue)
{
    VoltageCh2.AlarmLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh2TripLevelSet(float nValue)
{
    VoltageCh2.TripLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh3AlarmLevelSet(float nValue)
{
    VoltageCh3.AlarmLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh3TripLevelSet(float nValue)
{
    VoltageCh3.TripLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh4AlarmLevelSet(float nValue)
{
    VoltageCh4.AlarmLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void VoltageCh4TripLevelSet(float nValue)
{
    VoltageCh4.TripLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh1AlarmLevelSet(float nValue)
{
    CurrentCh1.AlarmLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh1TripLevelSet(float nValue)
{
    CurrentCh1.TripLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh2AlarmLevelSet(float nValue)
{
    CurrentCh2.AlarmLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh2TripLevelSet(float nValue)
{
    CurrentCh2.TripLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh3AlarmLevelSet(float nValue)
{
    CurrentCh3.AlarmLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh3TripLevelSet(float nValue)
{
    CurrentCh3.TripLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh4AlarmLevelSet(float nValue)
{
    CurrentCh4.AlarmLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CurrentCh4TripLevelSet(float nValue)
{
    CurrentCh4.TripLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LvCurrentCh1AlarmLevelSet(float nValue)
{
    LvCurrentCh1.AlarmLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LvCurrentCh1TripLevelSet(float nValue)
{
    LvCurrentCh1.TripLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LvCurrentCh2AlarmLevelSet(float nValue)
{
    LvCurrentCh2.AlarmLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LvCurrentCh2TripLevelSet(float nValue)
{
    LvCurrentCh2.TripLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LvCurrentCh3AlarmLevelSet(float nValue)
{
    LvCurrentCh3.AlarmLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LvCurrentCh3TripLevelSet(float nValue)
{
    LvCurrentCh3.TripLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void DriverVoltageAlarmLevelSet(float nValue)
{
    DriverVolt.AlarmLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void DriverVoltageTripLevelSet(float nValue)
{
    DriverVolt.TripLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver1CurrentAlarmLevelSet(float nValue)
{
    Driver1Curr.AlarmLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver1CurrentTripLevelSet(float nValue)
{
    Driver1Curr.TripLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver2CurrentAlarmLevelSet(float nValue)
{
    Driver2Curr.AlarmLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver2CurrentTripLevelSet(float nValue)
{
    Driver2Curr.TripLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

//Set DriverVoltage Interlock and Alarm Delay
void DriverVoltageDelay(unsigned char Delay_Set)
{
    DriverVolt.Alarm_Delay_ms = Delay_Set;
    DriverVolt.Itlk_Delay_ms = Delay_Set;
}

/////////////////////////////////////////////////////////////////////////////////////////////

//Set DriverCurrent 1 and 2 Interlock and Alarm Delay
void DriverCurrentDelay(unsigned char Delay_Set)
{
    Driver1Curr.Alarm_Delay_ms = Delay_Set;
    Driver1Curr.Itlk_Delay_ms = Delay_Set;

    Driver2Curr.Alarm_Delay_ms = Delay_Set;
    Driver2Curr.Itlk_Delay_ms = Delay_Set;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char VoltageCh1AlarmStatusRead(void)
{
    if(VoltageCh1.Enable)return VoltageCh1.Alarm;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char VoltageCh1TripStatusRead(void)
{
    if(VoltageCh1.Enable)return VoltageCh1.Trip;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char VoltageCh2AlarmStatusRead(void)
{
    if(VoltageCh2.Enable)return VoltageCh2.Alarm;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char VoltageCh2TripStatusRead(void)
{
    if(VoltageCh2.Enable)return VoltageCh2.Trip;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char VoltageCh3AlarmStatusRead(void)
{
    if(VoltageCh3.Enable)return VoltageCh3.Alarm;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char VoltageCh3TripStatusRead(void)
{
    if(VoltageCh3.Enable)return VoltageCh3.Trip;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char VoltageCh4AlarmStatusRead(void)
{
    if(VoltageCh4.Enable)return VoltageCh4.Alarm;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char VoltageCh4TripStatusRead(void)
{
    if(VoltageCh4.Enable)return VoltageCh4.Trip;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char CurrentCh1AlarmStatusRead(void)
{
    if(CurrentCh1.Enable)return CurrentCh1.Alarm;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char CurrentCh1TripStatusRead(void)
{
    if(CurrentCh1.Enable)return CurrentCh1.Trip;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char CurrentCh2AlarmStatusRead(void)
{
    if(CurrentCh2.Enable)return CurrentCh2.Alarm;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char CurrentCh2TripStatusRead(void)
{
    if(CurrentCh2.Enable)return CurrentCh2.Trip;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char CurrentCh3AlarmStatusRead(void)
{
    if(CurrentCh3.Enable)return CurrentCh3.Alarm;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char CurrentCh3TripStatusRead(void)
{
    if(CurrentCh3.Enable)return CurrentCh3.Trip;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char CurrentCh4AlarmStatusRead(void)
{
    if(CurrentCh4.Enable)return CurrentCh4.Alarm;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char CurrentCh4TripStatusRead(void)
{
    if(CurrentCh4.Enable)return CurrentCh4.Trip;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char LvCurrentCh1AlarmStatusRead(void)
{
    if(LvCurrentCh1.Enable)return LvCurrentCh1.Alarm;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char LvCurrentCh1TripStatusRead(void)
{
    if(LvCurrentCh1.Enable)return LvCurrentCh1.Trip;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char LvCurrentCh2AlarmStatusRead(void)
{
    if(LvCurrentCh2.Enable)return LvCurrentCh2.Alarm;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char LvCurrentCh2TripStatusRead(void)
{
    if(LvCurrentCh2.Enable)return LvCurrentCh2.Trip;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char LvCurrentCh3AlarmStatusRead(void)
{
    if(LvCurrentCh3.Enable)return LvCurrentCh3.Alarm;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char LvCurrentCh3TripStatusRead(void)
{
    if(LvCurrentCh3.Enable)return LvCurrentCh3.Trip;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char DriverVoltageAlarmStatusRead(void)
{
    if(DriverVolt.Enable)return DriverVolt.Alarm;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char DriverVolatgeTripStatusRead(void)
{
    if(DriverVolt.Enable)return DriverVolt.Trip;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Driver1CurrentAlarmStatusRead(void)
{
    if(Driver1Curr.Enable)return Driver1Curr.Alarm;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Driver1CurrentTripStatusRead(void)
{
    if(Driver1Curr.Enable)return Driver1Curr.Trip;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Driver2CurrentAlarmStatusRead(void)
{
    if(Driver2Curr.Enable)return Driver2Curr.Alarm;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Driver2CurrentTripStatusRead(void)
{
    if(Driver2Curr.Enable)return Driver2Curr.Trip;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void AdcClearAlarmTrip(void)
{
    VoltageCh1.Alarm = 0;
    VoltageCh1.Trip = 0;

    VoltageCh2.Alarm = 0;
    VoltageCh2.Trip = 0;

    VoltageCh3.Alarm = 0;
    VoltageCh3.Trip = 0;

    VoltageCh4.Alarm = 0;
    VoltageCh4.Trip = 0;

    CurrentCh1.Alarm = 0;
    CurrentCh1.Trip = 0;

    CurrentCh2.Alarm = 0;
    CurrentCh2.Trip = 0;

    CurrentCh3.Alarm = 0;
    CurrentCh3.Trip = 0;

    CurrentCh4.Alarm = 0;
    CurrentCh4.Trip = 0;

    LvCurrentCh1.Alarm = 0;
    LvCurrentCh1.Trip = 0;

    LvCurrentCh2.Alarm = 0;
    LvCurrentCh2.Trip = 0;

    LvCurrentCh3.Alarm = 0;
    LvCurrentCh3.Trip = 0;

    DriverVolt.Alarm = 0;
    DriverVolt.Trip = 0;

    Driver1Curr.Alarm = 0;
    Driver1Curr.Trip = 0;

    Driver2Curr.Alarm = 0;
    Driver2Curr.Trip = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

