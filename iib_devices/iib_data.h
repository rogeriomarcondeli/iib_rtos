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
 * @file iib_data.h
 * @brief Brief description of module
 * 
 * Detailed description
 *
 * @author allef.silva
 * @date 28 de ago de 2018
 *
 */

/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef IIB_DATA_H_
#define IIB_DATA_H_

/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#define NUM_MAX_IIB_SIGNALS     32

/////////////////////////////////////////////////////////////////////////////////////////////

typedef volatile struct
{
    union
    {
        volatile uint32_t   u32;
        volatile uint8_t    u8[4];
        volatile float      f;
    } iib_signals[NUM_MAX_IIB_SIGNALS];

    union
    {
        volatile uint32_t   u32;
        volatile uint8_t    u8[4];
        volatile float      f;
    } iib_itlk_lim[NUM_MAX_IIB_SIGNALS];

    union
    {
        volatile uint32_t   u32;
        volatile uint8_t    u8[4];
        volatile float      f;
    } iib_alm_lim[NUM_MAX_IIB_SIGNALS];

} control_framwork_t;

/////////////////////////////////////////////////////////////////////////////////////////////

extern volatile control_framwork_t g_controller_iib;

extern void init_control_framwork(volatile control_framwork_t *p_controller);

/////////////////////////////////////////////////////////////////////////////////////////////

/**
 * TODO: Put here your defines. Just what need 
 * to be accessed by other modules.
 */

/**
 * TODO: Put here your functions prototypes. Just what need 
 * to be accessed by other modules.
 */

/////////////////////////////////////////////////////////////////////////////////////////////

#endif /* IIB_DATA_H_ */

/////////////////////////////////////////////////////////////////////////////////////////////



