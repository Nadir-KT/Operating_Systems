/*
****************************************************************************
PROJECT : Drw2D driver
FILE    : $Id: r_drw2d_os.h 17258 2020-12-08 12:26:14Z shinya.tomari.wj@renesas.com $
============================================================================
DESCRIPTION
Driver for DRW2D
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2021
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

#ifndef R_DRW2D_OS_H_
#define R_DRW2D_OS_H_

#ifdef __cplusplus
extern "C" {
#endif


/* (note) Since the API must not use dynamic memory allocation we need to include the OS header 
          file(s) to reserve memory for OS objects (e.g. CRITICAL_SECTION).
 */
#if defined(R_DRW2D_OS_WIN32)
#include <windows.h>
#elif defined(R_DRW2D_OS_RENESAS)
#include <r_os_api.h>
#elif defined(R_DRW2D_OS_INTEGRITY)
/* Disable MISRA checks for GHS Integrity - it's already perfect by default ;) */
#if defined (__ghs__)
#pragma ghs startnomisra
#endif /* __ghs__ */
#include <INTEGRITY.h>
#include "r_typedefs.h"
#if defined (__ghs__)
#pragma ghs endnomisra
#endif
#elif defined(R_DRW2D_OS_NO_OS)
#else
#include <r_os_api.h>
#define R_DRW2D_OS_NO_OS
#endif



/***********************************************************
  Title: DRW2D OS abstraction

  DRW2D OS functions.

  Declaration of DRW2D OS functions.

  The generic DRW2D driver uses these functions. They have to be implemented
  within the driver library for a concrete device. (e.g. D1L, D1M)

  Support functions are functions that are not part of the driver itself 
  but they must be provided to integrate the driver on a particular board.
*/



/*******************************************************************************
  Section: Global Defines
*/


/***********************************************************
  Type: r_drw2d_Float_t

  DRW2D float type.
*/
#if defined(R_DRW2D_OS_INTEGRITY)
typedef float64_t r_drw2d_Float_t;
#else
typedef float32_t r_drw2d_Float_t;
#endif


/***********************************************************
  Enum: r_drw2d_OS_Error_t

  DRW2D OS abstraction layer error code.

  Values:
    R_DRW2D_OS_ERR_OK             - No error occurred.
    R_DRW2D_OS_ERR_NOT_SUPPORTED  - (Currently) unsupported feature
    R_DRW2D_OS_ERR_PARAM_NOT_SUPPORTED  - unsupported parameter
    R_DRW2D_OS_ERR_THREAD_CREATE  - Failed to create OS-specific thread
    R_DRW2D_OS_ERR_THREAD_INFO    - Invalid thread info
    R_DRW2D_OS_ERR_THREAD_HANDLE  - Invalid thread handle
    R_DRW2D_OS_ERR_THREAD_TIMEOUT - Timeout while waiting for thread.
    R_DRW2D_OS_ERR_THREAD_WAIT    - Failed to wait for OS thread
    R_DRW2D_OS_ERR_THREAD_DESTROY - Failed to destroy OS thread
    R_DRW2D_OS_ERR_MUTEX_HANDLE   - Invalid mutex handle
    R_DRW2D_OS_ERR_MUTEX_CREATE   - Failed to create mutex
    R_DRW2D_OS_ERR_MUTEX_DESTROY  - Failed to destroy mutex
    R_DRW2D_OS_ERR_MUTEX_LOCK     - Failed to lock mutex
    R_DRW2D_OS_ERR_MUTEX_UNLOCK   - Failed to unlock mutex
    R_DRW2D_OS_ERR_SEM_HANDLE     - Invalid semaphore handle
    R_DRW2D_OS_ERR_SEM_CREATE     - Failed to create semaphore
    R_DRW2D_OS_ERR_SEM_DESTROY    - Failed to destroy semaphore
    R_DRW2D_OS_ERR_SEM_WAIT       - Failed to wait for semaphore
    R_DRW2D_OS_ERR_SEM_POST       - Failed to post semaphore
*/
typedef enum
{
    R_DRW2D_OS_ERR_OK,
    R_DRW2D_OS_ERR_NOT_SUPPORTED,
    R_DRW2D_OS_ERR_PARAM_NOT_SUPPORTED,
    R_DRW2D_OS_ERR_THREAD_CREATE,
    R_DRW2D_OS_ERR_THREAD_INFO,
    R_DRW2D_OS_ERR_THREAD_HANDLE,
    R_DRW2D_OS_ERR_THREAD_TIMEOUT,
    R_DRW2D_OS_ERR_THREAD_WAIT,
    R_DRW2D_OS_ERR_THREAD_DESTROY,
    R_DRW2D_OS_ERR_MUTEX_HANDLE,
    R_DRW2D_OS_ERR_MUTEX_CREATE,
    R_DRW2D_OS_ERR_MUTEX_DESTROY,
    R_DRW2D_OS_ERR_MUTEX_LOCK,
    R_DRW2D_OS_ERR_MUTEX_UNLOCK,
    R_DRW2D_OS_ERR_SEM_HANDLE,
    R_DRW2D_OS_ERR_SEM_CREATE,
    R_DRW2D_OS_ERR_SEM_DESTROY,
    R_DRW2D_OS_ERR_SEM_WAIT,
    R_DRW2D_OS_ERR_SEM_POST,

    NUM_R_DRW2D_OS_ERR_CODES
} r_drw2d_OS_Error_t;



