/*
****************************************************************************
PROJECT : WM driver

============================================================================ 
DESCRIPTION
Window Manager support functions
============================================================================
                            C O P Y R I G H T
============================================================================
                       Copyright (c) 2013 - 2020
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

/***************************************************************************
  Title: WM support functions 
  
  WM (Window Manager) driver support functions

  The generic WM driver uses these functions. They have to be implemented 
  within the driver library for a concrete device. (e.g. D1L, D1M)

  Support functions are functions that are not part of the driver itself 
  but they must be provided to integrate the driver on a particular board.

*/

#ifndef R_WM_SYS_H
#define R_WM_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
  Section: Global Defines
*/

/***********************************************************
  Constant: R_WM_SYS_OK / R_WM_SYS_NG
  
  Definition of return code of SYS WM.
  
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Error
*/
#define R_WM_SYS_OK    (1u)
#define R_WM_SYS_NG    (0u)

/***************************************************************************
  Type: r_wm_sys_DevFeature_t

  Device specific features.
  
  Values:
    R_WM_SYS_FEATURE_RLE_LAYER_NO           - Get number of layers that support RLE decoding
    R_WM_SYS_FEATURE_SPRITE_LAYER_NO        - Get number of layers that support Sprite handling
    R_WM_SYS_FEATURE_SWITCH_CAPABILITIES    - Video Output is capable to switch the layers between 
                                              RLE and Sprite mode
    R_WM_SYS_FEATURE_GAMMA_CORRECTION       - Video Output has gamma correction feature
    R_WM_SYS_FEATURE_SCALING                - Video Output has scaling feature
    R_WM_SYS_FEATURE_LAST                   - Delimiter, must be last element

*/
typedef enum
{
    R_WM_SYS_FEATURE_RLE_LAYER_NO,
    R_WM_SYS_FEATURE_SPRITE_LAYER_NO,
    R_WM_SYS_FEATURE_SWITCH_CAPABILITIES,
    R_WM_SYS_FEATURE_GAMMA_CORRECTION,
    R_WM_SYS_FEATURE_SCALING,
    R_WM_SYS_FEATURE_LAST
} r_wm_sys_DevFeature_t;

/***********************************************************
  Enum: r_wm_State_t

  Description:
  The window manager can be in different states which may only 
  support a subset of all available API functions.
  This type helps to keep track of the current state of the WM.

  Members:
    R_WM_STATE_UNINITIALISED        - The driver is not yet initialized
    R_WM_STATE_INITIALISED          - The window is initialized by <R_WM_DevInit>
    R_WM_STATE_DISPLAY_INITIALIZED  - The display is initialized and can be turned on
    R_WM_STATE_DISPLAY_ACTIVE       - The display is activated

*/

typedef enum
{
    /*
    A zero-initialized array will automatically have this value. 
    Useful for variable <r_wm_Dev.State> 
    */
    R_WM_STATE_UNINITIALIZED = 0,
    R_WM_STATE_INITIALIZED,
    R_WM_STATE_DISPLAY_INITIALIZED,
    R_WM_STATE_DISPLAY_ACTIVE
} r_wm_State_t;

/***********************************************************
  Enum: r_wm_Memory_t

  Description:
  The window manager requires different access types to the memory. 
  Therefore the function <R_WM_Sys_Alloc> is called with a parameter, 
  which indicates the memory type requested by the window manager.

  Members:
    R_WM_MEM_CPU   - The memory needs to be accessible by the CPU.
    R_WM_MEM_VIDEO - It has to be possible to access the memory by the VOUT, VIN, GPU and CPU.
*/

typedef enum
{
    R_WM_MEM_CPU = 0,
    R_WM_MEM_VIDEO
} r_wm_Memory_t;

/***********************************************************
  Enum: r_wm_ScaleChg_t

  Description:
    The type is used to specify scaled size change behavior.
    See also functions <R_WM_Sys_WindowScaleSet>.

  Members:
    R_WM_SCALE_CHANGE_NONE     - No change. Set from instance value. (Internal use)
    R_WM_SCALE_CHANGE_SMALL    - Change scale-down size.
    R_WM_SCALE_CHANGE_LARGE    - Change scale-up size.
    R_WM_SCALE_CHANGE_WIN_SIZE - Change window size. (Internal use)
*/

typedef enum
{
    R_WM_SCALE_CHANGE_NONE  = 0,
    R_WM_SCALE_CHANGE_SMALL,
    R_WM_SCALE_CHANGE_LARGE,
    R_WM_SCALE_CHANGE_WIN_SIZE
} r_wm_ScaleChg_t;

/***************************************************************************
  Group: WM Basic interface functions
*/

/***************************************************************************
  Function: R_WM_Sys_StateSet
  
  Description:
    This function is used to change the state of WM driver.
    
    This function executes the following processing.  
      * This function change the current state of WM driver to requested state. 

  Parameter:
    Unit    - Specifies the WM Unit number.
    State   - Specifies the state of WM driver.

  Returns:
    R_WM_ERR_OK                - No error has occurred.
    R_WM_ERR_NG                - An error has occurred when function called from 
                                 an incorrect state.
*/
r_wm_Error_t R_WM_Sys_StateSet(const uint32_t     Unit,
                               const r_wm_State_t State);


/***************************************************************************
  Function: R_WM_Sys_StateGet
  
  Description:
    This function is used to get the state of WM driver. 
    This function is called from several APIs.
  
    This function executes the following processing.  
      * This function will return the current status of WM unit.
      * If unsupported WM unit number is specified, this function returns 
        R_WM_STATE_UNINITIALISED. 

  Parameter:
    Unit    - Specifies the WM Unit number.

  Returns:
    R_WM_STATE_UNINITIALISED       - WM Unit is Uninitialized state.
    R_WM_STATE_INITIALISED         - WM Unit is initialized.
    R_WM_STATE_DISPLAY_INITIALIZED - WM Unit is Display Initialized state.
    R_WM_STATE_DISPLAY_ACTIVE      - WM Unit is Display Active state.
*/
r_wm_State_t R_WM_Sys_StateGet(const uint32_t Unit);


/***************************************************************************
  Function: R_WM_Sys_InitGlobal
  
  Description:
    Initializes the global variables in WM porting layer.
    
    If R_BSP_SYS_INIT_USE is defined, user must call this function before calling WM APIs. 
    This function is called from R_DEV_SysInit provided as sample code. 
    
    If R_BSP_SYS_INIT_USE is not defined, global variables are declared with initial values. 
    This function call is not mandatory.
    
    This function executes the following processing.  
      * Initilize global variables.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    
  Parameter:
    None.
    
  Return value:
    None.
*/
void R_WM_Sys_InitGlobal(void);

/***************************************************************************
  Function: R_WM_Sys_DevInit
  
  Description:
    Intitializes the platform specific portion of the device.
    This function is called from <R_WM_DevInit>.

    This function executes the following processing.  
      * Initializes some global variables of specifies WM unit.
      * Registers the Event Callback to notify the VDCE, DISCOM and VOCA event.
      * Initializes VDCE by executing the function <R_VDCE_Init>.
      * Sets the callback function to detect VDCE errors by <R_VDCE_ErrorCallbackSet> 
      * Disables the display output by executing the function <R_VDCE_DisplayDisable>.
      * If another WM unit is uninitialized state (or does not exist):
        - - Initializes some global variables of another WM unit.
        - - Initializes SPEA by executing the function <R_SPEA_Init>.
        - - Sets the callback function to detect SPEA errors by <R_SPEA_SetErrorCallback>.
        - - Disables all the RLE engines and RLE units by executing the function 
            <R_SPEA_SetRle> and <R_SPEA_UnitEnable>.
        - - Initializes ECM sample driver 
        - - <R_WM_Sys_IsrVocaErr> is installed as ISR of INTVOCAERR interrupt.
        - - Enable INTVOCAERR interrupt.
      * Initializes DISCOM by executing the function <R_DISCOM_Init>.
      * Sets the callback function to detect DISCOM errors by <R_DISCOM_ErrorCallbackSet>.

  Customizing Points:
    If a OS is used, please change the OS synchronisation variables accordingly.
    Change the implementation of ECM sample driver control when the user implements 
    original ECM control.

  Parameter:
    Unit    - WM unit number.
    EventCb - Callback function for receiving events.
    
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_DevInit(const uint32_t Unit,
                          void           (*EventCb)(uint32_t           Unit, 
                                                    const r_wm_Event_t *Event));

/***************************************************************************
  Function: R_WM_Sys_DevDeinit
  
  Description:
    Deinitializes the SYS portion of the device.
    This function is called from <R_WM_DevDeinit>.

    This function executes the following processing.  
      * Prepare shutdown flag for trying to escape from any wait states.
      * Initializes some global variables.
      * If another WM unit is uninitialized state (or does not exist):
        - - De-initializes SPEA by executing the function <R_SPEA_DeInit>.
        - - Disables INTVOCAERR interrupt of ECM. 
        - - De-initializes ECM sample driver. 
        - - Disables VOCA interrupt by executing <R_VOCA_IntDisable>. 
        - - De-initializes VOCA driver by executing <R_VOCA_DeInit>. 
        - - Clears global flags that shows initialized VOCA driver.
      * Disables DISCOM interrupt by executing <R_DISCOM_IntDisable>. 
      * De-initializes DISCOM by executing the function <R_DISCOM_DeInit>.
      * De-initializes VDCE by executing the function <R_VDCE_DeInit>.

  Customizing Points:
    If an OS is used, please change the OS synchronization variables accordingly.
    Change the implementation of ECM sample driver control when the user implements 
    original ECM control.
    
  Parameter:
    Unit    - WM unit number.
    
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_DevDeinit(const uint32_t Unit);  

/***************************************************************************
  Function: R_WM_Sys_DevEventRegister
  
  Description:
    This function enables the VDCE, VOCA and DISCOM event callback. 
    This function is called from <R_WM_DevEventRegister>.
    
    VDCE Callback is also controlled by following functions. 
      * R_WM_Sys_ScreenEnable 
         - - R_WM_EVENT_VBLANK 
         - - R_WM_EVENT_SCANLINE 
         - - R_WM_EVENT_LAYER1_VBLANK
      * R_WM_Sys_CaptureEnable
         - - R_WM_EVENT_VI_VBLANK 

    Regarding R_WM_EVENT_VI_VBLANK and R_WM_EVENT_VI_OVERFLOW, these events occurs 
    when the capture is enabled. Specify the unit number of the capture side to Unit.

    This function executes the following processing.  
      * Sets the event trigger flag.
      * Executes the following processing depending on EventId.
     
     [R_WM_EVENT_SCANLINE]
      * Sets the scan line by executing <R_VDCE_IntcScanlineSet>.
     
     [R_WM_EVENT_OIR_SCANLINE]
      * Sets the scan line by executing <R_VDCE_IntcOirScanlineSet>.
      * Sets the internal callback function by executing <R_VDCE_IntcCallbackSet>.
      * Enables the interrupt by executing <R_VDCE_IntcEnable>.
     
     [R_WM_EVENT_VI_OVERFLOW]
     [R_WM_EVENT_LAYER0_UNDERFLOW]
     [R_WM_EVENT_LAYER1_UNDERFLOW]
     [R_WM_EVENT_LAYER2_UNDERFLOW]
     [R_WM_EVENT_LAYER3_UNDERFLOW]
     [R_WM_EVENT_OIR_VBLANK]
      * Sets the internal callback function by executing <R_VDCE_IntcCallbackSet>.
      * Enables the interrupt by executing <R_VDCE_IntcEnable>.
      
  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    
  Parameter:
    Unit        - WM unit number.
    EventId     - The ID of the event
    Arg         - Specifies the scan line number on which the event will trigger. 
                  This argument is valid when EventId is R_WM_EVENT_SCANLINE or 
                  R_WM_EVENT_OIR_SCANLINE.

    
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_DevEventRegister(const uint32_t        Unit,  
                                   const r_wm_EventId_t  EventId,
                                   const uint32_t        Arg);

/***************************************************************************
  Function: R_WM_Sys_IsShutdownActive
  
  Description:
    This function checks the shutdown status.
    Shutdown status is from the start of <R_WM_Sys_DevDeinit> process to next <R_WM_Sys_DevInit>.
    This function is called from <R_WM_FrameWait> in order to early-exit the job handling.

    This function executes the following processing.  
      * In the shutdown status, returns 1. Otherwise returns 0.
     
  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    
  Parameter:
    Unit    - WM unit number.
    
  Return value:
    1 if shutdown active, otherwise 0.
*/
uint32_t R_WM_Sys_IsShutdownActive(const uint32_t Unit);

