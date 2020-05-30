
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "iib_devices/input.h"
#include "board_drivers/hardware_def.h"
#include "peripheral_drivers/gpio/gpio_driver.h"

/////////////////////////////////////////////////////////////////////////////////////////////

static unsigned char BoardAddress = 0;

/////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    unsigned char Enable;

}gpdi_t;

/////////////////////////////////////////////////////////////////////////////////////////////

static gpdi_t Gpdi1;
static gpdi_t Gpdi2;
static gpdi_t Gpdi3;
static gpdi_t Gpdi4;
static gpdi_t Gpdi5;
static gpdi_t Gpdi6;
static gpdi_t Gpdi7;
static gpdi_t Gpdi8;
static gpdi_t Gpdi9;
static gpdi_t Gpdi10;
static gpdi_t Gpdi11;
static gpdi_t Gpdi12;
static gpdi_t Driver1TopError;
static gpdi_t Driver1BotError;
static gpdi_t Driver1OverTemp;
static gpdi_t Driver2TopError;
static gpdi_t Driver2BotError;
static gpdi_t Driver2OverTemp;

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char BoardAddressRead(void)
{
     BoardAddress = 0;

     if(read_pin(CAN_ADD_0_BASE, CAN_ADD_0_PIN)) BoardAddress |= 0b00000001;
     if(read_pin(CAN_ADD_1_BASE, CAN_ADD_1_PIN)) BoardAddress |= 0b00000010;
     if(read_pin(CAN_ADD_2_BASE, CAN_ADD_2_PIN)) BoardAddress |= 0b00000100;
     if(read_pin(CAN_ADD_3_BASE, CAN_ADD_3_PIN)) BoardAddress |= 0b00001000;
     if(read_pin(CAN_ADD_4_BASE, CAN_ADD_4_PIN)) BoardAddress |= 0b00010000;

     BoardAddress ^= 0b11111;
     BoardAddress &= 0b00011111;

     return BoardAddress;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void InputInit(void)
{
    
    // GPDI
    set_gpio_as_input(GPDI_1_BASE, GPDI_1_PIN);
    set_gpio_as_input(GPDI_2_BASE, GPDI_2_PIN);
    set_gpio_as_input(GPDI_3_BASE, GPDI_3_PIN);
    set_gpio_as_input(GPDI_4_BASE, GPDI_4_PIN);
    set_gpio_as_input(GPDI_5_BASE, GPDI_5_PIN);
    set_gpio_as_input(GPDI_6_BASE, GPDI_6_PIN);
    set_gpio_as_input(GPDI_7_BASE, GPDI_7_PIN);
    set_gpio_as_input(GPDI_8_BASE, GPDI_8_PIN);
    set_gpio_as_input(GPDI_9_BASE, GPDI_9_PIN);
    set_gpio_as_input(GPDI_10_BASE, GPDI_10_PIN);
    set_gpio_as_input(GPDI_11_BASE, GPDI_11_PIN);
    set_gpio_as_input(GPDI_12_BASE, GPDI_12_PIN);
    
    //Driver Status
    set_gpio_as_input(ERROR_DRIVER_1_TOP_BASE, ERROR_DRIVER_1_TOP_PIN);
    set_gpio_as_input(ERROR_DRIVER_1_BOT_BASE, ERROR_DRIVER_1_BOT_PIN);
    set_gpio_as_input(MODULE_1_OVER_TEMP_BASE, MODULE_1_OVER_TEMP_PIN);
    set_gpio_as_input(ERROR_DRIVER_2_TOP_BASE, ERROR_DRIVER_2_TOP_PIN);
    set_gpio_as_input(ERROR_DRIVER_2_BOT_BASE, ERROR_DRIVER_2_BOT_PIN);
    set_gpio_as_input(MODULE_2_OVER_TEMP_BASE, MODULE_2_OVER_TEMP_PIN);

    //CAN Address
    set_gpio_as_input(CAN_ADD_0_BASE, CAN_ADD_0_PIN);
    set_gpio_as_input(CAN_ADD_1_BASE, CAN_ADD_1_PIN);
    set_gpio_as_input(CAN_ADD_2_BASE, CAN_ADD_2_PIN);
    set_gpio_as_input(CAN_ADD_3_BASE, CAN_ADD_3_PIN);
    set_gpio_as_input(CAN_ADD_4_BASE, CAN_ADD_4_PIN);

    Gpdi1.Enable = 0;
    Gpdi2.Enable = 0;
    Gpdi3.Enable = 0;
    Gpdi4.Enable = 0;
    Gpdi5.Enable = 0;
    Gpdi6.Enable = 0;
    Gpdi7.Enable = 0;
    Gpdi8.Enable = 0;
    Gpdi9.Enable = 0;
    Gpdi10.Enable = 0;
    Gpdi11.Enable = 0;
    Gpdi12.Enable = 0;

    Driver1TopError.Enable = 0;
    Driver1BotError.Enable = 0;
    Driver1OverTemp.Enable = 0;

    Driver2TopError.Enable = 0;
    Driver2BotError.Enable = 0;
    Driver2OverTemp.Enable = 0;

}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi1Enable(void)
{
    Gpdi1.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi1Disable(void)
{
    Gpdi1.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi2Enable(void)
{
    Gpdi2.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi2Disable(void)
{
    Gpdi2.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi3Enable(void)
{
    Gpdi3.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi3Disable(void)
{
    Gpdi3.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi4Enable(void)
{
    Gpdi4.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi4Disable(void)
{
    Gpdi4.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi5Enable(void)
{
    Gpdi5.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi5Disable(void)
{
    Gpdi5.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi6Enable(void)
{
    Gpdi6.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi6Disable(void)
{
    Gpdi6.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi7Enable(void)
{
    Gpdi7.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi7Disable(void)
{
    Gpdi7.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi8Enable(void)
{
    Gpdi8.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi8Disable(void)
{
    Gpdi8.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi9Enable(void)
{
    Gpdi9.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi9Disable(void)
{
    Gpdi9.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi10Enable(void)
{
    Gpdi10.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi10Disable(void)
{
    Gpdi10.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi11Enable(void)
{
    Gpdi11.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi11Disable(void)
{
    Gpdi11.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi12Enable(void)
{
    Gpdi12.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Gpdi12Disable(void)
{
    Gpdi12.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver1TopErrorEnable(void)
{
    Driver1TopError.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver1TopErrorDisable(void)
{
    Driver1TopError.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver1BotErrorEnable(void)
{
    Driver1BotError.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver1BotErrorDisable(void)
{
    Driver1BotError.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver1OverTempEnable(void)
{
    Driver1OverTemp.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver1OverTempDisable(void)
{
    Driver1OverTemp.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver2TopErrorEnable(void)
{
    Driver2TopError.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver2TopErrorDisable(void)
{
    Driver2TopError.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver2BotErrorEnable(void)
{
    Driver2BotError.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver2BotErrorDisable(void)
{
    Driver2BotError.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver2OverTempEnable(void)
{
    Driver2OverTemp.Enable = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Driver2OverTempDisable(void)
{
    Driver2OverTemp.Enable = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Gpdi1Read(void)
{
    if(Gpdi1.Enable) return read_pin(GPDI_1_BASE, GPDI_1_PIN);
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Gpdi2Read(void)
{
    if(Gpdi2.Enable) return read_pin(GPDI_2_BASE, GPDI_2_PIN);
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Gpdi3Read(void)
{
    if(Gpdi3.Enable) return read_pin(GPDI_3_BASE, GPDI_3_PIN);
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Gpdi4Read(void)
{
    if(Gpdi4.Enable) return read_pin(GPDI_4_BASE, GPDI_4_PIN);
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Gpdi5Read(void)
{
    if(Gpdi5.Enable) return read_pin(GPDI_5_BASE, GPDI_5_PIN);
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Gpdi6Read(void)
{
    if(Gpdi6.Enable) return read_pin(GPDI_6_BASE, GPDI_6_PIN);
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Gpdi7Read(void)
{
    if(Gpdi7.Enable) return read_pin(GPDI_7_BASE, GPDI_7_PIN);
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Gpdi8Read(void)
{
    if(Gpdi8.Enable) return read_pin(GPDI_8_BASE, GPDI_8_PIN);
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
unsigned char Gpdi9Read(void)
{
    if(Gpdi9.Enable) return read_pin(GPDI_9_BASE, GPDI_9_PIN);
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Gpdi10Read(void)
{
    if(Gpdi10.Enable) return read_pin(GPDI_10_BASE, GPDI_10_PIN);
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Gpdi11Read(void)
{
    if(Gpdi11.Enable) return read_pin(GPDI_11_BASE, GPDI_11_PIN);
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Gpdi12Read(void)
{
    if(Gpdi12.Enable) return read_pin(GPDI_12_BASE, GPDI_12_PIN);
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Driver1TopErrorRead(void)
{
    if(Driver1TopError.Enable) return read_pin(ERROR_DRIVER_1_TOP_BASE, ERROR_DRIVER_1_TOP_PIN);
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Driver1BotErrorRead(void)
{
    if(Driver1BotError.Enable) return read_pin(ERROR_DRIVER_1_BOT_BASE, ERROR_DRIVER_1_BOT_PIN);
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Driver1OverTempRead(void)
{
    if(Driver1OverTemp.Enable) return !read_pin(MODULE_1_OVER_TEMP_BASE, MODULE_1_OVER_TEMP_PIN);
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Driver2TopErrorRead(void)
{
    if(Driver2TopError.Enable)return read_pin(ERROR_DRIVER_2_TOP_BASE, ERROR_DRIVER_2_TOP_PIN);
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Driver2BotErrorRead(void)
{
    if(Driver2BotError.Enable)return read_pin(ERROR_DRIVER_2_BOT_BASE, ERROR_DRIVER_2_BOT_PIN);
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

unsigned char Driver2OverTempRead(void)
{
    if(Driver2OverTemp.Enable) return !read_pin(MODULE_2_OVER_TEMP_BASE, MODULE_2_OVER_TEMP_PIN);
    else return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////



