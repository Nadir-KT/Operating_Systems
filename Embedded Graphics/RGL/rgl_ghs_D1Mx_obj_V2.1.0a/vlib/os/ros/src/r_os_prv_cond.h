/*
****************************************************************************
PROJECT : eeOS2
FILE    : $Id: r_os_prv_cond.h 11006 2016-11-07 16:32:44Z michael.golczewski $
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

#ifndef R_OS_PRV_COND_H_
#define R_OS_PRV_COND_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Section: Local Functions
*/

/*******************************************************************************
  Function: _CheckCondEvent

  Check for conditional variable events.

  If a conditional variable is signaled, this function is
  called. The function checks whether any thread was waiting
  for the specified conditional variable and will put in in
  ready state; only the first thread found is activated.

  Parameters:
  Cond       - Pointer to a conditional variable object

  Returns:
  ==0 - No thread has been set to ready state
  !=0 - A thread has been set to ready state
*/

static int32_t _CheckCondEvent(R_OS_ThreadCond_t *Cond);


/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_OS_Prv_ThreadCondInit

  Initialize a condition variable object.

  Parameters:
  Cond       - Pointer to a condition variable object
  Attr       - Condition variable attribute object or 0

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_Prv_ThreadCondInit(R_OS_ThreadCond_t *Cond,
                            const R_OS_ThreadCondAttr_t *Attr);


/*******************************************************************************
  Function: R_OS_Prv_ThreadCondDestroy

  Destroy a condition variable object.

  Parameters:
  Cond       - Pointer to a condition variable object

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_Prv_ThreadCondDestroy(R_OS_ThreadCond_t *Cond);


/*******************************************************************************
  Function: R_OS_Prv_ThreadCondWait

  Wair for a condition.

  Parameters:
  Cond       - Pointer to a condition variable object

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_Prv_ThreadCondWait(R_OS_ThreadCond_t *Cond,
                            R_OS_ThreadMutex_t *Mutex);


/*******************************************************************************
  Function: R_OS_Prv_ThreadCondSignal

  Signal a condition.

  Parameters:
  Cond       - Pointer to a condition variable object

  Returns:
  ==0 - OK
  !=0 - NG
*/

int32_t R_OS_Prv_ThreadCondSignal(R_OS_ThreadCond_t *Cond);


#ifdef __cplusplus
}
#endif

#endif  /* R_OS_PRV_COND_H_ */
