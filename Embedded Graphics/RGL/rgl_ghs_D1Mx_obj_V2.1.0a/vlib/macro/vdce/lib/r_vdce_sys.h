/*
****************************************************************************
PROJECT : VDCE driver
============================================================================
DESCRIPTION
VDCE support functions
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
  Title: VDCE Driver Support Functions

  VDCE (Video Data Controller E) driver support functions

  The generic VDCE driver uses these functions. They have to be implemented
  within the driver library for a concrete device. (e.g. RH850/D1L, RH850/D1M)

  Support functions are functions that are not part of the driver itself
  but they must be provided to integrate the driver on a particular board.

*/

#ifndef R_VDCE_SYS_H
#define R_VDCE_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
  Section: Global Defines
*/

/***************************************************************************
  Constant: Layer Index

  The definition for each layer.
*/
#define R_VDCE_SYS_MAX_LAYER_NUM           (R_VDCE_LAYER_LAST + 1u)
#define R_VDCE_SYS_LAYER_OIR               (R_VDCE_LAYER_LAST)
#define R_VDCE_SYS_LAYER_CAPT              (R_VDCE_LAYER_INPUT + 1u)
#define R_VDCE_SYS_LAYER_ALL               (0xFFFFFFFFu)

/***************************************************************************
  Section: Global Types
*/

/***************************************************************************
  Group: Enumerations and Types
*/

/***************************************************************************
  typedef: r_vdce_DeviceInfo_t

  Description:
  The structure holding the value of device information in the function 
  <R_VDCE_Sys_DeviceInfoGet>.

  Struct members:
  Device         - Device version number that VDCE assumes.
  VIWithDataEn   - availability of video input with data enable signal.
  SerialRGBEn    - availability of Serial RGB output.
  GammaCorrectEn - availability of Gamma correction.
  EnlargementEn  - availability of enlargement.
*/
typedef struct
{
    r_dev_Device_t  Device;
    uint8_t         VIWithDataEn;
    uint8_t         SerialRGBEn;
    uint8_t         GammaCorrectEn;
    uint8_t         EnlargementEn;
} r_vdce_DeviceInfo_t;

/***************************************************************************
  Enum: r_vdce_State_t

  Description:
  This type describes the state of VDCE.

  Values:
  R_VDCE_STATE_UNINITIALIZED - The VDCE driver is an uninitialized state.
  R_VDCE_STATE_INITIALIZED   - The VDCE driver is an initialization state.
  R_VDCE_STATE_IDLE          - The VDCE driver is an idle state.
  R_VDCE_STATE_EXECUTING     - The VDCE driver is an executing state.
*/
typedef enum
{
    R_VDCE_STATE_UNINITIALIZED = 0,
    R_VDCE_STATE_INITIALIZED,
    R_VDCE_STATE_IDLE,
    R_VDCE_STATE_EXECUTING
} r_vdce_State_t;

/***************************************************************************
  Section: Global Functions
*/
/***************************************************************************
  Group: VDCE driver Basic interface functions
*/

/***************************************************************************
  Function: R_VDCE_Sys_Init

  Description:
  This function initializes environment-dependent part.
  This function is called from <R_VDCE_Init>.

  This function is empty as default.

  Customizing Points:
  * It is NOT necessary to modify this function in general use-case.
  * If you want to add the de-initialization code of environment-depend (e.g. clock control),
    implement to this function.

  Parameter:
  Unit       - Instance number

  Return Value:
  see: <r_vdce_Error_t>
*/
r_vdce_Error_t R_VDCE_Sys_Init(const uint32_t Unit);

/***************************************************************************
  Function: R_VDCE_Sys_DeInit

  Description:
  This function de-initializes environment-dependent part.
  This function is called from <R_VDCE_DeInit>.

  This function is empty as default.

  Customizing Points:
  * It is NOT necessary to modify this function in general use-case.
  * If you want to add the de-initialization code of environment-depend (e.g. clock control),
    please implement to this function.

  Parameter:
  Unit       - Instance number

  Return Value:
  see: <r_vdce_Error_t>
*/
r_vdce_Error_t R_VDCE_Sys_DeInit(const uint32_t Unit);

