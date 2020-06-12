
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/ssi.h"
#include "driverlib/pin_map.h"
#include "board_drivers/hardware_def.h"
#include "peripheral_drivers/spi/spi.h"
#include "peripheral_drivers/gpio/gpio_driver.h"
#include "system/system.h"
#include "iib_devices/pt100.h"
#include "iib_devices/leds.h"

/////////////////////////////////////////////////////////////////////////////////////////////

// This lib reports a PT100 Temperature value from 0 to 255°C with a resolution of 1°C


/**
 * Configuration of the MAX31865 from MSB to LSB:
 * BIT      FUNCTION            ASSIGNMENT
 *  7       VBIAS               0=OFF            1=ON
 *  6       Conversion Mode     0=Normally OFF   1=AUTO
 *  5       1-Shot              0= -             1=1-Shot
 *  4       3-Wire              0=2- or 4-Wire   1=3-wire
 * 3,2      Faultdetection      set both to 0
 *  1       Fault Status Clear  set to 1
 *  0       50/60Hz filter      0=60Hz           1=50Hz
 */

//Registers defined in Table 1 on page 12 of the data sheet

/////////////////////////////////////////////////////////////////////////////////////////////

static unsigned char Configuration = 0b10000000; //0x80H
static unsigned char read_Configuration = 0b00000000; //0x00H
static unsigned char Write_High_Fault_Threshold_MSB = 0b10000011; //0x83H
static unsigned char Write_High_Fault_Threshold_LSB = 0b10000100; //0x84H
//static unsigned char Read_High_Fault_Threshold_MSB = 0b00000011; //0x03H
//static unsigned char Read_High_Fault_Threshold_LSB = 0b00000100; //0x04H
static unsigned char Write_Low_Fault_Threshold_MSB = 0b10000101; //0x85H
static unsigned char Write_Low_Fault_Threshold_LSB = 0b10000110; //0x86H
//static unsigned char Read_Low_Fault_Threshold_MSB = 0b00000101; //0x05H
//static unsigned char Read_Low_Fault_Threshold_LSB = 0b00000110; //0x06H
static unsigned char Fault_Status = 0b00000111; //0x07H

/////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Callendar-Van Dusen equation is used for temperature linearization.
 * Coeffeicant of equations are as follows:
 * R(T) = R0(1 + aT + bT^2 + c(T - 100)T^3)
 * Equation from : http://www.honeywell-sensor.com.cn/prodinfo/sensor_temperature/technical/c15_136.pdf
 */

static float a = 0.00390830;
static float b = -0.0000005775;
//static float c = -0.00000000000418301; // only for temperature = t < 0
float Reference_Resistor = 400.0; //Reference Resistor installed on the board.
float RTD_Resistance = 100.0; //RTD Resistance at 0 Degrees. Please refer to your RTD data sheet.

/////////////////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
// Declaração das variaveis globais
//******************************************************************************

typedef struct
{
    unsigned char Ch;
    unsigned char Enable;
    unsigned char Calibration;
    unsigned char Temperature;
    unsigned char AlarmLimit;
    unsigned char TripLimit;
    unsigned char CanNotCommunicate;
    unsigned char Error;
    unsigned char RtdOutOfRange;
    unsigned char Alarm;
    unsigned char Trip;
    unsigned char Alarm_Delay_s; // second
    unsigned char Alarm_DelayCount;
    unsigned char Itlk_Delay_s; // second
    unsigned char Itlk_DelayCount;
}pt100_t;

/////////////////////////////////////////////////////////////////////////////////////////////

static pt100_t Pt100Ch1;
static pt100_t Pt100Ch2;
static pt100_t Pt100Ch3;
static pt100_t Pt100Ch4;

/////////////////////////////////////////////////////////////////////////////////////////////

// Temperature Range: 0 to 255°C
/**
 * get_Temp() function checks if the fault bit (D0)of LSB RTD regiset is set.
 * If so, the conversion is aborted. If the fault bit is not set,
 * the conversion is initiated. The Digital Code is then computed
 * to a temperature value and printed on the serial console.
 *
 * For linearization, Callendar-Van Dusen equation is used.
 * R(T) = R0(1 + aT + bT^2 + c(T - 100)T^3)
 */
