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
  Title: D1Mx and D1Lx WM SYS API implementation  
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
#include "r_vdce_api.h"
#include "r_spea_api.h"
#include "r_sys_wm.h"
#include "r_voca_api.h"

/***********************************************************
  Section: Local Defines
*/

/* TODO: Move to macro_config  */
#define R_VOCA_UNIT0          (0u)

/***************************************************************************
 Constants: Internal RAM size 

 The definition for Internal RAM size for expective image.
 
 LOC_PIXEL_PER_RAM_ADR  - Pixel number per one address
*/
#define LOC_PIXEL_PER_RAM_ADR        (8u)

/***************************************************************************
 Constants: Minimum front porch size

 The definition for front porch size check.
 
 LOC_MIN_H_FRONT_PORCH  - Minimum front porch size
*/
#define LOC_MIN_H_FRONT_PORCH        (1u)


/***************************************************************************
 Constants: Activity monitor time 

 The definition for activity monitor time conversion
 
 LOC_ACT_MON_MAX_USEC     - Maximum activity monitor time. (usec)
 LOC_ACT_MON_CONV_COEF_A  - Convert with x(LOC_ACT_MON_CONV_COEF_A / LOC_ACT_MON_CONV_COEF_B)
 LOC_ACT_MON_CONV_COEF_B  - Convert with x(LOC_ACT_MON_CONV_COEF_A / LOC_ACT_MON_CONV_COEF_B)
*/

#define LOC_ACT_MON_MAX_USEC        (136467u)
#define LOC_ACT_MON_CONV_COEF_A     (3u)
#define LOC_ACT_MON_CONV_COEF_B     (100u)


/***********************************************************
  Section: Global Variables
*/

/***********************************************************
  Section: Local Function Declarations
*/
R_WM_STATIC void loc_VocaErrorCallback(const uint32_t         VocaUnit,
                                       const r_voca_Error_t   Error);

R_WM_STATIC uint32_t loc_SetMonitorArea(const uint32_t Unit, const r_wm_Voca_t *const Voca);

R_WM_STATIC uint32_t loc_SetInternalRam(const r_wm_Voca_t *const Voca);

R_WM_STATIC uint16_t loc_ConvUsecToHwTime(uint32_t UsecTime);

/***********************************************************
  Section: Local Functions
*/
/***********************************************************
  Function: loc_VocaErrorCallback

  Error callback function.
  This function reports the error by VOCA driver.
  
  Parameters:
  Unit          - VOCA Unit number.
  Error         - Error code of VOCA driver.

  Return Value:
  None.
*/
R_WM_STATIC void loc_VocaErrorCallback(const uint32_t         VocaUnit,
                                       const r_voca_Error_t   Error)
{
    /* Notify error */
    R_WM_SYS_REPORT_ERROR(VocaUnit, R_WM_ERR_VOCA_INTERNAL, Error);
    R_WM_ErrorHandler(VocaUnit, R_WM_ERR_VOCA_INTERNAL);
}

/***********************************************************
  Function: loc_SetMonitorArea

  This function executes R_VOCA_MonitorAreaSet from VOCA instance.
  
  Parameters:
  Unit     - WM unit number
  Voca     - VOCA instance.

  Return Value:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail
*/
R_WM_STATIC uint32_t loc_SetMonitorArea(const uint32_t Unit, const r_wm_Voca_t *const Voca)
{
    uint32_t            ret;
    r_voca_VoCh_t       video_ch;
    r_voca_MonArea_t    monitor_area;
    r_voca_Error_t      voca_ret;
    uint32_t            i;
    
    /* Set Video channel */
    if (Unit == R_WM_UNIT0)
    {
        video_ch = R_VOCA_VO_0;
    }
    else
    {
        video_ch = R_VOCA_VO_1;
    }
    
    /* Set up */
    monitor_area.MonAreaNum  = Voca->AreaNum;
    monitor_area.MonHStart   = Voca->PosX;
    monitor_area.MonVStart   = Voca->PosY;
    monitor_area.MonHSize    = Voca->Width;
    monitor_area.MonVSize    = Voca->Height;
    monitor_area.RamAddr     = Voca->RamAddr;
    monitor_area.Threshold   = Voca->Threshold;
    
    for (i = 0; i < R_WM_VOCA_CLUT_NUM; i++)
    {
        monitor_area.RefColor[i].RUpper = Voca->Clut[i].RUpper;
        monitor_area.RefColor[i].GUpper = Voca->Clut[i].GUpper;
        monitor_area.RefColor[i].BUpper = Voca->Clut[i].BUpper;
        monitor_area.RefColor[i].RLower = Voca->Clut[i].RLower;
        monitor_area.RefColor[i].GLower = Voca->Clut[i].GLower;
        monitor_area.RefColor[i].BLower = Voca->Clut[i].BLower;
    }
    /* Set Monitor Area */
    voca_ret = R_VOCA_MonitorAreaSet(R_VOCA_UNIT0, video_ch, &monitor_area);
    if (voca_ret != R_VOCA_ERR_OK)
    {
        ret = R_WM_SYS_NG;
    }
    else
    {   
        ret = R_WM_SYS_OK;
    }
    
    return ret;
}

