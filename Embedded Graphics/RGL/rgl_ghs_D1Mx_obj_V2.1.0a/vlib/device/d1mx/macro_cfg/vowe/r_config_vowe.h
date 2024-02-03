/*
****************************************************************************
PROJECT : VOWE driver
============================================================================
DESCRIPTION
VOWE(Video Output Warping Engine) configuration for D1Mx
============================================================================
                             C O P Y R I G H T
============================================================================
                            Copyright (c) 2015 - 2019
                                   by
                        Renesas Electronics (Europe) GmbH.
                            Arcadiastrasse 10
                           D-40472 Duesseldorf
                                Germany
                           All rights reserved.
============================================================================
Purpose: only for testing, not for mass production

DISCLAIMER

LICENSEE has read, understood and accepted the terms and conditions defined in
the license agreement, especially the usage rights. In any case, it is
LICENSEE's responsibility to make sure that any user of the software complies
with the terms and conditions of the signed license agreement.

SAMPLE CODE is not part of the licensed software, as such it must not be used in
mass-production applications. It can only be used for evaluation and
demonstration purposes at customer's premises listed in the signed license
agreement.
****************************************************************************
*/

#ifndef R_CONFIG_VOWE_H_
#define R_CONFIG_VOWE_H_

#if defined (R_DBG_PRINT_VOWE_REGS) 
#include <stdio.h> 
#include "r_dbg_api.h"
#endif

/***************************************************************************
  Title: VOWE Configuration for D1Mx
  
  VOWE Configuration
*/

/***************************************************************************
  Section: Global Defines
*/


/***************************************************************************
    Constant: R_VOWE_MACRO_NUM

    Number of VOWE macros in the device.
*/
#define R_VOWE_MACRO_NUM        (0x00000001u)

/***************************************************************************
    Constant: R_VOWE_UNIT0

    VOWE unit number
*/
#define R_VOWE_UNIT0            (0u)


/***************************************************************************
    Constant: R_VOWE_BASE_ADDR

    Base address and extra base address of the VOWE device.
*/
#define R_VOWE_BASE_ADDR        (0xF2000000u)
#define R_VOWE_EXT_BASE_ADDR    (0xFFC06040u)


/***************************************************************************
    Constant: R_VOWE_MAX_WIDTH, R_VOWE_MAX_HEIGHT

    The maximum with source image size.
*/
#define R_VOWE_MAX_WIDTH   (1280u)
#define R_VOWE_MAX_HEIGHT  (1024u)


/******************************************************************************
 Constant: R_VOWE_STRIDE_UNIT_SIZE 

 Define a VOWE stride unit size. The value which can be set is 128 or 32.
*/
/* VOWE stride unit size */
#define R_VOWE_STRIDE_UNIT_SIZE  (128uL)

/***************************************************************************
    Constant: R_VOWE_ADJUST_DISPLAY_TIMING


*/
#define R_VOWE_ADJUST_DISPLAY_TIMING    (8u)

/***************************************************************************
  Macro: R_VDCE_WRITE32

  Write VOWE register 32 bit.
*/

#ifdef R_DBG_PRINT_VOWE_REGS
#define R_VOWE_Write32(ADDR, VALUE)  {R_DBG_PrintRegCmd(32, (ADDR), (VALUE));  \
                                      *((volatile uint32_t*)(ADDR)) = (VALUE);  \
                                     }
#else
#define R_VOWE_Write32(ADDR, VALUE)  {(*((volatile uint32_t*)(ADDR)) = (VALUE)); }
#endif

/***************************************************************************
  Macro: R_VOWE_READ32

  Read VOWE register 32 bit.
*/
#ifdef R_DBG_PRINT_VOWE_REGS
#define R_VOWE_Read32(ADDR)   (R_DBG_PrintRegRead32((ADDR)))
#else
#define R_VOWE_Read32(ADDR)   (*((volatile uint32_t*)(ADDR)))
#endif

/*******************************************************************************
  Constant:  R_VOWE_STATIC

  Macro for 'static' storage class.
*/
#ifndef R_VOWE_BUILD_UNIT_TEST
  #define R_VOWE_STATIC       static
#else
  #define R_VOWE_STATIC
#endif


#endif /* R_CONFIG_VOWE_H_ */


