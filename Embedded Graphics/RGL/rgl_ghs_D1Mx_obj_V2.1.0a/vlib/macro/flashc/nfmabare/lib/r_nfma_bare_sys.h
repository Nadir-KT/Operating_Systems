/*
****************************************************************************
PROJECT : NFMA driver
FILE    : $Id: r_nfma_bare_sys.h 12467 2017-02-06 12:29:03Z tobyas.hennig $
============================================================================
DESCRIPTION
Driver for NFMA macro - driver SYS
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

#ifndef R_NFMABARE_SYS_H_
#define R_NFMABARE_SYS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Title: Generic NFMA Support Functions

  The generic NFMA driver uses these functions. They have
  to be implemented within the driver library for a concrete
  device.
*/

/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_SYS_NFMA_GetBaseAddr

  Return base address of NFMA macro instance
  
  Parameters:
  Unit       - Instance number
  
  Returns:
  base address
*/
uint32_t R_SYS_NFMA_GetBaseAddr(uint32_t instance);

/*******************************************************************************
  Function: R_SYS_NFMA_GetClkFreqMHz

  Return frequency of NFMA macro in MHz
  
  Parameters:
  Unit       - Instance number
  
  Returns:
  frequency
*/
/*******************************************************************************
  Function: R_SYS_NFMA_GetClkFreqMHz
*/
uint32_t R_SYS_NFMA_GetClkFreqMHz(uint32_t instance);


#ifdef __cplusplus
}
#endif


#endif /* R_NFMABARE_SYS_H_ */
