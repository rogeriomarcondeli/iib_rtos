/******************************************************************************
 * Copyright (C) 2017 by LNLS - Brazilian Synchrotron Light Laboratory
 *
 * Redistribution, modification or use of this software in source or binary
 * forms is permitted as long as the files maintain this copyright. LNLS and
 * the Brazilian Center for Research in Energy and Materials (CNPEM) are not
 * liable for any misuse of this material.
 *
 *****************************************************************************/

/**
 * @file fap_module.h
 * @brief Brief description of module
 * 
 * Detailed description
 *
 * @author allef.silva
 * @date 20 de out de 2018
 *
 */

/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_H_
#define FAP_H_

/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////////////////////

#define FAP_INPUT_OVERVOLTAGE_ITLK          0x00000001
#define FAP_OUTPUT_OVERVOLTAGE_ITLK         0x00000002
#define FAP_OUTPUT_OVERCURRENT_1_ITLK       0x00000004
#define FAP_OUTPUT_OVERCURRENT_2_ITLK       0x00000008
#define FAP_IGBT1_OVERTEMP_ITLK             0x00000010
#define FAP_IGBT2_OVERTEMP_ITLK             0x00000020
#define FAP_DRIVER_OVERVOLTAGE_ITLK         0x00000040
#define FAP_DRIVER1_OVERCURRENT_ITLK        0x00000080
#define FAP_DRIVER2_OVERCURRENT_ITLK        0x00000100
#define FAP_DRIVER1_ERROR_ITLK              0x00000200
#define FAP_DRIVER2_ERROR_ITLK              0x00000400
#define FAP_INDUC_OVERTEMP_ITLK             0x00000800
#define FAP_HS_OVERTEMP_ITLK                0x00001000
#define FAP_RELAY_ITLK                      0x00002000
#define FAP_RELAY_CONTACT_STICKING_ITLK     0x00004000
#define FAP_EXTERNAL_ITLK                   0x00008000
#define FAP_RACK_ITLK                       0x00010000
#define FAP_GROUND_LKG_ITLK                 0x00020000
#define FAP_BOARD_IIB_OVERTEMP_ITLK         0x00040000
#define FAP_BOARD_IIB_OVERHUMIDITY_ITLK     0x00080000

/////////////////////////////////////////////////////////////////////////////////////////////

#define FAP_INPUT_OVERVOLTAGE_ALM           0x00000001
#define FAP_OUTPUT_OVERVOLTAGE_ALM          0x00000002
#define FAP_OUTPUT_OVERCURRENT_1_ALM        0x00000004
#define FAP_OUTPUT_OVERCURRENT_2_ALM        0x00000008
#define FAP_IGBT1_OVERTEMP_ALM              0x00000010
#define FAP_IGBT2_OVERTEMP_ALM              0x00000020
#define FAP_DRIVER_OVERVOLTAGE_ALM          0x00000040
#define FAP_DRIVER1_OVERCURRENT_ALM         0x00000080
#define FAP_DRIVER2_OVERCURRENT_ALM         0x00000100
#define FAP_INDUC_OVERTEMP_ALM              0x00000200
#define FAP_HS_OVERTEMP_ALM                 0x00000400
#define FAP_GROUND_LKG_ALM                  0x00000800
#define FAP_BOARD_IIB_OVERTEMP_ALM          0x00001000
#define FAP_BOARD_IIB_OVERHUMIDITY_ALM      0x00002000

/////////////////////////////////////////////////////////////////////////////////////////////

/**
 * TODO: Put here your functions prototypes. Just what need 
 * to be accessed by other modules.
 */

/////////////////////////////////////////////////////////////////////////////////////////////

extern void init_fap(void);
extern void clear_fap_interlocks(void);
extern uint8_t check_fap_interlocks(void);
extern void clear_fap_alarms(void);
extern uint8_t check_fap_alarms(void);
extern void check_fap_indication_leds(void);
extern void fap_application_readings(void);
extern void send_fap_itlk_msg(void);
extern void fap_power_on_check(void);
extern void send_fap_data(void);

/////////////////////////////////////////////////////////////////////////////////////////////

#endif /* FAP_H_ */

/////////////////////////////////////////////////////////////////////////////////////////////


