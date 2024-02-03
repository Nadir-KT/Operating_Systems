/*
****************************************************************************
PROJECT : ROS
FILE    : $Id: r_os_prv_main.c 17131 2020-02-26 02:36:07Z shinya.tomari.wj@renesas.com $
AUTHOR  : $Author: shinya.tomari.wj@renesas.com $
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
#define R_OS_PRV_MAIN_GLOBALS_
#include "r_os_prv_main.h"
#include "r_os_asm_switch.h"


/*******************************************************************************
  Section: Local Variables
*/


/*******************************************************************************
  Variable: _SwitchPending

  Flag to indicate a pending thread switch.

  When the system is in a critical region, thread switches
  are not immediately executed. This flag is set and it
  is checked when a critical region is left.
*/

static uint8_t _SwitchPending;


/*******************************************************************************
  Variable: _InterruptLevel

  Count the interrupt nesting level.

  When the system is withing an ISR, a triggered switch
  will not be executed before all ISR are completed. This
  variable indicates the current interrupt level.
*/

static uint8_t _InterruptLevel;


/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_OS_Prv_ThreadWait
*/

void R_OS_Prv_ThreadWait(R_OS_ThreadState_t State, void *Resource)
{
    R_OS_ThreadCurrent->State         = State;
    R_OS_ThreadCurrent->Data.Resource = Resource;
    if (R_OS_CriticalRegion != 0)
    {
        R_OS_Sys_Error(-1);
    }
    else
    {
        R_OS_Asm_ThreadSwitchCooperative();
    }
}

/*******************************************************************************
  Function: R_OS_Prv_ThreadChange
*/

void R_OS_Prv_ThreadChange(void)
{
    R_OS_ThreadData_t *thread;

    /* Deactivate current running thread */
    if (R_OS_ThreadCurrent != 0)
    {
        if (R_OS_ThreadCurrent->State == R_OS_TSTATE_RUNNING)
        {
            R_OS_ThreadCurrent->State = R_OS_TSTATE_READY;
        }
    }

    /* Search 1st thread in list with status PS_READY */
    /* $$$$: If N threads have the same prio and are
             READY, only the first thread in the least will
             be executed. To be PTHREAD compliant, support
             for such case needs to be added.
     */
    thread = R_OS_ThreadRoot;
    while (thread != 0)
    {
        if (thread->State == R_OS_TSTATE_READY)
        {
            break;
        }
        thread = thread->Next;
    }

    /* Activate new thread */
    R_OS_ThreadCurrent = thread;
    if (R_OS_ThreadCurrent != 0)
    {
        R_OS_ThreadCurrent->State = R_OS_TSTATE_RUNNING;
    }
}

/*******************************************************************************
  Function: R_OS_Prv_ThreadSwitchCooperative
*/

void R_OS_Prv_ThreadSwitchCooperative(void)
{
    if (R_OS_CriticalRegion != 0)
    {
        _SwitchPending = 1;
    }
    else
    {
        R_OS_Asm_ThreadSwitchCooperative();
    }
}

/*******************************************************************************
  Function: R_OS_Prv_ThreadSchedule
*/

void R_OS_Prv_ThreadSchedule(void)
{
    if (R_OS_ThreadCurrent != 0)
    {
        R_OS_ThreadCurrent->State = R_OS_TSTATE_READY;
    }
    
    R_OS_Prv_ThreadSwitchCooperative();
}


/*******************************************************************************
  Function: R_OS_Prv_EnterCriticalRegion
*/

void R_OS_Prv_EnterCriticalRegion(void)
{
    R_OS_CriticalRegion++;
}


/*******************************************************************************
  Function: R_OS_Prv_LeaveCriticalRegion
*/

void R_OS_Prv_LeaveCriticalRegion(void)
{
    if (R_OS_CriticalRegion != 0)
    {
        R_OS_CriticalRegion--;
        if (0 == _InterruptLevel)
        {
            if (0 == R_OS_CriticalRegion)
            {
                if (_SwitchPending != 0)
                {
                    _SwitchPending = 0;
                    R_OS_Asm_ThreadSwitchCooperative();
                }
            }
        }
    }
}


/*******************************************************************************
  Function: R_OS_Prv_Start
*/

void R_OS_Prv_Start(void)
{
    _InterruptLevel           = 0;
    _SwitchPending            = 0;
    R_OS_CriticalRegion       = 0;
    R_OS_ThreadCurrent        = R_OS_ThreadRoot;
    R_OS_ThreadCurrent->State = R_OS_TSTATE_RUNNING;
    R_OS_Asm_ThreadActiveStart();
}


/*******************************************************************************
  Section: Global API Functions
*/

/*******************************************************************************
  Function: R_OS_Prv_EnterInterrupt
*/

void R_OS_Prv_EnterInterrupt(void)
{
    if (0 == _InterruptLevel)
    {
        R_OS_Prv_EnterCriticalRegion();
    }
    _InterruptLevel++;
}


/*******************************************************************************
  Function: R_OS_Prv_LeaveInterrupt
*/

void R_OS_Prv_LeaveInterrupt(void)
{
    __DI();
    if (_InterruptLevel != 0)
    {
        _InterruptLevel--;
        if (0 == _InterruptLevel)
        {
            _InterruptLevel++;
            R_OS_Prv_LeaveCriticalRegion();
            _InterruptLevel--;
            if (0 == R_OS_CriticalRegion)
            {
                if (_SwitchPending != 0)
                {
                    _SwitchPending = 0;
                    R_OS_Asm_ThreadSwitchPreemptive();
                }
            }
        }
    }
}

