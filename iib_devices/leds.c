
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "iib_devices/leds.h"
#include "board_drivers/hardware_def.h"
#include "peripheral_drivers/gpio/gpio_driver.h"
#include "system/system.h"

/////////////////////////////////////////////////////////////////////////////////////////////

void LedsInit(void)
{
   set_gpio_as_output(STATUS_LED_BASE, STATUS_LED_PIN);
   set_gpio_as_output(LED_1_BASE, LED_1_PIN);
   set_gpio_as_output(LED_2_BASE, LED_2_PIN);
   set_gpio_as_output(LED_3_BASE, LED_3_PIN);
   set_gpio_as_output(LED_4_BASE, LED_4_PIN);
   set_gpio_as_output(LED_5_BASE, LED_5_PIN);
   set_gpio_as_output(LED_6_BASE, LED_6_PIN);
   set_gpio_as_output(LED_7_BASE, LED_7_PIN);
   set_gpio_as_output(LED_8_BASE, LED_8_PIN);
   set_gpio_as_output(LED_9_BASE, LED_9_PIN);
   set_gpio_as_output(LED_10_BASE, LED_10_PIN);

   set_gpio_as_output(EXTERNAL_ENABLE_BASE, EXTERNAL_ENABLE_PIN);
   set_pin(EXTERNAL_ENABLE_BASE, EXTERNAL_ENABLE_PIN);
   
   clear_pin(STATUS_LED_BASE, STATUS_LED_PIN);
   clear_pin(LED_1_BASE, LED_1_PIN);
   clear_pin(LED_2_BASE, LED_2_PIN);
   clear_pin(LED_3_BASE, LED_3_PIN);
   clear_pin(LED_4_BASE, LED_4_PIN);
   clear_pin(LED_5_BASE, LED_5_PIN);
   clear_pin(LED_6_BASE, LED_6_PIN);
   clear_pin(LED_7_BASE, LED_7_PIN);
   clear_pin(LED_8_BASE, LED_8_PIN);
   clear_pin(LED_9_BASE, LED_9_PIN);
   clear_pin(LED_10_BASE, LED_10_PIN);

}

/////////////////////////////////////////////////////////////////////////////////////////////

