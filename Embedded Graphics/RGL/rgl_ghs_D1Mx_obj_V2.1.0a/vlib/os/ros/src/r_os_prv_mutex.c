/*
****************************************************************************
PROJECT : eeOS2
FILE    : $Id: r_os_prv_mutex.c 11006 2016-11-07 16:32:44Z michael.golczewski $
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
#include "r_os_prv_main.h"
#include "r_os_prv_mutex.h"


/*******************************************************************************
  Section: Local Functions
*/

static int32_t _CheckMutexEvent(R_OS_ThreadMutex_t *Mutex)
{
    R_OS_ThreadData_t *thread;

    thread  = R_OS_ThreadRoot;
    while (thread != 0)
    {
        if (thread->State == R_OS_TSTATE_WAITING)
        {
            if (thread->Data.Resource == Mutex)
            {
                Mutex->Lock   = 1;
                thread->State = R_OS_TSTATE_READY;
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
  Function: R_OS_Prv_ThreadMutexInit
*/

int32_t R_OS_Prv_ThreadMutexInit(R_OS_ThreadMutex_t *Mutex,
                             const R_OS_ThreadMutexAttr_t *Attr)
{
    int32_t rc;

    Mutex->Lock = 0;
    rc          = 0;
    return rc;
}

/*******************************************************************************
  Function: R_OS_Prv_ThreadMutexDestroy
*/

int32_t R_OS_Prv_ThreadMutexDestroy(R_OS_ThreadMutex_t *Mutex)
{
    int32_t rc;

    rc = 0;
    return rc;
}

/*******************************************************************************
  Function: R_OS_Prv_ThreadMutexLock
*/

int32_t R_OS_Prv_ThreadMutexLock(R_OS_ThreadMutex_t *Mutex)
{
    int32_t rc;

    if (Mutex->Lock != 0)
    {
        R_OS_Prv_ThreadWait(R_OS_TSTATE_WAITING, Mutex);
    }
    else
    {
        Mutex->Lock = 1;
    }
    rc = 0;
    return rc;
}

/*******************************************************************************
  Function: R_OS_Prv_ThreadMutexUnlock
*/

int32_t R_OS_Prv_ThreadMutexUnlock(R_OS_ThreadMutex_t *Mutex)
{
    int32_t change;

    if (Mutex->Lock == 0)
    {
        R_OS_Sys_Error(-1);
        return -1;
    }
    Mutex->Lock = 0;
    change = _CheckMutexEvent(Mutex);
    if (change != 0)
    {
        R_OS_Prv_ThreadSchedule();
    }
    return 0;
}

