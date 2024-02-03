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

#ifdef USE_ROS
#include "r_os_api.h"
#endif

#if defined R_WM_MSQ_DEQUEUE_PROFILING || defined R_WM_MSQ_ENQUEUE_PROFILING
#include "r_profile.h"
#endif

#ifdef R_WM_SYS_ERROR_TRACKING
#include "r_bsp_stdio_api.h"
#endif

/***********************************************************
  Section: Local Defines
*/


/***********************************************************
  Constants: Alpha value
*/

#define LOC_ALPHA_255               (0xFFu)
#define LOC_ALPHA_0                 (0x00u)

/***********************************************************
  Constants: Align check for RLE
*/

#define LOC_BUF_ALIGN_CHECK         (0x0000007Fu) /* 128 byte align */

/***********************************************************
  Constants: CLUT index
*/

#define LOC_CLUT_INDEX_0            (0u)

/***********************************************************
  Constants: Value for image scaling 
*/

#define LOC_SCALING_MIN_VAL         (4u)
#define LOC_SCALING_ERR_VAL         (0u)

/***********************************************************
  Constants: Memory position
*/

#define LOC_MEM_POS_X               (0) /* used as signed */
#define LOC_MEM_POS_Y               (0) /* used as signed */

/***********************************************************
  Constants: Layer number for capture
*/

#define LOC_CAP_LAYER_NUM           (2u)


/***********************************************************
  Section: Local Variables
*/
/***********************************************************
  Variable : r_wm_DevCabps
  
  Sys layer device object.
*/

#ifdef R_BSP_SYS_INIT_USE
static r_wm_WinCapbs_t r_wm_DevCabps[R_WM_MAX_LAYERS];
#else
static r_wm_WinCapbs_t r_wm_DevCabps[R_WM_MAX_LAYERS] = {
    /* Default features of VDCE HW layers */
    R_WM_WINCAPBS_RLE, R_WM_WINCAPBS_SPRITES, R_WM_WINCAPBS_SPRITES, R_WM_WINCAPBS_SPRITES 
};
#endif

/***********************************************************
  Section: Local Function Declarations
*/

R_WM_STATIC r_vdce_Format_t loc_GetVdceFormat(const r_wm_WinColorFmt_t Format);

R_WM_STATIC uint32_t loc_CheckHwLayerCapabilities(const uint32_t             Unit,
                                                  const uint32_t             Layer,
                                                  const r_wm_Window_t *const Win);

R_WM_STATIC uint32_t loc_WinSetViewportAndGeom(const uint32_t             Unit,
                                               const r_wm_Window_t *const Win,
                                               const uint32_t             Layer,
                                               const int32_t              PosX,
                                               const int32_t              PosY,
                                               const uint32_t             Pitch,
                                               const uint32_t             Width,
                                               const uint32_t             Height);

R_WM_STATIC uint32_t loc_WinSetFb(const uint32_t             Unit,
                                  const r_wm_Window_t *const Win,
                                  const int32_t              PosX,
                                  const int32_t              PosY,
                                  const void *const          Fb);

R_WM_STATIC uint32_t loc_VdceLayerEnable(const uint32_t             Unit, 
                                         const uint32_t             Layer);

R_WM_STATIC uint32_t loc_VdceLayerDisable(const uint32_t  Unit, 
                                          const uint32_t  Layer,
                                          uint32_t *const Disabled);

R_WM_STATIC uint32_t loc_WindowDelete(const uint32_t             Unit, 
                                      const uint32_t             Layer);

R_WM_STATIC uint32_t loc_WindowIsYuvLayer(const r_wm_Window_t *const Window);

R_WM_STATIC r_wm_WinCapbs_t loc_SysWinCapabilitiesGet(const uint32_t Unit, 
                                                      const uint32_t Layer);

R_WM_STATIC uint32_t loc_WindowIsOnScreen(const uint32_t             Unit, 
                                          const r_wm_Window_t *const Win);

R_WM_STATIC uint32_t loc_FindMatchLayer(const uint32_t             Unit, 
                                        const uint32_t             Layer, 
                                        const r_wm_Window_t *const CurWin);

R_WM_STATIC uint32_t loc_SetAlphaBlending(const uint32_t             Unit, 
                                          const uint32_t             Layer, 
                                          const r_wm_Window_t *const CurWin);

R_WM_STATIC uint32_t loc_SetLayerFormat(const uint32_t             Unit, 
                                        const uint32_t             Layer, 
                                        const r_wm_Window_t *const CurWin);
                                        
R_WM_STATIC uint32_t loc_SetLayerScaling(const uint32_t             Unit,
                                         const uint32_t             Layer,
                                         const r_wm_Window_t *const CurWin,
                                         const r_wm_ScaleChg_t      ChangeMode,
                                         const uint32_t             ScaledWidth,
                                         const uint32_t             ScaledHeight);

R_WM_STATIC uint32_t loc_SetLayerBaseAdrs(const uint32_t             Unit, 
                                          const uint32_t             Layer, 
                                          const r_wm_Window_t *const CurWin);

R_WM_STATIC uint32_t loc_SetupVdceLayer(const uint32_t             Unit, 
                                        const uint32_t             Layer, 
                                        const r_wm_Window_t *const CurWin);

R_WM_STATIC uint32_t loc_CheckLayer0Assignment(const r_wm_Window_t *const Win);

R_WM_STATIC uint32_t loc_CheckCaptureAssignment(const uint32_t             Unit,
                                                const r_wm_Window_t *const Win,
                                                const uint32_t             Layer);

R_WM_STATIC void     loc_LockLayerUpdate(const uint32_t             Unit, 
                                         const uint32_t             Layer,
                                         const uint8_t              Lock);

/***********************************************************
  Function: loc_GetVdceFormat.
  
  Description:
  Returns the frame buffer format according to the color mode of a window. 
  
  Parameters: 
  Format - The colour mode of a window. 
  
  Return Value: 
  <r_vdce_Format_t>
*/
R_WM_STATIC r_vdce_Format_t loc_GetVdceFormat(const r_wm_WinColorFmt_t Format)
{
    r_vdce_Format_t  vdce_format;

    switch (Format) 
    {
        case R_WM_COLORFMT_RGB565:
            vdce_format = R_VDCE_RGB565;
            break;
        case R_WM_COLORFMT_ARGB1555:
            vdce_format = R_VDCE_ARGB1555;
            break;
        case R_WM_COLORFMT_RGBA5551:
            vdce_format = R_VDCE_RGBA5551;
            break;
        case R_WM_COLORFMT_ARGB4444:
            vdce_format = R_VDCE_ARGB4444;
            break;
        case R_WM_COLORFMT_RGB0888:
        case R_WM_COLORFMT_RLE24RGB0888:
            vdce_format = R_VDCE_RGB0888;
            break;
        case R_WM_COLORFMT_ARGB8888:
        case R_WM_COLORFMT_RLE24ARGB8888:
            vdce_format = R_VDCE_ARGB8888; 
            break;
        case R_WM_COLORFMT_RGBA8888:
            vdce_format = R_VDCE_RGBA8888;
            break;
        case R_WM_COLORFMT_CLUT8:
            vdce_format = R_VDCE_CLUT8;
            break;
        case R_WM_COLORFMT_CLUT4:
            vdce_format = R_VDCE_CLUT4;
            break;
        case R_WM_COLORFMT_CLUT1:
            vdce_format = R_VDCE_CLUT1;
            break;
        case R_WM_COLORFMT_YCBCR_422:
            vdce_format = R_VDCE_YCBCR_422;
            break;
        case R_WM_COLORFMT_YCBCR_444:
            vdce_format = R_VDCE_YCBCR_444;
            break;
        case R_WM_COLORFMT_YUV_YUYV:
            vdce_format = R_VDCE_YUV_YUYV;
            break;
        case R_WM_COLORFMT_YUV_UYVY:
            vdce_format = R_VDCE_YUV_UYVY;
            break;
        case R_WM_COLORFMT_YUV_YVYU:
            vdce_format = R_VDCE_YUV_YVYU;
            break;
        case R_WM_COLORFMT_YUV_VYUY:
            vdce_format = R_VDCE_YUV_VYUY;
            break;
        case R_WM_COLORFMT_RGB0444:
        case R_WM_COLORFMT_RGBA4444:
        case R_WM_COLORFMT_RLE18ARGB8888:
        case R_WM_COLORFMT_RLE18RGB0888:
        case R_WM_COLORFMT_RLE8CLUT8:
        case R_WM_COLORFMT_RLE8CLUT4:
        case R_WM_COLORFMT_RLE8CLUT1:
        default:
            /* Not implemented */
            vdce_format = R_VDCE_FORMAT_LAST;
            break;
    }
    /* End of switch. */
    return vdce_format;
}

/***********************************************************
  Function: loc_WindowIsYuvLayer
  
  Description:
  Returns the value 0 or 1 according to the color mode of a window.
  
  Parameters: 
  Window - Pointer to the window structure. See <r_wm_Window_t> for more details.
  
  Return Value: 
  uint32_t
*/
R_WM_STATIC uint32_t loc_WindowIsYuvLayer(const r_wm_Window_t *const Window)
{
    uint32_t ret_val;
    
    switch (Window->ColorFmt)
    {
        case R_WM_COLORFMT_YUV_YUYV:
        case R_WM_COLORFMT_YUV_UYVY:
        case R_WM_COLORFMT_YUV_YVYU:
        case R_WM_COLORFMT_YUV_VYUY:
        case R_WM_COLORFMT_YCBCR_422:
        case R_WM_COLORFMT_YCBCR_444:
            ret_val = R_WM_TRUE;
            break;
        default:
            ret_val = R_WM_FALSE;
            break;
    }
    return ret_val;
}

/***********************************************************
  Function: loc_WindowIsRle
  
  Description:
  Return that specified window is RLE window or not.
  
  Parameters:
  Win    - Pointer to Window structure. 

  Return value: 
  R_WM_TRUE  - RLE window
  R_WM_FALSE - Non RLE window
*/
R_WM_STATIC uint32_t loc_WindowIsRle(const r_wm_Window_t  *const Win);