/***************************************************************************
  Function: R_VDCE_Sys_BaseAddrGet 

  Description:
  This function returns the base address of specified VDCE unit.
  This function is called from several VDCE APIs.

  This function executes the following processing.
  * Returns the base address of VDCE macro.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  Unit       - Instance number
  
  Return Value:
  base       - base address of specified VDCE unit.
  0          - specified unit does not exist.
*/
uint32_t R_VDCE_Sys_BaseAddrGet(const uint32_t Unit);

/***************************************************************************
  Function: R_VDCE_Sys_PixelClockSet

  Description:
  This function sets the pixel clock and video output settings.
  This function is called from <R_VDCE_Init>, <R_VDCE_DeInit>, <R_VDCE_DisplayEnable>,
  <R_VDCE_DisplayDisable> and <R_VDCE_DisplayTimingAdjust>.

  This function executes the following processing.
  * Calculates the division ratio of DOTCLK0/DOTCLK1 so that it is close to the specified clock.
  * Returns the actual clock of DOTCLK0/DOTCLK1.
  * Executes the following processing depending on the device.

  <D1M2(H)>
  * Selects DOTCLK0/DOTCLK1 as the divider of PLL2CLK.
  * Selects C_ISO_VDCE0CLK/C_ISO_VDCE1CLK as DOTCLK0/DOTCLK1 in case of LVTTL.
  * Selects C_ISO_VDCE0CLK/C_ISO_VDCE1CLK as the divider of DOTCLK0/DOTCLK1 in case of RSDS.
  * Selects C_ISO_RSDSCLK as DOTCLK0 or DOTCLK1 in case of RSDS.
  * Sets RSDSCFG register.
  * Sets CKSC_IVOEXS_CTL depending on VOSL field of VDCECTL. 

  <D1M1A>
  * Selects DOTCLK0/DOTCLK1 as the divider of PLL1CLK.
  * Selects C_ISO_VDCE0CLK/C_ISO_VDCE1CLK as DOTCLK0/DOTCLK1 in case of LVTTL/Serial RGB/VODDR.
  * Selects C_ISO_VDCE0CLK/C_ISO_VDCE1CLK as the divider of DOTCLK0/DOTCLK1 in case of OpenLDI.
  * Sets VODDR0SYSCNT, VODDR0CLKDIV register in case of VODDR.
  * Sets CKSC_IVOEXS_CTL depending on VOSL field of VDCECTL.

  <D1M1-V2, D1M1(H), D1L2(H)>
  * Selects DOTCLK0 as the divider of PLL0CLK.
  * Selects C_ISO_VDCE0CLK as DOTCLK0.

  Customizing Points:
  * If you use LVTTL output, NOT necessary to modify this function in general use-case.
  * If you use OPENLDI output, please define USE_VDCE_OPENLDI as 1.
  * If you use VODDR output, please define USE_VDCE_VODDR as 1.
  * If you use Serial RGB output, please define USE_VDCE_SERIALRGB as 1.
  * If you use Serial RGB output with Quadruple Speed (x4), please define USE_VDCE_SERIALRGB_SPEED
    as 4.
  * If you want to change the clock settings, please modify this function.
    Please refer H/W UM Figure 37.10 ~ 37.14.

  Parameter:
  Unit  -           Instance number
  Clock -           This parameter specifies the wanted pixel clock.
                    The pixel clock that is really set can differ from
                    this value due to hardware restrictions.
  OtherUnitActive - VDCE has maximum two units. If the opposite unit (Unit XOR 1) is in one of
                    the states R_VDCE_STATE_IDLE or R_VDCE_STATE_EXECUTING, set this flag to '1'.
                    Set it to '0' if the other Unit is R_VDCE_STATE_UNINITIALIZED or 
                    R_VDCE_STATE_INITIALIZED.
                    If the opposite unit does not exist, set it to '0'.
                    Used to enable VODDR if both Units are configured and running.
  Flags           - Display setting Flags. See <r_ddb_TimingFlags_t>.

  Return Value:
  non-zero         - Pixel clock really set.
  0               - Error occurred. 
*/
uint32_t R_VDCE_Sys_PixelClockSet(const uint32_t Unit,
                                  const uint32_t Clock,
                                  const uint32_t OtherUnitActive,
                                  const uint32_t Flags);

