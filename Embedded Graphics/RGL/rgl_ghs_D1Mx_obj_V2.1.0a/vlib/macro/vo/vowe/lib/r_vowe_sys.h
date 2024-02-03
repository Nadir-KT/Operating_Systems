/*
****************************************************************************
PROJECT : VOWE driver
============================================================================
DESCRIPTION
VOWE(Video Output Warping Engine) support functions
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

/***************************************************************************
  Title: VOWE Driver Support Functions

  VOWE (Video Output Warping Engine) driver support functions

  The generic VOWE driver uses these functions. They have to be implemented 
  within the driver library for a concrete device. (e.g. D1L, D1M)

  Support functions are functions that are not part of the driver itself 
  but they must be provided to integrate the driver on a particular board.
*/

#ifndef R_VOWE_SYS_H
#define R_VOWE_SYS_H

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
  Section: Global defines
*/
/***************************************************************************
  Group: Constants
*/

/***************************************************************************
  Constants: Source height margin

  This definition specifies the margin of source height.

  Values:
  R_VOWE_SOURCE_HEIGHT_MARGIN  - This definition specifies the margin of source height.

*/
#define R_VOWE_SOURCE_HEIGHT_MARGIN (4)


/***************************************************************************
  Group Enumeration and types
*/

/***************************************************************************
  Enum: r_vowe_Status_t
  Description:
  This type describes the VOWE driver state.

  Values:
  R_VOWE_STATE_DEINIT        - VOWE driver in de-initialized state.
  R_VOWE_STATE_INIT          - VOWE driver in initialized state.
  R_VOWE_STATE_IDLE          - VOWE driver in idle state.
  R_VOWE_STATE_EXEC          - VOWE driver in executing state.
*/
typedef enum
{
    R_VOWE_STATE_DEINIT = 0,
    R_VOWE_STATE_INIT,
    R_VOWE_STATE_IDLE,
    R_VOWE_STATE_EXEC
} r_vowe_Status_t;


/***************************************************************************
  Section: Global Functions
*/
/***************************************************************************
  Group: VOWE driver Basic interface functions
*/
/***************************************************************************
  Function: R_VOWE_Sys_Init

  Description:
  This function initializes environment-dependent part.
  This function is called from <R_VOWE_Init> function.

  This function executes the following processing.
  * Adjusts display timing (calls R_VDCE_DisplayTimingAdjust() function).

  Customizing Points:
  Please adjusts the Vlines parameter of R_VDCE_DisplayTimingAdjust() function.

  Parameter:
  Unit            - Instance number.

  Return value:
  R_VOWE_ERR_OK              - No error has occurred.
  R_VOWE_ERR_RANGE_UNIT      - Unit number is out of range.
  R_VOWE_ERR_FATAL_HW        - Fatal error has occurred at H/W.

  See <r_vowe_Error_t>.
*/
r_vowe_Error_t R_VOWE_Sys_Init(const uint32_t Unit);


/***************************************************************************
  Function: R_VOWE_Sys_DeInit

  Description:
  This function de-initializes environment-dependent part.
  This function is called from <R_VOWE_DeInit> function.

  This function is empty as default.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  If you want to add the de-initialization code of environment-depend (e.g. clock control), 
  please implement to this function.

  Parameter:
  Unit            - Instance number.

  Return value:
  R_VOWE_ERR_OK              - No error has occurred.
  R_VOWE_ERR_RANGE_UNIT      - Unit number is out of range.
  R_VOWE_ERR_FATAL_HW        - Fatal error has occurred at H/W.

  See <r_vowe_Error_t>.
*/
r_vowe_Error_t R_VOWE_Sys_DeInit(const uint32_t Unit);


/***************************************************************************
  Function: R_VOWE_Sys_InterruptEnable

  Description:
  This function enables the interrupt request.
  This function is called from <R_VOWE_Start> function.

  This function executes the following processing.
  * Enables the VOWEINT interrupt.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  Unit            - Instance number.

  Return value:
  none
*/
void R_VOWE_Sys_InterruptEnable(const uint32_t Unit);


