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
#include "r_ddb_api.h"      /* display database with display timings */
#include "r_vdce_api.h"
#include "r_config_vdce.h"
#include "r_config_spea.h"
#include "r_spea_api.h"
#include "r_config_wm.h"
#include "r_wm_api.h"
#include "r_wm_sys.h"
#include "r_sys_wm.h"

#ifdef USE_ROS
#include "r_os_api.h"
#endif

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
  Constants:  Unit number

  Definition of number of WM unit 

  LOC_WM_UNIT_NUM_0  - Number of WM unit is zero
  LOC_WM_UNIT_NUM_1  - Number of WM unit is one
  LOC_WM_UNIT_NUM_2  - Number of WM unit is two
  LOC_UNIT_HEX_FF    - Unkown unit number
*/

#define LOC_WM_UNIT_NUM_0           (0u) 
#define LOC_WM_UNIT_NUM_1           (1u) 
#define LOC_WM_UNIT_NUM_2           (2u) 

#ifdef R_WM_SYS_ERROR_TRACKING
#define LOC_UNIT_HEX_FF             (0xffu)
#endif

/***********************************************************
  Constants: Buffer number

  Definition for number of Buffer.
  
  LOC_BUF_NUM_3     - Number of frame buffer is three.
*/

#define LOC_BUF_NUM_3               (3u) 

/***********************************************************
  Constants: number of VDCE interrupt 

  Definition for the nubmer of VDCE interrupt to handle.
  
  LOC_VDCE_INT_NUM  - nubmer of VDCE interrupt to handle.
*/

#define LOC_VDCE_INT_NUM            (11u)

/***********************************************************
  Constants: VI Vsync count mask

  Definition for VI Vsync count mask.
  
  LOC_VSYNC_CNT_MASK     - Mask to get capturing timing.
*/

#define LOC_VSYNC_CNT_MASK          (0x01u) 
#define LOC_VSYNC_CNT_VALUE         (0x00u) 

/***********************************************************
  Constants: CDI control

  Control for CDI sample code.

  LOC_CDI_RESULT_OK  - Normal result of R_CDI_RbSetup
  LOC_CDI_MSG_NUM    - read / write message number
  LOC_CDI_FREE_OK    - Free memory is successed.
*/

#define LOC_CDI_RESULT_OK           (0) /* used as singed */
#define LOC_CDI_MSG_NUM             (1u)
#define LOC_CDI_FREE_OK             (0u) 

/***********************************************************
  Constants: OS control

  Control for OS sample code.

  LOC_SLEEP_TIME   - Wait time in OS ticks in DeInit process
  LOC_SEM_INIT_VAL - Semaphore iniitial value
*/

#ifdef LOC_USE_OS
#define LOC_SLEEP_TIME              (10u)
#define LOC_SEM_INIT_VAL            (0u)
#endif

/***********************************************************
  Section: Local Values
*/

#ifdef R_BSP_SYS_INIT_USE
R_WM_STATIC r_wm_State_t loc_UnitState[R_WM_DEV_NUM];
#else
R_WM_STATIC r_wm_State_t loc_UnitState[R_WM_DEV_NUM] = 
{
    R_WM_STATE_UNINITIALIZED
#if (R_WM_DEV_NUM > 1)
    ,
    R_WM_STATE_UNINITIALIZED
#endif
};
#endif /* R_BSP_SYS_INIT_USE */

/***********************************************************
  Section: Local Function Declarations
*/
R_WM_STATIC void     loc_VblankIsr(const uint32_t Unit, const uint32_t Layer);
R_WM_STATIC void     loc_ScanlineIsr(const uint32_t Unit);
R_WM_STATIC void     loc_VIVblankIsr(const uint32_t CapUnit);
R_WM_STATIC void     loc_IsrVO0Layer0Vblank(void);
R_WM_STATIC void     loc_IsrVO0Layer1Vblank(void);
R_WM_STATIC void     loc_IsrVO0Scanline(void);
R_WM_STATIC void     loc_IsrVI0Vblank(void);
R_WM_STATIC void     loc_IsrVI0Overflow(void);
R_WM_STATIC void     loc_IsrVO0Layer0underflow(void);
R_WM_STATIC void     loc_IsrVO0Layer1underflow(void);
R_WM_STATIC void     loc_IsrVO0Layer2underflow(void);
R_WM_STATIC void     loc_IsrVO0Layer3underflow(void);
R_WM_STATIC void     loc_IsrVO0OirVblank(void);
R_WM_STATIC void     loc_IsrVO0OirScanline(void);
R_WM_STATIC void     loc_IsrVO1Layer0Vblank(void);
R_WM_STATIC void     loc_IsrVO1Layer1Vblank(void);
R_WM_STATIC void     loc_IsrVO1Scanline(void);
R_WM_STATIC void     loc_IsrVI1Vblank(void);
R_WM_STATIC void     loc_IsrVI1Overflow(void);
R_WM_STATIC void     loc_IsrVO1Layer0underflow(void);
R_WM_STATIC void     loc_IsrVO1Layer1underflow(void);
R_WM_STATIC void     loc_IsrVO1Layer2underflow(void);
R_WM_STATIC void     loc_IsrVO1Layer3underflow(void);
R_WM_STATIC void     loc_VOLayerunderflowIsr(const uint32_t Unit, const uint32_t Layer);
R_WM_STATIC void     loc_VIOverflowIsr(const uint32_t CapUnit);
R_WM_STATIC void     loc_VdceErrorCallback(const uint32_t Unit, const r_vdce_Error_t Error);
R_WM_STATIC void     loc_SpeaErrorCallback(const uint32_t Unit, uint32_t Error);
R_WM_STATIC uint32_t loc_CheckInitlizedUnit(void);
R_WM_STATIC uint32_t loc_VdceInit(const uint32_t Unit);
R_WM_STATIC uint32_t loc_SpeaInit(const uint32_t Unit);

/***********************************************************

  Section: Global Variables
*/

/***********************************************************
  Variable: r_wm_DevSys

  Array for device instance specific data of type <r_wm_DevSys_t>
*/

/* will be initialized to zero by compiler/loader*/
R_WM_STATIC r_wm_DevSys_t r_wm_DevSys[R_WM_DEV_NUM];

/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Variable : ROS Synchronisation Objects
  
  Variables used by ROS for threadsafe operation.
*/
#ifdef LOC_USE_OS
R_WM_STATIC R_OS_ThreadMutex_t     loc_MutexMsgQueue[R_WM_DEV_NUM];
R_WM_STATIC R_OS_ThreadMutex_t     loc_MutexBufferMgmnt[R_WM_DEV_NUM];
R_WM_STATIC R_OS_ThreadMutex_t     loc_MutexWindows[R_WM_DEV_NUM];
R_WM_STATIC R_OS_ThreadMutex_t     loc_MutexDevice;
R_WM_STATIC R_OS_Sem_t             loc_SemaISRTrigger[R_WM_DEV_NUM][R_WM_EVENT_LAST];
#ifdef R_BSP_SYS_INIT_USE
R_WM_STATIC uint32_t               loc_MutexInitCreated;
#else
R_WM_STATIC uint32_t               loc_MutexInitCreated = R_WM_FALSE;
#endif
#endif /* LOC_USE_OS */

/***********************************************************
  Variable : loc_Wait*
  
  Synchronisation flags mainly used by non-threading configuration of WM 
  to sync with the interrupts.
*/
R_WM_STATIC volatile uint32_t loc_WaitVblankInt[R_WM_DEV_NUM];
R_WM_STATIC volatile uint32_t loc_WaitScanlineInt[R_WM_DEV_NUM];

/***********************************************************
  Variable : loc_WM_LastError_*
  
  If active, defines variables to save the last occurred error.
*/
#ifdef R_WM_SYS_ERROR_TRACKING
volatile const int8_t *loc_LastErrorFile    = R_NULL;
volatile uint32_t     loc_LastErrorInfo     = R_WM_SYS_NO_INFO;
volatile uint32_t     loc_LastErrorUnit     = R_WM_UNIT0;
volatile uint32_t     loc_LastErrorLine     = 0u;
volatile uint32_t     loc_LastErrorOverflow = R_WM_FALSE;
volatile uint32_t     loc_LastErrorNewError = R_WM_FALSE;
volatile r_wm_Error_t loc_LastErrorCode     = R_WM_ERR_OK;
#endif /* R_WM_SYS_ERROR_TRACKING */

/***********************************************************
  Variable : loc_*Heap_WM
  
  References to the memory manager used for data structure and buffer allocation.
*/
R_WM_STATIC r_cdi_Heap_t *loc_CpuHeapWM ;
R_WM_STATIC r_cdi_Heap_t *loc_VidHeap_WM;

/***********************************************************
  Section: Local Functions
*/