/***********************************************************
  Enum: r_drw2d_OS_Mem_t

  DRW2D framework OS abstraction layer memory allocation type.

  Values:
    R_DRW2D_OS_MEM_CPU   - Memory to be used by the CPU only
    R_DRW2D_OS_MEM_VIDEO - Memory to be used as video memory
*/
typedef enum
{
    R_DRW2D_OS_MEM_CPU   = 0,
    R_DRW2D_OS_MEM_VIDEO = 1
} r_drw2d_OS_Mem_t;



/***********************************************************
  Type: r_drw2d_OS_Thread_t

  OS-specific thread handle type.

  On R_OS, this will internally be cast to R_OS_Thread_t (uint32_t).

  On Win32, this will internally be cast to HANDLE (void*).

  On Linux, this will internally be cast to pthread_t.
*/
#if defined(R_DRW2D_OS_WIN32)
typedef HANDLE r_drw2d_OS_Thread_t;
#elif defined(R_DRW2D_OS_RENESAS)
typedef R_OS_Thread_t r_drw2d_OS_Thread_t;
#elif defined(R_DRW2D_OS_INTEGRITY)
typedef Task r_drw2d_OS_Thread_t;
#elif defined(R_DRW2D_OS_NO_OS)
#else
#error Unable to determine r_drw2d_OS_Thread_t
#endif



/***********************************************************
  Type: r_drw2d_OS_ThreadId_t

  Unique thread identifier.

  May not necessarily be the same as the thread handle (<r_drw2d_OS_Thread_t>).

  For example, when using pthreads, the pthread_t may be a structure (implementation dependent)
   that cannot be casted to a void pointer.

  See also:
   <R_DRW2D_OS_Thread_GetId>
*/
typedef void *r_drw2d_OS_ThreadId_t;



/*******************************************************************************
  Type: r_drw2d_OS_ThreadInfo_t

  Structure that manages thread information.
*/
typedef struct r_drw2d_OS_ThreadInfo_s r_drw2d_OS_ThreadInfo_t;



/***********************************************************
  Type: r_drw2d_OS_ThreadFxn_t

  Thread function type.

*/
typedef void (*r_drw2d_OS_ThreadFxn_t) (r_drw2d_OS_ThreadInfo_t *ThreadInfo);



