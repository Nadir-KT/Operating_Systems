/*
****************************************************************************
PROJECT : VLIB macro device level config
============================================================================ 
DESCRIPTION
ECM configuration for RH850/D1x
============================================================================
                            C O P Y R I G H T                            
============================================================================
                           Copyright (c) 2012 - 2019
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

********************************************************************************
*/

#ifndef R_CONFIG_ECM_H_
#define R_CONFIG_ECM_H_

#if defined (R_DBG_PRINT_DISCOM_REGS) || defined (R_DBG_PRINT_MSG)
#include<stdio.h>
#include "r_dbg_api.h"
#endif

/*******************************************************************************
  Title: ECM device configuration
  
  Configuration of the device dependanant part of the ECM macro 
*/

/*******************************************************************************
  Section: Global Defines
*/

/*******************************************************************************
  Constant: R_ECM_MACRO_NUM

  Number of macros in the device.
*/

#define R_ECM_MACRO_NUM         (1u)

/*******************************************************************************
  Constant: R_ECM_UNIT0

  Unit index number.
*/

#define R_ECM_UNIT0             (0u)


/*******************************************************************************
  Constant: R_ECM_BASE

   Base address for the macro registers 
*/

#define R_ECM_BASE              (0xffcb0000u)



/***********************************************************
  Macro: R_ECM_WRITE_REG

  Write ECMC register with predefined size.
  SIZE allowed values are 8, 16, 32
*/

#ifdef R_DBG_PRINT_ECM_REGS
    #define R_ECM_WRITE_REG(SIZE, ADDR, VALUE) {\
        R_DBG_PrintRegCmd((SIZE), (ADDR), (VALUE));\
        *((volatile uint##SIZE##_t*)(ADDR)) = (VALUE);\
    }
#else
    #define R_ECM_WRITE_REG(SIZE, ADDR, VALUE) {\
        *((volatile uint##SIZE##_t*)(ADDR)) = (VALUE);\
    }
#endif


/***********************************************************
  Macro: R_ECM_READ_REG

  Read ECMC register with predefined size.
  SIZE allowed values are 8, 16, 32
*/

#ifdef R_DBG_PRINT_ECM_REGS
    #define R_ECM_READ_REG(SIZE, ADDR) (R_DBG_PrintRegRead##SIZE##(ADDR))
#else
    #define R_ECM_READ_REG(SIZE, ADDR)  (*((volatile uint##SIZE##_t*)((ADDR)))) 
#endif


/***********************************************************
  Macro: R_ECM_WRITE_PROT_REG

  Write protected ECMC register with 32 Bit size.
*/

#ifdef R_DBG_PRINT_ECM_REGS
#define R_ECM_WRITE_PROT_REG(PROT_ADDR, ADDR, VALUE) { \
  *((volatile uint32_t*)(PROT_ADDR)) = (0x000000a5);\
  *((volatile uint32_t*)(ADDR))      = (VALUE);\
  *((volatile uint32_t*)(ADDR))      = (uint32_t)(~(VALUE));\
  *((volatile uint32_t*)(ADDR))      = (VALUE);\
R_DBG_PrintRegCmd(32, (ADDR), (VALUE));}
#else
#define R_ECM_WRITE_PROT_REG(PROT_ADDR, ADDR, VALUE) { \
  *((volatile uint32_t*)(PROT_ADDR)) = (0x000000a5);\
  *((volatile uint32_t*)(ADDR))      = (VALUE);\
  *((volatile uint32_t*)(ADDR))      = (uint32_t)(~(VALUE));\
  *((volatile uint32_t*)(ADDR))      = (VALUE);\
}
#endif

#endif /* R_CONFIG_ECM_H_ */

