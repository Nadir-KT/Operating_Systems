/*
****************************************************************************
PROJECT : ROS
FILE    : $Id: r_os_prv_tick.h 11006 2016-11-07 16:32:44Z michael.golczewski $
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

#ifndef R_OS_PRV_TICK_H_
#define R_OS_PRV_TICK_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Section: Local Functions
*/
/*******************************************************************************
  Function: _CheckTimeEvent

  Check threads waiting for a time, if they need activation.

  The function is called by 'R_OS_Prv_Tick' in will check
  for time triggered wakeups of threads.

  Parameters:
  void

  Returns:
  ==0 - No thread has been activated
  !=0 - One or more threads have been activated
*/

static int32_t _CheckTimeEvent(void);

/*******************************************************************************
  Function: _Sleep

  Set the calling thread into sleep mode.

  The function calculates the next time event and calls the
  scheduler after putting the calling thread into the sleeping
  state.

  Parameters:
  void

  Returns:
  void
*/

static void _Sleep(void);


/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_OS_Prv_GetTimeMs


  Return current system time in ms.

  Parameters:
  void

  Returns:
  Current systemt time
*/

int32_t R_OS_Prv_GetTimeMs(void);


/*******************************************************************************
  Function: R_OS_Prv_ThreadSleep

  Suspend a thread for specified time.

  The calling thread is suspended for at least 'Time' ticks.

  Parameters:
  Time          - Time in ticks

  Returns:
  void
*/

void R_OS_Prv_ThreadSleep(uint32_t Time);


#ifdef __cplusplus
}
#endif

#endif  /* R_OS_PRV_TICK_H_ */