/***************************************************************************
  Function: R_WM_Sys_DevCountGet
  
  Description:
    Gets the number of available WM units.
    This function is called from several WM APIs.

    This function executes the following processing.  
      * This function returns the available WM (VDCE) unit number depending on the device.

  Customizing Points:
    If user uses only WM Unit0, user can change the R_WM_DEV_NUM definition from 2 to 1. 
    It can be reduced the RAM size like a global variable defined in Porting layer. 
    In that case the user needs to guarantee not to specify Unit 1 for the WM API.
    
  Parameter:
    None.
    
  Return value:
    Number of WM units.
*/
uint32_t R_WM_Sys_DevCountGet(void);

/***************************************************************************
  Function: R_WM_Sys_DevInfoGet
  
  Description:
    This function gets the platform specific information. 
    This function is called from <R_WM_DevInit>.

    This function executes the following processing.  
      * Stores the default values to the argument.
      
  Customizing Points:
    The value may be changed according to device specification and user usage restriction.
    MaxPitch can be extended to VDCE maximum size (262136 / bpp).
    
  Parameter:
    Unit                  - WM unit number.
    MaxLayers             - Maximum number of layers supported
    MaxPitch              - Maximum layer pitch (stride) supported 
    MaxWidth              - Maximum layer width supported
    MaxHeight             - Maximum layer height supported
    SpritesOrderAscending - Sprites Z-order ascending or descending.
                            Ascending means that the sprite with the greater index
                            has the greater Z-order priority (comes on top of the sprites
                            with the lower index).
    
  Return value:
    None.
*/
void  R_WM_Sys_DevInfoGet(const uint32_t     Unit,
                          uint32_t *const    MaxLayers,
                          uint32_t *const    MaxPitch,
                          uint32_t *const    MaxWidth,
                          uint32_t *const    MaxHeight,
                          uint32_t *const    SpritesOrderAscending);

/***************************************************************************
  Function: R_WM_Sys_DeviceFeature
  
  Description:
    Gets device specific information about certain features.
    This function is called from <R_WM_WindowCapabilitiesSet> and <R_WM_Sys_DevInit>.
    Note that this function may be called before the <R_WM_Sys_DevDeinit>.
    
    This function executes the following processing.  
      * Return the values depending on target RH850/D1x device.
    
  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    
  Parameter:
    Feature  - Feature support to be checked
    
  Return value:
    Zero if not available or not supported.    
    Greater zero if supported. Value will number of supported instances if applicable.
*/
uint32_t R_WM_Sys_DeviceFeature(const r_wm_sys_DevFeature_t Feature);

/***********************************************************
  Function: R_WM_Sys_IsrVocaErr

  Description:
    This function is executed in response to the INTVOCAERR interrupt.
    This function is called from WM porting layer.

    This function executes the following processing depending on device.
    
    [RH850/D1Lx]
      * Does nothing.
    
    [RH850/D1Mx]
      * Disables INTVOCAERR interrupt of ECM.
      * Checks the initialized VOCA unit.
      * Gets interrupt factor by executing <R_VOCA_StatusGet>.
      * Clears interrupt factor by executing <R_VOCA_StatusClear>.
      * Notifies R_WM_EVENT_VOCA_MISMACTH event to user if applicable.
      * Notifies R_WM_EVENT_ACT_MON_ERROR event to user if applicable.
      * Checks the initialized DISCOM unit.
      * Gets interrupt factor by executing <R_DISCOM_StatusGet>.
      * Clears interrupt factor by executing <R_DISCOM_StatusClear>.
      * Notifies R_WM_EVENT_DISCOM_MISMACTH event to user if applicable.
      * Enables INTVOCAERR interrupt of ECM.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.

  Return value:
  None.

*/
void R_WM_Sys_IsrVocaErr(void);

/***************************************************************************
  Group: WM Basic internal frame synchronous control functions
*/

/***************************************************************************
  Function: R_WM_Sys_DevFrameStarted
  
  Description:
    Will be called immediately before the HW update for one frame
    starts taking place.
    This function is called from <R_WM_FrameExecuteNext>.

    This function executes the following processing.
      * Resets the all frame status flags to collect new status.
      * Resets the all VOCA status flags to collect new status.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
  
  Return value:
    None.
*/
void R_WM_Sys_DevFrameStarted(const uint32_t Unit);  

/***************************************************************************
  Function: R_WM_Sys_DevFrameFinished
  
  Description:
    This function handles the end of processing a sequence of message queues.
    This function is called from <R_WM_FrameExecuteNext>.

    This function executes the following processing.  
      * Update frame buffer if update is pending. 
        The pending occurs when the layer is locked in R_WM_EVENT_VI_VBLANK timing during capture.
      * Reassign all Sprite data of the sprite unit that got changed during this frame.
        - - Set configuration by <R_SPEA_SetSprite>.
        - - Enable / Disable the sprite data by <R_SPEA_SpriteEnable>. 
      * Set update request for changing Sprite unit with function <R_SPEA_UpdateUnit>.
      * Set update request for changing RLE unit with function <R_SPEA_UpdateUnit>.
      * If no window was visible in this frame, creates a SYNC signal manually 
        by toggling UPDTn bit in VDCECTL register (Refer HW UM 37.6).  
      * Updates CLUT by executing <R_VDCE_LayerClutSet> if the layer got changed during this frame.
      * Updates global status flags that holds window visible state.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
  
  Return value:
    None.
*/
void R_WM_Sys_DevFrameFinished(const uint32_t Unit);  

/***************************************************************************
  Function: R_WM_Sys_DevWaitForHwWriteReady
    
  Description:
    Waits until it is safe (in terms of time to finish) to start writing the hardware registers. 
    This function is called from <R_WM_FrameWait>.

    This function executes the following processing.  
      * Wait until Scan line interrupt triggers.

  Customizing Points:
    If a OS is used, please change the OS synchronisation variables accordingly.
    
  Parameter:
    Unit    - WM unit number.
  
  Return value:
    None.
*/
void R_WM_Sys_DevWaitForHwWriteReady(const uint32_t Unit);  

/***************************************************************************
  Function: R_WM_Sys_DevWaitForHwUpdated
  
  Description:
    Wait until the HW registers update is finished.
    This function is called from <R_WM_FrameWait>.
    
    This function executes the following processing.
      * Wait until VBLANK interrupt triggers.
  
  Customizing Points:
    If an OS is used, please change the OS synchronisation variables accordingly.
  
  Parameter:
    Unit    - WM unit number.
  
  Return value:
    None.
*/
void R_WM_Sys_DevWaitForHwUpdated(const uint32_t Unit);  


/***************************************************************************
  Function: R_WM_Sys_DevRootWindowSet
  
  Description:
    This function sets the root window for the device window linked list. 
    This function is called from message queue of following functions.
      * <R_WM_WindowCreate>
      * <R_WM_WindowDelete>
      * <R_WM_WindowMove>

    This function executes the following processing.
      * Set the root window in the window linked list.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    
  Parameter:
    Unit    - WM unit number.
    RootWin - Root window
    
  Return value:
    None.
*/
void R_WM_Sys_DevRootWindowSet(const uint32_t       Unit, 
                               r_wm_Window_t *const RootWin);

/***************************************************************************
  Function: R_WM_Sys_DevRootCaptureSet
  
  Description:
    This function sets the root capture surface for the capture surface linked list.
    This function is also called from message queue of following functions.
      * <R_WM_CaptureCreate> 
      * <R_WM_CaptureDelete>

    This function executes the following processing.
      * Set the root capture surface in the capture surface linked list.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.

  Parameter:
    CapUnit     - Capture unit
    RootCapt    - Capture root
    
  Return value:
    None.
*/
void R_WM_Sys_DevRootCaptureSet(const uint32_t        CapUnit, 
                                r_wm_Capture_t *const RootCapt);

/***************************************************************************
  Function: R_WM_Sys_DevRootVocaSet
  
  Description:
    This function sets the root VOCA monitor area for the linked list.
    This function is called from message queue of following functions.
      * <R_WM_ScreenVocaCreate>
      * <R_WM_ScreenVocaDelete>
      * <R_WM_ScreenVocaDeInit>

    This function executes the following processing depending on device.
    
    [RH850/D1Lx]
      * Does nothing.
    
    [RH850/D1Mx]
      * Sets the root VOCA monitor area in the linked list.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.

  Parameter:
    Unit       - WM unit number.
    RootVoca   - Voca monitor area root
    
  Return value:
    None.
*/
void R_WM_Sys_DevRootVocaSet(const uint32_t        Unit, 
                             r_wm_Voca_t    *const RootVoca);

