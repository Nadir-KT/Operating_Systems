/*
****************************************************************************
PROJECT : GRAPE
FILE    : $Id: fw_osal_api.h 11006 2016-11-07 16:32:44Z michael.golczewski $
AUTHOR  : $Author: michael.golczewski $
============================================================================ 
DESCRIPTION
GRAPE Operating System Abstraction Layer (OSAL)
============================================================================
                            C O P Y R I G H T                                    
============================================================================
                           Copyright (c) 2013
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

#ifndef FW_OSAL_API_H_
#define FW_OSAL_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Title: OSAL
  
  Operating System (OS) Abstraction Layer
  
  An application using the OSAL should include this header file.
*/

/*******************************************************************************
  Section: Includes
*/
#include "fw_osal_types.h"


/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: FW_OSAL_InitOS

  Initialises the OS and set the Tick call back function.
  
  Parameters:
  
  None

  Returns:
  
  None
*/
void FW_OSAL_InitOS(void);

/*******************************************************************************
  Function: FW_OSAL_InitOS

  Starts the OS.
  
  Parameters:
  
  None

  Returns:
  
  None
*/
void FW_OSAL_StartOS(void);


/*******************************************************************************
  Function: FW_OSAL_ThreadSleep

  Suspends a thread for specified time.

  The calling thread is suspended for at least 'Time' ticks.
  This function does not exist in the PTHREAD definition.

  Parameters:
  Time          - Time in ticks

  Returns:
  void
*/

void FW_OSAL_ThreadSleep(uint32_t Time); 

/*******************************************************************************
  Function: FW_OSAL_TimeGet

  Get time.

  Return the current system time in ticks
  This function does not exist in the PTHREAD definition.

  Parameters:
  void

  Returns:
  System time in ticks
*/

uint32_t FW_OSAL_TimeGet(void); 


/*******************************************************************************
  Function: FW_OSAL_ThreadCreate

  Creates a thread.

  Parameters:
  Start     - Pointer to the thread start function
  Arg       - Not used.
  Stack     - Start address of the stack the thread will be using
  StackSize - Size of the stack
  Priority  - Thread's priority
  
  Returns:
  uint32_t  - Id of the thread.
*/
uint32_t FW_OSAL_ThreadCreate(void *(*Start)(void* Para), void *Arg, void *Stack,
                           uint32_t StackSize, uint8_t Priority);

void FW_OSAL_ThreadDelete(void);

int32_t FW_OSAL_ThreadJoin(uint32_t ThreadId, void **ThreadReturn);

int32_t FW_OSAL_ThreadSelf(void);

/*******************************************************************************
  Function: FW_OSAL_ThreadExit

  Terminate a thread.

  The function will terminate the calling thread and not
  return to the calling function.

  Parameters:
  RetVal        - Return value for FW_OSAL_ThreadJoin

  Returns:
  void
*/

void FW_OSAL_ThreadExit(void *RetVal); 

void FW_OSAL_Sys_Error(int32_t Err);

void FW_OSAL_Sys_Idle(void);

/*******************************************************************************
  Function: FW_OSAL_SemaCreate

  Initialises the semaphore passed as parameter.

  Parameters:
  
  Sema     - Pointer to a variable of type <OSAL_Sema_t>
    
  Returns:
  
  int32_t  - Always 0
*/
int32_t FW_OSAL_SemaCreate(fw_osal_Sema_t *Sema);

/*******************************************************************************
  Function: FW_OSAL_SemaSignal
    
    Unlock the semaphore
    
  Parameters:
  
  Sema     - Pointer to a variable of type <OSAL_Sema_t>
    
  Returns:
  
  int32_t  - Always 0
*/
int32_t FW_OSAL_SemaSignal(fw_osal_Sema_t *Sema);

/*******************************************************************************
  Function: FW_OSAL_SemaWait

    Locks the semaphore
    
  Parameters:
  
  Sema     - Pointer to a variable of type <OSAL_Sema_t>
    
  Returns:
  
  int32_t  - Always 0
*/
int32_t FW_OSAL_SemaWait(fw_osal_Sema_t *Sema);

/*******************************************************************************
  Function: FW_OSAL_SemaDestroy

    Destroy the semaphore
    
  Parameters:
  
  Sema     - Pointer to a variable of type <OSAL_Sema_t>
    
  Returns:
  
  int32_t  - Always 0
*/
int32_t FW_OSAL_SemaDestroy(fw_osal_Sema_t *Sema);


/*******************************************************************************
  Function: FW_OSAL_MutexCreate

  Initialises the mutex passed as parameter.

  Parameters:
  Mutex     - Pointer to a variable of type <OSAL_Mutex_t>
    
  Returns:
  0
*/

int32_t FW_OSAL_MutexCreate(fw_osal_Mutex_t *Mutex);


/*******************************************************************************
  Function: FW_OSAL_MutexLock
    
  Unlock the mutex
    
  Parameters:
  Mutex     - Pointer to a variable of type <OSAL_Mutex_t>
    
  Returns:
  0
*/

int32_t FW_OSAL_MutexLock(fw_osal_Mutex_t *Mutex);


/*******************************************************************************
  Function: FW_OSAL_MutexUnlock

  Locks the mutex
    
  Parameters:
  Mutex     - Pointer to a variable of type <OSAL_Mutex_t>
    
  Returns:
  0
*/

int32_t FW_OSAL_MutexUnlock(fw_osal_Mutex_t *Mutex);


/*******************************************************************************
  Function: FW_OSAL_MutexDestroy

  Destroy the mutex
    
  Parameters:
  Mutex     - Pointer to a variable of type <OSAL_Mutex_t>
    
  Returns:
  0
*/

int32_t FW_OSAL_MutexDestroy(fw_osal_Mutex_t *Mutex);



/*******************************************************************************
  Function: FW_OSAL_EnterInterrupt

    Activates an OS interrupt.
    
  Parameters:
  
  None
    
  Returns:
  
  None
*/
void FW_OSAL_EnterInterrupt(void);

/*******************************************************************************
  Function: FW_OSAL_LeaveInterrupt

  Ends an OS interrupt call.

  Parameters:
  
  None
    
  Returns:
  
  None
*/
void FW_OSAL_LeaveInterrupt(void);




#ifdef __cplusplus
}
#endif

#endif /* FW_OSAL_API_H_ */
