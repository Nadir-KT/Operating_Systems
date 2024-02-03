/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: application.c 11057 2016-11-10 08:23:04Z shinya.tomari $
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

/***********************************************************
  Title: Application Module

  The module contains the declaration of the application
  list only.
*/

/***********************************************************
  Section: Includes
*/

#include "r_typedefs.h"     /* Renesas basic types, e.g. uint32_t */
#include "fw_hmial_api.h"
#include "application.h"
#ifdef APP_MENU
#include "app_menu.h"
#endif
#ifdef APP_CLOCK
#include "app_clock.h"
#endif
#ifdef APP_SIMPLEMT
#include "app_simplemt.h"
#endif
#ifdef APP_TEST
#include "app_test.h"
#endif
#ifdef APP_DRW2DCPU
#include "app_drw2dcpu.h"
#endif
#ifdef APP_TRIPCOMP
#include "app_tripcomp.h"
#endif
#ifdef APP_TUTORIAL
#include "app_tutorial.h"
#endif
#ifdef APP_OGLES_TEST
#include "app_ogles_test.h"
#endif
#ifdef APP_3DDEMO
#include "app_3dDemo.h"
#endif
#ifdef APP_DISPSYNC_TEST
#include "app_dispsynctest.h"
#endif


/***********************************************************
  Section: Global Variables

  Description see: <application.h>
*/

/***********************************************************
  Variable: AppList
*/

const App_t * const AppList[] = {
#ifdef APP_CLOCK
    &AppClock,
#endif
#ifdef APP_SIMPLEMT
    &AppSimpleMt,
#endif
#ifdef APP_TEST
    &AppTest,
#endif
#ifdef APP_DRW2DCPU
    &AppDrw2dCPU,
#endif
#ifdef APP_TRIPCOMP
    &AppTripComp,
#endif
#ifdef APP_TUTORIAL
    &AppTutorial,
#endif
#ifdef APP_OGLES_TEST
    &AppOGLESTest,
#endif
#ifdef APP_3DDEMO
    &App3dDemo,
#endif    
#ifdef APP_DISPSYNC_TEST
    &AppDispSyncTest,
#endif
    
/* Menu at the End */
#ifdef APP_MENU
    &AppMenu,
#endif
};

/***********************************************************
  Variable: AppNum
*/

const uint8_t AppNum = sizeof (AppList) / sizeof(App_t*);

