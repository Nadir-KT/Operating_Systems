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

SAMPLE CODE is not part of the licensed software, as such it must not be used in
mass-production applications. It can only be used for evaluation and
demonstration purposes at customer's premises listed in the signed license
agreement.

****************************************************************************
*/

#ifndef R_SYS_WM_H
#define R_SYS_WM_H

/***********************************************************
  Title: r_sys_wm.h
  
  D1x SYS WM Support Functions

  Declaration of local SYS WM support functions for the D1x device.
*/


/***********************************************************
  Section: Global Defines
*/

/***********************************************************
  Section: SYS WM Global Constants
*/

/***********************************************************
  Constant: R_WM_SYS_FLAG_ON
  
  Flag is on.
*/
#define R_WM_SYS_FLAG_ON                         (1uL)

/***********************************************************
  Constant: R_WM_SYS_GLOBAL_FLAG_*
  
  Definition of valid bits for type r_wm_DevSys_t.GlobalStatusFlags.
*/
/*
 Set if RLE Unit is active. This is used by the opposite WM Unit to correctly configure
 their shared resources. 
*/
#define R_WM_SYS_GLOBAL_FLAG_RLE0                (0x00000001uL)
#define R_WM_SYS_GLOBAL_FLAG_RLE1                (0x00000002uL)
#define R_WM_SYS_GLOBAL_FLAG_RLE2                (0x00000004uL)
#define R_WM_SYS_GLOBAL_FLAG_RLE3                (0x00000008uL)
/* 
  Set if a window was visible during the current frame. If not, special care is taken
  for Sprite engine updates. 
*/
#define R_WM_SYS_GLOBAL_FLAG_WIN_EN              (0x00000010uL)
/*
  Set if capturing is activated.
  This is used to to prevent deletion of active capturing windows 
*/
#define R_WM_SYS_GLOBAL_FLAG_CAPT_EN             (0x00000020uL)
#define R_WM_SYS_GLOBAL_FLAG_CAPT_BOB            (0x00000040uL)

/*
  Set if VOCA is activated.
*/
#define R_WM_SYS_GLOBAL_FLAG_VOCA_INIT           (0x00000080uL)

#define R_WM_SYS_GLOBAL_MASK_VOCA_EN            (0x00FFFF00u)
#define R_WM_SYS_GLOBAL_BIT_VOCA00_EN            (8u)
#define R_WM_SYS_GLOBAL_BIT_VOCA01_EN            (9u)
#define R_WM_SYS_GLOBAL_BIT_VOCA02_EN            (10u)
#define R_WM_SYS_GLOBAL_BIT_VOCA03_EN            (11u)
#define R_WM_SYS_GLOBAL_BIT_VOCA04_EN            (12u)
#define R_WM_SYS_GLOBAL_BIT_VOCA05_EN            (13u)
#define R_WM_SYS_GLOBAL_BIT_VOCA06_EN            (14u)
#define R_WM_SYS_GLOBAL_BIT_VOCA07_EN            (15u)
#define R_WM_SYS_GLOBAL_BIT_VOCA08_EN            (16u)
#define R_WM_SYS_GLOBAL_BIT_VOCA09_EN            (17u)
#define R_WM_SYS_GLOBAL_BIT_VOCA10_EN            (18u)
#define R_WM_SYS_GLOBAL_BIT_VOCA11_EN            (19u)
#define R_WM_SYS_GLOBAL_BIT_VOCA12_EN            (20u)
#define R_WM_SYS_GLOBAL_BIT_VOCA13_EN            (21u)
#define R_WM_SYS_GLOBAL_BIT_VOCA14_EN            (22u)
#define R_WM_SYS_GLOBAL_BIT_VOCA15_EN            (23u)

/* Set to shutdown the WM driver. This triggers early exit of some processing loops. */
#define R_WM_SYS_GLOBAL_FLAG_SHUTDOWN            (0x80000000uL)

/***********************************************************
  Constant: R_WM_SYS_FRAME_MASK_* / R_WM_SYS_FRAME_BIT_*
  
  Definition of valid bits and mask for type r_wm_DevSys_t.FrameStatusFlags.
*/
/* A sprite configuration has changed and needs to be updated */
#define R_WM_SYS_FRAME_MASK_SPEA_UPDATE          (0x00000FFFu)

