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
#include "r_config_spea.h"
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
  Macro: LOC_VDCECTL_UPDT0 / LOC_VDCECTL_UPDT1
  
  VDCECTL register setting value.
*/
#define LOC_VDCECTL_UPDT0   (1uL << 4)
#define LOC_VDCECTL_UPDT1   (1uL << 5)

/***********************************************************
  Macro: LOC_SPEA_UPDEN_ADR / LOC_SPEA_UPDEN_CLEAR
  
  SPEAUPDEN register address and clear value.
*/
#define LOC_SPEA_UPDEN_ADR    (0xFFC06048u)
#define LOC_SPEA_UPDEN_CLEAR  (0u)

/***********************************************************
  Macro: LOC_MAX_SPRITE_UNIT
  
  Number of Sprite unit / RLE unit 
*/
#define LOC_MAX_SPRITE_UNIT        (4u)

/***********************************************************
  Macro: LOC_UPDATE_CHECK_CNT
  
  Number of update complete check
*/
#if (defined(R_WM_MSQ_DEQUEUE_PROFILING) || defined(R_WM_MSQ_ENQUEUE_PROFILING))
#define LOC_UPDATE_CHECK_CNT       (10u)
#endif

/***********************************************************
  Constants: RLE engine control
*/

#define LOC_RLE_DISABLE_ENG0        (0x0Eu)
#define LOC_RLE_DISABLE_ENG1        (0x0Fu)

/***********************************************************
  Section: Global Variables
*/

/***********************************************************
  Section: Local Function Declarations
*/

R_WM_STATIC uint32_t loc_SpriteGetLyrNoFrmSpriteUnit(const uint32_t      Unit, 
                                                     const r_spea_Unit_t SpeaUnit);

R_WM_STATIC r_spea_Unit_t loc_GetRleUnitFrmLyrNo(const uint32_t Unit, 
                                                 const uint32_t Layer);

R_WM_STATIC uint8_t  loc_GetSpriteUnit(const r_spea_Unit_t SpeaUnit);

R_WM_STATIC uint8_t loc_GetOtherLayersSpriteCount(const uint32_t      Unit, 
                                                  const r_spea_Unit_t SpeaUnit);

R_WM_STATIC void loc_SetOtherLayersSpriteCount(const uint32_t      Unit, 
                                               const r_spea_Unit_t SpeaUnit,
                                               const uint8_t       SpriteCount);
                                               
R_WM_STATIC uint8_t  loc_GetSpriteNo(const uint32_t             Unit, 
                                     const r_wm_Sprite_t *const Sprite, 
                                     const r_spea_Unit_t        SpeaUnit);

R_WM_STATIC uint32_t loc_GetReassignFlag(const uint32_t      Unit, 
                                         const r_spea_Unit_t SpeaUnit);

R_WM_STATIC void     loc_SetSpeaUpdateFlag(const uint32_t      Unit, 
                                           const r_spea_Unit_t SpeaUnit);

R_WM_STATIC uint32_t loc_GetUpdateFlag(const uint32_t      Unit, 
                                       const r_spea_Unit_t SpeaUnit);

R_WM_STATIC uint32_t loc_SpriteResignAllSpritesOfLyr(const uint32_t             Unit,
                                                     const r_wm_Sprite_t *const RootSprite, 
                                                     const uint32_t             Layer);
R_WM_STATIC uint32_t loc_SetRleEngine(const uint32_t              Unit,
                                      const r_wm_WinColorFmt_t    ColorFmt, 
                                      const uint32_t              RleUnit, 
                                      const uint8_t               RleEng, 
                                      const uint32_t              Buf);

/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Variable : loc_speaLayer2UnitAssignment          
*/
R_WM_STATIC const r_spea_Unit_t loc_SpeaLayer2UnitAssignment[R_WM_DEV_NUM][R_WM_MAX_LAYERS] = {
    {R_SPEA_SU3, R_SPEA_SU0, R_SPEA_SU1, R_SPEA_SU2}
#if (R_WM_DEV_NUM > 1)
    ,
    {R_SPEA_SU3, R_SPEA_SU2, R_SPEA_SU1, R_SPEA_SU0}
#endif
};

R_WM_STATIC const r_spea_Unit_t loc_SpUnitUint2Enum[LOC_MAX_SPRITE_UNIT] = {
    R_SPEA_SU0, R_SPEA_SU1, R_SPEA_SU2, R_SPEA_SU3
};

/***********************************************************
  Section: WM Driver Local Functions
*/

/***********************************************************
  Function: R_WM_Sys_GetSpriteUnitFrmLyrNo 
  This function is used to obtain sprite unit from the layer number 

  Parameters:
  Unit - Specifies the WM Unit number.
  Layer -Specifies the Layer Number .

  Return Value :
  r_spea_Unit_t
*/ 

r_spea_Unit_t R_WM_Sys_GetSpriteUnitFrmLyrNo(const uint32_t Unit, 
                                             const uint32_t Layer)
{
    r_spea_Unit_t   ret;
    uint32_t        sp_max;

    if ((Unit < R_WM_DEV_NUM) && (Layer < R_WM_MAX_LAYERS))
    {
        sp_max = R_WM_Sys_DeviceFeature(R_WM_SYS_FEATURE_SPRITE_LAYER_NO);
        if ((sp_max < LOC_MAX_SPRITE_UNIT) && (Layer == R_VDCE_LAYER_SCALER0))
        {
            ret = R_SPEA_SU_LAST;
        }
        else
        {
            ret = loc_SpeaLayer2UnitAssignment[Unit][Layer];
        }
    }
    else
    {
        ret = R_SPEA_SU_LAST;
    }
    return ret;
}
/***********************************************************
  Function: loc_GetRleUnitFrmLyrNo

  This function is used to obtain RLE Unit from the layer number 

  Parameters:
  Unit -  Specifies the WM Unit number.
  Layer - Specifies the layer number.

  Return Value :
  r_spea_Unit_t
*/ 
R_WM_STATIC r_spea_Unit_t loc_GetRleUnitFrmLyrNo(const uint32_t Unit, 
                                                 const uint32_t Layer)
{
    r_spea_Unit_t               ret;
    uint32_t                    rle_max;
    
    static const r_spea_Unit_t loc_RleLayer2UnitAssignment[R_WM_DEV_NUM][ R_WM_MAX_LAYERS] = 
    {
        {R_SPEA_RLE0, R_SPEA_RLE1, R_SPEA_RLE2, R_SPEA_RLE3},
        {R_SPEA_RLE0, R_SPEA_RLE3, R_SPEA_RLE2, R_SPEA_RLE1}
    };

    if ((Unit < R_WM_DEV_NUM) && (Layer < R_WM_MAX_LAYERS))
    {
        rle_max = R_WM_Sys_DeviceFeature(R_WM_SYS_FEATURE_RLE_LAYER_NO);
        if ((rle_max < LOC_MAX_SPRITE_UNIT) && (Layer != R_VDCE_LAYER_SCALER0))
        {
            ret = R_SPEA_SU_LAST;
        }
        else
        {
            ret = loc_RleLayer2UnitAssignment[Unit][Layer];
        }
    }
    else
    {
        ret = R_SPEA_SU_LAST;
    }

    return ret;
}