R_WM_STATIC uint32_t loc_WindowIsRle(const r_wm_Window_t  *const Win)
{
    uint32_t ret_val;
    
    if ((Win->ColorFmt == R_WM_COLORFMT_RLE24ARGB8888) ||
        (Win->ColorFmt == R_WM_COLORFMT_RLE18ARGB8888) ||
        (Win->ColorFmt == R_WM_COLORFMT_RLE24RGB0888)  ||
        (Win->ColorFmt == R_WM_COLORFMT_RLE18RGB0888)  ||
        (Win->ColorFmt == R_WM_COLORFMT_RLE8CLUT8)     ||
        (Win->ColorFmt == R_WM_COLORFMT_RLE8CLUT4)     ||
        (Win->ColorFmt == R_WM_COLORFMT_RLE8CLUT1))
    {
        ret_val = R_WM_TRUE;
    }
    else
    {
        ret_val = R_WM_FALSE;
    }
    return ret_val;
}


/***********************************************************
  Function: R_WM_Sys_GetLayerPairByWin
  
  Description:
  Returns the map for which window is assigned to each layer.
  
  Parameters: 
  Unit - Specifies the WM unit number.
  Win  - Pointer to the window structure. See <r_wm_Window_t> for more details.
  
  Return Value: 
  <r_wm_sys_WinLayerPair_t> 
*/
r_wm_sys_WinLayerPair_t *R_WM_Sys_GetLayerPairByWin(const uint32_t             Unit, 
                                                    const r_wm_Window_t *const Win)
{
    r_wm_sys_WinLayerPair_t      *ret = R_NULL;
    uint32_t                     i;
    r_wm_DevSys_t                *dev_sys = R_WM_Sys_GetDevSys(Unit);
    
    if (Win == R_NULL)
    {
        ret = R_NULL;
    }
    else
    {
        for (i = 0u; i < R_WM_MAX_LAYERS; i++)
        {
            if (dev_sys->WinToLayerMap[i].Win == Win)
            {
                /* Check Layer value */
                if (dev_sys->WinToLayerMap[i].Layer < R_WM_MAX_LAYERS)
                {
                    ret = &(dev_sys->WinToLayerMap[i]);
                }
                else
                {
                    /* Do nothing. */
                }
                break;
            }
            else
            {
                /* Do nothing. */
            }
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_GetLayerPairByLayer
  
  Description: 
  Returns a WinLayerPair based on the given Layer.
  Returns NULL, if layer has no window.
  
  Parameters: 
  Unit - Specifies the WM unit number.
  Layer -
  
  Return Value: 
  Pointer to the the layer position of the window.
*/
r_wm_sys_WinLayerPair_t *R_WM_Sys_GetLayerPairByLayer(const uint32_t Unit, 
                                                      const uint32_t Layer)
{
    r_wm_sys_WinLayerPair_t       *ret = R_NULL;
    uint32_t                      i;
    r_wm_DevSys_t                 *dev_sys = R_WM_Sys_GetDevSys(Unit);

    for (i = 0u; i < R_WM_MAX_LAYERS; i++)
    {
        if (dev_sys->WinToLayerMap[i].Layer == Layer)
        {
            if (dev_sys->WinToLayerMap[i].Win != R_NULL)
            {
                ret = &(dev_sys->WinToLayerMap[i]);
            }
            else
            {
                /* Do nothing. */
            }
            break;
        }
        else
        {
            /* Do nothing. */
        }
    }
    return ret;
}

/***********************************************************
  Function: loc_WindowIsOnScreen
  
  Description:
  Return 1 if at least parts of the window are in visible screen area.
  Return 0, if window completely moved out of screen.
  
  Parameters:
  Unit - Specifies the WM unit number.
  Win  - Pointer to the window structure. See <r_wm_Window_t> for more details.
  
  Return value: 
  uint32_t  
*/
R_WM_STATIC uint32_t loc_WindowIsOnScreen(const uint32_t             Unit, 
                                          const r_wm_Window_t *const Win)
{
    uint32_t        ret;
    r_wm_DevSys_t   *dev_sys = R_WM_Sys_GetDevSys(Unit);
    
    /* PosY should be (-Height) < PosY < ScreenHeight */
    if (((-Win->PosY) >= (int32_t)Win->Height) || (Win->PosY >= dev_sys->ScreenHeight))
    {
        ret = R_WM_FALSE;
    }
    /* PosX should be (-Width) < PosX < ScreenWidth */
    else if (((-Win->PosX) >= (int32_t)Win->Width) || (Win->PosX >= dev_sys->ScreenWidth))
    {
        ret = R_WM_FALSE;
    }
    /* Check Width, Height */
    else if ((Win->Width == 0u) || (Win->Height == 0u))
    {
        ret = R_WM_FALSE;
    }
    else
    {
        ret = R_WM_TRUE;
    }
    return ret;
}

/***********************************************************
  Function: loc_CheckLayer0Assignment
  
  Description:
  Check the window feature for Layer0
  
  Textures using alpha channel that are placed on layer 0 will ignore alpha. 
  If alpha of RGBA should always be evaluated, workaround on API level is 
  to define dummy layer and place it on lowest z-Index but don't enable it.
  
  Parameters:
  Win   - Pointer to the window structure. See <r_wm_Window_t> for more details.
  
  Return value: 
  R_WM_TRUE  - Layer0 can accepts the window feature.
  R_WM_FALSE - Layer0 cannot accept the window feature.
*/
R_WM_STATIC uint32_t loc_CheckLayer0Assignment(const r_wm_Window_t *const Win)
{
    uint32_t    i;
    uint32_t    ret = R_WM_TRUE;

    /* Layer 0 can't do alpha blending by constant alpha */
    if (Win->Alpha != LOC_ALPHA_255)
    {
        ret = R_WM_FALSE;
    }
    else
    {
        /* Layer 0 can't do color keying */
        if (Win->ColorKey.Enabled == R_WM_TRUE)
        {
            ret = R_WM_FALSE;
        }
        else
        {
            /* Layer 0 can't do alpha blending by CLUT alpha */
            if ((Win->ColorFmt == R_WM_COLORFMT_CLUT8) || 
                (Win->ColorFmt == R_WM_COLORFMT_CLUT4) || 
                (Win->ColorFmt == R_WM_COLORFMT_CLUT1))
            {
                for (i = 0u; i < Win->ClutNumEntries; i++)
                {
                    if (Win->Clut[i].A != LOC_ALPHA_255)
                    {
                        /* Found (semi-)transparent pixel, this is not compatible with layer 0 */
                        ret = R_WM_FALSE;
                        break;
                    }
                    else
                    {
                        /* Do nothing. */
                    }
                }
            }
            else
            {
                /* Do nothing. */
            }
        }
    }
    return ret;
}

/***********************************************************
  Function: loc_CheckCaptureAssignment
  
  Description:
  Check the capture feature for specifed Layer.
  
  Parameters:
  Unit  - Specifies the WM unit number.
  Win   - Pointer to the window structure. See <r_wm_Window_t> for more details.
  Layer - Specifies the layer number. 
  
  Return value: 
  R_WM_TRUE  - Specified Layer can accepts the layer.
  R_WM_FALSE - Specified Layer can not accepts the layer.
*/
R_WM_STATIC uint32_t loc_CheckCaptureAssignment(const uint32_t             Unit, 
                                                const r_wm_Window_t *const Win,
                                                const uint32_t             Layer)
{
    r_wm_DevSys_t   *dev_sys;
    uint32_t        ret = R_WM_TRUE;
    uint32_t        capt_unit;
    
    for (capt_unit = R_WM_UNIT0; capt_unit < R_WM_DEV_NUM; capt_unit++)
    {
        dev_sys = R_WM_Sys_GetDevSys(capt_unit);
        if ((dev_sys->RootCapt != R_NULL) && (dev_sys->RootCapt->Window == Win))
        {
            if (capt_unit == Unit) 
            {
                if (Layer != R_VDCE_LAYER_SCALER0)
                {
                    ret = R_WM_FALSE;
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                if (Layer != R_VDCE_LAYER_SCALER1)
                {
                    ret = R_WM_FALSE;
                }
                else
                {
                    /* Do nothing */
                }
            }
            break;
        }
        else
        {
            /* Do nothing */
        }
    }
    return ret;
}

/***********************************************************
  Function: loc_SysWinCapabilitiesGet
  
  Description:
  Gets the attributes of the specified layer. Sprite or RLE.
  
  Parameters:
  Unit  - Specifies the WM unit number.
  Layer - Specifies the layer number. 
  
  Return value: 
  R_WM_TRUE  - Specified Layer can accepts the layer.
  R_WM_FALSE - Specified Layer can not accepts the layer.
*/
R_WM_STATIC r_wm_WinCapbs_t loc_SysWinCapabilitiesGet(const uint32_t Unit, 
                                                      const uint32_t Layer)
{
    r_wm_WinCapbs_t ret_val;
    
    if (Unit == R_WM_UNIT0)
    {
        /* Linear mapping for Unit 0 */
        ret_val = r_wm_DevCabps[Layer];
    }
    else
    {
        /* Different mapping for Unit 1 */
        switch (Layer)
        { 
            case R_VDCE_LAYER_SCALER0:
                ret_val = r_wm_DevCabps[R_VDCE_LAYER_SCALER0];
                break;
            case R_VDCE_LAYER_SCALER1:
                ret_val = r_wm_DevCabps[R_VDCE_LAYER_IMAGE3];
                break;
            case R_VDCE_LAYER_IMAGE2:
                ret_val = r_wm_DevCabps[R_VDCE_LAYER_IMAGE2];
                break;
            case R_VDCE_LAYER_IMAGE3:
                ret_val = r_wm_DevCabps[R_VDCE_LAYER_SCALER1];
                break;
            default:
                ret_val = r_wm_DevCabps[R_VDCE_LAYER_SCALER0];
                break;
        }
    }
    return ret_val;
}

/***********************************************************
  Function: loc_CheckHwLayerCapabilities
  
  Description:
  Check the window feature for specified Layer.
  
  Parameters:
  Unit  - Specifies the WM unit number.
  Layer - Specifies the layer number.
  Win   - Pointer to the window structure. See <r_wm_Window_t> for more details.
  
  Return value: 
  R_WM_TRUE  - Specified Layer can accepts the layer.
  R_WM_FALSE - Specified Layer can not accepts the layer.
   
*/
R_WM_STATIC uint32_t loc_CheckHwLayerCapabilities(const uint32_t             Unit, 
                                                  const uint32_t             Layer, 
                                                  const r_wm_Window_t *const Win)
{
    r_wm_WinCapbs_t     capability;
    uint32_t            win_is_rle;
    uint32_t            win_is_yuv;
    uint32_t            ret = R_WM_TRUE;
    
    /* Parameter checks */
    if ((Unit >= R_WM_DEV_NUM) || (Layer >=  R_WM_MAX_LAYERS) || (Win == R_NULL))
    {
        ret = R_WM_FALSE;
    }
    else
    {
        /* Check RLE window*/
        win_is_rle = loc_WindowIsRle(Win);
        if (win_is_rle == R_WM_TRUE)
        {
            capability = loc_SysWinCapabilitiesGet(Unit, Layer);
            if (capability != R_WM_WINCAPBS_RLE)
            {
                ret = R_WM_FALSE;
            }
            else
            {
                /* Do nothing. */
            }
        }
        else
        {
            /* Do nothing. */
        }
        if (ret == R_WM_TRUE)
        {
            /* Check YCbCr color format */
            win_is_yuv = loc_WindowIsYuvLayer(Win);
            if ((win_is_yuv == R_WM_TRUE) && (Layer > R_VDCE_LAYER_SCALER1))
            {
                ret = R_WM_FALSE;
            }
            else
            {
                /* Check Sprite window */
                if (Win->Mode == R_WM_WINMODE_SPRITES)
                {
                    capability = loc_SysWinCapabilitiesGet(Unit, Layer);
                    if (capability != R_WM_WINCAPBS_SPRITES)
                    {
                        ret = R_WM_FALSE;
                    }
                    else
                    {
                        /* Do nothing. */
                    }
                }
                else
                {
                    /* Do nothing. */
                }
                if (ret == R_WM_TRUE)
                {
                    /* Check window layer0 feature */
                    if (Layer == R_VDCE_LAYER_SCALER0)
                    {
                        ret = loc_CheckLayer0Assignment(Win);
                    }
                    else
                    {
                        /* Do nothing. */
                    }
                }
                else
                {
                    /* Do nothing. */
                }
                
                if (ret == R_WM_TRUE)
                {
                    /* Check capture assignment*/
                    ret = loc_CheckCaptureAssignment(Unit, Win, Layer);
                }
                else
                {
                    /* Do nothing. */
                }
            }
        }
        else
        {
            /* Do nothing. */
        }
    }

    return ret;
}

/***********************************************************
  Function: R_WM_Sys_UpdateAllCluts
  
  Description:
  Updates, enables or disables the color look up tables accordingly.
  
  Parameters:
  Unit - Specifies the WM unit number.
  
  Return value: 
  uint32_t 
*/

uint32_t R_WM_Sys_UpdateAllCluts(const uint32_t Unit)
{
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    uint32_t                        layer;
    uint32_t                        ret      = R_WM_SYS_OK;
    r_vdce_Error_t                  vdce_ret = R_VDCE_ERR_OK;
    r_wm_DevSys_t                   *dev_sys;
    const r_vdce_ClutARGB32_t       *clut;

    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        dev_sys = R_WM_Sys_GetDevSys(Unit);
        
        /* Check if a CLUT update is pending for any layer */
        if (((dev_sys->FrameStatusFlags) & R_WM_SYS_FRAME_MASK_CLUT_UPDATE) != 0uL)
        {
            for (layer = 0u; layer <  R_WM_MAX_LAYERS; layer++)
            {
                /* Read respective R_WM_SYS_FRAME_BIT_LAYER0_CLUT */
                if ((dev_sys->FrameStatusFlags & 
                    (R_WM_SYS_FLAG_ON << (R_WM_SYS_FRAME_BIT_LAYER0_CLUT + layer))) != 0u)
                {
                    win_layer_pair = R_WM_Sys_GetLayerPairByLayer(Unit, layer);
                    if ((win_layer_pair != R_NULL) && (win_layer_pair->Win->ClutNumEntries > 0u))
                    {
                        /* Convert VDCE strucure (Both structures have same member) */
                        clut =  (const r_vdce_ClutARGB32_t*)(win_layer_pair->Win->Clut); /* PRQA S 310 */
                        
                        /* Update CLUT */
                        vdce_ret = R_VDCE_LayerClutSet(
                                    Unit,
                                    layer, 
                                    LOC_CLUT_INDEX_0, 
                                    win_layer_pair->Win->ClutNumEntries,
                                    clut
                                    );
                    }
                    else
                    {
                        /* Window is invalid */
                        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
                        ret = R_WM_SYS_NG;
                    }
                }
                else
                {
                    /* Do nothing. */
                }
                /* Check loop break codition */
                if ((vdce_ret != R_VDCE_ERR_OK) || (ret == R_WM_SYS_NG))
                {
                    ret = R_WM_SYS_NG;
                    break;
                }
                else
                {
                    /* Do nothing. */
                }
            }
        }
        else
        {
            /* Do nothing. */
        }
    }
    return ret;
}

/***********************************************************
  Function: loc_FindMatchLayer
  
  Description:
  This function finds the first layer to match requested HW features
  
  Parameters:
  Unit   - Specifies the WM unit number.
  Layer  - specifies the layer number.
  CurWin - Pointer to the window structure. See <r_wm_Window_t> for more details.
  
  Return value: 
  Layer number matched'
*/
R_WM_STATIC uint32_t loc_FindMatchLayer(const uint32_t             Unit, 
                                        const uint32_t             Layer, 
                                        const r_wm_Window_t *const CurWin)
{
    uint32_t                ret_layer;
    uint32_t                capability;
    uint32_t                ret = R_WM_SYS_OK;
    
    /* Search from specified layer */
    for (ret_layer = Layer; ret_layer < R_WM_MAX_LAYERS; ret_layer++)
    {
        /* 
           If requested HW features cannot be mapped, abort! 
           Feature oriented layer mapping is user scope. 
        */
        capability = loc_CheckHwLayerCapabilities(Unit, ret_layer, CurWin);
        
        /* Delete the window, Regardless of capability */
        ret = loc_WindowDelete(Unit, ret_layer);
        if ((ret != R_WM_SYS_OK) || (capability == R_WM_TRUE))
        {
            break;
        }
        else
        {
            /* Do nothing */
        }
    }
    
    if (ret != R_WM_SYS_OK)
    {
        /* return error */
        ret_layer = R_WM_MAX_LAYERS;
    }
    else
    {
        /* Do nothing */
    }
    return ret_layer;
}

/***********************************************************
  Function: loc_SetAlphaBlending
  
  Description:
  This function validates the chroma key and alpha blending
  
  Parameters:
  Unit   - Specifies the WM unit number.
  Layer  - specifies the layer number.
  CurWin - Pointer to the window structure. See <r_wm_Window_t> for more details.
  
  Return value: 
  0 - Error occurred.
  1 - No Error.
*/
R_WM_STATIC uint32_t loc_SetAlphaBlending(const uint32_t             Unit, 
                                          const uint32_t             Layer, 
                                          const r_wm_Window_t *const CurWin)
{
    uint32_t          ret;
    r_vdce_CkARGB32_t ck_in;
    r_vdce_CkARGB32_t ck_out;
    r_vdce_Error_t    vdce_res;
    
    if (Layer == R_VDCE_LAYER_SCALER0)
    {
        /* Layer0 dose not support alpha blending */
        ret = R_WM_SYS_OK;
    }
    else
    {
        /* Note: Layer Alpha and Chroma Keying are exclusive. */
        
        if (CurWin->ColorKey.Enabled == R_WM_TRUE)
        {
            /* Disable const alpha  */
            vdce_res = R_VDCE_LayerAlphaConstDisable(Unit, Layer);
            if (vdce_res != R_VDCE_ERR_OK)
            {
                /* Do nothing */
            }
            else
            {
                /* Enable Chromakey */
                ck_in.R = CurWin->ColorKey.In.RgbKey.Red;
                ck_in.G = CurWin->ColorKey.In.RgbKey.Green;
                ck_in.B = CurWin->ColorKey.In.RgbKey.Blue;
                ck_in.A = LOC_ALPHA_0;
                
                ck_out.R = CurWin->ColorKey.Out.Red;
                ck_out.G = CurWin->ColorKey.Out.Green;
                ck_out.B = CurWin->ColorKey.Out.Blue;
                ck_out.A = CurWin->ColorKey.Out.Alpha;
                
                vdce_res = R_VDCE_LayerChromaKeyEnable(Unit, Layer, &ck_in, &ck_out);
            }
        }
        else
        {
            /* Disable Chromakey */
            vdce_res = R_VDCE_LayerChromaKeyDisable(Unit, Layer);
            if (vdce_res != R_VDCE_ERR_OK)
            {
                /* Do nothing */
            }
            else
            {
                /* Enable const alpha  */
                vdce_res = R_VDCE_LayerAlphaConstEnable(Unit, Layer, CurWin->Alpha);
            }
        }
        /* Check VDCE result */
        if (vdce_res != R_VDCE_ERR_OK)
        {
            ret = R_WM_SYS_NG;
        }
        else
        {
            /* Enable alpha blending */
            vdce_res = R_VDCE_LayerAlphaChannelEnable(Unit, Layer);
            if (vdce_res != R_VDCE_ERR_OK)
            {
                ret = R_WM_SYS_NG;
            }
            else 
            {
                if (CurWin->UsePremultipliedAlpha == R_WM_TRUE)
                {
                    /* Enable Pre-multied alpha blending */
                    vdce_res = R_VDCE_LayerPremultipliedAlphaEnable(Unit, Layer);
                }
                else
                {
                    /* Disable Pre-multied alpha blending */
                    vdce_res = R_VDCE_LayerPremultipliedAlphaDisable(Unit, Layer);
                }
                /* Check VDCE result */
                if (vdce_res != R_VDCE_ERR_OK)
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
  Function: loc_SetLayerScaling
  
  Description:
  This function sets image scaling in width and Height
  
  Parameters:
  Unit         - Specifies the WM unit number.
  Layer        - specifies the layer number.
  CurWin       - Pointer to the window structure. See <r_wm_Window_t> for more details.
  ChangeMode   - Scaled size change behaviour. See <r_wm_ScaleChg_t>.
  ScaledWidth  - New horizontal size. 
                 It is scaling-down size when ChangeMode = R_WM_SCALE_CHANGE_SMALL.
                 It is scaling-up   size when ChangeMode = R_WM_SCALE_CHANGE_LARGE.
                 It is window       size when ChangeMode = R_WM_SCALE_CHANGE_WIN_SIZE.
  ScaledHeight - New vertical size.
                 It is scaling-down size when ChangeMode = R_WM_SCALE_CHANGE_SMALL.
                 It is scaling-up   size when ChangeMode = R_WM_SCALE_CHANGE_LARGE.
                 It is window       size when ChangeMode = R_WM_SCALE_CHANGE_WIN_SIZE.
  
  Return value: 
  0 - Error occurred.
  1 - No Error.
*/

R_WM_STATIC uint32_t loc_SetLayerScaling(const uint32_t             Unit,
                                         const uint32_t             Layer,
                                         const r_wm_Window_t *const CurWin,
                                         const r_wm_ScaleChg_t      ChangeMode,
                                         const uint32_t             ScaledWidth,
                                         const uint32_t             ScaledHeight)
{
    uint32_t        ret;
    r_wm_DevSys_t   *dev_sys;
    uint32_t        scaling_res;
    r_vdce_Scale_t  scaling_x;
    r_vdce_Scale_t  scaling_y;
    uint32_t        scale_width;
    uint32_t        scale_height;
    uint32_t        new_width;
    uint32_t        new_height;
    uint32_t        capt_unit;
    uint32_t        win_width;
    uint32_t        win_height;
    
    /* update the scaling */
    /* Layer 0 & 1 only when scaling is enabled */
    if  (Layer > R_VDCE_LAYER_SCALER1)
    {
        ret = R_WM_SYS_OK;
    }
    else
    {
        /* Set disable value as default */
        scaling_x    = R_VDCE_SCALING_DISABLE;
        scaling_y    = R_VDCE_SCALING_DISABLE;
        scale_width  = LOC_SCALING_MIN_VAL;
        scale_height = LOC_SCALING_MIN_VAL;
        
        /* Check capture scaling change (scall-down) */
        if (ChangeMode == R_WM_SCALE_CHANGE_SMALL)
        {
            new_width  = ScaledWidth;
            new_height = ScaledHeight;
        }
        else 
        {
            /* Check capture device */
            if (Unit == R_WM_UNIT0)
            {
                capt_unit = (Layer == R_VDCE_LAYER_SCALER0) ? R_WM_UNIT0 : R_WM_UNIT1;
            }
            else
            {
                capt_unit = (Layer == R_VDCE_LAYER_SCALER0) ? R_WM_UNIT1 : R_WM_UNIT0;
            }
#if (R_WM_DEV_NUM == 1)
            if (capt_unit == R_WM_UNIT1)
            {
                new_width  = 0;
                new_height = 0;
            }
            else
#endif
            {
                dev_sys = R_WM_Sys_GetDevSys(capt_unit);
                if ((dev_sys->RootCapt != R_NULL) && (dev_sys->RootCapt->Window == CurWin))
                {
                    new_width  = dev_sys->RootCapt->ScaledWidth;
                    new_height = dev_sys->RootCapt->ScaledHeight;
                }
                else
                {
                    new_width  = 0;
                    new_height = 0;
                }
            }
        }
        if (new_width != 0u)
        {
            scaling_x   = R_VDCE_SCALING_SMALLER;
            scale_width = new_width; 
        }
        else
        {
            /* Do nothing */ 
        }
        
        if (new_height != 0u)
        {
            scaling_y    = R_VDCE_SCALING_SMALLER;
            scale_height = new_height; 
        }
        else
        {
            /* Do nothing */ 
        }
        
        /* Check window scaling change (scall-up) */
        if (ChangeMode == R_WM_SCALE_CHANGE_LARGE)
        {
            new_width  = ScaledWidth;
            new_height = ScaledHeight;
        }
        else 
        {
            new_width  = CurWin->ScaledWidth;
            new_height = CurWin->ScaledHeight;
        }

        if (ChangeMode == R_WM_SCALE_CHANGE_WIN_SIZE)
        {
            win_width  = ScaledWidth;
            win_height = ScaledHeight;
        }
        else 
        {
            win_width  = CurWin->Width;
            win_height = CurWin->Height;
        }
        
        if (scaling_x == R_VDCE_SCALING_DISABLE)
        {
            if ((new_width != 0u) && (new_width < win_width))
            {
                scaling_x   = R_VDCE_SCALING_LARGER;
                scale_width = new_width; 
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
        if (scaling_y == R_VDCE_SCALING_DISABLE)
        {
            if ((new_height != 0u) && (new_height < win_height))
            {
                scaling_y    = R_VDCE_SCALING_LARGER;
                scale_height = new_height; 
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
        
        /* Set Scaling X */
        scaling_res = R_VDCE_LayerImgScaleX(Unit, Layer, scale_width, scaling_x);
        if (scaling_res == LOC_SCALING_ERR_VAL)
        {
            ret = R_WM_SYS_NG;
        }
        else
        {
            /* Set Scaling Y */
            scaling_res = R_VDCE_LayerImgScaleY(Unit, Layer, scale_height, scaling_y);
            if (scaling_res == LOC_SCALING_ERR_VAL)
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
  Function: loc_SetLayerFormat
  
  Description:
  This function sets the color format and layer option.
  Summarized common processing for all layers.
  
  Parameters:
  Unit   - Specifies the WM unit number.
  Layer  - specifies the layer number.
  CurWin - Pointer to the window structure. See <r_wm_Window_t> for more details.
  
  Return value: 
  0 - Error occurred.
  1 - No Error.
*/
R_WM_STATIC uint32_t loc_SetLayerFormat(const uint32_t             Unit, 
                                        const uint32_t             Layer, 
                                        const r_wm_Window_t *const CurWin)
{
    uint32_t                ret;
    r_vdce_Error_t          vdce_res;
    r_vdce_Format_t         color_fmt;
    r_vdce_LayerMode_t      mode;
    r_wm_DevSys_t           *dev_sys;
    
    /* Set color format */
    color_fmt = loc_GetVdceFormat(CurWin->ColorFmt);
    vdce_res  = R_VDCE_LayerFormatSet(Unit, Layer, color_fmt);
    if (vdce_res != R_VDCE_ERR_OK)
    {
        ret = R_WM_SYS_NG;
    }
    else
    {
        /* Set CLUT update flag */
        if (CurWin->ClutNumEntries > 0u)
        {
            dev_sys = R_WM_Sys_GetDevSys(Unit);
            dev_sys->FrameStatusFlags |= 
             (R_WM_SYS_FLAG_ON << (R_WM_SYS_FRAME_BIT_LAYER0_CLUT + Layer));
        }
        else
        {
            /* Do nothing */
        }
        
        /* Check vertical mirroring */
        if ((((uint32_t)CurWin->Flags) & ((uint32_t)R_WM_WINFLAG_V_MIRROR)) != 0u)
        {
            mode = R_VDCE_LAYER_MODE_V_MIRORING;
        }
        else
        {
            mode = R_VDCE_LAYER_MODE_NONE;
        }
        
        /* Set layer mode */
        vdce_res = R_VDCE_LayerModeSet(Unit, Layer, mode);
        if (vdce_res != R_VDCE_ERR_OK)
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
  Function: loc_SetLayerBaseAdrs
  
  Description:
  This function sets layer base address.
  
  Parameters:
  Unit   - Specifies the WM unit number.
  Layer  - specifies the layer number.
  CurWin - Pointer to the window structure. See <r_wm_Window_t> for more details.
  
  Return value: 
  0 - Error occurred.
  1 - No Error.
*/
R_WM_STATIC uint32_t loc_SetLayerBaseAdrs(const uint32_t             Unit, 
                                          const uint32_t             Layer, 
                                          const r_wm_Window_t *const CurWin)
{
    const void      *fb;
    uint32_t        ret = R_WM_SYS_OK;
    r_spea_Unit_t   spea_unit;

    /* set the layer base address from the ON_SCREEN fb */
    if (CurWin->Mode != R_WM_WINMODE_SPRITES)
    {
        fb = R_WM_WindowVisibleBufGet(Unit, CurWin);
        if (fb != R_NULL)
        {
            ret = loc_WinSetFb(Unit, CurWin, CurWin->PosX, CurWin->PosY, fb);
        }
    }
    else
    {
        ret = loc_WinSetFb(Unit, CurWin, CurWin->PosX, CurWin->PosY, R_NULL);
        if (ret == R_WM_SYS_NG)
        {
            /* Do nothing */
        }
        else
        {
            if (CurWin->Surface.SpritesRoot != R_NULL)
            {
                spea_unit = R_WM_Sys_GetSpriteUnitFrmLyrNo(Unit, Layer);
                R_WM_Sys_SetSpriteReassignFlag(Unit, spea_unit);
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
  Function: loc_SetupVdceLayer
  
  Description:
  This function setups vdce layer.
  
  Parameters:
  Unit   - Specifies the WM unit number.
  Layer  - specifies the layer number.
  CurWin - Pointer to the window structure. See <r_wm_Window_t> for more details.
  
  Return value: 
  0 - Error occurred.
  1 - No Error.
*/
R_WM_STATIC uint32_t loc_SetupVdceLayer(const uint32_t             Unit, 
                                        const uint32_t             Layer, 
                                        const r_wm_Window_t *const CurWin)
{
    uint32_t  ret;
    
    /* Lock */
    loc_LockLayerUpdate(Unit, Layer, R_WM_TRUE);
    
    /* Set alpha blending feature */
    ret = loc_SetAlphaBlending(Unit, Layer, CurWin); 
    if (ret == R_WM_SYS_OK)
    {
        /* Set color format */
        ret = loc_SetLayerFormat(Unit, Layer, CurWin);
        if (ret == R_WM_SYS_OK)
        {
            /* Set scaling */
            ret = loc_SetLayerScaling(Unit, Layer, CurWin, R_WM_SCALE_CHANGE_NONE, 0, 0);
            if (ret == R_WM_SYS_OK)
            {
                /* Set base addr */
                ret = loc_SetLayerBaseAdrs(Unit, Layer, CurWin);
                if (ret == R_WM_SYS_OK)
                {
                    /* Set view port.  This will also enable the Layer */
                    ret = loc_WinSetViewportAndGeom(
                              Unit,
                              CurWin,
                              Layer,
                              CurWin->PosX,
                              CurWin->PosY,
                              CurWin->Pitch,
                              CurWin->Width,
                              CurWin->Height);
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
    }
    else
    {
        /* Do nothing */
    }

    /* Unlock */
    loc_LockLayerUpdate(Unit, Layer, R_WM_FALSE);

    return ret; 
}

/***********************************************************
  Function: R_WM_Sys_ReconfAllWindow
  
  Description:
  
  Parameters:
  Unit - Specifies the WM unit number.
  
  Return value: 
  uint32_t 
*/
uint32_t R_WM_Sys_ReconfAllWindow(const uint32_t Unit)
{
    r_wm_Window_t           *cur_win;
    uint32_t                slot  = 0u;
    uint32_t                layer = 0u;
    uint32_t                ret   = R_WM_SYS_OK;
    uint32_t                vdce_ret;
    r_wm_DevSys_t           *dev_sys;

    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        dev_sys = R_WM_Sys_GetDevSys(Unit);
        dev_sys->FrameStatusFlags = 0u;
        cur_win = dev_sys->RootWin;

        /* Iteration of NULL-terminated Linked-List. Maximum Number of expected 
           elements for this device family is R_WM_MAX_LAYERS.
        */
        while ((cur_win != R_NULL) && (layer < R_WM_MAX_LAYERS))
        {
            /* Find first layer to match requested HW features */
            layer = loc_FindMatchLayer(Unit, layer, cur_win);
            if (layer >= R_WM_MAX_LAYERS)
            {
                /* Do nothing */
            }
            else
            {
                /* Set Layer Map */
                dev_sys->WinToLayerMap[slot].Win = cur_win; 
                dev_sys->WinToLayerMap[slot].Layer = layer;
                
                /* Set VDCE layer */
                vdce_ret = loc_SetupVdceLayer(Unit, layer, cur_win);
                if (vdce_ret == R_WM_SYS_NG)
                {
                    ret = vdce_ret;
                    dev_sys->WinToLayerMap[slot].Layer = LOC_INVALID_LAYER;
                }
                else
                {
                    /* Do nothing */;
                }
                /* Next window */
                cur_win = cur_win->Next;
                layer++;
                slot++;
            }
        }
        /* Check if unassigned window is remained. */
        if (cur_win != R_NULL)
        {
            /* Turn off remaining layers */
            for (; slot < R_WM_MAX_LAYERS; slot++)
            {
                dev_sys->WinToLayerMap[slot].Win   = cur_win; 
                dev_sys->WinToLayerMap[slot].Layer = LOC_INVALID_LAYER;
                
                if (cur_win != R_NULL)
                {
                    cur_win = cur_win->Next;
                }
                else
                {
                    /* Do nothing */
                }
            }
            
            /* Return error */
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
            ret = R_WM_SYS_NG;
        }
        else
        {
            for (; layer < R_WM_MAX_LAYERS; layer++)
            {
                vdce_ret = loc_WindowDelete(Unit, layer);
                if (vdce_ret == R_WM_SYS_NG)
                {
                    ret = vdce_ret;
                }
                else
                {
                    /* Do nothing */
                }
            }
            /* Turn off remaining layers */
            for (; slot <  R_WM_MAX_LAYERS; slot++)
            {
                dev_sys->WinToLayerMap[slot].Win   = R_NULL; 
                dev_sys->WinToLayerMap[slot].Layer = LOC_INVALID_LAYER;
            }
        }
    }

    return ret;
}

/***********************************************************
  Function: loc_WinSetViewportAndGeom
  
  Description:
  Sets the view port and the physical window geometry according to the bpp, width, height, 
  X and Y positions and the status of the window.
  
  Parameters:
  Unit   - Specifies the WM unit number.
  Win    - Pointer to the window structure. See <r_wm_Window_t> for more details.
  Layer  - Specifies the layer number.
  PosX   - Specifies the X position of the window in pixels.
  PosY   - Specifies the Y position of the window in pixels.
  Pitch  - Specifies the line pitch of a frame buffer.
  Width  - Specifies the width of the window in pixels.
  Height - Specifies the height of the window in pixels.
  Bpp    - Specifies the bits per pixels.
  
  Return value: 
  uint32_t 
*/
R_WM_STATIC uint32_t loc_WinSetViewportAndGeom(const uint32_t             Unit,
                                               const r_wm_Window_t *const Win,
                                               const uint32_t             Layer,
                                               const int32_t              PosX,
                                               const int32_t              PosY,
                                               const uint32_t             Pitch,
                                               const uint32_t             Width,
                                               const uint32_t             Height)
{
    r_vdce_Error_t      vdce_res;
    uint32_t            win_is_on;
    uint32_t            ret = R_WM_SYS_OK;
    uint32_t            disabled;
    
    /* Check Width & Height */
    if ((Width > 0u) && (Height > 0u))
    {
        vdce_res = R_VDCE_LayerMemGeometrySet(Unit, Layer, Pitch, (uint32_t)Height);
        if (vdce_res != R_VDCE_ERR_OK)
        {
            ret = R_WM_SYS_NG;
        }
        else
        {
            vdce_res = R_VDCE_LayerViewPortSet(
                            Unit, 
                            Layer, 
                            LOC_MEM_POS_X, 
                            LOC_MEM_POS_Y, 
                            PosX, 
                            PosY, 
                            (int32_t)Width, 
                            (int32_t)Height);
            if (vdce_res != R_VDCE_ERR_OK)
            {
                ret = R_WM_SYS_NG;
            }
            else
            {
                /* Do nothing. */
            }
        }
    }
    else
    {
        /* Do nothing. */
    }
    if (ret == R_WM_SYS_OK)
    {
        if (Win->Status == R_WM_WINSTATUS_ENABLED)
        {
            /* do not enable the layer outside of the screen limitations */
            win_is_on = loc_WindowIsOnScreen(Unit, Win);
            if (win_is_on == R_WM_TRUE)
            {
                ret = loc_VdceLayerEnable(Unit, Layer);
            }
            else
            {
                ret = loc_VdceLayerDisable(Unit, Layer, &disabled);
            }
        }
        else
        {
            /* Do nothing. */
        }
    }
    else
    {
        /* Do nothing. */
    }
    return ret;
}

/***********************************************************
  Function: loc_WinSetFb
  
  Description:
  Enables the frame buffer by setting its virtual address and adjusting its screen position.
  
  Parameters:
  Unit - Specifies the WM unit number.
  Win - Pointer to the window structure. See <r_wm_Window_t> for more details.
  PosX - Specifies the X position of the window in pixels.
  PosY - Specifies the Y position of the window in pixels.
  Fb - Specifies the address of the frame buffer.
  
  Return value: 
  uint32_t 
*/
R_WM_STATIC uint32_t loc_WinSetFb(const uint32_t             Unit,
                                  const r_wm_Window_t *const Win,
                                  const int32_t              PosX,
                                  const int32_t              PosY,
                                  const void *const          Fb)
{
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    uint32_t                        base_adr;
    r_vdce_Error_t                  vdce_res;
    uint32_t                        win_is_rle;
    uint32_t                        ret = R_WM_SYS_OK;
    r_wm_WinCapbs_t                 win_caps_get;
    
    win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Win);
    if (win_layer_pair == R_NULL)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        /* Convert types to adjust VDCE I/F  */
        base_adr = (uint32_t)Fb; /* PRQA S 306 */
        
        /* Check RLE */
        win_is_rle = loc_WindowIsRle(Win);
        if (win_is_rle == R_WM_TRUE)
        {
            /* Check device capability */
            win_caps_get = loc_SysWinCapabilitiesGet(Unit, win_layer_pair->Layer);
            if (win_caps_get != R_WM_WINCAPBS_RLE)
            {
                R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
                ret = R_WM_SYS_NG;
            }
            else
            {
                /* RLE Buffers must be 128-byte aligned */
                if ((base_adr & LOC_BUF_ALIGN_CHECK) != 0u)
                {
                    R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_ADDRESS_ALIGNMENT, R_WM_SYS_NO_INFO);
                    ret = R_WM_SYS_NG;
                }
                else
                {
                    /* Enable RLE */
                    ret = R_WM_Sys_RleEnable(Unit, Win, win_layer_pair->Layer, R_WM_TRUE);
                    
                    /* set the address of the virtual layer */
                    base_adr = R_WM_SYS_VIRTUAL_ADDR_RLE;
                }
            }
        }
        /* Check Sprite */
        else if (Win->Mode == R_WM_WINMODE_SPRITES)
        {
            /* set the address of the virtual layer */
            if (Unit == R_WM_UNIT0)
            {
                base_adr = R_WM_SYS_VIRTUAL_ADDR_SPRITE_0;
            }
            else
            {
                base_adr = R_WM_SYS_VIRTUAL_ADDR_SPRITE_1;
            }
        }
        /* Normal Frame buffer */
        else
        {
            /* Do nothing. */
        }
        
        /* check if the negative start position workaround is needed */
        if (ret == R_WM_SYS_OK)
        {
            vdce_res = R_VDCE_LayerBaseSet(Unit, win_layer_pair->Layer, base_adr);
            if (vdce_res != R_VDCE_ERR_OK)
            {
                ret = R_WM_SYS_NG;
            }
            else
            {
                /* Do nothing. */
            }
        }
        else
        {
            /* Do nothing. */
        }
    }

    return ret;
}

/***********************************************************
  Function: loc_VdceLayerEnable
  
  Description:
  Enables the HW layer.
  
  Parameters:
  Unit  - Specifies the WM unit number.
  Layer - Specifies the layer number.
  
  Return value:
  <r_vdce_Error_t>
*/
R_WM_STATIC uint32_t loc_VdceLayerEnable(const uint32_t             Unit, 
                                         const uint32_t             Layer)
{
    r_vdce_Error_t  vdce_err;
    uint32_t        ret;
    r_wm_DevSys_t   *dev_sys = R_WM_Sys_GetDevSys(Unit);
    
    if ((dev_sys->HwLayerEnabled[Layer] & R_WM_SYS_LAYER_FLAG_EN) == 0u)
    {
        vdce_err = R_VDCE_LayerEnable(Unit, Layer);
        if (vdce_err  == R_VDCE_ERR_OK)
        {
            dev_sys->HwLayerEnabled[Layer] |= R_WM_SYS_LAYER_FLAG_EN;
            ret = R_WM_SYS_OK;
        }
        else
        {
            ret = R_WM_SYS_NG;
        }
    }
    else
    {
        /* Already enabled */
        ret = R_WM_SYS_OK;
    }
    return ret;
}

/***********************************************************
  Function: loc_VdceLayerDisable
  
  Description:
  Disables the HW layer.
  
  Parameters:
  Unit     - Specifies the WM unit number.
  Layer    - Specifies the layer number.
  Disabled - R_WM_TRUE: Disabled in the current call. R_WM_FALSE: Not
  
  Return value:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail
*/
R_WM_STATIC uint32_t loc_VdceLayerDisable(const uint32_t  Unit, 
                                          const uint32_t  Layer,
                                          uint32_t *const Disabled)
{
    r_vdce_Error_t  vdce_err;
    uint32_t        ret;
    r_wm_DevSys_t   *dev_sys = R_WM_Sys_GetDevSys(Unit);
    
    if ((dev_sys->HwLayerEnabled[Layer] & R_WM_SYS_LAYER_FLAG_EN) != 0u)
    {
        vdce_err = R_VDCE_LayerDisable(Unit, Layer);
        if (vdce_err == R_VDCE_ERR_OK)
        {
            *Disabled = R_WM_TRUE;
            dev_sys->HwLayerEnabled[Layer] &= (uint8_t)~R_WM_SYS_LAYER_FLAG_EN;
            ret = R_WM_SYS_OK;
        }
        else
        {
            /* Error */
            *Disabled = R_WM_FALSE;
            ret = R_WM_SYS_NG;
       }
    }
    else
    {
        /* Already disabled */
        *Disabled = R_WM_FALSE;
        ret = R_WM_SYS_OK;
    }
    return ret;
}

/***********************************************************
  Function: loc_WindowDelete
  
  Description:
  Disables the HW layer.
  
  Parameters:
  Unit     - Specifies the WM unit number.
  Win      - Pointer to the window structure. See <r_wm_Window_t> for more details.
  
  Return value:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail
*/
R_WM_STATIC uint32_t loc_WindowDelete(const uint32_t             Unit, 
                                      const uint32_t             Layer)
{
    uint32_t                     ret = R_WM_SYS_OK;
    uint32_t                     disabled;
    uint32_t                     win_is_rle;
    r_wm_sys_WinLayerPair_t      *win_layer_pair;
    r_spea_Unit_t                spea_unit;
    
    /* Get Window */
    win_layer_pair = R_WM_Sys_GetLayerPairByLayer(Unit, Layer);
    if (win_layer_pair == R_NULL)
    {
        /* Do nothing */
    }
    else
    {
        /* if the layer is enabled, disable the layer */
        if (win_layer_pair->Win->Status == R_WM_WINSTATUS_ENABLED)
        {
            ret = loc_VdceLayerDisable(Unit, Layer, &disabled);
        }
        else
        {
            /* Do nothing. */
        }
        /* Check RLE window */
        win_is_rle = loc_WindowIsRle(win_layer_pair->Win);
        if ((ret == R_WM_SYS_OK) && (win_is_rle == R_WM_TRUE))
        {
            /* Disable RLE */
            ret = R_WM_Sys_RleEnable(Unit, win_layer_pair->Win, Layer, R_WM_FALSE);
        }
        else
        {
            /* Do nothing */
        }
        
        /* Check Sprite window */
        if (ret == R_WM_SYS_OK)
        {
            if (win_layer_pair->Win->Mode == R_WM_WINMODE_SPRITES)
            {
                spea_unit = R_WM_Sys_GetSpriteUnitFrmLyrNo(Unit, Layer);
                R_WM_Sys_SetSpriteReassignFlag(Unit, spea_unit);
            }
            else
            {
                /* Do nothing */
            }
            
            /* Delete Window information */
            win_layer_pair->Win   = R_NULL;
            win_layer_pair->Layer = LOC_INVALID_LAYER;
        }
        else
        {
            /* Do nothing */
        }
    }

    return ret;
}


/***********************************************************
  Function: R_WM_Sys_InitCapabilities
*/
void R_WM_Sys_InitCapabilities(void)
{
    r_wm_DevCabps[R_VDCE_LAYER_SCALER0] = R_WM_WINCAPBS_RLE;
    r_wm_DevCabps[R_VDCE_LAYER_SCALER1] = R_WM_WINCAPBS_SPRITES;
    r_wm_DevCabps[R_VDCE_LAYER_IMAGE2]  = R_WM_WINCAPBS_SPRITES;
    r_wm_DevCabps[R_VDCE_LAYER_IMAGE3]  = R_WM_WINCAPBS_SPRITES;
}

/***********************************************************
  Function: loc_LockLayerUpdate
  
  Description:
  Lock the window base update.
  
  Parameters:
  Unit     - Specifies the WM unit number.
  Layer    - Specifies the layer number.
  Lock     - Lock (TRUE) or Unlock (FALSE)
  
*/
R_WM_STATIC void loc_LockLayerUpdate(const uint32_t             Unit, 
                                const uint32_t             Layer,
                                const uint8_t              Lock)
{
    r_wm_DevSys_t   *dev_sys;
    
    dev_sys = R_WM_Sys_GetDevSys(Unit);
    if (Lock == R_WM_TRUE)
    {
        dev_sys->HwLayerEnabled[Layer] |= R_WM_SYS_LAYER_FLAG_LOCK;
    }
    else
    {
        dev_sys->HwLayerEnabled[Layer] &= (uint8_t)~R_WM_SYS_LAYER_FLAG_LOCK;
    }
}

/***********************************************************
  Function: R_WM_Sys_UpdatePendingBuffer
*/
void R_WM_Sys_UpdatePendingBuffer(const uint32_t             Unit)
{
    r_wm_DevSys_t                *dev_sys;
    uint32_t                     layer;
    r_wm_WinBuffer_t             *buf;
    r_wm_sys_WinLayerPair_t      *win_layer_pair;

    dev_sys = R_WM_Sys_GetDevSys(Unit);
    
    for (layer = 0; layer < LOC_CAP_LAYER_NUM; layer++)
    {
        if (dev_sys->CaptPending[layer] == R_WM_TRUE)
        {
            /* Clear */
            dev_sys->CaptPending[layer] = R_WM_FALSE;
            
            /* Get window */
            win_layer_pair = R_WM_Sys_GetLayerPairByLayer(Unit, layer);
            if (win_layer_pair != R_NULL)
            {
                /* Get freshly captured buffer */
                buf = R_WM_Cap_DispBufGet(Unit, win_layer_pair->Win);
                
                if (buf != R_NULL)
                {
                    /* And display it with the next frame */
                    (void)R_WM_Sys_WindowSetFb(
                                    Unit, win_layer_pair->Win, buf->Data);
                    /* Fall through if error occurs. */
                }
                else
                {
                    /* Do Nothing */
                }
            }
        }
    }
}

/***********************************************************
  Section: WM Driver Support Functions
  
  Comments see: <WM Support Functions>
*/

/***********************************************************
  Group: WM Basic interface functions
*/

/***********************************************************
  Function: R_WM_Sys_DevRootWindowSet
*/
void R_WM_Sys_DevRootWindowSet(const uint32_t       Unit, 
                               r_wm_Window_t *const RootWin)
{
    r_wm_DevSys_t   *dev_sys;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
    }
    else
    {
        dev_sys = R_WM_Sys_GetDevSys(Unit);
        dev_sys->RootWin = RootWin;
    }
}

/***********************************************************
  Group: WM Video Output Window interface
*/

/***********************************************************
  Function: R_WM_Sys_WindowSetFb
*/
uint32_t R_WM_Sys_WindowSetFb(const uint32_t             Unit, 
                              const r_wm_Window_t *const Win, 
                              const void *const          Fb)
{
    uint32_t                        ret;
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    
    if ((Unit >= R_WM_DEV_NUM) || (Win == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Win);
        if (win_layer_pair == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
            ret = R_WM_SYS_NG;
        }
        else
        {
            /* Lock */
            loc_LockLayerUpdate(Unit, win_layer_pair->Layer, R_WM_TRUE);
            
            ret = loc_WinSetFb(Unit, Win, Win->PosX, Win->PosY, Fb);

            /* Unlock */
            loc_LockLayerUpdate(Unit, win_layer_pair->Layer, R_WM_FALSE);
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_WindowCapabilitiesSet
*/
uint32_t R_WM_Sys_WindowCapabilitiesSet(const r_wm_WinCapbs_t Capability0,
                                        const r_wm_WinCapbs_t Capability1,
                                        const r_wm_WinCapbs_t Capability2,
                                        const r_wm_WinCapbs_t Capability3)
{
    r_spea_Error_t  unit_caps_set;
    uint32_t        ret;

    r_wm_DevCabps[R_VDCE_LAYER_SCALER0] = Capability0;
    r_wm_DevCabps[R_VDCE_LAYER_SCALER1] = Capability1;
    r_wm_DevCabps[R_VDCE_LAYER_IMAGE2]  = Capability2;
    r_wm_DevCabps[R_VDCE_LAYER_IMAGE3]  = Capability3;

    unit_caps_set = R_SPEA_UnitCapabilitiesSet( 
        ((Capability0 == R_WM_WINCAPBS_RLE) ? R_SPEA_SPEACAPBS_RLE : R_SPEA_SPEACAPBS_SPRITE),
        ((Capability1 == R_WM_WINCAPBS_RLE) ? R_SPEA_SPEACAPBS_RLE : R_SPEA_SPEACAPBS_SPRITE),
        ((Capability2 == R_WM_WINCAPBS_RLE) ? R_SPEA_SPEACAPBS_RLE : R_SPEA_SPEACAPBS_SPRITE),
        ((Capability3 == R_WM_WINCAPBS_RLE) ? R_SPEA_SPEACAPBS_RLE : R_SPEA_SPEACAPBS_SPRITE));

    if (unit_caps_set != R_SPEA_ERR_OK)
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
  Function: R_WM_Sys_WindowCreate
*/
uint32_t R_WM_Sys_WindowCreate(const uint32_t             Unit, 
                               const r_wm_Window_t *const Win)
{
    uint32_t        ret;
    
    if ((Unit >= R_WM_DEV_NUM) || (Win == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        ret = R_WM_Sys_ReconfAllWindow(Unit);
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_WindowDelete
*/
uint32_t R_WM_Sys_WindowDelete(const uint32_t             Unit, 
                               const r_wm_Window_t *const Win)
{
    uint32_t                     ret;
    uint32_t                     i;
    r_wm_DevSys_t                *dev_sys;
    uint32_t                     layer = LOC_INVALID_LAYER;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        if (Win == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
            ret = R_WM_SYS_NG;
        }
        else
        {
            ret = R_WM_SYS_NG;
            dev_sys = R_WM_Sys_GetDevSys(Unit);
            for (i = 0u; i < R_WM_MAX_LAYERS; i++)
            {
                if (dev_sys->WinToLayerMap[i].Win == Win)
                {
                    layer = dev_sys->WinToLayerMap[i].Layer;
                    ret = R_WM_SYS_OK;
                    break;
                }
                else
                {
                    /* Do nothing. */
                }
            }
            if (ret != R_WM_SYS_OK)
            {
                R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
            }
            else
            {
                if (layer < R_WM_MAX_LAYERS)
                {
                    ret = loc_WindowDelete(Unit, layer);
                }
                else
                {
                    /* Do nothing (Already deleted.) */
                }
            }
        }
    }

    return ret;
}

/***********************************************************
  Function: R_WM_Sys_WindowEnable
*/
uint32_t R_WM_Sys_WindowEnable(const uint32_t              Unit, 
                               const r_wm_Window_t  *const Win, 
                               const uint32_t              Enabled)
{
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    uint32_t                        win_is_on;
    uint32_t                        ret;
    uint32_t                        disabled;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Win);
        if (win_layer_pair == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
            ret = R_WM_SYS_NG;
        }
        else
        {
            if (Enabled == R_WM_TRUE)
            {
                /* do not enable the layer outside of the screen limitations */
                win_is_on = loc_WindowIsOnScreen(Unit, Win);
                if (win_is_on == R_WM_FALSE)
                {
                    /* Do nothing. */
                    ret = R_WM_SYS_OK;
                }
                else
                {
                    ret = loc_VdceLayerEnable(Unit, win_layer_pair->Layer);
                }
            }
            else
            {
                ret = loc_VdceLayerDisable(Unit, win_layer_pair->Layer, &disabled);
            }
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_WindowPosSet
*/
uint32_t R_WM_Sys_WindowPosSet(const uint32_t             Unit, 
                               const r_wm_Window_t *const Win,
                               const int32_t              PosX,
                               const int32_t              PosY,
                               const int32_t              PosZ)
{
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    const void                      *fb;
    uint32_t                        ret = R_WM_SYS_OK;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        if (Win->PosZ != (uint32_t)PosZ)
        {
            /* the Z has changed, the windows need to be reassigned to the layers */
            ret = R_WM_Sys_ReconfAllWindow(Unit);
        }
        else
        {
            /* Do nothing. */
        }
        if (ret == R_WM_SYS_OK)
        {
            win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Win);
            if (win_layer_pair == R_NULL)
            {
                R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
                ret = R_WM_SYS_NG;
            }
            else
            {
                loc_LockLayerUpdate(Unit, win_layer_pair->Layer, R_WM_TRUE);
                
                ret = loc_WinSetViewportAndGeom(
                            Unit,  
                            Win,
                            win_layer_pair->Layer,
                            PosX,
                            PosY,
                            Win->Pitch,
                            Win->Width,
                            Win->Height);
                if (ret == R_WM_SYS_OK)
                {
                    /* due to negative offset workaround, the frame buffer start address 
                       might need correction 
                    */
                    if (Win->Mode != R_WM_WINMODE_SPRITES)
                    {
                        fb = R_WM_WindowVisibleBufGet(Unit, Win);
                        if (fb == R_NULL)
                        {
                            R_WM_SYS_REPORT_ERROR(
                                Unit, R_WM_ERR_NULL_PTR_ARGUMENT, R_WM_SYS_NO_INFO);
                            ret = R_WM_SYS_NG;
                        }
                        else
                        {
                            /* Do nothing. */
                        }
                    }
                    else
                    {
                        fb = R_NULL;
                    }
                    if (ret == R_WM_SYS_OK)
                    {
                        ret = loc_WinSetFb(Unit, Win, PosX, PosY, fb);
                    }
                    else
                    {
                        /* Do nothing. */
                    }
                }
                else
                {
                    /* Do nothing. */
                }
                
                loc_LockLayerUpdate(Unit, win_layer_pair->Layer, R_WM_FALSE);
            }
        }
        else
        {
            /* Do nothing. */
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_WindowGeomSet
*/
uint32_t R_WM_Sys_WindowGeomSet(const uint32_t             Unit,
                                const r_wm_Window_t *const Win,
                                const uint32_t             Pitch,
                                const uint32_t             Width,
                                const uint32_t             Height)
{
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    uint32_t                        ret;
    uint32_t                        disabled;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Win);
        if (win_layer_pair == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
            ret = R_WM_SYS_NG;
        }
        else
        {
            loc_LockLayerUpdate(Unit, win_layer_pair->Layer, R_WM_TRUE);
            
            /* The Layer must be disabled for geometry configuration */
            ret = loc_VdceLayerDisable(Unit, win_layer_pair->Layer, &disabled);
            if (ret ==  R_WM_SYS_OK)
            {
                /* Re-check scaling */
                ret = loc_SetLayerScaling(
                        Unit, 
                        win_layer_pair->Layer, 
                        Win, 
                        R_WM_SCALE_CHANGE_WIN_SIZE, 
                        Width, 
                        Height);
                if (ret == R_WM_SYS_OK)
                {
                    ret = loc_WinSetViewportAndGeom( 
                                Unit, 
                                Win,
                                win_layer_pair->Layer,
                                Win->PosX,
                                Win->PosY,
                                Pitch,
                                Width,
                                Height);
                    if (ret == R_WM_SYS_OK)
                    {
                        /* If the layer was enabled before configuration, enable it again */
                        if (disabled == R_WM_TRUE)
                        {
                            ret = loc_VdceLayerEnable(Unit, win_layer_pair->Layer);
                        }
                        else
                        {
                            /* Do nothing. */
                        }
                    }
                    else
                    {
                        /* Do nothing. */
                    }
                }
                else
                {
                    /* Do nothing. */
                }
            }
            else
            {
                /* Do nothing. */
            }
            
            loc_LockLayerUpdate(Unit, win_layer_pair->Layer, R_WM_FALSE);
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_WindowColorFmtSet
*/
uint32_t R_WM_Sys_WindowColorFmtSet(const uint32_t             Unit,
                                    const r_wm_Window_t *const Win,
                                    const r_wm_WinColorFmt_t   ColorFmt)
{

    r_wm_sys_WinLayerPair_t      *win_layer_pair;
    uint32_t                     ret = R_WM_SYS_OK;
    r_vdce_Error_t               vdce_res;
    uint8_t                      bpp_before;
    uint8_t                      bpp_after;
    r_vdce_Format_t              color_fmt;
    uint32_t                     disabled;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Win);
        if (win_layer_pair == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
            ret = R_WM_SYS_NG;
        }
        else
        {
            /* Check if bpp is changed */
            bpp_before  = R_WM_ColorFmtBitsPerPixGet(Win->ColorFmt);
            bpp_after   = R_WM_ColorFmtBitsPerPixGet(ColorFmt);
            if (bpp_before != bpp_after)
            {
                if (Win->Mode == R_WM_WINMODE_SPRITES)
                {
                    /* Change Pitch size */
                    vdce_res = R_VDCE_LayerMemGeometrySet(Unit, win_layer_pair->Layer, 
                                                              Win->Pitch, Win->Height);
                    if (vdce_res != R_VDCE_ERR_OK)
                    {
                        ret = R_WM_SYS_NG;
                    }
                    else
                    {
                        /* Do nothing. */
                    }
                }
                else
                {
                    /* Do nothing. */
                }
            }
            else
            {
                /* Do nothing. */
            }
            if (ret == R_WM_SYS_OK)
            {
                /* The Layer must be disabled for color format configuration */
                ret = loc_VdceLayerDisable(Unit, win_layer_pair->Layer, &disabled);
                if (ret == R_WM_SYS_OK)
                {
                    /* Set color format */
                    color_fmt = loc_GetVdceFormat(ColorFmt);
                    vdce_res  = R_VDCE_LayerFormatSet(Unit, win_layer_pair->Layer, color_fmt);
                    if (vdce_res != R_VDCE_ERR_OK)
                    {
                        ret = R_WM_SYS_NG;
                    }
                    else
                    {
                        /* If the layer was enabled before configuration, enable it again */
                        if (disabled == R_WM_TRUE)
                        {
                            ret = loc_VdceLayerEnable(Unit, win_layer_pair->Layer);
                        }
                        else
                        {
                            /* Do nothing. */
                        }
                    }
                }
                else
                {
                    /* Do nothing. */
                }
            }
            else
            {
                /* Do nothing. */
            }
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_WindowAlphaSet
*/
uint32_t R_WM_Sys_WindowAlphaSet(const uint32_t             Unit, 
                                 const r_wm_Window_t *const Win, 
                                 const uint8_t              Alpha)
{
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    uint32_t                        ret = R_WM_SYS_OK;
    r_vdce_Error_t                  err;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Win);
        if (win_layer_pair == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
            ret = R_WM_SYS_NG;
        }
        else
        {
            /* Do nothing. */
        }
        if (ret == R_WM_SYS_OK)
        {
            if (win_layer_pair->Win->ColorKey.Enabled == R_WM_TRUE)
            {
                R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
                ret = R_WM_SYS_NG;
            }
            else
            {
                if (win_layer_pair->Layer != R_VDCE_LAYER_SCALER0)
                {
                    err = R_VDCE_LayerAlphaConstEnable(Unit, win_layer_pair->Layer, Alpha);
                    if (err != R_VDCE_ERR_OK)
                    {
                        ret = R_WM_SYS_NG;
                    }
                    else
                    {
                        /* Do nothing. */
                    }
                }
                else
                {
                    /* Do nothing. */
                }
            }
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_WindowPremultipliedAlphaEnable
*/
uint32_t R_WM_Sys_WindowPremultipliedAlphaEnable(const uint32_t             Unit,
                                                 const r_wm_Window_t *const Win,
                                                 const uint8_t              Enabled)
{
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    uint32_t                        ret = R_WM_SYS_OK;
    r_vdce_Error_t                  err;
    uint32_t                        disabled;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Win);
        if (win_layer_pair == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
            ret = R_WM_SYS_NG;
        }
        else
        {
            if (win_layer_pair->Layer != R_VDCE_LAYER_SCALER0)
            {
                /* The Layer must be disabled for pre-multiplied alpha configuration */
                ret = loc_VdceLayerDisable(Unit, win_layer_pair->Layer, &disabled);
                if (ret == R_WM_SYS_OK)
                {
                    if (Enabled == R_WM_TRUE)
                    {
                        /* Enable pre-multi */
                        err = R_VDCE_LayerPremultipliedAlphaEnable(Unit, win_layer_pair->Layer);
                    }
                    else
                    {
                        /* Disable pre-multi */
                        err = R_VDCE_LayerPremultipliedAlphaDisable(Unit, win_layer_pair->Layer);
                    }
                    if (err != R_VDCE_ERR_OK)
                    {
                        ret = R_WM_SYS_NG;
                    }
                    else
                    {
                        /* If the layer was enabled before configuration, enable it again */
                        if (disabled == R_WM_TRUE)
                        {
                            ret = loc_VdceLayerEnable(Unit, win_layer_pair->Layer);
                        }
                        else
                        {
                            /* Do nothing. */
                        }
                    }
                }
                else
                {
                    /* Do nothing. */
                }
            }
            else
            {
                /* Do nothing. */
            }
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_WindowFlagsUpdate
*/
uint32_t R_WM_Sys_WindowFlagsUpdate(const uint32_t             Unit,
                                    const r_wm_Window_t *const Win, 
                                    const r_wm_WinFlags_t      SetFlags, 
                                    const r_wm_WinFlags_t      ClearFlags)
{
    r_wm_sys_WinLayerPair_t      *win_layer_pair;
    r_vdce_Error_t               vdce_res = R_VDCE_ERR_OK;
    uint32_t                     ret;
    uint32_t                     disabled;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Win);
        if (win_layer_pair == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
            ret = R_WM_SYS_NG;
        }
        else if ((((uint32_t)SetFlags) & ((uint32_t)ClearFlags)) !=0u)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_SYS_WIN_FLAG_UPDATE_FAILED, R_WM_SYS_NO_INFO);
            ret = R_WM_SYS_NG;
        }
        else 
        {
            loc_LockLayerUpdate(Unit, win_layer_pair->Layer, R_WM_TRUE);

            /* The Layer must be disabled  */
            ret = loc_VdceLayerDisable(Unit, win_layer_pair->Layer, &disabled);
            if (ret == R_WM_SYS_OK)
            {
                if ((((uint32_t)SetFlags) & ((uint32_t)R_WM_WINFLAG_V_MIRROR))!= 0u)
                {
                    /* Enable vertical mirroring */
                    vdce_res = R_VDCE_LayerModeSet(Unit, win_layer_pair->Layer, 
                                                   R_VDCE_LAYER_MODE_V_MIRORING);
                }
                else if ((((uint32_t)ClearFlags) & ((uint32_t)R_WM_WINFLAG_V_MIRROR)) != 0u)
                {
                    /* Disable vertical mirroring */
                    vdce_res = R_VDCE_LayerModeSet(Unit, win_layer_pair->Layer, 
                                                   R_VDCE_LAYER_MODE_NONE);
                }
                else
                {
                    /* Do nothing. */
                }
                if (vdce_res != R_VDCE_ERR_OK)
                {
                    ret = R_WM_SYS_NG;
                }
                else
                {
                    /* If the layer was enabled before configuration, enable it again */
                    if (disabled == R_WM_TRUE)
                    {
                        ret = loc_VdceLayerEnable(Unit, win_layer_pair->Layer);
                    }
                    else
                    {
                        /* Do nothing. */
                    }
                }
            }
            else
            {
                /* Do nothing. */
            }
            
            loc_LockLayerUpdate(Unit, win_layer_pair->Layer, R_WM_FALSE);
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_WindowClutSet
*/
uint32_t R_WM_Sys_WindowClutSet(const uint32_t                Unit,
                                const r_wm_Window_t *const    Win,
                                const uint32_t                NumEntries,
                                const r_wm_ClutEntry_t *const Clut)
{
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    uint32_t                        i;
    uint32_t                        reconf_flag = R_WM_FALSE;
    uint32_t                        ret = R_WM_SYS_OK;
    r_wm_DevSys_t                   *dev_sys = R_WM_Sys_GetDevSys(Unit);

    if ((Unit >= R_WM_DEV_NUM) || (NumEntries == 0u) || (Clut == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Win);
        if (win_layer_pair == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
            ret = R_WM_SYS_NG;
        }
        else
        {
            /* Layer 0 can't do alpha blending by CLUT alpha */
            if (win_layer_pair->Layer == R_VDCE_LAYER_SCALER0)
            {
                for (i = 0u; i < NumEntries; i++)
                {
                    if (Clut[i].A != LOC_ALPHA_255)
                    {
                        /* CLUTs using alpha values cause changes in Layer assignment */
                        reconf_flag = R_WM_TRUE;
                        break;
                    }
                    else
                    {
                        /* Do nothing. */
                    }
                }
            }
            else
            {
                /* Do nothing. */
            }
            
            if (reconf_flag == R_WM_TRUE)
            {
                /* Layer reconfigutration */
                ret = R_WM_Sys_ReconfAllWindow(Unit);
            }
            else
            {
                /* Set update flag */
                dev_sys->FrameStatusFlags |= 
                (R_WM_SYS_FLAG_ON << (R_WM_SYS_FRAME_BIT_LAYER0_CLUT + win_layer_pair->Layer));
            }
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_WindowColorKeyEnable
*/
uint32_t R_WM_Sys_WindowColorKeyEnable(const uint32_t             Unit,
                                       const r_wm_Window_t *const Win, 
                                       const uint32_t             Enabled)
{
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    r_vdce_Error_t                  vdce_res;
    uint32_t                        ret;
    r_vdce_CkARGB32_t               ck_in;
    r_vdce_CkARGB32_t               ck_out;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Win);
        if (win_layer_pair == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
            ret = R_WM_SYS_NG;
        }
        else
        {
            if (win_layer_pair->Layer == R_VDCE_LAYER_SCALER0)
            {
                /* R_WM_ERR_COLOR_KEYING_NOT_SUPPORTED_FOR_LAYER */
                R_WM_SYS_REPORT_ERROR(
                    Unit, R_WM_ERR_COLOR_KEYING_NOT_SUPPORTED_FOR_LAYER, R_WM_SYS_NO_INFO);
                ret = R_WM_SYS_NG;
            }
            else
            {
                if (Enabled == R_WM_TRUE)
                {
                    /* Layer Alpha and Chroma Keying are exclusive. 
                       Chroma Keying overrides Layer Alpha. */
                    vdce_res = R_VDCE_LayerAlphaConstDisable(Unit, win_layer_pair->Layer);
                    if (vdce_res != R_VDCE_ERR_OK)
                    {
                        /* Do nothing */
                    }
                    else
                    {
                        ck_in.R  = Win->ColorKey.In.RgbKey.Red;
                        ck_in.G  = Win->ColorKey.In.RgbKey.Green;
                        ck_in.B  = Win->ColorKey.In.RgbKey.Blue;
                        ck_in.A  = LOC_ALPHA_0;
                        ck_out.R = Win->ColorKey.Out.Red;
                        ck_out.G = Win->ColorKey.Out.Green;
                        ck_out.B = Win->ColorKey.Out.Blue;
                        ck_out.A = Win->ColorKey.Out.Alpha;
                        
                        vdce_res = R_VDCE_LayerChromaKeyEnable(Unit, win_layer_pair->Layer,
                                                               &ck_in, &ck_out);
                    }
                }
                else
                {
                    vdce_res = R_VDCE_LayerChromaKeyDisable(Unit, win_layer_pair->Layer);
                    if (vdce_res != R_VDCE_ERR_OK)
                    {
                        /* Do nothing */
                    }
                    else
                    {
                        /* Layer Alpha and Chroma Keying are exclusive. 
                           Layer Alpha can be enabled again. */
                        vdce_res = R_VDCE_LayerAlphaConstEnable(Unit, 
                                                                win_layer_pair->Layer, 
                                                                win_layer_pair->Win->Alpha);
                    }
                }
                
                if (vdce_res != R_VDCE_ERR_OK)
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
  Function: R_WM_Sys_WindowScaleSet
*/
uint32_t R_WM_Sys_WindowScaleSet(const uint32_t             Unit,
                                 const r_wm_Window_t *const Win,
                                 const r_wm_ScaleChg_t      ChangeMode,
                                 const uint32_t             ScaledWidth,
                                 const uint32_t             ScaledHeight)
{
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    uint32_t                        ret;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Win);
        if (win_layer_pair == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
            ret = R_WM_SYS_NG;
        }
        else
        {
            loc_LockLayerUpdate(Unit, win_layer_pair->Layer, R_WM_TRUE);
            
            /* Set Scaling  */
            ret = loc_SetLayerScaling(
                    Unit, win_layer_pair->Layer, Win, ChangeMode, ScaledWidth, ScaledHeight);

            loc_LockLayerUpdate(Unit, win_layer_pair->Layer, R_WM_FALSE);
        }
    }
    return ret;
}


