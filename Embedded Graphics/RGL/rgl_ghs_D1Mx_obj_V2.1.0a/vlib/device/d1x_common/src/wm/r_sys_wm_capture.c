/*
****************************************************************************
PROJECT : WM
============================================================================ 
DESCRIPTION
WM system functions for D1x
============================================================================
                            C O P Y R I G H T
============================================================================
                       Copyright (c) 2013 - 2019
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
#include "r_vdce_api.h"
#include "r_spea_api.h"
#include "r_config_wm.h"
#include "r_wm_api.h"
#include "r_wm_sys.h"
#include "r_sys_wm.h"
#include "r_dev_pin_types.h"
#include "r_dev_api.h"

#if (defined(R_WM_MSQ_DEQUEUE_PROFILING) || defined(R_WM_MSQ_ENQUEUE_PROFILING))
#include "r_profile.h"
#endif

#ifdef R_WM_SYS_ERROR_TRACKING
#include "r_bsp_stdio_api.h"
#endif

/***********************************************************
  Section: Local Defines
*/

/***********************************************************
  Macro: VDCECTL
  
  VDCECTL register address.
*/
#define LOC_VDCECTL         (0xFFC0601Cu)

/***********************************************************
  Macro: LOC_VDCECTL_*
  
  VDCECTL register set and mask value.
*/

#define LOC_VDCECTL_VISL0   (1uL<<0)
#define LOC_VDCECTL_VISL1   (1uL<<1)
#define LOC_VDCECTL_PXSL    (1uL<<3)
#define LOC_VDCECTL_FIXED_1 (1uL<<16)
#define LOC_VDCECTL_MASK    (0x7Fu)

/***********************************************************
  Macro: LOC_CAPMODE_UNMASK
  
  Unmask of r_wm_CapMode_t type value.
*/
#define LOC_CAPMODE_UNMASK   (0xF0000000u) 

/***********************************************************
  Constants: Byte per pixel
*/

#define LOC_4BYTE_PER_PIXEL  (4u)
#define LOC_2BYTE_PER_PIXEL  (2u)

/***********************************************************
  Constants: Buffer address
*/
#define LOC_BUF_UNUSED       (0u)

/***********************************************************
  Constants: Buffer number
*/

#define LOC_BUF_NUM_1           (1u) 

/***********************************************************
  Constants: Buffer stride
*/

#define LOC_BUF_STRIDE_X2       (2u) 

/***********************************************************
  Constants: Control interrupt number
*/
#define LOC_WM_CAPTURE_INT_NUM  (2u)


/***********************************************************
  Section: Global Variables
*/

/***********************************************************
  Section: Local Function Declarations
*/

/***********************************************************
  Section: Local Functions
*/

