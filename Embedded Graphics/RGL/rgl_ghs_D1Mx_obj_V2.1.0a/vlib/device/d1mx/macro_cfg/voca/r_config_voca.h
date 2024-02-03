/*
****************************************************************************
PROJECT : VOCA driver
============================================================================
DESCRIPTION
VOCA system functions
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
  Title: VOCA system functions

  VOCA driver system functions

******************************************************************************/

#ifndef R_CONFIG_VOCA_H
#define R_CONFIG_VOCA_H

#if defined (R_DBG_PRINT_VOCA_REGS) || defined (R_DBG_PRINT_MSG)
#include<stdio.h>
#include "r_dbg_api.h"
#endif

/*******************************************************************************
  Section: Global Defines
*/

/*******************************************************************************
    Constant: R_VOCA_MACRO_NUM

    Number of VOCA macros in the device.
*/
#define R_VOCA_MACRO_NUM                (1u)

/*******************************************************************************
    Constant: R_VOCA_UNIT

    The definition for the unit number.
*/
#define R_VOCA_UNIT0                    (0u)

/*******************************************************************************
    Constant: R_VOCA_BASE_ADDR

    Base address of the VOCA macro Units in the device.
*/
#define R_VOCA_BASE_ADDR                (0xFFFD8000u)

/*******************************************************************************
  Constants: Video channels
  
  The maximum number of available Video channels.

  R_VOCA_MAX_VO_CH_NUM      - For D1M1(H), D1M1-V2.
  R_VOCA_MAX_VO_CH_NUM2     - For D1M1A, D1M2(H).
*/
#define R_VOCA_MAX_VO_CH_NUM            (1u)
#define R_VOCA_MAX_VO_CH_NUM2           (2u)

/*******************************************************************************
  Constants: R_VOCA_MAX_MONITOR_AREA_NUM
  
  The maximum number of available monitor areas.
*/
#define R_VOCA_MAX_MONITOR_AREA_NUM     (16u)

/*******************************************************************************
    Constant: R_VOCA_HOFFSET

    The maximum with horizontal back porch offset of Video channel Display area.
*/
#define R_VOCA_HOFFSET                  (2047u)

/*******************************************************************************
  Constant: R_VOCA_VOFFSET

   The maximum with Vertical back porch offset of Video channel Display area.
*/
#define R_VOCA_VOFFSET                  (2047u)

/*******************************************************************************
    Constant: R_VOCA_HSIZE

    The maximum with horizontal size of Video channel Display area.
*/
#define R_VOCA_HSIZE                    (1280u)

/*******************************************************************************
  Constant: R_VOCA_VSIZE

   The maximum with Vertical size of Video channel Display area.
*/
#define R_VOCA_VSIZE                    (1024u)

/*******************************************************************************
    Constant: R_VOCA_MONAREA_HSIZE

    The maximum with horizontal size of Video Output Monitor area.
*/
#define R_VOCA_MONAREA_HSIZE            (128u)

/*******************************************************************************
  Constant: R_VOCA_MONAREA_VSIZE

   The maximum with Vertical size of Video Output Monitor area.
*/
#define R_VOCA_MONAREA_VSIZE            (128u)

/*******************************************************************************
  Macro: R_VOCA_Write32

  Write VOCA register 32 bit.
*/

#ifdef R_DBG_PRINT_VOCA_REGS
    #define R_VOCA_Write32(ADDR, VALUE)  {R_DBG_PrintRegCmd(32, (ADDR), (VALUE)); \
                                          *((volatile uint32_t*)(ADDR)) = (VALUE);}
#else
    #define R_VOCA_Write32(ADDR, VALUE)  {(*((volatile uint32_t*)(ADDR)) = (VALUE));}
#endif

/*******************************************************************************
  Macro: R_VOCA_Read32

  Read VOCA register 32 bit.
*/

#ifdef R_DBG_PRINT_VOCA_REGS
    #define R_VOCA_Read32(ADDR)   (R_DBG_PrintRegRead32((ADDR)))
#else
    #define R_VOCA_Read32(ADDR)   ((*((volatile uint32_t*)((ADDR)))))
#endif

/***********************************************************
  Constant: R_VOCA_STATIC
  
  Definition for reserved word "static".
*/

#ifndef R_VOCA_BUILD_UNIT_TEST
  #define R_VOCA_STATIC static
#else
  #define R_VOCA_STATIC
#endif

#endif /* R_CONFIG_VOCA_H */
