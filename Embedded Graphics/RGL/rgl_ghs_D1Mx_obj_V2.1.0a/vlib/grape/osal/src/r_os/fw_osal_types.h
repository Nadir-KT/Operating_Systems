/*
****************************************************************************
PROJECT : GFX_APPNOTE
FILE    : $Id: fw_osal_types.h 11006 2016-11-07 16:32:44Z michael.golczewski $
AUTHOR  : $Author: michael.golczewski $
============================================================================ 
DESCRIPTION
Sample program for demonstration of the Dx4 GFX features
============================================================================
                            C O P Y R I G H T                                    
============================================================================
                           Copyright (c) 2013
                                  by 
                       NEC Electronics (Europe) GmbH. 
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

#ifndef FW_OSAL_TYPES_H_
#define FW_OSAL_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Title: fw_osal_types.h
  
  OS Application layer's types

  This header file is included in r_os_api.h, user does not need to
  include it directly.
*/

/*******************************************************************************
  Section: Includes
*/
#include "r_os_api.h"

/*******************************************************************************
  Section: Global Types
*/

/*******************************************************************************
  Type: OSAL_Sema_t
  
  Semaphore type
  
*/

typedef R_OS_Sem_t fw_osal_Sema_t;



/*******************************************************************************
  Type: OSAL_Mutex_t
  
  Semaphore type
  
*/

typedef R_OS_ThreadMutex_t fw_osal_Mutex_t;


#ifdef __cplusplus
}
#endif

#endif /* FW_OSAL_TYPES_H_ */
