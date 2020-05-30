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
 * @file rectifier_module.h
 * @brief Brief description of module
 * 
 * Detailed description
 *
 * @author allef.silva
 * @date 20 de out de 2018
 *
 */

#ifndef RECTIFIER_MODULE_H_
#define RECTIFIER_MODULE_H_

#include<stdint.h>

#define RM_OUTPUT_OVERCURRENT_RECT1_ITLK       0x00000001
#define RM_OUTPUT_OVERCURRENT_RECT2_ITLK       0x00000002
#define RM_OUTPUT_OVERVOLTAGE_RECT1_ITLK       0x00000004
#define RM_OUTPUT_OVERVOLTAGE_RECT2_ITLK       0x00000008
#define RM_LEAKAGE_OVERCURRENT_ITLK            0x00000010
#define RM_HS_OVERTEMP_ITLK                    0x00000020
#define RM_WATER_OVERTEMP_ITLK                 0x00000040
#define RM_MODULE1_OVERTEMP_ITLK               0x00000080
#define RM_MODULE2_OVERTEMP_ITLK               0x00000100
#define RM_INDUCTOR1_OVERTEMP_ITLK             0x00000200
#define RM_INDUCTOR2_OVERTEMP_ITLK             0x00000400
#define RM_PHASE_FAULT_ITLK                    0x00000800
#define RM_AC_OVERCURRENT_ITLK                 0x00001000
#define RM_AC_TRANSF_OVERTEMP_ITLK             0x00002000
#define RM_WATER_FLOW_ITLK                     0x00004000

#define RM_OUTPUT_OVERCURRENT_RECT1_ALM        0x00000001
#define RM_OUTPUT_OVERCURRENT_RECT2_ALM        0x00000002
#define RM_OUTPUT_OVERVOLTAGE_RECT1_ALM        0x00000004
#define RM_OUTPUT_OVERVOLTAGE_RECT2_ALM        0x00000008
#define RM_LEAKAGE_OVERCURRENT_ALM             0x00000010
#define RM_HS_OVERTEMP_ALM                     0x00000020
#define RM_WATER_OVERTEMP_ALM                  0x00000040
#define RM_MODULE1_OVERTEMP_ALM                0x00000080
#define RM_MODULE2_OVERTEMP_ALM                0x00000100
#define RM_INDUCTOR1_OVERTEMP_ALM              0x00000200
#define RM_INDUCTOR2_OVERTEMP_ALM              0x00000400

extern void init_rectifier_module(void);
extern void clear_rectifier_interlocks(void);
extern uint8_t check_rectifier_interlocks(void);
extern void clear_rectifier_alarms(void);
extern uint8_t check_rectifier_alarms(void);
extern void check_rectifier_indication_leds(void);
extern void rectifier_application_readings(void);
extern void rectifier_map_vars(void);
extern void send_rectf_itlk_msg(void);
extern void send_rm_data(void);

#endif /* RECTIFIER_MODULE_H_ */
