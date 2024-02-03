/*
****************************************************************************
PROJECT : ROS2
FILE    : $Id: r_os_thread.h 11006 2016-11-07 16:32:44Z michael.golczewski $
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

#ifndef R_OS_THREAD_H_
#define R_OS_THREAD_H_

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
  Section: Global Types
*/

/*******************************************************************************
  Type: R_OS_ThreadCreateParam_t

  Auxiliary type for thread creation via syscall.

  Members:
  Thread        - Pointer to a thread structure
  Attr          - Pointer to thread attribues or zero for default 
*/

typedef struct {
    R_OS_Thread_t     *Thread;
    R_OS_ThreadAttr_t *Attr;
} R_OS_ThreadCreateParam_t;


#ifdef __cplusplus
}
#endif

#endif  /* R_OS_THREAD_H_ */
