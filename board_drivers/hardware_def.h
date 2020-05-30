/******************************************************************************
 * Copyright (C) 2017 by LNLS - Power Electronics Group (ELP)
 *
 * Redistribution, modification or use of this software in source or binary
 * forms is permitted as long as the files maintain this copyright. Users are
 * permitted to modify this and use it without restrictions. LNLS is not
 * liable for any misuse of this material.
 *
 *****************************************************************************/
/**
 * @file hardware_def.h
 * @brief Hardware definitions for Interlock board.
 *
 * @author Allef Araujo
 * @date 11/08/2017
 *
 */

/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HARDWARE_DEF_H_
#define HARDWARE_DEF_H_

/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

/////////////////////////////////////////////////////////////////////////////////////////////

#define SYSCLOCK    120000000

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// GPIO for LEDs
//*****************************************************************************
#define LED_1_BASE          GPIO_PORTB_BASE
#define LED_2_BASE          GPIO_PORTB_BASE
#define LED_3_BASE          GPIO_PORTE_BASE
#define LED_4_BASE          GPIO_PORTE_BASE
#define LED_5_BASE          GPIO_PORTK_BASE
#define LED_6_BASE          GPIO_PORTE_BASE
#define LED_7_BASE          GPIO_PORTC_BASE
#define LED_8_BASE          GPIO_PORTC_BASE
#define LED_9_BASE          GPIO_PORTC_BASE
#define LED_10_BASE         GPIO_PORTC_BASE

#define LED_1_PIN           GPIO_PIN_4
#define LED_2_PIN           GPIO_PIN_5
#define LED_3_PIN           GPIO_PIN_5
#define LED_4_PIN           GPIO_PIN_4
#define LED_5_PIN           GPIO_PIN_3
#define LED_6_PIN           GPIO_PIN_3
#define LED_7_PIN           GPIO_PIN_6
#define LED_8_PIN           GPIO_PIN_7
#define LED_9_PIN           GPIO_PIN_4
#define LED_10_PIN          GPIO_PIN_5

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// GPIO for GPDI
//*****************************************************************************
#define GPDI_1_BASE         GPIO_PORTM_BASE
#define GPDI_2_BASE         GPIO_PORTM_BASE
#define GPDI_3_BASE         GPIO_PORTM_BASE
#define GPDI_4_BASE         GPIO_PORTM_BASE
#define GPDI_5_BASE         GPIO_PORTL_BASE
#define GPDI_6_BASE         GPIO_PORTL_BASE
#define GPDI_7_BASE         GPIO_PORTL_BASE
#define GPDI_8_BASE         GPIO_PORTL_BASE
#define GPDI_9_BASE         GPIO_PORTL_BASE
#define GPDI_10_BASE        GPIO_PORTL_BASE
#define GPDI_11_BASE        GPIO_PORTL_BASE
#define GPDI_12_BASE        GPIO_PORTL_BASE

#define GPDI_1_PIN          GPIO_PIN_3
#define GPDI_2_PIN          GPIO_PIN_2
#define GPDI_3_PIN          GPIO_PIN_1
#define GPDI_4_PIN          GPIO_PIN_0
#define GPDI_5_PIN          GPIO_PIN_0
#define GPDI_6_PIN          GPIO_PIN_1
#define GPDI_7_PIN          GPIO_PIN_2
#define GPDI_8_PIN          GPIO_PIN_3
#define GPDI_9_PIN          GPIO_PIN_4
#define GPDI_10_PIN         GPIO_PIN_5
#define GPDI_11_PIN         GPIO_PIN_6
#define GPDI_12_PIN         GPIO_PIN_7

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// GPIO for GPDO
//*****************************************************************************
#define GPDO_1_BASE         GPIO_PORTH_BASE
#define GPDO_2_BASE         GPIO_PORTH_BASE
#define GPDO_3_BASE         GPIO_PORTH_BASE
#define GPDO_4_BASE         GPIO_PORTH_BASE

#define GPDO_1_PIN          GPIO_PIN_3
#define GPDO_2_PIN          GPIO_PIN_2
#define GPDO_3_PIN          GPIO_PIN_1
#define GPDO_4_PIN          GPIO_PIN_0

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// GPIO for ethernet LEDs
//*****************************************************************************
#define ETH_RXTX_BASE       GPIO_PORTK_BASE
#define ETH_LINK_BASE       GPIO_PORTK_BASE

#define ETH_RXTX_PIN        GPIO_PIN_4
#define ETH_LINK_PIN        GPIO_PIN_7

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// GPIO for test points
//*****************************************************************************
#define TP_1_BASE           GPIO_PORTM_BASE
#define TP_2_BASE           GPIO_PORTM_BASE
#define TP_3_BASE           GPIO_PORTM_BASE

#define TP_1_PIN            GPIO_PIN_4
#define TP_2_PIN            GPIO_PIN_5
#define TP_3_PIN            GPIO_PIN_6

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// GPIO for Relays
//*****************************************************************************
#define RELAY_1_BASE           GPIO_PORTF_BASE
#define RELAY_2_BASE           GPIO_PORTF_BASE

