
/////////////////////////////////////////////////////////////////////////////////////////////

// Sensor: Si7005-B-FM1
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "iib_devices/board_temp_hum.h"
#include "system/system.h"
#include "peripheral_drivers/gpio/gpio_driver.h"
#include "peripheral_drivers/i2c/i2c_driver.h"
#include "board_drivers/hardware_def.h"

/////////////////////////////////////////////////////////////////////////////////////////////

#define SlaveAddress 0x40
#define RegisterAddress0 0x00
#define RegisterAddress1 0x01
#define RegisterAddress2 0x02
#define RegisterAddress3 0x03
#define RegisterAddress11 0x11

/////////////////////////////////////////////////////////////////////////////////////////////

#define a0 (-4.7844)
#define a1  0.4008
#define a2 (-0.00393)
#define q0  0.1973
#define q1  0.00237

/////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    unsigned char Enable;
    unsigned char Value;
    unsigned char AlarmLimit;
    unsigned char TripLimit;
    unsigned char Alarm;
    unsigned char Trip;
    unsigned char Alarm_Delay_ms; // milisecond
    unsigned char Alarm_DelayCount;
    unsigned char Itlk_Delay_ms; // milisecond
    unsigned char Itlk_DelayCount;
}rh_tempboard_t;

/////////////////////////////////////////////////////////////////////////////////////////////

static rh_tempboard_t TemperatureBoard;
static rh_tempboard_t RelativeHumidity;

/////////////////////////////////////////////////////////////////////////////////////////////

static unsigned char Start1 = 0x01;
static unsigned char Start2 = 0x11;

/////////////////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
// /* Read the temperature */
//******************************************************************************
void BoardTemperatureStartConversion(void)
{

  toggle_pin(TP_1_BASE, TP_1_PIN);

  I2C5Send(SlaveAddress, 2, RegisterAddress3, Start2);

  toggle_pin(TP_1_BASE, TP_1_PIN);

}

/////////////////////////////////////////////////////////////////////////////////////////////

