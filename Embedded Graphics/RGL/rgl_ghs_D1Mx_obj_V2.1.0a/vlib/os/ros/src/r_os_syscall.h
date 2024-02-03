/*
****************************************************************************
PROJECT : eeOS2
FILE    : $Id: r_os_syscall.h 11006 2016-11-07 16:32:44Z michael.golczewski $
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

#ifndef R_OS_SYSCALL_H_
#define R_OS_SYSCALL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Section: Global Types

  Definition of global data types
*/

typedef enum {
    R_OS_SYSCALL_NOP,
    R_OS_SYSCALL_OSSTART,
    R_OS_SYSCALL_THREADCREATE,
    R_OS_SYSCALL_THREADEXIT,
    R_OS_SYSCALL_THREADJOIN,
    R_OS_SYSCALL_THREADSLEEP,
    R_OS_SYSCALL_THREADSELF,
    R_OS_SYSCALL_MUTEXINIT,
    R_OS_SYSCALL_MUTEXDESTROY,
    R_OS_SYSCALL_MUTEXLOCK,
    R_OS_SYSCALL_MUTEXUNLOCK,
    R_OS_SYSCALL_CONDINIT,
    R_OS_SYSCALL_CONDDESTROY,
    R_OS_SYSCALL_CONDWAIT,
    R_OS_SYSCALL_CONDSIGNAL,
    R_OS_SYSCALL_SEMINIT,
    R_OS_SYSCALL_SEMDESTROY,
    R_OS_SYSCALL_SEMWAIT,
    R_OS_SYSCALL_SEMTRYWAIT,
    R_OS_SYSCALL_SEMPOST,
    R_OS_SYSCALL_SEMGETVALUE,
    R_OS_SYSCALL_GETTIMEMS,

    R_OS_SYSCALL_GETPID,
    R_OS_SYSCALL_EXIT,
    R_OS_SYSCALL_WAITMS,
    R_OS_SYSCALL_WAITUNTIL,
    R_OS_SYSCALL_SEMCTL,
    R_OS_SYSCALL_SEMCREATE,
    R_OS_SYSCALL_SEMOP,
    R_OS_SYSCALL_SETPRIO,
    R_OS_SYSCALL_ENTERCRITICAL,
    R_OS_SYSCALL_LEAVECRITICAL,
    R_OS_SYSCALL_RBCREATE,
    R_OS_SYSCALL_RBRECEIVE,
    R_OS_SYSCALL_RBSEND,
    R_OS_SYSCALL_RBSENDNOWAIT,
    R_OS_SYSCALL_EVSIGNAL,
    R_OS_SYSCALL_EVCLEAR,
    R_OS_SYSCALL_EVWAIT,
    R_OS_SYSCALL_TICK  /* test only! */
} R_OS_SysCall_t;


/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_OS_SysCall

  Switch to supervisor mode and call 'eeOS_Prv_CSysCall'. 

  This function is usually written in assembly language.

  Parametes:
  Cmd     - System call command
  Par1    - Parameter 1
  Par2    - Parameter 2
  Par3    - Parameter 3

  Returns:
  Return value depends on command
*/

int32_t R_OS_SysCall(R_OS_SysCall_t Cmd, void *Par1,
                 void *Par2, void *Par3);


/*******************************************************************************
  Function: R_OS_Prv_CSysCall

  Execute OS call.

  This funtion is called by 'eeOS_SysCall' after switching 
  to supervisor mode.

  Parameters:
  Cmd     - System call command
  Par1    - Parameter 1
  Par2    - Parameter 2
  Par3    - Parameter 3

  Returns:
  Return value depends on command
*/

int32_t R_OS_Prv_CSysCall(R_OS_SysCall_t Cmd, void *Par1,
                      void *Par2, void *Par3);

#ifdef __cplusplus
}
#endif

#endif  /* R_OS_SYSCALL_H_ */
