/*
****************************************************************************
PROJECT : eeOS2
FILE    : $Id: r_os_prv_mutex.h 11006 2016-11-07 16:32:44Z michael.golczewski $
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

#ifndef R_OS_PRV_MUTEX_H_
#define R_OS_PRV_MUTEX_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Section: Local Functions
*/
/*******************************************************************************
  Function: _CheckMutexEvent

  Check for mutex events.

  If a mutex becomes available, this function is called.
  The function checks whether any thread was waiting for
  the specified mutex and will put in in ready state.
  only the first thread found is activated.

  Parameters:
  Mutex      - Pointer to a mutex object

  Returns:
  ==0 - No thread has been set to ready state
  !=0 - A thread has been set to ready state
*/

static int32_t _CheckMutexEvent(R_OS_ThreadMutex_t *Mutex);

/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_OS__Prv_ThreadMutexInit

  Initialize a thread mutex object.

  Parameters:
  Mutex      - Pointer to a mutex object
  Attr       - Pointer to a mutex attributes type or 0

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_Prv_ThreadMutexInit(R_OS_ThreadMutex_t *Mutex,
                             const R_OS_ThreadMutexAttr_t *Attr);


/*******************************************************************************
  Function: R_OS_Prv_ThreadMutexDestroy

  Destroy a thread mutex object.

  Parameters:
  Mutex      - Pointer to a mutex object

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_Prv_ThreadMutexDestroy(R_OS_ThreadMutex_t *Mutex);


/*******************************************************************************
  Function: R_OS_Prv_ThreadMutexLock

  Lock a thread mutex object.

  Parameters:
  Mutex      - Pointer to a mutex object

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_Prv_ThreadMutexLock(R_OS_ThreadMutex_t *Mutex);


/*******************************************************************************
  Function: R_OS_Prv_ThreadMutexUnlock

  Unlock a thread mutex object.

  Parameters:
  Mutex      - Pointer to a mutex object

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_Prv_ThreadMutexUnlock(R_OS_ThreadMutex_t *Mutex);


#ifdef __cplusplus
}
#endif

#endif  /* R_OS_PRV_MUTEX_H_ */