/***********************************************************
  Type: r_drw2d_OS_Mutex_t

  Abstract type representing an OS mutex .
*/
#if defined(R_DRW2D_OS_WIN32)
typedef CRITICAL_SECTION r_drw2d_OS_Mutex_t;
#elif defined(R_DRW2D_OS_RENESAS)
typedef R_OS_ThreadMutex_t r_drw2d_OS_Mutex_t;
#elif defined(R_DRW2D_OS_INTEGRITY)
typedef LocalMutex r_drw2d_OS_Mutex_t;
#elif defined(R_DRW2D_OS_NO_OS)
typedef void* r_drw2d_OS_Mutex_t;
#else
#error Unable to determine r_drw2d_OS_Mutex_t
#endif



/***********************************************************
  Constant: R_DRW2D_OS_THREAD_WAIT_INFINITE
  
  'Magic' timeout value for <R_DRW2D_OS_Thread_Wait>.
*/
#define R_DRW2D_OS_THREAD_WAIT_INFINITE  0u







#ifndef R_DRW2D_OS_NO_OS
/*******************************************************************************
  Type: r_drw2d_OS_ThreadInfo_s

  Thread Information.

  Will be passed to the thread entry function.

  Members:
    Function  - Thread function
    UserData  - Pointer to User Data
    Prio      - Priority
    StackAddr - Stack address
    StackSize - Stack size
    Thread    - OS-specific thread handle or structure
    Result    - Result value of the thread function.
                (The argument "RetVal" when calling the R_DRW2D_OS_Thread_Exit function is stored.)
*/

typedef struct r_drw2d_OS_ThreadInfo_s
{
    r_drw2d_OS_ThreadFxn_t Function;
    void *                 UserData;
    uint32_t               Prio;
    void *                 StackAddr;
    uint32_t               StackSize;
    r_drw2d_OS_Thread_t    Thread;
    void *                 Result;
} r_drw2d_OS_ThreadInfo_s_t;
#endif /* R_DRW2D_OS_NO_OS */


/*******************************************************************************
  Type: r_drw2d_OS_Semaphore_t
*/
typedef struct
{
#if defined(R_DRW2D_OS_WIN32)
   HANDLE Handle;
#elif defined(R_DRW2D_OS_RENESAS)
   R_OS_Sem_t Sem;
#elif defined(R_DRW2D_OS_INTEGRITY)
   Semaphore Sem;
#elif defined(R_DRW2D_OS_NO_OS)
   int8_t unused;
#else
#error Unable to determine r_drw2d_OS_Semaphore_t
#endif
} r_drw2d_OS_Semaphore_t;





/***********************************************************
  Section: Global Functions
*/


/***********************************************************
  Group: Thread functions
*/



/***********************************************************
  Function: R_DRW2D_OS_Thread_Create

  Creates and starts a new thread.

  This function is empty as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If you want to customize depending on the OS environment,
    please implement a function to create a thread.

  Parameter:
    ThreadInfo - Pointer to thread info.
                 The entry *Function* and *UserData* fields must be set.
                 Please set *Prio*, *StackAddr* and *StackSize* as necessary.
                 If there are NOT set, the default value is set from OS.

  Return Value:
    R_DRW2D_OS_ERR_OK            - No error occurred.
    R_DRW2D_OS_ERR_THREAD_CREATE - Failed to create OS-specific thread
    R_DRW2D_OS_ERR_THREAD_INFO   - Invalid thread info

  See also:
    <R_DRW2D_OS_Thread_Wait>, <R_DRW2D_OS_Thread_Destroy>
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Thread_Create (r_drw2d_OS_ThreadInfo_t *ThreadInfo);



/***********************************************************
  Function: R_DRW2D_OS_Thread_Wait

  Waits until the thread terminates or a time-out interval elapses.

  Unless the function timed out, the return value of the thread function 
  will be stored in the *Result* field of the *ThreadInfo* structure.

  This function is empty as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If you want to customize depending on the OS environment,
    please implement a function that waits until the terminates or
    a time-out interval elapses.

  Parameter:
    ThreadInfo - Pointer to thread info.
       Timeout - Time-out in Milliseconds. 0 to wait indefinitely
                 (see <R_DRW2D_OS_THREAD_WAIT_INFINITE>).

  Return Value:
    R_DRW2D_OS_ERR_OK             - No error occurred.
    R_DRW2D_OS_ERR_THREAD_TIMEOUT - Timeout while waiting for thread.

  See also:
    <R_DRW2D_OS_Thread_Create>, <R_DRW2D_OS_Thread_Destroy>
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Thread_Wait (r_drw2d_OS_ThreadInfo_t *ThreadInfo,
                                           uint32_t                 Timeout
                                           );



/***********************************************************
  Function: R_DRW2D_OS_Thread_GetId

  Get the unique identifier for the current thread.

  This function is empty as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If you want to customize depending on the OS environment,
    please implement a function that gets id of thread.

  Parameter:
    None

  Return Value:
    Unique identifier for the current thread
*/
r_drw2d_OS_ThreadId_t R_DRW2D_OS_Thread_GetId (void);



