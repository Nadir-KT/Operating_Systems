/*

****************************************************************************
PROJECT : Device Driver 
============================================================================
DESCRIPTION
RH850/D1x device initialization
============================================================================
                            C O P Y R I G H T
============================================================================
                       Copyright (c) 2019
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

/*******************************************************************************
  Title: device init functions
*/

/*******************************************************************************
  Section: Includes
*/
#include "r_typedefs.h"
#include "r_dev_api.h"

#ifdef R_BSP_SYS_INIT_USE
#include "r_ddb_api.h" 
#include "r_wm_api.h"
#include "r_wm_sys.h"
#include "r_spea_api.h"
#include "r_spea_sys.h"
#include "r_vdce_api.h"
#include "r_vdce_sys.h"
#include "r_vowe_api.h"
#include "r_vowe_sys.h"
#include "r_jcua_api.h"
#include "r_jcua_sys.h"
#endif

/*******************************************************************************
  Section: Functions
*/
void R_DEV_SysInit(void)
{
#ifdef R_BSP_SYS_INIT_USE
    R_WM_Sys_InitGlobal();
    R_SPEA_SYS_InitGlobal();
    R_VDCE_Sys_InitGlobal();
    R_VOWE_Sys_InitGlobal();
    R_JCUA_Sys_InitGlobal();
    R_DISCOM_Sys_InitGlobal();
    R_VOCA_Sys_InitGlobal();
#endif
}
