/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: application.h 11057 2016-11-10 08:23:04Z shinya.tomari $
============================================================================ 
DESCRIPTION
Sample program for demonstration of the D1MX GFX features
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2014
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

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "img_format.h"

#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************
  Title: Application Interface
  
  This header defines the interface, which an application
  has to provide to be executed by the framework.
*/

/***********************************************************
  Section: Global Types

  Definition of global data types
*/

/***********************************************************
  Type: App_t

  Application structure.

  Each application has to define such a strucure in order to be
  supported by the GFX application note framework.

  Members:
  Icon        - Pointer to the application icon
  Name        - Name of the application
  Description - One line description of the application
  Control     - Pointer to control elements callback table
  Flags       - Application requirements
  Init        - Init function of the application
  DeInit      - DeInit function of the application
*/

typedef struct App_s 
{
    const Img_t               *Icon;
    char                      *Name;
    char                      *Description;
    const fw_hmial_Control_t  *Control;
    uint32_t                   Flags;
    void (*Init)(void);
    void (*DeInit)(void);
} App_t;


/***********************************************************
  Section: Global Variables
*/

/***********************************************************
  Variable: AppList

  List of applications.

  This array contains a list of pointers to all available
  applications. Its content is defined in 'application.c'.
*/

extern const App_t * const AppList[];


/***********************************************************
  Variable: AppNum

  Number of demos in the 'AppList'.
*/

extern const uint8_t AppNum;


#ifdef __cplusplus
}
#endif

#endif /* _APPLICATION_H_ */
