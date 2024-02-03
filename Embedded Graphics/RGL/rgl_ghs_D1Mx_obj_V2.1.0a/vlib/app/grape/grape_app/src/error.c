/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: error.c 11057 2016-11-10 08:23:04Z shinya.tomari $
============================================================================ 
DESCRIPTION
Sample program for demonstration of the GFX features
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
  Title: Error Handler Module
  
  This is the application's error handler.
*/


/***********************************************************
  Section: Includes
*/

#include "r_typedefs.h"     /* Renesas basic types, e.g. uint32_t */
#include "fw_clibal_api.h"
#include "fw_osal_api.h"
#include "error.h"

/***********************************************************
  Section: Global Functions

  Description see: <error.h>
*/


/***********************************************************
  Function: Error
*/

void Error(int Err)
{
    FW_CLIBAL_PrintF("ERROR (code = 0x%08x): Thread is now sleeping forever...\n", Err);
    while (Err)
    {
        FW_OSAL_ThreadSleep(100);
    }
}