/***************************************************************************
  Function: R_WM_Sys_DevRootDiscomSet
  
  Description:
    This function sets the root DISCOM device for the linked list. 
    This function is called from message queue of following functions.
      * <R_WM_DiscomCreate>
      * <R_WM_DiscomDelete>
    
    This function executes the following processing depending on device.
    
    [RH850/D1Lx]
      * Does nothing.
    
    [RH850/D1Mx]
      * Sets the root DISCOM device in the linked list.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.

  Parameter:
    Unit       - WM unit number.
    RootDiscom - Discom root
    
  Return value:
    None.
*/
void R_WM_Sys_DevRootDiscomSet(const uint32_t        Unit, 
                               r_wm_Discom_t  *const RootDiscom);

/***************************************************************************
  Group: WM Video Output Screen interface functions
*/

/***************************************************************************
  Function: R_WM_Sys_ScreenTimingSet
  
  Description:
    This function sets the screen timings manually. 
    This function enables VDCE interrupts and sets default scan line timing.
    This function is called from <R_WM_ScreenTimingSet> and <R_WM_ScreenTimingSetByName>.

    This function executes the following processing.
      * Sets the display timing, including display resolution, sync position, blank widths 
        and pixel clocks by executing <R_VDCE_DisplayTimingSet>.
      * Sets the scan line by executing the function <R_VDCE_IntcScanlineSet>.
         - - Scan line is (timing->ScreenHeight - 50) in case of (timing->ScreenHeight > 240)
         - - Scan line is (timing->ScreenHeight - (timing->ScreenHeight / 10)) in case of 
             (timing->ScreenHeight <= 240)
      * Resets the global variable to keep screen information.

  Customizing Points:
    The default value of Scan Line can be customized.
  
  Parameter:
    Unit    - WM unit number.
    Timing  - Screen Timing. See <r_ddb_Timing_t>.
    
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_ScreenTimingSet(const uint32_t              Unit, 
                                  const r_ddb_Timing_t *const Timing);  

/***************************************************************************
  Function: R_WM_Sys_ScreenTimingSetByName
  
  Description:
    Sets the video timings by referencing the display database.
    This function is called from <R_WM_ScreenTimingSetByName>.

    This function executes the following processing.
      * Get display timing parameter from <R_DDB_GetDisplayTiming>.
      * Executes <R_WM_Sys_ScreenTimingSet>.
      
  Customizing Points:
    Display database is described in vlib/macro/vo/ddb/src/r_ddb_timing.c.
    User can add the display database to r_ddb_timing.c.
  
  Parameter:
    Unit                - WM unit number.
    Name                - Name of the display database. 

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_ScreenTimingSetByName(const uint32_t         Unit,
                                        const int8_t    *const Name);

/***************************************************************************
  Function: R_WM_Sys_ScreenEnable
  
  Description:
    This function enables or disables the screen. 
    This function is called from <R_WM_DevDeinit>.
    This function is also called from message queue of following functions.
      * <R_WM_ScreenEnable>
      * <R_WM_ScreenDisable>

    This function executes the following processing.
    
    For Enable
      * Sets the callback for the following interrupt type by executing <R_VDCE_IntcCallbackSet>.
         - - R_VDCE_INTC_SCANLINE 
         - - R_VDCE_INTC_VBLANK
         - - R_VDCE_INTC_VBLANK_1
      * Enables the following interrupts by executing <R_VDCE_IntcEnable>.
         - - R_VDCE_INTC_SCANLINE 
         - - R_VDCE_INTC_VBLANK
         - - R_VDCE_INTC_VBLANK_1
      * Sets the callback for the following interrupt type by executing <R_VDCE_IntcCallbackSet>
        when the event is set by <R_WM_Sys_DevEventRegister>.
         - - R_VDCE_INTC_ERR_LAYER0_UNDERFLOW
         - - R_VDCE_INTC_ERR_LAYER1_UNDERFLOW
         - - R_VDCE_INTC_ERR_LAYER2_UNDERFLOW
         - - R_VDCE_INTC_ERR_LAYER3_UNDERFLOW
         - - R_VDCE_INTC_OIR_VBLANK
         - - R_VDCE_INTC_OIR_SCANLINE
      * Enables the following interrupts by executing <R_VDCE_IntcEnable>
        when the event is set by <R_WM_Sys_DevEventRegister>.
         - - R_VDCE_INTC_ERR_LAYER0_UNDERFLOW
         - - R_VDCE_INTC_ERR_LAYER1_UNDERFLOW
         - - R_VDCE_INTC_ERR_LAYER2_UNDERFLOW
         - - R_VDCE_INTC_ERR_LAYER3_UNDERFLOW
         - - R_VDCE_INTC_OIR_VBLANK
         - - R_VDCE_INTC_OIR_SCANLINE
      * Enables the display output by executing <R_VDCE_DisplayEnable>.
      * When Unit is 0, checks global flags and if VOCA is not initialized, executes following 
        processing.
         - - Initializes the VOCA driver by executing <R_VOCA_Init>. 
         - - Sets global flags that shows initialized VOCA driver.
         - - Sets the callback function to detect VOCA errors by executing 
             <R_VOCA_ErrorCallbackSet>.
         - - Enables VOCA interrupt by executing <R_VOCA_IntEnable>.

    For Disable
      * Disables the following interrupts by executing <R_VDCE_IntcDisable>.
         - - R_VDCE_INTC_SCANLINE 
         - - R_VDCE_INTC_VBLANK
         - - R_VDCE_INTC_CAP_VBLANK
         - - R_VDCE_INTC_ERR_CAP_WRITE_OVERFLOW
         - - R_VDCE_INTC_ERR_LAYER0_UNDERFLOW
         - - R_VDCE_INTC_ERR_LAYER1_UNDERFLOW
         - - R_VDCE_INTC_ERR_LAYER2_UNDERFLOW
         - - R_VDCE_INTC_ERR_LAYER3_UNDERFLOW
         - - R_VDCE_INTC_VBLANK_1
         - - R_VDCE_INTC_OIR_VBLANK
         - - R_VDCE_INTC_OIR_SCANLINE
      * Disables the display output by executing <R_VDCE_DisplayDisable>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit                - WM unit number.
    Enabled             - 0 : disabled, 1 : enabled
  
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_ScreenEnable(const uint32_t Unit, 
                               const uint32_t Enabled);

/***************************************************************************
  Function: R_WM_Sys_ScreenBgColorSet
  
  Description:
    This function sets the background color of the display. 
    This function is called from message queue of <R_WM_ScreenBgColorSet>.

    This function executes the following processing.
      * Sets the display background color by executing <R_VDCE_DisplayColorSet>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    
  Parameter:
    Unit                - WM unit number.
    Red, Green, Blue    - The components of the background color
  
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_ScreenBgColorSet(const uint32_t Unit,
                                   const uint8_t  Red,
                                   const uint8_t  Green,
                                   const uint8_t  Blue);

/***************************************************************************
  Function: R_WM_Sys_ScreenColorCurveSet
  
  Description:
    This function will set a curve to be used as custom gamma or color correction curve. 
    Using this curve, each RGB color channel is individually corrected according to the given 
    curve.
    For the correction in VDCE, the incoming color information of each channel is split into 32 
    equal segments each covering 8 color values. 
    For these 8 values, the same gain factor applies.

    To configure the segments, a start and an end value need to be given. 
    This requires 33 reference points to be passed to this function. 
    For each segment of each color, the gain factor must be in range [x0.25 .. x2.0], 
    thus the values between two reference points may have a difference in range of [0 .. 16]. 
    This function will overwrite the settings of <R_WM_ScreenGammaSet>.

    This function is called from message queue of <R_WM_ScreenColorCurveSet>.

    This function executes the following processing.
      * Calculates RGB gamma correction parameters.
      * Sets the RGB gamma correction parameters by executing <R_VDCE_DisplayGammaCorrectSet>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit         - See description of <R_WM_DevInit>
    ColorCurve   - Pointer to Table of reference points <r_wm_ClutEntry_t>.
                   ALPHA value of data type is unused.
    NumEntries   - Number of reference points. NumEntries should specify 33 fixed.
    
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_ScreenColorCurveSet(const uint32_t                Unit,
                                      const r_wm_ClutEntry_t *const ColorCurve,
                                      const uint32_t                NumEntries);

/***************************************************************************
  Function: R_WM_Sys_ScreenGammaSet

  Description:
    This function sets the output gamma correction. 
    This function will overwrite the settings of <R_WM_Sys_ScreenColorCurveSet>.
    This function is called from message queue of <R_WM_ScreenGammaSet>.

    This function executes the following processing.
      * If all the three Gamma Parameters are 128, the Gamma Correction is turned OFF 
        by executing <R_VDCE_DisplayGammaCorrectSet>.
      * Otherwise, calculate gamma curve and make 33 entry points and executes same 
        process as <R_WM_Sys_ScreenColorCurveSet>.

  Customizing Points:
    This function calculates y = x ^ (1/Gamma) by the approximate expression to get gamma curve.
    If user wants to improve arithmetic accuracy, please change the implement. 
    (e.g. use powf in <math.h>).

  Parameter:
    Unit       - See description of <R_WM_DevInit>
    GammaRed   - Gamma correction factor (32..255 => 0.25..2.0, 128=1.0)
    GammaGreen - Gamma correction factor (32..255 => 0.25..2.0, 128=1.0)
    GammaBlue  - Gamma correction factor (32..255 => 0.25..2.0, 128=1.0)

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_ScreenGammaSet(const uint32_t Unit, 
                                 const uint8_t  GammaRed, 
                                 const uint8_t  GammaGreen, 
                                 const uint8_t  GammaBlue);

/***************************************************************************
  Function: R_WM_Sys_ScreenColorFormatSet

  Description:
    This function sets the color format of the video output signal.
    This function is called from <R_WM_ScreenColorFormatSet>.

    This function executes the following processing.
      * Sets the video output data endian by executing <R_VDCE_DisplayOutEndianSet>.
      * Swaps the video output data red and blue channel by executing <R_VDCE_DisplayOutSwapBR>.
      * Sets the video output format by executing <R_VDCE_DisplayOutFormatSet>.
      * Sets the dither mode to truncate mode or random pattern dither by executing 
        <R_VDCE_DisplayCalibrationSet>.

  Customizing Points:
    Since it does not correspond to the Serial RGB output of VDCE, 
    this function can be modified to support Serial RGB output. 
    User can modify the dither mode.

  Parameter:
    Unit   - WM unit number.
    OutFmt - A color format of <r_wm_OutColorFmt_t> plus optional bit flags.

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_ScreenColorFormatSet(const uint32_t            Unit,
                                       const r_wm_OutColorFmt_t  OutFmt);

/***************************************************************************
  Function: R_WM_Sys_ScreenVocaInit

  Description:
    This function setups VOCA H/W for specified WM unit.
    This function is called from message queue of <R_WM_ScreenVocaInit>.

    This function executes the following processing depending on device.
    
    [RH850/D1Lx]
      * Returns error.

    [RH850/D1Mx]
      * Gets display timing by executing <R_VDCE_DisplayTimingSet>.
      * Check minimum horizontal front porch size. 
      * Calculates <r_voca_Param_t> parameters. It's depending on active high or active low.
      * Sets display timing by executing <R_VOCA_ParamSet>.
    
  Customizing Points:
    It is NOT necessary to modify this function in general use-case.

  Parameter:
    Unit   - WM unit number.
    
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_ScreenVocaInit(const uint32_t                Unit);

/***************************************************************************
  Function: R_WM_Sys_ScreenVocaDeInit

  Description:
    This function disables all VOCA monitoring (Video output monitor and Activity monitor) 
    and deletes all created VOCA monitor areas of specified WM unit.
    This function is called from message queue of <R_WM_ScreenVocaDeInit>.
    
    This function executes the following processing depending on device.
    
    [RH850/D1Lx]
      * Returns error.

    [RH850/D1Mx]
      * Gets created video output monitor area from global flags.
      * Disables all created video output monitor area by executing 
        <R_VOCA_VideoOutputCheckDisable>.
      * Clears global flags that shows created video output monitor area.
      * Disables activity monitor by executing <R_VOCA_ActiveMonitorDisable>.
    
  Customizing Points:
    It is NOT necessary to modify this function in general use-case.

  Parameter:
    Unit   - WM unit number.
    
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_ScreenVocaDeInit(const uint32_t                Unit);

/***************************************************************************
  Function: R_WM_Sys_ScreenVocaCreate

  Description:
    This function creates a video output monitor area to specified WM unit.
    This function is called from message queue of <R_WM_ScreenVocaCreate>.
    
    This function executes the following processing depending on device.
    
    [RH850/D1Lx]
      * Returns error.

    [RH850/D1Mx]
      * Gets created video output monitor area from global flags of another WM unit.
      * Checks for the duplication of creating monitor area number.
      * Sets video output monitor area parameters by executing <R_VOCA_MonitorAreaSet>.
      * Sets expected image to internal RAM by executing <R_VOCA_ColorRamSet> 
        if Voca->Size is not 0.
      * Sets global flags that shows created video output monitor area.
    
  Customizing Points:
    It is NOT necessary to modify this function in general use-case.

  Parameter:
    Unit   - WM unit number.
    Voca   - Voca structure pointer, see <r_wm_Voca_t>

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_ScreenVocaCreate(const uint32_t                Unit,
                                   const r_wm_Voca_t      *const Voca);

/***************************************************************************
  Function: R_WM_Sys_ScreenVocaDelete

  Description:
    This function deletes a video output monitor area from specified WM unit.
    This function is called from message queue of <R_WM_ScreenVocaDelete>.
    
    This function executes the following processing depending on device.
    
    [RH850/D1Lx]
      * Returns error.

    [RH850/D1Mx]
      * Disables the video output monito area by executing <R_WM_Sys_ScreenVocaEnable>.
      * Clears global flags that shows created video output monitor area.
      
  Customizing Points:
    It is NOT necessary to modify this function in general use-case.

  Parameter:
    Unit   - WM unit number.
    Voca   - Voca structure pointer, see <r_wm_Voca_t>

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_ScreenVocaDelete(const uint32_t                Unit,
                                   const r_wm_Voca_t      *const Voca);

/***************************************************************************
  Function: R_WM_Sys_ScreenVocaEnable

  Description:
    This function enables / disables a video output monitor area.
    This function is called from message queue of <R_WM_ScreenVocaEnable> and 
    <R_WM_ScreenVocaDisable>.
    This function is also called from <R_WM_Sys_ScreenVocaDelete>.
    
    This function executes the following processing depending on device.
    
    [RH850/D1Lx]
      * Returns error.

    [RH850/D1Mx]
    For Enable
      * Enables video output checker by executing <R_VOCA_VideoOutputCheckEnable>.

    For Disable
      * Disables video output checker by executing <R_VOCA_VideoOutputCheckDisable>.
    
  Customizing Points:
    It is NOT necessary to modify this function in general use-case.

  Parameter:
    Unit    - WM unit number.
    Voca    - Voca structure pointer, see <r_wm_Voca_t>
    Enabled - 0 : disabled, 1 : enabled

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_ScreenVocaEnable(const uint32_t                Unit,
                                   const r_wm_Voca_t      *const Voca,
                                   const uint32_t                Enabled);

/***************************************************************************
  Function: R_WM_Sys_ScreenVocaExpImgSet

  Description:
    This function sets the expected Image to internal RAM.
    This function also updates the Threshold and RamAddr for specified monitor area.
    This function is called from message queue of <R_WM_ScreenVocaExpImgSet>.
    
    This function executes the following processing depending on device.
    
    [RH850/D1Lx]
      * Returns error.

    [RH850/D1Mx]
      * If Size is not 0, sets VOCA status flags to update internal RAM in 
        <R_WM_Sys_ScreenVocaUpdate>.
      * If RamAddr or Threshold is changed from previous settings, 
        sets VOCA status flags to update VOCA registers in <R_WM_Sys_ScreenVocaUpdate>.
    
  Customizing Points:
    It is NOT necessary to modify this function in general use-case.

  Parameter:
    Unit      - WM unit number.
    Voca      - Voca structure pointer, see <r_wm_Voca_t>
    Threshold - Acceptable mismatch (difference) of a Monitor Area. 
    RamAddr   - Internal RAM start index to update. Same as RamAddr of <r_wm_Voca_t>.
    ExpSize   - The number of array of Data to update. 
                If Size is 0, only Threshold and RamAddr for specified monitor area is updated.
    ExpImg    - Pointer to update expected image array. 

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_ScreenVocaExpImgSet(const uint32_t                Unit,
                                      const r_wm_Voca_t      *const Voca,
                                      const uint32_t                Threshold,
                                      const uint16_t                RamAddr, 
                                      const uint16_t                ExpSize, 
                                      const uint16_t         *const ExpImg);

/***************************************************************************
  Function: R_WM_Sys_ScreenVocaClutSet

  Description:
    This function changes the CLUT data of VOCA.
    This function is called from message queue of <R_WM_ScreenVocaClutSet>.
    
    This function executes the following processing depending on device.
    
    [RH850/D1Lx]
      * Returns error.

    [RH850/D1Mx]
      * Sets VOCA status flags to update VOCA registers in <R_WM_Sys_ScreenVocaUpdate>.
      
  Customizing Points:
    It is NOT necessary to modify this function in general use-case.

  Parameter:
    Unit       - WM unit number.
    Voca       - Voca structure pointer, see <r_wm_Voca_t>
    NumEntries - Number of Clut entries. The range is 1 to 4.
    Clut       - Pointer to array of Clut.

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_ScreenVocaClutSet(const uint32_t                    Unit, 
                                    r_wm_Voca_t                *const Voca,
                                    const uint8_t                     NumEntries,
                                    const r_wm_VocaClutEntry_t *const Clut);

/***************************************************************************
  Function: R_WM_Sys_ScreenActMonEnable

  Description:
    This function enables / disables activity monitor of VOCA.
    LowerTime and UpperTime can be set in (100/3 = 33.3...) microseconds units. 
    WM porting layer rounds to the nearest value.
    This function is called from message queue of <R_WM_ScreenActivityMonEnable> and 
    <R_WM_ScreenActivityMonDisable>.
    
    This function executes the following processing depending on device.
    
    [RH850/D1Lx]
    * Returns error.

    [RH850/D1Mx]
    For Enable
    * Converts UpperTime and LowerTime to 33.33... microseconds unit.
    * Enables video output checker by executing <R_VOCA_ActiveMonitorEnable>.

    For Disable
    * Disables video output checker by executing <R_VOCA_ActiveMonitorDisable>.
        
  Customizing Points:
    It is NOT necessary to modify this function in general use-case.

  Parameter:
    Unit      - WM unit number.
    UpperTime - Upper detection time. Unit is micro seconds.
    LowerTime - Lower detection time. Unit is micro seconds.
    Enabled   - 0 : disabled, 1 : enabled

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_ScreenActMonEnable(const uint32_t                Unit,
                                     const uint32_t                UpperTime,
                                     const uint32_t                LowerTime,
                                     const uint32_t                Enabled);

/***************************************************************************
  Function: R_WM_Sys_ScreenVocaUpdate

  Description:
    This function executes jobs for VOCA in message queue.
    This function is called from <R_WM_FrameExecuteVoca>.
    
    This function executes the following processing depending on device.
    
    [RH850/D1Lx]
      * Returns R_WM_SYS_OK.

    [RH850/D1Mx]
      * Searches video output monitor area from root of linked list. 
        If created video output monitor area is found, performs the following process.
        - - Gets register update flag from VOCA status flags. 
            The flag is set by <R_WM_ScreenVocaExpImgSet> and <R_WM_ScreenVocaClutSet>.
        - - If register update flag is on, sets the expected CRC by executing 
            <R_VOCA_MonitorAreaSet>.
        - - Gets internal RAM update flag from the VOCA status flags. 
            The flag is set by <R_WM_ScreenVocaExpImgSet>.
        - - If internal RAM update flag is on, sets the expected CRC by executing 
            <R_VOCA_ColorRamSet>.
    
  Customizing Points:
    It is NOT necessary to modify this function in general use-case.

  Parameter:
    Unit   - WM unit number.
    
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_ScreenVocaUpdate(const uint32_t                Unit);


/***************************************************************************
  Group: WM Video Output Windows interface functions
*/

