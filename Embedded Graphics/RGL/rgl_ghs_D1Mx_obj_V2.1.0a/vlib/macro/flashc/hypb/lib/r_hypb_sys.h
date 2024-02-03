/*
****************************************************************************
PROJECT : HYPB driver
FILE    : $Id: r_hypb_sys.h 14751 2017-09-25 04:27:33Z shinya.tomari $
============================================================================
DESCRIPTION
HYPB support functions
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2016
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
  Title: HYPB Driver support functions

  HYPB (HyperBus Controller) driver support functions

  The generic HYPB driver uses these functions. They have to be implemented 
  within the driver library for a concrete device. (e.g. D1L, D1M)

  Support functions are functions that are not part of the driver itself 
  but they must be provided to integrate the driver on a particular board.
*/

#ifndef R_HYPB_SYS_H
#define R_HYPB_SYS_H

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
  Section: Global Functions
*/
/***************************************************************************
  Group: HYPB driver Basic interface functions
*/

/***************************************************************************
  Function: R_HYPB_Sys_Init

  Description:
  This function initializes environment-dependent part.
  This function is called from <R_HYPB_Init>.
  
  This function executes the following processing.
  * Checks whether this LSI supports HYPB function.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  If you want to add the initialization code of environment-depend (e.g. clock control), 
  please implement to this function.

  Parameter:
  Unit - Instance number.

  Return value:
  R_HYPB_ERR_OK               - No error has occurred.
  R_HYPB_ERR_RANGE_UNIT       - Unit number is out of range.
  R_HYPB_ERR_DEVICE           - HYPB driver is not applicable to target d1x device.
  R_HYPB_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  
  See <r_hypb_Error_t>.
*/
r_hypb_Error_t R_HYPB_Sys_Init(const uint32_t Unit);


/***************************************************************************
  Function: R_HYPB_Sys_DeInit

  Description:
  This function de-initializes environment-dependent part.
  This function is called from <R_HYPB_DeInit>.

  This function is empty as default.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  If you want to add the de-initialization code of environment-depend (e.g. clock control), 
  please implement to this function.

  Parameter:
  Unit - Instance number.

  Return value:
  R_HYPB_ERR_OK               - No error has occurred.
  R_HYPB_ERR_RANGE_UNIT       - Unit number is out of range.
  R_HYPB_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  
  See <r_hypb_Error_t>.
*/
r_hypb_Error_t R_HYPB_Sys_DeInit(const uint32_t Unit);


/***************************************************************************
  Function: R_HYPB_Sys_BaseAddrGet

  Description:
  This function gives back the base address of HYPB H/W macro's register.
  This function is called from several HYPB APIs.

  This function executes the following processing.
  * Returns the base address of HYPB H/W macro's register.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  
  Parameter:
  Unit    - Instance number.

  Return value:
  Macro base address.
*/
uint32_t R_HYPB_Sys_BaseAddrGet(const uint32_t Unit);

/***************************************************************************
  Function: R_HYPB_Sys_ConfigRegSet

  Description:
  This function makes the value to set configuration register.
  This function is called from <R_HYPB_Open>.

  This function executes the following processing.
  * Checks the original configuration register value. 
  >  - If it looks normal value, uses the value as-is.
  >  - If it doesn't look normal value, uses the register default value.
  * Reflects latency code to register value.
  * Reflects parameter-sector settings to register value in case of Hyper Flash.
  * Returns the making register value.

  Customizing Points:
  This function is sample code for (I)S26KL512S/(I)S26KL256S/(I)S26KL128S and IS66WVH16M8BLL.
  If you use other Hyper flash or Hyper RAM, it must be changed according to the connected device
  specification.
  
  Parameter:
  Unit    - Instance number.
  Config  - Pointer to <r_hypb_Config_t> structure.
  RegVal  - Original value of configuration register.

  Return value:
  Set value to configuration register.
*/
uint16_t R_HYPB_Sys_ConfigRegSet(const uint32_t                Unit, 
                                 const r_hypb_Config_t * const Config, 
                                 const uint16_t                RegVal);


