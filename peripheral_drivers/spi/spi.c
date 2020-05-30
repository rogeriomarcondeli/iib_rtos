
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "board_drivers/hardware_def.h"
#include "peripheral_drivers/gpio/gpio_driver.h"

/////////////////////////////////////////////////////////////////////////////////////////////

void spi_init()
{
    // Chip Select
    set_gpio_as_output(GPIO_PORTA_BASE, GPIO_PIN_3);

    // The SSI0 peripheral must be enabled for use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);


    // Configure the pin muxing for SSI0 functions on port A2, A3, A4, and A5.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.

    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    //GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA4_SSI0XDAT0);
    GPIOPinConfigure(GPIO_PA5_SSI0XDAT1);

    //
    // Configure the GPIO settings for the SSI pins.  This function also gives
    // control of these pins to the SSI hardware.  Consult the data sheet to
    // see which functions are allocated per pin.
    // The pins are assigned as follows:
    //      PA5 - SSI0Tx
    //      PA4 - SSI0Rx
    //      PA3 - SSI0Fss
    //      PA2 - SSI0CLK
    // TODO: change this to select the port/pin you are using.
    //
//    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 |
//                   GPIO_PIN_2);
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_2);

    // SPI Mode 3 for MAX31865
    SSIConfigSetExpClk(SSI0_BASE, SYSCLOCK, SSI_FRF_MOTO_MODE_3,
                                        SSI_MODE_MASTER, 1000000, 8);


    // Enable the SSI0 module.
    SSIEnable(SSI0_BASE);

    set_pin(GPIO_PORTA_BASE, GPIO_PIN_3);
}

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t read_spi_byte(uint8_t reg)
{
    uint32_t data;

    while(SSIDataGetNonBlocking(SSI0_BASE, &data))
    {
    }

    clear_pin(GPIO_PORTA_BASE, GPIO_PIN_3);

    SSIDataPut(SSI0_BASE, reg);
    SSIDataGet(SSI0_BASE, &data); //Dummy response
    SSIDataPut(SSI0_BASE, 0xFF); //Dummy
    SSIDataGet(SSI0_BASE, &data); // Data

    set_pin(GPIO_PORTA_BASE, GPIO_PIN_3);

    return data;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void read_spi_data(uint8_t reg, uint32_t *data, uint8_t size)
{
    uint32_t i;

    while(SSIDataGetNonBlocking(SSI0_BASE, &data[0]))
    {
    }

    clear_pin(GPIO_PORTA_BASE, GPIO_PIN_3);

    SSIDataPut(SSI0_BASE, reg);
    SSIDataGet(SSI0_BASE, &data[0]); //Dummy response
    for(i = 0; i < size; i++) {
        SSIDataPut(SSI0_BASE, 0xFF); //Dummy
        SSIDataGet(SSI0_BASE, &data[i]); // Data
    }

    set_pin(GPIO_PORTA_BASE, GPIO_PIN_3);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void write_spi_byte(uint8_t reg, uint32_t data)
{
    uint32_t dummy;
    clear_pin(GPIO_PORTA_BASE, GPIO_PIN_3);
    SSIDataPut(SSI0_BASE, reg);
    SSIDataGet(SSI0_BASE, &dummy);
    SSIDataPut(SSI0_BASE, data);
    SSIDataGet(SSI0_BASE, &dummy);
    set_pin(GPIO_PORTA_BASE, GPIO_PIN_3);
}

/////////////////////////////////////////////////////////////////////////////////////////////