/***********************************************************
  Function: loc_SpriteGetLyrNoFrmSpriteUnit

   This function is used to obtain Layer Number from the sprite unit 

  Parameters:
  Unit - Specifies the WM Unit number.
  SpeaUnit - Specifies the sprite engine unit number 

  Return Value :
  uint32_t
*/
R_WM_STATIC uint32_t loc_SpriteGetLyrNoFrmSpriteUnit(const uint32_t      Unit, 
                                                     const r_spea_Unit_t SpeaUnit)
{
    uint32_t layer;
    
    for (layer = 0u; layer < R_WM_MAX_LAYERS; layer++)
    {
        if (loc_SpeaLayer2UnitAssignment[Unit][layer] == SpeaUnit)
        {
            break;
        }
        else
        {
            /* Do Nothing */
        }
    }
    return layer;
}

/***********************************************************
  Function: R_WM_Sys_SetSpriteReassignFlag
  see<r_sys_wm.h>
*/
void R_WM_Sys_SetSpriteReassignFlag(const uint32_t      Unit, 
                                    const r_spea_Unit_t SpeaUnit)
{
    uint32_t        mask = 0u;
    r_wm_DevSys_t   *dev_sys;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
    }
    else
    {
        dev_sys = R_WM_Sys_GetDevSys(Unit);
        switch (SpeaUnit)
        {
            case R_SPEA_SU0:
                mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_SU0_REASSIGN;
                break;
            case R_SPEA_SU1:
                mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_SU1_REASSIGN;
                break;
            case R_SPEA_SU2:
                mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_SU2_REASSIGN;
                break;
            case R_SPEA_SU3:
                mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_SU3_REASSIGN;
                break;
            default:
                break;
        }
        dev_sys->FrameStatusFlags |= mask;
    }
}

