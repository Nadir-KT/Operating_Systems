/*
****************************************************************************
PROJECT : eeOS2
FILE    : $Id: r_os_mutex.c 11006 2016-11-07 16:32:44Z michael.golczewski $
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


/*******************************************************************************
  Section: Global API Functions
*/

/*******************************************************************************
  Function: R_OS_ThreadMutexInit
*/

int32_t R_OS_ThreadMutexInit(R_OS_ThreadMutex_t *Mutex,
                                const R_OS_ThreadMutexAttr_t *Attr)
{
    int32_t rc;

    rc = R_OS_SysCall(R_OS_SYSCALL_MUTEXINIT, Mutex, (void*)Attr, 0);
    return rc;
}

/*******************************************************************************
  Function: R_OS_ThreadMutexDestroy
*/

int32_t R_OS_ThreadMutexDestroy(R_OS_ThreadMutex_t *Mutex)
{
    int32_t rc;

    rc = R_OS_SysCall(R_OS_SYSCALL_MUTEXDESTROY, Mutex, 0, 0);
    return rc;
}

/*******************************************************************************
  Function: R_OS_ThreadMutexLock
*/

int32_t R_OS_ThreadMutexLock(R_OS_ThreadMutex_t *Mutex)
{
    int32_t rc;

    rc = R_OS_SysCall(R_OS_SYSCALL_MUTEXLOCK, Mutex, 0, 0);
    return rc;
}

/*******************************************************************************
  Function: R_OS_ThreadMutexUnlock
*/

int32_t R_OS_ThreadMutexUnlock(R_OS_ThreadMutex_t *Mutex)
{
    int32_t rc;

    rc = R_OS_SysCall(R_OS_SYSCALL_MUTEXUNLOCK, Mutex, 0, 0);
    return rc;
}

