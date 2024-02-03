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
#include "r_discom_api.h"
#include "r_config_discom.h"

/***********************************************************
  Section: Local Defines
*/

/***********************************************************
  Constant: LOC_WM_UNIT_UNKNOWN
  
  Definition of unknown WM unit.
*/
#define LOC_WM_UNIT_UNKNOWN   (0xFF)

/***********************************************************
  Section: Global Variables
*/

/***********************************************************
  Section: Local Function Declarations
*/
R_WM_STATIC uint32_t loc_ConvDiscomUnitToWmUnit(const uint32_t DiscomUnit);

R_WM_STATIC void     loc_ConvWmUnitToDiscomUnit(const uint32_t  Unit, 
                                                uint32_t *const DiscomUnitMin, 
                                                uint32_t *const DiscomUnitMax);
R_WM_STATIC void     loc_DiscomErrorCallback(const uint32_t         DiscomUnit,
                                             const r_discom_Error_t Error);

/***********************************************************
  Section: Local Functions
*/

/***********************************************************
  Function: loc_ConvDiscomUnitToWmUnit

  Convert from Discom Unit to WM unit.
  
  Parameters:
  DiscomUnit  - Specifies the Discom Unit number.

  Return Value:
  0 or 1 - WM unit number. 
  0xFF   - Unkown.
*/

R_WM_STATIC uint32_t loc_ConvDiscomUnitToWmUnit(const uint32_t DiscomUnit)
{
    uint32_t wm_unit;
    
    if ((DiscomUnit == R_DISCOM_UNIT0) || (DiscomUnit == R_DISCOM_UNIT1))
    {
        wm_unit = R_WM_UNIT0;
    }
    else if ((DiscomUnit == R_DISCOM_UNIT2) || (DiscomUnit == R_DISCOM_UNIT3))
    {
        wm_unit = R_WM_UNIT1;
    }
    else
    {
        wm_unit = LOC_WM_UNIT_UNKNOWN;
    }
    return wm_unit;
}

/***********************************************************
  Function: loc_ConvWmUnitToDiscomUnit

  Convert from WM unit to min and max Discom Unit.
  
  Parameters:
  Unit          - Specifies the WM Unit number.
  DiscomUnitMin - Outputs minimum Discom Unit number.
  DiscomUnitMax - Outputs maximum Discom Unit number.

  Return Value:
  None.
*/

R_WM_STATIC void loc_ConvWmUnitToDiscomUnit(const uint32_t Unit, 
                                            uint32_t *const DiscomUnitMin, 
                                            uint32_t *const DiscomUnitMax)
{
    if (Unit == R_WM_UNIT0) 
    {
        *DiscomUnitMin = R_DISCOM_UNIT0;
        *DiscomUnitMax = R_DISCOM_UNIT1;
    }
    else if (Unit == R_WM_UNIT1)
    {
        *DiscomUnitMin = R_DISCOM_UNIT2;
        *DiscomUnitMax = R_DISCOM_UNIT3;
    }
    else
    {
        *DiscomUnitMin = LOC_WM_UNIT_UNKNOWN;
        *DiscomUnitMax = LOC_WM_UNIT_UNKNOWN;
    }
}

/***********************************************************
  Function: loc_DiscomErrorCallback

  Error callback function.
  This function reports the error by DISCOM driver.
  
  Parameters:
  Unit          - Discom Unit number.
  Error         - Error code of Discom driver.

  Return Value:
  None.
*/
R_WM_STATIC void loc_DiscomErrorCallback(const uint32_t         DiscomUnit,
                                         const r_discom_Error_t Error)
{
    uint32_t wm_unit;
    
    /* Convert WM unit */
    wm_unit = loc_ConvDiscomUnitToWmUnit(DiscomUnit);

    /* Notify error */
    R_WM_SYS_REPORT_ERROR(wm_unit, R_WM_ERR_DISCOM_INTERNAL, Error);
    R_WM_ErrorHandler(wm_unit, R_WM_ERR_DISCOM_INTERNAL);
}