/***************************************************************************
  Function: R_WM_Sys_WindowSetFb
  
  Description:
    Sets the visible (front) buffer for the window.
    This function is called from message queue of following functions.
      * <R_WM_WindowSwap>
      * <R_WM_WindowExternalBufSet>

    This function executes the following processing.
      * Lock the layer with the global flag while updating VDCE layer functions.
    
    Following processing is depending on window type. 
    
    [Frame buffer window]
      * Sets the address of frame buffer by executing <R_VDCE_LayerBaseSet>.
    
    [Sprite window]
      * Sets the address of virtual frame buffer by executing <R_VDCE_LayerBaseSet>.

    [RLE window]
      * Checks if the layer is available as RLE window. 
      * Checks the alignment of start address. 
      * Enables the RLE engine and RLE unit.
        - - Sets the RLE source address, color format and ebnables the RLE engine by executing 
            <R_SPEA_SetRle>.
        - - Sets the update flag to process by next <R_WM_Sys_DevFrameFinished>.
        - - If RLE unit is disabled, enables the RLE unit by executing <R_SPEA_UnitEnable>.
      * Sets the starting address of virtual frame buffer by executing <R_VDCE_LayerBaseSet>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    If user wants to move the synthesized start address for sprite window, change the definition 
    of R_WM_SYS_VIRTUAL_ADDR_SPRITE_0 and R_WM_SYS_VIRTUAL_ADDR_SPRITE_1 value.
  
  Parameter:
    Unit    -   WM unit number.
    Win     -   Window structure pointer, see <r_wm_Window_t>
    Fb      -   Buffer
  
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_WindowSetFb(const uint32_t             Unit, 
                              const r_wm_Window_t *const Win, 
                              const void *const          Fb);

/***************************************************************************
  Function: R_WM_Sys_WindowCapabilitiesSet

  Description:
    This function configures the selectable window type, RLE window or Sprite window 
    for each layer set. 
    One layer of WM Unit 0 and one layer of WM Unit 1 have the same configuration.
    Attention: The layer order of WM unit 1 is different from the layer order of WM unit 0. 
    This function is called from <R_WM_WindowCapabilitiesSet>.
    Note that this function can be executed before the initialization.
    
    This function executes the following processing.
      * Configures the selectable window type by executing <R_SPEA_UnitCapabilitiesSet>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.

  Parameter:
    Capability0  - Selects mode for VO0 Layer0 and VO1 Layer0
    Capability1  - Selects mode for VO0 Layer1 and VO1 Layer3
    Capability2  - Selects mode for VO0 Layer2 and VO1 Layer2
    Capability3  - Selects mode for VO0 Layer3 and VO1 Layer1

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_WindowCapabilitiesSet(const r_wm_WinCapbs_t Capability0,
                                        const r_wm_WinCapbs_t Capability1,
                                        const r_wm_WinCapbs_t Capability2,
                                        const r_wm_WinCapbs_t Capability3
                                       );

/***************************************************************************
  Function: R_WM_Sys_WindowCreate
  
  Description:
    This function reconfigures all windows of the specified WM unit.
    This function is called from message queue of <R_WM_WindowCreate>.

    This function executes the following processing.
      * Gets the target window from root window.
      * Loop from Layer0 to Layer3 and executes the following processed.
        - - Disables the layer by executing <R_VDCE_LayerDisable>. 
        - - Checks if the window can be created on the layer. 
            If it can be created, executes the following processed.
            - - - Lock the layer with the global flag while updating VDCE layer functions.
            - - - If Chromakey is enabled:
               - - * Disables the constant alpha by executing <R_VDCE_LayerAlphaConstDisable>. 
               - - * Enables the chromakey by executing <R_VDCE_LayerChromaKeyEnable>.
            - - - If Chromakey is disabled:
               - - * Disables the chromakey by executing <R_VDCE_LayerChromaKeyDisable>. 
               - - * Enables the constant alpha by executing <R_VDCE_LayerAlphaConstEnable>.
            - - - Sets color format by executing <R_VDCE_LayerFormatSet>.
            - - - Sets the update flag to process by next <R_WM_Sys_DevFrameFinished> 
                  if CLUT is enabled.
            - - - Sets the scaling-up/down by executing <R_VDCE_LayerImgScaleX> and 
                  <R_VDCE_LayerImgScaleY>.
            - - - Enables alpha channel by executing <R_VDCE_LayerAlphaChannelEnable>.
            - - - Enables / Disables pre-multiple alpha channel by executing 
                  <R_VDCE_LayerPremultipliedAlphaEnable> / <R_VDCE_LayerPremultipliedAlphaDisable>.
            - - - Sets vertical mirroring by executing <R_VDCE_LayerModeSet>.
            - - - Gets the On-screen frame buffer and executes same process as 
                  <R_WM_Sys_WindowSetFb> to set the start address of the frame buffer.
            - - - Sets the re-assigned flag to process by next <R_WM_Sys_DevFrameFinished> 
                  in case of Sprite window.
            - - - Sets the layers memory geometry by executing <R_VDCE_LayerMemGeometrySet>.
            - - - Sets the layers viewport parameters by <R_VDCE_LayerViewPortSet>.
            - - - Enables the layer by executing <R_VDCE_LayerEnable> if the layer is already 
                  enabled before this sequence is executed.
            - - - Get next window from the window list.
      * An error is returned if the layer to which the window can be assigned is not found.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    
  Parameter:
    Unit                - WM unit number.
    Window              - Window structure pointer, see <r_wm_Window_t>
  
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_WindowCreate(const uint32_t              Unit, 
                               const r_wm_Window_t *const  Win);

/***************************************************************************
  Function: R_WM_Sys_WindowDelete
  
  Description:
    Deletes a window.
    This function is called from message queue of <R_WM_WindowDelete>.

    This function executes the following processing.
    * Disables the specified layer by executing the function <R_VDCE_LayerDisable>, 
      if that layer is in enabled.
    * If the window is RLE window:
      - - Disables the RLE engine by <R_SPEA_SetRle>. 
      - - Disables the RLE unit by <R_SPEA_UnitEnable> if shared RLE unit is already disabled. 
      - - Sets the update flag to process by next <R_WM_Sys_DevFrameFinished>.
    * Update the global variables to manage the layer. 

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    
  Parameter:
    Unit                - WM unit number.
    Window              - Window structure pointer, see <r_wm_Window_t>
  
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_WindowDelete(const uint32_t             Unit, 
                               const r_wm_Window_t *const Win);

/***************************************************************************
  Function: R_WM_Sys_WindowEnable
  
  Description:
    This function will enable/disable the window. 
    If the window position is outside of the screen, this function does not enable the window.
    This function is called from message queue of <R_WM_WindowEnable>.

    This function executes the following processing.
    
    For Enable:
    * Checks if the window position is outside of the screen.
    * Enables the specified layer by executing the function <R_VDCE_LayerEnable>.
    
    For Disable:
    * Disables the specified layer by executing the function <R_VDCE_LayerDisable>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
    Win     - Window structure pointer, see <r_wm_Window_t>
    Enabled - Enabled/Disabled
  
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_WindowEnable(const uint32_t             Unit, 
                               const r_wm_Window_t *const Win, 
                               const uint32_t             Enabled);

/***************************************************************************
  Function: R_WM_Sys_WindowPosSet
  
  Description:
    This function sets window position as specified in the Paramets PosX & PosY. 
    Also sets the layer position as specified in parameter PosZ.
    This function is called from message queue of <R_WM_WindowMove>.

    This function executes the following processing.
    * Lock the layer with the global flag while updating VDCE layer functions.
    * When PosZ is changed from previous value, 
      all window are re-constructed and changes layer assignment of the Windows. 
      Executes same behavior as <R_WM_Sys_WindowCreate>.
    * Set the layers memory geometry by executing <R_VDCE_LayerMemGeometrySet>.
    * Set the layers viewport parameters by <R_VDCE_LayerViewPortSet>.
    * If the layer is already enabled, following 
      - - If the window position is outside of screen, disables the layer by executing 
          <R_VDCE_LayerDisable>.
      - - If the window position is inside of screen, enables the layer by executing 
          <R_VDCE_LayerEnable>.
    * Sets the starting address of frame buffer by executing the function <R_VDCE_LayerBaseSet>
    * Gets the On-screen frame buffer and executes same process as <R_WM_Sys_WindowSetFb> to set 
      the start address of the frame buffer.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit                    - WM unit number.
    Win                     - Window structure pointer, see <r_wm_Window_t>
    PosX, PosY, and PosZ    - New window position
  
  Return value:
    1 if successful, otherwise 0.
    
  Note:
     Setting the PosZ can yield reconfiguring all HW layers.
*/
uint32_t R_WM_Sys_WindowPosSet(const uint32_t             Unit, 
                               const r_wm_Window_t *const Win, 
                               const int32_t              PosX,
                               const int32_t              PosY, 
                               const int32_t              PosZ);

