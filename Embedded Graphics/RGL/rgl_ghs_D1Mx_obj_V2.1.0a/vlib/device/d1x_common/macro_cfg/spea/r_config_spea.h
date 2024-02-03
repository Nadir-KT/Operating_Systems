/*
****************************************************************************
PROJECT : VLIB macro device level config
============================================================================ 
DESCRIPTION
SPEA configuration for r7f701412
============================================================================
                            C O P Y R I G H T
============================================================================
                       Copyright (c) 2013 - 2019
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

#ifndef R_CONFIG_SPEA_H
#define R_CONFIG_SPEA_H

#if defined (R_DBG_PRINT_SPEA_REGS) || defined (R_DBG_PRINT_MSG)
#include<stdio.h>
#include "r_dbg_api.h"
#endif

/******************************************************************************
  Title: SPEA device configuration

  Configuration of the device dependent part of the SPEA macro
*/

/*******************************************************************************
  Section: Global Defines 
*/

/*******************************************************************************
  Constant: R_SPEA_MACRO_NUM

  Number of macros in the device.
*/
#define R_SPEA_MACRO_NUM         (1u)
#define R_SPEA_D1M1A_MACRO_NUM   (4u)

/***************************************************************************
 Constants: Unit Number

 The definition of the unit / engine number.
*/
#define R_SPEA_UNIT0         (0u)
#define R_SPEA_RLE_UNIT0     (0u)
#define R_SPEA_RLE_UNIT1     (1u)
#define R_SPEA_RLE_UNIT2     (2u)
#define R_SPEA_RLE_UNIT3     (3u)
#define R_SPEA_RLE_ENG0      (0u)
#define R_SPEA_RLE_ENG1      (1u)
#define R_SPEA_SPRITE_UNIT0  (0u)
#define R_SPEA_SPRITE_UNIT1  (1u)
#define R_SPEA_SPRITE_UNIT2  (2u)
#define R_SPEA_SPRITE_UNIT3  (3u)

/*******************************************************************************
  Constant:  R_SPEA_MAX_SPRITE

  Number of sprites per unit
*/
#define R_SPEA_MAX_SPRITE        (16u)

/*******************************************************************************
  Constant:  R_SPEA_MAX_SPRITE_UNIT

  Number of sprite units per macro
*/
#define R_SPEA_MAX_SPRITE_UNIT        (3u)
#define R_SPEA_D1M1A_MAX_SPRITE_UNIT  (4u)

/*******************************************************************************
  Constant:  R_SPEA_MAX_RLE_UNIT

  Number of RLE engine per RLE Unit
*/
#define R_SPEA_MAX_RLE_UNIT       (2u)
#define R_SPEA_D1M1A_MAX_RLE_UNIT (2u)

/*******************************************************************************
  Constant:  R_SPEA_VIRTUAL_AREA

  Width of a virtual layer in byte
*/
#define R_SPEA_VIRTUAL_AREA    (8192u)

/*******************************************************************************
  Constant:  R_SPEA_MAX_*

  Maximum Posx/PosY/Width/Height of sprite data in byte
*/
#define R_SPEA_MAX_POS_X       (8184u)
#define R_SPEA_MAX_POS_Y       (8191u)
#define R_SPEA_MAX_WIDTH       (8184u)
#define R_SPEA_MAX_HEIGHT      (2047u)

/*******************************************************************************
  Constant:  R_SPEA_BUS_WIDTH_BITS

  Width of the connected bus in bits
*/
#define R_SPEA_BUS_WIDTH_BITS   (64u)

/*******************************************************************************
  Constant: R_SPEA_BASE

   Base address for the macro registers 
*/
#define R_SPEA_BASE             (0xFFFD4000u)

/***********************************************************
  Macro: R_SPEA_WriteReg

  Write SPEAC register with predefined size.
  SIZE allowed values are 8, 16, 32
*/
#ifdef R_DBG_PRINT_SPEA_REGS
    /* Assign type according to the size of register */
    #define R_SPEA_WriteReg(SIZE, ADDR, VALUE) {R_DBG_PrintRegCmd((SIZE), (ADDR), (VALUE));\
                                                *((volatile uint##SIZE##_t*)(ADDR)) = (VALUE);}
#else
    /* Assign type according to the size of register */
    #define R_SPEA_WriteReg(SIZE, ADDR, VALUE) { *((volatile uint##SIZE##_t*)(ADDR)) = (VALUE);}
#endif

/***********************************************************
  Macro: R_SPEA_ReadReg

  Read SPEAC register with predefined size.
  SIZE allowed values are 8, 16, 32
*/
#ifdef R_DBG_PRINT_SPEA_REGS
    #define R_SPEA_ReadReg(SIZE, ADDR) (R_DBG_PrintRegRead##SIZE##(ADDR))
#else
    /* Assign type according to the size of register */
    #define R_SPEA_ReadReg(SIZE, ADDR)  (*((volatile uint##SIZE##_t*)((ADDR))))
#endif


/***********************************************************
  Constant: R_SPEA_STATIC
  
  Definition for reserved word "static".
*/

#ifndef R_SPEA_BUILD_UNIT_TEST
 #define R_SPEA_STATIC static
#else
 #define R_SPEA_STATIC
#endif


#endif /* R_CONFIG_SPEA_H */
