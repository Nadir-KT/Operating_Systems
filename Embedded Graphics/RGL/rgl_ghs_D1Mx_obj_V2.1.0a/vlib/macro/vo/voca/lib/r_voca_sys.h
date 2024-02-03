/*
****************************************************************************
PROJECT : VOCA driver
============================================================================
DESCRIPTION
VOCA support functions
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

SAMPLE CODE is not part of the licensed software, as such it must not be used in
mass-production applications. It can only be used for evaluation and
demonstration purposes at customer's premises listed in the signed license
agreement.
****************************************************************************
*/

/***************************************************************************
  Title: VOCA Driver Support Functions

  VOCA (Video Output Checker A) driver support functions

  The generic VOCA driver uses these functions. They have to be implemented
  within the driver library for a concrete device. (e.g. RH850/D1M)

  Support functions are functions that are not part of the driver itself
  but they must be provided to integrate the driver on a particular board.

*/

#ifndef  R_VOCA_SYS_H
#define  R_VOCA_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
  Section: Global Defines
*/

/***************************************************************************
  Section: Global Types
*/

/***************************************************************************
  Group: Enumerations and Types
*/

/****************************************************************************
  Enum: r_voca_State_t

  Description:
  This type describes the state of VOCA.

  Values:
  R_VOCA_STATE_UNINITIALIZED - The VOCA driver is in uninitialized state.
  R_VOCA_STATE_INITIALIZED   - The VOCA driver is in initialized state.
  R_VOCA_STATE_IDLE          - The VOCA driver is in idle state.
  R_VOCA_STATE_EXECUTING     - The VOCA driver is in executing state.
*/
typedef enum
{
    R_VOCA_STATE_UNINITIALIZED = 0,
    R_VOCA_STATE_INITIALIZED,
    R_VOCA_STATE_IDLE,
    R_VOCA_STATE_EXECUTING
} r_voca_State_t;

/***************************************************************************
  Section: Global Functions
*/

/***************************************************************************
  Group: VOCA driver support functions
*/

/***************************************************************************
  Function: R_VOCA_Sys_Init

  Description:
  This function initializes environment-dependent part.
  This function is called from <R_VOCA_Init>.

  This function is empty as default.

  Customizing Points:
  * It is NOT necessary to modify this function in general use-case.
  * If you want to add the de-initialization code of environment-depend (e.g. clock control),
    implement to this function.

  Parameter:
  Unit       - Instance number

  Return Value:
  see: <r_voca_Error_t>
*/
r_voca_Error_t R_VOCA_Sys_Init(const uint32_t Unit);

/***************************************************************************
  Function: R_VOCA_Sys_DeInit

  Description:
  This function de-initializes environment-dependent part.
  This function is called from <R_VOCA_DeInit>.

  This function is empty as default.

  Customizing Points:
  * It is NOT necessary to modify this function in general use-case.
  * If you want to add the de-initialization code of environment-depend (e.g. clock control),
    please implement to this function.

  Parameter:
  Unit       - Instance number

  Return Value:
  see: <r_voca_Error_t>
*/
r_voca_Error_t R_VOCA_Sys_DeInit(const uint32_t Unit);

/***************************************************************************
  Function: R_VOCA_Sys_InterruptEnable

  Description:
  This function enables interrupt request.
  This function is called from <R_VOCA_IntEnable>.

  This function is empty as default.

  Customizing Points:
  * It is not necessary to modify this function in general use-case.

  Parameter:
  Unit       - Instance number

  Return Value:
  None.
*/
void R_VOCA_Sys_InterruptEnable(const uint32_t Unit);

/***************************************************************************
  Function: R_VOCA_Sys_InterruptDisable

  Description:
  This function disables interrupt request.
  This function is called from <R_VOCA_IntDisable>.

  This function is empty as default.

  Customizing Points:
  * It is not necessary to modify this function in general use-case.

  Parameter:
  Unit       - Instance number

  Return Value:
  None.
*/
void R_VOCA_Sys_InterruptDisable(const uint32_t Unit);

