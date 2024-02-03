/*
****************************************************************************
PROJECT : GRAPE
FILE    : $Id: fw_boal_api.h 11006 2016-11-07 16:32:44Z michael.golczewski $
AUTHOR  : $Author: michael.golczewski $
============================================================================ 
DESCRIPTION
GRAPE Board Abstraction Layer (BOAL)
============================================================================
                            C O P Y R I G H T                                    
============================================================================
                           Copyright (c) 2011
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

#ifndef R_FW_BOAL_API_H_
#define R_FW_BOAL_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Title: BOAL
  
  Board API - Board setup abstraction layer

  An application using the BOAL should include this header file.
*/


/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: FW_BOAL_LowLevelInit

  Initialises the board before OS startup.
  
  Parameters:
  void

  Returns:
  void
*/

void FW_BOAL_LowLevelInit(void);


/*******************************************************************************
  Function: FW_BOAL_LowLevelDeInit

  DeInitialises the board after OS has stopped.
  
  Parameters:
  void

  Returns:
  void
*/

void FW_BOAL_LowLevelDeInit(void);


/*******************************************************************************
  Function: FW_BOAL_OsLevelInit

  Initialises the board after OS startup.
  
  Parameters:
  void

  Returns:
  void
*/

void FW_BOAL_OsLevelInit(void);


/*******************************************************************************
  Function: FW_BOAL_OsLevelDeInit

  DeInitialises the board before OS has stopped.
  
  Parameters:
  void

  Returns:
  void
*/

void FW_BOAL_OsLevelDeInit(void);


#ifdef __cplusplus
}
#endif

#endif /* R_FW_BOAL_API_H_ */
