/*
****************************************************************************
PROJECT : VLIB Macro Driver 
============================================================================ 
DESCRIPTION
Generic ECM driver
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

****************************************************************************
*/


#ifndef R_ECM_MAIN_H_
#define R_ECM_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Title: Main Module Internal Interface

  Only the driver modules shall include this header.
*/

/*******************************************************************************
  Section: Defines
*/
/*******************************************************************************
  Constant: R_ECM_BASE_OFFSET

   Base address offsets for the ECM register groups
*/

#define R_ECM_BASE_OFFSET_MASTER    0x0000u /* Offset of ECM Master Registers Address List */
#define R_ECM_BASE_OFFSET_CHECKER   0x1000u /* Offset of ECM Checker Registers Address List */
#define R_ECM_BASE_OFFSET_COMMON    0x2000u /* Offset of ECM Common Registers Address List */


/*******************************************************************************
  Constant: R_ECM_REG_

   ECM Register Offsets 
*/

/* ECM Master Registers */
#define R_ECM_REG_MESET       0x00u     /* ECM master error set trigger register */
#define R_ECM_REG_MECLR       0x04u     /* ECM master error clear trigger register */
#define R_ECM_REG_MESSTR0     0x08u     /* ECM master error source status register 0 */
#define R_ECM_REG_MESSTR1     0x0Cu     /* ECM master error source status register 1 */
#define R_ECM_REG_MPCMD0      0x10u     /* ECM master protection command register */

/* ECM Checker Registers */
#define R_ECM_REG_CESET       0x00u     /* ECM checker error set trigger register */
#define R_ECM_REG_CECLR       0x04u     /* ECM checker error clear trigger register */
#define R_ECM_REG_CESSTR0     0x08u     /* ECM checker error source status register 0 */
#define R_ECM_REG_CESSTR1     0x0Cu     /* ECM checker error source status register 1 */
#define R_ECM_REG_CPCMD0      0x10u     /* ECM checker protection command register */

/* ECM Common Registers */
#define R_ECM_REG_EPCFG       0x00u     /* ECM error pulse configuration register */
#define R_ECM_REG_MICFG0      0x04u     /* ECM maskable interrupt configuration register 0 */
#define R_ECM_REG_MICFG1      0x08u     /* ECM maskable interrupt configuration register 1 */
#define R_ECM_REG_NMICFG0     0x0Cu     /* ECM FE level non-maskable interrupt configuration register 0 */
#define R_ECM_REG_NMICFG1     0x10u     /* ECM FE level non-maskable interrupt configuration register 1 */
#define R_ECM_REG_IRCFG0      0x14u     /* ECM internal reset configuration register 0 */
#define R_ECM_REG_IRCFG1      0x18u     /* ECM internal reset configuration register 1 */
#define R_ECM_REG_EMK0        0x1Cu     /* ECM error mask register 0 */
#define R_ECM_REG_EMK1        0x20u     /* ECM error mask register 1 */
#define R_ECM_REG_ESSTC0      0x24u     /* ECM error source status clear register 0 */
#define R_ECM_REG_ESSTC1      0x28u     /* ECM error source status clear register 1 */
#define R_ECM_REG_PCMD1       0x2Cu     /* ECM protection command register */
#define R_ECM_REG_PS          0x30u     /* ECM protection status register */
#define R_ECM_REG_PE0         0x34u     /* ECM pseudo error trigger register 0 */
#define R_ECM_REG_PE1         0x38u     /* ECM pseudo error trigger register 1 */
#define R_ECM_REG_DTMCTL      0x3Cu     /* ECM delay timer control register */
#define R_ECM_REG_DTMR        0x40u     /* ECM delay timer register */
#define R_ECM_REG_DTMCMP      0x44u     /* ECM delay timer compare register */
#define R_ECM_REG_DTMCFG0     0x48u     /* ECM delay timer configuration register 0 */
#define R_ECM_REG_DTMCFG1     0x4Cu     /* ECM delay timer configuration register 1 */
#define R_ECM_REG_DTMCFG2     0x50u     /* ECM delay timer configuration register 2 */
#define R_ECM_REG_DTMCFG3     0x54u     /* ECM delay timer configuration register 3 */

/* ECM error pulse control register */
#define R_ECM_REG_EPCTL       0xFFC0600Cu

/* This is the reset controller's software reset register */
#define R_RESET_SWRESA        0xfff80a04uL

/*******************************************************************************
  Section: Global Types
*/

/*******************************************************************************
  Type: r_ecm_Dev_t

  Device instance specific data.
  
  All instance specific data is kept within that structure.

  Members:
  BasAddr         - Macro register base address
  FeintCallback   - Pointer table to Isr callback functions
*/

typedef struct 
{
    uint32_t   BaseAddr;
    void       (*FeintCallback[R_DEV_ECMERR_LAST])(void);
} r_ecm_Dev_t;

/*******************************************************************************
  Section: Extern Variable
*/

extern r_ecm_Dev_t r_ecm_Dev[R_ECM_MACRO_NUM];



#ifdef __cplusplus
}
#endif

#endif /* R_ECM_MAIN_H_  */