/***********************************************************
  Section: External Functions for WM SYS layer
*/

/***********************************************************
  Function: R_WM_Sys_DiscomInit

  see: <r_sys_wm.h>
*/
uint32_t R_WM_Sys_DiscomInit(const uint32_t Unit)
{
    uint32_t            ret = R_WM_SYS_OK;
    uint32_t            discom_min;
    uint32_t            discom_max;
    uint32_t            discom_unit;
    r_discom_Error_t    discom_ret;
    
    loc_ConvWmUnitToDiscomUnit(Unit, &discom_min, &discom_max);
    
    for (discom_unit = discom_min; discom_unit <= discom_max; discom_unit++)
    {
        /* Init driver */
        discom_ret = R_DISCOM_Init(discom_unit);
        if (discom_ret != R_DISCOM_ERR_OK)
        {
            ret = R_WM_SYS_NG;
            break;
        }
        else
        {
            /* Do nothing */
        }
    }

    /* Install error callback */
    if (ret == R_WM_SYS_OK)
    {
        discom_ret = R_DISCOM_ErrorCallbackSet(discom_min, &loc_DiscomErrorCallback);
        if (discom_ret != R_DISCOM_ERR_OK)
        {
            ret = R_WM_SYS_NG;
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
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_DiscomDeInit

  see: <r_sys_wm.h>
*/
uint32_t R_WM_Sys_DiscomDeInit(const uint32_t Unit)
{
    uint32_t            ret = R_WM_SYS_OK;
    uint32_t            discom_min;
    uint32_t            discom_max;
    uint32_t            discom_unit;
    r_discom_Error_t    discom_ret;
    
    loc_ConvWmUnitToDiscomUnit(Unit, &discom_min, &discom_max);
    
    for (discom_unit = discom_min; discom_unit <= discom_max; discom_unit++)
    {
        /* Disable interrupt */
        (void)R_DISCOM_IntDisable(discom_unit);
        
        /* Deinit driver */
        discom_ret = R_DISCOM_DeInit(discom_unit);
        if (discom_ret != R_DISCOM_ERR_OK)
        {
            ret = R_WM_SYS_NG;
        }
        else
        {
            /* Do nothing */
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_IsrDiscom

  see: <r_sys_wm.h>
*/
void R_WM_Sys_IsrDiscom(void)
{
    uint32_t        wm_unit;
    uint32_t        discom_min;
    uint32_t        discom_max;
    uint32_t        discom_unit;
    uint32_t        factor = R_WM_FALSE;
    r_wm_DevSys_t   *dev_sys;
    r_wm_Event_t    event = {R_WM_EVENT_DISCOM_MISMACTH};
    r_wm_State_t    status;
    
    for (wm_unit = R_WM_UNIT0; wm_unit < R_WM_DEV_NUM; wm_unit++)
    {
        /* Get global data */
        dev_sys = R_WM_Sys_GetDevSys(wm_unit);
        
        status = R_WM_Sys_StateGet(wm_unit);
        if (status != R_WM_STATE_UNINITIALIZED)
        {
            event.Data = 0u;
            
            /* Get max unit */
            loc_ConvWmUnitToDiscomUnit(wm_unit, &discom_min, &discom_max);
            for (discom_unit = discom_min; discom_unit <= discom_max; discom_unit++)
            {
                /* Get the factor */
                (void)R_DISCOM_StatusGet(discom_unit, &factor);
                
                if (factor == R_WM_FALSE)
                {
                    /* Do nothing */
                }
                else
                {
                    /* Clear the factor */
                    (void)R_DISCOM_StatusClear(discom_unit);
                    
                    /* Set callback data */
                    event.Data |= (R_WM_SYS_FLAG_ON << discom_unit);
                }
            }

            if (event.Data != 0u)
            {
                /* Execute callback */
                if ((dev_sys->RegEvents & 
                    (R_WM_SYS_FLAG_ON << (uint32_t)R_WM_EVENT_DISCOM_MISMACTH)) != 0u)
                {
                    if (dev_sys->EventCb != R_NULL)
                    {
                        dev_sys->EventCb(wm_unit, &event);
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    /* Do Nothing */
                }
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            /* Do Nothing */
        }
    }
}

/***********************************************************
  Section: WM Driver Support Functions
  
  Comments see: <WM Support Functions>
*/

/***********************************************************
  Function: R_WM_Sys_DevRootDiscomSet

  see: <r_wm_sys.h>
*/
void R_WM_Sys_DevRootDiscomSet(const uint32_t        Unit, 
                               r_wm_Discom_t  *const RootDiscom)
{
    r_wm_DevSys_t       *dev_sys;
    dev_sys = R_WM_Sys_GetDevSys(Unit);
    
    /* Set root pointer */
    dev_sys->RootDiscom = RootDiscom;
}

/***********************************************************
  Function: R_WM_Sys_DiscomCreate

  see: <r_wm_sys.h>
*/
uint32_t R_WM_Sys_DiscomCreate(const uint32_t              Unit, 
                               const r_wm_Discom_t  *const Discom)
{
    uint32_t            ret;
    r_discom_Error_t    discom_ret;
    r_discom_Param_t    discom_param;

    if ((Unit >= R_WM_DEV_NUM) || (Discom == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        /* Check association between DISCOM unit and WM unit */
        if (Unit != loc_ConvDiscomUnitToWmUnit((uint32_t)Discom->DiscomUnit))
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
            ret = R_WM_SYS_NG;
        }
        else
        {
            /* Set parameter */
            discom_param.HOffset   = Discom->PosX;
            discom_param.VOffset   = Discom->PosY;
            discom_param.HSize     = Discom->Width;
            discom_param.VSize     = Discom->Height;
            discom_param.AlphaMode = R_DISCOM_ALPHA_DEFAULT;
            discom_param.Alpha     = 0xFF;
            
            discom_ret = R_DISCOM_ParamSet((uint32_t)Discom->DiscomUnit, &discom_param);
            if (discom_ret != R_DISCOM_ERR_OK)
            {
                ret = R_WM_SYS_NG;
            }
            else
            {
                /* Set CRC value */
                discom_ret = R_DISCOM_CrcSet(
                                (uint32_t)Discom->DiscomUnit, 
                                Discom->ExpCrc, 
                                R_DISCOM_PERIOD_VBLANK);
                if (discom_ret != R_DISCOM_ERR_OK)
                {
                    ret = R_WM_SYS_NG;
                }
                else
                {
                    ret = R_WM_SYS_OK;
                }
            }
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_DiscomDelete

  see: <r_wm_sys.h>
*/
uint32_t R_WM_Sys_DiscomDelete(const uint32_t              Unit, 
                               const r_wm_Discom_t  *const Discom)
{
    /* Disable Discom */
    return R_WM_Sys_DiscomEnable(Unit, Discom, R_WM_FALSE);
}

/***********************************************************
  Function: R_WM_Sys_DiscomEnable

  see: <r_wm_sys.h>
*/
uint32_t R_WM_Sys_DiscomEnable(const uint32_t              Unit, 
                               const r_wm_Discom_t  *const Discom,
                               const uint32_t              Enabled)
{
    uint32_t            ret;
    r_discom_Error_t    discom_ret;
    
    if ((Unit >= R_WM_DEV_NUM) || (Discom == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        if (Enabled == R_WM_TRUE)
        {
            /* Enable interrupt */
            discom_ret = R_DISCOM_IntEnable((uint32_t)Discom->DiscomUnit);
            if (discom_ret != R_DISCOM_ERR_OK)
            {
                ret = R_WM_SYS_NG;
            }
            else
            {
                /* Enable Discom driver */
                discom_ret = R_DISCOM_Enable((uint32_t)Discom->DiscomUnit);
                if (discom_ret != R_DISCOM_ERR_OK)
                {
                    ret = R_WM_SYS_NG;
                }
                else
                {
                    ret = R_WM_SYS_OK;
                }
            }
        }
        else
        {
            /* Note  */
            /*  R_DISCOM_IntDisable is not executed in this timing  */
            /*  to notify and clear the last interrupt.             */
            /*  R_DISCOM_IntDisable is executed in R_WM_Sys_DiscomDeInit. */
            
            /* Disable Discom driver */
            discom_ret = R_DISCOM_Disable((uint32_t)Discom->DiscomUnit);
            if (discom_ret != R_DISCOM_ERR_OK)
            {
                ret = R_WM_SYS_NG;
            }
            else
            {
                ret = R_WM_SYS_OK;
            }
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_DiscomCrcSet

  see: <r_wm_sys.h>
*/
uint32_t R_WM_Sys_DiscomCrcSet(const uint32_t              Unit, 
                               const r_wm_Discom_t  *const Discom,
                               const uint32_t              ExpCrc)
{
    uint32_t            ret;
    r_wm_DevSys_t       *dev_sys;
    uint32_t            mask;

    if ((Unit >= R_WM_DEV_NUM) || (Discom == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        /* Check Discom unit */
        mask = R_WM_SYS_FLAG_ON << ((uint32_t)Discom->DiscomUnit + R_WM_SYS_FRAME_BIT_DISCOM0);
        
        /* Update Frame Status flag */
        dev_sys = R_WM_Sys_GetDevSys(Unit);
        dev_sys->FrameStatusFlags |= mask;
        
        ret = R_WM_SYS_OK;
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_DiscomCrcGet

  see: <r_wm_sys.h>
*/
uint32_t R_WM_Sys_DiscomCrcGet(const uint32_t              Unit, 
                               const r_wm_Discom_t  *const Discom,
                               uint32_t             *const Crc)
{
    uint32_t            ret;
    r_discom_Error_t    discom_ret;

    if ((Unit >= R_WM_DEV_NUM) || (Discom == R_NULL) || (Crc == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        /* Get CRC value */
        discom_ret = R_DISCOM_CrcGet((uint32_t)Discom->DiscomUnit, Crc);
        if (discom_ret != R_DISCOM_ERR_OK)
        {
            ret = R_WM_SYS_NG;
        }
        else
        {
            ret = R_WM_SYS_OK;
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_DiscomUpdate

  see: <r_wm_sys.h>
*/
uint32_t R_WM_Sys_DiscomUpdate(const uint32_t  Unit)
{
    uint32_t            ret = R_WM_SYS_OK;
    r_wm_DevSys_t       *dev_sys;
    r_wm_Discom_t       *cur_discom;
    uint32_t            mask;
    uint32_t            i;
    r_discom_Error_t    discom_ret;

    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        /* Get global data */
        dev_sys = R_WM_Sys_GetDevSys(Unit);
        if ((dev_sys->FrameStatusFlags & R_WM_SYS_FRAME_MASK_DISCOM) != 0u)
        {
            /* Check Discom chain */
            cur_discom = dev_sys->RootDiscom;
            for (i = 0; i < R_WM_MAX_DISCOM_PER_UNIT; i++)
            {
                if (cur_discom == R_NULL)
                {
                    break;
                }
                else
                {
                    mask = R_WM_SYS_FLAG_ON 
                           << ((uint32_t)cur_discom->DiscomUnit + R_WM_SYS_FRAME_BIT_DISCOM0);
                    if ((dev_sys->FrameStatusFlags & mask) != 0u)
                    {
                        /* Update CRC */
                        discom_ret = R_DISCOM_CrcSet(
                                        (uint32_t)cur_discom->DiscomUnit, 
                                        cur_discom->ExpCrc, 
                                        R_DISCOM_PERIOD_VBLANK);
                        if (discom_ret != R_DISCOM_ERR_OK)
                        {
                            ret = R_WM_SYS_NG;
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
                    
                    /* Check next */
                    cur_discom = cur_discom->Next;
                }
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    return ret;
}