/***************************************************************************
  Function: R_WM_Sys_WindowGeomSet
  
  Description:
    This function sets the window dimensions.
    This function is called from message queue of <R_WM_WindowResize>.

    This function executes the following processing.
    * Lock the layer with the global flag while updating VDCE layer functions.
    * Disables the layer by executing <R_VDCE_LayerDisable> if the layer is enabled.
    * Sets the layers memory geometry by executing <R_VDCE_LayerMemGeometrySet>.
    * Sets the layers viewport parameters by <R_VDCE_LayerViewPortSet>.
    * Sets the layers scaling parameters by <R_VDCE_LayerImgScaleX> and <R_VDCE_LayerImgScaleY>.
    * Enables the layer by executing <R_VDCE_LayerEnable> if the layer is already enabled before 
      this function is called.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
    Win     - Window structure pointer, see <r_wm_Window_t>
    Pitch   - The distance in pixels between subsequent rows in the framebuffer memory (>= Width)
    Width   - The window framebuffer width in pixels
    Height  - The window framebuffer height in pixels
  
  Return value:
    1 if successful, otherwise 0.
  
  D1Mx specific:
    The pitch must be 128 bytes aligned.
*/
uint32_t R_WM_Sys_WindowGeomSet(const uint32_t             Unit,
                                const r_wm_Window_t *const Win,
                                const uint32_t             Pitch,
                                const uint32_t             Width,
                                const uint32_t             Height);

