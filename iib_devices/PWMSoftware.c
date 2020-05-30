
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_gpio.h"
#include "inc/hw_timer.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "board_drivers/hardware_def.h"
#include "peripheral_drivers/gpio/gpio_driver.h"
#include "iib_devices/leds.h"
#include "iib_devices/PWMSoftware.h"

/////////////////////////////////////////////////////////////////////////////////////////////

// PWM1.Period = System Clock Frequency (Hz) / PWM Frequency (Hz)
// PWM1.Period = 120Mhz / 1Khz
// PWM1.Period = 120000

void PWM1Software_SetDuty(volatile uint32_t enable, unsigned char duty, unsigned long period)
{
    PWM1.Enable = enable;

    PWM1.Period = period;

    if(duty>100) return;

    if(duty==0)
    {
       PWM1.Low = PWM1.Period;
    }
    else if(duty==100)
    {
       PWM1.Low = 0;
    }
    else
    {
       PWM1.Low = (unsigned long) ((double) duty/100 * PWM1.Period);
    }

    PWM1.High =  PWM1.Period - PWM1.Low;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// PWM2.Period = System Clock Frequency (Hz) / PWM Frequency (Hz)
// PWM2.Period = 120Mhz / 1Khz
// PWM2.Period = 120000

void PWM2Software_SetDuty(volatile uint32_t enable, unsigned char duty, unsigned long period)
{
    PWM2.Enable = enable;

    PWM2.Period = period;

    if(duty>100) return;

    if(duty==0)
    {
       PWM2.Low = PWM2.Period;
    }
    else if(duty==100)
    {
       PWM2.Low = 0;
    }
    else
    {
       PWM2.Low = (unsigned long) ((double) duty/100 * PWM2.Period);
    }

    PWM2.High =  PWM2.Period - PWM2.Low;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void IntPWM1SoftHandler(void)
{
    if(TimerIntStatus(TIMER2_BASE,true) & TIMER_TIMA_TIMEOUT)
    {
        TimerIntClear(TIMER2_BASE,TIMER_TIMA_TIMEOUT);

        if(PWM1.Enable)
        {
            if(PWM1.Low)
            {

                GPIOPinWrite(LED_6_BASE, LED_6_PIN, LED_6_PIN);
                //GPIOPinWrite(LED_7_BASE, LED_7_PIN, LED_7_PIN);
                //GPIOPinWrite(LED_8_BASE, LED_8_PIN, LED_8_PIN);

                TimerLoadSet(TIMER2_BASE, TIMER_A, PWM1.Low);

                PWM1.Enable = 0;
            }
        }
        else
        {
            if(PWM1.High)
            {
                GPIOPinWrite(LED_6_BASE, LED_6_PIN, 0);
                //GPIOPinWrite(LED_7_BASE, LED_7_PIN, 0);
                //GPIOPinWrite(LED_8_BASE, LED_8_PIN, 0);

                TimerLoadSet(TIMER2_BASE, TIMER_A, PWM1.High);

                PWM1.Enable = 1;
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Timer_PWM1_Init(void)
{
    // Enable timer 2.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);

    // Configure the two 32-bit periodic timers.
    TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER2_BASE, TIMER_A, PWM1.Low - 1);

    // Setup the interrupts for the timer timeouts.
    IntEnable(INT_TIMER2A);
    TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntRegister(TIMER2_BASE, TIMER_A, IntPWM1SoftHandler);
    IntPrioritySet(INT_TIMER2A, 2);

    // Enable the timer 2.
    TimerEnable(TIMER2_BASE, TIMER_A);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void PWM1SoftwareInit(void)
{

    PWM1Software_SetDuty(0,0,120000);

    Timer_PWM1_Init();

    PWM1Software_SetDuty(0,2,120000);

}

/////////////////////////////////////////////////////////////////////////////////////////////








