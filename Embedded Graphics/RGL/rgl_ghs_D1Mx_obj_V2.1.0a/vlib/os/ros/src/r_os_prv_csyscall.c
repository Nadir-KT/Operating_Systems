/*
****************************************************************************
PROJECT : eeOS2
FILE    : $Id: r_os_prv_csyscall.c 11006 2016-11-07 16:32:44Z michael.golczewski $
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
#include "r_os_prv_main.h"
#include "r_os_thread.h"
#include "r_os_prv_thread.h"
#include "r_os_prv_tick.h"
#include "r_os_prv_mutex.h"
#include "r_os_prv_cond.h"
#include "r_os_prv_sem.h"


/*******************************************************************************
  Section: Global Functions
*/

int32_t R_OS_Prv_CSysCall(R_OS_SysCall_t Cmd, void *Par1,
                      void *Par2, void *Par3)
{
    int32_t rc;

    /* $$$$: The following construction has a drawback
       related to the OS code size. Even if an application
       does not use e.g. MUTEX functions, the MUTEX module
       will be linked, because there is a reference to it.
     */
    switch (Cmd)
    {
    case R_OS_SYSCALL_NOP:
        rc = 0;
        break;
    case R_OS_SYSCALL_THREADCREATE:
        rc = R_OS_Prv_ThreadCreate(Par1, (R_OS_ThreadFunc_t)Par2, Par3);
        break;
    case R_OS_SYSCALL_THREADSLEEP:
        R_OS_Prv_ThreadSleep((uint32_t)Par1);
        rc = 0;
        break;
    case R_OS_SYSCALL_THREADEXIT:
        R_OS_Prv_ThreadExit(Par1);
        rc = 0;
        break;
    case R_OS_SYSCALL_THREADJOIN:
        rc = R_OS_Prv_ThreadJoin((R_OS_Thread_t)Par1, Par2);
        break;
    case R_OS_SYSCALL_THREADSELF:
        rc = R_OS_Prv_ThreadSelf();
        break;
    case R_OS_SYSCALL_OSSTART:
        R_OS_Prv_Start();
        rc = 0;
        break;
    case R_OS_SYSCALL_MUTEXINIT:
        rc = R_OS_Prv_ThreadMutexInit(Par1, Par2);
        break;
    case R_OS_SYSCALL_MUTEXDESTROY:
        rc = R_OS_Prv_ThreadMutexDestroy(Par1);
        break;
    case R_OS_SYSCALL_MUTEXLOCK:
        rc = R_OS_Prv_ThreadMutexLock(Par1);
        break;
    case R_OS_SYSCALL_MUTEXUNLOCK:
        rc = R_OS_Prv_ThreadMutexUnlock(Par1);
        break;
    case R_OS_SYSCALL_CONDINIT:
        rc = R_OS_Prv_ThreadCondInit(Par1, Par2);
        break;
    case R_OS_SYSCALL_CONDDESTROY:
        rc = R_OS_Prv_ThreadCondDestroy(Par1);
        break;
    case R_OS_SYSCALL_CONDWAIT:
        rc = R_OS_Prv_ThreadCondWait(Par1, Par2);
        break;
    case R_OS_SYSCALL_CONDSIGNAL:
        rc = R_OS_Prv_ThreadCondSignal(Par1);
        break;
    case R_OS_SYSCALL_SEMINIT:
        rc = R_OS_Prv_SemInit(Par1, (int32_t)Par2, (uint32_t)Par3);
        break;
    case R_OS_SYSCALL_SEMDESTROY:
        rc = R_OS_Prv_SemDestroy(Par1);
        break;
    case R_OS_SYSCALL_SEMWAIT:
        rc = R_OS_Prv_SemWait(Par1);
        break;
    case R_OS_SYSCALL_SEMTRYWAIT:
        rc = R_OS_Prv_SemTryWait(Par1);
        break;
    case R_OS_SYSCALL_SEMPOST:
        rc = R_OS_Prv_SemPost(Par1);
        break;
    case R_OS_SYSCALL_SEMGETVALUE:
        rc = R_OS_Prv_SemGetValue(Par1, Par2);
        break;
    case R_OS_SYSCALL_GETTIMEMS:
        rc = R_OS_Prv_GetTimeMs();
        break;
    default:
        rc = -1;
        break;
    }
    return rc;
}