/***************************************************************************
  Function: R_HYPB_Sys_StatusRegCheck

  Description:
  This function returns the result code from status register value.
  This function also judges the time out of status polling.
  This function is called from <R_HYPB_DataErase> and <R_HYPB_DataWrite>.

  This function executes the following processing.
  * Checks the calling counter. If it reaches to time-out count, returns R_HYPB_ERR_TIMEOUT. 
  * Checks READY bit. If it shows BUSY state, returns R_HYPB_ERR_NOT_ACCEPTABLE. 
  * Checks Erase / Program Status bit. it shows Successful, returns R_HYPB_ERR_OK. 
  * Checks Sector Lock Status bit. 
  >   - it shows Sector Locked Error, returns R_HYPB_ERR_PROTECTED. 
  >   - it shows Sector Not Locked, returns R_HYPB_ERR_ABORTED. 
  
  Customizing Points:
  * This function is sample code for (I)S26KL512S/(I)S26KL256S/(I)S26KL128S and IS66WVH16M8BLL.
    If you use other Hyper flash or Hyper RAM, it must be changed according to the connected device
    specification.
  * If you want to change time-out count, please modify the definition R_HYPB_STATUS_TIMEOUT in 
    r_config_hypb.h.
  * This function is called from busy wait. Please implement CPU relaxing to avoid blocking. 
  
  Parameter:
  Unit       - Instance number.
  CmdType    - Command type.
  RegVal     - Value of the status register.
  Count      - Call counter.

  Return value:
  R_HYPB_ERR_OK               - No error has occurred.
  R_HYPB_ERR_RANGE_UNIT       - Unit number is out of range.
  R_HYPB_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  R_HYPB_ERR_NOT_ACCEPTABLE   - A process is executing.
  R_HYPB_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_HYPB_ERR_PROTECTED        - A process is aborted because of memory protection.
  R_HYPB_ERR_ABORTED          - A process is aborted because of other reason.
  R_HYPB_ERR_TIMEOUT          - Status polling is timeout.
  
  See <r_hypb_Error_t>.
*/
r_hypb_Error_t R_HYPB_Sys_StatusRegCheck(const uint32_t         Unit,
                                         const r_hypb_CmdType_t CmdType,
                                         const uint16_t         RegVal,
                                         const uint32_t         Count);


/***************************************************************************
  Function: R_HYPB_Sys_SyncPeripheral

  Description:
  This function performs synchronization processing to reflect updated
  contents of the control register.
  This function is called from several HYPB APIs.

  This function executes the following processing.
  * SYNCP instruction of RH850 is executed.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  None

  Return value:
  None
*/
void R_HYPB_Sys_SyncPeripheral(void);

/***************************************************************************
  Group: HYPB driver OS interface functions
*/

/***************************************************************************
  Function: R_HYPB_Sys_Lock

  Description:
  This function locks the HYPB driver access to the specified unit from other threads.
  This function is called from several HYPB APIs.
  
  This function is empty as default.
  
  Customizing Points:
  Please implement the lock process by mutex or semaphore if HYPB API is called from 
  multi-thread.

  Parameter:
  Unit - Instance number.

  Return value:
  R_HYPB_ERR_OK               - No error has occurred.
  R_HYPB_ERR_RANGE_UNIT       - Unit number is out of range.
  R_HYPB_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  
  See <r_hypb_Error_t>.
*/
r_hypb_Error_t R_HYPB_Sys_Lock(const uint32_t Unit);


/***************************************************************************
  Function: R_HYPB_Sys_Unlock

  Description:
  This function unlocks the HYPB driver access to the specified unit from other threads.
  This function is called from several HYPB APIs.

  This function is empty as default.

  Customizing Points:
  Please implement the unlock process depending on <R_HYPB_Sys_Lock>.

  Parameter:
  Unit - Instance number.

  Return value:
  R_HYPB_ERR_OK               - No error has occurred.
  R_HYPB_ERR_RANGE_UNIT       - Unit number is out of range.
  R_HYPB_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  
  See <r_hypb_Error_t>.
*/
r_hypb_Error_t R_HYPB_Sys_Unlock(const uint32_t Unit);


/***************************************************************************
  Function: R_HYPB_Sys_Relax

  Description:
  This function relaxes CPU to avoid blocking.
  This function is called from <R_HYPB_DataWrite> and <R_HYPB_DataRead>.

  This function is empty as default.

  Customizing Points:
  Please implement the relaxing or re-scheduling process if necessary.

  Parameter:
  Unit - Instance number.

  Return value:
  R_HYPB_ERR_OK               - No error has occurred.
  R_HYPB_ERR_RANGE_UNIT       - Unit number is out of range.
  R_HYPB_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  
  See <r_hypb_Error_t>.
*/
r_hypb_Error_t R_HYPB_Sys_Relax(const uint32_t Unit);


#ifdef __cplusplus
}
#endif

#endif /* R_HYPB_SYS_H */