#define R_WM_SYS_FRAME_BIT_RLE0_UPDATE           (0u)
#define R_WM_SYS_FRAME_BIT_RLE1_UPDATE           (1u)
#define R_WM_SYS_FRAME_BIT_RLE2_UPDATE           (2u)
#define R_WM_SYS_FRAME_BIT_RLE3_UPDATE           (3u)

#define R_WM_SYS_FRAME_BIT_SU0_UPDATE            (4u)
#define R_WM_SYS_FRAME_BIT_SU1_UPDATE            (5u)
#define R_WM_SYS_FRAME_BIT_SU2_UPDATE            (6u)
#define R_WM_SYS_FRAME_BIT_SU3_UPDATE            (7u)

/* The sprite order has changed and needs to be reassigned */
#define R_WM_SYS_FRAME_BIT_SU0_REASSIGN          (8u)
#define R_WM_SYS_FRAME_BIT_SU1_REASSIGN          (9u)
#define R_WM_SYS_FRAME_BIT_SU2_REASSIGN          (10u)
#define R_WM_SYS_FRAME_BIT_SU3_REASSIGN          (11u)

/* The CLUT or the layer order has changed and the CLUT must be updated */
#define R_WM_SYS_FRAME_MASK_CLUT_UPDATE          (0x0000F000u)

#define R_WM_SYS_FRAME_BIT_LAYER0_CLUT           (12u)
#define R_WM_SYS_FRAME_BIT_LAYER1_CLUT           (13u)
#define R_WM_SYS_FRAME_BIT_LAYER2_CLUT           (14u)
#define R_WM_SYS_FRAME_BIT_LAYER3_CLUT           (15u)

/* The discom order has changed  */
#define R_WM_SYS_FRAME_MASK_DISCOM               (0x000F0000u)

#define R_WM_SYS_FRAME_BIT_DISCOM0               (16u)
#define R_WM_SYS_FRAME_BIT_DISCOM1               (17u)
#define R_WM_SYS_FRAME_BIT_DISCOM2               (18u)
#define R_WM_SYS_FRAME_BIT_DISCOM3               (19u)

/***********************************************************
  Constant: R_WM_SYS_VOCA_MASK_* / R_WM_SYS_VOCA_BIT_*
  
  Definition of valid bits and mask for type r_wm_DevSys_t.VocaStatusFlags.
*/
#define R_WM_SYS_VOCA_MASK                      (0xFFFFFFFFu)

#define R_WM_SYS_VOCA_BIT_AREA00_REG             (0u)
#define R_WM_SYS_VOCA_BIT_AREA01_REG             (1u)
#define R_WM_SYS_VOCA_BIT_AREA02_REG             (2u)
#define R_WM_SYS_VOCA_BIT_AREA03_REG             (3u)
#define R_WM_SYS_VOCA_BIT_AREA04_REG             (4u)
#define R_WM_SYS_VOCA_BIT_AREA05_REG             (5u)
#define R_WM_SYS_VOCA_BIT_AREA06_REG             (6u)
#define R_WM_SYS_VOCA_BIT_AREA07_REG             (7u)
#define R_WM_SYS_VOCA_BIT_AREA08_REG             (8u)
#define R_WM_SYS_VOCA_BIT_AREA09_REG             (9u)
#define R_WM_SYS_VOCA_BIT_AREA10_REG             (10u)
#define R_WM_SYS_VOCA_BIT_AREA11_REG             (11u)
#define R_WM_SYS_VOCA_BIT_AREA12_REG             (12u)
#define R_WM_SYS_VOCA_BIT_AREA13_REG             (13u)
#define R_WM_SYS_VOCA_BIT_AREA14_REG             (14u)
#define R_WM_SYS_VOCA_BIT_AREA15_REG             (15u)

#define R_WM_SYS_VOCA_BIT_AREA00_RAM             (16u)
#define R_WM_SYS_VOCA_BIT_AREA01_RAM             (17u)
#define R_WM_SYS_VOCA_BIT_AREA02_RAM             (18u)
#define R_WM_SYS_VOCA_BIT_AREA03_RAM             (19u)
#define R_WM_SYS_VOCA_BIT_AREA04_RAM             (20u)
#define R_WM_SYS_VOCA_BIT_AREA05_RAM             (21u)
#define R_WM_SYS_VOCA_BIT_AREA06_RAM             (22u)
#define R_WM_SYS_VOCA_BIT_AREA07_RAM             (23u)
#define R_WM_SYS_VOCA_BIT_AREA08_RAM             (24u)
#define R_WM_SYS_VOCA_BIT_AREA09_RAM             (25u)
#define R_WM_SYS_VOCA_BIT_AREA10_RAM             (26u)
#define R_WM_SYS_VOCA_BIT_AREA11_RAM             (27u)
#define R_WM_SYS_VOCA_BIT_AREA12_RAM             (28u)
#define R_WM_SYS_VOCA_BIT_AREA13_RAM             (29u)
#define R_WM_SYS_VOCA_BIT_AREA14_RAM             (30u)
#define R_WM_SYS_VOCA_BIT_AREA15_RAM             (31u)

