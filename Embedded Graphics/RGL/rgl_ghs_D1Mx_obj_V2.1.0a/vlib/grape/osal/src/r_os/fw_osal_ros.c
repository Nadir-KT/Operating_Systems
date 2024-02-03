/*
****************************************************************************
PROJECT : GFX_APPNOTE
FILE    : $Id: fw_osal_ros.c 11006 2016-11-07 16:32:44Z michael.golczewski $
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

/*******************************************************************************
  Title: OSAL r_os module
  
  This module manages the OS Threads
*/

/*******************************************************************************
  Section: Includes
*/
#include "r_typedefs.h"
#include "fw_osal_api.h"
#include "r_tick_api.h"
#include "r_bsp_api.h"

/*******************************************************************************
  Section: Local Defines
*/

/*******************************************************************************
  Defines: _THREAD_NUM
  
  Number of threads allowed
*/

#define LOC_THREAD_NUM        22

/*******************************************************************************
  Defines: _THREAD_STACKSIZE
  
  Stack Size needed by the OS Threads
*/
#define LOC_THREAD_STACKSIZE  2048



/*******************************************************************************
  Section: Local Variables
*/

/*******************************************************************************
  Variable: locThreadData
  
  Array of thread data.
  See <R_OS_ThreadData_t>
*/
static R_OS_ThreadData_t locThreadData[LOC_THREAD_NUM];

/*******************************************************************************
  Variable: locThreadStack
  
  Stack used by the OS threads
*/
static uint32_t          locThreadStack[LOC_THREAD_STACKSIZE];


/*******************************************************************************
  Section: Local Functions
*/

#ifdef GHS_PROFILE
extern void __ghs_manprf_timer_handler(void);

/*******************************************************************************
  Function: locTickIsr

  Wrapper function for calling more than one timer ISR.

  Usually the OS timer tick function is registered for the
  tick timer 0 ISR. In case profiling with target time measurement
  shall be used, the OS tick function and the compiler library
  profiling function needs to be called.

  Parameters:
  void

  Returns:
  void
*/

static void locTickIsr(void) {
    static int cc = 0;

    if ((cc % 10) == 0) 
    {
        __ghs_manprf_timer_handler();
    }
    cc++;
    R_OS_Prv_Tick();
}
#endif


/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: FW_OSAL_Sys_Error
  
  See: r_os_sys.h for details
*/
void R_OS_Sys_Error(int32_t Err)
{
    while (0 != Err)
    {
    }
}

/*******************************************************************************
  Function: FW_OSAL_Sys_Idle
  
  See: r_os_sys.h for details
*/
void R_OS_Sys_Idle(void)
{
    for(;;)
    {
    }
}


/*******************************************************************************
  Function: FW_OSAL_TimeGet

  See: fw_osal_api.h for details
*/

uint32_t FW_OSAL_TimeGet(void)
{
    uint32_t mtime;

    mtime = R_OS_GetTimeMs();
    return mtime;
}


/*******************************************************************************
  Function: FW_OSAL_ThreadSleep
  
  See: fw_osal_api.h for details
*/
void FW_OSAL_ThreadSleep(uint32_t Time)
{
    R_OS_ThreadSleep(Time);
}

/*******************************************************************************
  Function: FW_OSAL_ThreadCreate
  
  See: fw_osal_api.h for details
*/
uint32_t FW_OSAL_ThreadCreate(void *(*Start)(void* Para), void *Arg, void *Stack,
                                uint32_t StackSize, uint8_t Priority)
{
    R_OS_Thread_t           thread;
    R_OS_ThreadAttr_t       attr;
    R_OS_ThreadSchedParam_t sched_param;

    R_OS_ThreadAttrInit(&attr);
    sched_param.SchedPriority = Priority;
    R_OS_ThreadAttrSetSchedParam(&attr, &sched_param);
    R_OS_ThreadAttrSetStackAddr(&attr, Stack);
    R_OS_ThreadAttrSetStackSize(&attr, StackSize);
    R_OS_ThreadCreate(&thread, &attr, Start, Arg);

    return (uint32_t)thread;
}


/*******************************************************************************
  Function: FW_OSAL_ThreadDelete
  
  See: fw_osal_api.h for details
*/
void FW_OSAL_ThreadDelete(void)
{
    R_OS_ThreadExit(0);
    return;
}