/***********************************************************
  Function: loc_GetReassignFlag
  This function is used to obtain Reassign Flag 

  Parameters:
  Unit - Specifies the WM unit number 
  SpeaUnit - Specifies the sprite engine unit number.

  ReturnValue:
  R_WM_TRUE   - The flag is on.
  R_WM_FALSE  - The flag is off.
*/
R_WM_STATIC uint32_t loc_GetReassignFlag(const uint32_t      Unit, 
                                         const r_spea_Unit_t SpeaUnit)
{
    uint32_t        mask = 0u;
    uint32_t        ret_val;
    r_wm_DevSys_t   *dev_sys = R_WM_Sys_GetDevSys(Unit);
    
    switch (SpeaUnit)
    {
        case R_SPEA_SU0:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_SU0_REASSIGN;
            break;
        case R_SPEA_SU1:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_SU1_REASSIGN;
            break;
        case R_SPEA_SU2:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_SU2_REASSIGN;
            break;
        case R_SPEA_SU3:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_SU3_REASSIGN;
            break;
        default:
            break;
    }
    if ((dev_sys->FrameStatusFlags & mask) != 0u)
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
  Function: loc_SetSpeaUpdateFlag            
  
  Description:
  Register the Sprite Unit/RLE Unit to be updated with the next frame.

  Parameters:
    Unit -     Specifies the WM Unit Number.
    SpeaUnit - Specifies the Sprtie / RLE Unit.

  Return Value:
    None.
*/
R_WM_STATIC void loc_SetSpeaUpdateFlag(const uint32_t      Unit, 
                                       const r_spea_Unit_t SpeaUnit)
{
    uint32_t        mask = 0u;
    r_wm_DevSys_t   *dev_sys;

    dev_sys = R_WM_Sys_GetDevSys(Unit);
    
    switch (SpeaUnit)
    {
        case R_SPEA_RLE0:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_RLE0_UPDATE;
            break;
        case R_SPEA_RLE1:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_RLE1_UPDATE;
            break;
        case R_SPEA_RLE2:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_RLE2_UPDATE;
            break;
        case R_SPEA_RLE3:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_RLE3_UPDATE;
            break;
        case R_SPEA_SU0:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_SU0_UPDATE;
            break;
        case R_SPEA_SU1:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_SU1_UPDATE;
            break;
        case R_SPEA_SU2:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_SU2_UPDATE;
            break;
        case R_SPEA_SU3:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_SU3_UPDATE;
            break;
        default:
            break;
    }
    dev_sys->FrameStatusFlags |= mask;
}

/***********************************************************
  Function: loc_GetUpdateFlag
  Get the Sprite Unit defined by r_spea_Unit_t to be updated with the next frame.

  Parameters:
  Unit - Specifies the WM Unit Number 
  SpeaUnit - Specifies the sprite engine unit number 

  Return Value:
  R_WM_TRUE   - The flag is on.
  R_WM_FALSE  - The flag is off.
*/
R_WM_STATIC uint32_t loc_GetUpdateFlag(const uint32_t      Unit, 
                                       const r_spea_Unit_t SpeaUnit)
{
    uint32_t      mask = 0u;
    uint32_t      ret_val;
    r_wm_DevSys_t *dev_sys = R_WM_Sys_GetDevSys(Unit);
    
    switch (SpeaUnit)
    {
        case R_SPEA_RLE0:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_RLE0_UPDATE;
            break;
        case R_SPEA_RLE1:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_RLE1_UPDATE;
            break;
        case R_SPEA_RLE2:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_RLE2_UPDATE;
            break;
        case R_SPEA_RLE3:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_RLE3_UPDATE;
            break;
        case R_SPEA_SU0:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_SU0_UPDATE;
            break;
        case R_SPEA_SU1:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_SU1_UPDATE;
            break;
        case R_SPEA_SU2:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_SU2_UPDATE;
            break;
        case R_SPEA_SU3:
            mask = R_WM_SYS_FLAG_ON << R_WM_SYS_FRAME_BIT_SU3_UPDATE;
            break;
        default:
            break;
    }
    if ((dev_sys->FrameStatusFlags & mask) != 0u)
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
  Function: loc_GetSpriteUnit

  This function is used to obtain sprite unit 

  Parameters:
  SpeaUnit - Specifies the sprite engine unit number 

  Return Value :
  sprite unit with uint8_t type 
*/
R_WM_STATIC uint8_t loc_GetSpriteUnit(const r_spea_Unit_t SpeaUnit)
{
    uint8_t sp_unit;
    
    for (sp_unit = 0u; sp_unit < LOC_MAX_SPRITE_UNIT; sp_unit++)
    {
        if (loc_SpUnitUint2Enum[sp_unit] == SpeaUnit)
        {
            break;
        }
    }
    return sp_unit;
}


/***********************************************************
  Function: loc_GetOtherLayersSpriteCount

  This Function is used to obtain sprite count of other 
  unit's window that is using the same sprite unit

  Parameters:
  Unit       - Specifies the WM Unit Number of the current Unit.
  Sprite     - Pointer to any Sprite of the current Window.

  Return Value:
  Sprite count
*/ 
R_WM_STATIC uint8_t loc_GetOtherLayersSpriteCount(const uint32_t      Unit, 
                                                  const r_spea_Unit_t SpeaUnit)
{
    uint8_t                 sprite_cnt = 0u;
#if (R_WM_DEV_NUM > 1)
    uint32_t                other_layer;
    uint32_t                other_wm_unit;
    r_wm_DevSys_t           *other_dev_sys;

    /* Check shared unit status */
    if (Unit == R_WM_UNIT0)
    {
        other_wm_unit = R_WM_UNIT1;
    }
    else
    {
        other_wm_unit = R_WM_UNIT0;
    }
    
    /* Resolve Layer of other Unit */
    other_layer   = loc_SpriteGetLyrNoFrmSpriteUnit(other_wm_unit, SpeaUnit);
    other_dev_sys = R_WM_Sys_GetDevSys(other_wm_unit);
    sprite_cnt    = other_dev_sys->SpriteCount[other_layer];
#endif
    return sprite_cnt;
}

/***********************************************************
  Function: loc_SetOtherLayersSpriteCount

  This Function is used to obtain sprite count of other 
  unit's window that is using the same sprite unit

  Parameters:
  Unit        - Specifies the WM Unit Number of the current Unit.
  Sprite      - Pointer to any Sprite of the current Window.
  SpriteCount - Sprite count.
*/ 
R_WM_STATIC void loc_SetOtherLayersSpriteCount(const uint32_t      Unit, 
                                               const r_spea_Unit_t SpeaUnit,
                                               const uint8_t       SpriteCount)
{
#if (R_WM_DEV_NUM > 1)
    uint32_t                other_layer;
    uint32_t                other_wm_unit;
    r_wm_DevSys_t           *other_dev_sys;

    /* Check shared unit status */
    if (Unit == R_WM_UNIT0)
    {
        other_wm_unit = R_WM_UNIT1;
    }
    else
    {
        other_wm_unit = R_WM_UNIT0;
    }
    
    /* Resolve Layer of other Unit */
    other_layer   = loc_SpriteGetLyrNoFrmSpriteUnit(other_wm_unit, SpeaUnit);
    other_dev_sys = R_WM_Sys_GetDevSys(other_wm_unit);
    other_dev_sys->SpriteCount[other_layer] = SpriteCount;
#endif
}

/***********************************************************
  Function: loc_GetSpriteNo

  This Function is used to obtain sprite number 

  Parameters:
  Unit       - Specifies the WM Unit Number.
  Sprite     - Pointer to the Sprite structure generated by R_WM_SpriteCreate.

  Return Value:
  uint8_t
*/ 
R_WM_STATIC uint8_t loc_GetSpriteNo(const uint32_t             Unit, 
                                    const r_wm_Sprite_t *const Sprite, 
                                    const r_spea_Unit_t        SpeaUnit)
{
    const r_wm_Sprite_t     *tmp_sprite;
    uint8_t                 sprite_no   = LOC_INVALID_SPRITE;
    uint8_t                 ret         = LOC_INVALID_SPRITE;
    uint8_t                 sprite_cnt  = 0;
    uint8_t                 other_sprite_cnt;
    /*
      Iteration of NULL-terminated Linked-List. Maximum Number of expected 
      elements for this device family is R_WM_MAX_SPRITES_PER_LAYERS.
    */
    if (Sprite != R_NULL) 
    {
        tmp_sprite = Sprite->Window->Surface.SpritesRoot;
        while ((tmp_sprite != R_NULL) && (sprite_cnt < R_WM_MAX_SPRITES_PER_LAYER))
        {
            if (tmp_sprite == Sprite)
            {
                sprite_no = sprite_cnt;
            }
            else
            {
                /* Do nothing */
            }
            
            sprite_cnt++;
            tmp_sprite = tmp_sprite->Next;
            
        }
        
        if (sprite_no == LOC_INVALID_SPRITE)
        {
            /* Not found  */
        }
        else
        {
            /* As two layers may share one sprite unit (one on display0, other on display1), 
               both displays can show different sprites, but combined number of sprites 
               on display0 and on display1 must be <= 16 (R_WM_MAX_SPRITES_PER_LAYER).
               
               Display1 will use sprite_no [0, 1, 2 ...], display0 will use sprite_no [.., 13, 14, 15].
               Therefore, if <loc_GetSpriteNo> is counting for display1, 
               we have to align to highest number.
               Please note, that sprite numbers itself still count upwards and are not reversed.
               
               Example of internal mapping:

                        WM1 ->                        <- WM0
               Sprites: [0|1|2|3|4|5|6|7|8|9|10|12|13|14|15]
                    WM1:[0,1,2,3]             WM0:[ 0, 1, 2]
             */

            /* For display0 we have to align to highest indices. 
               We start such, that we end up on sprite 15 
            */
            if (Unit == R_WM_UNIT0)
            {
                ret = (R_WM_MAX_SPRITES_PER_LAYER - sprite_cnt) + sprite_no;
            }
            else
            {
                other_sprite_cnt = loc_GetOtherLayersSpriteCount(Unit, SpeaUnit);
                
                if ((other_sprite_cnt + sprite_no) < R_WM_MAX_SPRITES_PER_LAYER)
                {
                    ret = sprite_no;
                }
                else
                {
                    /* Overflow */
                    ret = LOC_INVALID_SPRITE;
                }
            }
        }
    }
    
    return ret;
}


/***********************************************************
  Group: WM SPEA Update Functions
*/

/***********************************************************
  Function: loc_SpriteResignAllSpritesOfLyr
  
  Rewrite the sprite configuration for the sprite unit identified by the given RootSprite

  Parameters:
  Unit       - Specifies the WM Unit Number.
  RootSprite - <R_WM_Sprite_t> structure pointer
  Layer      - Specifies the layer number

  Return Value:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail
  
*/ 
R_WM_STATIC uint32_t loc_SpriteResignAllSpritesOfLyr(const uint32_t             Unit,
                                                     const r_wm_Sprite_t *const RootSprite,
                                                     const uint32_t             Layer)
{
    /* reassign all the sprites  */
    r_spea_SpriteConfig_t   sprite_conf;
    const r_wm_Sprite_t     *sprite   = RootSprite;
    uint32_t                ret_val   = R_WM_SYS_OK;
    r_spea_Error_t          spea_res;
    r_spea_Unit_t           spea_unit;
    uint8_t                 sp_unit;
    uint8_t                 sprite_no;
    uint8_t                 sprite_no_start;
    uint8_t                 sprite_no_end;
    uint8_t                 sprite_cnt_cur = 0u;
    uint8_t                 sprite_cnt_other;
    r_wm_DevSys_t           *dev_sys;
    
    /* Lock, SPEA will be aceessd by 2 unit */
    R_WM_Sys_LockDevice(Unit);

    spea_unit = R_WM_Sys_GetSpriteUnitFrmLyrNo(Unit, Layer);
    sp_unit   = loc_GetSpriteUnit(spea_unit);
    
    /* Get sprite start number */
    /* 
        Iteration of NULL-terminated Linked-List. Maximum Number of expected 
        elements for this device family is R_WM_MAX_SPRITES_PER_LAYER.
    */
    sprite_cnt_other = loc_GetOtherLayersSpriteCount(Unit, spea_unit);
    if (Unit == R_WM_UNIT1) 
    {
        sprite_no_start = 0u;
        sprite_no_end   = R_WM_MAX_SPRITES_PER_LAYER - sprite_cnt_other;
    }
    else
    {
        sprite_no_start = loc_GetSpriteNo(Unit, RootSprite, spea_unit);
        sprite_no_end   = R_WM_MAX_SPRITES_PER_LAYER;
    }
    
    sprite_no = sprite_no_start;
    while ((sprite != R_NULL) && (sprite_no < sprite_no_end))
    {
        /* Convert types to adjust SPEA I/F */
        sprite_conf.PosX    = (uint16_t)sprite->PosX;
        if (Unit == R_WM_UNIT0)
        {
            sprite_conf.PosY    = (uint16_t)sprite->PosY;
        }
        else
        {
            sprite_conf.PosY    = (uint16_t)sprite->PosY + R_WM_SYS_VIRTUAL_HEIGHT_UNIT0;
        }
        sprite_conf.Width   = (uint16_t)sprite->Width;
        sprite_conf.Height  = (uint16_t)sprite->Height;
        sprite_conf.SrcAddr = (uint32_t)sprite->Data; /* PRQA S 306 */
        sprite_conf.VSync   = (uint8_t)Unit;
        if (sprite->Status == R_WM_SPRITESTATUS_ENABLED)
        {
            sprite_conf.Enable = R_WM_TRUE;
        }
        else
        {
            sprite_conf.Enable = R_WM_FALSE;
        }
        
        spea_res = R_SPEA_SetSprite(
                    R_SPEA_UNIT0, 
                    sp_unit,
                    sprite_no, 
                    &sprite_conf,
                    R_WM_ColorFmtBitsPerPixGet(sprite->Window->ColorFmt));
        if (spea_res != R_SPEA_ERR_OK)
        {
            ret_val = R_WM_SYS_NG;
            break;
        }
        else
        {
            sprite_no++;
            sprite_cnt_cur++;
            sprite = sprite->Next;
        }
    }
    if (ret_val == R_WM_SYS_OK)
    {
        dev_sys = R_WM_Sys_GetDevSys(Unit);
        dev_sys->SpriteCount[Layer] = sprite_cnt_cur;
        
        /* As two layers share one sprite unit (one on display0, other on display1), 
           we cannot simply disable all remaining sprites.
           We have to start after the sprites of Unit0 and need to stop before the sprites 
           of Unit1.
         */
        
        /* Example of internal mapping:

                    WM1 ->                        <- WM0
           Sprites: [0|1|2|3|4|5|6|7|8|9|10|12|13|14|15]
                WM1:[0,1,2,3]             WM0:[ 0, 1, 2]
                            |<--------------->|
                                  Disable
         */
        if (Unit == R_WM_UNIT1) 
        {
            sprite_no_start = sprite_cnt_cur;
            sprite_no_end   = R_WM_MAX_SPRITES_PER_LAYER - sprite_cnt_other;
        } 
        else 
        {
            /* Update sprite count of other unit */
            if ((sprite_cnt_cur + sprite_cnt_other) > R_WM_MAX_SPRITES_PER_LAYER)
            {
                sprite_cnt_other = R_WM_MAX_SPRITES_PER_LAYER - sprite_cnt_cur;
                loc_SetOtherLayersSpriteCount(Unit, spea_unit, sprite_cnt_other);
            }
            
            sprite_no_start = sprite_cnt_other;
            sprite_no_end   = R_WM_MAX_SPRITES_PER_LAYER - sprite_cnt_cur;
        }
        
        for (sprite_no = sprite_no_start; sprite_no < sprite_no_end; sprite_no++)
        {
            /* Disable sprite */
            spea_res = R_SPEA_SpriteEnable(
                                        R_SPEA_UNIT0, 
                                        sp_unit, 
                                        sprite_no, 
                                        R_WM_FALSE);
            if (spea_res != R_SPEA_ERR_OK)
            {
                ret_val = R_WM_SYS_NG;
                break;
            }
            else
            {
                /* Do Nothing */
            }
        }
    }
    else
    {
        /*Do nothing.. */
    }
    if (ret_val == R_WM_SYS_OK) 
    {
        /* Set respective R_WM_SYS_FRAME_STATUS_SPEA_SUn_UPDATE_BIT */
        loc_SetSpeaUpdateFlag(Unit, spea_unit);
    }
    else 
    {
        /*Do nothing. */
    }

    /* Unlock */
    R_WM_Sys_UnlockDevice(Unit);

    return ret_val;
}

/***********************************************************
  Function: R_WM_Sys_ReassignAllSprites
  
  see <r_sys_wm.h>
*/ 
uint32_t R_WM_Sys_ReassignAllSprites(const uint32_t Unit)
{
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    uint32_t                        layer;
    uint32_t                        ret_val = R_WM_SYS_OK;
    uint32_t                        sprite_getreassign_flag;
    r_spea_Unit_t                   spea_unit;
    uint32_t                        sp_max;
    uint32_t                        sp_unit;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        /* Check device feature for Sprite Unit SU3 */
        sp_max = R_WM_Sys_DeviceFeature(R_WM_SYS_FEATURE_SPRITE_LAYER_NO);
        
        /* Rewrite the Sprite configuration for those units */
        /* that got changed during the last frame */
        for (sp_unit = 0u; sp_unit < sp_max; sp_unit++)
        {
            /* Convert to enum type */
            spea_unit = loc_SpUnitUint2Enum[sp_unit];
            
            /* Read respective R_WM_SYS_FRAME_STATUS_SPEA_SUn_REASSIGN_BIT */
            sprite_getreassign_flag = loc_GetReassignFlag(Unit, spea_unit);
            if (sprite_getreassign_flag == R_WM_TRUE)
            {
                /* Update respective window's sprites */
                layer = loc_SpriteGetLyrNoFrmSpriteUnit(Unit, spea_unit);
                win_layer_pair = R_WM_Sys_GetLayerPairByLayer(Unit, layer);
                
                if ((win_layer_pair == R_NULL) || 
                    (win_layer_pair->Win->Mode != R_WM_WINMODE_SPRITES))
                {
                    /* Layer is not in use or is not sprite layer (anymore), */
                    /* deinitialize all sprites */
                    ret_val = loc_SpriteResignAllSpritesOfLyr(Unit, R_NULL, layer);
                }
                else
                {
                    ret_val = loc_SpriteResignAllSpritesOfLyr(
                                    Unit, 
                                    win_layer_pair->Win->Surface.SpritesRoot,
                                    win_layer_pair->Layer);
                }
            }
            else
            {
                /* Do Nothing */
            }
        }
    }
    return ret_val;
}

/***********************************************************
  Function: R_WM_Sys_UpdateAllSprites
  
  see <r_sys_wm.h>
*/
uint32_t R_WM_Sys_UpdateAllSprites(const uint32_t Unit)
{
    uint32_t        update_flag;
    uint32_t        ret_val = R_WM_SYS_OK;
    r_spea_Unit_t   spea_unit;
    uint32_t        sp_max;
    uint32_t        sp_unit;
    r_spea_Error_t  spea_res;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        /* Check device feature about Sprite Unit 3 */
        sp_max = R_WM_Sys_DeviceFeature(R_WM_SYS_FEATURE_SPRITE_LAYER_NO);
        
        /* Set update request bit for those Sprite Unit Registers, */
        /* that have been written to in the last frame */
        for (sp_unit = 0u; sp_unit < sp_max; sp_unit++)
        {
            /* Convert to enum type */
            spea_unit = loc_SpUnitUint2Enum[sp_unit];
            
            /* Read respective R_WM_SYS_FRAME_STATUS_SPEA_SUn_UPDATE_BIT */
            update_flag = loc_GetUpdateFlag(Unit, spea_unit);
            if (update_flag == R_WM_TRUE)
            {
                spea_res = R_SPEA_UpdateUnit(R_SPEA_UNIT0, spea_unit, (uint8_t)Unit);
                if (spea_res != R_SPEA_ERR_OK)
                {
                    ret_val = R_WM_SYS_NG;
                    break;
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                /* do nothing */
            }
        }
    }
    return ret_val;
}

/***********************************************************
  Function: R_WM_Sys_UpdateAllRles
  see<r_sys_wm.h>
*/
uint32_t R_WM_Sys_UpdateAllRles(const uint32_t Unit)
{
    uint32_t        update_flag;
    uint32_t        ret_val = R_WM_SYS_OK;
    r_spea_Unit_t   spea_unit;
    uint32_t        rle_unit;
    r_spea_Error_t  spea_res;
    uint32_t        rle_max;
    
    R_WM_STATIC const r_spea_Unit_t loc_RleUnitUint2Enum[LOC_MAX_SPRITE_UNIT] = {
        R_SPEA_RLE0, R_SPEA_RLE1, R_SPEA_RLE2, R_SPEA_RLE3
    };
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        /* Check device feature about RLE unit */
        rle_max = R_WM_Sys_DeviceFeature(R_WM_SYS_FEATURE_RLE_LAYER_NO);
        
        /* Process update request for those RLE Unit Registers, */
        /* if it has been written to in the last frame */
        for (rle_unit = 0u; rle_unit < rle_max; rle_unit++)
        {
            /* Convert to enum type */
            spea_unit = loc_RleUnitUint2Enum[rle_unit];
            
            update_flag = loc_GetUpdateFlag(Unit, spea_unit);
            if (update_flag == R_WM_TRUE)
            {
                /* For RLE's (rather than Sprites) only parameter <Unit> is relevant. */
                spea_res = R_SPEA_UpdateUnit(rle_unit, spea_unit, (uint8_t)Unit);
                if (spea_res != R_SPEA_ERR_OK)
                {
                    ret_val = R_WM_SYS_NG;
                    break;
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
    return ret_val;
}

/***********************************************************
  Function: R_WM_Sys_SynchronizeSprites
  see<r_sys_wm.h>
*/
uint32_t R_WM_Sys_SynchronizeSprites(const uint32_t Unit)
{
    r_wm_DevSys_t   *dev_sys;
    uint32_t        vdcectl_data;
    uint32_t        speaupden_data;
    uint32_t        ret_val = R_WM_SYS_OK;
    
    /*
        If no window was visible in this frame, 
        pending SPEA Update Requests cannot be completed immediately.
        => Manually create a SYNC signal. 
        Flickering or artifacts are no problem as nothing is visible on-screen!
    */
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        dev_sys = R_WM_Sys_GetDevSys(Unit);
        if (((dev_sys->FrameStatusFlags & R_WM_SYS_FRAME_MASK_SPEA_UPDATE) != 0u)
         && ((dev_sys->GlobalStatusFlags & R_WM_SYS_GLOBAL_FLAG_WIN_EN) == 0u))
        {
            /* Clear SPEAUPDEN register to select S/W update mdoe */
            speaupden_data = R_DEV_READ_REG(32, LOC_SPEA_UPDEN_ADR);
            R_DEV_WRITE_REG(32, LOC_SPEA_UPDEN_ADR, LOC_SPEA_UPDEN_CLEAR);
            
            /* Set VDCECTL register */
            vdcectl_data = R_DEV_READ_REG(32, LOC_VDCECTL);

#if (defined(R_WM_MSQ_DEQUEUE_PROFILING) || defined(R_WM_MSQ_ENQUEUE_PROFILING))
            R_WM_Profile_Log_CheckSPEAUpd();
#endif /* R_WM_MSQ_DEQUEUE_PROFILING || R_WM_MSQ_ENQUEUE_PROFILING */

            /* Check UM 37.6 "Hardware and Software Update" about the use of the UPDTn Bit */
            /* Set SYNC(Unit) Signal to 1 */
            if (Unit == R_WM_UNIT0)
            {
                R_DEV_WRITE_REG(32, LOC_VDCECTL, (uint32_t)(vdcectl_data | LOC_VDCECTL_UPDT0));
            }
            else
            {
                R_DEV_WRITE_REG(32, LOC_VDCECTL, (uint32_t)(vdcectl_data | LOC_VDCECTL_UPDT1));
            }
            /* Set to 0 to create falling edge */
            R_DEV_WRITE_REG(32, LOC_VDCECTL, vdcectl_data);
            
            /* Roll back SPEAUPDEN register */
            R_DEV_WRITE_REG(32, LOC_SPEA_UPDEN_ADR, speaupden_data);
            
            
#if (defined(R_WM_MSQ_DEQUEUE_PROFILING) || defined(R_WM_MSQ_ENQUEUE_PROFILING))
            R_WM_Profile_Log_SPEASWSync();
            for (vdcectl_data = 0u; vdcectl_data < LOC_UPDATE_CHECK_CNT; vdcectl_data++)
            {
                R_WM_Profile_Log_CheckSPEAUpd();
            }
#endif /* R_WM_MSQ_DEQUEUE_PROFILING || R_WM_MSQ_ENQUEUE_PROFILING */
        }
        else
        {
            /* Do Nothing */
        }
    }
    return ret_val;
}

/***********************************************************
  Section: WM Driver Support Functions
  
  Comments see: <WM Support Functions>
*/

/***********************************************************
  Group: WM Video Output Sprite interface
*/

/***********************************************************
  Function: R_WM_Sys_SpriteCreate
  see<r_wm_sys.h>
*/ 
uint32_t R_WM_Sys_SpriteCreate(const uint32_t             Unit, 
                               const r_wm_Sprite_t *const Sprite)
{
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    r_spea_Unit_t                   spea_unit;
    uint32_t                        ret_val = R_WM_SYS_OK;

    if ((Unit >= R_WM_DEV_NUM) || (Sprite == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        /* Check Y-position, Y-position is restricted by WM porting layer specification. */
        if (Unit == R_WM_UNIT0)
        {
#if (R_WM_SYS_VIRTUAL_HEIGHT_UNIT0 != 0)
            if ((Sprite->PosY >= R_WM_SYS_VIRTUAL_HEIGHT_UNIT0) ||
                ((Sprite->PosY + Sprite->Height) > R_WM_SYS_VIRTUAL_HEIGHT_UNIT0))
            {
                R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_RANGE_WM, R_WM_SYS_NO_INFO);
                ret_val = R_WM_SYS_NG;
            }
            else
            {
                /* Do nothing */
            }
#endif
        }
        else
        {
#if (R_WM_SYS_VIRTUAL_HEIGHT_UNIT1 != 0)
            if ((Sprite->PosY >= R_WM_SYS_VIRTUAL_HEIGHT_UNIT1) ||
                ((Sprite->PosY + Sprite->Height) > R_WM_SYS_VIRTUAL_HEIGHT_UNIT1)) 
            {
                R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_RANGE_WM, R_WM_SYS_NO_INFO);
                ret_val = R_WM_SYS_NG;
            }
            else
            {
                /* Do nothing */
            }
#endif
        }
        
        if (ret_val == R_WM_SYS_OK)
        {
            win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Sprite->Window);
            if (win_layer_pair == R_NULL)
            {
                R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
                ret_val = R_WM_SYS_NG;
            }
            else
            {
                spea_unit = R_WM_Sys_GetSpriteUnitFrmLyrNo(Unit, win_layer_pair->Layer);
                R_WM_Sys_SetSpriteReassignFlag(Unit, spea_unit);
            }
        }
    }
    return ret_val;
}

/***********************************************************
  Function: R_WM_Sys_SpriteEnable
  see<r_wm_sys.h>
*/
uint32_t R_WM_Sys_SpriteEnable(const uint32_t             Unit, 
                               const r_wm_Sprite_t *const Sprite, 
                               const uint32_t             Enabled)
{
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    uint8_t                         sprite_no;
    uint8_t                         sprite_unit;
    uint32_t                        ret_val = R_WM_SYS_OK;
    r_spea_Unit_t                   spea_unit;
    r_spea_Error_t                  spea_res;
    
    if ((Unit >= R_WM_DEV_NUM) || (Sprite == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Sprite->Window);
        if (win_layer_pair == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
            ret_val = R_WM_SYS_NG;
        }
        else
        {
            /* Lock, SPEA will be aceessd by 2 unit */
            R_WM_Sys_LockDevice(Unit);
            
            spea_unit   = R_WM_Sys_GetSpriteUnitFrmLyrNo(Unit, win_layer_pair->Layer);
            sprite_unit = loc_GetSpriteUnit(spea_unit);
            sprite_no   = loc_GetSpriteNo(Unit, Sprite, spea_unit);
            if (sprite_no == LOC_INVALID_SPRITE)
            {
                R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_RANGE_WM, R_WM_SYS_NO_INFO);
                ret_val = R_WM_SYS_NG;
            }
            else
            {
                /* Enable/Disable sprite */
                spea_res = R_SPEA_SpriteEnable(
                                R_SPEA_UNIT0, 
                                sprite_unit, 
                                sprite_no, 
                                (uint8_t)Enabled);
                if (spea_res != R_SPEA_ERR_OK)
                {
                    ret_val = R_WM_SYS_NG;
                }
                else
                {
                    /* Set respective R_WM_SYS_FRAME_STATUS_SPEA_SUn_UPDATE_BIT */
                    R_WM_Sys_SetSpriteReassignFlag(Unit, spea_unit);
                }
            }
            
            /* Unlock */
            R_WM_Sys_UnlockDevice(Unit);
        }
    }
    return ret_val;
}

