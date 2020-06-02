
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_gpio.h"
#include "inc/hw_timer.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "board_drivers/hardware_def.h"
#include "peripheral_drivers/gpio/gpio_driver.h"
#include "iib_devices/can_bus.h"
#include "iib_devices/adc_internal.h"
#include "iib_devices/application.h"
#include "iib_devices/leds.h"
#include "iib_devices/output.h"
#include "iib_devices/input.h"
#include "iib_devices/board_temp_hum.h"
#include "iib_devices/ntc_isolated_i2c.h"
#include "iib_devices/pt100.h"
#include "iib_devices/task_iib.h"
#include "iib_devices/iib_data.h"
#include "iib_devices/pwm_software.h"
#include "system/system.h"

/////////////////////////////////////////////////////////////////////////////////////////////

#define ON  ~0
#define OFF 0

/////////////////////////////////////////////////////////////////////////////////////////////

#define SYSCLOCK    120000000

/////////////////////////////////////////////////////////////////////////////////////////////

volatile static uint32_t millis = 0;

volatile static uint8_t can_timestamp_100ms = 0;

/////////////////////////////////////////////////////////////////////////////////////////////

static uint32_t ui32SysClock;

/////////////////////////////////////////////////////////////////////////////////////////////

uint32_t SysCtlClockGetTM4C129(void)
{
    return ui32SysClock;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void delay_ms(uint32_t time)
{
    volatile uint32_t temp = millis;
    while ((millis - temp) < time);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void IntTimer1msHandler(void)
{
    // Clear the timer interrupt.
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // var count delay function
    millis++;

    // can time stamp
    if(can_timestamp_100ms >= 10)
    {
        RunToggle();

        can_timestamp_100ms = 0;

        RunToggle();
    }
    else can_timestamp_100ms++;

    //RunToggle();
    //sample_adc();
    //RunToggle();
    //task_1_ms();
}

/////////////////////////////////////////////////////////////////////////////////////////////

/*void IntTimer100usHandler(void)
{
    // Clear the timer interrupt.
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    RunToggle();
    task_100_us();
    RunToggle();
}*/

/////////////////////////////////////////////////////////////////////////////////////////////

void Timer_1ms_Init(void)
{
    // Enable timer 1.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    // Configure the two 32-bit periodic timers.
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER1_BASE, TIMER_A, (SYSCLOCK / 1000) - 1);

    // Setup the interrupts for the timer timeouts.
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntRegister(TIMER1_BASE, TIMER_A, IntTimer1msHandler);
    IntPrioritySet(INT_TIMER1A, 1);

    // Enable the timer 1.
    TimerEnable(TIMER1_BASE, TIMER_A);
}

/////////////////////////////////////////////////////////////////////////////////////////////

/*void Timer_100us_Init(void)
{
    // Enable timer 0.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Configure the two 32-bit periodic timers.
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, (SYSCLOCK / 10000) - 1);

    // Setup the interrupts for the timer timeouts.
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntRegister(TIMER0_BASE, TIMER_A, IntTimer100usHandler);
    IntPrioritySet(INT_TIMER0A, 0);

    // Enable the timer 0.
    TimerEnable(TIMER0_BASE, TIMER_A);
}*/

/////////////////////////////////////////////////////////////////////////////////////////////

void init_system(void)
{

    ui32SysClock = SysCtlClockFreqSet((SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                                       SYSCTL_XTAL_25MHZ | SYSCTL_CFG_VCO_480), 120000000);

    pinout_config();

    init_control_framwork(&g_controller_iib);

    AdcsInit();

    //LEDs initialization
    LedsInit();

    //Digital Input initialization
    InputInit();

    //Digital Output initialization
    OutputInit();

    InitCan(ui32SysClock);

    Timer_1ms_Init();

    //Timer_100us_Init();

    PWM1SoftwareInit();

    //PT100 channels initialization
    Pt100Init();

    //Rh & Board Temp sensors initialization
    RhBoardTempSenseInit();

    //ADS1014 with NTC 5K Igbt1 and Igbt2 initialization
    NtcInit();

    //Led test
    LedPong();

    //Block application to sign that CAN Address is out of range
    while(get_can_address() == 0 || get_can_address() >= 496)
    {
        //Blink bar
        LedBarBlink();
        delay_ms(40);
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////