void BoardTemperatureRead(void)
{
    uint8_t Status=1;
    uint8_t TemperatureH;
    uint8_t TemperatureL;
    uint16_t Tempa;
    uint16_t Tempb;
    uint16_t Tempc;

    toggle_pin(TP_1_BASE, TP_1_PIN);

    while(Status==1)
    {

         Status = I2C5Receive(SlaveAddress, RegisterAddress0);

    }

    toggle_pin(TP_1_BASE, TP_1_PIN);

    TemperatureH = I2C5Receive(SlaveAddress, RegisterAddress1);

    toggle_pin(TP_1_BASE, TP_1_PIN);

    TemperatureL = I2C5Receive(SlaveAddress, RegisterAddress2);

    toggle_pin(TP_1_BASE, TP_1_PIN);

    Tempa = TemperatureH;
    Tempb = Tempa<<8;

    Tempb += TemperatureL;
    Tempc = Tempb>>2;

    TemperatureBoard.Value = (Tempc/32) - 50;

    if(TemperatureBoard.Value > TemperatureBoard.AlarmLimit)
    {
        if(TemperatureBoard.Alarm_DelayCount < TemperatureBoard.Alarm_Delay_ms) TemperatureBoard.Alarm_DelayCount++;
        else
        {
           TemperatureBoard.Alarm_DelayCount = 0;
           TemperatureBoard.Alarm = 1;
        }
    }
    else TemperatureBoard.Alarm_DelayCount = 0;

    if(TemperatureBoard.Value > TemperatureBoard.TripLimit)
    {
        if(TemperatureBoard.Itlk_DelayCount < TemperatureBoard.Itlk_Delay_ms) TemperatureBoard.Itlk_DelayCount++;
        else
        {
           TemperatureBoard.Itlk_DelayCount = 0;
           TemperatureBoard.Trip = 1;
        }
    }
    else TemperatureBoard.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
// /* Read humidity. The 'compensate' variable contains the current temperature used to calculate the temperature compensation.*/
//******************************************************************************
void RelativeHumidityStartConversion(void)
{

  toggle_pin(TP_2_BASE, TP_2_PIN);

  I2C5Send(SlaveAddress, 2, RegisterAddress3, Start1);

  toggle_pin(TP_2_BASE, TP_2_PIN);

}

void RelativeHumidityRead(void)
{
    unsigned char Status=1;
    unsigned char RelativeHumidityH;
    unsigned char RelativeHumidityL;
    unsigned int RelHuma;
    unsigned int RelHumb;
    unsigned int RelHumc;

    float curve;
    float rawHumidity;
    float linearHumidity;

    while(Status==1)
    {

         Status = I2C5Receive(SlaveAddress, RegisterAddress0);

    }

    toggle_pin(TP_2_BASE, TP_2_PIN);

    RelativeHumidityH = I2C5Receive(SlaveAddress, RegisterAddress1);

    toggle_pin(TP_2_BASE, TP_2_PIN);

    RelativeHumidityL = I2C5Receive(SlaveAddress, RegisterAddress2);

    toggle_pin(TP_2_BASE, TP_2_PIN);

    RelHuma = RelativeHumidityH;
    RelHumb = RelHuma<<8;

    RelHumb += RelativeHumidityL;
    RelHumc = RelHumb>>4;

    rawHumidity = RelHumc;

    curve = (rawHumidity/16.0)-24.0 ;

    linearHumidity = curve - ( (curve * curve) * a2 + curve * a1 + a0);
    linearHumidity = linearHumidity + ( TemperatureBoard.Value - 30.0 ) * ( linearHumidity * q1 + q0 );

    RelativeHumidity.Value = linearHumidity;

    if(RelativeHumidity.Value > RelativeHumidity.AlarmLimit)
    {
        if(RelativeHumidity.Alarm_DelayCount < RelativeHumidity.Alarm_Delay_ms) RelativeHumidity.Alarm_DelayCount++;
        else
        {
           RelativeHumidity.Alarm_DelayCount = 0;
           RelativeHumidity.Alarm = 1;
        }
    }
    else RelativeHumidity.Alarm_DelayCount = 0;

    if(RelativeHumidity.Value > RelativeHumidity.TripLimit)
    {
        if(RelativeHumidity.Itlk_DelayCount < RelativeHumidity.Itlk_Delay_ms) RelativeHumidity.Itlk_DelayCount++;
        else
        {
           RelativeHumidity.Itlk_DelayCount = 0;
           RelativeHumidity.Trip = 1;
        }
    }
    else RelativeHumidity.Itlk_DelayCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
// Init Si7005-B SENSOR
//******************************************************************************
void RhBoardTempSenseInit(void)
{

    set_gpio_as_output(TP_1_BASE, TP_1_PIN);
    set_gpio_as_output(TP_2_BASE, TP_2_PIN);



    // performs I2C initialization
    InitI2C5();

    // configura CS como output
    set_gpio_as_output(GPIO_PORTB_BASE, GPIO_PIN_2);
    clear_pin(GPIO_PORTB_BASE, GPIO_PIN_2);

    delay_ms(100);

    TemperatureBoard.Enable = 0;
    TemperatureBoard.Value = 0;
    TemperatureBoard.AlarmLimit = 90;
    TemperatureBoard.TripLimit = 100;
    TemperatureBoard.Alarm = 0;
    TemperatureBoard.Trip = 0;
    TemperatureBoard.Alarm_Delay_ms = 0; // milisecond
    TemperatureBoard.Alarm_DelayCount = 0;
    TemperatureBoard.Itlk_Delay_ms = 0; // milisecond
    TemperatureBoard.Itlk_DelayCount = 0;

    RelativeHumidity.Enable = 0;
    RelativeHumidity.Value = 0;
    RelativeHumidity.AlarmLimit = 90;
    RelativeHumidity.TripLimit = 100;
    RelativeHumidity.Alarm = 0;
    RelativeHumidity.Trip = 0;
    RelativeHumidity.Alarm_Delay_ms = 0; // milisecond
    RelativeHumidity.Alarm_DelayCount = 0;
    RelativeHumidity.Itlk_Delay_ms = 0; // milisecond
    RelativeHumidity.Itlk_DelayCount = 0;

}

/////////////////////////////////////////////////////////////////////////////////////////////

void BoardTempEnable(void)
{
    TemperatureBoard.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void BoardTempDisable(void)
{
    TemperatureBoard.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void RhEnable(void)
{
    RelativeHumidity.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void RhDisable(void)
{
    RelativeHumidity.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char BoardTempRead(void)
{
    if(TemperatureBoard.Enable)return TemperatureBoard.Value;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char RhRead(void)
{
    if(RelativeHumidity.Enable)return RelativeHumidity.Value;
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void BoardTempAlarmLevelSet(unsigned char nValue)
{
    TemperatureBoard.AlarmLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void BoardTempTripLevelSet(unsigned char nValue)
{
    TemperatureBoard.TripLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void BoardTempDelay(unsigned char Delay_Set)
{
    TemperatureBoard.Alarm_Delay_ms = Delay_Set;
    TemperatureBoard.Itlk_Delay_ms = Delay_Set;

}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char BoardTempAlarmStatusRead(void)
{
   if(TemperatureBoard.Enable)return TemperatureBoard.Alarm;
   else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char BoardTempTripStatusRead(void)
{
   if(TemperatureBoard.Enable)return TemperatureBoard.Trip;
   else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void RhAlarmLevelSet(unsigned char nValue)
{
    RelativeHumidity.AlarmLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void RhTripLevelSet(unsigned char nValue)
{
    RelativeHumidity.TripLimit = nValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void RhDelay(unsigned char Delay_Set)
{
    RelativeHumidity.Alarm_Delay_ms = Delay_Set;
    RelativeHumidity.Itlk_Delay_ms = Delay_Set;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char RhAlarmStatusRead(void)
{
   if(RelativeHumidity.Enable)return RelativeHumidity.Alarm;
   else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char RhTripStatusRead(void)
{
   if(RelativeHumidity.Enable)return RelativeHumidity.Trip;
   else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void RhBoardTempClearAlarmTrip(void)
{
    TemperatureBoard.Alarm = 0;
    TemperatureBoard.Trip = 0;

    RelativeHumidity.Alarm = 0;
    RelativeHumidity.Trip = 0;
   
}

/////////////////////////////////////////////////////////////////////////////////////////////