/***********************************************************
  Function: R_WM_Sys_SpriteDelete
  see<r_wm_sys.h>
*/
uint32_t R_WM_Sys_SpriteDelete(const uint32_t             Unit, 
                               const r_wm_Sprite_t *const Sprite)
{
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    r_spea_Unit_t                   spea_unit;
    uint32_t                        ret_val = R_WM_SYS_OK;
    
    if ((Unit >= R_WM_DEV_NUM) || (Sprite == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Sprite->Window);
        if (win_layer_pair == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
            ret_val = R_WM_SYS_NG;
        }
        else
        {
            spea_unit = R_WM_Sys_GetSpriteUnitFrmLyrNo(Unit, win_layer_pair->Layer);
            R_WM_Sys_SetSpriteReassignFlag(Unit, spea_unit);
        }
    }
    return ret_val;
}

/***********************************************************
  Function: R_WM_Sys_SpriteMove
  see< r_wm_sys.h>
*/
uint32_t R_WM_Sys_SpriteMove(const uint32_t             Unit, 
                             const r_wm_Sprite_t *const Sprite,
                             const uint32_t             PosX, 
                             const uint32_t             PosY, 
                             const uint32_t             PosZ)
{
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    uint8_t                         sprite_no;
    uint8_t                         sprite_unit;
    uint16_t                        new_pos_y;
    uint32_t                        ret_val = R_WM_SYS_OK;
    r_spea_Error_t                  spea_res;
    r_spea_Unit_t                   spea_unit;
    
    if ((Unit >= R_WM_DEV_NUM) || (Sprite == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        /* Check Y-position, Y-position is restricted by WM porting layer specification. */
        if (Unit == R_WM_UNIT0)
        {
#if (R_WM_SYS_VIRTUAL_HEIGHT_UNIT0 != 0)
            if ((PosY >= R_WM_SYS_VIRTUAL_HEIGHT_UNIT0) ||
                ((PosY + Sprite->Height) > R_WM_SYS_VIRTUAL_HEIGHT_UNIT0))
            {
                R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_RANGE_WM, R_WM_SYS_NO_INFO);
                ret_val = R_WM_SYS_NG;
            }
            else
#endif
            {
                new_pos_y = (uint16_t)PosY;
            }
        }
        else
        {
#if (R_WM_SYS_VIRTUAL_HEIGHT_UNIT1 != 0)
            if ((PosY >= R_WM_SYS_VIRTUAL_HEIGHT_UNIT1) ||
                ((PosY + Sprite->Height) > R_WM_SYS_VIRTUAL_HEIGHT_UNIT1)) 
            {
                R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_RANGE_WM, R_WM_SYS_NO_INFO);
                ret_val = R_WM_SYS_NG;
            }
            else
#endif
            {
                /* Add offset of Unit0 height  */
                new_pos_y = (uint16_t)PosY + R_WM_SYS_VIRTUAL_HEIGHT_UNIT0;
            }
        }
        
        if (ret_val == R_WM_SYS_OK)
        {
            win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Sprite->Window);
            if (win_layer_pair == R_NULL)
            {
                R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
                ret_val = R_WM_SYS_NG;
            }
            else 
            {
                /* Lock, SPEA will be aceessd by 2 unit */
                R_WM_Sys_LockDevice(Unit);

                /* Get sprite unit & sprite index */
                spea_unit   = R_WM_Sys_GetSpriteUnitFrmLyrNo(Unit, win_layer_pair->Layer);
                sprite_unit = loc_GetSpriteUnit(spea_unit);
                sprite_no   = loc_GetSpriteNo(Unit, Sprite, spea_unit);
                if (sprite_no == LOC_INVALID_SPRITE)
                {
                    R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_RANGE_WM, R_WM_SYS_NO_INFO);
                    ret_val = R_WM_SYS_NG;
                }
                else
                {
                    /* Set new position */
                    spea_res = R_SPEA_SetSpritePos(
                                    R_SPEA_UNIT0, 
                                    sprite_unit, 
                                    sprite_no, 
                                    (uint16_t)PosX, 
                                    new_pos_y,
                                    R_WM_ColorFmtBitsPerPixGet(Sprite->Window->ColorFmt));
                    if (spea_res != R_SPEA_ERR_OK)
                    {
                        ret_val = R_WM_SYS_NG;
                    }
                    else
                    {
                        /* If z-Pos of the spite has changes, */
                        /* all sprite registers have to be reassigned. */
                        if (Sprite->PosZ != PosZ)
                        {
                            R_WM_Sys_SetSpriteReassignFlag(Unit, spea_unit);
                        }
                        else
                        {
                            loc_SetSpeaUpdateFlag(Unit, spea_unit);
                        }
                    }
                }
                
                /* Unlock */
                R_WM_Sys_UnlockDevice(Unit);
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
  Function: R_WM_Sys_SpriteBufSet
  see<r_wm_sys.h>
*/ 
uint32_t R_WM_Sys_SpriteBufSet(const uint32_t             Unit, 
                               const r_wm_Sprite_t *const Sprite, 
                               const void          *const Buf)
{
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    r_spea_SpriteConfig_t           sprite_conf;
    uint8_t                         sprite_no;
    uint8_t                         sprite_unit;
    uint32_t                        ret_val = R_WM_SYS_OK;
    r_spea_Error_t                  spea_res;
    r_spea_Unit_t                   spea_unit;
    
    if ((Unit >= R_WM_DEV_NUM) || (Sprite == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Sprite->Window);
        if (win_layer_pair == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
            ret_val = R_WM_SYS_NG;
        }
        else
        {
            /* Lock, SPEA will be aceessd by 2 unit */
            R_WM_Sys_LockDevice(Unit);

            spea_unit   = R_WM_Sys_GetSpriteUnitFrmLyrNo(Unit, win_layer_pair->Layer);
            sprite_unit = loc_GetSpriteUnit(spea_unit);
            sprite_no   = loc_GetSpriteNo(Unit, Sprite, spea_unit);
            if (sprite_no == LOC_INVALID_SPRITE)
            {
                R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_RANGE_WM, R_WM_SYS_NO_INFO);
                ret_val = R_WM_SYS_NG;
            }
            else
            {
                /* Convert types to adjust SPEA I/F */
                sprite_conf.PosX    = (uint16_t)Sprite->PosX;
                if (Unit == R_WM_UNIT0)
                {
                    sprite_conf.PosY = (uint16_t)Sprite->PosY;
                }
                else
                {
                    sprite_conf.PosY = (uint16_t)Sprite->PosY + R_WM_SYS_VIRTUAL_HEIGHT_UNIT0;
                }
                sprite_conf.Width   = (uint16_t)Sprite->Width;
                sprite_conf.Height  = (uint16_t)Sprite->Height;
                sprite_conf.SrcAddr = (uint32_t)Buf; /* PRQA S 306 */
                sprite_conf.VSync   = (uint8_t)Unit;
                if (Sprite->Status == R_WM_SPRITESTATUS_ENABLED)
                {
                    sprite_conf.Enable = R_WM_TRUE;
                }
                else
                {
                    sprite_conf.Enable = R_WM_FALSE;
                }
                spea_res = R_SPEA_SetSprite(
                                R_SPEA_UNIT0, 
                                sprite_unit, 
                                sprite_no, 
                                &sprite_conf, 
                                R_WM_ColorFmtBitsPerPixGet(Sprite->Window->ColorFmt));
                if (spea_res != R_SPEA_ERR_OK)
                {
                    ret_val = R_WM_SYS_NG;
                }
                else
                {
                    /* Set respective R_WM_SYS_FRAME_STATUS_SPEA_SUn_UPDATE_BIT */
                    loc_SetSpeaUpdateFlag(Unit, spea_unit);
                }
            }
            
            /* Unlock */
            R_WM_Sys_UnlockDevice(Unit);
        }
    }
    return ret_val;
}