/***********************************************************
  Constant: R_WM_SYS_LAYER_FLAG_*
  
  Definition of valid bits and mask for type r_wm_DevSys_t.HwLayerEnabled
*/
#define R_WM_SYS_LAYER_FLAG_EN                   (0x01u)
#define R_WM_SYS_LAYER_FLAG_CAPT_EN              (0x02u)
#define R_WM_SYS_LAYER_FLAG_LOCK                 (0x04u)

/***********************************************************
  Constant: LOC_INVALID_LAYER
  
  Defines an invalid layer constant.
*/
#define LOC_INVALID_LAYER   (0xFFFFFFFFu)

/***********************************************************
  Constant: LOC_INVALID_SPRITE 
  
  Defines an invalid sprite constant.
*/
#define LOC_INVALID_SPRITE  (R_WM_MAX_SPRITES_PER_LAYER)

/***********************************************************
  Constants: Virtual address
*/

#define R_WM_SYS_VIRTUAL_ADDR_RLE           (0x30000000u)

/***********************************************************
  Constant: R_WM_SYS_VIRTUAL_OFFS_UNIT1 / R_WM_SYS_VIRTUAL_ADDR_SPRITE_x 
  
  Defines the y-offset of the sprites of Unit1 inside 
  the virtual sprite layer.
  
  Both Unit0 and Unit1 share one virtual sprite layer.
  
  SPEA0: Unit0-Layer1 + Unit1-Layer3
  SPEA1: Unit0-Layer2 + Unit1-Layer2
  SPEA2: Unit0-Layer3 + Unit1-Layer1
  SPEA3: Unit0-Layer0 + Unit1-Layer0 (D1M1A, D1M1-V2 only)
  
  The virtual layer has a height of 8192 bytes. By default the
  virtual layer is split in the center. The first 4096 bytes
  belong to Unit0, the other 4096 bytes belong to Unit1.

  For Unit1, both the sprite position in memory and the sprite 
  position on-screen is automatically adapted by the OFFSET,
  
  No adaptions of sprite position are needed for regular API calls!
  
>                                      |<-- 8192 Byte ->|
>  R_WM_SYS_VIRTUAL_ADDR_SPRITE_0 ---->+----------------+ -------------
>                                  A   | Virtual buffer |  A         A
>   R_WM_SYS_VIRTUAL_HEIGHT_UNIT0  |   |   for Unit0    |  | 4096    |
>                                  V   |                |  V  Byte   | 8192
>  R_WM_SYS_VIRTUAL_ADDR_SPRITE_1 ---->+----------------+ ---------  |  Byte
>                                  A   | Virtual buffer |  A         |
>   R_WM_SYS_VIRTUAL_HEIGHT_UNIT1  |   |   for Unit1    |  | 4096    |
>                                  V   |                |  V  Byte   V
>                                      +----------------+ -------------

*/

#define R_WM_SYS_VIRTUAL_HEIGHT_UNIT0   (4096u)
#define R_WM_SYS_VIRTUAL_HEIGHT_UNIT1   (4096u)
#define R_WM_SYS_VIRTUAL_ADDR_SPRITE_0  (0x30000000u)
#define R_WM_SYS_VIRTUAL_ADDR_SPRITE_1  (0x30000000u + (R_WM_SYS_VIRTUAL_HEIGHT_UNIT0 * 8192u))

/***********************************************************
  Macro: LOC_USE_OS

  Flag signaling if the OS is used
*/
#ifdef USE_ROS
#define LOC_USE_OS
#endif

/***********************************************************
  Macro: R_WM_SYS_ERROR_TRACKING 
  
  If active, defines a function to save the last occurred error.
*/
/* #define R_WM_SYS_ERROR_TRACKING */

#ifdef R_WM_SYS_ERROR_TRACKING
#define R_WM_SYS_NO_INFO                        (0u)


