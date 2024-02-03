/*
****************************************************************************
PROJECT : ROS2
FILE    : $Id: r_os_api.h 11006 2016-11-07 16:32:44Z michael.golczewski $
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
  Title: ROS Functions

  An application using the ROS as operating system must include this
  file
*/


#ifndef R_OS_API_H_
#define R_OS_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Title: ROS

  Renesas Embedded Operating System
*/


/*******************************************************************************
  Section: Global API Types

  Definition of global data types
*/

/*******************************************************************************
  Type: R_OS_Thread_t

  Thread identifier.
*/

typedef uint32_t R_OS_Thread_t;


/*******************************************************************************
  Type: R_OS_ThreadSchedParam_t

  Scheduling parameter type.

  Members:
  SchedPriority - Higher number means higher prio
*/

typedef struct {
    uint32_t SchedPriority;
} R_OS_ThreadSchedParam_t;


/*******************************************************************************
  Type: R_OS_ThreadAttr_t

  Thread attributes.

  Members:
  IsInitialized - Flag is 1 if the structure is initialized
  StackAddr     - Address of the stack
  StackSize     - Size of the stack
  SchedParam    - Scheduling parameter
*/

typedef struct R_OS_ThreadAttr_s {
    uint32_t                     IsInitialized;
    void                    *StackAddr;
    uint32_t                     StackSize;
    R_OS_ThreadSchedParam_t  SchedParam;
} R_OS_ThreadAttr_t;


/*******************************************************************************
  Enum: R_OS_ThreadState_t

  Thread state definition.

>
>             NEW
>              |
>              V
>      +---> READY ---+
>      |       ^      |
>      |       |      V
>   WAITING <--+-- RUNNING
>                     |
>      :              V
>   SUSPENDED       DEAD
> 

  Values:
  R_OS_TSTATE_FREE        - Not existing
  R_OS_TSTATE_NEW         - Created, but not started
  R_OS_TSTATE_READY       - Ready for execution, but not running
  R_OS_TSTATE_WAITING     - Waiting for mutex or condition
  R_OS_TSTATE_RUNNING     - Executing
  R_OS_TSTATE_SUSPENDED   - Supsended
  R_OS_TSTATE_DEAD        - Killed
  R_OS_TSTATE_SLEEPING    - Sleeping; this is not a PTHREAD state
*/

typedef enum {
    R_OS_TSTATE_FREE,
    R_OS_TSTATE_NEW,
    R_OS_TSTATE_READY,
    R_OS_TSTATE_WAITING,
    R_OS_TSTATE_RUNNING,
    R_OS_TSTATE_SUSPENDED,
    R_OS_TSTATE_DEAD,
    R_OS_TSTATE_SLEEPING    /* This is not a PTHREAD state */
} R_OS_ThreadState_t;


/*******************************************************************************
  Type: R_OS_ThreadData_t

  Thread structure.
  This type does not exist in the PTHREAD definition. It
  is used to store the data of a thread. In PTRHEAD functions
  only a thread ID is used.

  Members:
  Stack       - Current stack pointer
  StackBase   - Base address of the stack
  StackSize   - Size of the stack
  ThreadId    - ID of the thread (used for API calls)
  State       - Current state of the thread
  Priority    - Thread priority
  Data        - Pointer to requested resoucre or wakeup time
  MemMap      - not used
  Next        - Pointer to next thread
*/

typedef struct R_OS_ThreadData_s {
    void                       *Stack;
    void                       *StackBase;
    uint32_t                    StackSize;
    R_OS_Thread_t               ThreadId;
    R_OS_ThreadState_t          State;
    uint32_t                    Priority;
    union {
        uint32_t   Time;
        void      *Resource;
    } Data;
    void                       *MemMap;
    struct  R_OS_ThreadData_s  *Next;
} R_OS_ThreadData_t;


/*******************************************************************************
  Type: R_OS_ThreadMutexAttr_t

  Mutex attribute type.

  Members:
  IsInitialized - tbd
  Recursive     - tbd
*/

typedef struct {
  uint32_t  IsInitialized;
  uint32_t  Recursive;
} R_OS_ThreadMutexAttr_t;


/*******************************************************************************
  Type: R_OS_ThreadMutex_t

  Mutex type.

  Members:
  Lock    - 0 == ulocked, 1 == locked
*/

typedef struct {
    uint8_t Lock;
} R_OS_ThreadMutex_t;


/*******************************************************************************
  Type: R_OS_ThreadCondAttr_t

  Condition variable attribute type.

  Members:
  tbd
*/

typedef struct {
    int32_t Dummy;
} R_OS_ThreadCondAttr_t;


/*******************************************************************************
  Type: R_OS_ThreadCond_t

  Condition variable type.

  Members:
    Mutex - pointer to a <R_OS_ThreadMutex_t>
*/

typedef struct {
    R_OS_ThreadMutex_t *Mutex;
} R_OS_ThreadCond_t;


