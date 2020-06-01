
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "iib_devices/can_bus.h"
#include "iib_devices/iib_data.h"
#include "iib_devices/board_temp_hum.h"
#include "iib_devices/input.h"
#include "iib_devices/application.h"
#include "iib_devices/adc_internal.h"
#include "iib_devices/leds.h"
#include "board_drivers/hardware_def.h"
#include "peripheral_drivers/gpio/gpio_driver.h"

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//
// A flag to indicate that some transmission error occurred.
//
//*****************************************************************************
volatile bool g_bErrFlag = 0;

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//
// A flag for the interrupt handler to indicate that a message was received.
//
//*****************************************************************************
volatile bool g_bRXFlag = 0;

/////////////////////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 *                          Object Messages
 *****************************************************************************/
volatile uint8_t msg_obj_sent;

/////////////////////////////////////////////////////////////////////////////////////////////

tCANMsgObject transmit_message;
tCANMsgObject event_message;
tCANMsgObject receive_message;

/////////////////////////////////////////////////////////////////////////////////////////////

uint8_t itlk_message_data[INTERLOCK_MESSAGE_LEN];
uint8_t alarm_message_data[ALARM_MESSAGE_LEN];
uint8_t request_data_rx[DATA_REQUEST_MESSAGE_LEN];
uint8_t request_data_tx[DATA_SEND_MESSAGE_LEN];
uint8_t reset_msg_data[RESET_ITLK_MESSAGE_LEN];
uint8_t heart_beat_data[HEART_BEAT_MESSAGE_LEN];

/////////////////////////////////////////////////////////////////////////////////////////////

volatile uint8_t can_address    = 0;

/////////////////////////////////////////////////////////////////////////////////////////////

static void handle_reset_message(void);

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//
// This function is the interrupt handler for the CAN peripheral.  It checks
// for the cause of the interrupt, and maintains a count of all messages that
// have been transmitted.
//
//*****************************************************************************
void can_isr(void)
{
    uint32_t ui32Status;

    // Read the CAN interrupt status to find the cause of the interrupt
    ui32Status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);

    // If the cause is a controller status interrupt, then get the status
    if(ui32Status == CAN_INT_INTID_STATUS)
    {

        // Read the controller status.  This will return a field of status
        // error bits that can indicate various errors.  Error processing
        // is not done in this example for simplicity.  Refer to the
        // API documentation for details about the error status bits.
        // The act of reading this status will clear the interrupt.  If the
        // CAN peripheral is not connected to a CAN bus with other CAN devices
        // present, then errors will occur and will be indicated in the
        // controller status.
        ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);


        // Set a flag to indicate some errors may have occurred.
        g_bErrFlag = 1;
    }


    // Check if the cause is message object 1, which what we are using for
    // sending messages.
    else if(ui32Status == INTERLOCK_MESSAGE_OBJ_ID)
    {

        // Getting to this point means that the TX interrupt occurred on
        // message object 1, and the message TX is complete.  Clear the
        // message object interrupt.

        CANIntClear(CAN0_BASE, INTERLOCK_MESSAGE_OBJ_ID);

        /* Tx object. Nothing to do for now. */


        // Since the message was sent, clear any error flags.
        g_bErrFlag = 0;
    }

    else if(ui32Status == ALARM_MESSAGE_OBJ_ID)
    {
        CANIntClear(CAN0_BASE, ALARM_MESSAGE_OBJ_ID);

        /* Tx object. Nothing to do for now. */
        g_bErrFlag = 0;
    }

    else if(ui32Status == RESET_ITLK_MESSAGE_OBJ_ID)
    {
        CANIntClear(CAN0_BASE, RESET_ITLK_MESSAGE_OBJ_ID);

        handle_reset_message();

        g_bErrFlag = 0;
    }

    else if(ui32Status == DATA_REQUEST_MESSAGE_OBJ_ID)
    {
        CANIntClear(CAN0_BASE, DATA_REQUEST_MESSAGE_OBJ_ID);

        // Do Nothing
        g_bErrFlag = 0;

    }

    else if (ui32Status == DATA_SEND_OBJ_ID)
    {
        CANIntClear(CAN0_BASE, DATA_SEND_OBJ_ID);

        msg_obj_sent = 1;

        g_bErrFlag = 0;
    }

    else if(ui32Status == RECV_PARAM_MESSAGE_OBJ_ID)
    {
        CANIntClear(CAN0_BASE, RECV_PARAM_MESSAGE_OBJ_ID);

        /* TODO: Update params. */

        g_bErrFlag = 0;
    }

    else if (ui32Status == HEART_BEAT_MESSAGE_OB_ID)
    {
        CANIntClear(CAN0_BASE, HEART_BEAT_MESSAGE_OB_ID);

        /* Tx object. Nothing to do for now. */

        g_bErrFlag = 0;
    }


    // Otherwise, something unexpected caused the interrupt.  This should
    // never happen.

    else
    {

        // Spurious interrupt handling can go here.

    }
}

