/*
****************************************************************************
PROJECT : GRAPE
FILE    : $Id: fw_hmial_api.h 11006 2016-11-07 16:32:44Z michael.golczewski $
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

#ifndef FW_HMIAL_H_
#define FW_HMIAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Title: HMIAL
  
  HMIAL API - Funcions to interact with controls

  An application using the HMIAL shall include this header file.
*/

/***********************************************************
  Type: fw_hmial_Control_t

  HMI Control structure.

  An application can define such a strucure to get the control 
  over the board's HMI elements.

  Members:
  KnobPress            - KNOB press callback
  KnobRelease          - KNOB release callback
  KnobRight            - KNOB right callback
  KnobLeft             - KNOB left callback
  ButtonUpPress        - BUTTON up press callback
  ButtonUpRelease      - BUTTON up release callback
  ButtonDownPress      - BUTTON down press callback
  ButtonDownRelease    - BUTTON down release callback
  ButtonMiddlePress    - BUTTON middle press callback
  ButtonMiddleRelease  - BUTTON middle release callback
  ButtonLeftPress      - BUTTON left press callback
  ButtonLeftRelease    - BUTTON left release callback
  ButtonRightPress     - BUTTON right press callback
  ButtonRightRelease   - BUTTON right release callback
*/

typedef struct 
{
    void (*KnobPress)(void);
    void (*KnobRelease)(void);
    void (*KnobRight)(void);
    void (*KnobLeft)(void);
    void (*ButtonUpPress)(void);
    void (*ButtonUpRelease)(void);
    void (*ButtonDownPress)(void);
    void (*ButtonDownRelease)(void);
    void (*ButtonMiddlePress)(void);
    void (*ButtonMiddleRelease)(void);
    void (*ButtonLeftPress)(void);
    void (*ButtonLeftRelease)(void);
    void (*ButtonRightPress)(void);
    void (*ButtonRightRelease)(void);
} fw_hmial_Control_t;


/***********************************************************
  Section: Global Functions
*/

/***********************************************************
  Function: FW_HMIAL_Init

  Initialises the HMIAL.
  
  Parameters:
  void

  Returns:
  void
*/

void FW_HMIAL_Init(void);


/***********************************************************
  Function: FW_HMIAL_DeInit

  DeInitialises the HMIAL.
  
  Parameters:
  void

  Returns:
  void
*/

void FW_HMIAL_DeInit(void);


/***********************************************************
  Function: FW_HMIAL_SetControl

  Use the specified callback table for HMI elements.

  Parameters:
  Control   - Function table for HMI callback functions

  Returns:
  void
*/

void FW_HMIAL_SetControl(const fw_hmial_Control_t *Control);


#ifdef __cplusplus
}
#endif

#endif /* FW_HMIAL_H_ */