/***************************************************************************
  Function: R_WM_Sys_WindowColorFmtSet
  
  Description:
    This function sets the color format.
    This function is called from message queue of <R_WM_WindowColorFmtSet>.

    This function executes the following processing.
    * When window type is sprite window and bpp of color format is changed from previous setting, 
      sets the layer memory geometry by executing <R_VDCE_LayerMemGeometrySet>.
    * Disables the layer by executing <R_VDCE_LayerDisable> if the layer is enabled.
    * Set the color format by executing <R_VDCE_LayerFormatSet>.
    * Enables the layer by executing <R_VDCE_LayerEnable> if the layer is already enabled 
      before this function is called.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit        -   WM unit number.
    Win         -   Window structure pointer, see <r_wm_Window_t>
    ColorFmt    -   See <r_wm_WinColorFmt_t>
  
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_WindowColorFmtSet(const uint32_t                Unit,
                                    const r_wm_Window_t *const    Win,
                                    const r_wm_WinColorFmt_t      ColorFmt);

/***************************************************************************
  Function: R_WM_Sys_WindowAlphaSet
  
  Description:
    This function will set the constant alpha for the window. 
    If specified window is assigned to layer 0, this function does nothing.  
    This function is called from message queue of <R_WM_WindowAlphaSet>.
    
    This function executes the following processing.
    * Returns the error when the Chromakey is Enabled.
    * Enables the layers alpha constant by executing the function <R_VDCE_LayerAlphaConstEnable>. 

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
    Win     - Window structure pointer, see <r_wm_Window_t>
    Alpha   - Alpha value (0-255)

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_WindowAlphaSet(const uint32_t             Unit, 
                                 const r_wm_Window_t *const Win, 
                                 const uint8_t              Alpha);

/***************************************************************************
  Function: R_WM_Sys_WindowPremultipliedAlphaEnable
  
  Description:
    This function enables or disables the pre-multiplied alpha mode for the window. 
    If specified window is assigned to layer 0, this function does nothing.  
    This function is called from message queue of following functions. 
    * <R_WM_WindowPremultipliedAlphaEnable> 
    * <R_WM_WindowPremultipliedAlphaDisable> 

    This function executes the following processing.
    * Disable the layer by executing <R_VDCE_LayerDisable> if the layer is enabled.
    * If Enabled is 1, enables the pre-multiplied alpha mode by executing 
      <R_VDCE_LayerPremultipliedAlphaEnable>.
    * If Enabled is 0, disables the pre-multiplied alpha mode by executing 
      <R_VDCE_LayerPremultipliedAlphaDisable>.
    * Enables the layer by executing <R_VDCE_LayerEnable> if the layer is already enabled 
      before this function is called.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
    Win     - Window structure pointer, see <r_wm_Window_t>
    Enabled - True/False
  
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_WindowPremultipliedAlphaEnable(const uint32_t             Unit, 
                                                 const r_wm_Window_t *const Win, 
                                                 const uint8_t              Enabled);

/***************************************************************************
  Function: R_WM_Sys_WindowFlagsUpdate
  
  Description:
    This function will provide various ON/OFF switches for different functionalities of window. 
    This function is called from message queue of following functions. 
    * <R_WM_WindowVerticalMirrorEnable> 
    * <R_WM_WindowVerticalMirrorDisable>

    This function executes the following processing.
    * Lock the layer with the global flag while updating VDCE layer functions.
    * Disables the layer by executing <R_VDCE_LayerDisable> if the layer is enabled.
    * If the SetFlags = R_WM_WINFLAG_V_MIRROR, enable 
      the vertical mirroring by executing <R_VDCE_LayerModeSet>.
    * If the ClearFlags = R_WM_WINFLAG_V_MIRROR,
      disable the vertical mirroring by executing <R_VDCE_LayerModeSet>.
    * Enables the layer by executing <R_VDCE_LayerEnable> if the layer is already enabled 
      before this function is called. 
    
  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit        - WM unit number.
    Win         - Window structure pointer, see <r_wm_Window_t>
    SetFlags    - New flags to be set
    ClearFlags  - New flags to be cleared
  
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_WindowFlagsUpdate (const uint32_t             Unit,
                                     const r_wm_Window_t *const Win,
                                     const r_wm_WinFlags_t      SetFlags, 
                                     const r_wm_WinFlags_t      ClearFlags);

/***************************************************************************
  Function: R_WM_Sys_WindowClutSet
  
  Description:
    This function will set the color lookup-table for the window. 
    This function is called from message queue of <R_WM_WindowClutSet>.
    The parameter NumEntires and Clut should be updated to Window structure 
    before this function is called.

    This function executes the following processing.
    * If specified window is assigned to Layer 0 and one of alpha value of CLUT is not 0xFF 
      (Clut[n].A != 0xFF), all windows will be re-constructed and changes layer assignment of 
      the Windows. Executes same behavior as <R_WM_Sys_WindowCreate>.
    * Sets the update flag to process by next <R_WM_Sys_DevFrameFinished>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit        - WM unit number.
    Win         - Window structure pointer, see <r_wm_Window_t>
    NumEntries  - The number of color lookup-table entries
    Clut        - The color lookup-table pointer
  
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_WindowClutSet(const uint32_t                Unit, 
                                const r_wm_Window_t *const    Win,
                                const uint32_t                NumEntries, 
                                const r_wm_ClutEntry_t *const Clut);

/***************************************************************************
  Function: R_WM_Sys_WindowColorKeyEnable
  
  Description:
    This function enables or disables the chroma keying for the window. 
    This function returns error if specified window is assigned to Layer0. 
    This function is called from message queue of following functions.
    * <R_WM_WindowColorKeyEnable>
    * <R_WM_WindowColorKeyDisable>

    This function executes the following processing.

    For Enable:
    * Disables the constant alpha by executing <R_VDCE_LayerAlphaConstDisable>.
    * Enables the chroma keying by executing <R_VDCE_LayerChromaKeyEnable>.

    For Disable:
    * Disables the chroma keying by executing <R_VDCE_LayerChromaKeyDisable>. 
    * Enables the alpha constant by executing <R_VDCE_LayerAlphaConstEnable>.

  Parameter:
    Unit    -   WM unit number.
    Win     -   Window structure pointer, see <r_wm_Window_t>
    Enabled -   Enabled/Disabled
  
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_WindowColorKeyEnable(const uint32_t             Unit, 
                                       const r_wm_Window_t *const Win, 
                                       const uint32_t             Enabled);

/***************************************************************************
  Function: R_WM_Sys_WindowScaleSet
  
  Description:
    This function changes the scaling mode and scaled size for the window. 
    This function is called from message queue of following functions.
    * <R_WM_WindowScaledSizeSet>
    * <R_WM_CaptureScaledSizeSet>

    This function executes the following processing.
    * Lock the layer with the global flag while updating VDCE layer functions.
    * Changes the horizontal scaling behavior and scaling size by executing 
      <R_VDCE_LayerImgScaleX>. 
    * Changes the vertical scaling behavior and scaling size by executing 
      <R_VDCE_LayerImgScaleY>. 
    * If both scaling-down and scaling-up are enabled, only scaling-down is effective.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit         - WM unit number.
    Win          - Window structure pointer, see <r_wm_Window_t>
    ChangeMode   - Change behavior. See <r_wm_ScaleChg_t>.
    ScaledWidth  - The horizontal scaling-down or scaling-up size.
                   If horizontal scaling is disabled, set to 0.
    ScaledHeight - The vertical scaling-down or scaling-up size.
                   If vertical scaling is disabled, set to 0.
    
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_WindowScaleSet(const uint32_t             Unit,
                                 const r_wm_Window_t *const Win,
                                 const r_wm_ScaleChg_t      ChangeMode,
                                 const uint32_t             ScaledWidth,
                                 const uint32_t             ScaledHeight);

/***************************************************************************
  Group: WM Video Output Sprite interface functions
*/

/***************************************************************************
  Function: R_WM_Sys_SpriteCreate
  
  Description:
    This function will add a sprite data to the sprite window. 
    This function is called from message queue of <R_WM_SpriteCreate>.

    This function executes the following processing.
      * Checks if Sprite->PosY and (Sprite->PosY + Sprite->Height) are within range.
      * Sets the re-assigned flag to process by next <R_WM_Sys_DevFrameFinished>.
      * Execute <R_WM_Sys_SpriteEnable> to disable the sprite data as default.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
    Sprite  - Sprite structure pointer, see <r_wm_Sprite_t>
  
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_SpriteCreate(const uint32_t             Unit, 
                               const r_wm_Sprite_t *const Sprite);

/***************************************************************************
  Function: R_WM_Sys_SpriteEnable
  
  Description:
    This function enables or disables the sprite data. 
    This function is called from message queue of following functions.
    * <R_WM_SpriteEnable> 
    * <R_WM_SpriteDisable>

    This function executes the following processing.
    * Gets the sprite index from sprite window list in PosZ order. 
      If valid sprite index is not obtained, this function returns error. 
    * Enables or disables the sprite data by <R_SPEA_SpriteEnable>.
    * Sets the re-assigned flag to process by next <R_WM_Sys_DevFrameFinished>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
    Sprite  - Sprite structure pointer, see <r_wm_Sprite_t>
    Enabled - Enabled/disabled
    
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_SpriteEnable(const uint32_t             Unit, 
                               const r_wm_Sprite_t *const Sprite, 
                               const uint32_t             Enabled);

/***************************************************************************
  Function: R_WM_Sys_SpriteDelete
  
  Description:
    This function deletes the sprite from the window.
    This function is called from message queue of <R_WM_SpriteDelete>.

    This function executes the following processing.
    * Sets the re-assigned flag to process by next <R_WM_Sys_DevFrameFinished>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
    Sprite  - Sprite structure pointer, see <r_wm_Sprite_t>

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_SpriteDelete(const uint32_t             Unit, 
                               const r_wm_Sprite_t *const Sprite);

/***************************************************************************
  Function: R_WM_Sys_SpriteMove
  
  Description:
    This function moves the sprite data on the sprite window. 
    This function is called from message queue of <R_WM_SpriteMove>.

    This function executes the following processing.
    * Checks if Sprite->PosY and (Sprite->PosY + Sprite->Height) are within range.
    * If Unit is 1, adds the offset to PosY value.
    * Changes the sprite position by <R_SPEA_SetSpritePos>.
    * Sets the re-assigned flag to process by next <R_WM_Sys_DevFrameFinished> if PosZ is changed. 
    * Sets the update flag to process by next <R_WM_Sys_DevFrameFinished> 

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit                - WM unit number.
    Sprite              - Sprite structure pointer, see <r_wm_Sprite_t>
    PosX, PosY, PosZ    - New sprite position
  
  Return value:
    1 if successful, otherwise 0.
    
  Note:
    Changing PosZ can yield HW reconfiguration of all sprites on the same window.
*/
uint32_t R_WM_Sys_SpriteMove(const uint32_t             Unit,
                             const r_wm_Sprite_t *const Sprite,
                             const uint32_t             PosX, 
                             const uint32_t             PosY, 
                             const uint32_t             PosZ);