/***********************************************************
  Function: R_WM_Sys_WindowDeleteAllSprites
  see<r_wm_sys.h>
*/ 
uint32_t R_WM_Sys_WindowDeleteAllSprites(const uint32_t             Unit,
                                         const r_wm_Window_t *const Win)
{
    r_wm_sys_WinLayerPair_t         *win_layer_pair;
    uint8_t                         sprite_no;
    uint8_t                         sprite_unit;
    uint32_t                        ret_val = R_WM_SYS_OK;
    r_spea_Unit_t                   spea_unit;
    r_spea_Error_t                  spea_res;
    uint8_t                         sprite_no_start;
    uint8_t                         sprite_no_end;
    r_wm_DevSys_t                   *dev_sys;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        win_layer_pair = R_WM_Sys_GetLayerPairByWin(Unit, Win);
        if (win_layer_pair == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, R_WM_SYS_NO_INFO);
            ret_val = R_WM_SYS_NG;
        }
        else
        {
            /* Lock, SPEA will be aceessd by 2 unit */
            R_WM_Sys_LockDevice(Unit);

            dev_sys = R_WM_Sys_GetDevSys(Unit);
            spea_unit   = R_WM_Sys_GetSpriteUnitFrmLyrNo(Unit, win_layer_pair->Layer);
            sprite_unit = loc_GetSpriteUnit(spea_unit);
            
            /* As two layers share one sprite unit (one on display0, other on display1), 
               we cannot simply disable all sprites.
               We have to exclude either the sprites of Unit0 or Unit1.
             */
            if (Unit == R_WM_UNIT1) 
            {
                sprite_no_start = 0u;
                sprite_no_end   = dev_sys->SpriteCount[win_layer_pair->Layer];
            } 
            else 
            {
                sprite_no_start = loc_GetSpriteNo(Unit, Win->Surface.SpritesRoot, spea_unit);
                sprite_no_end   = R_WM_MAX_SPRITES_PER_LAYER;
            }

            for (sprite_no = sprite_no_start; sprite_no < sprite_no_end; sprite_no++)
            {
                /* Disable sprite */
                spea_res = R_SPEA_SpriteEnable(
                                R_SPEA_UNIT0, 
                                sprite_unit, 
                                sprite_no, 
                                R_WM_FALSE);
                if (spea_res != R_SPEA_ERR_OK)
                {
                    ret_val = R_WM_SYS_NG;
                    break;
                }
                else
                {
                    /* Do Nothing */
                }
            }
            if (ret_val == R_WM_SYS_OK)
            {
                dev_sys->SpriteCount[win_layer_pair->Layer] = 0u;
                
                /* Set respective R_WM_SYS_FRAME_STATUS_SPEA_SUn_UPDATE_BIT */
                loc_SetSpeaUpdateFlag(Unit, spea_unit);
            }
            else
            {
                /*Do nothing. */
            }
            
            /* Unlock */
            R_WM_Sys_UnlockDevice(Unit);
        }
    }
    return ret_val;
}

