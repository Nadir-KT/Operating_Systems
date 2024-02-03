/*
****************************************************************************
PROJECT : WM
============================================================================ 
DESCRIPTION
WM system functions for D1x
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
****************************************************************************
*/

/***********************************************************
  Title: D1Mx WM SYS API implementation  
*/

/***********************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_cdi_api.h"
#include "r_ddb_api.h"
#include "r_config_wm.h"
#include "r_wm_api.h"
#include "r_wm_sys.h"
#include "r_spea_api.h"
#include "r_sys_wm.h"
#include "r_ecm_api.h"

/***********************************************************
  Section: Local Defines
*/
#define LOC_ECM_UNIT            (0u)
#define LOC_ECM_ENABLE          (1u)
#define LOC_ECM_DISABLE         (0u)

/***********************************************************
  Section: External Functions for WM SYS layer
*/
/***********************************************************
  Function: R_WM_Sys_EcmInit

  see: <r_sys_wm.h>
*/
uint32_t R_WM_Sys_EcmInit(void)
{
    r_ecm_Error_t  err;
    uint32_t       ret_val;
    
    /* Enable ECM */
    err = R_ECM_Init(LOC_ECM_UNIT);
    if (err == R_ECM_ERR_OK)
    {
        err = R_ECM_ClearErrorSourceStatus(LOC_ECM_UNIT, R_DEV_ECMERR_VOCERR);
        if (err == R_ECM_ERR_OK)
        {
            err = R_ECM_SetIsrCallback(
                    LOC_ECM_UNIT, R_ECM_INT_FEINT, R_DEV_ECMERR_VOCERR, &R_WM_Sys_IsrVocaErr);
            if (err == R_ECM_ERR_OK)
            {
                err = R_ECM_EnableFeint(LOC_ECM_UNIT, R_DEV_ECMERR_VOCERR, LOC_ECM_ENABLE);
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }
    
    /* Check error */
    if (err == R_ECM_ERR_OK)
    {
        ret_val = R_WM_SYS_OK;
    }
    else
    {
        ret_val = R_WM_SYS_NG;
    }
    return ret_val;
}

/***********************************************************
  Function: R_WM_Sys_EcmDeInit

  see: <r_sys_wm.h>
*/
uint32_t R_WM_Sys_EcmDeInit(void)
{
    r_ecm_Error_t  err;
    uint32_t       ret_val;
    
    /* DeInit ECM */
    err = R_ECM_DeInit(LOC_ECM_UNIT);
    if (err == R_ECM_ERR_OK)
    {
        ret_val = R_WM_SYS_OK;
    }
    else
    {
        ret_val = R_WM_SYS_NG;
    }
    return ret_val;
}

/***********************************************************
  Section: WM Driver Support Functions
  
  Comments see: <r_wm_sys.h>
*/
void R_WM_Sys_IsrVocaErr(void)
{
    /* Disable interrupt */
    (void)R_ECM_EnableFeint(LOC_ECM_UNIT, R_DEV_ECMERR_VOCERR, LOC_ECM_DISABLE);
    
    /* Call VOCA */
    R_WM_Sys_IsrVcoa();
    
    /* Call DISCOM */
    R_WM_Sys_IsrDiscom();
    
    /* Enable interrupt */
    (void)R_ECM_EnableFeint(LOC_ECM_UNIT, R_DEV_ECMERR_VOCERR, LOC_ECM_ENABLE);
}