/*******************************************************************************
  Function: FW_OSAL_ThreadExit
  
  See: fw_osal_api.h for details
*/
void FW_OSAL_ThreadExit(void *RetVal)
{
    R_OS_ThreadExit(RetVal);
}

/*******************************************************************************
  Function: FW_OSAL_ThreadJoin
  
  See: fw_osal_api.h for details
*/
int32_t FW_OSAL_ThreadJoin(uint32_t ThreadId, void **ThreadReturn)
{
    int32_t x;

    x = (int32_t)R_OS_ThreadJoin(ThreadId, ThreadReturn);
    return x;
}

/*******************************************************************************
  Function: FW_OSAL_ThreadSelf
  
  See: fw_osal_api.h for details
*/
int32_t FW_OSAL_ThreadSelf(void)
{
    return (int32_t)R_OS_ThreadSelf();
}

/*******************************************************************************
  Function: FW_OSAL_InitOS
  
  See: fw_osal_api.h for details
*/

void FW_OSAL_InitOS(void)
{
    R_OS_Init(locThreadData, LOC_THREAD_NUM, locThreadStack, LOC_THREAD_STACKSIZE);
#ifdef GHS_PROFILE
    R_TICK_SetIsrCallback(0, &locTickIsr);
#else
    R_TICK_SetIsrCallback(0, &R_OS_Prv_Tick);
#endif
}


/*******************************************************************************
  Function: FW_OSAL_StartOS
  
  See: fw_osal_api.h for details
*/
void FW_OSAL_StartOS(void)
{
    R_OS_Start();
}

/*******************************************************************************
  Function: FW_OSAL_SemaCreate
  
  See: fw_osal_api.h for details
*/
int32_t FW_OSAL_SemaCreate(fw_osal_Sema_t *Sema)
{
    R_OS_SemInit(Sema, 0, 0);
    return 0;
}

/*******************************************************************************
  Function: FW_OSAL_SemaDestroy
  
  See: fw_osal_api.h for details
*/
int32_t FW_OSAL_SemaDestroy(fw_osal_Sema_t *Sema)
{
    int32_t ret;
    
    ret = R_OS_SemDestroy(Sema);
    
    return ret;
}

/*******************************************************************************
  Function: FW_OSAL_SemaSignal
  
  See: fw_osal_api.h for details
*/
int32_t FW_OSAL_SemaSignal(fw_osal_Sema_t *Sema)
{
    R_OS_SemPost(Sema);
    return 0;
}

/*******************************************************************************
  Function: FW_OSAL_SemaWait
  
  See: fw_osal_api.h for details
*/
int32_t FW_OSAL_SemaWait(fw_osal_Sema_t *Sema)
{
    R_OS_SemWait(Sema);
    return 0;
}



/*******************************************************************************
  Function: FW_OSAL_MutexCreate

  See: fw_osal_api.h for details
*/

int32_t FW_OSAL_MutexCreate(fw_osal_Mutex_t *Mutex)
{
    int32_t rc;

    rc = R_OS_ThreadMutexInit(Mutex, 0);
    return rc;
}


/*******************************************************************************
  Function: FW_OSAL_MutexLock
    
  See: fw_osal_api.h for details
*/

int32_t FW_OSAL_MutexLock(fw_osal_Mutex_t *Mutex)
{
    int32_t rc;

    rc = R_OS_ThreadMutexLock(Mutex);
    return rc;
}


/*******************************************************************************
  Function: FW_OSAL_MutexUnlock

  See: fw_osal_api.h for details
*/

int32_t FW_OSAL_MutexUnlock(fw_osal_Mutex_t *Mutex)
{
    int32_t rc;

    rc = R_OS_ThreadMutexUnlock(Mutex);
    return rc;
}


/*******************************************************************************
  Function: FW_OSAL_MutexDestroy

  See: fw_osal_api.h for details
*/

int32_t FW_OSAL_MutexDestroy(fw_osal_Mutex_t *Mutex)
{
    int32_t rc;

    rc = R_OS_ThreadMutexDestroy(Mutex);
    return rc;
}


/*******************************************************************************
  Function: FW_OSAL_EnterInterrupt
  
  See: fw_osal_api.h for details
*/
void FW_OSAL_EnterInterrupt(void)
{
    R_OS_Prv_EnterInterrupt();
}

/*******************************************************************************
  Function: FW_OSAL_LeaveInterrupt
  
  See: fw_osal_api.h for details
*/
void FW_OSAL_LeaveInterrupt(void)
{
    R_OS_Prv_LeaveInterrupt();
}

