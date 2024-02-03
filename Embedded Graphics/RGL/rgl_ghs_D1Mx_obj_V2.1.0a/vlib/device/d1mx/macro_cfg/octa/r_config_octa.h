/*
****************************************************************************
PROJECT : OCTA driver
FILE    : $Id: r_config_octa.h 13009 2017-03-08 04:57:47Z shinobu.uehara $
============================================================================
DESCRIPTION
OCTA configuration for d1x
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2017
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
  Title: OCTA Driver configuration

  Configuration of the device dependent part of the OCTA macro
*/

#ifndef R_CONFIG_OCTA_H
#define R_CONFIG_OCTA_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
  Section: Includes
*/

#if defined (R_DBG_PRINT_OCTA_REGS) 
   #include <stdio.h> 
   #include "r_dbg_api.h"
#endif
#include "r_dev_api.h"

/***************************************************************************
  Section: Global Defines
*/

/***************************************************************************
  Constant: R_OCTA_BASE_ADD

  Base address for the macro registers.
*/
#define R_OCTA_BASE_ADD             (0xF2FF8000uL)

/***************************************************************************
  Constant: R_OCTA_MACRO_NUM

  Number of macros in the device.
*/
#define R_OCTA_MACRO_NUM            (1u)

/***************************************************************************
  Constant: R_OCTA_CONNECTED_DEVICE_NUM

  Number of connected the device for Octa RAM/flash memory.
*/
#define R_OCTA_CONNECTED_DEVICE_NUM (2u)

/***************************************************************************
  Constant: R_OCTA_STATUS_TIMEOUT

  Define the time-out count of the status register check.
  According to MX25LW51245G data sheet, the maximum value to sector erase is 
  480 milliseconds. 
  
*/
#define R_OCTA_STATUS_TIMEOUT       (480000uL)

/*******************************************************************************
  Section: Global Macros
*/

/***************************************************************************
  Macro: R_OCTA_WRITE32

  Write OCTA register by 32 bit.
*/

#define R_OCTA_WRITE32(ADDR, VALUE)  {R_DEV_WRITE_REG(32, (ADDR), (VALUE));}

/***************************************************************************
  Macro: R_OCTA_WRITE16

  Write OCTA register by 16 bit.
*/

#define R_OCTA_WRITE16(ADDR, VALUE)  {R_DEV_WRITE_REG(16, (ADDR), (VALUE));}

/***************************************************************************
  Macro: R_OCTA_WRITE8

  Write OCTA register by 32 bit.
*/

#define R_OCTA_WRITE8(ADDR, VALUE)  {R_DEV_WRITE_REG(8, (ADDR), (VALUE));}

/***************************************************************************
  Macro: R_OCTA_READ32

  Read OCTA register by 32 bit.
*/

#define R_OCTA_READ32(ADDR)          (R_DEV_READ_REG(32, (ADDR)))


#ifdef __cplusplus
}
#endif

#endif /* R_CONFIG_OCTA_H */


