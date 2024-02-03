/*
****************************************************************************
PROJECT : eeOS2
FILE    : $Id: r_os_prv_thread.c 11006 2016-11-07 16:32:44Z michael.golczewski $
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
#include "r_os_sys.h"
#include "r_os_thread.h"
#include "r_os_prv_thread.h"
#include "r_os_prv_main.h"
#include "r_os_asm_switch.h"


/*******************************************************************************
  Section: Local Defines
*/

#define LOC_ROS_DEFAULT_STACKSIZE 0x400
#define LOC_ROS_FIRST_THREADID    1000
#define LOC_ROS_UINT_MAX          0xffffffff


/*******************************************************************************
  Section: Local Variables
*/

/*******************************************************************************
  Variable: _NextTID

  Next thread identifier.

  The variable is used to assign an identifier to a
  newly created thread.
*/

static R_OS_Thread_t  _NextTID = LOC_ROS_FIRST_THREADID;


/*******************************************************************************
  Section: Local Functions
*/
/*******************************************************************************
  Function: _AllocThreadData
*/
static R_OS_ThreadData_t *_AllocThreadData(void)
{
    uint32_t i;

    i = 0;
    while (i < R_OS_ThreadPoolSize)
    {
        if (R_OS_ThreadPool[i].State == R_OS_TSTATE_FREE)
        {
            break;
        }
        i++;
    }
    if (i >= R_OS_ThreadPoolSize)
    {
        R_OS_Sys_Error(-1);
        return 0;
    }
    return &R_OS_ThreadPool[i];
}

/*******************************************************************************
  Function: _IsStackInUse
*/
static int32_t _IsStackInUse(void *Addr)
{
    R_OS_ThreadData_t *thread;

    thread = R_OS_ThreadRoot;
    while (thread != 0)
    {
        if (thread->StackBase == Addr)
        {
            return 1;
        }
        thread = thread->Next;
    }
    return 0;
}

/*******************************************************************************
  Function: _AllocThreadStack
*/
static void *_AllocThreadStack(uint32_t Size)
{
    void *stack;
    uint32_t  x;

    stack = R_OS_ThreadStack;
    while ((char*)stack < ((char*)R_OS_ThreadStack + R_OS_ThreadStackSize))
    {
        x = _IsStackInUse(stack);
        if (x == 0)
        {
            break;
        }
        stack = (char*)stack + LOC_ROS_DEFAULT_STACKSIZE;
    }
    if ((char*)stack >= ((char*)R_OS_ThreadStack + R_OS_ThreadStackSize))
    {
        R_OS_Sys_Error(-1);
        return 0;
    }
    return stack;
}

/*******************************************************************************
  Function: _InsertThread
*/
static void _InsertThread(R_OS_ThreadData_t *Thread)
{
    R_OS_ThreadData_t *thread;
    R_OS_ThreadData_t *prev;

    if (R_OS_ThreadRoot == 0)
    {
        R_OS_ThreadRoot = Thread;
        return;
    }
    prev = 0;
    for (thread = R_OS_ThreadRoot; thread != 0; thread = thread->Next)
    {
        if (Thread->Priority >= thread->Priority)
        {
            break;
        }
        prev = thread;
    }
    if (prev != 0)
    {
        Thread->Next         = prev->Next;
        prev->Next           = Thread;
    }
    else
    {
        Thread->Next         = R_OS_ThreadRoot;
        R_OS_ThreadRoot      = Thread;
    }
}


/*******************************************************************************
  Function: _DeleteThread
*/
static void _DeleteThread(R_OS_ThreadData_t *Thread)
{
    R_OS_ThreadData_t *thread;
    R_OS_ThreadData_t *prev;

    prev = 0;
    for (thread = R_OS_ThreadRoot; thread != 0; thread = thread->Next)
    {
        if (Thread == thread)
        {
            break;
        }
        prev = thread;
    }
    if (prev != 0)
    {
        prev->Next = Thread->Next;
    }
    else
    {
        R_OS_ThreadRoot = Thread->Next;
    }
}


/*******************************************************************************
  Function: _SetupStack
*/
static void _SetupStack(R_OS_ThreadAttr_t *Attr)
{
    Attr->StackAddr     = _AllocThreadStack(LOC_ROS_DEFAULT_STACKSIZE);
    Attr->StackSize     = LOC_ROS_DEFAULT_STACKSIZE;
}