/***************************************************************************
  Function: R_VDCE_Sys_IntcInit

  Description:
  This function initializes interrupt controller (INTC) for specified VDCE unit.
  This function is called from <R_VDCE_Init>. 

  This function executes the following processing.
  * Clears and Enables the VDCE interrupts of INTC except for VDCE0GR3VBLANK / VDCE1GR3VBLANK.

  Note:
  VDCE0GR3VBLANK and VDCE1GR3VBLANK are not level interrupt, so these are enabled with 
  individual control.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  Unit       - Instance number

  Return Value:
  see: <r_vdce_Error_t>
*/
r_vdce_Error_t R_VDCE_Sys_IntcInit(const uint32_t Unit);

/***************************************************************************
  Function: R_VDCE_Sys_IntcDeinit

  Description:
  This function de-initializes interrupt controller (INTC) for specified VDCE unit.
  This function is called from <R_VDCE_DeInit>.

  This function executes the following processing.
  * Clears and Disables the VDCE interrupts of INTC.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  Unit       - Instance number

  Return Value:
  see: <r_vdce_Error_t>
*/
r_vdce_Error_t R_VDCE_Sys_IntcDeinit(const uint32_t Unit);

/***************************************************************************
  Function: R_VDCE_Sys_IntcEnable

  Description:
  This function enables the specified interrupt of interrupt controller (INTC).
  This function is called from <R_VDCE_IntcEnable>.

  This function executes the following processing.
  * Clears and Enables the specified interrupts of INTC.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  Unit   -    Instance number
  IntSel -    Interrupt type

  Return Value:
  See <r_vdce_Error_t> 
*/
r_vdce_Error_t R_VDCE_Sys_IntcEnable(const uint32_t       Unit,
                                     const r_dev_IntSel_t IntSel);

/***************************************************************************
  Function: R_VDCE_Sys_IntcDisable

  Description:
  This function disables the specified interrupt of interrupt controller (INTC).
  This function is called from <R_VDCE_IntcDisable>.

  This function executes the following processing.
  * Clears and Disables the specified interrupts of INTC.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  Unit   -    Instance number
  IntSel -    Interrupt type

  Return Value:
  See <r_vdce_Error_t> 
*/
r_vdce_Error_t R_VDCE_Sys_IntcDisable(const uint32_t       Unit,
                                      const r_dev_IntSel_t IntSel);

/***************************************************************************
  Function: R_VDCE_Sys_IntcPrepareClearCheck

  Description:
  This function prepares the interrupt clear checking.
  This function is called from <R_VDCE_IntcEnable>.

  This function executes the following processing.
  * Calculates the waiting count corresponding to 4Cycle of specified synchronization clock.
  * Waits for INT_STAx to change 0->1 by NOP instruction.
  * Returns the time-out count of register polling (INT_STAx to change 1->0).

  Customizing Points:
  * It is NOT necessary to modify this function in general use-case.
  * If you use external video input (capture), please set minimum VIN clock to LOC_SYNC_FREQ_MIN.
  LOC_SYNC_FREQ_MIN also can be small enough like default setting.
  * You may change LOC_CPU_FREQ_MAX definition depending on the target device.

  Parameter:
  SyncFreq - It specifies the frequency of synchronization clock.
             If 0 is specified, the value defined as LOC_SYNC_FREQ_MIN is used.

  Return Value:
  Time-out count of register polling.
*/

uint32_t R_VDCE_Sys_IntcPrepareClearCheck(const uint32_t SyncFreq);

/***************************************************************************
  Function: R_VDCE_Sys_IntcClearCheck

  Description:
  This function waits until the interrupt is cleared.
  This function is called from <R_VDCE_Isr>.

  This function executes the following processing.
  * Waits for EICn.EIRFn to change 1->0.

  Customizing Points:
  * It is NOT necessary to modify this function in general use-case.
  * If you want to change the time-out count, please modify the definition of LOC_WAIT_CLEAR_COUNT.

  Parameter:
  Unit    - Instance number.
  IntType - Interrupt type.

  Return Value:
  See <r_vdce_Error_t> 
*/
r_vdce_Error_t R_VDCE_Sys_IntcClearCheck(const uint32_t          Unit,
                                         const r_vdce_IntType_t IntcType);

