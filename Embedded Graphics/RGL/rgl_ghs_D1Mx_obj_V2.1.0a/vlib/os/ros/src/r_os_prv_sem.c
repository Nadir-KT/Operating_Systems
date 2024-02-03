/*
****************************************************************************
PROJECT : eeOS2
FILE    : $Id: r_os_prv_sem.c 11006 2016-11-07 16:32:44Z michael.golczewski $
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
#include "r_os_prv_sem.h"


/*******************************************************************************
  Section: Local Functions
*/


static int32_t _CheckSemEvent(R_OS_Sem_t *Sem)
{
    R_OS_ThreadData_t *thread;

    thread  = R_OS_ThreadRoot;
    while (thread != 0)
    {
        if (thread->State == R_OS_TSTATE_WAITING)
        {
            if (thread->Data.Resource == Sem)
            {
                Sem->Value--;
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
  Function: R_OS_Prv_SemInit
*/

int32_t R_OS_Prv_SemInit(R_OS_Sem_t *Sem, int32_t PShared, uint32_t Value)
{
    Sem->Value = Value;
    return 0;
}


/*******************************************************************************
  Function: R_OS_Prv_SemDestroy 
*/

int32_t R_OS_Prv_SemDestroy(R_OS_Sem_t *Sem)
{
    return 0;
}


/*******************************************************************************
  Function: R_OS_Prv_SemWait
*/

int32_t R_OS_Prv_SemWait(R_OS_Sem_t *Sem)
{
    if (Sem->Value > 0)
    {
        Sem->Value--;
        return 0;
    }
    R_OS_Prv_ThreadWait(R_OS_TSTATE_WAITING, Sem);
    return 0;
}


/*******************************************************************************
  Function: R_OS_Prv_SemTryWait
*/

int32_t R_OS_Prv_SemTryWait(R_OS_Sem_t *Sem)
{
    if (Sem->Value > 0)
    {
        Sem->Value--;
        return 0;
    }
    return -1;
}


/*******************************************************************************
  Function: R_OS_Prv_SemPost
*/

int32_t R_OS_Prv_SemPost(R_OS_Sem_t *Sem)
{
    int32_t change;

    Sem->Value++;
    change = _CheckSemEvent(Sem);
    if (change != 0)
    {
        R_OS_Prv_ThreadSchedule();
    }
    return 0;
}


/*******************************************************************************
  Function: R_OS_Prv_SemGetValue
*/

int32_t R_OS_Prv_SemGetValue(R_OS_Sem_t *Sem, int32_t *Value)
{
    *Value = Sem->Value;
    return 0;
}

