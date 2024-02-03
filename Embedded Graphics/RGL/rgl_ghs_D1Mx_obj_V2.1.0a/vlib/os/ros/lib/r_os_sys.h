/*
****************************************************************************
PROJECT : ROS2
FILE    : $Id: r_os_sys.h 11006 2016-11-07 16:32:44Z michael.golczewski $
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

#ifndef R_OS_SYS_H_
#define R_OS_SYS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Title: ROS Support Functions

  The generic ROS uses these functions. They have to be 
  implemented by the application
*/

/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_OS_Sys_Error

  Error handler.

  Parameters:
  Err        - Error code

  Returns:
  void
*/

void R_OS_Sys_Error(int32_t Err);


/*******************************************************************************
  Function: R_OS_Sys_Idle

  Idle loop.

  Parameters:
  void

  Returns:
  void
*/

void R_OS_Sys_Idle(void);

#ifdef __cplusplus
}
#endif

#endif /* R_OS_SYS_H_ */