/***********************************************************
  Function: loc_VblankIsr
  
  Description:
  Generic handler for the VBLANK interrupt.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  
  return:
  None
*/
R_WM_STATIC void loc_VblankIsr(const uint32_t Unit, const uint32_t Layer)
{

    r_wm_WinBuffer_t             *buf;
    r_wm_Event_t                 event;
    uint32_t                     buf_addr;
    r_wm_DevSys_t                *dev_sys;
    uint32_t                     capt_unit;
    r_wm_sys_WinLayerPair_t      *win_layer_pair;
    
#if defined(R_WM_SKIP_FRAME_ON_SPEA_BUSY)
    uint32_t         spea_update_pending;
    uint32_t         dev_feature_val;
    
    /* get update flag and filter for unit - if flag is set, do not process this interrupt */
    if (Layer == R_VDCE_LAYER_SCALER0)
    {
        spea_update_pending =  R_SPEA_UnitUpdateActive(R_SPEA_UNIT0, R_SPEA_SU0);
        spea_update_pending |= R_SPEA_UnitUpdateActive(R_SPEA_UNIT0, R_SPEA_SU1);
        spea_update_pending |= R_SPEA_UnitUpdateActive(R_SPEA_UNIT0, R_SPEA_SU2);
        dev_feature_val = R_WM_Sys_DeviceFeature(R_WM_SYS_FEATURE_SPRITE_LAYER_NO);
        if (dev_feature_val == R_SPEA_D1M1A_MAX_SPRITE_UNIT)
        {
            spea_update_pending |= R_SPEA_UnitUpdateActive(R_SPEA_UNIT0, R_SPEA_SU3);
        }
        else
        {
            /* Do Nothing */
        }
        /* Mask with target RLE engine (= WM Unit) */
        spea_update_pending &= (R_WM_SYS_FLAG_ON << Unit);
    }
    else
    {
        spea_update_pending = 0u;
    }
    
    if (spea_update_pending != 0u)
    {
#if (defined(R_WM_MSQ_DEQUEUE_PROFILING) || defined(R_WM_MSQ_ENQUEUE_PROFILING))
        R_WM_Profile_Log_VBLANK_Skipped();
#endif /* R_WM_MSQ_DEQUEUE_PROFILING || R_WM_MSQ_ENQUEUE_PROFILING */
    }
    else
#endif /* R_WM_SKIP_FRAME_ON_SPEA_BUSY */
    {
        if (Layer == R_VDCE_LAYER_SCALER0)
        {
#ifdef LOC_USE_OS
            while (loc_WaitVblankInt[Unit] != 0u)
            {
                R_OS_SemPost(&loc_SemaISRTrigger[Unit][R_WM_EVENT_VBLANK]);
                loc_WaitVblankInt[Unit]--;
            }
#else /* LOC_USE_OS */
            if (loc_WaitVblankInt[Unit] == R_WM_TRUE)
            {
                loc_WaitVblankInt[Unit] = R_WM_FALSE;
            }
            else
            {
                /* Do Nothing */
            }
#endif /* LOC_USE_OS */
            
            capt_unit = Unit;
            event.Id  = R_WM_EVENT_VBLANK;
        }
        else
        {
            capt_unit = (Unit == R_WM_UNIT0) ?  R_WM_UNIT1 : R_WM_UNIT0;
            event.Id  = R_WM_EVENT_LAYER1_VBLANK;
        }

        /* Capture buffer swap */
        dev_sys = R_WM_Sys_GetDevSys(Unit);
        if ((dev_sys->HwLayerEnabled[Layer] & R_WM_SYS_LAYER_FLAG_CAPT_EN) != 0u)
        {
            win_layer_pair = R_WM_Sys_GetLayerPairByLayer(Unit, Layer);
            if ((win_layer_pair != R_NULL)
             && (win_layer_pair->Win->Surface.Fb.BufNum >= LOC_BUF_NUM_3))
            {
                /* Get a free buffer */
                buf = R_WM_Cap_CapBufGet(Unit, win_layer_pair->Win);
                if (buf != R_NULL)
                {
                    /* Convert type to adjust VDCE API specification */
                    buf_addr = (uint32_t)buf->Data; /* PRQA S 306  */
                    
                    /* And capture into it starting with the next frame */
                    (void)R_VDCE_CapBufSet(capt_unit, buf_addr, buf_addr);
                    /* Fall through if error occurs. */
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
        
        /* Notify Callback */
        if ((r_wm_DevSys[Unit].RegEvents  & (R_WM_SYS_FLAG_ON << (uint32_t)event.Id)) != 0u)
        {
            if (r_wm_DevSys[Unit].EventCb != R_NULL)
            {
                r_wm_DevSys[Unit].EventCb(Unit, &event);
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
    } /* spea_update_pending */
}

/***********************************************************
  Function: loc_ScanlineIsr
  
  description:
  Generic handler for the SCANLINE interrupt.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  
  return:
  None
  
*/
R_WM_STATIC void loc_ScanlineIsr(const uint32_t Unit)
{
    r_wm_Event_t event = {R_WM_EVENT_SCANLINE};

    if ((r_wm_DevSys[Unit].RegEvents & (R_WM_SYS_FLAG_ON << (uint32_t)R_WM_EVENT_SCANLINE)) != 0u)
    {
        if (r_wm_DevSys[Unit].EventCb != R_NULL)
        {
            r_wm_DevSys[Unit].EventCb(Unit, &event);
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

#ifdef LOC_USE_OS
    while (loc_WaitScanlineInt[Unit] != 0u)
    {
        R_OS_SemPost(&loc_SemaISRTrigger[Unit][R_WM_EVENT_SCANLINE]);
        loc_WaitScanlineInt[Unit]--;
    }
#else /* LOC_USE_OS */
    if (loc_WaitScanlineInt[Unit] == R_WM_TRUE)
    {
        loc_WaitScanlineInt[Unit] = R_WM_FALSE;
    }
    else
    {
        /* Do Nothing */
    }
#endif /* LOC_USE_OS */
}

/***********************************************************
  Function: loc_VIVblankIsr
  
  Description:
  Generic handler for the Video Input VBLANK interrupt.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  
  return:
  None
*/
R_WM_STATIC void loc_VIVblankIsr(const uint32_t CapUnit)
{
    r_wm_WinBuffer_t *buf;
    r_wm_Event_t     event = {R_WM_EVENT_VI_VBLANK};
    uint32_t         cnt_mask;
    uint32_t         layer;
    uint32_t         wm_unit;
    
    r_wm_DevSys[CapUnit].CaptCount++;
    
    if (((r_wm_DevSys[CapUnit].GlobalStatusFlags & R_WM_SYS_GLOBAL_FLAG_CAPT_EN) != 0u) &&
        (r_wm_DevSys[CapUnit].RootCapt         != R_NULL) && 
        (r_wm_DevSys[CapUnit].RootCapt->Window != R_NULL) &&
        (((uint32_t)r_wm_DevSys[CapUnit].RootCapt->Mode & (uint32_t)R_WM_CAPMODE_SYNC_ONLY) == 0u))
    {
        if ((r_wm_DevSys[CapUnit].GlobalStatusFlags & R_WM_SYS_GLOBAL_FLAG_CAPT_BOB) != 0u)
        {
            /* Write once every two Vsync */
            cnt_mask = LOC_VSYNC_CNT_MASK;
        }
        else
        {
            /* Write every Vsync */
            cnt_mask = LOC_VSYNC_CNT_VALUE;
        }
        if ((r_wm_DevSys[CapUnit].CaptCount & cnt_mask) == LOC_VSYNC_CNT_VALUE)
        {
            /* Can we do triple buffering? */
            if (r_wm_DevSys[CapUnit].RootCapt->Window->Surface.Fb.BufNum >= LOC_BUF_NUM_3)
            {
                /* Check layer 0 of same unit */
                layer = R_VDCE_LAYER_SCALER0;
                wm_unit = CapUnit;

#if (R_WM_DEV_NUM > 1)
                if ((r_wm_DevSys[wm_unit].HwLayerEnabled[layer] & R_WM_SYS_LAYER_FLAG_CAPT_EN) != 0u)
                {
                    /* Do nothing */
                }
                else
                {
                    /* It should be layer 1 */
                    layer   = R_VDCE_LAYER_SCALER1;
                    wm_unit = (CapUnit == R_WM_UNIT0) ?  R_WM_UNIT1 : R_WM_UNIT0;
                }
#endif
                /* Check lock flag */
                if ((r_wm_DevSys[wm_unit].HwLayerEnabled[layer] & R_WM_SYS_LAYER_FLAG_LOCK) != 0u)
                {
                    /* pendign update */
                    r_wm_DevSys[wm_unit].CaptPending[layer] = R_WM_TRUE;
                }
                else
                {
                    /* Get freshly captured buffer  */
                    buf = R_WM_Cap_DispBufGet(wm_unit, r_wm_DevSys[CapUnit].RootCapt->Window);
                    
                    if (buf != R_NULL)
                    {
                        /* And display it with the next frame */
                        /* Convert types to adjust VDCE I/F  */
                        (void)R_VDCE_LayerBufSet(wm_unit, layer, (uint32_t)buf->Data); /* PRQA S 306 */
                        /* Fall through if error occurs. */
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
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
    
    if ((r_wm_DevSys[CapUnit].RegEvents & (R_WM_SYS_FLAG_ON << (uint32_t)R_WM_EVENT_VI_VBLANK)) != 0u)
    {
        if (r_wm_DevSys[CapUnit].EventCb != R_NULL)
        {
            r_wm_DevSys[CapUnit].EventCb(CapUnit, &event);
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

/***********************************************************
  Function: loc_VIOverflowIsr
  
  Description:
  Generic handler for the overflow interrupt.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  
  return:
  None
*/
R_WM_STATIC void loc_VIOverflowIsr(const uint32_t CapUnit)
{
    r_wm_Event_t event = {R_WM_EVENT_VI_OVERFLOW};

    if ((r_wm_DevSys[CapUnit].RegEvents & (R_WM_SYS_FLAG_ON << (uint32_t)R_WM_EVENT_VI_OVERFLOW))
        != 0u)
    {
        if (r_wm_DevSys[CapUnit].EventCb != R_NULL)
        {
            r_wm_DevSys[CapUnit].EventCb(CapUnit, &event);
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


/***********************************************************
  Function: loc_VOLayerunderflowIsr
  
  Description:
  Generic handler for the undeflow interrupt.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Layer  - VDCE layer Number (0,1,2,3)
  
  return:
  None
*/
R_WM_STATIC void loc_VOLayerunderflowIsr(const uint32_t Unit, const uint32_t Layer)
{
    r_wm_Event_t event = {R_WM_EVENT_LAYER0_UNDERFLOW};

    /* see what layer is received */
    if (Layer == R_VDCE_LAYER_SCALER0)
    {
        event.Id = R_WM_EVENT_LAYER0_UNDERFLOW;
    }
    else if (Layer == R_VDCE_LAYER_SCALER1)
    {
        event.Id = R_WM_EVENT_LAYER1_UNDERFLOW;
    }
    else if (Layer == R_VDCE_LAYER_IMAGE2)
    {
        event.Id = R_WM_EVENT_LAYER2_UNDERFLOW ;
    }
    else /* layer 3 underflow */
    {
        event.Id = R_WM_EVENT_LAYER3_UNDERFLOW;
    }

    if ((r_wm_DevSys[Unit].RegEvents & (R_WM_SYS_FLAG_ON << (uint32_t)event.Id)) != 0u)
    {
        if (r_wm_DevSys[Unit].EventCb != R_NULL)
        {
            r_wm_DevSys[Unit].EventCb(Unit, &event);
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

/***********************************************************
  Function: loc_IsrVO0Layer0Vblank
  
  Description:
  VDCE Vsync ISR for Unit 0 Scaler 0.
  
  Parameter:
  None.
  
  return:
  None.
*/
R_WM_STATIC void loc_IsrVO0Layer0Vblank(void)
{
    loc_VblankIsr(R_WM_UNIT0, R_VDCE_LAYER_SCALER0);
}

/***********************************************************
  Function: loc_IsrVO0Layer1Vblank
  
  Description:
  VDCE Vsync ISR for Unit 0 Scaler 1.
  
  Parameter:
  None.
  
  return:
  None.
*/
R_WM_STATIC void loc_IsrVO0Layer1Vblank(void)
{
    loc_VblankIsr(R_WM_UNIT0, R_VDCE_LAYER_SCALER1);
}

/***********************************************************
  Function: loc_IsrVO0Scanline
  
  Description:
  VDCE SCANLINE ISR for VOUT unit 0.
  
  Parameter:
  None.
  
  return:
  None.
*/

R_WM_STATIC void loc_IsrVO0Scanline(void)
{
    loc_ScanlineIsr(R_WM_UNIT0);
}

/***********************************************************
  Function: loc_IsrVI0Vblank
  
  Description:
  VDCE VBLANK ISR for VIN unit 0.
  
  Parameter:
  None.
  
  return:
  None.
*/
R_WM_STATIC void loc_IsrVI0Vblank(void)
{
    loc_VIVblankIsr(R_WM_UNIT0);
}

/***********************************************************
  Function: loc_IsrVI0Overflow
  
  Description:
  VDCE Overflow ISR for VIN unit 0.
  
  Parameter:
  None.
  
  return:
  None.
*/
R_WM_STATIC void loc_IsrVI0Overflow(void)
{
    loc_VIOverflowIsr(R_WM_UNIT0);
}

/***********************************************************
  Function: loc_IsrVO0Layer0underflow
  
  Description:
  VDCE underflow layer 0 ISR for VO unit 0.
  
  Parameter:
  None.
  
  return:
  None.
*/
R_WM_STATIC void loc_IsrVO0Layer0underflow(void)
{
    loc_VOLayerunderflowIsr(R_WM_UNIT0, R_VDCE_LAYER_SCALER0);
}

/***********************************************************
  Function: loc_IsrVO0Layer1underflow
  
  Description:
  VDCE underflow layer 1 ISR for VO unit 0.
  
  Parameter:
  None.
  
  return:
  None.
*/
R_WM_STATIC void loc_IsrVO0Layer1underflow(void)
{
    loc_VOLayerunderflowIsr(R_WM_UNIT0, R_VDCE_LAYER_SCALER1);
}

/***********************************************************
  Function: loc_IsrVO0Layer2underflow
  
  Description:
  VDCE underflow layer 2 ISR for VO unit 0.
  
  Parameter:
  None.
  
  return:
  None.
*/
R_WM_STATIC void loc_IsrVO0Layer2underflow(void)
{
    loc_VOLayerunderflowIsr(R_WM_UNIT0, R_VDCE_LAYER_IMAGE2);
}

/***********************************************************
  Function: loc_IsrVO0Layer3underflow
  
  Description:
  VDCE underflow layer 3 ISR for VO unit 0.
  
  Parameter:
  None.
  
  return:
  None.
*/
R_WM_STATIC void loc_IsrVO0Layer3underflow(void)
{
    loc_VOLayerunderflowIsr(R_WM_UNIT0, R_VDCE_LAYER_IMAGE3);
}

/***********************************************************
  Function: loc_IsrVO0OirVblank
  
  Description:
  VDCE OIR Vsync ISR for Unit 0.
  
  Parameter:
  None.
  
  return:
  None.
*/
R_WM_STATIC void loc_IsrVO0OirVblank(void)
{
    r_wm_Event_t event = {R_WM_EVENT_OIR_VBLANK};
    uint32_t     unit = 0u;
    
    if ((r_wm_DevSys[unit].RegEvents & (R_WM_SYS_FLAG_ON << (uint32_t)event.Id)) != 0u)
    {
        if (r_wm_DevSys[unit].EventCb != R_NULL)
        {
            r_wm_DevSys[unit].EventCb(unit, &event);
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

/***********************************************************
  Function: loc_IsrVO0OirScanline
  
  Description:
  VDCE OIR scanline ISR for Unit 0.
  
  Parameter:
  None.
  
  return:
  None.
*/
R_WM_STATIC void loc_IsrVO0OirScanline(void)
{
    r_wm_Event_t event = {R_WM_EVENT_OIR_SCANLINE};
    uint32_t     unit = 0u;
    
    if ((r_wm_DevSys[unit].RegEvents & (R_WM_SYS_FLAG_ON << (uint32_t)event.Id)) != 0u)
    {
        if (r_wm_DevSys[unit].EventCb != R_NULL)
        {
            r_wm_DevSys[unit].EventCb(unit, &event);
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

#if (R_WM_DEV_NUM > 1)

/***********************************************************
  Function: loc_IsrVO1Layer0Vblank
  
  Description:
  VDCE Vsync ISR for Unit 1 Scaler 0.
  
  Parameter:
  None.
  
  return:
  None.
*/

R_WM_STATIC void loc_IsrVO1Layer0Vblank(void)
{
    loc_VblankIsr(R_WM_UNIT1, R_VDCE_LAYER_SCALER0);
}

/***********************************************************
  Function: loc_IsrVO1Layer1Vblank
  
  Description:
  VDCE Vsync ISR for Unit 1 Scaler 1.
  
  Parameter:
  None.
  
  return:
  None.
*/

R_WM_STATIC void loc_IsrVO1Layer1Vblank(void)
{
    loc_VblankIsr(R_WM_UNIT1, R_VDCE_LAYER_SCALER1);
}

/***********************************************************
  Function: loc_IsrVO1Scanline
  
  Description:
  VDCE SCANLINE ISR for VOUT unit 1.
  
  Parameter:
  None.
  
  return:
  None.
*/

R_WM_STATIC void loc_IsrVO1Scanline(void)
{
    loc_ScanlineIsr(R_WM_UNIT1);
}

/***********************************************************
  Function: loc_IsrVI1Vblank
  
  Description:
  VDCE VBLANK ISR for VIN unit 1.
  
  Parameter:
  None.
  
  return:
  None.
*/
R_WM_STATIC void loc_IsrVI1Vblank(void)
{
    loc_VIVblankIsr(R_WM_UNIT1);
}

/***********************************************************
  Function: loc_IsrVI1Overflow
  
  Description:
  VDCE Overflow ISR for VIN unit 1.
  
  Parameter:
  None.
  
  return:
  None.
*/
R_WM_STATIC void loc_IsrVI1Overflow(void)
{
    loc_VIOverflowIsr(R_WM_UNIT1);
}

/***********************************************************
  Function: loc_IsrVO1Layer0underflow
  
  Description:
  VDCE underflow layer 0 ISR for VO unit 1.
  
  Parameter:
  None.
  
  return:
  None.
*/
R_WM_STATIC void loc_IsrVO1Layer0underflow(void)
{
    loc_VOLayerunderflowIsr(R_WM_UNIT1, R_VDCE_LAYER_SCALER0);
}

/***********************************************************
  Function: loc_IsrVO1Layer1underflow
  
  Description:
  VDCE underflow layer 1 ISR for VO unit 1.
  
  Parameter:
  None.
  
  return:
  None.
*/
R_WM_STATIC void loc_IsrVO1Layer1underflow(void)
{
    loc_VOLayerunderflowIsr(R_WM_UNIT1, R_VDCE_LAYER_SCALER1);
}

/***********************************************************
  Function: loc_IsrVO1Layer2underflow
  
  Description:
  VDCE underflow layer 2 ISR for VO unit 1.
  
  Parameter:
  None.
  
  return:
  None.
*/
R_WM_STATIC void loc_IsrVO1Layer2underflow(void)
{
    loc_VOLayerunderflowIsr(R_WM_UNIT1, R_VDCE_LAYER_IMAGE2);
}

/***********************************************************
  Function: loc_IsrVO1Layer3underflow
  
  Description:
  VDCE underflow layer 3 ISR for VO unit 1.
  
  Parameter:
  None.
  
  return:
  None.
*/
R_WM_STATIC void loc_IsrVO1Layer3underflow(void)
{
    loc_VOLayerunderflowIsr(R_WM_UNIT1, R_VDCE_LAYER_IMAGE3);
}

#endif /* (R_WM_DEV_NUM > 1) */
 
#ifdef R_WM_SYS_ERROR_TRACKING
/***********************************************************
Function: R_WM_Sys_GetLastError

see <r_wm_sys.h>
*/
r_wm_Error_t R_WM_Sys_GetLastError(uint32_t *const Unit, 
                                   uint32_t *const AdditionalInfo, 
                                   uint32_t *const Overflow, 
                                   uint32_t *const New)
{
    if (Unit != R_NULL)
    {
        *Unit = loc_LastErrorUnit;
    }
    else
    {
        /* Do Nothing */
    }

    if (AdditionalInfo != R_NULL)
    {
        *AdditionalInfo = loc_LastErrorInfo;
    }
    else
    {
        /* Do Nothing */
    }

    if (Overflow != R_NULL)
    {
        *Overflow = loc_LastErrorOverflow;
    }
    else
    {
        /* Do Nothing */
    }

    if (New != R_NULL)
    {
        *New = loc_LastErrorNewError;
    }
    else
    {
        /* Do Nothing */
    }
    
    loc_LastErrorNewError = R_WM_FALSE;
    loc_LastErrorOverflow = R_WM_FALSE;
    
    return loc_LastErrorCode;
}

/***********************************************************
  Function: loc_WM_Sys_SetLastError
  
  Description:
  capture the last error occurred.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Layer  - VDCE layer Number (0,1,2,3)
  
  return:
  None
*/
void loc_WM_Sys_SetLastError(const uint32_t      Unit, 
                             const r_wm_Error_t  Error, 
                             const uint32_t      AdditionalInfo, 
                             const int8_t *const File,
                             const uint32_t      Line)
{
    if (AdditionalInfo != R_WM_SYS_NO_INFO)
    {
        R_BSP_STDIO_Printf("ERROR in %s, Line %u, Unit %u, Code %u/%u\n", File, Line, Unit,
                            Error, AdditionalInfo);
    }
    else
    {
        R_BSP_STDIO_Printf("ERROR in %s, Line %u, Unit %u, Code %u\n", File, Line, Unit,
                           Error);
    }
    
    if (loc_LastErrorNewError == R_WM_FALSE)
    {
        loc_LastErrorCode = Error;
        loc_LastErrorInfo = AdditionalInfo;
        loc_LastErrorUnit = Unit;
        loc_LastErrorFile = File;
        loc_LastErrorLine = Line;
        loc_LastErrorNewError = R_WM_TRUE;
    }
    else
    {
        loc_LastErrorOverflow = R_WM_TRUE;
    }
}

#endif /* R_WM_SYS_ERROR_TRACKING */

/***********************************************************
  Function: loc_VdceErrorCallback
  
  Description:
  Function registered as error callback for the VDCE driver.
  
  
  Parameter:
  Unit  - See description of <R_WM_DevInit>
  Error - The type describes the error code of VDCE driver functions.
  
  return:
  None
*/
R_WM_STATIC void loc_VdceErrorCallback(const uint32_t       Unit, 
                                       const r_vdce_Error_t Error)
{
    if (Error != R_VDCE_ERR_OK)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_VOUT_INTERNAL, Error);
        R_WM_ErrorHandler(Unit, R_WM_ERR_VOUT_INTERNAL);
    }
    else
    {
        /* Do Nothing */
    }
}

/***********************************************************
  Function: loc_SpeaErrorCallback
  
  Description:
  Function registered as error callback for the SPEA driver.
  
  Parameter:
  Unit  - See description of <R_WM_DevInit>
  Error - The type describes the error code of SPEA.
  
  return:
  None
*/
R_WM_STATIC void loc_SpeaErrorCallback(const uint32_t Unit, 
                                       uint32_t       Error)
{
    if (Error != (uint32_t)R_SPEA_ERR_OK)
    {
        R_WM_SYS_REPORT_ERROR(R_SPEA_UNIT0, R_WM_ERR_SPEA_INTERNAL, Error);
        R_WM_ErrorHandler(R_SPEA_UNIT0, R_WM_ERR_SPEA_INTERNAL);
    }
    else
    {
        /* Do Nothing */
    }
}

/***********************************************************
  Function: loc_CheckInitlizedUnit
  
  Description:
  Return the number of units that is not UNINITIALIZED state
  
  Parameter:
  Unit  - See description of <R_WM_DevInit>
  
  return:
  The number of initalized Unit.
*/
R_WM_STATIC uint32_t loc_CheckInitlizedUnit(void)
{
    uint32_t        unit;
    uint32_t        count = 0u;
    r_wm_State_t    status;
    
    for (unit = 0; unit < R_WM_DEV_NUM; unit++)
    {
        /* Check WM unit status */
        status = R_WM_Sys_StateGet(unit);
        if (status != R_WM_STATE_UNINITIALIZED)
        {
            count++;
        }
    }
    return count;
}

/***********************************************************
  Function: loc_VdceInit
  
  Description:
  Initialize VDCE driver.
  
  Parameter:
  Unit  - See description of <R_WM_DevInit>
  
  return:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail
*/
R_WM_STATIC uint32_t loc_VdceInit(const uint32_t Unit)
{
    uint32_t        ret_val;
    r_vdce_Error_t  vdce_error_status;
    
    vdce_error_status = R_VDCE_Init(Unit);
    if (vdce_error_status != R_VDCE_ERR_OK)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_VOUT_INTERNAL, vdce_error_status);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        /* Set VDCE error callback */
        vdce_error_status = R_VDCE_ErrorCallbackSet(&loc_VdceErrorCallback);
        if (vdce_error_status != R_VDCE_ERR_OK)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_VOUT_INTERNAL, vdce_error_status);
            ret_val = R_WM_SYS_NG;
        }
        else
        {
            /* Display Disable */
            vdce_error_status = R_VDCE_DisplayDisable(Unit);
            if (vdce_error_status != R_VDCE_ERR_OK)
            {
                ret_val = R_WM_SYS_NG;
            }
            else
            {
                ret_val = R_WM_SYS_OK;
            }
        }
    }
    return ret_val;
}

/***********************************************************
  Function: loc_SpeaInit
  
  Description:
  Initialize SPEA driver.
  
  Parameter:
  Unit  - See description of <R_WM_DevInit>
  
  return:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail
*/
R_WM_STATIC uint32_t loc_SpeaInit(const uint32_t Unit)
{
    uint32_t        ret_val;
    r_spea_Error_t  spea_error_status;
    
    /* Initialize SPEA when first Initalize */
    spea_error_status = R_SPEA_Init(R_SPEA_UNIT0);
    if (spea_error_status != R_SPEA_ERR_OK)
    {
        R_WM_SYS_REPORT_ERROR(
            R_SPEA_UNIT0, R_WM_ERR_SPEA_INTERNAL, spea_error_status);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        /* Set SPEA error callback */
        R_SPEA_SetErrorCallback(&loc_SpeaErrorCallback);
        
        ret_val = R_WM_Sys_RleInit(Unit);
    }
    return ret_val;
}

/***********************************************************
  Section: WM Driver Support Functions
  
  Comments see: <WM Support Functions>
*/

/***********************************************************
  Function: R_WM_Sys_InitGlobal

  see <r_wm_sys.h>
*/
void R_WM_Sys_InitGlobal(void)
{
    uint32_t i;
    
    for (i = 0u; i < R_WM_DEV_NUM; i++)
    {
        loc_UnitState[i] = R_WM_STATE_UNINITIALIZED;
    }
#ifdef LOC_USE_OS
    loc_MutexInitCreated = R_WM_FALSE;
#endif
    
    R_WM_Sys_InitCapabilities();

}

/***********************************************************
  Function: R_WM_Sys_DeviceFeature
  
  see <r_wm_sys.h>
*/
uint32_t R_WM_Sys_DeviceFeature(const r_wm_sys_DevFeature_t Feature)
{
    r_dev_Device_t  device;
    uint32_t        ret_val;

    device = R_DEV_GetDev();
    
    switch (Feature) 
    {
        case R_WM_SYS_FEATURE_RLE_LAYER_NO:
            if ((device == R_DEV_R7F701418) || (device == R_DEV_R7F701441) || 
                (device == R_DEV_R7F701442)) 
            {
                ret_val = R_SPEA_D1M1A_MACRO_NUM;
            }
            else 
            {
                ret_val = R_SPEA_MACRO_NUM;
            }
            break;
            
        case R_WM_SYS_FEATURE_SPRITE_LAYER_NO:
            if ((device == R_DEV_R7F701418) || (device == R_DEV_R7F701441) || 
                (device == R_DEV_R7F701442)) 
            {
                ret_val = R_SPEA_D1M1A_MAX_SPRITE_UNIT;
            }
            else 
            {
                ret_val = R_SPEA_MAX_SPRITE_UNIT;
            }
            break;
            
        case R_WM_SYS_FEATURE_SWITCH_CAPABILITIES:
            /* Only supported by newest devices. Older devices have a static assignment */
            if ((device == R_DEV_R7F701441) || (device == R_DEV_R7F701418) || 
                (device == R_DEV_R7F701442))
            {
                ret_val = R_WM_TRUE;
            }
            else 
            {
                ret_val = R_WM_FALSE;
            }
            break;
            
        case R_WM_SYS_FEATURE_GAMMA_CORRECTION:
            /* Not supported by D1L2(H). Lower devices haven't a compatible VO at all */
            if (device <= R_DEV_R7F701403)
            {
                ret_val = R_WM_FALSE;
            }
            else 
            {
                ret_val = R_WM_TRUE;
            }
            break;
            
        case R_WM_SYS_FEATURE_SCALING:
            /* Not supported by D1L2(H). Lower devices haven't a compatible VO at all */
            if (device <= R_DEV_R7F701403)
            {
                ret_val = R_WM_FALSE;
            }
            else 
            {
                ret_val = R_WM_TRUE;
            }
            break;
            
        default:
            ret_val = R_WM_FALSE;
            break;
    }
    return ret_val;
}

/***********************************************************
  Group: WM Basic interface functions
*/

/***********************************************************
  Function: R_WM_Sys_StateSet
  
  see <r_wm_sys.h>
*/
r_wm_Error_t R_WM_Sys_StateSet(const uint32_t     Unit,
                               const r_wm_State_t State)
{
    r_wm_Error_t err = R_WM_ERR_OK;
    
    if (Unit < R_WM_DEV_NUM)
    {
        loc_UnitState[Unit] = State;
    }
    else
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NG, R_WM_SYS_NO_INFO);
        err = R_WM_ERR_NG;
    }
    return err;
}

/***********************************************************
  Function: R_WM_Sys_StateGet
  
  see <r_wm_sys.h>
*/
r_wm_State_t R_WM_Sys_StateGet(const uint32_t Unit)
{
    r_wm_State_t status;
    
    if (Unit < R_WM_DEV_NUM)
    {
        status = loc_UnitState[Unit];
    }
    else
    {
        /* Unkown unit status is UNINITIALIZED */
        status = R_WM_STATE_UNINITIALIZED;
    }
    return status;
}

/***********************************************************
  Function: R_WM_Sys_DevCountGet
  
  see <r_wm_sys.h>
*/
uint32_t R_WM_Sys_DevCountGet(void)
{
    r_dev_Device_t  device;
    uint32_t        unit_num;
    
    device = R_DEV_GetDev();
    
    /* Check RH850/D1M2(H) & RH850/D1M1A */
    if (((device >= R_DEV_R7F701408) && (device <= R_DEV_R7F701412))
      || (device == R_DEV_R7F701441))
    {
        unit_num = LOC_WM_UNIT_NUM_2;
    }
    else
    {
        unit_num = LOC_WM_UNIT_NUM_1;
    }
    
    return unit_num;
}

/***********************************************************
  Function: R_WM_Sys_DevInfoGet
  
  see <r_wm_sys.h>
*/
void  R_WM_Sys_DevInfoGet(const uint32_t  Unit,
                          uint32_t *const MaxLayers,
                          uint32_t *const MaxPitch, 
                          uint32_t *const MaxWidth, 
                          uint32_t *const MaxHeight,
                          uint32_t *const SpritesOrderAscending)
{
    if (MaxLayers != R_NULL)
    {
        *MaxLayers =  R_WM_MAX_LAYERS;
    }
    else
    {
        /* Do Nothing */
    }

    if (MaxPitch != R_NULL)
    {
       *MaxPitch = R_WM_MAX_LAYER_PITCH;
    }
    else
    {
        /* Do Nothing */
    }

    if (MaxWidth != R_NULL)
    {
       *MaxWidth = R_WM_MAX_LAYER_WIDTH;
    }
    else
    {
        /* Do Nothing */
    }

    if (MaxHeight != R_NULL)
    {
       *MaxHeight = R_WM_MAX_LAYER_HEIGHT;
    }
    else
    {
        /* Do Nothing */
    }

    if (SpritesOrderAscending != R_NULL)
    {
        *SpritesOrderAscending = R_WM_FALSE;
    }
    else
    {
        /* Do Nothing */
    }
}

/***********************************************************
  Function: R_WM_Sys_DevInit
  
  see <r_wm_sys.h>
*/



uint32_t R_WM_Sys_DevInit(const uint32_t Unit,
                          void           (*EventCb)(uint32_t           Unit, 
                                                    const r_wm_Event_t *Event))
{
    uint32_t        i;
    uint32_t        ret_val;
    uint32_t        unit_count;
#if (R_WM_DEV_NUM > 1)
    uint32_t        other_unit;
#endif

    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
#ifdef LOC_USE_OS
        /* create a mutex for access to the event queue*/
        R_OS_ThreadMutexInit(&loc_MutexMsgQueue[Unit], R_NULL);

        /* create a mutex for access to the buffers of a window */
        R_OS_ThreadMutexInit(&loc_MutexBufferMgmnt[Unit], R_NULL);

        /* create a mutex for global WM locking */
        R_OS_ThreadMutexInit(&loc_MutexWindows[Unit], R_NULL);

        for (i = 0; i < R_WM_EVENT_LAST; ++i)
        {
            R_OS_SemInit(&loc_SemaISRTrigger[Unit][i], R_FALSE, LOC_SEM_INIT_VAL);
        }

        /* Init global values */
        loc_WaitVblankInt[Unit]   = 0u;
        loc_WaitScanlineInt[Unit] = 0u;
#else
        /* Init global values */
        loc_WaitVblankInt[Unit]   = R_WM_FALSE;
        loc_WaitScanlineInt[Unit] = R_WM_FALSE;
#endif /* LOC_USE_OS */

        for (i = 0; i <  R_WM_MAX_LAYERS; ++i)
        {
            r_wm_DevSys[Unit].WinToLayerMap[i].Win   = R_NULL;
            r_wm_DevSys[Unit].WinToLayerMap[i].Layer = LOC_INVALID_LAYER;
            r_wm_DevSys[Unit].HwLayerEnabled[i]      = 0u;
            r_wm_DevSys[Unit].CaptPending[i]         = R_WM_FALSE;
            r_wm_DevSys[Unit].SpriteCount[i]        = 0u;
        }
        r_wm_DevSys[Unit].RootWin           = R_NULL;
        r_wm_DevSys[Unit].RootCapt          = R_NULL;
        r_wm_DevSys[Unit].RootVoca          = R_NULL;
        r_wm_DevSys[Unit].RootDiscom        = R_NULL;
        r_wm_DevSys[Unit].EventCb           = EventCb;
        r_wm_DevSys[Unit].RegEvents         = 0u;
        r_wm_DevSys[Unit].FrameStatusFlags  = 0u;
        r_wm_DevSys[Unit].GlobalStatusFlags = 0u;
        r_wm_DevSys[Unit].VocaStatusFlags   = 0u;
        r_wm_DevSys[Unit].ScreenWidth       = 0;
        r_wm_DevSys[Unit].ScreenHeight      = 0;
        r_wm_DevSys[Unit].CaptCount         = 0u;
        
        /* Init another unit if this is 1st call  */
        unit_count = loc_CheckInitlizedUnit();
#if (R_WM_DEV_NUM > 1)
        if (unit_count == LOC_WM_UNIT_NUM_0)
        {
            /* WM somtimes use other unit inoframation.  */
            other_unit = (Unit == R_WM_UNIT0) ? R_WM_UNIT1 : R_WM_UNIT0;
            for (i = 0; i <  R_WM_MAX_LAYERS; ++i)
            {
                r_wm_DevSys[other_unit].WinToLayerMap[i].Win   = R_NULL;
                r_wm_DevSys[other_unit].WinToLayerMap[i].Layer = LOC_INVALID_LAYER;
                r_wm_DevSys[other_unit].HwLayerEnabled[i]      = 0u;
                r_wm_DevSys[other_unit].CaptPending[i]         = R_WM_FALSE;
                r_wm_DevSys[other_unit].SpriteCount[i]         = 0u;
            }
            r_wm_DevSys[other_unit].RootWin           = R_NULL;
            r_wm_DevSys[other_unit].RootCapt          = R_NULL;
            r_wm_DevSys[other_unit].EventCb           = R_NULL;
            r_wm_DevSys[other_unit].RegEvents         = 0u;
            r_wm_DevSys[other_unit].FrameStatusFlags  = 0u;
            r_wm_DevSys[other_unit].GlobalStatusFlags = 0u;
            r_wm_DevSys[other_unit].ScreenWidth       = 0;
            r_wm_DevSys[other_unit].ScreenHeight      = 0;
            r_wm_DevSys[other_unit].CaptCount         = 0u;
            r_wm_DevSys[other_unit].RootVoca          = R_NULL;
            r_wm_DevSys[other_unit].RootDiscom        = R_NULL;
        }
        else
        {
            /* Do nothing */
        }
#endif
        /* Initialize VDCE */
        ret_val = loc_VdceInit(Unit);
        if (ret_val == R_WM_SYS_OK)
        {
            /* Initialize DISCOM */
            ret_val = R_WM_Sys_DiscomInit(Unit);
            if(ret_val == R_WM_SYS_OK)
            {
                if (unit_count == LOC_WM_UNIT_NUM_0)
                {
                    /* Initialize SPEA */
                    ret_val = loc_SpeaInit(Unit);
                    if (ret_val == R_WM_SYS_OK)
                    {
                        /* Init ECM */
                        ret_val = R_WM_Sys_EcmInit();
                    }
                    else
                    {
                        /* Do nothing */
                    }
                    
                    if (ret_val != R_WM_SYS_OK)
                    {
                        /* Error recovery */
                        (void)R_SPEA_DeInit(R_SPEA_UNIT0);
                        (void)R_WM_Sys_EcmDeInit();
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
        
        if (ret_val != R_WM_SYS_OK)
        {
            /* Error recovery */
            (void)R_WM_Sys_DiscomDeInit(Unit);
            (void)R_VDCE_DeInit(Unit);
        }
        else
        {
            /* Do nothing */
        }
    }
    return ret_val;
}

/***********************************************************
  Function: R_WM_Sys_DevDeinit
  
  see <r_wm_sys.h>
*/
uint32_t R_WM_Sys_DevDeinit(const uint32_t  Unit)
{
    uint32_t        ret_val;
    uint32_t        voca_err_val;
    uint32_t        discom_err_val;
    uint32_t        ecm_err_val;
    r_vdce_Error_t  vdce_error_status;
    r_spea_Error_t  spea_error_status;
    uint32_t        unit_count;
#ifdef LOC_USE_OS
    uint32_t        i;
#endif /* LOC_USE_OS */

    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        /* Prevent OS resource access */
        if ((r_wm_DevSys[Unit].GlobalStatusFlags & R_WM_SYS_GLOBAL_FLAG_SHUTDOWN) == 0u)
        {
            /* Preparing shutdown by trying to escape from any wait states */
            r_wm_DevSys[Unit].GlobalStatusFlags |= R_WM_SYS_GLOBAL_FLAG_SHUTDOWN;

#ifdef LOC_USE_OS
            /* Make sure, wm isn't waiting for some events */
            while (loc_WaitVblankInt[Unit] != 0u)
            {
                R_OS_SemPost(&loc_SemaISRTrigger[Unit][R_WM_EVENT_VBLANK]);
                loc_WaitVblankInt[Unit]--;
            }
            while (loc_WaitScanlineInt[Unit] != 0u)
            {
                R_OS_SemPost(&loc_SemaISRTrigger[Unit][R_WM_EVENT_SCANLINE]);
                loc_WaitScanlineInt[Unit]--;
            }
            
            /* Wait for MSG Queue to exit */
            R_OS_ThreadSleep(LOC_SLEEP_TIME);   

            /* delete mutex */
            R_OS_ThreadMutexDestroy(&loc_MutexWindows[Unit]);
            R_OS_ThreadMutexDestroy(&loc_MutexMsgQueue[Unit]);
            R_OS_ThreadMutexDestroy(&loc_MutexBufferMgmnt[Unit]);

            for (i = 0u; i < R_WM_EVENT_LAST; ++i)
            {
                R_OS_SemDestroy(&loc_SemaISRTrigger[Unit][i]);
            }
#else /* LOC_USE_OS */
            loc_WaitVblankInt[Unit]   = R_WM_FALSE;
            loc_WaitScanlineInt[Unit] = R_WM_FALSE;
#endif /* LOC_USE_OS */
        }
        
        /* Deinit SPEA, ECM and VOCA when remain unit is only this unit.  */
        unit_count = loc_CheckInitlizedUnit();
        if (unit_count == LOC_WM_UNIT_NUM_1)
        {
            spea_error_status = R_SPEA_DeInit(R_SPEA_UNIT0);
            
            ecm_err_val  = R_WM_Sys_EcmDeInit();
            
            voca_err_val = R_WM_Sys_DevVocaDeInit(Unit);
        }
        else
        {
            spea_error_status = R_SPEA_ERR_OK;
            voca_err_val      = R_WM_SYS_OK;
            ecm_err_val       = R_WM_SYS_OK;
        }
        
        /* DeInit DISCOM */
        discom_err_val = R_WM_Sys_DiscomDeInit(Unit);
        
        /* Deinit VDCE, even if SPEA deinit is error */
        vdce_error_status = R_VDCE_DeInit(Unit);
        
        /* Check result */
        if ((vdce_error_status != R_VDCE_ERR_OK) 
         || (spea_error_status != R_SPEA_ERR_OK) 
         || (voca_err_val   == R_WM_SYS_NG)
         || (discom_err_val == R_WM_SYS_NG)
         || (ecm_err_val    == R_WM_SYS_NG))
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
  Function: R_WM_Sys_EnableEvent
  
  see <r_wm_sys.h>
*/
uint32_t R_WM_Sys_EnableEvent(const uint32_t Unit, r_wm_EventId_t EventId, const uint32_t Enabled)
{

    static void (*const loc_WM_Isr[R_WM_DEV_NUM][LOC_VDCE_INT_NUM])(void) =
    {
        { &loc_IsrVO0Layer0Vblank, 
          &loc_IsrVO0Scanline, 
          &loc_IsrVI0Vblank, 
          &loc_IsrVI0Overflow, 
          &loc_IsrVO0Layer0underflow, 
          &loc_IsrVO0Layer1underflow, 
          &loc_IsrVO0Layer2underflow,
          &loc_IsrVO0Layer3underflow, 
          &loc_IsrVO0Layer1Vblank,
          &loc_IsrVO0OirVblank,
          &loc_IsrVO0OirScanline }
#if (R_WM_DEV_NUM > 1)
        ,
        { &loc_IsrVO1Layer0Vblank, 
          &loc_IsrVO1Scanline, 
          &loc_IsrVI1Vblank, 
          &loc_IsrVI1Overflow, 
          &loc_IsrVO1Layer0underflow, 
          &loc_IsrVO1Layer1underflow, 
          &loc_IsrVO1Layer2underflow, 
          &loc_IsrVO1Layer3underflow, 
          &loc_IsrVO1Layer1Vblank,
          R_NULL,
          R_NULL }
#endif
    };
    static const r_vdce_IntType_t   loc_VdceEvent[LOC_VDCE_INT_NUM] = {
        R_VDCE_INTC_VBLANK                , /* R_WM_EVENT_VBLANK           */
        R_VDCE_INTC_SCANLINE              , /* R_WM_EVENT_SCANLINE         */
        R_VDCE_INTC_CAP_VBLANK            , /* R_WM_EVENT_VI_VBLANK        */
        R_VDCE_INTC_ERR_CAP_WRITE_OVERFLOW, /* R_WM_EVENT_VI_OVERFLOW      */
        R_VDCE_INTC_ERR_LAYER0_UNDERFLOW  , /* R_WM_EVENT_LAYER0_UNDERFLOW */
        R_VDCE_INTC_ERR_LAYER1_UNDERFLOW  , /* R_WM_EVENT_LAYER1_UNDERFLOW */
        R_VDCE_INTC_ERR_LAYER2_UNDERFLOW  , /* R_WM_EVENT_LAYER2_UNDERFLOW */
        R_VDCE_INTC_ERR_LAYER3_UNDERFLOW  , /* R_WM_EVENT_LAYER3_UNDERFLOW */
        R_VDCE_INTC_VBLANK_1              , /* R_WM_EVENT_LAYER1_VBLANK    */
        R_VDCE_INTC_OIR_VBLANK            , /* R_WM_EVENT_OIR_VBLANK       */
        R_VDCE_INTC_OIR_SCANLINE            /* R_WM_EVENT_OIR_SCANLINE     */
    };
    uint32_t        ret_val = R_WM_SYS_OK;
    r_vdce_Error_t  vdce_err;
    
    /* Check argument */
    if ((uint32_t)EventId >= LOC_VDCE_INT_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        /* Check callback */
        if (loc_WM_Isr[Unit][EventId] == R_NULL)
        {
            /* Do nothing */
        }
        else
        {
            /* Check mandatory interrupt */
            if ((EventId == R_WM_EVENT_VBLANK)
             || (EventId == R_WM_EVENT_SCANLINE)
             || (EventId == R_WM_EVENT_VI_VBLANK)
             || (EventId == R_WM_EVENT_LAYER1_VBLANK)
            /* Check optional interrupt specified by R_WM_Sys_DevEventRegister */
             || ((r_wm_DevSys[Unit].RegEvents & (R_WM_SYS_FLAG_ON << (uint32_t)EventId)) != 0u))
            {
                if (Enabled == R_WM_TRUE)
                {
                    /* Set callback */
                    vdce_err = R_VDCE_IntcCallbackSet(
                                Unit, loc_VdceEvent[EventId], loc_WM_Isr[Unit][EventId]);
                    if (vdce_err != R_VDCE_ERR_OK)
                    {
                        ret_val = R_WM_SYS_NG;
                    }
                    else
                    {
                        vdce_err = R_VDCE_IntcEnable(Unit, loc_VdceEvent[EventId]);
                    }
                }
                else
                {
                    vdce_err = R_VDCE_IntcDisable(Unit, loc_VdceEvent[EventId]);
                }
                if (vdce_err != R_VDCE_ERR_OK)
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
    }
    return ret_val;
}

/***********************************************************
  Function: R_WM_Sys_DevEventRegister
  
  see <r_wm_sys.h>
*/
uint32_t R_WM_Sys_DevEventRegister(const uint32_t        Unit,
                                   const r_wm_EventId_t  EventId,
                                   const uint32_t        Arg)
{
    uint32_t ret_val = R_WM_SYS_OK;
    r_vdce_Error_t vdce_error_status;

    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        /* Set event flag */
        r_wm_DevSys[Unit].RegEvents |= (R_WM_SYS_FLAG_ON << (uint32_t)EventId);
        
        if (EventId == R_WM_EVENT_SCANLINE)
        {
            vdce_error_status = R_VDCE_IntcScanlineSet(Unit, Arg);
            if (vdce_error_status != R_VDCE_ERR_OK)
            {
                ret_val = R_WM_SYS_NG;
            }
            else
            {
                /* Do nothing */
            }
        }
        else if (EventId == R_WM_EVENT_OIR_SCANLINE)
        {
            vdce_error_status = R_VDCE_IntcOirScanlineSet(Unit, Arg);
            if (vdce_error_status != R_VDCE_ERR_OK)
            {
                ret_val = R_WM_SYS_NG;
            }
            else
            {
                ret_val = R_WM_Sys_EnableEvent(Unit, EventId, R_WM_TRUE);
            }
        }
        else if ((EventId == R_WM_EVENT_VI_OVERFLOW) 
              || (EventId == R_WM_EVENT_LAYER0_UNDERFLOW) 
              || (EventId == R_WM_EVENT_LAYER1_UNDERFLOW) 
              || (EventId == R_WM_EVENT_LAYER2_UNDERFLOW) 
              || (EventId == R_WM_EVENT_LAYER3_UNDERFLOW) 
              || (EventId == R_WM_EVENT_OIR_VBLANK)) 
        {
            ret_val = R_WM_Sys_EnableEvent(Unit, EventId, R_WM_TRUE);
        }
        else
        {
            /* nothing */
        }
    }
    return ret_val;
}

/***********************************************************
  Function: R_WM_Sys_IsShutdownActive
  
  see <r_wm_sys.h>
*/
uint32_t R_WM_Sys_IsShutdownActive(const uint32_t Unit)
{
    uint32_t ret_val;

    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_FALSE;
    }
    else
    {
        if ((r_wm_DevSys[Unit].GlobalStatusFlags & R_WM_SYS_GLOBAL_FLAG_SHUTDOWN) != 0u)
        {
            ret_val = R_WM_TRUE;
        }
        else
        {
            ret_val = R_WM_FALSE;
        }
    }
    return ret_val;
}

/***********************************************************
  Group: WM Basic internal frame synchronous control functions
*/

/***********************************************************
  Function: R_WM_Sys_DevFrameStarted
  
  see <r_wm_sys.h>
*/
void R_WM_Sys_DevFrameStarted(const uint32_t Unit)
{
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
    }
    else
    {
        /* reset the sprite unit update flags */
        r_wm_DevSys[Unit].FrameStatusFlags = 0u;
        r_wm_DevSys[Unit].VocaStatusFlags  = 0u;
    }
}

/***********************************************************
  Function: R_WM_Sys_DevFrameFinished
  
  see <r_wm_sys.h>
*/
void R_WM_Sys_DevFrameFinished(const uint32_t Unit)
{
    /* check the sprite unit update flags */
    uint32_t layer;
    uint32_t enable;
    uint32_t sprite_status;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
    }
    else
    {
        /* Lock because GlobalStatusFlags can have asynchronous access.  */
        /* e.g. Accessed from other unit control. */
        R_WM_Sys_LockDevice(Unit);
        
        /* Check if we will have visible windows in the following frame */
        enable = R_WM_FALSE;
        for (layer = 0; layer <  R_WM_MAX_LAYERS; layer++)
        {
            if ((r_wm_DevSys[Unit].HwLayerEnabled[layer] & R_WM_SYS_LAYER_FLAG_EN) != 0u)
            {
                enable = R_WM_TRUE;
                break;
            }
            else
            {
                /* Do Nothing */
            }
        }
        if (enable == R_WM_TRUE)
        {
            r_wm_DevSys[Unit].GlobalStatusFlags |= R_WM_SYS_GLOBAL_FLAG_WIN_EN;
        }
        else
        {
            r_wm_DevSys[Unit].GlobalStatusFlags &= ~R_WM_SYS_GLOBAL_FLAG_WIN_EN;
        }
        
        R_WM_Sys_UnlockDevice(Unit);


        /* Check if we will have pending flag */
        R_WM_Sys_UpdatePendingBuffer(Unit);
        
        /* Rewrite the Sprite configuration for those units */
        /* that got changed during the last frame */
        sprite_status = R_WM_Sys_ReassignAllSprites(Unit);
        if (sprite_status == R_WM_SYS_NG)
        {
            /*
              This API function <R_WM_Sys_DevFrameFinished> cannot return a value by design, 
              so just report and continue 
            */
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NG, R_WM_SYS_NO_INFO);
        }
        else
        {
            /* Do Nothing */
        }
        
        /*
          Set update request bit for those Sprite Unit Registers, 
          that have been written to in the last frame 
        */
        sprite_status = R_WM_Sys_UpdateAllSprites(Unit);
        if (sprite_status == R_WM_SYS_NG)
        {
            /*
              This API function <R_WM_Sys_DevFrameFinished> cannot return a value by design, 
              so just report and continue 
            */
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NG, R_WM_SYS_NO_INFO);
        }
        else
        {
            /* Do Nothing */
        }

        /*
          Process update request for those RLE Unit Registers, 
          if it has been written to in the last frame 
        */
        sprite_status = R_WM_Sys_UpdateAllRles(Unit);
        if (sprite_status == R_WM_SYS_NG)
        {
            /* 
            This API function <R_WM_Sys_DevFrameFinished> cannot return a value by design, 
            so just report and continue 
            */
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NG, R_WM_SYS_NO_INFO);
        }
        else
        {
            /* Do Nothing */
        }
        
        /*
          If no window was visible in this frame, 
          pending SPEA Update Requests cannot be completed immediately.
          => Manually create a SYNC signal. 
             Flickering or artifacts are no problem as nothing is visible 
             on-screen when this part is executed! 
        */
        sprite_status = R_WM_Sys_SynchronizeSprites(Unit);
        if (sprite_status == R_WM_SYS_NG)
        {
            /* 
            This API function <R_WM_Sys_DevFrameFinished> cannot return a value by design, 
            so just report and continue 
            */
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NG, R_WM_SYS_NO_INFO);
        }
        else
        {
            /* Do Nothing */
        }
        
        
        /* Check if a CLUT update is pending for any layer */
        sprite_status = R_WM_Sys_UpdateAllCluts(Unit);
        if (sprite_status == R_WM_SYS_NG)
        {
            /*
            This API function <R_WM_Sys_DevFrameFinished> cannot return a value by design, 
            so just report and continue 
            */
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NG, R_WM_SYS_NO_INFO);
        }
        else
        {
            /* Do Nothing */
        }
        
    }
}

/***********************************************************
  Function: R_WM_Sys_DevWaitForHwWriteReady
  
  see <r_wm_sys.h>
*/
void R_WM_Sys_DevWaitForHwWriteReady(const uint32_t Unit)
{
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
    }
    else
    {
#ifdef LOC_USE_OS
        /* wait for the SCANLINE interrupt */
        loc_WaitScanlineInt[Unit]++;

        /* Don't wait if a shutdown request flag is set */
        if ((r_wm_DevSys[Unit].GlobalStatusFlags & R_WM_SYS_GLOBAL_FLAG_SHUTDOWN) != 0u)
        {
            return;
        }
        else
        {
            /* Do Nothing */
        }
        R_OS_SemWait(&loc_SemaISRTrigger[Unit][R_WM_EVENT_SCANLINE]);
#else /* LOC_USE_OS */
        /* wait for the SCANLINE interrupt */
        loc_WaitScanlineInt[Unit] = R_WM_TRUE;

        while (loc_WaitScanlineInt[Unit] == R_WM_TRUE)
        {
        }
#endif /* LOC_USE_OS */
    }
}

/***********************************************************
  Function: R_WM_Sys_DevWaitForHwUpdated
  
  see <r_wm_sys.h>
*/
void R_WM_Sys_DevWaitForHwUpdated(const uint32_t Unit)
{
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
    }
    else
    {
#ifdef LOC_USE_OS
        /* wait for the VBLANK interrupt */
        loc_WaitVblankInt[Unit]++;

        /* Don't wait if a shutdown request flag is set */
        if ((r_wm_DevSys[Unit].GlobalStatusFlags & R_WM_SYS_GLOBAL_FLAG_SHUTDOWN) != 0u)
        {
            return;
        }
        else
        {
            /* Do Nothing */
        }
        R_OS_SemWait(&loc_SemaISRTrigger[Unit][R_WM_EVENT_VBLANK]);
#else /* LOC_USE_OS */
        /* wait for the VBLANK interrupt */
        loc_WaitVblankInt[Unit] = R_WM_TRUE;

        while (loc_WaitVblankInt[Unit] == R_WM_TRUE)
        {
        }
#endif /* LOC_USE_OS */
    }
}

/***********************************************************
  Group: WM Memory functions
*/


/***********************************************************
  Function: R_WM_Sys_HeapSet
  
  see <r_wm_sys.h>
*/
void R_WM_Sys_Heap_Set(void *const Cpu, 
                       void *const Video)
{
    loc_CpuHeapWM  = (r_cdi_Heap_t*)Cpu;
    loc_VidHeap_WM = (r_cdi_Heap_t*)Video;
}

/***********************************************************
  Function: R_WM_Sys_Alloc
  
  see <r_wm_sys.h>
*/
void *R_WM_Sys_Alloc(const uint32_t      Size,
                     const r_wm_Memory_t MemType)
{
    void    *addr;
    
    switch (MemType)
    {
    case R_WM_MEM_CPU:
        if (loc_CpuHeapWM == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(LOC_UNIT_HEX_FF, R_WM_ERR_MALLOC_FAILED, R_WM_SYS_NO_INFO);
            addr = R_NULL;
        }
        else
        {
            addr = R_CDI_Alloc(loc_CpuHeapWM, Size);
        }
        break;
    case R_WM_MEM_VIDEO:
        if (loc_VidHeap_WM == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(LOC_UNIT_HEX_FF, R_WM_ERR_MALLOC_FAILED, R_WM_SYS_NO_INFO);
            addr = R_NULL;
        }
        else
        {
            addr = R_CDI_Alloc(loc_VidHeap_WM, Size);
        }
        break;
    default: 
        R_WM_SYS_REPORT_ERROR(LOC_UNIT_HEX_FF, R_WM_ERR_MALLOC_FAILED, R_WM_SYS_NO_INFO);
        addr = R_NULL;
        break;
    }
    return addr;
}

/***********************************************************
  Function: R_WM_Sys_Free
  
  see <r_wm_sys.h>
*/
uint32_t R_WM_Sys_Free(const void  *const    Memory,
                       const r_wm_Memory_t   MemType)
{
    uint32_t ret_val;
    uint32_t cdi_err;
    uint32_t mem_addr;
    
    /* Convert type to adjust CDI I/F */
    mem_addr = (uint32_t)Memory;  /* PRQA S 306  */
    
    if (MemType == R_WM_MEM_CPU)
    {
        if (loc_CpuHeapWM == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(LOC_UNIT_HEX_FF, R_WM_ERR_FREE_FAILED, R_WM_SYS_NO_INFO);
            ret_val = R_WM_SYS_NG;
        }
        else
        {
            cdi_err = R_CDI_Free(mem_addr, loc_CpuHeapWM);
            if (cdi_err == LOC_CDI_FREE_OK)
            {
                ret_val = R_WM_SYS_OK;
            }
            else
            {
                ret_val = R_WM_SYS_NG;
            }
        }
    }
    else if (MemType == R_WM_MEM_VIDEO)
    {
        if (loc_VidHeap_WM == R_NULL)
        {
            R_WM_SYS_REPORT_ERROR(LOC_UNIT_HEX_FF, R_WM_ERR_FREE_FAILED, R_WM_SYS_NO_INFO);
            ret_val = R_WM_SYS_NG;
        }
        else
        {
            cdi_err = R_CDI_Free(mem_addr, loc_VidHeap_WM);
            if (cdi_err == LOC_CDI_FREE_OK)
            {
                ret_val = R_WM_SYS_OK;
            }
            else
            {
                ret_val = R_WM_SYS_NG;
            }
        }
    }
    else
    {
        R_WM_SYS_REPORT_ERROR(LOC_UNIT_HEX_FF, R_WM_ERR_FREE_FAILED, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    return ret_val;
}

/***********************************************************
  Group: WM OS interface
*/


/***********************************************************
  Function: R_WM_Sys_LockWindows
  
  see <r_wm_sys.h>
*/
void R_WM_Sys_LockWindows(const uint32_t Unit)
{
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        R_WM_ErrorHandler(Unit, R_WM_ERR_INVALID_WM_UNIT);
    }
    else
    {
#ifdef LOC_USE_OS
        R_OS_ThreadMutexLock(&loc_MutexWindows[Unit]);
#endif
    }
}


/***********************************************************
  Function: R_WM_Sys_UnlockWindows
  
  see <r_wm_sys.h>
*/
void R_WM_Sys_UnlockWindows(const uint32_t Unit)
{
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        R_WM_ErrorHandler(Unit, R_WM_ERR_INVALID_WM_UNIT);
    }
    else
    {
#ifdef LOC_USE_OS
        R_OS_ThreadMutexUnlock(&loc_MutexWindows[Unit]);
#endif
    }
}


/***********************************************************
  Function: R_WM_Sys_LockMsgQueue
  
  see <r_wm_sys.h>
*/
void R_WM_Sys_LockMsgQueue(const uint32_t Unit)
{
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        R_WM_ErrorHandler(Unit, R_WM_ERR_INVALID_WM_UNIT);
    }
    else
    {
#ifdef LOC_USE_OS
        R_OS_ThreadMutexLock(&loc_MutexMsgQueue[Unit]);
#endif
    }
}

/***********************************************************
  Function: R_WM_Sys_UnlockMsgQueue
  
  see <r_wm_sys.h>
*/
void R_WM_Sys_UnlockMsgQueue(const uint32_t Unit)
{
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        R_WM_ErrorHandler(Unit, R_WM_ERR_INVALID_WM_UNIT);
    }
    else
    {
#ifdef LOC_USE_OS
        R_OS_ThreadMutexUnlock(&loc_MutexMsgQueue[Unit]);
#endif
    }
}

/***********************************************************
  Function: R_WM_Sys_LockBuffers
  
  see <r_wm_sys.h>
*/
void R_WM_Sys_LockBuffers(const uint32_t Unit)
{
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        R_WM_ErrorHandler(Unit, R_WM_ERR_INVALID_WM_UNIT);
    }
    else
    {
#ifdef LOC_USE_OS
        R_OS_ThreadMutexLock(&loc_MutexBufferMgmnt[Unit]);
#endif
    }
}


/***********************************************************
  Function: R_WM_Sys_UnlockBuffers
  
  see <r_wm_sys.h>
*/
void R_WM_Sys_UnlockBuffers(const uint32_t Unit)
{
    if (Unit >= (uint32_t)R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        R_WM_ErrorHandler(Unit, R_WM_ERR_INVALID_WM_UNIT);
    }
    else
    {
#ifdef LOC_USE_OS
        R_OS_ThreadMutexUnlock(&loc_MutexBufferMgmnt[Unit]);
#endif
    }
}

/***********************************************************
  Function: R_WM_Sys_LockDevice
  
  see <r_wm_sys.h>
*/
void R_WM_Sys_LockDevice(const uint32_t Unit)
{
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        R_WM_ErrorHandler(Unit, R_WM_ERR_INVALID_WM_UNIT);
    }
    else
    {
#ifdef LOC_USE_OS
        if (loc_MutexInitCreated == R_WM_FALSE)
        {
            R_OS_ThreadMutexInit(&loc_MutexDevice, R_NULL);
            loc_MutexInitCreated = R_WM_TRUE;
        }
        else
        {
            /* Do Nothing */
        }
        R_OS_ThreadMutexLock(&loc_MutexDevice);
#endif
    }
}

/***********************************************************
  Function: R_WM_Sys_UnlockDevice
  
  see <r_wm_sys.h>
*/
void R_WM_Sys_UnlockDevice(const uint32_t Unit)
{
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        R_WM_ErrorHandler(Unit, R_WM_ERR_INVALID_WM_UNIT);
    }
    else
    {
#ifdef LOC_USE_OS
        R_OS_ThreadMutexUnlock(&loc_MutexDevice);
#endif
    }
}

/***********************************************************
  Group: WM Messaging interface
*/

/***********************************************************
  Function: R_WM_Sys_MsgQueueSetup
  
  see <r_wm_sys.h>
*/
uint32_t R_WM_Sys_MsgQueueSetup(const uint32_t Unit, 
                                void *const    MsgQueueStorage, 
                                const uint32_t Size)
{
    uint32_t ret_val;
    int32_t  cdi_result;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        /* Setup message queue */
        cdi_result = R_CDI_RbSetup(
                        &r_wm_DevSys[Unit].EvQueueRingBuf, 
                        MsgQueueStorage, 
                        Size, 
                        sizeof(r_wm_Msg_t));
        if (cdi_result == LOC_CDI_RESULT_OK)
        {
            ret_val = R_WM_SYS_OK;
        }
        else
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NG, R_WM_SYS_NO_INFO);
            ret_val = R_WM_SYS_NG;
        }
    }
    return ret_val;
}

/***********************************************************
  Function: R_WM_Sys_MsgQueueRead
  
  see <r_wm_sys.h>
*/
uint32_t R_WM_Sys_MsgQueueRead(const uint32_t    Unit, 
                               r_wm_Msg_t *const Msg)
{
    uint32_t ret_val;
    uint32_t rbread_num;

    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        rbread_num = R_CDI_RbRead(&r_wm_DevSys[Unit].EvQueueRingBuf, Msg, LOC_CDI_MSG_NUM);
        if (rbread_num != LOC_CDI_MSG_NUM)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NG, R_WM_SYS_NO_INFO);
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
  Function: R_WM_Sys_MsgQueueWrite
  
  see <r_wm_sys.h>
*/
uint32_t R_WM_Sys_MsgQueueWrite(const uint32_t    Unit, 
                                r_wm_Msg_t *const Msg)
{
    uint32_t ret_val;
    uint32_t rbwrite_num;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret_val = R_WM_SYS_NG;
    }
    else
    {
        rbwrite_num = R_CDI_RbWrite(&r_wm_DevSys[Unit].EvQueueRingBuf, Msg, LOC_CDI_MSG_NUM);
        if (rbwrite_num != LOC_CDI_MSG_NUM)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NG, R_WM_SYS_NO_INFO);
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
  Function: R_WM_Sys_GetDevSys
*/
r_wm_DevSys_t *R_WM_Sys_GetDevSys(const uint32_t Unit)
{
    r_wm_DevSys_t *ret_ptr;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);

        /* Fail safe */
        ret_ptr = &r_wm_DevSys[R_WM_UNIT0];
    }
    else
    {
        ret_ptr = &r_wm_DevSys[Unit];
    }
    return ret_ptr;
}