/***********************************************************
  Function: loc_SetInternalRam

  This function executes R_VOCA_ColorRamSet from VOCA instance.
  
  Parameters:
  Voca     - VOCA instance.

  Return Value:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail
*/
R_WM_STATIC uint32_t loc_SetInternalRam(const r_wm_Voca_t *const Voca)
{
    uint32_t            ret;
    uint16_t            pixel_num;
    r_voca_Error_t      voca_ret;

    if (Voca->ExpSize != 0u)
    {
        /* Calculate pixel number  */
        pixel_num = Voca->ExpSize * LOC_PIXEL_PER_RAM_ADR;
        
        /* Set expected image in internal RAM */
        voca_ret = R_VOCA_ColorRamSet(
                    R_VOCA_UNIT0, Voca->RamAddr, pixel_num, Voca->ExpImg);
        if (voca_ret != R_VOCA_ERR_OK)
        {
            ret = R_WM_SYS_NG;
        }
        else
        {
            ret = R_WM_SYS_OK;
        }
    }
    else
    {
        /* Not update if size is 0 */
        ret = R_WM_SYS_OK;
    }
    
    return ret;
}


/***********************************************************
  Function: loc_ConvUsecToHwTime

  Convert from microseconds to H/W time for activity monitor.
  
  Parameters:
  UsecTime      - time in micorseconds.

  Return Value:
  H/W time. (100/3 usec units)
  
*/
R_WM_STATIC uint16_t loc_ConvUsecToHwTime(uint32_t UsecTime)
{
    uint32_t  hw_time;
    uint32_t  usec_time;
    
    /* Check upper limit */
    if (UsecTime > LOC_ACT_MON_MAX_USEC)
    {
        usec_time = LOC_ACT_MON_MAX_USEC;
    }
    else
    {
        usec_time = UsecTime;
    }
    
    /* Multiply by 3/100 with round-off */
    hw_time = ((usec_time * LOC_ACT_MON_CONV_COEF_A) 
                + (LOC_ACT_MON_CONV_COEF_B >> 1)) / LOC_ACT_MON_CONV_COEF_B;
    return (uint16_t)hw_time;
}


