/*
****************************************************************************
PROJECT : HYPB driver
FILE    : $Id: r_hypdb_issi.h 12586 2017-02-13 06:21:33Z shinya.tomari $
============================================================================
DESCRIPTION
Flash driver for a macronix device
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
  Title: Serial flash command table

  Note: This file defines the macro define and the flash command table for
        ISSI IS26KS128S hyper flash device and IS66WVH16M8BLL hyper RAM device.
        If you use other hyper flash device, change setting
        parameters according to the model number of the target.

*/

#ifndef R_HYPBDB_ISSI_H
#define R_HYPBDB_ISSI_H

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
  Section: Global Defines
*/

/***************************************************************************
  Constant: R_HYPDB_FLASH_MEMORY_SIZE

  Memory size (Byte) of Hyper flash.
  
  RH850/D1Mx supports up to 16 Mbytes flash.
*/
#define R_HYPDB_FLASH_MEMORY_SIZE       (16 * 1024 * 1024)

/***************************************************************************
  Constant: R_HYPDB_FLASH_SECTOR_SIZE

  Uniform Sector size (Byte) of Hyper flash.
*/
#define R_HYPDB_FLASH_SECTOR_SIZE       (256 * 1024)

/***************************************************************************
  Constant: R_HYPDB_FLASH_PARAM_SEC_SIZE

  Parameter Sector size (Byte) of Hyper flash.
*/
#define R_HYPDB_FLASH_PARAM_SEC_SIZE    (4 * 1024)

/***************************************************************************
  Constant: R_HYPDB_FLASH_PARAM_SEC_NUM

  Parameter Sector size (Byte) of Hyper flash.
*/
#define R_HYPDB_FLASH_PARAM_SEC_NUM     (8)

/***************************************************************************
  Constant: R_HYPDB_FLASH_WRITE_BUF_SIZE

  Write buffer size (Byte) of Hyper flash.
*/
#define R_HYPDB_FLASH_WRITE_BUF_SIZE    (512)

/***************************************************************************
  Constant: R_HYPDB_RAM_MEMORY_SIZE

  Memory size (Byte) of Hyper RAM.

  RH850/D1Mx supports up to 16 Mbytes RAM and supports only 1 die.
*/
#define R_HYPDB_RAM_MEMORY_SIZE         (8 * 1024 * 1024)

/***************************************************************************
  Section: Global Variables
*/

extern r_hypb_Command_t  r_hypdb_FlashCmdTbl;
extern r_hypb_Command_t  r_hypdb_RamCmdTabl;

#ifdef __cplusplus
}
#endif

#endif /* R_HYPBDB_ISSI_H */