/////////////////////////////////////////////////////////////////////////////////////////////

void InitCan(uint32_t ui32SysClock)
{

    // Configure the GPIO pin muxing to select CAN0 functions for these pins.
    GPIOPinConfigure(GPIO_PA0_CAN0RX);
    GPIOPinConfigure(GPIO_PA1_CAN0TX);

    // Enable the alternate function on the GPIO pins.  The above step selects
    GPIOPinTypeCAN(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // The GPIO port and pins have been set up for CAN.  The CAN peripheral
    // must be enabled.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);

    // Initialize the CAN controller
    CANInit(CAN0_BASE);

    // Set up the bit rate for the CAN bus 1Mbps
    CANBitRateSet(CAN0_BASE, ui32SysClock, 1000000);

    // Enable interrupts on the CAN peripheral.
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

    CANIntRegister(CAN0_BASE, can_isr);

    IntPrioritySet(INT_CAN0, 1);

    // Enable the CAN interrupt on the processor (NVIC).
    IntEnable(INT_CAN0);

    // Enable the CAN for operation.
    CANEnable(CAN0_BASE);

    receive_message.ui32MsgID = ParamsSetMsgId;
    receive_message.ui32MsgIDMask = 0xfffff;
    receive_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    receive_message.ui32MsgLen = RECV_PARAM_MESSAGE_LEN;

    CANMessageSet(CAN0_BASE, RECV_PARAM_MESSAGE_OBJ_ID, &receive_message,
                                                              MSG_OBJ_TYPE_RX);
    /*
     * Message object to reset events (interlock and alarms)
     * */

    receive_message.ui32MsgID = 0;
    receive_message.ui32MsgLen = RESET_ITLK_MESSAGE_LEN;

    CANMessageSet(CAN0_BASE, RESET_ITLK_MESSAGE_OBJ_ID, &receive_message,
                                                              MSG_OBJ_TYPE_RX);

    receive_message.ui32MsgID = DataRequestMsgId;
    receive_message.ui32MsgLen = DATA_REQUEST_MESSAGE_LEN;

    CANMessageSet(CAN0_BASE, DATA_REQUEST_MESSAGE_OBJ_ID, &receive_message,
                                                              MSG_OBJ_TYPE_RX);

    transmit_message.ui32MsgID = 0;
    transmit_message.ui32MsgIDMask = 0;
    transmit_message.ui32Flags = MSG_OBJ_TX_INT_ENABLE;

    // Module ID
    can_address = BoardAddressRead();

    if (can_address == 0) can_address = 1;

}

/////////////////////////////////////////////////////////////////////////////////////////////

void handle_reset_message(void)
{
    uint8_t id;

    receive_message.pui8MsgData = reset_msg_data;
    CANMessageGet(CAN0_BASE, RESET_ITLK_MESSAGE_OBJ_ID, &receive_message, 0);

    id = reset_msg_data[0];

    if (id == 1)
	{
        InterlockClear();
        AlarmClear();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

void send_data_message(uint8_t var)
{

    request_data_tx[0] = can_address;
    request_data_tx[1] = var;
    request_data_tx[2] = 0;
    request_data_tx[3] = 0;
    request_data_tx[4] = g_controller_iib.iib_signals[var].u8[0];
    request_data_tx[5] = g_controller_iib.iib_signals[var].u8[1];
    request_data_tx[6] = g_controller_iib.iib_signals[var].u8[2];
    request_data_tx[7] = g_controller_iib.iib_signals[var].u8[3];

    transmit_message.ui32MsgID =  DataSendMsgId;
    transmit_message.ui32MsgLen = DATA_SEND_MESSAGE_LEN;
    transmit_message.pui8MsgData = request_data_tx;

    CANMessageSet(CAN0_BASE, DATA_SEND_OBJ_ID, &transmit_message,
                                                          MSG_OBJ_TYPE_TX);
}

/////////////////////////////////////////////////////////////////////////////////////////////

uint16_t get_can_address(void)
{
    return can_address;
}

/////////////////////////////////////////////////////////////////////////////////////////////


