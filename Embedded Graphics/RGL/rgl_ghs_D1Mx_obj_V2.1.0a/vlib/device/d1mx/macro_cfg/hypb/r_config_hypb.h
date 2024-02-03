/*
****************************************************************************
PROJECT : HYPB driver
FILE    : $Id: r_config_hypb.h 12704 2017-02-20 10:24:03Z shinya.tomari $
============================================================================
DESCRIPTION
HYPB configuration for d1x
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2016
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

/***************************************************************************
  Title: HYPB Driver configuration

  Configuration of the device dependent part of the HYPB macro
*/

#ifndef R_CONFIG_HYPB_H
#define R_CONFIG_HYPB_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
  Section: Includes
*/

#if defined (R_DBG_PRINT_HYPB_REGS) 
   #include <stdio.h> 
   #include "r_dbg_api.h"
#endif
#include "r_dev_api.h"

/***************************************************************************
  Section: Global Defines
*/

/***************************************************************************
  Constant: R_HYPB_BASE_ADD

  Base address for the macro registers.
*/
#define R_HYPB_BASE_ADD             (0xF2FF8000uL)

/***************************************************************************
  Constant: R_HYPB_MACRO_NUM

  Number of macros in the device.
*/
#define R_HYPB_MACRO_NUM            (1u)

/***************************************************************************
  Constant: R_HYPB_STATUS_TIMEOUT

  Define the time-out count of the status register check.
  According to IS26KL512S data sheet, the maximum value to sector erase is 
  2.9 seconds. 
  
*/
#define R_HYPB_STATUS_TIMEOUT       (3000000uL)

/*******************************************************************************
  Section: Global Macros
*/

/***************************************************************************
  Macro: R_HYPB_WRITE32

  Write HYPB register by 32 bit.
*/

#define R_HYPB_WRITE32(ADDR, VALUE)  {R_DEV_WRITE_REG(32, (ADDR), (VALUE));}

/***************************************************************************
  Macro: R_HYPB_WRITE64

  Write HYPB register by 64 bit.
*/

#define R_HYPB_WRITE64(ADDR, VALUE)  {R_DEV_WRITE_REG(64, (ADDR), (VALUE));}

/***************************************************************************
  Macro: R_HYPB_READ32

  Read HYPB register by 32 bit.
*/

#define R_HYPB_READ32(ADDR)          (R_DEV_READ_REG(32, (ADDR)))


#ifdef __cplusplus
}
#endif

#endif /* R_CONFIG_HYPB_H */