void get_Temp(pt100_t *pt100)
{
    unsigned int msb_rtd;
    unsigned int lsb_rtd;
    unsigned char fault_test = 0;
    float R;
    float Temp;
    float TempT;
    float RTD;
    
    msb_rtd = read_spi_byte(0x01);
    lsb_rtd = read_spi_byte(0x02);
    
    fault_test = lsb_rtd & 0x01;

    if(fault_test == 0)
    {
        // Clear RTD out of range flag
        pt100->RtdOutOfRange = 0;

        RTD = ((msb_rtd << 7) + ((lsb_rtd & 0xFE) >> 1)); // Combining RTD_MSB and RTD_LSB to protray decimal value. Removing MSB and LSB during shifting/Anding

        R = (RTD * Reference_Resistor) / 32768; // Conversion of ADC RTD code to resistance

        Temp = -RTD_Resistance * a + sqrt(RTD_Resistance * RTD_Resistance * a * a - 4 * RTD_Resistance * b * (RTD_Resistance - R)); // Conversion of RTD resistance to Temperature

        TempT = Temp / (2 * RTD_Resistance * b);

        if(TempT < 0.0) TempT = 0.0;
        else if(TempT > 255.0) TempT = 255.0;
    }
    else
    {
         //"Error was detected. The RTD resistance measured is not within the range specified in the Threshold Registers."
         pt100->RtdOutOfRange = 1;
         TempT = 0.0;
    }
    pt100->Temperature = TempT;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Pt100Channel(pt100_t *pt100)
{
    switch(pt100->Ch)
    {
        case 1:
             set_pin(RTD_CS_A0_BASE, RTD_CS_A0_PIN);
             set_pin(RTD_CS_A1_BASE, RTD_CS_A1_PIN);
             break;
        case 2:
             set_pin(RTD_CS_A0_BASE, RTD_CS_A0_PIN);
             clear_pin(RTD_CS_A1_BASE, RTD_CS_A1_PIN);
             break;
        case 3:
             clear_pin(RTD_CS_A0_BASE, RTD_CS_A0_PIN);
             clear_pin(RTD_CS_A1_BASE, RTD_CS_A1_PIN);
             break;
        case 4:
             clear_pin(RTD_CS_A0_BASE, RTD_CS_A0_PIN);
             set_pin(RTD_CS_A1_BASE, RTD_CS_A1_PIN);
             break;
        default:
             break;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Pt100InitChannel(pt100_t *pt100)
{
     unsigned int Fault_Error = 0;    // Variable to read Fault register and compute faults
     unsigned int value = 0;

     // Set mux channel
     Pt100Channel(pt100);

     //Try to clear the error
     write_spi_byte(Configuration, 0b10000010);
     
     delay_ms(100);

     write_spi_byte(Configuration,0b11010000);             // Enabling Vbias of max31865
     value = read_spi_byte(read_Configuration);            // Reading contents of Configuration register to verify communication with max31865 is done properly
     
     if (value == 208)
     {
         write_spi_byte(Write_High_Fault_Threshold_MSB, 0xFF);    // Writing High Fault Threshold MSB
         write_spi_byte(Write_High_Fault_Threshold_LSB, 0xFF);    // Writing High Fault Threshold LSB
         write_spi_byte(Write_Low_Fault_Threshold_MSB, 0x00);     // Writing Low Fault Threshold MSB
         write_spi_byte(Write_Low_Fault_Threshold_LSB, 0x00);     // Writing Low Fault Threshold LSB

         //"Communication successful with max31865"
         pt100->CanNotCommunicate = 0;

         // Prior to getting started with RTD to Digital Conversion, Users can do a preliminary test to detect if their is a fault in RTD connection with max31865
         Fault_Error = read_spi_byte(Fault_Status);

         // If their is no fault detected, the get_Temp() is called and it initiates the conversion. The results are displayed on the serial console
         if (Fault_Error == 0)
         {

           pt100->Error = 0;

         }
         else
         {
    
           //Save the error
           pt100->Error = Fault_Error;
    
         }

     }
     else
     {
         //" Unable to communicate with the device. Please check your connections and try again"
         pt100->CanNotCommunicate = 1;

     }
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Pt100ReadChannel(pt100_t *pt100)
{
    unsigned int Fault_Error = 0;    // Variable to read Fault register and compute faults

    // Set mux channel
    Pt100Channel(pt100);

    // Prior to getting started with RTD to Digital Conversion, Users can do a preliminary test to detect if their is a fault in RTD connection with max31865
    Fault_Error = read_spi_byte(Fault_Status);

    // If their is no fault detected, the get_Temp() is called and it initiates the conversion. The results are displayed on the serial console
    if (Fault_Error == 0)
    {
        // Calling get_Temp() to read RTD registers and convert to Temperature reading
        get_Temp(pt100);
        pt100->Error = Fault_Error;

        if(pt100->Temperature >= pt100->AlarmLimit)
        {
           if(pt100->Alarm_DelayCount < pt100->Alarm_Delay_s) pt100->Alarm_DelayCount++;
           else
           {
              pt100->Alarm_DelayCount = 0;
              pt100->Alarm = 1;
           }
        }

        else pt100->Alarm_DelayCount = 0;

        if(pt100->Temperature >= pt100->TripLimit)
        {
           if(pt100->Itlk_DelayCount < pt100->Itlk_Delay_s) pt100->Itlk_DelayCount++;
           else
           {
              pt100->Itlk_DelayCount = 0;
              pt100->Trip = 1;
           }
        }
        else pt100->Itlk_DelayCount = 0;
    }
    else
    {
        //Save the error
        pt100->Temperature = 0;
        pt100->Error = Fault_Error;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Pt100ChannelClear(pt100_t *pt100)
{
    // Set mux channel
    Pt100Channel(pt100);
    //Try to clear the error
    write_spi_byte(Configuration, 0b10000010);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Pt100Init(void)
{

    set_gpio_as_input(RTD_DRDY_1_BASE, RTD_DRDY_1_PIN);
    set_gpio_as_input(RTD_DRDY_2_BASE, RTD_DRDY_2_PIN);
    set_gpio_as_input(RTD_DRDY_3_BASE, RTD_DRDY_3_PIN);
    set_gpio_as_input(RTD_DRDY_4_BASE, RTD_DRDY_4_PIN);

    set_gpio_as_output(RTD_CS_A0_BASE, RTD_CS_A0_PIN);
    set_gpio_as_output(RTD_CS_A1_BASE, RTD_CS_A1_PIN);

    clear_pin(RTD_CS_A0_BASE, RTD_CS_A0_PIN);
    clear_pin(RTD_CS_A1_BASE, RTD_CS_A1_PIN);

    // Setting SPI clock to 4 MHz. Please note it is important to initialize SPI bus prior to making any changes.
    spi_init();

    Pt100Ch1.Ch                 = 1;
    Pt100Ch1.Enable             = 0;
    Pt100Ch1.Calibration        = 0;
    Pt100Ch1.Temperature        = 0;
    Pt100Ch1.AlarmLimit         = 100;
    Pt100Ch1.TripLimit          = 110;
    Pt100Ch1.Error              = 0;
    Pt100Ch1.RtdOutOfRange      = 0;
    Pt100Ch1.Alarm              = 0;
    Pt100Ch1.Trip               = 0;
    Pt100Ch1.Alarm_Delay_s      = 0;
    Pt100Ch1.Alarm_DelayCount   = 0;
    Pt100Ch1.Itlk_Delay_s       = 0;
    Pt100Ch1.Itlk_DelayCount    = 0;
    Pt100InitChannel(&Pt100Ch1);

    Pt100Ch2.Ch                 = 2;
    Pt100Ch2.Enable             = 0;
    Pt100Ch2.Calibration        = 0;
    Pt100Ch2.Temperature        = 0;
    Pt100Ch2.AlarmLimit         = 100;
    Pt100Ch2.TripLimit          = 110;
    Pt100Ch2.Error              = 0;
    Pt100Ch2.RtdOutOfRange      = 0;
    Pt100Ch2.Alarm              = 0;
    Pt100Ch2.Trip               = 0;
    Pt100Ch2.Alarm_Delay_s      = 0;
    Pt100Ch2.Alarm_DelayCount   = 0;
    Pt100Ch2.Itlk_Delay_s       = 0;
    Pt100Ch2.Itlk_DelayCount    = 0;
    Pt100InitChannel(&Pt100Ch2);

    Pt100Ch3.Ch                 = 3;
    Pt100Ch3.Enable             = 0;
    Pt100Ch3.Calibration        = 0;
    Pt100Ch3.Temperature        = 0;
    Pt100Ch3.AlarmLimit         = 100;
    Pt100Ch3.TripLimit          = 110;
    Pt100Ch3.Error              = 0;
    Pt100Ch3.RtdOutOfRange      = 0;
    Pt100Ch3.Alarm              = 0;
    Pt100Ch3.Trip               = 0;
    Pt100Ch3.Alarm_Delay_s      = 0;
    Pt100Ch3.Alarm_DelayCount   = 0;
    Pt100Ch3.Itlk_Delay_s       = 0;
    Pt100Ch3.Itlk_DelayCount    = 0;
    Pt100InitChannel(&Pt100Ch3);

    Pt100Ch4.Ch                 = 4;
    Pt100Ch4.Enable             = 0;
    Pt100Ch4.Calibration        = 0;
    Pt100Ch4.Temperature        = 0;
    Pt100Ch4.AlarmLimit         = 100;
    Pt100Ch4.TripLimit          = 110;
    Pt100Ch4.Error              = 0;
    Pt100Ch4.RtdOutOfRange      = 0;
    Pt100Ch4.Alarm              = 0;
    Pt100Ch4.Trip               = 0;
    Pt100Ch4.Alarm_Delay_s      = 0;
    Pt100Ch4.Alarm_DelayCount   = 0;
    Pt100Ch4.Itlk_Delay_s       = 0;
    Pt100Ch4.Itlk_DelayCount    = 0;
    Pt100InitChannel(&Pt100Ch4);
 
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Board Functions
//******************************************************************************

// Channel 1 Temperature Sample
void Pt100Ch1Sample(void)
{
    //Led1TurnOn();
    Pt100ReadChannel(&Pt100Ch1);
    //Led1TurnOff();
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Channel 2 Temperature Sample
void Pt100Ch2Sample(void)
{
    //Led2TurnOn();
    Pt100ReadChannel(&Pt100Ch2);
    //Led2TurnOff();
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Channel 3 Temperature Sample
void Pt100Ch3Sample(void)
{
    //Led3TurnOn();
    Pt100ReadChannel(&Pt100Ch3);
    //Led3TurnOff();
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Channel 4 Temperature Sample
void Pt100Ch4Sample(void)
{
    //Led4TurnOn();
    Pt100ReadChannel(&Pt100Ch4);
    //Led4TurnOff();
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Application Functions
//******************************************************************************

// PT100 Channel 1 Enable
void Pt100Ch1Enable(void)
{
    Pt100Ch1.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// PT100 Channel 1 Disable
void Pt100Ch1Disable(void)
{
    Pt100Ch1.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// PT100 Channel 2 Enable
void Pt100Ch2Enable(void)
{
    Pt100Ch2.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// PT100 Channel 2 Disable
void Pt100Ch2Disable(void)
{
    Pt100Ch2.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// PT100 Channel 3 Enable
void Pt100Ch3Enable(void)
{
    Pt100Ch3.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// PT100 Channel 3 Disable
void Pt100Ch3Disable(void)
{
    Pt100Ch3.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// PT100 Channel 4 Enable
void Pt100Ch4Enable(void)
{
    Pt100Ch4.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// PT100 Channel 4 Disable
void Pt100Ch4Disable(void)
{
    Pt100Ch4.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 1 Temperature value
unsigned char Pt100ReadCh1(void)
{
    if(Pt100Ch1.Enable)return Pt100Ch1.Temperature;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 2 Temperature value
unsigned char Pt100ReadCh2(void)
{
    if(Pt100Ch2.Enable)return Pt100Ch2.Temperature;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 3 Temperature value
unsigned char Pt100ReadCh3(void)
{
    if(Pt100Ch3.Enable)return Pt100Ch3.Temperature;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 4 Temperature value
unsigned char Pt100ReadCh4(void)
{
    if(Pt100Ch4.Enable)return Pt100Ch4.Temperature;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Set Channel 1 Temperature Alarme Level
void Pt100SetCh1AlarmLevel(unsigned char alarm)
{
    Pt100Ch1.AlarmLimit = alarm;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 1 Temperature Alarme Level
unsigned char Pt100ReadCh1AlarmLevel(void)
{
    return Pt100Ch1.AlarmLimit;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Set Channel 1 Temperature Trip Level
void Pt100SetCh1TripLevel(unsigned char trip)
{
    Pt100Ch1.TripLimit = trip;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 1 Temperature Trip Level
unsigned char Pt100ReadCh1TripLevel(void)
{
    return Pt100Ch1.TripLimit;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Set Channel 1 Interlock and Alarm Delay
void Pt100SetCh1Delay(unsigned char Delay_Set)
{
    Pt100Ch1.Alarm_Delay_s = Delay_Set;
    Pt100Ch1.Itlk_Delay_s = Delay_Set;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Set Channel 2 Temperature Alarme Level
void Pt100SetCh2AlarmLevel(unsigned char alarm)
{
    Pt100Ch2.AlarmLimit = alarm;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 2 Temperature Alarme Level
unsigned char Pt100ReadCh2AlarmLevel(void)
{
    return Pt100Ch2.AlarmLimit;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Set Channel 2 Temperature Trip Level
void Pt100SetCh2TripLevel(unsigned char trip)
{
    Pt100Ch2.TripLimit = trip;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 2 Temperature Trip Level
unsigned char Pt100ReadCh2TripLevel(void)
{
    return Pt100Ch2.TripLimit;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Set Channel 2 Interlock and Alarm Delay
void Pt100SetCh2Delay(unsigned char Delay_Set)
{
    Pt100Ch2.Alarm_Delay_s = Delay_Set;
    Pt100Ch2.Itlk_Delay_s = Delay_Set;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Set Channel 3 Temperature Alarme Level
void Pt100SetCh3AlarmLevel(unsigned char alarm)
{
    Pt100Ch3.AlarmLimit = alarm;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 3 Temperature Alarme Level
unsigned char Pt100ReadCh3AlarmLevel(void)
{
    return Pt100Ch3.AlarmLimit;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Set Channel 3 Temperature Trip Level
void Pt100SetCh3TripLevel(unsigned char trip)
{
    Pt100Ch3.TripLimit = trip;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 3 Temperature Trip Level
unsigned char Pt100ReadCh3TripLevel(void)
{
    return Pt100Ch3.TripLimit;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Set Channel 3 Interlock and Alarm Delay
void Pt100SetCh3Delay(unsigned char Delay_Set)
{
    Pt100Ch3.Alarm_Delay_s = Delay_Set;
    Pt100Ch3.Itlk_Delay_s = Delay_Set;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Set Channel 4 Temperature Alarme Level
void Pt100SetCh4AlarmLevel(unsigned char alarm)
{
    Pt100Ch4.AlarmLimit = alarm;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 4 Temperature Alarme Level
unsigned char Pt100ReadCh4AlarmLevel(void)
{
    return Pt100Ch4.AlarmLimit;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Set Channel 4 Temperature Trip Level
void Pt100SetCh4TripLevel(unsigned char trip)
{
    Pt100Ch4.TripLimit = trip;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 4 Temperature Trip Level
unsigned char Pt100ReadCh4TripLevel(void)
{
    return Pt100Ch4.TripLimit;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Set Channel 4 Interlock and Alarm Delay
void Pt100SetCh4Delay(unsigned char Delay_Set)
{
    Pt100Ch4.Alarm_Delay_s = Delay_Set;
    Pt100Ch4.Itlk_Delay_s = Delay_Set;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Pt100ClearAlarmTrip(void)
{

    Pt100Ch1.Alarm = 0;
    Pt100Ch1.Trip = 0;
 
    Pt100Ch2.Alarm = 0;
    Pt100Ch2.Trip = 0;
 
    Pt100Ch3.Alarm = 0;
    Pt100Ch3.Trip = 0;
 
    Pt100Ch4.Alarm = 0;
    Pt100Ch4.Trip = 0;

}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Pt100ReadCh1AlarmSts(void)
{
    if(Pt100Ch1.Enable)return Pt100Ch1.Alarm;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Pt100ReadCh1TripSts(void)
{
    if(Pt100Ch1.Enable)return Pt100Ch1.Trip;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Pt100ReadCh2AlarmSts(void)
{
    if(Pt100Ch2.Enable)return Pt100Ch2.Alarm;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Pt100ReadCh2TripSts(void)
{
    if(Pt100Ch2.Enable)return Pt100Ch2.Trip;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Pt100ReadCh3AlarmSts(void)
{
    if(Pt100Ch3.Enable)return Pt100Ch3.Alarm;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Pt100ReadCh3TripSts(void)
{
    if(Pt100Ch3.Enable)return Pt100Ch3.Trip;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Pt100ReadCh4AlarmSts(void)
{
    if(Pt100Ch4.Enable)return Pt100Ch4.Alarm;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Pt100ReadCh4TripSts(void)
{
    if(Pt100Ch4.Enable)return Pt100Ch4.Trip;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 1 Error flag
unsigned char Pt100ReadCh1Error(void)
{
    if(Pt100Ch1.Enable)return Pt100Ch1.Error;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 2 Error flag
unsigned char Pt100ReadCh2Error(void)
{
    if(Pt100Ch2.Enable)return Pt100Ch2.Error;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 3 Error flag
unsigned char Pt100ReadCh3Error(void)
{
    if(Pt100Ch3.Enable)return Pt100Ch3.Error;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 4 Error flag
unsigned char Pt100ReadCh4Error(void)
{
    if(Pt100Ch4.Enable)return Pt100Ch4.Error;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 1 Can Not Communicate flag
unsigned char Pt100ReadCh1CNC(void)
{
    if(Pt100Ch1.Enable)return Pt100Ch1.CanNotCommunicate;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 2 Can Not Communicate flag
unsigned char Pt100ReadCh2CNC(void)
{
    if(Pt100Ch2.Enable)return Pt100Ch2.CanNotCommunicate;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 3 Can Not Communicate flag
unsigned char Pt100ReadCh3CNC(void)
{
    if(Pt100Ch3.Enable)return Pt100Ch3.CanNotCommunicate;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 4 Can Not Communicate flag
unsigned char Pt100ReadCh4CNC(void)
{
    if(Pt100Ch4.Enable)return Pt100Ch4.CanNotCommunicate;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 1 RTD Out Of Range flag
unsigned char Pt100ReadCh1RtdSts(void)
{
    if(Pt100Ch1.Enable)return Pt100Ch1.RtdOutOfRange;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 2 RTD Out Of Range flag
unsigned char Pt100ReadCh2RtdSts(void)
{
    if(Pt100Ch2.Enable)return Pt100Ch2.RtdOutOfRange;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 3 RTD Out Of Range flag
unsigned char Pt100ReadCh3RtdSts(void)
{
    if(Pt100Ch3.Enable)return Pt100Ch3.RtdOutOfRange;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Read Channel 4 RTD Out Of Range flag
unsigned char Pt100ReadCh4RtdSts(void)
{
    if(Pt100Ch4.Enable)return Pt100Ch4.RtdOutOfRange;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Try to clear Channel 1 Error flag
void Pt100Ch1Clear(void)
{
    Pt100ChannelClear(&Pt100Ch1);
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Try to clear Channel 2 Error flag
void Pt100Ch2Clear(void)
{
    Pt100ChannelClear(&Pt100Ch2);
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Try to clear Channel 3 Error flag
void Pt100Ch3Clear(void)
{
    Pt100ChannelClear(&Pt100Ch3);
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Try to clear Channel 4 Error flag
void Pt100Ch4Clear(void)
{
    Pt100ChannelClear(&Pt100Ch4);
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Try to reset Ch1
void Pt100Ch1Reset(void)
{
    Pt100InitChannel(&Pt100Ch1);
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Try to reset Ch2
void Pt100Ch2Reset(void)
{
    Pt100InitChannel(&Pt100Ch2);
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Try to reset Ch3
void Pt100Ch3Reset(void)
{
    Pt100InitChannel(&Pt100Ch3);
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Try to reset Ch4
void Pt100Ch4Reset(void)
{
    Pt100InitChannel(&Pt100Ch4);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void ErrorCheckHandle(void)
{
    if(Pt100ReadCh1Error()) Pt100Ch1Clear();
    if(Pt100ReadCh2Error()) Pt100Ch2Clear();
    if(Pt100ReadCh3Error()) Pt100Ch3Clear();
    if(Pt100ReadCh4Error()) Pt100Ch4Clear();

    //Check CanNotCommunicate
    //Indicate channel problem, need a PT100 initialization or a chip verification
    // Reset() -> Reset implemented by software


    //Check RTD Out Of Range
    //send a command to indication layer to signalize the user
    if(Pt100ReadCh1RtdSts()) Pt100Ch1Reset();
    if(Pt100ReadCh2RtdSts()) Pt100Ch2Reset();
    if(Pt100ReadCh3RtdSts()) Pt100Ch3Reset();
    if(Pt100ReadCh4RtdSts()) Pt100Ch4Reset();
}

/////////////////////////////////////////////////////////////////////////////////////////////





