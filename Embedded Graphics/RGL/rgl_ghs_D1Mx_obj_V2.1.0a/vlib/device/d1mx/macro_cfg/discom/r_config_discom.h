/*
****************************************************************************
PROJECT : DISCOM driver
============================================================================
DESCRIPTION
DISCOM system functions
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2019
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

/******************************************************************************
  Title: DISCOM system functions

  DISCOM driver system functions

******************************************************************************/

#ifndef R_CONFIG_DISCOM_H
#define R_CONFIG_DISCOM_H

#if defined (R_DBG_PRINT_DISCOM_REGS) || defined (R_DBG_PRINT_MSG)
#include<stdio.h>
#include "r_dbg_api.h"
#endif

/*******************************************************************************
  Section: Global Defines
*/

/*******************************************************************************
  Constant: R_DISCOM_MACRO_NUM

  Number of macros in the device.
*/
#define R_DISCOM_MACRO_NUM         (4u)

/*******************************************************************************
  Constant: R_DISCOM_UNIT

  The definition for the unit number.
*/
#define R_DISCOM_UNIT0             (0u)
#define R_DISCOM_UNIT1             (1u)
#define R_DISCOM_UNIT2             (2u)
#define R_DISCOM_UNIT3             (3u)

/*******************************************************************************
  Constant: R_DISCOM_BASE

   Base address for the macro registers 
*/
#define R_DISCOM_BASE_ADDR0        (0xF2009000u)
#define R_DISCOM_BASE_ADDR1        (0xF2009400u)
#define R_DISCOM_BASE_ADDR2        (0xF2009800u)
#define R_DISCOM_BASE_ADDR3        (0xF2009C00u)

/*******************************************************************************
  Constant: R_DISCOM_HOFFSET

   The maximum with horizontal start position.
*/
#define R_DISCOM_HOFFSET           (1279u)

/*******************************************************************************
  Constant: R_DISCOM_VOFFSET

   The maximum with Vertical start position.
*/
#define R_DISCOM_VOFFSET           (1023u)

/*******************************************************************************
  Constant: R_DISCOM_HSIZE

   The maximum with horizontal size.
*/
#define R_DISCOM_HSIZE             (1280u)

/*******************************************************************************
  Constant: R_DISCOM_VSIZE

   The maximum with Vertical size.
*/
#define R_DISCOM_VSIZE             (1024u)

/*******************************************************************************
  Macro: R_DISCOM_Write32

  Write DISCOM register 32 bit.
*/

#ifdef R_DBG_PRINT_DISCOM_REGS
    #define R_DISCOM_Write32(ADDR, VALUE)  {R_DBG_PrintRegCmd(32, (ADDR), (VALUE)); \
                                            *((volatile uint32_t*)(ADDR)) = (VALUE);}
#else
    #define R_DISCOM_Write32(ADDR, VALUE)  {(*((volatile uint32_t*)(ADDR)) = (VALUE));}
#endif

/*******************************************************************************
  Macro: R_DISCOM_Read32

  Read DISCOM register 32 bit.
*/

#ifdef R_DBG_PRINT_DISCOM_REGS
    #define R_DISCOM_Read32(ADDR)   (R_DBG_PrintRegRead32((ADDR)))
#else
    #define R_DISCOM_Read32(ADDR)   ((*((volatile uint32_t*)((ADDR)))))
#endif

/*******************************************************************************
  Constant: R_DISCOM_STATIC
  
  Definition for reserved word "static".
*/

#ifndef R_DISCOM_BUILD_UNIT_TEST
  #define R_DISCOM_STATIC static
#else
  #define R_DISCOM_STATIC
#endif

#endif /* R_CONFIG_DISCOM_H */

