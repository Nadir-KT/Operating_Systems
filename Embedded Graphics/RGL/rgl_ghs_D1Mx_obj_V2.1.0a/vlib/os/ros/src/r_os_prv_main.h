/*
****************************************************************************
PROJECT : ROS
FILE    : $Id: r_os_prv_main.h 11006 2016-11-07 16:32:44Z michael.golczewski $
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

#ifndef R_OS_PRV_MAIN_H_
#define R_OS_PRV_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Section: Global Defines
*/

#undef LOC_ROS_EXTERN_
#ifndef R_OS_PRV_MAIN_GLOBALS_
#define LOC_ROS_EXTERN_ extern
#else
#define LOC_ROS_EXTERN_
#endif

/*******************************************************************************
  Section: Global Variables
*/

/*******************************************************************************
  Variable: R_OS_ThreadPool

  Pointer to the thread data pool.

  When a thread is created, this pool is used to keep the
  thread's internal data. The pool is provided during OS
  startup by the application.
*/

LOC_ROS_EXTERN_ R_OS_ThreadData_t *R_OS_ThreadPool;


/*******************************************************************************
  Variable: R_OS_ThreadPoolSize

  Size of the data pool.

  This is the number of elements in the thread pool.
  See: <R_OS_ThreadPool>
*/

LOC_ROS_EXTERN_ uint32_t R_OS_ThreadPoolSize;


/*******************************************************************************
  Variable: R_OS_ThreadStack

  Pointer to the thread stack pool.

  If a thread does not specify a stack during its creation,
  the OS allocates the stack in this pool. The pool is
  provided by the application during startup of the OS.
*/

LOC_ROS_EXTERN_ void *R_OS_ThreadStack;


/*******************************************************************************
  Variable: R_OS_ThreadStackSize

  Size of the stack pool.

  This is the number of bytes in the thread stack pool.
  See: <R_OS_ThreadStack>
*/

LOC_ROS_EXTERN_ uint32_t R_OS_ThreadStackSize;


/*******************************************************************************
  Variable: R_OS_ThreadCurrent

  Pointer to the current thread.

  This pointer points to the currently running thread.
  If the value is zero, the idle loop is running.
*/

LOC_ROS_EXTERN_ R_OS_ThreadData_t *R_OS_ThreadCurrent;


/*******************************************************************************
  Variable: R_OS_ThreadRoot

  Pointer to the first thread.

  The OS manages all threads in a queue. The threads are
  sorted by priority within that queue. This variable is
  a pointer to the first thread of the queue.
*/

LOC_ROS_EXTERN_ R_OS_ThreadData_t *R_OS_ThreadRoot;


/*******************************************************************************
  Variable: R_OS_CriticalRegion

  Critical region indicator.

  This counter indicates, that the scheduler must not execute
  a thread switch immediately, because the execution is within
  a critical region. For example an ISR will indicate a critical
  region to ensure, that the complete ISR is executed, before
  the scheduler activates a thread.
*/

LOC_ROS_EXTERN_ uint8_t R_OS_CriticalRegion;


/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_OS_Prv_Start

  Start the scheduler.

  Parameters:
  void

  Returns:
  void
*/

void R_OS_Prv_Start(void);


/*******************************************************************************
  Function: R_OS_Prv_ThreadWait

  Switch the current thread to wait state.

  The funtion is called to prepare are thread for waiting
  for the specified resource.

  Parameters:
  State      - See: <R_OS_ThreadState_t>
  Resource   - Specified a resource or a time

  Returns:
  void
*/

void R_OS_Prv_ThreadWait(R_OS_ThreadState_t State, void *Resource);


/*******************************************************************************
  Function: R_OS_Prv_ThreadChange

  Change the current thread.

  The funtion stops execution of the current state. Then it
  will activate the thread with the highest priority, which
  is ready for execution. The function is called by the
  assembler part of the OS.

  Parameters:
  void

  Returns:
  void
*/

void R_OS_Prv_ThreadChange(void);


/*******************************************************************************
  Function: R_OS_Prv_ThreadSwitchCooperative

  Execute a cooperative thread change.

  Save current thread status, change current thread and activate 
  it. If it is called from within a critical region (e.g. ISR),
  it will not execute the switch, but set a pending flag. The
  switch is executed when the critical region will be left.

  Parameters:
  void

  Returns:
  void
*/

void R_OS_Prv_ThreadSwitchCooperative(void);


/*******************************************************************************
  Function: R_OS_Prv_ThreadSchedule

  Execute a cooperative switch.

  The function sets the current process into ready state
  and calls the assembler scheduler. The function is used,
  if a resoucre becomes available and therefore a thread
  could be activated.

  Parameters:
  void

  Returns:
  void
*/

void R_OS_Prv_ThreadSchedule(void);


/*******************************************************************************
  Function: R_OS_Prv_EnterCriticalRegion

  Disable scheduler.

  Prevent potential task switches caused by OS calls.

  Parameters:
  void

  Returns:
  void
*/

void R_OS_Prv_EnterCriticalRegion(void);


/*******************************************************************************
  Function: R_OS_Prv_LeaveCriticalRegion

  Disable scheduler.

  Prevent potential task switches caused by OS calls.

  Parameters:
  void

  Returns:
  void
*/

void R_OS_Prv_LeaveCriticalRegion(void);


#ifdef __cplusplus
}
#endif

#endif  /* R_OS_PRV_MAIN_H_ */