/***********************************************************
  Function: loc_SetRleEngine
  
  Description:
  Sets and Enable the RLE unit.
  
  Parameters:
  Unit    - Specifies the WM unit number.
  Win     - Pointer to the window structure. See <r_wm_Window_t> for more details.
  RleUnit - RLE unit number 
  Buf     - Specifies buffer address. If 0, disable RLE engine.
  
  Return value: 
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail
*/
R_WM_STATIC uint32_t loc_SetRleEngine(const uint32_t              Unit,
                                      const r_wm_WinColorFmt_t    ColorFmt, 
                                      const uint32_t              RleUnit, 
                                      const uint8_t               RleEng, 
                                      const uint32_t              Buf)
{
    r_spea_RleConfig_t  rle_conf;
    uint32_t            ret = R_WM_SYS_OK;
    r_spea_Error_t      sepa_err;

    switch (ColorFmt)
    {
        case R_WM_COLORFMT_RLE24ARGB8888:
        case R_WM_COLORFMT_RLE24RGB0888:
            rle_conf.ColorMode = R_SPEA_CM_24BPP;
            break;
        case R_WM_COLORFMT_RLE18ARGB8888:
        case R_WM_COLORFMT_RLE18RGB0888:
            rle_conf.ColorMode = R_SPEA_CM_18BPP;
            break;
        case R_WM_COLORFMT_RLE8CLUT8:
        case R_WM_COLORFMT_RLE8CLUT4:
        case R_WM_COLORFMT_RLE8CLUT1:
            rle_conf.ColorMode = R_SPEA_CM_8BPP;
            break;
        default:
            ret = R_WM_SYS_NG;
            break;
    }
    if (ret == R_WM_SYS_NG)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_COLORFMT, R_WM_SYS_NO_INFO);
    }
    else
    {
        rle_conf.VirtStart  = R_WM_SYS_VIRTUAL_ADDR_RLE;
        
        if (Buf != 0u)
        {
            /* Enable RLE engine */
            rle_conf.MemStart   = Buf;
            rle_conf.VideoOutID = RleEng;
        }
        else
        {
            /* Disable RLE engine */
            rle_conf.MemStart   = R_WM_SYS_VIRTUAL_ADDR_RLE;
            if (RleEng == R_SPEA_RLE_ENG0)
            {
                rle_conf.VideoOutID = LOC_RLE_DISABLE_ENG0;
            }
            else
            {
                rle_conf.VideoOutID = LOC_RLE_DISABLE_ENG1;
            }
        }
        
        sepa_err = R_SPEA_SetRle(RleUnit, RleEng, &rle_conf);
        if (sepa_err != R_SPEA_ERR_OK)
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
  Function: R_WM_Sys_RleInit
  
  see <r_wm_sys.h>
*/
uint32_t R_WM_Sys_RleInit(const uint32_t Unit) 
{
    uint32_t        ret_val = R_WM_SYS_OK;
    uint32_t        ret0;
    uint32_t        ret1;
    uint32_t        rle_unit;
    uint32_t        rle_max;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        /* Check device feature about RLE unit */
        rle_max = R_WM_Sys_DeviceFeature(R_WM_SYS_FEATURE_RLE_LAYER_NO);
        
        /* Disable all RLE engine to set unique Video out ID */
        for (rle_unit = 0u; rle_unit < rle_max; rle_unit++)
        {
            ret0 = loc_SetRleEngine(
                        Unit, R_WM_COLORFMT_RLE24ARGB8888, rle_unit, R_SPEA_RLE_ENG0, 0u);
            ret1 = loc_SetRleEngine(
                        Unit, R_WM_COLORFMT_RLE24ARGB8888, rle_unit, R_SPEA_RLE_ENG1, 0u);
            
            if ((ret0 != R_WM_SYS_OK) || (ret1 != R_WM_SYS_OK))
            {
                if (ret0 != R_WM_SYS_OK)
                {
                    ret_val = ret0;
                }
                else
                {
                    ret_val = ret1;
                }
                break;
            }
            else
            {
                /* Do nothing */
            }
        }
    }
    return ret_val;
}

