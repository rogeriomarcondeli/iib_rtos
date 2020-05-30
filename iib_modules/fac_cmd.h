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
 * @file command_drawer.h
 * @brief Brief description of module
 * 
 * Detailed description
 *
 * @author allef.silva
 * @date 20 de out de 2018
 *
 */

#ifndef FAC_CMD_H_
#define FAC_CMD_H_

#include <stdint.h>

#define FAC_CMD_CAPBANK_OVERVOLTAGE_ITLK    0x00000001
#define FAC_CMD_OUTPUT_OVERVOLTAGE_ITLK     0x00000002
#define FAC_CMD_HS_OVERTEMP_ITLK            0x00000004
#define FAC_CMD_INDUC_OVERTEMP_ITLK         0x00000008
#define FAC_CMD_EXTERNAL1_ITLK              0x00000010
#define FAC_CMD_EXTERNAL2_ITLK              0x00000020
#define FAC_CMD_GROUND_LKG_ITLK             0x00000040

#define FAC_CMD_CAPBANK_OVERVOLTAGE_ALM     0x00000001
#define FAC_CMD_OUTPUT_OVERVOLTAGE_ALM      0x00000002
#define FAC_CMD_HS_OVERTEMP_ALM             0x00000004
#define FAC_CMD_INDUC_OVERTEMP_ALM          0x00000008
#define FAC_CMD_GROUND_LKG_ALM              0x00000010

extern void init_fac_cmd(void);
extern void clear_fac_cmd_interlocks(void);
extern uint8_t check_fac_cmd_interlocks(void);
extern void clear_fac_cmd_alarms(void);
extern uint8_t check_fac_cmd_alarms(void);
extern void check_fac_cmd_indication_leds(void);
extern void fac_cmd_application_readings(void);
extern void send_fac_cmd_itlk_msg(void);
extern void send_fac_cmd_data(void);
extern void fac_cmd_power_on_check();

#endif /* COMMAND_DRAWER_H_ */
