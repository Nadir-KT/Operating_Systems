/*
****************************************************************************
PROJECT : JCUA driver
============================================================================
DESCRIPTION
JCUA Configuration for d1x
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

#ifndef R_CONFIG_JCUA_H
#define R_CONFIG_JCUA_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
  Title: JCUA Driver configuration
  
  Configuration of the device dependent part of the JCUA macro
*/

/***************************************************************************
  Section: Global defines
*/

/***************************************************************************
  Constant: R_JCUA_MACRO_NUM 

  Number of macros in the device.
*/

#define    R_JCUA_MACRO_NUM  (1u)

/***************************************************************************
  Constant: R_JCUA_UNIT0

  Unit number.
*/

#define    R_JCUA_UNIT0      (0u)

/***************************************************************************
  Constant: R_JCUA_STRIDE_UNIT_SIZE

  Define a JCUA stride unit size. The value which can be set is 128 or 32.

  Note: The set value is based on the setting of VDCE driver. In D1x, setting is 128.
*/

#define    R_JCUA_STRIDE_UNIT_SIZE  (128u)

#if (R_JCUA_STRIDE_UNIT_SIZE == 128u)
#define R_JCUA_STRIDE_MASK      (0x00007F80u)
#define R_JCUA_STRIDE_ANDMASK   (0x0000007Fu)
#else
#define R_JCUA_STRIDE_MASK      (0x00007FE0u)
#define R_JCUA_STRIDE_ANDMASK   (0x0000001Fu)
#endif

/****************************************************************************
  Constant: R_JCUA_BASE_ADD

  Base address for the macro registers.
*/
#define R_JCUA_BASE_ADD             (0xF200C000u)

/****************************************************************************
  Constant: R_JCUA_Write32

  Write JCUA register 32bit.
*/
#define R_JCUA_Write32(ADDR, VALUE)  (*((volatile uint32_t*)(ADDR)) = (VALUE))

/****************************************************************************
  Constant: R_JCUA_Write8

  Write JCUA register 8bit.
*/
#define R_JCUA_Write8(ADDR, VALUE)   (*((volatile uint8_t*)(ADDR)) = ((uint8_t)(VALUE)))

/****************************************************************************
  Constant: R_JCUA_Read32

  Read JCUA register 32bit.
*/
#define R_JCUA_Read32(ADDR)          (*((volatile uint32_t*)((ADDR))))

/****************************************************************************
  Constant: R_JCUA_Read8

  Read JCUA register 8bit.
*/
#define R_JCUA_Read8(ADDR)           (*((volatile uint8_t*)((ADDR))))

/***************************************************************************
  Constants: R_JCUA_STATIC
  
  This constant defines the keyword 'static'
*/

#ifndef R_JCUA_BUILD_UNIT_TEST
  #define R_JCUA_STATIC              static
#else
  #define R_JCUA_STATIC
#endif

#ifdef __cplusplus
}
#endif

#endif /* R_CONFIG_JCUA_H */