/*******************************************************************************
  Function: _SetupDefaultAttr
*/
static void _SetupDefaultAttr(R_OS_ThreadAttr_t *Attr)
{
    Attr->IsInitialized            = 1;
    Attr->SchedParam.SchedPriority = 0;
    _SetupStack(Attr);
}


/*******************************************************************************
  Function: _CheckJoinEvent
*/
static int32_t _CheckJoinEvent(R_OS_ThreadData_t *Thread, void *RetVal)
{
    R_OS_ThreadData_t *thread;

    thread  = R_OS_ThreadRoot;
    while (thread != 0)
    {
        if (thread->State == R_OS_TSTATE_WAITING)
        {
            if (thread->Data.Resource == Thread)
            {
                thread->State = R_OS_TSTATE_READY;
                thread->Data.Resource = RetVal;
                return 1;
            }
        }
        thread = thread->Next;
    }
    return 0;
}


/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_OS_Prv_ThreadCreate
*/

int32_t R_OS_Prv_ThreadCreate(R_OS_ThreadCreateParam_t *Param,
                          R_OS_ThreadFunc_t StartFunction, void *Arg)
{
    R_OS_ThreadData_t *thread;
    R_OS_ThreadAttr_t  attr;

    if (Param->Attr != 0)
    {
        attr = *Param->Attr;
        if ((attr.StackAddr == 0) || (attr.StackSize == 0))
        {
            _SetupStack(&attr);
        }
    }
    else
    {
        _SetupDefaultAttr(&attr);
    }

    thread            = _AllocThreadData();
    thread->State     = R_OS_TSTATE_NEW;
    thread->Next      = 0;
    thread->Priority  = attr.SchedParam.SchedPriority;
    thread->ThreadId  = _NextTID;
    *Param->Thread    = thread->ThreadId;
    thread->StackBase = attr.StackAddr;
    thread->StackSize = attr.StackSize;
    thread->Stack     = (char*)thread->StackBase + thread->StackSize;
    if ((thread->StackBase == 0) || (thread->StackSize == 0))
    {
        R_OS_Sys_Error(-1);
        return -1;
    }

    _NextTID++; /* FIXME: Check overrun */
    R_OS_Asm_ThreadPrepareStack(thread, (void*)StartFunction, Arg);
    _InsertThread(thread);
    thread->State     = R_OS_TSTATE_READY;
    return 0;
}


/*******************************************************************************
  Function: R_OS_Prv_ThreadExit
*/

void R_OS_Prv_ThreadExit(void *RetVal)
{
    R_OS_ThreadData_t *thread;

    /* Remove ME from thread chain */
    R_OS_Prv_EnterCriticalRegion();
    R_OS_ThreadCurrent->State = R_OS_TSTATE_DEAD;
    thread                    = R_OS_ThreadCurrent;
    R_OS_ThreadCurrent        = 0;
    _DeleteThread(thread);
    /* $$$$: 
       To be 100% clean, it would be necessary to switch
       to the system stack for the following code
    */
    thread->State             = R_OS_TSTATE_FREE;
    _CheckJoinEvent(thread, RetVal);
    R_OS_Prv_ThreadSwitchCooperative();
    R_OS_Prv_LeaveCriticalRegion();
    /* If you would  get here, there is something wrong! */
    R_OS_Sys_Error(-1);
}


/*******************************************************************************
  Function: R_OS_Prv_ThreadJoin
*/

int32_t R_OS_Prv_ThreadJoin(R_OS_Thread_t Thread, void **ThreadReturn)
{
    R_OS_ThreadData_t *thread;

    thread  = R_OS_ThreadRoot;
    while (thread != 0)
    {
        if (thread->ThreadId == Thread)
        {
            R_OS_Prv_ThreadWait(R_OS_TSTATE_WAITING, thread); 
            if (ThreadReturn != 0)
            {
                *ThreadReturn = R_OS_ThreadCurrent->Data.Resource;
            }
            return 0;
        }
        thread = thread->Next;
    }
    return -1;
}

/*******************************************************************************
  Function: R_OS_Prv_ThreadSelf
*/

R_OS_Thread_t R_OS_Prv_ThreadSelf(void)
{
    R_OS_Thread_t thread;

    thread = R_OS_ThreadCurrent->ThreadId;
    return thread;
}