#define R_WM_SYS_REPORT_ERROR(Unit,Error,AdditionalInfo)    \
            (loc_WM_Sys_SetLastError((Unit),(Error),(AdditionalInfo), __FILE__, __LINE__))

void loc_WM_Sys_SetLastError(uint32_t            Unit, 
                             r_wm_Error_t        Error,
                             uint32_t            AdditionalInfo, 
                             const int8_t *const File, 
                             uint32_t            Line);
#else /* R_WM_SYS_ERROR_TRACKING */
#define R_WM_SYS_REPORT_ERROR(x,y,z)
#endif /* R_WM_SYS_ERROR_TRACKING */

/***********************************************************
  Section: SYS WM Global Types
*/

/***********************************************************
  Type: r_wm_sys_WinLayerPair_t

  Reflects the layer position of a window or vice versa.
*/
typedef struct
{
    r_wm_Window_t *Win;
    uint32_t      Layer;
} r_wm_sys_WinLayerPair_t;

/***********************************************************
  Type: r_wm_DevSys_t

  Device instance specific data.
  
  All instance specific data is kept within that structure.

  Members:
  EvQueueRingBuf    - Buffer for the messages of the window manager
  WinToLayerMap     - Map for which whindow is assigned to which layer
  HwLayerEnabled    - Which window is currently set to Enabled
  RootWin           - pointer to first (lowest) window
  RootCapt          - pointer to first capture
  RootVoca          - pointer to first VOCA monitor area instance.
  RootDiscom        - pointer to first DISCOM unit instance.
  RegEvents         - Keep track of registered events. Bitfield, 
                      check <r_wm_Event_t> for available bits.
  EventCb           - Callback function for system events
  FrameStatusFlags  - These flags may be set during the processing of a frame in order to toggle 
                      updates after finishing the current message queue.
                      See definition of R_WM_SYS_FRAME_BIT_*. 
  GlobalStatusFlags - Keep Track of different HW Status. See definition of R_WM_SYS_GLOBAL_FLAG_*
  VocaStatusFlags   - Keep Track of VOCA monitor area Status. See definition of R_WM_SYS_VOCA_BIT_*
  ScreenWidth       - Screen width. 
  ScreenHeight      - Screen height. 
  CaptCount         - Vsync count for video input.
  CaptPending       - Capture pending flag. This flag is updated by interrupt, 
                      so independence from other flags.
  SpriteCount       - Sprite count assigned to SPEA driver.
*/
typedef struct r_wm_DevSys_s
{
    r_cdi_RBuf_t                    EvQueueRingBuf;
    r_wm_sys_WinLayerPair_t         WinToLayerMap[R_WM_MAX_LAYERS];
    r_wm_Window_t                   *RootWin;
    r_wm_Capture_t                  *RootCapt;
    r_wm_Voca_t                     *RootVoca;
    r_wm_Discom_t                   *RootDiscom;
    void                            (*EventCb)(uint32_t Unit, const r_wm_Event_t *Event);
    uint8_t                         HwLayerEnabled[R_WM_MAX_LAYERS];
    uint32_t                        RegEvents;
    uint32_t                        FrameStatusFlags;
    uint32_t                        GlobalStatusFlags;
    uint32_t                        VocaStatusFlags;
    int32_t                         ScreenWidth;
    int32_t                         ScreenHeight;
    uint32_t                        CaptCount;
    uint8_t                         CaptPending[R_WM_MAX_LAYERS];
    uint8_t                         SpriteCount[R_WM_MAX_LAYERS];
} r_wm_DevSys_t;


/***************************************************************************
  Group: Global Variables
*/

/***********************************************************
  Section: SYS WM Global Functions
*/

/***********************************************************
  Function: R_WM_Sys_LocGetSpriteUnit
  Description:
  Return the SPEA unit assigned to the given layer 
  as defined by r_spea_Unit_t.

  Parameters:
    Unit -   Specifies the WM Unit Number.
    Layer -  Specifies WM Layer Number.

  Return Value:
    Spea unit 
*/
r_spea_Unit_t R_WM_Sys_GetSpriteUnitFrmLyrNo(const uint32_t Unit, 
                                             const uint32_t Layer);

