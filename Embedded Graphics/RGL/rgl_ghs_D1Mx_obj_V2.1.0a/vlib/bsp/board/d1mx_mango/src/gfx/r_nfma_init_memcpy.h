/*
****************************************************************************
PROJECT : BSP - Raw NAND flash routines
FILE    : $Id: r_nfma_init_memcpy.h 12471 2017-02-06 12:55:42Z tobyas.hennig $
============================================================================
DESCRIPTION
Main functions of the NFMA init and memcopy
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
Purpose: only for testing

DISCLAIMER
This software is supplied by Renesas Electronics Corporation and is only
intended for use with Renesas products. No other uses are authorized. This
software is owned by Renesas Electronics Corporation and is protected under
all applicable laws, including copyright laws.
THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
Renesas reserves the right, without notice, to make changes to this software
and to discontinue the availability of this software. By using this software,
you agree to the additional terms and conditions found by accessing the
following link:
http://www.renesas.com/disclaimer *
Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.

****************************************************************************
*/

/*******************************************************************************
  Title: NFMA init and memcopy

*/

#ifndef R_NFMA_INIT_H
#define R_NFMA_INIT_H

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
  Section Includes
*/

/*******************************************************************************
  Constant: NF_UNIT0

  Instance number of used NFMA macro
*/
#define NF_UNIT0        0u

/*******************************************************************************
  Constant: NF_FLASH_COPY_START

  Start address of data to copy from NAND flash device
*/
#define NF_FLASH_COPY_START     0x00000000u

/*******************************************************************************
  Constant: NF_FLASH_COPY_DST

  destination to where data is copy to by NFMA
*/
#define NF_FLASH_COPY_DST       (D1M1A_SDRAM_BASE)  /* copy to SDRAM */

/*******************************************************************************
  Constant: NF_FLASH_COPY_SIZE

  Size of data to copy from NAND flash device
*/
#define NF_FLASH_COPY_SIZE      (D1M1A_SDRAM_SIZE)  /* copy as much as fit in SDRAM */


/***********************************************************
  Section Global Constants
*/


/*******************************************************************************
  Section Global Functions
*/


/***************************************************************************
  Function: NF_Init

  Description:
  Initialisation function for the NFMA

  Parameter:
  Unit       - instance number of the NFMA macro.

  Return value:
  None
*/
void NF_Init(uint32_t Unit);

/***************************************************************************
  Function: NF_MemCpy

  Description:
  Copy data from NFMA (NFMA acts as XC master that performs copy operation)
  to other XC slave memory (e.g. SDRAM)

  Parameter:
  Unit       - instance number of the NFMA macro.
  Dst        - Destination address (of XC slave memory as seen from NFMA XC master)
  Src        - Source address (of memory connected to NFMA macro)
  len        - Length of data to be copied (in byte)

  Return value:
  None
*/
void NF_MemCpy(uint32_t Unit, uint32_t Dst, uint32_t Src, uint32_t len);


#ifdef __cplusplus
}
#endif

#endif /* R_NFMA_INIT_H */