/*******************************************************************************
  Type: R_OS_Sem_t

  Semaphore type.

  Members:
    Value - tbd
*/

typedef struct {
    int32_t Value;
} R_OS_Sem_t;



typedef void *(*R_OS_ThreadFunc_t)(void* Arg);

/*******************************************************************************
  Section: Global API Functions
*/

/*******************************************************************************
  Function: R_OS_Init

  Inilialize the OS.

  Parameters:
  Pool          - Pointer to an thread data array
  PoolSize      - Number of elements in the thread data array
  Stack         - Memory area to be used as thread stack
  StackSize     - Byte size of the thread stack

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_Init(R_OS_ThreadData_t *Pool, uint32_t PoolSize,
                     void *Stack, uint32_t StackSize); 


/*******************************************************************************
  Function: R_OS_Start

  Start the scheduler.
  This is usually called from the main function of the program.
  The stack of the caller function will be destroyed; e.g.
  auto variables of this function will be removed. The function
  will also not return, even if there is no thread existing.

  Parameters:
  void

  Returns:
  void
*/

void R_OS_Start(void); 


/*******************************************************************************
  Function: R_OS_GetTimeMs

  Return current system time in ms.

  Parameters:
  void

  Returns:
  Current systemt time
*/

int32_t R_OS_GetTimeMs(void);


/*******************************************************************************
  Function: R_OS_ThreadAttrInit

  Initialize the thread attributes object.

  Parameters:
  Attr          - Pointer to thread attribues

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_ThreadAttrInit(R_OS_ThreadAttr_t *Attr);


/*******************************************************************************
  Function: R_OS_ThreadAttrSetStackAddr

  Set address of the stack.

  Parameters:
  Attr       - Pointer to thread attribues
  Addr       - Address of stack memory

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_ThreadAttrSetStackAddr(R_OS_ThreadAttr_t *Attr, void *Addr);


/*******************************************************************************
  Function: R_OS_ThreadAttrSetStackSize

  Set size of the stack.

  Parameters:
  Attr       - Pointer to thread attribues <R_OS_ThreadAttr_t>
  Size       - Size of the stack memory

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_ThreadAttrSetStackSize(R_OS_ThreadAttr_t *Attr, uint32_t Size);


/*******************************************************************************
  Function: R_OS_ThreadAttrSetSchedParam

  Set scheduling parameters.

  Parameters:
  Attr       - Pointer to thread attribues <R_OS_ThreadAttr_t>
  SchedParam - Scheduling parameter, see: <R_OS_ThreadSchedParam_t>

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_ThreadAttrSetSchedParam(R_OS_ThreadAttr_t *Attr,
                                        const R_OS_ThreadSchedParam_t *SchedParam);


/*******************************************************************************
  Function: R_OS_ThreadCreate

  Create a thread.

  Parameters:
  Thread        - Pointer to a thread structure
  Attr          - Pointer to thread attribues or zero for default 
  StartFunction - Function to call after creation of the thread
  Arg           - Argument for the 'StartFunction'

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_ThreadCreate(R_OS_Thread_t *Thread, R_OS_ThreadAttr_t *Attr,
                      R_OS_ThreadFunc_t StartFunction, void *Arg); 


/*******************************************************************************
  Function: R_OS_ThreadExit

  Terminate a thread.

  The function will terminate the calling thread and not
  return to the calling function.

  Parameters:
  RetVal        - Pointer to return value

  Returns:
  void
*/

void R_OS_ThreadExit(void *RetVal); 


/*******************************************************************************
  Function: R_OS_ThreadJoin

  Wait for thread termination.

  Parameters:
  Thread        - Thread structure
  ThreadReturn  - Pointer to a storage for the return value

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_ThreadJoin(R_OS_Thread_t Thread, void **ThreadReturn);


/*******************************************************************************
  Function: R_OS_ThreadSleep

  Suspend a thread for specified time.

  The calling thread is suspended for at least 'Time' ticks.
  This function does not exist in the PTHREAD definition.

  Parameters:
  Time          - Time in ticks

  Returns:
  void
*/

void R_OS_ThreadSleep(uint32_t Time); 


/*******************************************************************************
  Function: R_OS_ThreadSelf

  Returns the thread id of the calling thread.

  Parameters:
  void

  Returns:
  The thread id <R_OS_Thread_t> of the calling thread.
*/

R_OS_Thread_t R_OS_ThreadSelf(void); 


