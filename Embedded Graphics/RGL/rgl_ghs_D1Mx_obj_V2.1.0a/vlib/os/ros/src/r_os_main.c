/*
****************************************************************************
PROJECT : ROS2
FILE    : $Id: r_os_main.c 11006 2016-11-07 16:32:44Z michael.golczewski $
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


/*******************************************************************************
  Section: Global API Functions
*/

/*******************************************************************************
  Function: R_OS_Init
*/

int32_t R_OS_Init(R_OS_ThreadData_t *Pool, uint32_t PoolSize,
                     void *Stack, uint32_t StackSize)
{
    R_OS_ThreadPool      = Pool;
    R_OS_ThreadPoolSize  = PoolSize;
    R_OS_ThreadStack     = Stack;
    R_OS_ThreadStackSize = StackSize;
    R_OS_ThreadCurrent   = 0;
    return 0;
}


/*******************************************************************************
  Function: R_OS_Start
*/

void R_OS_Start(void)
{
    R_OS_SysCall(R_OS_SYSCALL_OSSTART, 0, 0, 0);
}


/*******************************************************************************
  Function: R_OS_GetTimeMs
*/

int32_t R_OS_GetTimeMs(void)
{
    int32_t x;

    x = R_OS_SysCall(R_OS_SYSCALL_GETTIMEMS, 0, 0, 0);
    return x;
}