#define RELAY_1_PIN            GPIO_PIN_3
#define RELAY_2_PIN            GPIO_PIN_4

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// Offboard I2C Isolated
//*****************************************************************************
#define I2C_DRIVER_BASE             I2C2_BASE
#define I2C_DRIVER_PIN_BASE         GPIO_PORTN_BASE
#define I2C_DRIVER_SCL_PIN          GPIO_PIN_5
#define I2C_DRIVER_SDA_PIN          GPIO_PIN_4
#define I2C_DRIVER_SCL              GPIO_PN5_I2C2SCL
#define I2C_DRIVER_SDA              GPIO_PN4_I2C2SDA
#define I2C_DRIVER_SYSCTL           SYSCTL_PERIPH_I2C2
#define I2C_DRIVER_FIFO_CTL         I2C_2_FIFOCTL

#define I2C_EXTERNAL_BASE           I2C6_BASE
#define I2C_EXTERNAL_PIN_BASE       GPIO_PORTA_BASE
#define I2C_EXTERNAL_SCL_PIN        GPIO_PIN_6
#define I2C_EXTERNAL_SDA_PIN        GPIO_PIN_7
#define I2C_EXTERNAL_SCL            GPIO_PA6_I2C6SCL
#define I2C_EXTERNAL_SDA            GPIO_PA7_I2C6SDA
#define I2C_EXTERNAL_SYSCTL         SYSCTL_PERIPH_I2C6
#define I2C_EXTERNAL_FIFO_CTL       I2C_6_FIFOCTL

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// Onboard I2C
//*****************************************************************************
#define I2C_ONBOARD_BASE        I2C5_BASE
#define I2C_ONBOARD_PIN_BASE    GPIO_PORTB_BASE
#define I2C_ONBOARD_SCL_PIN     GPIO_PIN_0
#define I2C_ONBOARD_SDA_PIN     GPIO_PIN_1
#define I2C_ONBOARD_SCL         GPIO_PB0_I2C5SCL
#define I2C_ONBOARD_SDA         GPIO_PB1_I2C5SDA
#define I2C_ONBOARD_SYSCTL      SYSCTL_PERIPH_I2C5
#define I2C_ONBOARD_FIFO_CTL    I2C_5_FIFOCTL

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// CAN communication
//*****************************************************************************
#define CAN_SYSCTL          SYSCTL_PERIPH_CAN0
#define CAN_BASE            GPIO_PORTA_BASE
#define CAN_PINS            (GPIO_PIN_0 | GPIO_PIN_1)
#define CAN_RX              GPIO_PA0_CAN0RX
#define CAN_TX              GPIO_PA1_CAN0TX

#define CAN_ADD_0_BASE      GPIO_PORTP_BASE
#define CAN_ADD_1_BASE      GPIO_PORTN_BASE
#define CAN_ADD_2_BASE      GPIO_PORTN_BASE
#define CAN_ADD_3_BASE      GPIO_PORTN_BASE
#define CAN_ADD_4_BASE      GPIO_PORTN_BASE

#define CAN_ADD_0_PIN       GPIO_PIN_5
#define CAN_ADD_1_PIN       GPIO_PIN_1
#define CAN_ADD_2_PIN       GPIO_PIN_3
#define CAN_ADD_3_PIN       GPIO_PIN_0
#define CAN_ADD_4_PIN       GPIO_PIN_2

#define CAN_POWER_STS_BASE  GPIO_PORTG_BASE
#define CAN_POWER_STS_PIN   GPIO_PIN_0

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// RTD SPI Port
//*****************************************************************************
#define RTD_SPI_BASE    SSI0_BASE
#define RTD_SPI_SYSCTL  SYSCTL_PERIPH_SSI0

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// GPIO for RTD SPI pins
//*****************************************************************************
#define RTD_GPIO_PORT_BASE      GPIO_PORTA_BASE
#define RTD_SPI_CLK             GPIO_PIN_2
#define RTD_SPI_SDI             GPIO_PIN_4  //Tx
#define RTD_SPI_SDO             GPIO_PIN_5  //Rx
#define RTD_SPI_PINS            (RTD_SPI_SDI | RTD_SPI_SDO | RTD_SPI_CLK)

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// GPIO for Chip Select - RTD
//*****************************************************************************
#define RTD_CS_A0_BASE          GPIO_PORTK_BASE
#define RTD_CS_A1_BASE          GPIO_PORTK_BASE
#define RTD_SPI_CS_BASE         GPIO_PORTA_BASE

#define RTD_CS_A0_PIN           GPIO_PIN_5
#define RTD_CS_A1_PIN           GPIO_PIN_6
#define RTD_SPI_CS_PIN          GPIO_PIN_3

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// GPIO for Data Ready pins - RTD
//*****************************************************************************
#define RTD_DRDY_1_BASE         GPIO_PORTG_BASE
#define RTD_DRDY_2_BASE         GPIO_PORTF_BASE
#define RTD_DRDY_3_BASE         GPIO_PORTF_BASE
#define RTD_DRDY_4_BASE         GPIO_PORTF_BASE