/***************************************************************************
  Function: R_VOWE_Sys_InterruptDisable

  Description:
  This function disables the interrupt request.
  This function is called from the callback function of VOWE.

  This function executes the following processing.
  * Disables the VOWEINT interrupt.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  Unit            - Instance number.

  Return value:
  none
*/
void R_VOWE_Sys_InterruptDisable(const uint32_t Unit);


/***************************************************************************
  Function: R_VOWE_Sys_BaseAddrGet

  Description:
  This function gives back the base address of VOWE H/W macro's register.
  This function is called from several VOWE APIs.

  This function executes the following processing.
  * Returns the base address of VOWE H/W macro's register.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  Unit            - Instance number.

  Return value:
  Macro base address.
*/
uint32_t R_VOWE_Sys_BaseAddrGet(const uint32_t Unit);


/***************************************************************************
  Function: R_VOWE_Sys_ExtBaseAddrGet

  Description:
  This function gives back the base address of extra register.
  This function is called from several VOWE APIs.

  This function executes the following processing.
  * Returns the base address of extra register.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  Unit            - Instance number.

  Return value:
  Macro extra base address.
*/
uint32_t R_VOWE_Sys_ExtBaseAddrGet(const uint32_t Unit);


/***************************************************************************
  Group: VOWE driver VDCE interface functions
*/
/***************************************************************************
  Function: R_VOWE_Sys_VDCEnable

  Description:
  This function make to enables the VDCE's feature for VOWE driver and allocates 
  the VDCE's resources for VOWE driver.
  This function is called from <R_VOWE_Open>.

  This function executes the following processing.
  * Sets base address by <R_VDCE_OirBaseSet>
  * Sets buffer mode by <R_VDCE_OirRingBufferEnable> or <R_VDCE_OirRingBufferDisable>.
  * Sets delay line by <R_VDCE_OirVSyncDelaySet> in case of ring buffer mode.
  * Sets stride by <R_VDCE_OirMemGeometrySet>.
  * Sets image width and height (add 4 lines to the actual height) by <R_VDCE_OirViewPortSet>. 
  * Sets output color format by <R_VDCE_OirFormatSet>.
  * Sets destination mode by <R_VDCE_OirModeSet>.
  * Starts the Output Image Generator block (OIR) by <R_VDCE_OirEnable>.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  Unit            - Instance number.
  WorkBufferMode  - Work buffer mode. See <r_vowe_BufferMode_t>
  VdceBufferAddr  - Pointer to the top address of the work buffer to set to VDCE.
  VdceBufferSize  - The size of work buffer.
  Stride          - Stride of output image.
  ColorFormat     - Color format of output image. See <r_vowe_ColorFormat_t>.
  SourceWidth     - Source image width (pixel).
  SourceHeight    - Source image height (pixel).
  RingBufferDelay - Delay line of ring buffer. 
  DestMode        - Destination mode. 

  Return value:
  R_VOWE_ERR_OK              - No error has occurred.
  R_VOWE_ERR_RANGE_UNIT      - The unit-number error.
  R_VOWE_ERR_SYS_VDCE        - The error at the SYS function of VDC.

  See <r_vowe_Error_t>.
*/
r_vowe_Error_t R_VOWE_Sys_VDCEnable(const uint32_t               Unit,
                                    const r_vowe_BufferMode_t    WorkBufferMode,
                                    const uint8_t*         const VdceBufferAddr,
                                    const uint32_t               VdceBufferSize,
                                    const uint32_t               Stride,
                                    const r_vowe_ColorFormat_t   ColorFormat,
                                    const uint32_t               SourceWidth,
                                    const uint32_t               SourceHeight,
                                    const uint32_t               RingBufferDelay,
                                    const r_vowe_DestMode_t      DestMode);


