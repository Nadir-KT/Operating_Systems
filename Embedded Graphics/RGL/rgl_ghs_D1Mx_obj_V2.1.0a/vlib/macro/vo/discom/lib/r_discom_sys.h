/*
****************************************************************************
PROJECT : DISCOM driver
============================================================================
DESCRIPTION
DISCOM support functions
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
  Title: DISCOM Driver Support Functions

  DISCOM (Display Output Comparator) driver support functions

  The generic DISCOM driver uses these functions. They have to be implemented
  within the driver library for a concrete device. (e.g. RH850/D1M)

  Support functions are functions that are not part of the driver itself
  but they must be provided to integrate the driver on a particular board.

*/

#ifndef R_DISCOM_SYS_H
#define R_DISCOM_SYS_H

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
  Enum: r_discom_State_t

  Description:
  This type describes the state of DISCOM.

  Values:
  R_DISCOM_STATE_UNINITIALIZED - The DISCOM driver is in uninitialized state.
  R_DISCOM_STATE_INITIALIZED   - The DISCOM driver is in initialized state.
  R_DISCOM_STATE_IDLE          - The DISCOM driver is in idle state.
  R_DISCOM_STATE_EXECUTING     - The DISCOM driver is in executing state.
*/
typedef enum
{
    R_DISCOM_STATE_UNINITIALIZED = 0,
    R_DISCOM_STATE_INITIALIZED,
    R_DISCOM_STATE_IDLE,
    R_DISCOM_STATE_EXECUTING
} r_discom_State_t;

/***************************************************************************
  Section: Global Functions
*/

/***************************************************************************
  Group: DISCOM driver support functions
*/

/***************************************************************************
  Function: R_DISCOM_Sys_Init

  Description:
  This function initializes environment-dependent part.
  This function is called from <R_DISCOM_Init>.

  This function is empty as default.

  Customizing Points:
  * It is NOT necessary to modify this function in general use-case.
  * If you want to add the de-initialization code of environment-depend (e.g. clock control),
    implement to this function.

  Parameter:
  Unit       - Instance number

  Return Value:
  see: <r_discom_Error_t>
*/
r_discom_Error_t R_DISCOM_Sys_Init(const uint32_t Unit);

/***************************************************************************
  Function: R_DISCOM_Sys_DeInit

  Description:
  This function de-initializes environment-dependent part.
  This function is called from <R_DISCOM_DeInit>.

  This function is empty as default.

  Customizing Points:
  * It is NOT necessary to modify this function in general use-case.
  * If you want to add the de-initialization code of environment-depend (e.g. clock control),
    please implement to this function.

  Parameter:
  Unit       - Instance number

  Return Value:
  see: <r_discom_Error_t>
*/
r_discom_Error_t R_DISCOM_Sys_DeInit(const uint32_t Unit);

/***************************************************************************
  Function: R_DISCOM_Sys_InterruptEnable

  Description:
  This function enables interrupt request.
  This function is called from <R_DISCOM_IntEnable>.

  This function is empty as default.

  Customizing Points:
  * It is not necessary to modify this function in general use-case.

  Parameter:
  Unit       - Instance number

  Return Value:
  None.
*/
void R_DISCOM_Sys_InterruptEnable(const uint32_t Unit);

/***************************************************************************
  Function: R_DISCOM_Sys_InterruptDisable

  Description:
  This function disables interrupt request.
  This function is called from <R_DISCOM_IntDisable>.

  This function is empty as default.

  Customizing Points:
  * It is not necessary to modify this function in general use-case.

  Parameter:
  Unit       - Instance number

  Return Value:
  None.
*/
void R_DISCOM_Sys_InterruptDisable(const uint32_t Unit);

/*******************************************************************************
  Function: R_DISCOM_Sys_BaseAddrGet

  This function gives back the base address of DISCOM H/W register.
  This function is called from several DISCOM APIs.

  Customizing Points:
  * It is not necessary to modify this function in general use-case.

  Parameters:
  Unit       - Instance number

  Returns:
  not 0 - Base address of DISCOM H/W register.
      0 - Unit is invalid.
*/
uint32_t R_DISCOM_Sys_BaseAddrGet(const uint32_t Unit);

/*******************************************************************************
  Function: R_DISCOM_Sys_StateSet

  This function is used to change the state of DISCOM driver.
  DISCOM driver requires managing 1 status per unit.
  This function is called from several APIs.

  Customizing Points:
  * It is not necessary to modify this function in general use-case.

  Parameters:
  Unit       - Instance number
  State      - The state of DISCOM driver functions.

  Returns:
  see: <r_discom_Error_t>
*/
r_discom_Error_t R_DISCOM_Sys_StateSet(const uint32_t         Unit,
                                       const r_discom_State_t State);

/*******************************************************************************
  Function: R_DISCOM_Sys_StateGet

  This function is used to get the state of DISCOM driver.
  DISCOM driver requires managing 1 status per unit.
  This function is called from several APIs.

  Customizing Points:
  * It is not necessary to modify this function in general use-case.

  Parameters:
  Unit       - Instance number

  Returns:
  see: <r_discom_State_t>
*/
r_discom_State_t R_DISCOM_Sys_StateGet(const uint32_t Unit);

/*******************************************************************************
  Function: R_DISCOM_Sys_InitGlobal

  Initializes the global variables in DISCOM porting layer.
  If R_BSP_SYS_INIT_USE is defined, user must call this function before calling DISCOM APIs.
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
void R_DISCOM_Sys_InitGlobal(void);

/***************************************************************************
  Group: DISCOM driver OS interface functions
*/

/***************************************************************************
  Function: R_DISCOM_Sys_Lock

  Description:
  This function locks the DISCOM driver access to the specified unit for other threads.
  This function is called from several DISCOM APIs.

  This function is empty as default.

  Customizing Points:
  Please implement the lock process by mutex or semaphore if DISCOM API is called from
  multi-thread.

  Parameter:
  Unit       - Instance number

  Return Value:
  see: <r_discom_Error_t>
*/
r_discom_Error_t R_DISCOM_Sys_Lock(const uint32_t Unit);

/***************************************************************************
  Function: R_DISCOM_Sys_Unlock

  Description:
  This function unlocks the DISCOM driver access to the specified unit for other threads.
  This function is called from several DISCOM APIs.

  This function is empty as default.

  Customizing Points:
  Please implement the unlock process depending on <R_DISCOM_Sys_Lock>.

  Parameter:
  Unit       - Instance number

  Return Value:
  see: <r_discom_Error_t>
*/
r_discom_Error_t R_DISCOM_Sys_Unlock(const uint32_t Unit);

#ifdef __cplusplus
}
#endif

#endif /* R_DISCOM_SYS_H */