/***********************************************************
  Function: R_DRW2D_OS_Thread_Sleep

  Sleep for the given number of milliseconds.

  This function is empty as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If you want to customize depending on the OS environment,
    please implement a function that sleeps the thread.

  Parameter:
    MilliSeconds - Number of milliseconds to sleep

  Return Value:
    None
*/
void R_DRW2D_OS_Thread_Sleep (uint32_t MilliSeconds);



/***********************************************************
  Function: R_DRW2D_OS_Thread_Exit

  Terminate thread. Must be called from thread when terminate the thread.

  This function is empty as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If you want to customize depending on the OS environment,
    please implement a function that exits the thread.

  Parameter:
    ThreadInfo - Pointer to thread info.
        RetVal - Return value. Stored in ThreadInfo->Result.

  Return Value:
    None
*/
void R_DRW2D_OS_Thread_Exit (r_drw2d_OS_ThreadInfo_t *ThreadInfo,
                             void                    *RetVal
                             );



/***********************************************************
  Function: R_DRW2D_OS_Thread_Destroy

  De-initializes a thread and destroys the OS specific thread handle.

  This function is empty as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If you want to customize depending on the OS environment,
    please implement a function that waits until the terminates.

  Parameter:
    ThreadInfo - Pointer to thread info.

  Return Value:
    R_DRW2D_OS_ERR_OK                  - No error occurred.
    R_DRW2D_OS_ERR_THREAD_WAIT         - Failed to wait for OS thread.
    R_DRW2D_OS_ERR_THREAD_HANDLE       - Invalid thread handle.
    R_DRW2D_OS_ERR_PARAM_NOT_SUPPORTED - unsupported parameter.
    R_DRW2D_OS_ERR_THREAD_INFO         - Invalid thread info.

  See also:
    <R_DRW2D_OS_Thread_Create>, <R_DRW2D_OS_Thread_Wait>
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Thread_Destroy (r_drw2d_OS_ThreadInfo_t *ThreadInfo);



/***********************************************************
  Group: Mutex functions
*/



