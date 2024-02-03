/*
****************************************************************************
PROJECT : GRAPE - HMIAL
FILE    : $Id: fw_hmial_knob.c 11006 2016-11-07 16:32:44Z michael.golczewski $
AUTHOR  : $Author: michael.golczewski $
============================================================================ 
DESCRIPTION
GRAPE Human Machine Interface  Abstraction Layer (HMIAL)
============================================================================
                            C O P Y R I G H T                                    
============================================================================
                           Copyright (c) 2012
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

/***********************************************************
  Title: BOAL RCAR Linux module
  
  This module manages the BOAL functions.
*/

/***********************************************************
  Section: Includes
*/
#include "r_typedefs.h"
#include "r_bsp_api.h"         /* Board support package */
#include "r_bsp_hmi_api.h"
#include "fw_osal_api.h"
#include "fw_hmial_api.h"

/***********************************************************
  Section: Local Defines
*/


static fw_hmial_Control_t *locControl;

/***********************************************************
  Section: Local Functions
*/


/***********************************************************
  Function: locHmiKnobRight

  KNOB turn right callback.

  Parameters:
  void

  Returns:
  void
*/

static void locHmiKnobRight(void);
static void locHmiKnobRight(void) 
{ 
    if (R_NULL != locControl)
    {
        if (R_NULL != locControl->KnobRight)
        {
            locControl->KnobRight();
        }
    }
}


/***********************************************************
  Function: locHmiKnobLeft

  KNOB turn left callback.


  Parameters:
  void

  Returns:
  void
*/

static void locHmiKnobLeft(void);
static void locHmiKnobLeft(void) 
{ 
    if (R_NULL != locControl)
    {
        if (R_NULL != locControl->KnobLeft)
        {
            locControl->KnobLeft();
        }
    }
}

/***********************************************************
  Function: locHmiKnobDownPress

  KNOB down press callback.

  Parameters:
  void

  Returns:
  void
*/
static void locHmiKnobDownPress(void);
static void locHmiKnobDownPress(void) 
{ 
    if (R_NULL != locControl)
    {
        if (R_NULL != locControl->KnobPress)
        {
            locControl->KnobPress();
        }
    }
}


/***********************************************************
  Section: Global Functions
*/

/***********************************************************
  Function: FW_HMIAL_Init
  
  See: fw_hmial_api.h for details
*/

void FW_HMIAL_Init(void) 
{
    R_BSP_SetButtonCallback(BSP_BTN_LEFT_TURN,  &locHmiKnobLeft); 
    R_BSP_SetButtonCallback(BSP_BTN_RIGHT_TURN, &locHmiKnobRight); 
    R_BSP_SetButtonCallback(BSP_BTN_OK_PRESS,   &locHmiKnobDownPress); 
}


/***********************************************************
  Function: FW_HMIAL_DeInit
  
  See: fw_hmial_api.h for details
*/

void FW_HMIAL_DeInit(void) 
{
}


/***********************************************************
  Function: FW_HMIAL_SetControl

  See: fw_hmial_api.h for details
*/

void FW_HMIAL_SetControl(const fw_hmial_Control_t *Control) 
{
    locControl = (fw_hmial_Control_t *)Control;
}