/***************************************************************************
  Function: R_VOWE_Sys_VDCDisable

  Description:
  This function makes to disables the VDCE's feature for VOWE driver and releases
  the VDCE's resources for VOWE driver.
  This function is called from <R_VOWE_Close>.

  This function executes the following processing.
  * Stops the Output Image Generator block (OIR) by <R_VDCE_OirDisable>.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  Unit            - Instance number.

  Return value:
  R_VOWE_ERR_OK              - No error has occurred.
  R_VOWE_ERR_RANGE_UNIT      - The unit-number error.
  R_VOWE_ERR_SYS_VDC         - The error at the SYS function of VDC.

  See <r_vowe_Error_t>.
*/
r_vowe_Error_t R_VOWE_Sys_VDCDisable(const uint32_t Unit);

/***************************************************************************
  Function: R_VOWE_Sys_StateSet
  
  Description:
  This function is used to change the state of VOWE driver
  
  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  
  Parameter:
  Unit            - Instance number.
  State           - Specifies the required state of VOWE driver.

  Return Value:
  R_VOWE_ERR_OK              - No error has occurred.
  R_VOWE_ERR_RANGE_UNIT      - The unit-number error.

  See <r_vowe_Status_t> <r_vowe_Error_t>
*/
r_vowe_Error_t R_VOWE_Sys_StateSet(const uint32_t    Unit, const r_vowe_Status_t State);
/**************************************************************************
  Function: R_VOWE_Sys_StateGet
  
  Description:
  This function is used to get the state of VOWE driver

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  
  Parameter:
  Unit            - Instance number.

  Return Value:
  R_VOWE_STATE_DEINIT  - This is a status that the VOWE driver is not initialized.
  R_VOWE_STATE_INIT    - This is a status that the VOWE driver is initialized and 
                         all the internal variables are set to default values.
  R_VOWE_STATE_IDLE    - This is a status when all the configuration and callback data of VOWE for 
                         warping correction has been set and no warping have occurred yet.
  R_VOWE_STATE_EXEC -    This is a status that the VOWE driver is performing a warping. 

  see <r_vowe_Status_t>
*/
r_vowe_Status_t R_VOWE_Sys_StateGet(const uint32_t Unit);

/***************************************************************************
  Function: R_VOWE_Sys_InitGlobal
  
  Description:
  Initializes the global variables in VOWE porting layer.
  
  If R_BSP_SYS_INIT_USE is defined, user must call this function before calling VOWE APIs. 
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
void R_VOWE_Sys_InitGlobal(void);

/***************************************************************************
  Group: VOWE driver OS interface functions
*/
/***************************************************************************
  Function: R_VOWE_Sys_Lock

  Description:
  This function locks the VOWE driver access to the specified unit for other threads.
  This function is called from several VOWE APIs.

  This function is empty as default.
  
  Customizing Points:
  Please implement the lock process by mutex or semaphore if VOWE API is called from 
  multi-thread.

  Parameter:
  Unit            - Instance number.

  Return value:
  R_VOWE_ERR_OK              - No error has occurred.
  R_VOWE_ERR_RANGE_UNIT      - The unit-number error.
  R_VOWE_ERR_FATAL_OS        - Fatal error has occurred at OS interface.

  See <r_vowe_Error_t>.
*/
r_vowe_Error_t R_VOWE_Sys_Lock(const uint32_t Unit);


/***************************************************************************
  Function: R_VOWE_Sys_Unlock

  Description:
  This function unlocks the VOWE driver access to the specified unit for other threads.
  This function is called from several VOWE APIs.

  This function is empty as default.

  Customizing Points:
  Please implement the unlock process depending on <R_VOWE_Sys_Lock>.

  Parameter:
  Unit            - Instance number.

  Return value:
  R_VOWE_ERR_OK              - No error has occurred.
  R_VOWE_ERR_RANGE_UNIT      - The unit-number error.
  R_VOWE_ERR_FATAL_OS        - Fatal error has occurred at OS interface.

  See <r_vowe_Error_t>.
*/
r_vowe_Error_t R_VOWE_Sys_Unlock(const uint32_t Unit);



#ifdef __cplusplus
}
#endif

#endif /* R_VOWE_SYS_H */