/***********************************************************
  Function: R_DRW2D_OS_Mutex_Create

  Creates and initializes an OS mutex.

  This function is empty as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If you want to customize depending on the OS environment,
    please implement a function that creates the mutex.

  Parameter:
    Mutex - Reference to OS-specific mutex handle or structure, see <r_drw2d_OS_Mutex_t>.

  Return Value:
    R_DRW2D_OS_ERR_OK - No error occurred.

  See also:
   <R_DRW2D_OS_Mutex_Destroy>, <R_DRW2D_OS_Mutex_Lock>, <R_DRW2D_OS_Mutex_Unlock>
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Mutex_Create (r_drw2d_OS_Mutex_t *Mutex);



/***********************************************************
  Function: R_DRW2D_OS_Mutex_Destroy

  Destroys a mutex.

  This function is empty as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If you want to customize depending on the OS environment,
    please implement a function that destroys the mutex.

  Parameter:
    Mutex - The mutex to be destroyed.

  Return Value:
    R_DRW2D_OS_ERR_OK            - No error occurred.
    R_DRW2D_OS_ERR_MUTEX_DESTROY - Failed to destroy mutex.

  See also:
   <R_DRW2D_OS_Mutex_Create>, <R_DRW2D_OS_Mutex_Lock>, <R_DRW2D_OS_Mutex_Unlock>
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Mutex_Destroy (r_drw2d_OS_Mutex_t *Mutex);



/***********************************************************
  Function: R_DRW2D_OS_Mutex_Lock

  Locks a mutex.

  This function is empty as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If you want to customize depending on the OS environment,
    please implement a function that locks the mutex.

  Parameter:
    Mutex - The mutex to be locked.

  Return Value:
    R_DRW2D_OS_ERR_OK           - No error occurred.
    R_DRW2D_OS_ERR_MUTEX_LOCK   - Failed to lock mutex.
    R_DRW2D_OS_ERR_MUTEX_HANDLE - Invalid mutex handle.

  See also:
   <R_DRW2D_OS_Mutex_Create>, <R_DRW2D_OS_Mutex_Destroy>, <R_DRW2D_OS_Mutex_Unlock>
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Mutex_Lock (r_drw2d_OS_Mutex_t *Mutex);



/***********************************************************
  Function: R_DRW2D_OS_Mutex_Unlock

  Unlocks a mutex.

  This function is empty as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If you want to customize depending on the OS environment,
    please implement a function that unlocks the mutex.

  Parameter:
    Mutex - The mutex to be unlocked.

  Return Value:
    R_DRW2D_OS_ERR_OK           - No error occurred.
    R_DRW2D_OS_ERR_MUTEX_UNLOCK - Failed to unlock mutex.
    R_DRW2D_OS_ERR_MUTEX_HANDLE - Invalid mutex handle.

  See also:
   <R_DRW2D_OS_Mutex_Create>, <R_DRW2D_OS_Mutex_Destroy>, <R_DRW2D_OS_Mutex_Lock>
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Mutex_Unlock (r_drw2d_OS_Mutex_t *Mutex);





/***********************************************************
  Group: Semaphore functions
*/



/***********************************************************
  Function: R_DRW2D_OS_Semaphore_Create

  Creates a semaphore.

  This function is empty as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If you want to customize depending on the OS environment,
    please implement a function that create the semaphore.

  Parameter:
             Sem - Location of the unnamed semaphore.
    InitialValue - Initial value of the semaphore.

  Return Value:
    R_DRW2D_OS_ERR_OK         - No error occurred.
    R_DRW2D_OS_ERR_SEM_CREATE - Failed to create semaphore.
    R_DRW2D_OS_ERR_SEM_HANDLE - Invalid semaphore handle.

  See also:
   <R_DRW2D_OS_Semaphore_Destroy>, <R_DRW2D_OS_Semaphore_Wait>, <R_DRW2D_OS_Semaphore_Post>
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Semaphore_Create (r_drw2d_OS_Semaphore_t *Sem,
                                                uint32_t                InitialValue
                                                );



/***********************************************************
  Function: R_DRW2D_OS_Semaphore_Destroy

  Destroys a semaphore.

  This function is empty as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If you want to customize depending on the OS environment,
    please implement a function that destroys the semaphore.

  Parameter:
    Sem - Location of the unnamed semaphore.

  Return Value:
    R_DRW2D_OS_ERR_OK          - No error occurred.
    R_DRW2D_OS_ERR_SEM_DESTROY - Failed to destroy semaphore.
    R_DRW2D_OS_ERR_SEM_HANDLE  - Invalid semaphore handle.

  See also:
   <R_DRW2D_OS_Semaphore_Create>, <R_DRW2D_OS_Semaphore_Wait>, <R_DRW2D_OS_Semaphore_Post>
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Semaphore_Destroy (r_drw2d_OS_Semaphore_t *Sem);



/***********************************************************
  Function: R_DRW2D_OS_Semaphore_Wait

  Wait until semaphore can be decremented.

  This function is empty as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If you want to customize depending on the OS environment,
    please implement a function that waits until semaphore
    can be decremented.

  Parameter:
    Sem - Location of the unnamed semaphore.

  Return Value:
    R_DRW2D_OS_ERR_OK         - No error occurred.
    R_DRW2D_OS_ERR_SEM_WAIT   - Failed to wait for semaphore.
    R_DRW2D_OS_ERR_SEM_HANDLE - Invalid semaphore handle.

  See also:
   <R_DRW2D_OS_Semaphore_Create>, <R_DRW2D_OS_Semaphore_Destroy>, <R_DRW2D_OS_Semaphore_Post>
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Semaphore_Wait (r_drw2d_OS_Semaphore_t *Sem);



/***********************************************************
  Function: R_DRW2D_OS_Semaphore_Post

  Increment semaphore.

  This function is empty as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If you want to customize depending on the OS environment,
    please implement a function that increments the semaphore.

  Parameter:
    Sem - Location of the unnamed semaphore.

  Return Value:
    R_DRW2D_OS_ERR_OK         - No error occurred.
    R_DRW2D_OS_ERR_SEM_POST   - Failed to post semaphore.
    R_DRW2D_OS_ERR_SEM_HANDLE - Invalid semaphore handle.

  See also:
   <R_DRW2D_OS_Semaphore_Create>, <R_DRW2D_OS_Semaphore_Destroy>, <R_DRW2D_OS_Semaphore_Wait>
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Semaphore_Post (r_drw2d_OS_Semaphore_t *Sem);





/***********************************************************
  Group: Memory utility functions
*/