/***********************************************************
  Function: R_WM_Sys_RleEnable
  
  see <r_wm_sys.h>
*/
uint32_t R_WM_Sys_RleEnable(const uint32_t              Unit,
                            const r_wm_Window_t  *const Win, 
                            const uint32_t              Layer,
                            const uint8_t               Enable) 
{
    uint32_t            ret_val;
    uint32_t            rle_unit;
    r_spea_Unit_t       spea_unit;
    r_spea_Error_t      sepa_err;
    r_wm_DevSys_t       *dev_sys;
    uint32_t            other_wm_unit;
    uint32_t            rle_addr;
    
    if ((Unit >= R_WM_DEV_NUM) || (Win == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        /* Lock because GlobalStatusFlags can have asynchronous access.  */
        /* e.g. Accessed from other unit control. */
        R_WM_Sys_LockDevice(Unit);

        /* Get SPEA Unit */
        spea_unit = loc_GetRleUnitFrmLyrNo(Unit, Layer);
        rle_unit  = (uint32_t)spea_unit;
        
        dev_sys = R_WM_Sys_GetDevSys(Unit);
        
        if (Enable == R_WM_TRUE)
        {
            /* Get ON_SCREEN buffer, Convert type to adjust SPEA I/F  */
            rle_addr = (uint32_t)R_WM_WindowVisibleBufGet(Unit, Win); /* PRQA S 306 */
            if (rle_addr == 0u)
            {
                R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_SUCH_WINDOW, R_WM_SYS_NO_INFO);
                ret_val = R_WM_SYS_NG;
            }
            else
            {
                /* Enable RLE engine */
                ret_val = loc_SetRleEngine(
                        Unit, Win->ColorFmt, rle_unit, (uint8_t)Unit, rle_addr);
            }
            if (ret_val == R_WM_SYS_OK)
            {
                dev_sys->GlobalStatusFlags |= (R_WM_SYS_GLOBAL_FLAG_RLE0 << rle_unit);

                /* Set respective R_WM_SYS_FRAME_STATUS_SPEA_RLEn_UPDATE_BIT */
                loc_SetSpeaUpdateFlag(Unit, spea_unit);
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* Disable RLE engine */
            ret_val = loc_SetRleEngine(Unit, Win->ColorFmt, rle_unit, (uint8_t)Unit, 0u);
            if (ret_val == R_WM_SYS_OK)
            {
                dev_sys->GlobalStatusFlags &= ~(R_WM_SYS_GLOBAL_FLAG_RLE0 << rle_unit);

                /* Set respective R_WM_SYS_FRAME_STATUS_SPEA_RLEn_UPDATE_BIT */
                loc_SetSpeaUpdateFlag(Unit, spea_unit);
            }
            else
            {
                /* Do nothing */
            }
        }
        if (ret_val == R_WM_SYS_OK)
        {
#if (R_WM_DEV_NUM > 1)
            /* Check shared unit status */
            if (Unit == R_WM_UNIT0)
            {
                other_wm_unit = R_WM_UNIT1;
            }
            else
            {
                other_wm_unit = R_WM_UNIT0;
            }
            
            dev_sys = R_WM_Sys_GetDevSys(other_wm_unit);
            if ((dev_sys->GlobalStatusFlags & (R_WM_SYS_GLOBAL_FLAG_RLE0 << rle_unit)) != 0uL)
            {
                /* If other unit is enabled, we do not control RLE unit */
            }
            else
#endif
            {
                /* Enable / Disable RLE Unit */
                sepa_err = R_SPEA_UnitEnable(rle_unit, spea_unit, Enable);
                if (sepa_err != R_SPEA_ERR_OK)
                {
                    ret_val = R_WM_SYS_NG;
                }
                else
                {
                    /* Do nothing */
                }
            }
        }
        
        /* Unlock */
        R_WM_Sys_UnlockDevice(Unit);
    }
    return ret_val;
}