void LedPong(void)
{
    set_pin(LED_1_BASE, LED_1_PIN);
    delay_ms(60);

    set_pin(LED_2_BASE, LED_2_PIN);
    clear_pin(LED_1_BASE, LED_1_PIN);
    delay_ms(60);
    
    set_pin(LED_3_BASE, LED_3_PIN);
    clear_pin(LED_2_BASE, LED_2_PIN);
    delay_ms(60);
    
    set_pin(LED_4_BASE, LED_4_PIN);
    clear_pin(LED_3_BASE, LED_3_PIN);
    delay_ms(60);
    
    set_pin(LED_5_BASE, LED_5_PIN);
    clear_pin(LED_4_BASE, LED_4_PIN);
    delay_ms(60);

    set_pin(LED_6_BASE, LED_6_PIN);
    clear_pin(LED_5_BASE, LED_5_PIN);
    delay_ms(60);

    set_pin(LED_7_BASE, LED_7_PIN);
    clear_pin(LED_6_BASE, LED_6_PIN);
    delay_ms(60);

    set_pin(LED_8_BASE, LED_8_PIN);
    clear_pin(LED_7_BASE, LED_7_PIN);
    delay_ms(60);

    set_pin(LED_9_BASE, LED_9_PIN);
    clear_pin(LED_8_BASE, LED_8_PIN);
    delay_ms(60);

    set_pin(LED_10_BASE, LED_10_PIN);
    clear_pin(LED_9_BASE, LED_9_PIN);
    delay_ms(60);
    
    set_pin(LED_9_BASE, LED_9_PIN);
    clear_pin(LED_10_BASE, LED_10_PIN);
    delay_ms(60);

    set_pin(LED_8_BASE, LED_8_PIN);
    clear_pin(LED_9_BASE, LED_9_PIN);
    delay_ms(60);
    
    set_pin(LED_7_BASE, LED_7_PIN);
    clear_pin(LED_8_BASE, LED_8_PIN);
    delay_ms(60);

    set_pin(LED_6_BASE, LED_6_PIN);
    clear_pin(LED_7_BASE, LED_7_PIN);
    delay_ms(60);

    set_pin(LED_5_BASE, LED_5_PIN);
    clear_pin(LED_6_BASE, LED_6_PIN);
    delay_ms(60);
    
    set_pin(LED_4_BASE, LED_4_PIN);
    clear_pin(LED_5_BASE, LED_5_PIN);
    delay_ms(60);
    
    set_pin(LED_3_BASE, LED_3_PIN);
    clear_pin(LED_4_BASE, LED_4_PIN);
    delay_ms(60);
    
    set_pin(LED_2_BASE, LED_2_PIN);
    clear_pin(LED_3_BASE, LED_3_PIN);
    delay_ms(60);
    
    set_pin(LED_1_BASE, LED_1_PIN);
    clear_pin(LED_2_BASE, LED_2_PIN);
    delay_ms(60);

    clear_pin(LED_1_BASE, LED_1_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LedVU(void)
{
    toggle_pin(LED_1_BASE, LED_1_PIN);
    delay_ms(60);
    toggle_pin(LED_2_BASE, LED_2_PIN);
    delay_ms(60);
    toggle_pin(LED_3_BASE, LED_3_PIN);
    delay_ms(60);
    toggle_pin(LED_4_BASE, LED_4_PIN);
    delay_ms(60);
    toggle_pin(LED_5_BASE, LED_5_PIN);
    delay_ms(60);
    toggle_pin(LED_6_BASE, LED_6_PIN);
    delay_ms(60);
    toggle_pin(LED_7_BASE, LED_7_PIN);
    delay_ms(60);
    toggle_pin(LED_8_BASE, LED_8_PIN);
    delay_ms(60);
    toggle_pin(LED_9_BASE, LED_9_PIN);
    delay_ms(60);
    toggle_pin(LED_10_BASE, LED_10_PIN);
    delay_ms(60);
    clear_pin(LED_10_BASE, LED_10_PIN);
    delay_ms(60);

    toggle_pin(LED_9_BASE, LED_9_PIN);
    delay_ms(60);
    toggle_pin(LED_8_BASE, LED_8_PIN);
    delay_ms(60);
    toggle_pin(LED_7_BASE, LED_7_PIN);
    delay_ms(60);
    toggle_pin(LED_6_BASE, LED_6_PIN);
    delay_ms(60);
    toggle_pin(LED_5_BASE, LED_5_PIN);
    delay_ms(60);
    toggle_pin(LED_4_BASE, LED_4_PIN);
    delay_ms(60);
    toggle_pin(LED_3_BASE, LED_3_PIN);
    delay_ms(60);
    toggle_pin(LED_2_BASE, LED_2_PIN);
    delay_ms(60);
    toggle_pin(LED_1_BASE, LED_1_PIN);
    delay_ms(60);
    clear_pin(LED_1_BASE, LED_1_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LedBarRunning(void)
{
    toggle_pin(LED_1_BASE, LED_1_PIN);
    delay_ms(40);
    toggle_pin(LED_2_BASE, LED_2_PIN);
    delay_ms(40);
    toggle_pin(LED_3_BASE, LED_3_PIN);
    delay_ms(40);
    toggle_pin(LED_4_BASE, LED_4_PIN);
    delay_ms(40);
    toggle_pin(LED_5_BASE, LED_5_PIN);
    delay_ms(40);
    toggle_pin(LED_6_BASE, LED_6_PIN);
    delay_ms(40);
    toggle_pin(LED_7_BASE, LED_7_PIN);
    delay_ms(40);
    toggle_pin(LED_8_BASE, LED_8_PIN);
    delay_ms(40);
    toggle_pin(LED_9_BASE, LED_9_PIN);
    delay_ms(40);
    toggle_pin(LED_10_BASE, LED_10_PIN);
    delay_ms(40);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LedBarBlink(void)
{
    if(read_pin(LED_1_BASE, LED_1_PIN))
    {
       clear_pin(LED_1_BASE, LED_1_PIN);
       clear_pin(LED_2_BASE, LED_2_PIN);
       clear_pin(LED_3_BASE, LED_3_PIN);
       clear_pin(LED_4_BASE, LED_4_PIN);
       clear_pin(LED_5_BASE, LED_5_PIN);
       clear_pin(LED_6_BASE, LED_6_PIN);
       clear_pin(LED_7_BASE, LED_7_PIN);
       clear_pin(LED_8_BASE, LED_8_PIN);
       clear_pin(LED_9_BASE, LED_9_PIN);
       clear_pin(LED_10_BASE, LED_10_PIN);
    }
    else
    {
       set_pin(LED_1_BASE, LED_1_PIN);
       set_pin(LED_2_BASE, LED_2_PIN);
       set_pin(LED_3_BASE, LED_3_PIN);
       set_pin(LED_4_BASE, LED_4_PIN);
       set_pin(LED_5_BASE, LED_5_PIN);
       set_pin(LED_6_BASE, LED_6_PIN);
       set_pin(LED_7_BASE, LED_7_PIN);
       set_pin(LED_8_BASE, LED_8_PIN);
       set_pin(LED_9_BASE, LED_9_PIN);
       set_pin(LED_10_BASE, LED_10_PIN);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////

void LedBarOff(void)
{
    clear_pin(LED_1_BASE, LED_1_PIN);
    clear_pin(LED_2_BASE, LED_2_PIN);
    clear_pin(LED_3_BASE, LED_3_PIN);
    clear_pin(LED_4_BASE, LED_4_PIN);
    clear_pin(LED_5_BASE, LED_5_PIN);
    clear_pin(LED_6_BASE, LED_6_PIN);
    clear_pin(LED_7_BASE, LED_7_PIN);
    clear_pin(LED_8_BASE, LED_8_PIN);
    clear_pin(LED_9_BASE, LED_9_PIN);
    clear_pin(LED_10_BASE, LED_10_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led1TurnOn(void)
{
    set_pin(LED_1_BASE, LED_1_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led1TurnOff(void)
{
    clear_pin(LED_1_BASE, LED_1_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led1Toggle(void)
{
    toggle_pin(LED_1_BASE, LED_1_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led2TurnOn(void)
{
    set_pin(LED_2_BASE, LED_2_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led2TurnOff(void)
{
    clear_pin(LED_2_BASE, LED_2_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led2Toggle(void)
{
    toggle_pin(LED_2_BASE, LED_2_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led3TurnOn(void)
{
    set_pin(LED_3_BASE, LED_3_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led3TurnOff(void)
{
    clear_pin(LED_3_BASE, LED_3_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led3Toggle(void)
{
    toggle_pin(LED_3_BASE, LED_3_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led4TurnOn(void)
{
    set_pin(LED_4_BASE, LED_4_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led4TurnOff(void)
{
    clear_pin(LED_4_BASE, LED_4_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led4Toggle(void)
{
    toggle_pin(LED_4_BASE, LED_4_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led5TurnOn(void)
{
    set_pin(LED_5_BASE, LED_5_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led5TurnOff(void)
{
    clear_pin(LED_5_BASE, LED_5_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led5Toggle(void)
{
    toggle_pin(LED_5_BASE, LED_5_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led6TurnOn(void)
{
    set_pin(LED_6_BASE, LED_6_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led6TurnOff(void)
{
    clear_pin(LED_6_BASE, LED_6_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led6Toggle(void)
{
    toggle_pin(LED_6_BASE, LED_6_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led7TurnOn(void)
{
    set_pin(LED_7_BASE, LED_7_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led7TurnOff(void)
{
    clear_pin(LED_7_BASE, LED_7_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led7Toggle(void)
{
    toggle_pin(LED_7_BASE, LED_7_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led8TurnOn(void)
{
    set_pin(LED_8_BASE, LED_8_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led8TurnOff(void)
{
    clear_pin(LED_8_BASE, LED_8_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led8Toggle(void)
{
    toggle_pin(LED_8_BASE, LED_8_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led9TurnOn(void)
{
    set_pin(LED_9_BASE, LED_9_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led9TurnOff(void)
{
    clear_pin(LED_9_BASE, LED_9_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led9Toggle(void)
{
    toggle_pin(LED_9_BASE, LED_9_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led10TurnOn(void)
{
    set_pin(LED_10_BASE, LED_10_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led10TurnOff(void)
{
    clear_pin(LED_10_BASE, LED_10_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void Led10Toggle(void)
{
    toggle_pin(LED_10_BASE, LED_10_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void RunToggle(void)
{
    toggle_pin(STATUS_LED_BASE, STATUS_LED_PIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////