#define RTD_DRDY_1_PIN          GPIO_PIN_1
#define RTD_DRDY_2_PIN          GPIO_PIN_2
#define RTD_DRDY_3_PIN          GPIO_PIN_1
#define RTD_DRDY_4_PIN          GPIO_PIN_0

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// GPIO for Chip Select - Temperature and Humidity sensor
//*****************************************************************************
#define RH_SENSE_CS_BASE        GPIO_PORTB_BASE

#define RH_SENSE_CS_PIN         GPIO_PIN_2

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// GPIO for ACDC Power Supply
//*****************************************************************************
#define DRIVER_VOLT_BASE        GPIO_PORTK_BASE
#define DRIVER_1_AMP_BASE       GPIO_PORTK_BASE
#define DRIVER_2_AMP_BASE       GPIO_PORTK_BASE

#define DRIVER_VOLT_PIN        GPIO_PIN_0
#define DRIVER_1_AMP_PIN       GPIO_PIN_1
#define DRIVER_2_AMP_PIN       GPIO_PIN_2

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// GPIO for error module
//*****************************************************************************
#define ERROR_DRIVER_1_TOP_BASE     GPIO_PORTQ_BASE
#define ERROR_DRIVER_1_BOT_BASE     GPIO_PORTQ_BASE
#define MODULE_1_OVER_TEMP_BASE     GPIO_PORTQ_BASE
#define ERROR_DRIVER_2_TOP_BASE     GPIO_PORTQ_BASE
#define ERROR_DRIVER_2_BOT_BASE     GPIO_PORTQ_BASE
#define MODULE_2_OVER_TEMP_BASE     GPIO_PORTP_BASE

#define ERROR_DRIVER_1_TOP_PIN     GPIO_PIN_3
#define ERROR_DRIVER_1_BOT_PIN     GPIO_PIN_2
#define MODULE_1_OVER_TEMP_PIN     GPIO_PIN_1
#define ERROR_DRIVER_2_TOP_PIN     GPIO_PIN_0
#define ERROR_DRIVER_2_BOT_PIN     GPIO_PIN_4
#define MODULE_2_OVER_TEMP_PIN     GPIO_PIN_0

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// GPIO for input voltage
//*****************************************************************************
#define VOLTAGE_1_BASE  GPIO_PORTD_BASE
#define VOLTAGE_2_BASE  GPIO_PORTD_BASE
#define VOLTAGE_3_BASE  GPIO_PORTD_BASE
#define VOLTAGE_4_BASE  GPIO_PORTD_BASE

#define VOLTAGE_1_PIN  GPIO_PIN_6
#define VOLTAGE_2_PIN  GPIO_PIN_7
#define VOLTAGE_3_PIN  GPIO_PIN_5
#define VOLTAGE_4_PIN  GPIO_PIN_4

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// GPIO for hall current sensor
//*****************************************************************************
#define CURRENT_1_BASE  GPIO_PORTE_BASE
#define CURRENT_2_BASE  GPIO_PORTE_BASE
#define CURRENT_3_BASE  GPIO_PORTE_BASE
#define CURRENT_4_BASE  GPIO_PORTD_BASE

#define CURRENT_1_PIN  GPIO_PIN_0
#define CURRENT_2_PIN  GPIO_PIN_1
#define CURRENT_3_PIN  GPIO_PIN_2
#define CURRENT_4_PIN  GPIO_PIN_3

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// GPIO for leakage voltage
//*****************************************************************************
#define LV_SIGNAL_1_BASE    GPIO_PORTD_BASE
#define LV_SIGNAL_2_BASE    GPIO_PORTD_BASE
#define LV_SIGNAL_3_BASE    GPIO_PORTD_BASE

#define LV_SIGNAL_1_PIN     GPIO_PIN_2
#define LV_SIGNAL_2_PIN     GPIO_PIN_1
#define LV_SIGNAL_3_PIN     GPIO_PIN_0

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// GPIO for overcurrent signal
//*****************************************************************************
#define EXTERNAL_ENABLE_BASE    GPIO_PORTJ_BASE
#define EXTERNAL_OVERCURRENT_BASE           GPIO_PORTJ_BASE

#define EXTERNAL_ENABLE_PIN     GPIO_PIN_0
#define EXTERNAL_OVERCURRENT_PIN            GPIO_PIN_1

/////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// GPIO for status LED
//*****************************************************************************
#define STATUS_LED_BASE     GPIO_PORTM_BASE
#define STATUS_LED_PIN      GPIO_PIN_7

/////////////////////////////////////////////////////////////////////////////////////////////

extern void pinout_config();

/////////////////////////////////////////////////////////////////////////////////////////////

#endif /* HARDWARE_DEF_H_ */

/////////////////////////////////////////////////////////////////////////////////////////////