/***************************************************************************
  Function: R_VDCE_Sys_PortInit

  Description:
  This function initializes Port for specified VDCE unit.
  This function is called from <R_VDCE_Init>.

  This function is empty as default.

  Customizing Points:
  * It is NOT necessary to modify this function in general use-case.
  * You may add the port setting in this function.

  Parameter:
  Unit       - Instance number

  Return Value:
  see: <r_vdce_Error_t>
*/
r_vdce_Error_t R_VDCE_Sys_PortInit(const uint32_t Unit);

/***************************************************************************
  Function: R_VDCE_Sys_PortDeInit

  Description:
  This function de-initializes Port for specified VDCE unit.
  This function is called from <R_VDCE_DeInit>.

  This function is empty as default.

  Customizing Points:
  * It is NOT necessary to modify this function in general use-case.
  * You may add the port setting in this function.

  Parameter:
  Unit -    Instance number

  Return Value:
  See <r_vdce_Error_t> 
*/
r_vdce_Error_t R_VDCE_Sys_PortDeInit(const uint32_t Unit);

/***************************************************************************
  Function: R_VDCE_Sys_HsyncActLevelSet

  Description:
  This function sets output Hsync signal level.
  This function is called from <R_VDCE_DeInit>, <R_VDCE_DisplayEnable> and
  <R_VDCE_DisplayTimingAdjust>. 

  If this function returns 0, VDCE macro will handle the Hsync level.

  If this function returns 1, VDCE macro will not handle the Hsync level.
  VDCE outputs Hsync with active high.

  This function is empty and returns 0 as default.

  Customizing Points:
  * It is NOT necessary to modify this function in general use-case.
  * If you want to handle Hsync level outside VDCE (e.g. Port), please implement to this function
    and returns 1.

  Parameter:
  Unit       - Instance number
  Level      - Hsync level (0: Active Low, 1: Active High)

  Return Value:
  0          - This function doesn't handle Hsync level.
  1          - This function handles Hsync level.
*/
uint32_t R_VDCE_Sys_HsyncActLevelSet(const uint32_t Unit, const int32_t Level);

/***************************************************************************
  Function: R_VDCE_Sys_VsyncActLevelSet

  Description:
  This function sets output Vsync signal level.
  This function is called from <R_VDCE_DeInit>, <R_VDCE_DisplayEnable> and
  <R_VDCE_DisplayTimingAdjust>. 

  If this function returns 0, VDCE macro will handle the Vsync level.
  
  If this function returns 1, VDCE macro will not handle the Vsync level.
  VDCE outputs Vsync with active high.

  This function is empty and returns 0 as default.

  Customizing Points:
  * It is NOT necessary to modify this function in general use-case.
  * If you want to handle Vsync level outside VDCE (e.g. Port), please implement to this function
    and returns 1.

  Parameter:
  Unit       - Instance number
  Level      - Vsync level (0:Active Low, 1:Active High)

  Return Value:
  0          - This function doesn't handle Vsync level.
  1          - This function handles Vsync level.
*/
uint32_t R_VDCE_Sys_VsyncActLevelSet(const uint32_t Unit, const int32_t Level);

