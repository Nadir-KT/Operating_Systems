/*
****************************************************************************
PROJECT : eeOS2
FILE    : $Id: r_os_prv_sem.h 11006 2016-11-07 16:32:44Z michael.golczewski $
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

#ifndef R_OS_PRV_SEM_H_
#define R_OS_PRV_SEM_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Section: Local Functions
*/
/*******************************************************************************
  Function: _CheckSemEvent

  Check for semaphore events.

  If a semaphore becomes available, this function is called.
  The function checks whether any thread was waiting for
  the specified semaphore and will put in in ready state.
  only the first thread found is activated.

  Parameters:
  Sem        - Pointer to a mutex object

  Returns:
  ==0 - No thread has been set to ready state
  !=0 - A thread has been set to ready state
*/

static int32_t _CheckSemEvent(R_OS_Sem_t *Sem);

/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_OS_Prv_SemInit

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

int32_t R_OS_Prv_SemInit(R_OS_Sem_t *Sem, int32_t PShared, uint32_t Value);


/*******************************************************************************
  Function: R_OS_Prv_SemDestroy 

  Destroy an unnamed semaphore.

  Parameters:
  Sem        - Location of the unnamed semaphore

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_Prv_SemDestroy(R_OS_Sem_t *Sem);


/*******************************************************************************
  Function: R_OS_Prv_SemWait

  Wait for an unnamed semaphore.

  Parameters:
  Sem        - Location of the unnamed semaphore

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_Prv_SemWait(R_OS_Sem_t *Sem);


/*******************************************************************************
  Function: R_OS_Prv_SemTryWait

  Try to decrement an unnamed semaphore.
  If the operation cannot be executed immediately, the
  function will not switch the calling thread into 
  EEOS_TSTATE_WAITING.   

  Parameters:
  Sem        - Location of the unnamed semaphore

  Returns:
  ==0 - OK
  !=0 - NG, operation could not be executed
*/

int32_t R_OS_Prv_SemTryWait(R_OS_Sem_t *Sem);


/*******************************************************************************
  Function: R_OS_Prv_SemPost

  Unlock an unnamed semaphore.

  Parameters:
  Sem        - Location of the unnamed semaphore

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_Prv_SemPost(R_OS_Sem_t *Sem);


/*******************************************************************************
  Function: R_OS_Prv_SemGetValue

  Get the current semaphore value.

  Parameters:
  Sem        - Location of the unnamed semaphore
  Value      - Pointer to the location to store the value

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_Prv_SemGetValue(R_OS_Sem_t *Sem, int32_t *Value);


#ifdef __cplusplus
}
#endif

#endif  /* R_OS_PRV_SEM_H_ */