/***********************************************************
  Function: R_WM_Sys_DevRootCaptureSet

  see: <r_wm_sys.h>
*/
void R_WM_Sys_DevRootCaptureSet(const uint32_t        CapUnit, 
                                r_wm_Capture_t *const RootCapt)
{
    r_wm_DevSys_t *dev_sys;

    if (CapUnit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(CapUnit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
    }
    else
    {
        dev_sys = R_WM_Sys_GetDevSys(CapUnit);
        dev_sys->RootCapt = RootCapt;
    }
}

/***********************************************************
  Function: loc_SetVideoInputPath
*/
R_WM_STATIC void loc_SetVideoInputPath(const r_wm_Capture_t *const Capt);

R_WM_STATIC void loc_SetVideoInputPath(const r_wm_Capture_t *const Capt)
{
    uint32_t val;

    if ((((uint32_t)Capt->Mode & (uint32_t)R_WM_CAPMODE_YUV_ITU656) != 0u) ||
        (((uint32_t)Capt->Mode & (uint32_t)R_WM_CAPMODE_YUV_8BIT) != 0u))
    {
        /*
            setup the HW for the ITU mode:
            select ITU656 (CLK + DATA7..0) for analog camera connected to SAA71xx
        */
        val = R_DEV_READ_REG(32, LOC_VDCECTL); 
        if (Capt->CapUnit == R_WM_UNIT1)
        {
            val &= ~LOC_VDCECTL_VISL1;
        }
        else if (Capt->CapUnit == R_WM_UNIT0)
        {
            val &= ~LOC_VDCECTL_VISL0;
            val &= ~LOC_VDCECTL_PXSL;
        }
        else
        {
            /* Not avialable */
        }
        R_DEV_WRITE_REG(32, LOC_VDCECTL, ((val & LOC_VDCECTL_MASK) | LOC_VDCECTL_FIXED_1));    
    }
    else
    {
        /*  setup the HW for the RGB888 mode:*/
        val = R_DEV_READ_REG(32, LOC_VDCECTL);
        if (Capt->CapUnit == R_WM_UNIT1)
        {
            val |= LOC_VDCECTL_VISL1;
        }
        else if (Capt->CapUnit == R_WM_UNIT0)
        {
            val |= LOC_VDCECTL_VISL0;
        }
        else
        {
            /* Not avialable */
        }
        R_DEV_WRITE_REG(32, LOC_VDCECTL, ((val & LOC_VDCECTL_MASK) | LOC_VDCECTL_FIXED_1));
        
    }
}

/***********************************************************
  Function: loc_SetVdceLayer
  
*/
R_WM_STATIC uint32_t loc_SetVdceLayer(const uint32_t              Unit, 
                                      const r_wm_Capture_t *const Capt, 
                                      const uint32_t              Layer);

R_WM_STATIC uint32_t loc_SetVdceLayer(const uint32_t              Unit, 
                                      const r_wm_Capture_t *const Capt, 
                                      const uint32_t              Layer)
{
    uint32_t        ret_val = R_WM_SYS_OK;
    r_vdce_Error_t  vdce_res;
    
    /* Set delay */
    vdce_res = R_VDCE_LayerVSyncDelaySet(Unit, Layer, Capt->Delay);
    if (vdce_res != R_VDCE_ERR_OK)
    {
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        if ((((uint32_t)Capt->Mode & (uint32_t)R_WM_CAPMODE_YUV_ITU656) != 0u) ||
            (((uint32_t)Capt->Mode & (uint32_t)R_WM_CAPMODE_YUV_8BIT) != 0u))
        {
            /* Set Matrix */
            vdce_res = R_VDCE_LayerMatrixBT601Set(Capt->CapUnit, R_VDCE_LAYER_INPUT);
            if (vdce_res != R_VDCE_ERR_OK)
            {
                ret_val = R_WM_SYS_NG;
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
    
    return ret_val;
}

/***********************************************************
  Function: loc_SetBufGeometry
  
*/
R_WM_STATIC uint32_t loc_SetBufGeometry(const uint32_t              Unit, 
                                        const r_wm_Capture_t *const Capt);

R_WM_STATIC uint32_t loc_SetBufGeometry(const uint32_t              Unit, 
                                        const r_wm_Capture_t *const Capt)
{
    uint32_t                        ret_val = R_WM_SYS_OK;
    uint32_t                        byte_per_pixel = 0u;
    r_vdce_Fbformat_t               fb_fmt = R_VDCE_FB_FORMAT_RGB0888;
    uint32_t                        buf_addr;
    uint32_t                        first_addr;
    uint32_t                        second_addr;
    uint32_t                        stride;
    r_vdce_CapRate_t                rate;
    r_vdce_CapField_t               field;
    r_vdce_Error_t                  vdce_res;
    r_wm_DevSys_t                   *dev_sys;
    
    /* Buffer format */
    switch (Capt->Window->ColorFmt)
    {
    case R_WM_COLORFMT_RGB0888:
    case R_WM_COLORFMT_ARGB8888:
        fb_fmt         = R_VDCE_FB_FORMAT_RGB0888;
        byte_per_pixel = LOC_4BYTE_PER_PIXEL;
        break;
    case R_WM_COLORFMT_RGB565:
        fb_fmt         = R_VDCE_FB_FORMAT_RGB565;
        byte_per_pixel = LOC_2BYTE_PER_PIXEL;
        break;
    default:
        ret_val = R_WM_SYS_NG;
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
        break;
    }
    
    if (ret_val == R_WM_SYS_NG)
    {
        /* Do nothing */
    }
    else
    {
        /* Convert type to adjust VDCE API specification */
        buf_addr = (uint32_t)(Capt->Window->Surface.Fb.Buffer[0].Data);  /* PRQA S 306  */
        stride   = Capt->StrideX;
        
        first_addr  = buf_addr;
        second_addr = LOC_BUF_UNUSED;
        
        /* Check interlace input */
        if ((((uint32_t)Capt->Mode & (uint32_t)R_WM_CAPMODE_YUV_ITU656) != 0u) ||
            (((uint32_t)Capt->Mode & (uint32_t)R_WM_CAPMODE_YUV_8BIT) != 0u))
        {
            if (((uint32_t)Capt->Mode & (uint32_t)R_WM_CAPMODE_BOB_TOP) != 0u)
            {
                rate        = R_VDCE_CAP_RATE_PER2;
                field       = R_VDCE_CAP_FIELD_TOP;
            }
            else if (((uint32_t)Capt->Mode & (uint32_t)R_WM_CAPMODE_BOB_BOTTOM) != 0u)
            {
                rate        = R_VDCE_CAP_RATE_PER2;
                field       = R_VDCE_CAP_FIELD_BOTTOM;
            }
            else if (((uint32_t)Capt->Mode & (uint32_t)R_WM_CAPMODE_WEAVE) != 0u)
            {
                /*
                  Interlace issue:
                  With ITU656 and the PAL/NTSC standard field 2 (bottom field) has an 
                  extra line in the blanking period therefore capture for this field must 
                  be aligned correctly with field 1 (top field)
                
                    interlaced view (PAL):
                    Top field    - blank line 22
                    Bottom field - blank line 23
                    Top field    - Video line 1
                    Bottom field - Video line 2
                    Top field    - Video line 3
                    Bottom field - Video line 4
                 Since each field is captured independently and referenced from a line 
                 number we have (in memory)
                        (Address)                  (Address + 1)
                    Top field - blank line 22   Bottom field - blank 22
                    Top field - Video line 1    Bottom field - blank 23 
                    Top field - Video line 3    Bottom field - Video line 2 
                                                Bottom field - Video line 4
                 When viewed on a monitor we end up with a synchronisation issue..
                    Top field    - blank line 22
                    Bottom field - blank line 22
                    Top field    - Video line 1
                    Bottom field - blank line 23 (extra blank line for bottom field)
                    Top field    - Video line 3
                    Bottom field - Video line 2
                 A simple solution is to set the top field capture address to occur after 
                 the bottom field, effectively shifting the top field image to align 
                 correctly...
                    Bottom field - blank line 22
                    Top field    - blank line 22
                    Bottom field - blank line 23 (extra blank line for bottom field)
                    Top field    - Video line 1
                    Bottom field - Video line 2
                    Top field    - Video line 3
                */
                stride      = Capt->StrideX * LOC_BUF_STRIDE_X2;
                
                second_addr = buf_addr;
                first_addr  = buf_addr + (Capt->StrideX * byte_per_pixel);
                rate        = R_VDCE_CAP_RATE_PER1;
                field       = R_VDCE_CAP_FIELD_TOP;
            }
            else /* Write both field to same buffer */
            {
                rate        = R_VDCE_CAP_RATE_PER1;
                field       = R_VDCE_CAP_FIELD_TOP;
            }
        }
        else /* Progressive */
        {
            if (((uint32_t)Capt->Mode & (uint32_t)R_WM_CAPMODE_BOB_TOP) != 0u)
            {
                rate        = R_VDCE_CAP_RATE_PER2;
                field       = R_VDCE_CAP_FIELD_FRAME;
            }
            else
            {
                rate        = R_VDCE_CAP_RATE_PER1;
                field       = R_VDCE_CAP_FIELD_FRAME;
            }
        }
        
        /* Set buffer geometry and capture position */
        vdce_res = R_VDCE_CapBufGeometrySetup(
                        Capt->CapUnit,
                        first_addr, 
                        second_addr,
                        stride,
                        Capt->Width,
                        Capt->Height,
                        Capt->StartX,
                        Capt->StartY1,
                        fb_fmt);
        /* Cehck VDCE result */
        if (vdce_res != R_VDCE_ERR_OK)
        {
            ret_val = R_WM_SYS_NG;
        }
        else
        {
            /* Set Writing rate */
            vdce_res = R_VDCE_CapRateSet(Capt->CapUnit, rate, field);
            if (vdce_res != R_VDCE_ERR_OK)
            {
                ret_val = R_WM_SYS_NG;
            }
            else
            {
                /* Lock because GlobalStatusFlags can have asynchronous access.  */
                /* e.g. Accessed from other unit control. */
                R_WM_Sys_LockDevice(Unit);
                
                /* Set global flag to "Unit" side */
                dev_sys = R_WM_Sys_GetDevSys(Capt->CapUnit);
                if (rate == R_VDCE_CAP_RATE_PER1)
                {
                    dev_sys->GlobalStatusFlags &= ~R_WM_SYS_GLOBAL_FLAG_CAPT_BOB;
                }
                else
                {
                    dev_sys->GlobalStatusFlags |= R_WM_SYS_GLOBAL_FLAG_CAPT_BOB;
                }
                
                R_WM_Sys_UnlockDevice(Unit);
            }
        }
    }
    
    return ret_val;

}

/***********************************************************
  Function: R_WM_Sys_CaptureCreate
  see<R_WM_Sys_.h>
*/
uint32_t R_WM_Sys_CaptureCreate(const uint32_t              Unit, 
                                const r_wm_Capture_t *const Capt)
{
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    uint32_t                        ret_val;
    r_vdce_Error_t                  vdce_res;
    r_vdce_CapMode_t                cap_mode;
    
    if ((Unit >= R_WM_DEV_NUM) || (Capt == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        /*
          need to reconf all the windows 
          because the layer assignment needs to start from the layer 0 
          where the VIN can be output to in case of the same VDCE channel for the VIN and VOUT
        */
        ret_val = R_WM_Sys_ReconfAllWindow(Unit);
        if (ret_val == R_WM_SYS_NG)
        {
            /* Do nothing */
        }
        else
        {
            /* get rid of 4 WM internal MSBs that do not map to VDCE opts */
            cap_mode 
            = (r_vdce_CapMode_t)((uint32_t)Capt->Mode & (~LOC_CAPMODE_UNMASK)); /* PRQA S 4394  */
        
            if (((uint32_t)Capt->Mode & (uint32_t)R_WM_CAPMODE_SYNC_ONLY) != 0u)
            {
                /* Set up H/W outside VDCE */
                loc_SetVideoInputPath(Capt);
                
                /* Set capture mode */
                vdce_res = R_VDCE_CapModeSet(Capt->CapUnit, cap_mode);
                if (vdce_res != R_VDCE_ERR_OK)
                {
                    ret_val = R_WM_SYS_NG;
                }
                else
                {
                    ret_val = R_WM_SYS_OK;
                }
            }
            else
            {
                win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Capt->Window);
                if (win_layer_pair == R_NULL)
                {
                    R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, R_WM_SYS_NO_INFO);
                    ret_val = R_WM_SYS_NG;
                }
                /* WEAVE Deinterlacing can work only single buffer */
                else if ((((uint32_t)Capt->Mode & (uint32_t)R_WM_CAPMODE_WEAVE) != 0u) &&
                         (Capt->Window->Surface.Fb.BufNum > LOC_BUF_NUM_1))
                {
                    R_WM_SYS_REPORT_ERROR(
                        Unit, R_WM_ERR_SYS_CAPTURE_CREATE_FAILED, R_WM_SYS_NO_INFO);
                    ret_val = R_WM_SYS_NG;
                }
                else
                {
                    /* Set up H/W outside VDCE */
                    loc_SetVideoInputPath(Capt);
                    
                    /* Set capture mode */
                    vdce_res = R_VDCE_CapModeSet(Capt->CapUnit, cap_mode);
                    if (vdce_res != R_VDCE_ERR_OK)
                    {
                        ret_val = R_WM_SYS_NG;
                    }
                    else
                    {
                        /* Set capture buffer and viewport */
                        ret_val = loc_SetBufGeometry(Unit, Capt);
                        if (ret_val == R_WM_SYS_NG)
                        {
                            /* Do nothing */
                        }
                        else
                        {
                            /* Set VDCE Layer feature and callback */
                            ret_val = loc_SetVdceLayer(Unit, Capt, win_layer_pair->Layer);
                        }
                    }
                }
            }
        }
    }
    return ret_val;
}

/***********************************************************
  Function: R_WM_Sys_CaptureDelete
  see<r_wm_sys.h>
*/
uint32_t R_WM_Sys_CaptureDelete(const uint32_t              Unit, 
                                const r_wm_Capture_t *const Capt)
{
    uint32_t ret_val;
    
    ret_val = R_WM_Sys_CaptureEnable(Unit, Capt, R_WM_FALSE);
    return ret_val;
}

/***********************************************************
  Function: R_WM_Sys_CaptureEnable
  see<r_wm_sys.h>
*/
uint32_t R_WM_Sys_CaptureEnable(const uint32_t              Unit, 
                                const r_wm_Capture_t *const Capt, 
                                const uint32_t              Enabled)
{
    uint32_t            ret_val  = R_WM_SYS_OK;
    r_wm_DevSys_t       *dev_sys_cap;
    r_wm_DevSys_t       *dev_sys_disp;
    r_vdce_Error_t      vdce_res;
    uint32_t            layer;
    uint32_t            skip_flag = R_WM_FALSE;
    uint32_t            i;
    r_wm_EventId_t      capture_int[LOC_WM_CAPTURE_INT_NUM] = {
                            R_WM_EVENT_VI_VBLANK,
                            R_WM_EVENT_VI_OVERFLOW
                        };
    
    if ((Unit >= R_WM_DEV_NUM) || (Capt == R_NULL) || (Capt->CapUnit >= R_WM_DEV_NUM))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        if (Unit == Capt->CapUnit)
        {
            layer = R_VDCE_LAYER_SCALER0;
        }
        else
        {
            layer = R_VDCE_LAYER_SCALER1;
        }
        
        /* Lock because GlobalStatusFlags can have asynchronous access.  */
        /* e.g. Accessed from other unit control. */
        R_WM_Sys_LockDevice(Unit);
        
        dev_sys_disp = R_WM_Sys_GetDevSys(Unit);
        dev_sys_cap  = R_WM_Sys_GetDevSys(Capt->CapUnit);
        
        if (Enabled == R_WM_TRUE)
        {
            /* Check enable flag */
            if ((dev_sys_cap->GlobalStatusFlags & R_WM_SYS_GLOBAL_FLAG_CAPT_EN) == 0u)
            {
                skip_flag = R_WM_FALSE;
            }
            else
            {
                /* Already enabled, Do nothing  */
                skip_flag = R_WM_TRUE;
            }
        }
        else
        {
            /* Check enable flag */
            if ((dev_sys_cap->GlobalStatusFlags & R_WM_SYS_GLOBAL_FLAG_CAPT_EN) != 0u)
            {
                skip_flag = R_WM_FALSE;
            }
            else
            {
                /* Already disabled, Do nothing  */
                skip_flag = R_WM_TRUE;
            }
        }
        
        if (skip_flag == R_WM_FALSE)
        {
            for (i = 0u; i < LOC_WM_CAPTURE_INT_NUM; i++)
            {
                /* Enable/Disable interrupt */
                ret_val = R_WM_Sys_EnableEvent(Capt->CapUnit, capture_int[i], Enabled);
                if (ret_val != R_WM_SYS_OK)
                {
                    break;
                }
                else
                {
                    /* Do nothing */
                }
            }
            
            if (ret_val == R_WM_SYS_OK)
            {
                if (Enabled == R_WM_TRUE)
                {
                    /* Capture enable */
                    vdce_res = R_VDCE_CapEnable(Capt->CapUnit, Unit);
                    if (vdce_res != R_VDCE_ERR_OK)
                    {
                        ret_val = R_WM_SYS_NG;
                    }
                    else
                    {
                        /* Set capture flag to link layer */
                        dev_sys_disp->HwLayerEnabled[layer] |= R_WM_SYS_LAYER_FLAG_CAPT_EN;
                        dev_sys_disp->CaptPending[layer] = R_WM_FALSE;
                        
                        dev_sys_cap->GlobalStatusFlags |= R_WM_SYS_GLOBAL_FLAG_CAPT_EN;
                        dev_sys_cap->CaptCount          = 0u;
                    }
                }
                else
                {
                    /* Disable callback */
                    vdce_res = R_VDCE_CapDisable(Capt->CapUnit);
                    if (vdce_res != R_VDCE_ERR_OK)
                    {
                        ret_val = R_WM_SYS_NG;
                    }
                    else
                    {
                        /* Unset capture flag to link layer */
                        dev_sys_disp->HwLayerEnabled[layer] 
                         &= (uint8_t)~R_WM_SYS_LAYER_FLAG_CAPT_EN;

                        dev_sys_cap->GlobalStatusFlags &= ~R_WM_SYS_GLOBAL_FLAG_CAPT_EN;
                    }
                }
            }
            else
            {
                /* Do nothing */
            }
        }

        /* Lock because GlobalStatusFlags can have asynchronous access.  */
        /* e.g. Accessed from other unit control. */
        R_WM_Sys_UnlockDevice(Unit);
        
    }
    return ret_val;
}

/***********************************************************
  Function: R_WM_Sys_CaptureViewPortSet
  
  see<r_wm_sys.h>
*/

uint32_t R_WM_Sys_CaptureViewPortSet(const uint32_t              Unit, 
                                     const r_wm_Capture_t *const Capt,
                                     const uint32_t              StartX,
                                     const uint32_t              StartY,
                                     const uint32_t              Width,
                                     const uint32_t              Height)
{
    uint32_t            ret_val;
    r_vdce_Error_t      vdce_res;
    
    if ((Unit >= R_WM_DEV_NUM) || (Capt == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        /* Set view port */
        vdce_res = R_VDCE_CapViewPortSet(Capt->CapUnit, Width, Height, StartX, StartY);
        if (vdce_res != R_VDCE_ERR_OK)
        {
            ret_val = R_WM_SYS_NG;
        }
        else
        {
            ret_val = R_WM_SYS_OK;
        }
    }
    return ret_val;
}

/***********************************************************
  Function: R_WM_Sys_CaptureExtVsyncSet
  
  see<r_wm_sys.h>
*/
uint32_t R_WM_Sys_CaptureExtVsyncSet(const uint32_t              Unit, 
                                     const r_wm_Capture_t *const Capt,
                                     const uint16_t              HsyncCycle,
                                     const uint32_t              VsyncMaskUs,
                                     const uint32_t              VsyncLackUs)
{
    uint32_t            ret_val;
    r_vdce_Error_t      vdce_res;
    
    if ((Unit >= R_WM_DEV_NUM) || (Capt == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        /* Set view port */
        vdce_res = R_VDCE_CapExtVsyncSet(Capt->CapUnit, HsyncCycle, VsyncMaskUs, VsyncLackUs);
        if (vdce_res != R_VDCE_ERR_OK)
        {
            ret_val = R_WM_SYS_NG;
        }
        else
        {
            ret_val = R_WM_SYS_OK;
        }
    }
    return ret_val;
}

