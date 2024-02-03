/*
****************************************************************************
PROJECT : ROS2
FILE    : $Id: r_os_thread.c 11006 2016-11-07 16:32:44Z michael.golczewski $
AUTHOR  : $Author: michael.golczewski $
============================================================================ 
DESCRIPTION
OS for internal investigation with a pthread like API
============================================================================
                            C O P Y R I G H T
============================================================================
                       Copyright (c) 2013 - 2014
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


/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"     /* Renesas basic types, e.g. uint32_t */
#include "r_os_api.h"
#include "r_os_syscall.h"
#include "r_os_thread.h"


/*******************************************************************************
  Section: Global API Functions
*/

/*******************************************************************************
  Function: R_OS_ThreadCreate
*/

int32_t R_OS_ThreadCreate(R_OS_Thread_t *Thread, R_OS_ThreadAttr_t *Attr,
                      R_OS_ThreadFunc_t StartFunction, void *Arg)
{
    R_OS_ThreadCreateParam_t param;
    uint32_t                     rc;

    param.Thread = Thread;
    param.Attr   = Attr;
    rc = R_OS_SysCall(R_OS_SYSCALL_THREADCREATE, &param, (void*)StartFunction, Arg);
    return rc;
}

/*******************************************************************************
  Function: R_OS_ThreadExit
*/

void R_OS_ThreadExit(void *RetVal)
{
    R_OS_SysCall(R_OS_SYSCALL_THREADEXIT, RetVal, 0, 0);
}


/*******************************************************************************
  Function: R_OS_ThreadJoin
*/

int32_t R_OS_ThreadJoin(R_OS_Thread_t Thread, void **ThreadReturn)
{
    R_OS_SysCall(R_OS_SYSCALL_THREADJOIN, (void*)Thread, ThreadReturn, 0);
    return 0;
}


/*******************************************************************************
  Function: R_OS_ThreadSleep
*/

void R_OS_ThreadSleep(uint32_t Time)
{
    R_OS_SysCall(R_OS_SYSCALL_THREADSLEEP, (void*)Time, 0, 0);
}


/*******************************************************************************
  Function: R_OS_ThreadSelf
*/

R_OS_Thread_t R_OS_ThreadSelf(void)
{
    R_OS_Thread_t rc;
    
    rc = R_OS_SysCall(R_OS_SYSCALL_THREADSELF, 0, 0, 0);
    return rc;
}


/*******************************************************************************
  Function: R_OS_ThreadAttrSetStackAddr
*/

int32_t R_OS_ThreadAttrSetStackAddr(R_OS_ThreadAttr_t *Attr, void *Addr)
{
    Attr->StackAddr = Addr;
    return 0;
}

/*******************************************************************************
  Function: R_OS_ThreadAttrSetStackSize
*/

int32_t R_OS_ThreadAttrSetStackSize(R_OS_ThreadAttr_t *Attr, uint32_t Size)
{
    Attr->StackSize = Size;
    return 0;
}


/*******************************************************************************
  Function: R_OS_ThreadAttrSetSchedParam
*/

int32_t R_OS_ThreadAttrSetSchedParam(R_OS_ThreadAttr_t *Attr,
                                 const R_OS_ThreadSchedParam_t *SchedParam)
{
    Attr->SchedParam = *SchedParam;
    return 0;
}


/*******************************************************************************
  Function: R_OS_ThreadAttrInit
*/

int32_t R_OS_ThreadAttrInit(R_OS_ThreadAttr_t *Attr)
{
    Attr->IsInitialized            = 1;
    Attr->StackAddr                = 0;
    Attr->StackSize                = 0;
    Attr->SchedParam.SchedPriority = 0;
    return 0;
}