/***************************************************************************
  Function: R_WM_Sys_SpriteBufSet
  
  Description:
    This function sets the sprite source data address. 
    This function is called from message queue of <R_WM_SpriteBufSet>.

    This function executes the following processing.
    * Sets sprite data source address by executing <R_SPEA_SetSprite>.
    * Sets the update flag to process by next <R_WM_Sys_DevFrameFinished>. 

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
    Window  - Window structure pointer, see <r_wm_Window_t>

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_SpriteBufSet(const uint32_t             Unit, 
                               const r_wm_Sprite_t *const Sprite, 
                               const void *const          Buf);

/***************************************************************************
  Function: R_WM_Sys_WindowDeleteAllSprites
  
  Description:
    This function deletes all sprite data on the window.
    This function is called from message queue of <R_WM_WindowDeleteAllSprites>.

    This function executes the following processing.
    * Disables the all sprite data on the window by executing <R_SPEA_SpriteEnable>.
    * Sets the re-assigned flag to process by next <R_WM_Sys_DevFrameFinished>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
    Win     - Window structure pointer, see <r_wm_Window_t>

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_WindowDeleteAllSprites(const uint32_t             Unit,
                                         const r_wm_Window_t *const Win);

/***************************************************************************
  Group: Video Capture interface functions
*/

/***************************************************************************
  Function: R_WM_Sys_CaptureCreate
  
  Description:
    The HW specific part of the <R_WM_CaptureCreate>.
    This function is called from message queue of <R_WM_CaptureCreate>.

    This function executes the following processing.
    * All windows are re-constructed and changes layer assignment of the Windows. 
      Executes same behavior as <R_WM_Sys_WindowCreate>.
    * Sets VDCECTL register depending on Input video format.
    * Sets the capturing mode by executing <R_VDCE_CapModeSet>.
    
    If R_WM_CAPMODE_SYNC_ONLY is not set:
    * Sets capture buffer and viewport by executing <R_VDCE_CapBufGeometrySetup>.
    * Sets writing rate and field mode by executing <R_VDCE_CapRateSet>.
    * Sets global status flag to BOB deinterlace mode or not.
    * Sets the Vsync delay by executing <R_VDCE_LayerVSyncDelaySet>.
    * Sets color matrix by executing <R_VDCE_LayerMatrixBT601Set>.
    
  Customizing Points:
    * User can modify VDCECTL setting and color matrix selection.
  
  Parameter:
    Unit    - WM unit number.
    Capt    - Capture device object pointer, see <r_wm_Capture_t>

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_CaptureCreate(const uint32_t              Unit, 
                                const r_wm_Capture_t *const Capt);

/***************************************************************************
  Function: R_WM_Sys_CaptureDelete
  
  Description:
    This function deletes a video capture surface. 
    This function is called from message queue of <R_WM_CaptureDelete>.
    
    This function executes the following processing
    * Disables the video capturing by executing <R_WM_Sys_CaptureEnable>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.

  Parameter:
    Unit    - WM unit number.
    Capt    - Capture device pointer, see <r_wm_Capture_t>
  
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_CaptureDelete(const uint32_t              Unit, 
                                const r_wm_Capture_t *const Capt);

/***************************************************************************
  Function: R_WM_Sys_CaptureEnable
  
  Description:
    This function enables or disables the capture surface. 
    This function is called from message queue of following functions.
    * <R_WM_CaptureEnable> 
    * <R_WM_CaptureDisable> 
    
    This function executes the following processing.
    
    For Enable
    * Sets callback of R_VDCE_INTC_CAP_VBLANK event by executing <R_VDCE_IntcCallbackSet>.
    * Enables the capture window by executing <R_VDCE_CapEnable>.
    * Enables the R_VDCE_INTC_CAP_VBLANK interrupt by executing <R_VDCE_IntcEnable>.
    * Updates global flag to manage capturing status.

    For Disable
    * Disables the R_VDCE_INTC_CAP_VBLANK interrupt by executing <R_VDCE_IntcDisable>.
    * Disables the capture window by executing <R_VDCE_CapDisable>. 
    * Updates global flag to manage capturing status.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
    Window  - Window structure pointer, see <r_wm_Window_t>
    Enabled - Enabled/disabled flag
  
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_CaptureEnable(const uint32_t              Unit, 
                                const r_wm_Capture_t *const Capt, 
                                const uint32_t              Enabled);

/***************************************************************************
  Function: R_WM_Sys_CaptureViewPortSet
  
  Description:
    This function changes the capturing position and size.
    This function is called from message queue of following functions.
      * <R_WM_CaptureMove>
      * <R_WM_CaptureResize>

    This function executes the following processing.
      * Changes the capture position and size by executing <R_VDCE_CapViewPortSet>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
    Capt    - Capture device pointer, see <r_wm_Capture_t>
    StartX  - X position of capturing start. Unit is pixels. 
    StartY  - Y position of capturing start. Unit is pixels. 
    Width   - Width of capturing video data. Unit is pixels.
    Height  - Height of capturing video data. Unit is pixels.
  
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_CaptureViewPortSet(const uint32_t              Unit, 
                                     const r_wm_Capture_t *const Capt,
                                     const uint32_t              StartX,
                                     const uint32_t              StartY,
                                     const uint32_t              Width,
                                     const uint32_t              Height);

/***************************************************************************
  Function: R_WM_Sys_CaptureExtVsyncSet
  
  Description:
    This function sets the Hsync cycle of input signal and Vsync protection.
    This function is called from <R_WM_FrameExecuteNext> 
    that has message queue of <R_WM_CaptureExtVsyncSet>.

    This function executes the following processing.
      * Sets the Hsync cycle of input signal and Vsync protection by executing 
        <R_VDCE_CapExtVsyncSet>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit         - WM unit number.
    Window       - Capture device pointer, see <r_wm_Capture_t>
    HsyncCycle   - Horizontal cycle of input signal. Unit is cycle (pixel). The range is 4 to 2046.
    VsyncMaskUs  - Prevent Vsync coming faster than VsyncMaskUs [usec].
    VsyncLackUs  - Compensate Vsync coming slower than VsyncLackUs [usec]. 
  
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_CaptureExtVsyncSet(const uint32_t              Unit, 
                                     const r_wm_Capture_t *const Capt,
                                     const uint16_t              HsyncCycle,
                                     const uint32_t              VsyncMaskUs,
                                     const uint32_t              VsyncLackUs);

/***************************************************************************
  Group: Discom interface functions
*/

/***************************************************************************
  Function: R_WM_Sys_DiscomCreate
  
  Description:
    This function creates a Discom device to specified WM unit. 
    This function is called from <R_WM_FrameExecuteNext>
    that has message queue of <R_WM_DiscomCreate>.

    This function executes the following processing depending on device.

    [RH850/D1Lx]
      * Returns error.

    [RH850/D1Mx]
      * Checks the consistency between WM Unit and Discom->DiscomUnit.
      * Sets start position and length by executing <R_DISCOM_ParamSet>.
      * Sets expected CRC value by executing <R_DISCOM_CrcSet>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
    Discom  - Discom device object pointer, see <r_wm_Discom_t>

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_DiscomCreate(const uint32_t              Unit, 
                               const r_wm_Discom_t  *const Discom);

/***************************************************************************
  Function: R_WM_Sys_DiscomDelete
  
  Description:
    This function deletes a Discom device.
    This function is called from <R_WM_FrameExecuteNext>
    that has message queue of <R_WM_DiscomDelete>.

    This function executes the following processing depending on device.

    [RH850/D1Lx]
      * Returns error.

    [RH850/D1Mx]
      * Disables DISCOM device by <R_WM_Sys_DiscomEnable>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
    Discom  - Discom device object pointer, see <r_wm_Discom_t>

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_DiscomDelete(const uint32_t              Unit, 
                               const r_wm_Discom_t  *const Discom);

/***************************************************************************
  Function: R_WM_Sys_DiscomEnable
  
  Description:
    This function enables / disables a Discom device.
    This function is called from <R_WM_FrameExecuteNext>
    that has message queue of <R_WM_DiscomEnable> and <R_WM_DiscomDisable>.
    This function is also called from <R_WM_Sys_DiscomDelete>.

    This function executes the following processing depending on device.

    [RH850/D1Lx]
      * Returns error.

    [RH850/D1Mx]
    
    For Enable
      * Enables DISCOM interrupts by executing <R_DISCOM_IntEnable>.
      * Enables calculation and comparator by executing <R_DISCOM_Enable>.
    
    For Disable
      * Disables calculation and comparator by executing <R_DISCOM_Disable>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
    Discom  - Discom device object pointer, see <r_wm_Discom_t>
    Enabled - 0 : disabled, 1 : enabled

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_DiscomEnable(const uint32_t              Unit, 
                               const r_wm_Discom_t  *const Discom,
                               const uint32_t              Enabled);

/***************************************************************************
  Function: R_WM_Sys_DiscomCrcSet
  
  Description:
    This function changes expected CRC to DISCOM device.
    This function is called from <R_WM_FrameExecuteNext>
    that has message queue of <R_WM_DiscomCrcSet>.

    This function executes the following processing.
    
    [RH850/D1Lx]
      * Returns error.

    [RH850/D1Mx]
      * Sets global flags to update CRC in <R_WM_Sys_DiscomUpdate>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
    Discom  - Discom device object pointer, see <r_wm_Discom_t>
    ExpCrc  - Expected CRC value

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_DiscomCrcSet(const uint32_t              Unit, 
                               const r_wm_Discom_t  *const Discom,
                               const uint32_t              ExpCrc);

/***************************************************************************
  Function: R_WM_Sys_DiscomCrcGet
  
  Description:
    This function gets the latest calculated CRC value.
    This function is called from <R_WM_DiscomCrcGet>.

    This function executes the following processing.
    
    [RH850/D1Lx]
      * Returns error.

    [RH850/D1Mx]
      * Gets current CRC value by executing <R_DISCOM_CrcGet>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
    Discom  - Discom device object pointer, see <r_wm_Discom_t>
    Crc     - The latest calculated CRC value.

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_DiscomCrcGet(const uint32_t              Unit, 
                               const r_wm_Discom_t  *const Discom,
                               uint32_t             *const Crc);

/***************************************************************************
  Function: R_WM_Sys_DiscomUpdate
  
  Description:
    This function executes jobs for DISCOM in message queue.
    This function is called from <R_WM_FrameWait> and <R_WM_FrameExecuteDiscom>.

    This function executes the following processing.
    
    [RH850/D1Lx]
      * Returns R_WM_SYS_OK.

    [RH850/D1Mx]
      * Searches Discom device from root of linked list. 
        If created Discom device is found, performs the following process.
        - - Gets update flag from the frame status flags. 
            The flag is set by <R_WM_Sys_DiscomCrcSet>.
        - - If update flag is on, sets the expected CRC by executing <R_DISCOM_CrcSet>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.

  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_DiscomUpdate(const uint32_t Unit);

/***************************************************************************
  Group: WM Messaging interface functions.
*/

