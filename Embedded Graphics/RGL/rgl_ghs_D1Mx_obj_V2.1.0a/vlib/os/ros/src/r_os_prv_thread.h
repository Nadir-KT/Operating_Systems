/*
****************************************************************************
PROJECT : ROS
FILE    : $Id: r_os_prv_thread.h 11006 2016-11-07 16:32:44Z michael.golczewski $
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

#ifndef R_OS_PRV_THREAD_H_
#define R_OS_PRV_THREAD_H_

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
  Section: Local Functions
*/

/*******************************************************************************
  Function: _AllocThreadData

  Allocate a thread data object from the pool.

  Parameters:
  void

  Returns:
  ==0 - No object allocated
  !=0 - Pointer to a thread data object
*/
static R_OS_ThreadData_t *_AllocThreadData(void);

/*******************************************************************************
  Function: _IsStackInUse

  Check for thread in use.

  The function checks whether the specfied stack base address
  is already in use by another thread.

  Parameters:
  Addr       - Pointer to the stack area

  Returns:
  ==0 - Stack us not in use
  !=0 - Stack is already used
*/
static int32_t _IsStackInUse(void *Addr);

/*******************************************************************************
  Function: _AllocThreadStack

  Allocate stack from the stack pool.

  The memory pool management is quite simple. Therefore
  it will only work correctly, if the requested stack
  size is always '_DEFAULT_STACKSIZE'.

  Parameters:
  Size       - Stack size; has to be '_DEFAULT_STACKSIZE'

  Returns:
  ==0 - Unable to allocate a stack
  !=0 - Pointer to a stack area
*/
static void *_AllocThreadStack(uint32_t Size);

/*******************************************************************************
  Function: _InsertThread

  Place a thread into the thread queue.

  The function assumes a priority ordered thread queue and
  will insert the new thread into this queue.

  Parameters:
  Thread     - Pointer to a thread data object

  Returns:
  void
*/

static void _InsertThread(R_OS_ThreadData_t *Thread);

/*******************************************************************************
  Function: _DeleteThread

  Remove a thread into the thread queue.

  The function assumes, that it has exclusive access to
  the thread queue.

  Parameters:
  Thread     - Pointer to a thread data object

  Returns:
  void
*/

static void _DeleteThread(R_OS_ThreadData_t *Thread);

/*******************************************************************************
  Function: _SetupStack

  Setup a default stack.

  If no stack is specified when a thread is created, this
  function is used to setup a default stack.

  Parameters:
  Attr          - Pointer to thread attribues

  Returns:
  void
*/

static void _SetupStack(R_OS_ThreadAttr_t *Attr);

/*******************************************************************************
  Function: _SetupDefaultAttr

  Setup default thread attributes including the stack.

  If no thread attributes are specified during thread
  creation, this function is used to created default
  attributes.

  Parameters:
  Attr          - Pointer to thread attribues

  Returns:
  void
*/

static void _SetupDefaultAttr(R_OS_ThreadAttr_t *Attr);

/*******************************************************************************
  Function: _CheckJoinEvent

  Check for thread exit events.

  If a thread exits, this function is called.
  The function checks whether any thread was waiting for
  the specified thread to exit and will put in in ready
  state; only the first thread found is activated.

  Parameters:
  Thread      - Pointer to a thread object
  RetVal      - Return value for any R_OS_ThreadJoin

  Returns:
  ==0 - No thread has been set to ready state
  !=0 - A thread has been set to ready state
*/

static int32_t _CheckJoinEvent(R_OS_ThreadData_t *Thread, void *RetVal);



/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_OS_Prv_ThreadCreate

  Create a thread.

  This function is called by the syscall function to execute
  the thread creation.

  Parameters:
  Param         - Thread creation parameter
  StartFunction - Function to call after creation of the thread
  Arg           - Argument for the 'StartFunction'

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_Prv_ThreadCreate(R_OS_ThreadCreateParam_t *Param,
                          R_OS_ThreadFunc_t StartFunction, void *Arg);


/*******************************************************************************
  Function: R_OS_Prv_ThreadExit

  Terminate a thread.

  The function is called by the syscall function to execute
  the thread temination.

  Parameters:
  RetVal        - Pointer to return value

  Returns:
  void
*/

void R_OS_Prv_ThreadExit(void *RetVal); 


/*******************************************************************************
  Function: R_OS_Prv_ThreadJoin

  Wait for thread termination.

  The function is called by the syscall function to execute
  the thread joining.

  Parameters:
  Thread        - Thread structure
  ThreadReturn  - tbd

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_Prv_ThreadJoin(R_OS_Thread_t Thread, void **ThreadReturn);


/*******************************************************************************
  Function: R_OS_Prv_ThreadSelf

  Returns the thread id of the calling thread.

  The function is called by the syscall function to return
  the thread id of the calling thread.

  Parameters:

  Returns:
  The thread id of the calling thread.
*/

R_OS_Thread_t R_OS_Prv_ThreadSelf(void); 


#ifdef __cplusplus
}
#endif

#endif  /* R_OS_PRV_THREAD_H_ */
