/*
****************************************************************************
PROJECT : eeOS2
FILE    : $Id: r_os_prv_tick.c 11006 2016-11-07 16:32:44Z michael.golczewski $
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
#include "r_os_asm_switch.h"
#include "r_os_prv_main.h"
#include "r_os_prv_tick.h"

/*******************************************************************************
  Section: Local Defines
*/

/*******************************************************************************
  Define: _INT_MAX

  Maximum value of an integer (32-bit).
*/

#define LOC_ROS_INT_MAX 0x7fffffff

/*******************************************************************************
  Section: Local Variables
*/

/*******************************************************************************
  Variable: _SysTime

  Current system time.
*/

static int32_t _SysTime;

/*******************************************************************************
  Variable: _TimeEvent

  Time for next time triggered action.
*/

static int32_t _TimeEvent = LOC_ROS_INT_MAX;


/*******************************************************************************
  Section: Local Functions
*/

/*******************************************************************************
  Function: _CheckTimeEvent
*/
static int32_t _CheckTimeEvent(void)
{
    R_OS_ThreadData_t *thread;
    int32_t               tdiff;
    int32_t               mindiff;
    int32_t               activate;

    activate = 0;
    mindiff  = LOC_ROS_INT_MAX;
    thread  = R_OS_ThreadRoot;
    while (thread != 0)
    {
        if (thread->State == R_OS_TSTATE_SLEEPING)
        {
            tdiff = _SysTime - thread->Data.Time;
            if (tdiff >= 0)
            {
                thread->State = R_OS_TSTATE_READY;
                activate = 1;
            }
            else
            {
                tdiff = 0 - tdiff;
                if (tdiff < mindiff)
                {
                    mindiff = tdiff;
                }
            }
        }
        thread = thread->Next;
    }
    _TimeEvent = _SysTime + mindiff;
    return activate;
}


/*******************************************************************************
  Function: _Sleep
*/
static void _Sleep(void)
{
    int32_t tdiffa;
    int32_t tdiffb;

    R_OS_ThreadCurrent->State = R_OS_TSTATE_SLEEPING;
    tdiffa = _SysTime - _TimeEvent;
    tdiffb = _SysTime - R_OS_ThreadCurrent->Data.Time;
    if (tdiffa < tdiffb)
    {
        _TimeEvent = R_OS_ThreadCurrent->Data.Time;
    }
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
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_OS_Prv_GetTimeMs
*/

int32_t R_OS_Prv_GetTimeMs(void)
{
    return _SysTime;
}


/*******************************************************************************
  Function: R_OS_Prv_ThreadSleep
*/

void R_OS_Prv_ThreadSleep(uint32_t Time)
{
    R_OS_ThreadCurrent->Data.Time = _SysTime + Time;
    _Sleep();
}


/*******************************************************************************
  Section: Global API Functions
*/

/*******************************************************************************
  Function: R_OS_Prv_Tick
*/

void R_OS_Prv_Tick(void)
{
    int32_t tdiff;

    _SysTime++;
    tdiff = _SysTime - _TimeEvent;
    if (tdiff >= 0)
    {
        tdiff = _CheckTimeEvent();
        if (tdiff != 0)
        {
            R_OS_Asm_ThreadSwitchPreemptive();
        }
    }
}

