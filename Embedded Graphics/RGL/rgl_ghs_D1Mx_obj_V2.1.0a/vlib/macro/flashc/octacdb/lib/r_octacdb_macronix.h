/*
****************************************************************************
PROJECT : OCTA driver
FILE    : $Id: r_octacdb_macronix.h 13331 2017-04-24 05:56:43Z shinobu.uehara $
============================================================================
DESCRIPTION
Flash driver for a macronix device
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
  Title: Octa flash command table

  Note: This file defines the macro define and the flash command table for
        Macronix MX25LW51245G octa flash device and JSC JSC64SSU8AGDY octa RAM device.
        If you use other octa flash device, change setting
        parameters according to the model number of the target.

*/

#ifndef R_OCTADB_MACRONIX_H
#define R_OCTADB_MACRONIX_H

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
  Section: Global Defines
*/

/***************************************************************************
  Constant: R_OCTADB_FLASH_MEMORY_SIZE

  Memory size (Byte) of Octa flash.
*/
#define R_OCTADB_FLASH_MEMORY_SIZE       (64 * 1024 * 1024)

/***************************************************************************
  Constant: R_OCTADB_FLASH_SECTOR_SIZE

  Erase sector size (Byte) of Octa flash.
*/
#define R_OCTADB_FLASH_SECTOR_SIZE       (4 * 1024)

/***************************************************************************
  Constant: R_OCTADB_FLASH_PAGE_SIZE

  Page size (Byte) of Octa flash.
*/
#define R_OCTADB_FLASH_PAGE_SIZE    (256)

/***************************************************************************
  Constant: R_OCTADB_FLASH_DQS_EN_CNT_OPI

  DQS enable count value of Octa flash OPI mode.
*/
#define R_OCTADB_FLASH_DQS_EN_CNT_OPI    (8)

/***************************************************************************
  Constant: R_OCTADB_FLASH_DQS_EN_CNT_DOPI

  DQS enable count value of Octa flash DOPI mode.
*/
#define R_OCTADB_FLASH_DQS_EN_CNT_DOPI   (5)

/***************************************************************************
  Constant: R_OCTADB_RAM_MEMORY_SIZE

  Memory size (Byte) of Octa RAM.
*/
#define R_OCTADB_RAM_MEMORY_SIZE         (8 * 1024 * 1024)

/***************************************************************************
  Constant: R_OCTADB_RAM_DQS_EN_CNT

  DQS enable count value of Octa RAM.
*/
#define R_OCTADB_RAM_DQS_EN_CNT          (6)

/***************************************************************************
  Section: Global Variables
*/

extern r_octa_Command_t  r_octacdb_FlashCmdTbl;
extern r_octa_Command_t  r_octacdb_RamCmdTabl;

#ifdef __cplusplus
}
#endif

#endif /* R_OCTADB_MACRONIX_H */