/*******************************************************************************
  Function: R_VOCA_Sys_BaseAddrGet

  This function gives back the base address of VOCA H/W register.
  This function is called from several VOCA APIs.

  Customizing Points:
  * It is not necessary to modify this function in general use-case.

  Parameters:
  Unit       - Instance number

  Returns:
  not 0 - Base address of VOCA H/W register.
      0 - Unit is invalid.
*/
uint32_t R_VOCA_Sys_BaseAddrGet(const uint32_t Unit);

/*******************************************************************************
  Function: R_VOCA_Sys_MaxVideoChannelGet

  This function gives back the maximum number of Video channel depending on target device.
  This function is called from several VOCA APIs.

  Customizing Points:
  * It is not necessary to modify this function in general use-case.

  Parameters:
  Unit       - Instance number

  Returns:
  not 0 - The maximum number of Video channel depending on target device.
      0 - Unit is invalid

*/
uint32_t R_VOCA_Sys_MaxVideoChannelGet(const uint32_t Unit);

/*******************************************************************************
  Function: R_VOCA_Sys_StateSet

  This function is used to change the state of VOCA driver.
  VOCA driver requires managing 1 status per unit.
  This function is called from several APIs.

  Customizing Points:
  * It is not necessary to modify this function in general use-case.

  Parameters:
  Unit       - Instance number
  State      - The state of VOCA driver functions.

  Returns:
  see: <r_voca_Error_t>
*/
r_voca_Error_t R_VOCA_Sys_StateSet(const uint32_t         Unit,
                                       const r_voca_State_t State);

/*******************************************************************************
  Function: R_VOCA_Sys_StateGet

  This function is used to get the state of VOCA driver.
  VOCA driver requires managing 1 status per unit.
  This function is called from several APIs.

  Customizing Points:
  * It is not necessary to modify this function in general use-case.

  Parameters:
  Unit       - Instance number

  Returns:
  see: <r_voca_State_t>
*/
r_voca_State_t R_VOCA_Sys_StateGet(const uint32_t Unit);

/*******************************************************************************
  Function: R_VOCA_Sys_InitGlobal

  Initializes the global variables in VOCA porting layer.
  If R_BSP_SYS_INIT_USE is defined, user must call this function before calling VOCA APIs.
  This function is called from R_DEV_SysInit provided as sample code.
  If R_BSP_SYS_INIT_USE is not defined, global variables are declared with initial values.
  This function call is not mandatory.

  This function executes the following processing:  
  * Initialize global variables.

  Customizing Points:
  * It is not necessary to modify this function in general use-case.

  Parameters:
  None.

  Returns:
  None.
*/
void R_VOCA_Sys_InitGlobal(void);

/***************************************************************************
  Group: VOCA driver OS interface functions
*/

/***************************************************************************
  Function: R_VOCA_Sys_Lock

  Description:
  This function locks the VOCA driver access to the specified unit for other threads.
  This function is called from several VOCA APIs.

  This function is empty as default.

  Customizing Points:
  Please implement the lock process by mutex or semaphore if VOCA API is called from
  multi-thread.

  Parameter:
  Unit       - Instance number

  Return Value:
  see: <r_voca_Error_t>
*/
r_voca_Error_t R_VOCA_Sys_Lock(const uint32_t Unit);

/***************************************************************************
  Function: R_VOCA_Sys_Unlock

  Description:
  This function unlocks the VOCA driver access to the specified unit for other threads.
  This function is called from several VOCA APIs.

  This function is empty as default.

  Customizing Points:
  Please implement the unlock process depending on <R_VOCA_Sys_Lock>.

  Parameter:
  Unit       - Instance number

  Return Value:
  see: <r_voca_Error_t>
*/
r_voca_Error_t R_VOCA_Sys_Unlock(const uint32_t Unit);

#ifdef __cplusplus
}
#endif  


#endif /* _R_VOCA_SYS_H */
