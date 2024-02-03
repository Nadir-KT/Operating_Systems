/************************************************************************************************
PROJECT : Octa calibration
FILE    : $Id: r_octa_calib.h 13492 2017-05-18 03:54:06Z shinobu.uehara $
============================================================================
DESCRIPTION
Main funcitons of the OCTA test application
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
Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.

****************************************************************************
*/

/*******************************************************************************
  Title: Serial flash calibration

*/

#ifndef R_OCTA_CALIB_H
#define R_OCTA_CALIB_H

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
  Section Includes
*/

#include "r_octa_api.h"

/***************************************************************************
  Constant: LOC_DELAY_PART_~

  Delay trim type.
  
  LOC_DELAY_PART_FINE          - fine part.
  LOC_DELAY_PART_COARSE        - coarse part.
*/
#define LOC_DELAY_PART_FINE   (0x01)
#define LOC_DELAY_PART_COARSE (0x02)

/***************************************************************************
  Function: OCTA_CalibrationPreparation

  Description:
  This function executes the preration for manual calibration.

  Parameter:
  Unit    - Instance number.
  Channel - Device Channel number.
  Config  - Pointer to the <r_octa_Config_t> structure.

  Return value:
  See <octa_Error_t>.
*/
r_octa_Error_t OCTA_CalibrationPreparation(uint32_t Unit, uint32_t Channel, r_octa_Config_t * Config);


/***************************************************************************
  Function: OCTA_Calibration

  Description:
  This function executes the manual calibration.

  Parameter:
  Unit    - Instance number.
  Channel - Device Channel number.
  Config  - Pointer to the <r_octa_Config_t> structure.

  Return value:
  See <octa_Error_t>.
*/
r_octa_Error_t OCTA_Calibration(uint32_t Unit, uint32_t Channel, r_octa_Config_t * Config);


/***************************************************************************
  Function: OCTA_CalibrationCalcDelayValue

  Description:
  This function sets the calcurate delay value.

  Parameter:
  Unit    - Instance number.
  Channel - Device Channel number.
  Config  - Pointer to the <r_octa_Config_t> structure.
  Delay   - Delay value.
  Mode    - Delay value part.

  Return value:
  See <octa_Error_t>.
*/
static r_octa_Error_t OCTA_CalibrationCalcDelayValue(uint32_t Unit, uint32_t Channel,
                                                          r_octa_Config_t * Config,
                                                          uint32_t          Delay,
                                                          uint32_t          Mode);


#ifdef __cplusplus
}
#endif

#endif /* R_OCTA_CALIB_H */