/***********************************************************
  Function: R_DRW2D_OS_Mem_Alloc

  Allocate memory.

  This function is return 0 as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If you want to customize depending on the OS environment,
    please implement a function that allocates the memory.

  Parameter:
     NumBytes - Number of bytes to allocate
    Alignment - Address alignment (power of two) (0/1=any, 2=align to 16bit word boundary,
                4=align to 32bit dword boundary, ..)
         Type - R_DRW2D_OS_MEM_CPU (CPU usage only) or R_DRW2D_OS_MEM_VIDEO (CPU+GPU usage)

  Return Value:
    Start address of allocated memory or R_NULL if the allocation failed.
*/
void *R_DRW2D_OS_Mem_Alloc (uint32_t NumBytes, uint32_t Alignment, r_drw2d_OS_Mem_t Type);



/***********************************************************
  Function: R_DRW2D_OS_Mem_Free

  Free memory previously allocated with <R_DRW2D_OS_Mem_Alloc>.

  This function is empty as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If you want to customize depending on the OS environment,
    please implement a function that frees the memory.

  Parameter:
    Addr - Start address of allocated memory

  Return Value:
    None
*/
void R_DRW2D_OS_Mem_Free (void *Addr);



/***********************************************************
  Function: R_DRW2D_OS_Mem_Clear

  Clear memory area (fill with 0)

  This function is empty as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If you want to customize depending on the OS environment,
    please implement a function that clears the memory.

  Parameter:
        Addr - Start address of allocated memory
    NumBytes - Number of bytes to clear

  Return Value:
    None
*/
void R_DRW2D_OS_Mem_Clear (void *Addr, uint32_t NumBytes);



/***********************************************************
  Function: R_DRW2D_OS_Mem_Copy

  Copy memory area

  This function is empty as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If you want to customize depending on the OS environment,
    please implement a function that copies the memory.

  Parameter:
    DestAddr - Buffer to copy to
     SrcAddr - Buffer to copy from
    NumBytes - Number of bytes to copy

  Return Value:
    None
*/
void R_DRW2D_OS_Mem_Copy (void *DestAddr, const void *SrcAddr, uint32_t NumBytes);


#ifdef __cplusplus
}
#endif 

#endif /* R_DRW2D_OS_H_ */
