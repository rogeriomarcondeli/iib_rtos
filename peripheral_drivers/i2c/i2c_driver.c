
/////////////////////////////////////////////////////////////////////////////////////////////

/*
 * i2c_driver.c
 *
 *  Created on: 14 de ago de 2017
 *      Author: joao.rosa
 */
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_i2c.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "i2c_driver.h"
#include "board_drivers/hardware_def.h"
#include "peripheral_drivers/gpio/gpio_driver.h"

/////////////////////////////////////////////////////////////////////////////////////////////

#define I2C_READ true
#define I2C_WRITE false

/////////////////////////////////////////////////////////////////////////////////////////////

//initialize I2C module 5
//Slightly modified version of TI's example code
void InitI2C5(void)
{

    // Configure Pins for I2C2 Master Interface
    GPIOPinConfigure(GPIO_PB0_I2C5SCL);
    GPIOPinConfigure(GPIO_PB1_I2C5SDA);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_1);
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_0);

    // Stop the Clock, Reset and Enable I2C Module
    // in Master Function
    SysCtlPeripheralDisable(SYSCTL_PERIPH_I2C5);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C5);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C5);

    // Wait for the Peripheral to be ready for programming
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C5));

    // Initialize and Configure the Master Module
    //I2CMasterInitExpClk(I2C5_BASE, SysCtlClockGet(), false); //para 100Khz.
    I2CMasterInitExpClk(I2C5_BASE, SysCtlClockGet(), true); //para 400Khz.

    // Enable the Glitch Filter
    I2CMasterGlitchFilterConfigSet(I2C5_BASE, I2C_MASTER_GLITCH_FILTER_8);

}

/////////////////////////////////////////////////////////////////////////////////////////////

//sends an I2C command to the specified slave
void I2C5Send(uint8_t slave_addr, uint8_t num_of_args, ...)
{
    uint8_t i;

    // Tell the master module what address it will place on the bus when
    // communicating with the slave.
    I2CMasterSlaveAddrSet(I2C5_BASE, slave_addr, false);

    //stores list of variable number of arguments
    va_list vargs;

    //specifies the va_list to "open" and the last fixed argument
    //so vargs knows where to start looking
    va_start(vargs, num_of_args);

    //put data to be sent into FIFO
    I2CMasterDataPut(I2C5_BASE, va_arg(vargs, uint32_t));

    // Wait until MCU is done transferring.
    while(I2CMasterBusy(I2C5_BASE));

    //if there is only one argument, we only need to use the
    //single send I2C function
    if(num_of_args == 1)
    {
        //Initiate send of data from the MCU
        I2CMasterControl(I2C5_BASE, I2C_MASTER_CMD_SINGLE_SEND);

        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C5_BASE));

        //"close" variable argument list
        va_end(vargs);
    }

    //otherwise, we start transmission of multiple bytes on the
    //I2C bus
    else
    {
        //Initiate send of data from the MCU
        I2CMasterControl(I2C5_BASE, I2C_MASTER_CMD_BURST_SEND_START);

        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C5_BASE));


        //send num_of_args-2 pieces of data, using the
        //BURST_SEND_CONT command of the I2C module
        for(i = 1; i < (num_of_args - 1); i++)
        {
            //put next piece of data into I2C FIFO
            I2CMasterDataPut(I2C5_BASE, va_arg(vargs, uint32_t));
            //send next data that was just placed into FIFO
            I2CMasterControl(I2C5_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);

            // Wait until MCU is done transferring.
            while(I2CMasterBusy(I2C5_BASE));

        }

        //put last piece of data into I2C FIFO
        I2CMasterDataPut(I2C5_BASE, va_arg(vargs, uint32_t));
        //send next data that was just placed into FIFO
        I2CMasterControl(I2C5_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
        // Wait until MCU is done transferring.
        while(I2CMasterBusBusy(I2C5_BASE));

        //"close" variable args list
        va_end(vargs);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////

//read specified register on slave device
uint32_t I2C5Receive(uint32_t slave_addr, uint8_t reg)
{
    //specify that we are writing (a register address) to the
    //slave device
    I2CMasterSlaveAddrSet(I2C5_BASE, slave_addr, false);

    //specify register to be read
    I2CMasterDataPut(I2C5_BASE, reg);

    //send control byte and register address byte to slave device
    I2CMasterControl(I2C5_BASE, I2C_MASTER_CMD_BURST_SEND_START);

    //wait for MCU to finish transaction
    while(!(I2CMasterBusy(I2C5_BASE)));

    while(I2CMasterBusy(I2C5_BASE));

    //specify that we are going to read from slave device
    I2CMasterSlaveAddrSet(I2C5_BASE, slave_addr, true);

    //send control byte and read from the register we
    //specified
    I2CMasterControl(I2C5_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

    //wait for MCU to finish transaction
    while(I2CMasterBusy(I2C5_BASE));

    //return data pulled from the specified register
    return I2CMasterDataGet(I2C5_BASE);
}

/////////////////////////////////////////////////////////////////////////////////////////////

//initialize I2C module 2
//Slightly modified version of TI's example code
void InitI2C2(void)
{

    // Configure Pins for I2C2 Master Interface
    GPIOPinConfigure(GPIO_PN5_I2C2SCL);
    GPIOPinConfigure(GPIO_PN4_I2C2SDA);
    GPIOPinTypeI2C(GPIO_PORTN_BASE, GPIO_PIN_4);
    GPIOPinTypeI2CSCL(GPIO_PORTN_BASE, GPIO_PIN_5);

    // Stop the Clock, Reset and Enable I2C Module
    // in Master Function
    SysCtlPeripheralDisable(SYSCTL_PERIPH_I2C2);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);

    // Wait for the Peripheral to be ready for programming
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C2));

    // Initialize and Configure the Master Module
    //I2CMasterInitExpClk(I2C2_BASE, SysCtlClockGet(), false); //para 100Khz.
    I2CMasterInitExpClk(I2C2_BASE, SysCtlClockGet(), true); //para 400Khz.

    // Enable the Glitch Filter
    I2CMasterGlitchFilterConfigSet(I2C2_BASE, I2C_MASTER_GLITCH_FILTER_8);

}

/////////////////////////////////////////////////////////////////////////////////////////////