/***************************************************************************
  Function: R_VDCE_Sys_ClockActEdgeSet

  Description:
  This function sets VO_DATA[23:0] change timing.
  This function is called from <R_VDCE_DeInit>, <R_VDCE_DisplayEnable> and
  <R_VDCE_DisplayTimingAdjust>. 

  If this function returns 0, VDCE macro will handle the VO_DATA[23:0] change timing.

  If this function returns 1, VDCE macro will not handle the VO_DATA[23:0] change timing.
  VDCE outputs VO_DATA[23:0] with rising edge of clock.

  This function executes the following processing.
  * Sets PINV45_0 and returns 1 in case of VDCE0 LVTTL mode.
  * Sets PINV47_8 and returns 1 in case of VDCE1 LVTTL mode.
  * Does nothing and returns 1 in case of RSDS mode.

  Customizing Points:
  * It is NOT necessary to modify this function in general use-case.
  * If you want to change the handling, please modify this function.

  Parameter:
  Unit       - Instance number
  Flags      - Display setting Flags. See <r_ddb_TimingFlags_t>
               If R_DDB_DISP_FLAG_NEGCLK flag is off, data change with rising edge of clock.
               If R_DDB_DISP_FLAG_NEGCLK flag is on, data change with falling edge of clock.

  Return Value:
  0          - This function doesn't handle data change timing. 
  1          - This function handles data change timing.
*/
uint32_t R_VDCE_Sys_ClockActEdgeSet(const uint32_t Unit,
                                    const uint32_t Flags);

/***************************************************************************
  Function: R_VDCE_Sys_DesyncActLevelSet

  Description:
  This function sets output DE (Data Enable) signal level.
  This function is called from <R_VDCE_DeInit>, <R_VDCE_DisplayEnable> and
  <R_VDCE_DisplayTimingAdjust>. 

  If this function returns 0, VDCE macro will handle the DE signal level.

  If this function returns 1, VDCE macro will not handle the DE signal level.
  VDCE outputs DE signal with active high.

  This function is empty and returns 0 as default.

  Customizing Points:
  * It is NOT necessary to modify this function in general use-case.
  * If you want to handle DE signal level outside VDCE (e.g. Port), please implement to this
    function and returns 1.

  Parameter:
  Unit       - Instance number
  Level      - DE signal level (0:Active Low, 1:Active High)

  Return Value:
  0          - This function doesn't handle DE signal level. 
  1          - This function handles DE signal level.
*/
uint32_t R_VDCE_Sys_DesyncActLevelSet(const uint32_t Unit, const int32_t Level);

/***************************************************************************
  Function: R_VDCE_Sys_StateSet

  Description:
  This function set state to each of or all layers. 
  This function is called from several VDCE APIs.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  
  Parameter:
  Unit    - It specifies the video output number.
  LayerNr - It specifies the layer number to use.
  State   - The state of VDCE driver functions.

  Return Value:
  See <r_vdce_Error_t>
*/
r_vdce_Error_t R_VDCE_Sys_StateSet(const uint32_t       Unit,
                                   const uint32_t       LayerNr,
                                   const r_vdce_State_t State);

/***************************************************************************
  Function: R_VDCE_Sys_StateGet

  Description:
  This function is used to get the state of VDCE driver. 
  This function is called from several VDCE APIs.
  In case of all layers, this function returns state based on the order of priority.
  This function puts a priority of state on Executing, Idle, Initialized and Uninitialized.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  
  Parameter:
  Unit    - It specifies the video output number.
  LayerNr - It specifies the layer number to use.

  Return value:
  See <r_vdce_State_t>.
*/
r_vdce_State_t R_VDCE_Sys_StateGet(const uint32_t       Unit,
                                   const uint32_t       LayerNr);

/***************************************************************************
  Function: R_VDCE_Sys_InitGlobal
  
  Description:
  Initializes the global variables in VDCE porting layer.
  
  If R_BSP_SYS_INIT_USE is defined, user must call this function before calling VDCE APIs. 
  This function is called from R_DEV_SysInit provided as sample code. 
  
  If R_BSP_SYS_INIT_USE is not defined, global variables are declared with initial values. 
  This function call is not mandatory.
  
  This function executes the following processing:  
  * Initialize global variables.
  
  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  
  Parameter:
  None.
    
  Return value:
  None.
*/
void R_VDCE_Sys_InitGlobal(void);

/***************************************************************************
  Group: VDCE driver OS interface functions
*/

/***************************************************************************
  Function: R_VDCE_Sys_Lock

  Description:
  This function locks the VDCE driver access to the specified unit for other threads.

  This function is empty as default.

  Customizing Points:
  Please implement the lock process by mutex or semaphore if VDCE API is called from
  multi-thread.

  Parameter:
  Unit       - Instance number

  Return Value:
  see: <r_vdce_Error_t>
*/
r_vdce_Error_t R_VDCE_Sys_Lock(const uint32_t Unit);