/***********************************************************
  Section: External Functions for WM SYS layer
*/
/***********************************************************
  Function: R_WM_Sys_DevVocaInit

  see: <r_sys_wm.h>
*/
uint32_t R_WM_Sys_DevVocaInit(const uint32_t Unit)
{
    uint32_t            ret = R_WM_SYS_OK;
    r_wm_DevSys_t       *dev_sys;
    r_voca_Error_t      voca_ret;

    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        if (Unit == R_WM_UNIT0)
        {
            /* Lock because GlobalStatusFlags can have asynchronous access. */
            /* e.g. Accessed from other unit control. */
            R_WM_Sys_LockDevice(Unit);
            
            /* Get device */
            dev_sys = R_WM_Sys_GetDevSys(Unit);
            if ((dev_sys->GlobalStatusFlags & R_WM_SYS_GLOBAL_FLAG_VOCA_INIT) == 0u)
            {
                /* Init VOCA driver when WM unit is 0. */
                /* Because S/W reset (VOCA H/W feature) is executed */
                /* by the VDCE Unit0 Vsync interrupt. */
                voca_ret = R_VOCA_Init(R_VOCA_UNIT0);
                if (voca_ret != R_VOCA_ERR_OK)
                {
                    ret = R_WM_SYS_NG;
                }
                else
                {
                    /* Set flag */
                    dev_sys->GlobalStatusFlags |= R_WM_SYS_GLOBAL_FLAG_VOCA_INIT;
                    
                    /* Set error callback */
                    voca_ret = R_VOCA_ErrorCallbackSet(Unit, &loc_VocaErrorCallback);
                    if (voca_ret != R_VOCA_ERR_OK)
                    {
                        ret = R_WM_SYS_NG;
                    }
                    else
                    {
                        /* Ebanle interrupt */
                        voca_ret = R_VOCA_IntEnable(R_VOCA_UNIT0);
                        if (voca_ret != R_VOCA_ERR_OK)
                        {
                            ret = R_WM_SYS_NG;
                        }
                        else
                        {
                            /* Do nothing */
                        }
                    }
                }
            }
            else
            {
                /* Allready Initialized, do nothing */
            }

            /* Unlock */
            R_WM_Sys_UnlockDevice(Unit);
        }
        else
        {
            /* Do nothing */
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_DevVocaDeInit

  see: <r_sys_wm.h>
*/
uint32_t R_WM_Sys_DevVocaDeInit(const uint32_t Unit)
{
    uint32_t            ret = R_WM_SYS_OK;
    r_wm_DevSys_t       *dev_sys;
    r_voca_Error_t      voca_ret;

    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        /* Lock because GlobalStatusFlags can have asynchronous access. */
        R_WM_Sys_LockDevice(Unit);
        
        /* Get flag of WM Unit0 (fixed) */
        dev_sys = R_WM_Sys_GetDevSys(R_WM_UNIT0);
        if ((dev_sys->GlobalStatusFlags & R_WM_SYS_GLOBAL_FLAG_VOCA_INIT) != 0u)
        {
            /* Disable interrupt */
            (void)R_VOCA_IntDisable(R_VOCA_UNIT0);
            
            /* DeInit the VOCA driver */
            voca_ret = R_VOCA_DeInit(R_VOCA_UNIT0);
            if (voca_ret != R_VOCA_ERR_OK)
            {
                ret = R_WM_SYS_NG;
            }
            else
            {
                /* Unset flag of Unit0 */
                dev_sys->GlobalStatusFlags &= ~R_WM_SYS_GLOBAL_FLAG_VOCA_INIT;
            }
        }
        else
        {
            /* Not Initialized, do nothing */
        }
        
        /* Unlock */
        R_WM_Sys_UnlockDevice(Unit);
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_IsrVcoa

  see: <r_sys_wm.h>
*/
void R_WM_Sys_IsrVcoa(void)
{
    uint32_t              wm_unit;
    r_wm_DevSys_t         *dev_sys;
    r_wm_Event_t          event;
    uint32_t              mask;
    r_voca_AreaStatus_t   int_factor = {0};
    
    /* Check initialized flag */
    dev_sys = R_WM_Sys_GetDevSys(R_WM_UNIT0);
    if ((dev_sys->GlobalStatusFlags & R_WM_SYS_GLOBAL_FLAG_VOCA_INIT) != 0u)
    {
        /* Get interrupt factor */
        (void)R_VOCA_StatusGet(R_VOCA_UNIT0, &int_factor);

        /* Clear interrupt factor */
        (void)R_VOCA_StatusClear(R_VOCA_UNIT0);

        for (wm_unit = R_WM_UNIT0; wm_unit < R_WM_DEV_NUM; wm_unit++)
        {
            /* Get glocal data */
            dev_sys = R_WM_Sys_GetDevSys(wm_unit);
            
            mask = (dev_sys->GlobalStatusFlags & R_WM_SYS_GLOBAL_MASK_VOCA_EN) 
                    >> R_WM_SYS_GLOBAL_BIT_VOCA00_EN;
            
            /* Check R_WM_EVENT_VOCA_MISMACTH */
            event.Id   = R_WM_EVENT_VOCA_MISMACTH;
            event.Data = int_factor.MonArea & mask;
            
            if (event.Data != 0u)
            {
                /* Execute callback */
                if ((dev_sys->RegEvents & 
                    (R_WM_SYS_FLAG_ON << (uint32_t)R_WM_EVENT_VOCA_MISMACTH)) != 0u)
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
            
            /* Check R_WM_EVENT_ACT_MON_ERROR */
            mask = R_WM_SYS_FLAG_ON << wm_unit;
            event.Id   = R_WM_EVENT_ACT_MON_ERROR;
            event.Data = int_factor.VoCh & mask;
            
            if (event.Data != 0u)
            {
                /* Execute callback */
                if ((dev_sys->RegEvents & 
                    (R_WM_SYS_FLAG_ON << (uint32_t)R_WM_EVENT_ACT_MON_ERROR)) != 0u)
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
    }
}


/***********************************************************
  Section: WM Driver Support Functions
  
  Comments see: <WM Support Functions>
*/

/***********************************************************
  Function: R_WM_Sys_DevRootVocaSet

  see: <r_wm_sys.h>
*/
void R_WM_Sys_DevRootVocaSet(const uint32_t        Unit, 
                             r_wm_Voca_t    *const RootVoca)
{
    r_wm_DevSys_t       *dev_sys;
    dev_sys = R_WM_Sys_GetDevSys(Unit);
    
    /* Set root pointer */
    dev_sys->RootVoca = RootVoca;
}

/***********************************************************
  Function: R_WM_Sys_ScreenVocaInit

  see: <r_wm_sys.h>
*/
uint32_t R_WM_Sys_ScreenVocaInit(const uint32_t Unit)
{
    uint32_t            ret = R_WM_SYS_OK;
    r_ddb_Timing_t      timing;
    r_vdce_Error_t      vdce_ret;
    uint32_t            h_offset;
    uint32_t            v_offset;
    r_voca_Error_t      voca_ret;
    r_voca_Param_t      voca_param;
    r_voca_VoCh_t       video_ch;

    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        /* Get display timing from VDCE driver */
        vdce_ret = R_VDCE_DisplayTimingGet(Unit, &timing);
        if (vdce_ret != R_VDCE_ERR_OK)
        {
            ret = R_WM_SYS_NG;
        }
        else
        {
            /* Check Horizontal front porch. VOCA needs one pixel. See H/W UM 40.3.3.1. */
            /* Vertical front porch and back porch are checked by VDCE driver. */
            if (timing.H.FrontPorch < LOC_MIN_H_FRONT_PORCH)
            {
                R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_DISPLAY_TIMING_SET, R_WM_SYS_NO_INFO);
                ret = R_WM_SYS_NG;
            }
            else
            {
                if ((timing.Flags & (uint32_t)R_DDB_DISP_FLAG_HSYNC_ACTHI) != 0u)
                {
                    /* active high timing means offset is backporch */
                    h_offset = timing.H.BlankWidth - timing.H.FrontPorch - timing.H.SyncWidth;
                }
                else
                {
                    /* active low timing means offset is sync + backporch */
                    h_offset = timing.H.BlankWidth - timing.H.FrontPorch;
                }
                if ((timing.Flags & (uint32_t)R_DDB_DISP_FLAG_VSYNC_ACTHI) != 0u)
                {
                    /* active high timing means offset is backporch */
                    v_offset = timing.V.BlankWidth - timing.V.FrontPorch - timing.V.SyncWidth;
                }
                else
                {
                    /* active low timing means offset is sync + backporch */
                    v_offset = timing.V.BlankWidth - timing.V.FrontPorch;
                }
                
                voca_param.HOffset = (uint16_t)h_offset;
                voca_param.VOffset = (uint16_t)v_offset;
                voca_param.HSize   = (uint16_t)timing.ScreenWidth;
                voca_param.VSize   = (uint16_t)timing.ScreenHeight;
                
                /* Set Video channel */
                if (Unit == R_WM_UNIT0)
                {
                    video_ch = R_VOCA_VO_0;
                }
                else
                {
                    video_ch = R_VOCA_VO_1;
                }
                
                /* Set display timing to VOCA driver */
                voca_ret = R_VOCA_ParamSet(R_VOCA_UNIT0, video_ch, &voca_param);
                if (voca_ret != R_VOCA_ERR_OK)
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
  Function: R_WM_Sys_ScreenVocaDeInit

  see: <r_wm_sys.h>
*/
uint32_t R_WM_Sys_ScreenVocaDeInit(const uint32_t Unit)
{
    uint32_t            ret = R_WM_SYS_OK;
    r_wm_DevSys_t       *dev_sys;
    uint32_t            mask;
    uint32_t            area_num;
    r_voca_VoCh_t       video_ch;
    r_voca_Error_t      voca_ret;

    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        /* Lock because GlobalStatusFlags can have asynchronous access. */
        R_WM_Sys_LockDevice(Unit);
        
        /* Get from chain */
        dev_sys  = R_WM_Sys_GetDevSys(Unit);
        
        /* Get all created Video output Monitor */
        mask  = (dev_sys->GlobalStatusFlags & R_WM_SYS_GLOBAL_MASK_VOCA_EN);
        if (mask != 0u)
        {
            area_num = mask >> R_WM_SYS_GLOBAL_BIT_VOCA00_EN;
            
            /* Disable all Video output Monitor */
            voca_ret = R_VOCA_VideoOutputCheckDisable(R_VOCA_UNIT0, area_num);
            if (voca_ret != R_VOCA_ERR_OK)
            {
                ret = R_WM_SYS_NG;
            }
            else
            {
                /* Clear enable flag */
                dev_sys->GlobalStatusFlags &= ~mask;
            }
        }
        else
        {
            /* Do nothing */
        }
        
        /* Set Video channel */
        if (Unit == R_WM_UNIT0)
        {
            video_ch = R_VOCA_VO_0;
        }
        else
        {
            video_ch = R_VOCA_VO_1;
        }
        
        /* Disable Activity monitor */
        voca_ret = R_VOCA_ActiveMonitorDisable(R_VOCA_UNIT0, video_ch);
        if (voca_ret != R_VOCA_ERR_OK)
        {
            ret = R_WM_SYS_NG;
        }
        else
        {
            /* Do nothing */
        }

        /* Unlock */
        R_WM_Sys_UnlockDevice(Unit);
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_ScreenVocaCreate

  see: <r_wm_sys.h>
*/
uint32_t R_WM_Sys_ScreenVocaCreate(const uint32_t                Unit,
                                   const r_wm_Voca_t      *const Voca)
{
    uint32_t            ret;
    r_wm_DevSys_t       *dev_sys;
    uint32_t            mask;
#if (R_WM_DEV_NUM > 1)
    r_wm_DevSys_t       *dev_sys_other;
    uint32_t            other_unit;
#endif

    if ((Unit >= R_WM_DEV_NUM) || (Voca == R_NULL) || (Voca->AreaNum >= R_WM_MAX_VOCA_MONITOR_AREA))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        mask = R_WM_SYS_FLAG_ON << ((uint32_t)Voca->AreaNum + R_WM_SYS_GLOBAL_BIT_VOCA00_EN);

        /* Lock because GlobalStatusFlags can have asynchronous access. */
        R_WM_Sys_LockDevice(Unit);
        
#if (R_WM_DEV_NUM > 1)
        /* Get flags from another unit */
        other_unit = (Unit == R_WM_UNIT0) ? R_WM_UNIT1 : R_WM_UNIT0;
        dev_sys_other = R_WM_Sys_GetDevSys(other_unit);
        
        /* Check duplication */
        if ((mask & dev_sys_other->GlobalStatusFlags) != 0u)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_SYS_VOCA_CREATE_FAILED, R_WM_SYS_NO_INFO);
            ret = R_WM_SYS_NG;
        }
        else
#endif
        {
            /* Set monitor area */
            ret = loc_SetMonitorArea(Unit, Voca);
            if (ret == R_WM_SYS_OK)
            {
                /* Set internal RAM */
                ret = loc_SetInternalRam(Voca);
                if (ret == R_WM_SYS_OK)
                {
                    /* Update flags of specified unit */
                    dev_sys = R_WM_Sys_GetDevSys(Unit);
                    dev_sys->GlobalStatusFlags |= mask;
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
        
        /* Unlock */
        R_WM_Sys_UnlockDevice(Unit);
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_ScreenVocaDelete

  see: <r_wm_sys.h>
*/
uint32_t R_WM_Sys_ScreenVocaDelete(const uint32_t                Unit,
                                   const r_wm_Voca_t      *const Voca)
{
    uint32_t            ret = R_WM_SYS_OK;
    r_wm_DevSys_t       *dev_sys;
    uint32_t            mask;
    
    /* Lock because GlobalStatusFlags can have asynchronous access. */
    R_WM_Sys_LockDevice(Unit);
    
    /* Disable Video output checker */
    ret = R_WM_Sys_ScreenVocaEnable(Unit, Voca, R_WM_FALSE);
    if (ret == R_WM_SYS_OK)
    {
        /*  Clear enable flag */
        dev_sys = R_WM_Sys_GetDevSys(Unit);
        mask = R_WM_SYS_FLAG_ON << ((uint32_t)Voca->AreaNum + R_WM_SYS_GLOBAL_BIT_VOCA00_EN);
        dev_sys->GlobalStatusFlags &= ~mask;
    }
    else
    {
        /* Do nothing */
    }

    /* Unlock */
    R_WM_Sys_UnlockDevice(Unit);
    
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_ScreenVocaEnable

  see: <r_wm_sys.h>
*/
uint32_t R_WM_Sys_ScreenVocaEnable(const uint32_t                Unit,
                                   const r_wm_Voca_t      *const Voca,
                                   const uint32_t                Enabled)
{
    uint32_t            ret = R_WM_SYS_OK;
    uint32_t            area_num;
    r_voca_Error_t      voca_ret;

    if ((Unit >= R_WM_DEV_NUM) || (Voca == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        /* Make area num */
        area_num = R_WM_SYS_FLAG_ON << (uint32_t)Voca->AreaNum;
        
        if (Enabled == R_WM_TRUE)
        {
            /* Enable Video output checker */
            voca_ret = R_VOCA_VideoOutputCheckEnable(R_VOCA_UNIT0, area_num);
            if (voca_ret != R_VOCA_ERR_OK)
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
            /* Disable Video output checker */
            voca_ret = R_VOCA_VideoOutputCheckDisable(R_VOCA_UNIT0, area_num);
            if (voca_ret != R_VOCA_ERR_OK)
            {
                ret = R_WM_SYS_NG;
            }
            else
            {
                /* Do nothing */
            }
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_ScreenVocaExpImgSet

  see: <r_wm_sys.h>
*/
uint32_t R_WM_Sys_ScreenVocaExpImgSet(const uint32_t                Unit,
                                      const r_wm_Voca_t      *const Voca,
                                      const uint32_t                Threshold,
                                      const uint16_t                RamAddr, 
                                      const uint16_t                ExpSize, 
                                      const uint16_t         *const ExpImg)
{
    uint32_t            ret = R_WM_SYS_OK;
    uint32_t            mask;
    r_wm_DevSys_t       *dev_sys;
    

    if ((Unit >= R_WM_DEV_NUM) || (Voca == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        /* Make flags to update monitor area */
        mask = 0u;

        if (ExpSize == 0u)
        {
            /* Not updated */
        }
        else
        {
            mask |= R_WM_SYS_FLAG_ON << ((uint32_t)Voca->AreaNum + R_WM_SYS_VOCA_BIT_AREA00_RAM);
        }
        if ((RamAddr == Voca->RamAddr) && (Threshold == Voca->Threshold))
        {
            /* Not updated */
        }
        else
        {
            /* Flag on to update monitor area  */
            mask |= R_WM_SYS_FLAG_ON << ((uint32_t)Voca->AreaNum + R_WM_SYS_VOCA_BIT_AREA00_REG);
        }
        
        /* Update Frame Status flag */
        dev_sys = R_WM_Sys_GetDevSys(Unit);
        dev_sys->VocaStatusFlags |= mask;
   }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_ScreenVocaClutSet

  see: <r_wm_sys.h>
*/
uint32_t R_WM_Sys_ScreenVocaClutSet(const uint32_t                    Unit, 
                                    r_wm_Voca_t                *const Voca,
                                    const uint8_t                     NumEntries,
                                    const r_wm_VocaClutEntry_t *const Clut)
{
    uint32_t            ret = R_WM_SYS_OK;
    r_wm_DevSys_t       *dev_sys;
    uint32_t            mask;

    if ((Unit >= R_WM_DEV_NUM) || (Voca == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        /* Flag on to update monitor area  */
        mask = R_WM_SYS_FLAG_ON << ((uint32_t)Voca->AreaNum + R_WM_SYS_VOCA_BIT_AREA00_REG);
        
        /* Update Frame Status flag */
        dev_sys = R_WM_Sys_GetDevSys(Unit);
        dev_sys->VocaStatusFlags |= mask;
        
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_ScreenActMonEnable

  see: <r_wm_sys.h>
*/
uint32_t R_WM_Sys_ScreenActMonEnable(const uint32_t                Unit,
                                     const uint32_t                UpperTime,
                                     const uint32_t                LowerTime,
                                     const uint32_t                Enabled)
{
    uint32_t            ret = R_WM_SYS_OK;
    uint16_t            max_time;
    uint16_t            min_time;
    r_voca_VoCh_t       video_ch;
    r_voca_Error_t      voca_ret;

    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        /* Set Video channel */
        if (Unit == R_WM_UNIT0)
        {
            video_ch = R_VOCA_VO_0;
        }
        else
        {
            video_ch = R_VOCA_VO_1;
        }
        
        if (Enabled == R_WM_TRUE)
        {
            /* Convert 0.033 ms unit */
            max_time = loc_ConvUsecToHwTime(UpperTime);
            min_time = loc_ConvUsecToHwTime(LowerTime);
            
            /* Enable activity monitor */
            voca_ret = R_VOCA_ActiveMonitorEnable(R_VOCA_UNIT0, video_ch, max_time, min_time);
            if (voca_ret != R_VOCA_ERR_OK)
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
            /* Disable activity monitor */
            voca_ret = R_VOCA_ActiveMonitorDisable(R_VOCA_UNIT0, video_ch);
            if (voca_ret != R_VOCA_ERR_OK)
            {
                ret = R_WM_SYS_NG;
            }
            else
            {
                /* Do nothing */
            }
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_ScreenVocaUpdate

  see: <r_wm_sys.h>
*/
uint32_t R_WM_Sys_ScreenVocaUpdate(const uint32_t  Unit)
{
    uint32_t            ret = R_WM_SYS_OK;
    r_wm_DevSys_t       *dev_sys;
    r_wm_Voca_t         *cur_voca;
    uint32_t            mask;
    uint32_t            i;

    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        /* Get global data */
        dev_sys = R_WM_Sys_GetDevSys(Unit);
        if ((dev_sys->VocaStatusFlags & R_WM_SYS_VOCA_MASK) != 0u)
        {
            /* Check VOCA chain */
            cur_voca = dev_sys->RootVoca;
            for (i = 0; i < R_WM_MAX_VOCA_MONITOR_AREA; i++)
            {
                if ((cur_voca == R_NULL) || (ret == R_WM_SYS_NG))
                {
                    break;
                }
                else
                {
                    /* Check update flag */
                    mask = R_WM_SYS_FLAG_ON 
                           << ((uint32_t)cur_voca->AreaNum + R_WM_SYS_VOCA_BIT_AREA00_REG);
                    if ((dev_sys->VocaStatusFlags & mask) != 0u)
                    {
                        /* Update monitor area */
                        ret = loc_SetMonitorArea(Unit, cur_voca);
                    }
                    else
                    {
                        /* Do nothing */
                    }
                    
                    if (ret == R_WM_SYS_OK)
                    {
                        /* Check update flag */
                        mask = R_WM_SYS_FLAG_ON 
                               << ((uint32_t)cur_voca->AreaNum + R_WM_SYS_VOCA_BIT_AREA00_RAM);
                        if ((dev_sys->VocaStatusFlags & mask) != 0u)
                        {
                            /* Update internal RAM */
                            ret = loc_SetInternalRam(cur_voca);
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
                    cur_voca = cur_voca->Next;
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