/***********************************************************
  Function: R_WM_Sys_SetSpriteReassignFlag
  
  Description:
  Register the Sprite Unit defined by r_spea_Unit_t to be rewritten with the next frame.

  Parameters:
    Unit     - Specifies the WM Unit Number.
    SpeaUnit - Specifies the Unit of Sprite Engine.

  Return Value:
  None.
*/
void R_WM_Sys_SetSpriteReassignFlag(const uint32_t      Unit, 
                                    const r_spea_Unit_t SpeaUnit);

/***********************************************************
  Function: R_WM_Sys_ReassignAllSprites    
  
  Description:
  Rewrites all SPEA Registers that need to be updated.

  Parameters:
  Unit - Specifies the WM Unit Number.

  Return Value:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail 
*/
uint32_t R_WM_Sys_ReassignAllSprites(const uint32_t Unit);

/***********************************************************
  Function: R_WM_Sys_UpdateAllSprites   
  
  Description:
  Triggers a HW refresh for all previously written SPEA Sprite registers.

  Parameters:
    Unit - Specifies the WM Unit Number.

  Return Value:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail 
*/
uint32_t R_WM_Sys_UpdateAllSprites(const uint32_t Unit);

/***********************************************************
  Function: R_WM_Sys_UpdateAllRles      
  
  Description:
  Triggers a HW refresh for all previously written SPEA RLE registers.

  Parameters:
    Unit - Specifies the WM Unit Number.

  Return Value:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail 

*/
uint32_t R_WM_Sys_UpdateAllRles(const uint32_t Unit);

/***********************************************************
  Function: R_WM_Sys_SynchronizeSprites
  
  Description:
  If no window was visible in this frame, 
  pending SPEA Update Requests cannot be completed immediately.
   => Manually create a SYNC signal. 
      Flickering or artifacts are no problem as nothing is visible on-screen.

  Parameters:
    Unit - Specifies the WM Unit Number.

  Return Value:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail 
*/
uint32_t R_WM_Sys_SynchronizeSprites(const uint32_t Unit); 

/***********************************************************
  Function: R_WM_Sys_UpdateAllCluts 
  
  Description:
  Rewrites all CLUT RAMs that need to be updated.

  Parameters:
    Unit - Specifies the WM Unit Number.

  Return Value:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail 
*/
uint32_t R_WM_Sys_UpdateAllCluts(const uint32_t Unit);  

/***********************************************************
  Function: R_WM_Sys_GetLayerPairByWin 
  
  Description:
  Return a WinLayerPair based on the given Window.
  Return NULL, if window has no layer.

  Parameters:
    Unit - Specifies the WM Unit Number.
    Win  - Pointer to Window structure. 

  Return Value:
    Pointer to <r_wm_sys_WinLayerPair_t>
*/
r_wm_sys_WinLayerPair_t *R_WM_Sys_GetLayerPairByWin(const uint32_t             Unit, 
                                                    const r_wm_Window_t *const Win);

/***********************************************************
  Function: R_WM_Sys_GetLayerPairByLayer 
  
  Description:
  Return a WinLayerPair based on the given Layer.
  Return NULL, if window has no layer.

  Parameters:
    Unit  - Specifies the WM Unit Number.
    Layer - Specifies WM Layer Number.

  Return Value:
    Pointer to <r_wm_sys_WinLayerPair_t>
*/
r_wm_sys_WinLayerPair_t *R_WM_Sys_GetLayerPairByLayer(const uint32_t Unit, 
                                                      const uint32_t Layer);

/***********************************************************
  Function: R_WM_Sys_ReconfAllWindow 
  
  Description:
  Transfer all changes done to the window configuration down to the hardware.

  Parameters:
    Unit - Specifies the WM Unit Number.

  Return Value:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail 
*/
uint32_t R_WM_Sys_ReconfAllWindow(const uint32_t Unit);

/***********************************************************
  Function: R_WM_Sys_RleInit
  
  Description:
  Initialize the all RLE engines.
  
  Parameters:
  Unit - Specifies the WM Unit Number.

  Return value: 
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail
*/
uint32_t R_WM_Sys_RleInit(const uint32_t Unit); 

/***********************************************************
  Function: R_WM_Sys_RleEnable
  
  Description:
  Enable / Disable the RLE engine and RLE unit.
  
  Parameters:
  Unit   - Specifies the WM unit number.
  Win    - Pointer to Window structure. 
  Layer  - Specifies the layer number. 
  Enable - 0:Disable, 1:Enable. 
  
  Return value: 
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail
*/
uint32_t R_WM_Sys_RleEnable(const uint32_t              Unit,
                            const r_wm_Window_t  *const Win, 
                            const uint32_t              Layer,
                            const uint8_t               Enable);