/*******************************************************************************
  Function: R_OS_ThreadMutexInit

  Initialize a thread mutex object.

  Parameters:
  Mutex      - Pointer to a mutex object
  Attr       - Pointer to a mutex attributes type or 0

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_ThreadMutexInit(R_OS_ThreadMutex_t *Mutex,
                                const R_OS_ThreadMutexAttr_t *Attr);


/*******************************************************************************
  Function: R_OS_ThreadMutexDestroy

  Destroy a thread mutex object.

  Parameters:
  Mutex      - Pointer to a mutex object

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_ThreadMutexDestroy(R_OS_ThreadMutex_t *Mutex);


/*******************************************************************************
  Function: R_OS_ThreadMutexLock

  Lock a thread mutex object.

  Parameters:
  Mutex      - Pointer to a mutex object

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_ThreadMutexLock(R_OS_ThreadMutex_t *Mutex);


/*******************************************************************************
  Function: R_OS_ThreadMutexUnlock

  Unlock a thread mutex object.

  Parameters:
  Mutex      - Pointer to a mutex object

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_ThreadMutexUnlock(R_OS_ThreadMutex_t *Mutex);


/*******************************************************************************
  Function: R_OS_ThreadCondInit

  Initialize a condition variable object.

  Parameters:
  Cond       - Pointer to a condition variable object
  Attr       - Condition variable attribute object or 0

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_ThreadCondInit(R_OS_ThreadCond_t *Cond,
                               const R_OS_ThreadCondAttr_t *Attr);


/*******************************************************************************
  Function: R_OS_ThreadCondSignal

  Signal a condition.

  Parameters:
  Cond       - Pointer to a condition variable object

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_ThreadCondSignal(R_OS_ThreadCond_t *Cond);


/*******************************************************************************
  Function: R_OS_ThreadCondWait

  Wair for a condition.

  Parameters:
  Cond       - Pointer to a condition variable object

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_ThreadCondWait(R_OS_ThreadCond_t *Cond, R_OS_ThreadMutex_t *Mutex);


/*******************************************************************************
  Function: R_OS_ThreadCondDestroy

  Destroy a condition variable object.

  Parameters:
  Cond       - Pointer to a condition variable object

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_ThreadCondDestroy(R_OS_ThreadCond_t *Cond);


/*******************************************************************************
  Function: R_OS_SemInit

  Initialise the unnamed semaphore at the specified address.

  Parameters:
  Sem        - Location of the unnamed semaphore
  PShared    - ==0 shared between threads, 
               !=0 shared between processes (not supported yet)
  Value      - Initial value of the semaphore 

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_SemInit(R_OS_Sem_t *Sem, int32_t PShared, uint32_t Value);


/*******************************************************************************
  Function: R_OS_SemDestroy 

  Destroy an unnamed semaphore.

  Parameters:
  Sem        - Location of the unnamed semaphore

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_SemDestroy(R_OS_Sem_t *Sem);


/*******************************************************************************
  Function: R_OS_SemWait

  Wait for an unnamed semaphore.

  Parameters:
  Sem        - Location of the unnamed semaphore

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_SemWait(R_OS_Sem_t *Sem);


/*******************************************************************************
  Function: R_OS_SemTryWait

  Try to decrement an unnamed semaphore.
  If the operation cannot be executed immediately, the
  function will not switch the calling thread into 
  R_OS_TSTATE_WAITING.   

  Parameters:
  Sem        - Location of the unnamed semaphore

  Returns:
  ==0 - OK
  !=0 - NG, operation could not be executed
*/

int32_t R_OS_SemTryWait(R_OS_Sem_t *Sem);


/*******************************************************************************
  Function: R_OS_SemPost

  Unlock an unnamed semaphore.

  Parameters:
  Sem        - Location of the unnamed semaphore

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_SemPost(R_OS_Sem_t *Sem);


/*******************************************************************************
  Function: R_OS_SemGetValue

  Get the current semaphore value.

  Parameters:
  Sem        - Location of the unnamed semaphore
  Value      - Pointer to the location to store the value

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_SemGetValue(R_OS_Sem_t *Sem, int32_t *Value);


/*******************************************************************************
  Function: R_OS_Prv_Tick

  ROS timer tick function.

  An application has to call this function from within the
  ISR function of its timer. The function assumes execution
  in the privileged mode of the CPU.

  Parameters:
  void

  Returns:
  void
*/

void R_OS_Prv_Tick(void);


/*******************************************************************************
  Function: R_OS_Prv_EnterInterrupt

  Signal activation of an ISR to the OS.

  If the applications wants to use OS calls from inside of
  an ISR, it has to inform the OS by call of this function,
  before calling other OS functions. Any thread switch will
  be postponed until the call of R_OS_Prv_LeaveInterrupt.

  Parameters:
  void

  Returns:
  void
*/

void R_OS_Prv_EnterInterrupt(void);


/*******************************************************************************
  Function: R_OS_Prv_LeaveInterrupt

  Signal the end of an ISR to the OS.

  Any pending thread switch will be executed; see:
  R_OS_Prv_EnterInterrupt.

  Parameters:
  void

  Returns:
  void
*/

void R_OS_Prv_LeaveInterrupt(void);


#ifdef __cplusplus
}
#endif

#endif /* R_OS_API_H_ */