/***************************************************************************
  Function: R_WM_Sys_MsgQueueSetup
  
  Description:
    Setup the message queue.
    This function is called from <R_WM_DevInit>.

    This function executes the following processing.
      * Initialisation of internal ringbuffer for WM configuration messages

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit                -   WM unit number.
    MsgQueueStorage     -   Storage for the message queue
    Size                -   Number of elements in the message queue
    
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_MsgQueueSetup(const uint32_t  Unit, 
                                void *const     MsgQueueStorage, 
                                const uint32_t  Size);

/***************************************************************************
  Function: R_WM_Sys_MsgQueueRead
  
  Description:
    Reads one message and removes it from the queue.
    This function is called from <R_WM_FrameExecuteNext>.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
    Msg     - Pointer to the message, see <r_wm_Msg_t>
  
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_MsgQueueRead(const uint32_t    Unit, 
                               r_wm_Msg_t *const Msg);

/***************************************************************************
  Function: R_WM_Sys_MsgQueueWrite
  
  Description:
    Writes one message to the queue.
    This function is called from several APIs that use message queue system.

  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
  
  Parameter:
    Unit    - WM unit number.
    Msg     - Pointer to the message, see <r_wm_Msg_t>
  
  Return value:
    1 if successful, otherwise 0.
*/
uint32_t R_WM_Sys_MsgQueueWrite(const uint32_t    Unit, 
                                r_wm_Msg_t *const Msg);

/***************************************************************************
  Group: WM Memory functions
*/

/***************************************************************************
  Function: R_WM_Sys_Heap_Set

  Description:
    This will set the platform specific heaps.
    This function is called from <R_WM_DevInit>.

    This function executes the following processing.
      * The referenced heaps will be stored driver-internal.
      * The heaps are valid for both instances

  Customizing Points:
    If a custom allocator is used, please update this function accordingly.
  
  Parameter:
    Cpu    - CPU heap 
    Video  - Video heap 
  
  Return value:None.
*/
void R_WM_Sys_Heap_Set(void *const Cpu, 
                       void *const Video);

/***************************************************************************
  Function: R_WM_Sys_Alloc
  
  Description:
    Allocates memory. This function will be used by the framework
    if the window FB mode is R_WM_WINBUF_ALLOC_INTERNAL.
    This function is called from <R_WM_WindowCreate>.

  Customizing Points:
    If a custom allocator is used, please update this function accordingly.
  
  Parameter:
    Size                - Size of the memory block
    MemType             - The type of the memory to be allocated, see <r_wm_Memory_t>
    
  Return value:
    Pointer to the allocated memory.
*/
void *R_WM_Sys_Alloc(const uint32_t      Size,
                     const r_wm_Memory_t MemType);

/***************************************************************************
  Function: R_WM_Sys_Free
  
  Description:
    Deallocates memory allocated with <R_WM_Sys_Free>. This function will
    be used by the framework if the window FB mode is R_WM_WINBUF_ALLOC_INTERNAL.
    This function is called from several WM APIs.

  Customizing Points:
    If a custom allocator is used, please update this function accordingly.
    
  Parameter:
    Memory      - Memory pointer
    MemType     - The type of the memory to be allocated, see <r_wm_Memory_t>
    
  Return value:
    1 if successful, otherwise 0.
   
*/
uint32_t R_WM_Sys_Free(const void  *const  Memory,
                       const r_wm_Memory_t MemType);

/***************************************************************************
  Group: WM OS interface functions
*/

/***************************************************************************
  Function: R_WM_Sys_LockWindows
  
  Description:
    This function locks the windows of WM driver access to the specified unit from other threads.
    This function is called from several WM APIs.

  Customizing Points:
    If an OS is used and the WM API is called from multiple threads,
    please implement the lock process by mutex or semaphore.

    
  Parameter:
    Unit    - WM unit number.
    
  Return value:
    None.
*/
void R_WM_Sys_LockWindows(const uint32_t Unit);

/***************************************************************************
  Function: R_WM_Sys_UnlockWindows
  
  Description:
    This function unlocks the window of WM driver access to the specified unit from other threads.
    This function is called from several WM APIs.

  Customizing Points:
    If an OS is used and the WM API is called from multiple threads,
    please implement the lock process by mutex or semaphore.
    
  Parameter:
    Unit    - WM unit number.
    
  Return value:
    None.
*/
void R_WM_Sys_UnlockWindows(const uint32_t Unit);

/***************************************************************************
  Function: R_WM_Sys_LockMsgQueue
  
  Description:
    This function locks the message queue of WM driver access to the specified 
    unit from other threads. This function is called from several WM APIs.

  Customizing Points:
    If an OS is used and the WM API is called from multiple threads,
    please implement the lock process by mutex or semaphore.
    
  Parameter:
    Unit    - WM unit number.
    
  Return value:
    None.
*/
void R_WM_Sys_LockMsgQueue(const uint32_t Unit);

/***************************************************************************
  Function: R_WM_Sys_UnlockMsgQueue
  
  Description:
    This function unlocks the message queue of WM driver access to the 
    specified unit from other threads. This function is called from several WM APIs.

  Customizing Points:
    If an OS is used and the WM API is called from multiple threads,
    please implement the lock process by mutex or semaphore according to <R_WM_Sys_LockMsgQueue>.
    
  Parameter:
    Unit    - WM unit number.
    
  Return value:
    None.
*/
void R_WM_Sys_UnlockMsgQueue(const uint32_t Unit);

/***************************************************************************
  Function: R_WM_Sys_LockBuffers
  
  Description:
    This function locks the buffers of WM driver access to the specified unit from other threads.
    This function is called from several WM APIs.

  Customizing Points:
    If an OS is used and the WM API is called from multiple threads,
    please implement the lock process by mutex or semaphore.
    
  Parameter:
    Unit    - WM unit number.
    
  Return value:
    None.
*/
void R_WM_Sys_LockBuffers(const uint32_t Unit); 

/***************************************************************************
  Function: R_WM_Sys_UnlockBuffers
  
  Description:
    This function unlocks the buffers of WM driver access to the specified unit from other threads.
    This function is called from several WM APIs.

  Customizing Points:
    If an OS is used and the WM API is called from multiple threads,
    please implement the lock process by mutex or semaphore according to <R_WM_Sys_LockBuffers>.
    
  Parameter:
    Unit    - WM unit number.
    
  Return value:
    None.
*/
void R_WM_Sys_UnlockBuffers(const uint32_t Unit);

/***************************************************************************
  Function: R_WM_Sys_LockDevice
  
  Description:
    This function locks the all devices.
    There is only one resource to lock, not each WM Unit.
    This function is called from <R_WM_DevInit>.
    This function is also called from following porting layer functions.
    * <R_WM_Sys_DevFrameFinished>
    * <R_WM_Sys_SpriteEnable>
    * <R_WM_Sys_SpriteMove>
    * <R_WM_Sys_SpriteBufSet>
    * <R_WM_Sys_WindowDeleteAllSprites>
    * <R_WM_Sys_WindowCreate>
    * <R_WM_Sys_WindowSetFb>
    * <R_WM_Sys_WindowPosSet>
    * <R_WM_Sys_CaptureCreate>
    * <R_WM_Sys_CaptureEnable>

  Customizing Points:
    Please implement the lock process by mutex or semaphore if WM API is called from 
    multi-thread.
    
  Parameter:
    Unit    - WM unit number.
    
  Return value:
    None.
*/
void R_WM_Sys_LockDevice(const uint32_t Unit);  

/***************************************************************************
  Function: R_WM_Sys_UnlockDevice
  
  Description:
    This function unlocks the all devices.
    There is only one resource to lock, not each WM Unit.
    This function is called from <R_WM_DevInit>.
    This function is also called from following porting layer functions.
    * <R_WM_Sys_DevFrameFinished>
    * <R_WM_Sys_SpriteEnable>
    * <R_WM_Sys_SpriteMove>
    * <R_WM_Sys_SpriteBufSet>
    * <R_WM_Sys_WindowDeleteAllSprites>
    * <R_WM_Sys_WindowCreate>
    * <R_WM_Sys_WindowSetFb>
    * <R_WM_Sys_WindowPosSet>
    * <R_WM_Sys_CaptureCreate>
    * <R_WM_Sys_CaptureEnable>

  Customizing Points:
    If an OS is used and the WM API is called from multiple threads,
    please implement the unlock process by mutex or semaphore according to <R_WM_Sys_LockDevice>.
    
  Parameter:
    Unit    - WM unit number.
    
  Return value:
    None.
*/
void R_WM_Sys_UnlockDevice(const uint32_t Unit);  

/***************************************************************************
  Group: WM Other interface functions
*/

/***************************************************************************
  Function: R_WM_Sys_GetLastError

  Description:
    Due to technical reasons, the WM SYS layer only returns success/failure without specific error 
    codes. Using this function, you can request information about the last error that happened in 
    the SYS layer of the WM. Calling this function will reset the error flags and allows for a new 
    error to be recorded. 
    
    To use this function, the WM SYS layer must be recompiled with the flag 
    R_WM_SYS_ERROR_TRACKING.
    
  Customizing Points:
    It is NOT necessary to modify this function in general use-case.
    For further debugging, the function may be expanded to provide <File> and <Line> of the error.
  
  Parameter:
    Unit              - Reference parameter to optionally get the WM Unit that caused the error.
    AdditionalInfo    - Reference parameter to optionally get additional error codes from VDCE 
                        or SPEA driver.
    Overflow          - Reference parameter to optionally get information if errors happened after 
                        this one occurred.
    New               - Reference parameter to optionally check if this is a new error with 
                        respect to the last call to <R_WM_Sys_GetLastError>
  
  Returns:
    r_wm_Error_t      - Error code of the error.
*/

r_wm_Error_t R_WM_Sys_GetLastError(uint32_t *const Unit,
                                   uint32_t *const AdditionalInfo,
                                   uint32_t *const Overflow,
                                   uint32_t *const New);

#ifdef __cplusplus
}
#endif

#endif /* R_WM_SYS_H */