/***************************************************************************
  Function: R_VDCE_Sys_Unlock

  Description:
  This function unlocks the VDCE driver access to the specified unit for other threads.

  This function is empty as default.

  Customizing Points:
  Please implement the unlock process depending on <R_VDCE_Sys_Lock>.

  Parameter:
  Unit       - Instance number

  Return Value:
  see: <r_vdce_Error_t>
*/
r_vdce_Error_t R_VDCE_Sys_Unlock(const uint32_t Unit);

/***************************************************************************
  Function: R_VDCE_Sys_AllLock

  Description:
  This function locks the VDCE driver access to the all unit for other threads.
  This function is called from following functions.
  * <R_VDCE_ErrorCallbackSet>
  * <R_VDCE_CapEnable>
  * <R_VDCE_CapDisable>

  This function executes the following processing.
  * Call <R_VDCE_Sys_Lock> to all available unit.

  Customizing Points:
  Please implement the lock process by mutex or semaphore if VDCE API is called from
  multi-thread.

  Parameter:
  None.

  Return Value:
  see: <r_vdce_Error_t>
*/
r_vdce_Error_t R_VDCE_Sys_AllLock(void);

/***************************************************************************
  Function: R_VDCE_Sys_AllUnlock

  Description:
  This function unlocks the VDCE driver access to the all unit for other threads.
  This function is called from following functions.
  * <R_VDCE_ErrorCallbackSet>
  * <R_VDCE_CapEnable>
  * <R_VDCE_CapDisable>

  This function executes the following processing.
  * Call <R_VDCE_Sys_Unlock> to all available unit.

  Customizing Points:
  Please implement the unlock process depending on <R_VDCE_Sys_AllLock>.

  Parameter:
  None.

  Return Value:
  see: <r_vdce_Error_t>
*/
r_vdce_Error_t R_VDCE_Sys_AllUnlock(void);

/***************************************************************************
  Function: R_VDCE_Sys_VIChannelCheck

  Description:
  This function returns the availability of specified video input channel.
  This function is called from R_VDCE_Cap~ APIs.

  This function executes the following processing.
  * Returns R_VDCE_ERR_NOT_ACCEPTABLE if target device does not support the Video input.
  * Returns R_VDCE_ERR_RANGE_UNIT if specified unit does not support the Video input.
  * Returns R_VDCE_ERR_OK if target device of specified unit supports the Video input.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  Unit       - Instance number of Video input

  Return Value:
  see: <r_vdce_Error_t>
*/
r_vdce_Error_t R_VDCE_Sys_VIChannelCheck(const uint32_t Unit);

/***************************************************************************
  Function: R_VDCE_Sys_MaxResolutionGet

  Description:
  This function returns the resolution information.
  This function is called from <R_VDCE_DisplayTimingSet>.

  This function executes the following processing.
  * Stores the maximum video output resolution of VDCE macro depending on the target device.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  ResolutionHmax - This is a pointer to the max horizontal resolution.
  ResolutionVmax - This is a pointer to the max vertical resolution.

  Return Value:
  see: <r_vdce_Error_t>
*/
r_vdce_Error_t R_VDCE_Sys_MaxResolutionGet(uint32_t *ResolutionHmax,
                                           uint32_t *ResolutionVmax);

/***************************************************************************
  Function: R_VDCE_Sys_DeviceInfoGet

  Description:
  This function returns the device information.
  This function is called from several VDCE APIs.

  This function executes the following processing.
  * Stores the VDCE macro information to DevInfo depending on the target device.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  DevInfo - This is a pointer to information that is different from each devices.
            See <r_vdce_DeviceInfo_t>.

  Return Value:
  see: <r_vdce_Error_t>
*/
r_vdce_Error_t R_VDCE_Sys_DeviceInfoGet(r_vdce_DeviceInfo_t *DevInfo);

#ifdef __cplusplus
}
#endif

#endif /* R_VDCE_SYS_H */