/***********************************************************
  Function: R_WM_Sys_InitCapabilities
  
  Description:
  Initialize RLE/Sprite window Capabirities
  
  Parameters:
  none
  
  Return value: 
  none
*/
void R_WM_Sys_InitCapabilities(void);

/***********************************************************
  Function: R_WM_Sys_UpdatePendingBuffer

  Description:
  Update pending buffer.

  Parameters:
  Unit - Specifies the WM Unit Number.

  Return Value:
  none
*/
void R_WM_Sys_UpdatePendingBuffer(const uint32_t Unit);


/***********************************************************
  Function: R_WM_Sys_GetDevSys

  Description:
  Get device structure pointer.

  Parameters:
  Unit - Specifies the WM Unit Number.

  Return Value:
  Pointer to <r_wm_DevSys_t>
*/
r_wm_DevSys_t *R_WM_Sys_GetDevSys(const uint32_t Unit);


/***********************************************************
  Function: R_WM_Sys_EnableEvent
  
  Description:
    This function enables / disables VDCE interrupt ans set callback.
  
  Parameters:
  Unit    - Specifies the WM Unit Number.
  EventId - See <r_wm_EventId_t>.
  Enabled - 0:disabled, 1:enabled
  
  Return value: 
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail
*/
uint32_t R_WM_Sys_EnableEvent(const uint32_t Unit, r_wm_EventId_t EventId, const uint32_t Enabled);

/***************************************************************************
  Function: R_WM_Sys_DevVocaInit

  Description:
  This function intializes VOCA driver.
  This function is called from <R_WM_Sys_ScreenEnable>.
  
  Parameter:
  Unit    - Specifies the WM Unit Number.
    
  Return value:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail
*/
uint32_t R_WM_Sys_DevVocaInit(const uint32_t Unit);

/***************************************************************************
  Function: R_WM_Sys_DevVocaDeInit

  Description:
  This function de-intializes VOCA driver.
  This function is called from <R_WM_Sys_DevDeinit>.

  Parameter:
  Unit    - Specifies the WM Unit Number.
    
  Return value:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail
*/
uint32_t R_WM_Sys_DevVocaDeInit(const uint32_t Unit);

/***************************************************************************
  Function: R_WM_Sys_IsrVcoa

  Description:
  This function is ISR subroutine to handle VOCA error interrupt.
  This function is called from <R_WM_Sys_IsrVocaErr>.

  Parameter:
  None
    
  Return value:
  None
*/
void R_WM_Sys_IsrVcoa(void);

/***************************************************************************
  Function: R_WM_Sys_DiscomInit

  Description:
  This function intializes DISCOM driver.
  This function is called from <R_WM_Sys_DevInit>.
  
  Parameter:
  Unit    - Specifies the WM Unit Number.
    
  Return value:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail
*/
uint32_t R_WM_Sys_DiscomInit(const uint32_t Unit);

/***************************************************************************
  Function: R_WM_Sys_DiscomDeInit

  Description:
  This function de-intializes DISCOM driver.
  This function is called from <R_WM_Sys_DevDeinit>.

  Parameter:
  Unit    - Specifies the WM Unit Number.
    
  Return value:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail
*/
uint32_t R_WM_Sys_DiscomDeInit(const uint32_t Unit);

/***************************************************************************
  Function: R_WM_Sys_IsrDiscom

  Description:
  This function is ISR subroutine to handle DISCOM error interrupt.
  This function is called from <R_WM_Sys_IsrVocaErr>.

  Parameter:
  None
    
  Return value:
  None
*/
void R_WM_Sys_IsrDiscom(void);

/***************************************************************************
  Function: R_WM_Sys_EcmInit

  Description:
  This function initializes ECM driver and enables the ECM interrupt.
  This function is called from <R_WM_Sys_DevInit>.

  Parameter:
  None
    
  Return value:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail
*/
uint32_t R_WM_Sys_EcmInit(void);

/***************************************************************************
  Function: R_WM_Sys_EcmDeInit

  Description:
  This function disables the ECM interrupt and deinitializes the driver.
  This function is called from <R_WM_Sys_DevDeInit>.

  Parameter:
  None
    
  Return value:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail
*/
uint32_t R_WM_Sys_EcmDeInit(void);


#endif /* R_SYS_WM_H */

