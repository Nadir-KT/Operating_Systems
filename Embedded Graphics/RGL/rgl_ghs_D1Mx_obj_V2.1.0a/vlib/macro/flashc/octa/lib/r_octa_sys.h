/*
****************************************************************************
PROJECT : OCTA driver
FILE    : $Id: r_octa_sys.h 15447 2017-12-20 00:25:00Z shinobu.uehara $
============================================================================
DESCRIPTION
OCTA support functions
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2017
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
  Title: OCTA Driver support functions

  OCTA (OctaBus Controller) driver support functions

  The generic OCTA driver uses these functions. They have to be implemented 
  within the driver library for a concrete device. (e.g. D1L, D1M)

  Support functions are functions that are not part of the driver itself 
  but they must be provided to integrate the driver on a particular board.
*/

#ifndef R_OCTA_SYS_H
#define R_OCTA_SYS_H

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
  Section: Global Functions
*/
/***************************************************************************
  Group: OCTA driver Basic interface functions
*/

/***************************************************************************
  Function: R_OCTA_Sys_Init

  Description:
  This function initializes environment-dependent part.
  This function is called from <R_OCTA_Init>.

  This function executes the following processing.
  * Checks whether this LSI supports OCTA function.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  If you want to add the initialization code of environment-depend (e.g. clock control), 
  please implement to this function.

  Parameter:
  Unit - Instance number.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_DEVICE           - OCTA driver is not applicable to this LSI.
  R_OCTA_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  
  See <r_octa_Error_t>.
*/
r_octa_Error_t R_OCTA_Sys_Init(const uint32_t Unit);


/***************************************************************************
  Function: R_OCTA_Sys_DeInit

  Description:
  This function de-initializes environment-dependent part.
  This function is called from <R_OCTA_DeInit>.

  This function is empty as default.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  If you want to add the de-initialization code of environment-depend (e.g. clock control), 
  please implement to this function.

  Parameter:
  Unit - Instance number.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  
  See <r_octa_Error_t>.
*/
r_octa_Error_t R_OCTA_Sys_DeInit(const uint32_t Unit);


/***************************************************************************
  Function: R_OCTA_Sys_BaseAddrGet

  Description:
  This function gives back the base address of OCTA H/W macro's register.
  This function is called from several OCTA APIs.

  This function executes the following processing.
  * Returns the register base address of OCTA H/W macro's register.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  Unit    - Instance number.

  Return value:
  Macro base address.
*/
uint32_t R_OCTA_Sys_BaseAddrGet(const uint32_t Unit);

    
/***************************************************************************
  Function: R_OCTA_Sys_ExternalAddrSpaceBaseAddrGet

  Description:
  This function gives back the base address of external address space.
  This function is called from several OCTA APIs.

  This function executes the following processing.
  * Returns the base address of external address space.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  Unit    - Instance number.
  Channel - Device Channel number.

  Return value:
  base address of external address space.
*/
uint32_t R_OCTA_Sys_ExternalAddrSpaceBaseAddrGet(const uint32_t Unit,
                                                 const uint32_t Channel);

    
/***************************************************************************
  Function: R_OCTA_Sys_DefaultDummyLengthGet

  Description:
  This function gives back the default dummy length.
  This function is called from several OCTA APIs.

  This function executes the following processing.
  * Returns the value of dummy length according to the device type.

  Customizing Points:
  * This function is sample code for MX65L12A64AA/MX25LW51245G/MX66LM1G45G and JSC64SSU8AGDY.
    If you use other OCTA flash or OCTA RAM, it must be changed according to the connected device
    specification.
  * Changes the return value of dummy length according to the connected device.

  Parameter:
  Unit       - Instance number.
  DeviceType - Control device type. 0:Octa Flash, 1:Octa RAM

  Return value:
  default dummy length.
*/
uint32_t R_OCTA_Sys_DefaultDummyLengthGet(const uint32_t Unit,
                                          const uint32_t Channel,
                                          const uint8_t DeviceType);


/***************************************************************************
  Function: R_OCTA_Sys_BankSizeGet

  Description:
  This function gives back the bank byte-size.
  This function is called from <R_OCTA_DataWriteRWW>.

  This function executes the following processing.
  * Returns the value of bank size of Octa flash memory with supporting RWW.
    The bank size must be power-of-two value (2^n).

  Customizing Points:
  * This function is sample code for MX25LW51245G.
    If you use other OCTA flash, it must be changed according to the connected device
    specification.
  * Changes the return value of bank size of Octa flash memory.

  Parameter:
  None

  Return value:
  bank size.
*/
uint32_t R_OCTA_Sys_BankSizeGet(void);


/***************************************************************************
  Function: R_OCTA_Sys_DummyDataRead

  Description:
  This function executes Dummy Data Read.
  This function is called from <R_OCTA_DataWriteRWW>.

  This function executes the following processing.
  * Read size is 32-bit. 
    If Base address is Bank0, read dummy data to other Bank.
    If Base address is other Bank, read dummy data to Bank0.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  Base       - Base address of external address space.
  Addr       - Write address with RWW.
  BankSize   - Bank byte-size of connected Octa flash memory.

  Return value:
  None
*/
void R_OCTA_Sys_DummyDataRead(const uint32_t    Base,
                              const uint32_t    Addr,
                              const uint32_t    BankSize);


/***************************************************************************
  Function: R_OCTA_Sys_ConfigRegSet

  Description:
  This function makes the value to set the configuration register.
  This function is called from <R_OCTA_Open>.

  This function executes the following processing.
  * Reflects specified parameter to register value.

  Customizing Points:
  This function is sample code for MX65L12A64AA/MX25LW51245G/MX66LM1G45G and JSC64SSU8AGDY.
  If you use other OCTA flash or OCTA RAM, it must be changed according to the connected device
  specification.

  Parameter:
  Unit    - Instance number.
  Channel - Device Channel number.
  Config  - Pointer to the <r_octa_Config_t> structure.
  RegVal  - Original value of the configuration register.
  Type    - Configuration Type.

  Return value:
  Set value to the configuration register.
*/
uint16_t R_OCTA_Sys_ConfigRegSet(const uint32_t                  Unit,
                                 const uint32_t                  Channel,
                                 const r_octa_Config_t * const   Config,
                                 const uint16_t                  RegVal,
                                 const r_octa_StateType_t        Type);

    
/***************************************************************************
  Function: R_OCTA_Sys_StatusRegCheck

  Description:
  This function checks the value from the status register each Status Type,
  and it returns the result code.
  This function also judges the time out of status polling.
  This function is called from <R_OCTA_Open>, <R_OCTA_DataProtect>, <R_OCTA_DataErase>,
  <R_OCTA_DataWrite> and <R_OCTA_DataWriteRWW>.


  This function executes the following processing.
  * Checks the calling counter. If it reaches to time-out count, returns R_OCTA_ERR_TIMEOUT. 
  * Checks READY bit. If it shows BUSY state, returns R_OCTA_ERR_NOT_ACCEPTABLE. 
  * Checks Erase / Program Status bit. it shows Successful, returns R_OCTA_ERR_OK. 
  
  Customizing Points:
  * This function is sample code for MX65L12A64AA/MX25LW51245G/MX66LM1G45G and JSC64SSU8AGDY.
    If you use other OCTA flash or OCTA RAM, it must be changed according to the connected device
    specification.
  * If you want to change time-out count, please modify the definition R_OCTA_STATUS_TIMEOUT in 
    r_config_octa.h.
  * This function is called from busy wait. Please implement CPU relaxing to avoid blocking. 

  Parameter:
  Unit    - Instance number.
  Channel - Device Channel number.
  RegPos  - Bit position to check in the status register.
  RegVal  - Value of the status register.
  Type    - Status Type.
  Count   - Call counter.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  R_OCTA_ERR_NOT_ACCEPTABLE   - A process is executing.
  R_OCTA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_OCTA_ERR_PROTECTED        - A process is aborted because of memory protection.
  R_OCTA_ERR_ABORTED          - A process is aborted because of other reason.
  R_OCTA_ERR_TIMEOUT          - Status polling is timeout.
  
  See <r_octa_Error_t>.
*/
r_octa_Error_t R_OCTA_Sys_StatusRegCheck(const uint32_t           Unit, 
                                         const uint32_t           Channel,
                                         const uint16_t           RegPos,
                                         const uint16_t           RegVal,
                                         const r_octa_StateType_t Type,
                                         const uint32_t           Count);

    
/***************************************************************************
  Function: R_OCTA_Sys_StatusRegSet

  Description:
  This function makes the value of status register to set protection mode.

  This function executes the following processing.
  * Makes the value of status register to set protection mode.

  Customizing Points:
  * This function is sample code for MX65L12A64AA/MX25LW51245G/MX66LM1G45G.
    If you use other OCTA flash, it must be changed according to the connected device
    specification.
  * Generates the value of status register which set protection mode according to the connected 
    device specification.

  Parameter:
  Unit    - Instance number.
  Channel - Device Channel number.
  RegPos  - Bit position of the protection mode in the status register.
  RegVal  - Original value of the status register.
  Mode    - Block Protection mode.
            <0> protection mode.
            <1> un-protection mode.

  Return value:
  Set value to the status register.
*/
uint16_t R_OCTA_Sys_StatusRegSet(const uint32_t Unit,
                                 const uint32_t Channel,
                                 const uint16_t RegPos,
                                 const uint16_t RegVal,
                                 const uint8_t  Mode);

    
/***************************************************************************
  Function: R_OCTA_Sys_SyncPeripheral

  Description:
  This function performs synchronization processing to reflect updated
  contents of the control register.
  This function is called from several OCTA APIs.

  This function executes the following processing.
  * SYNCP instruction of RH850 is executed.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  None

  Return value:
  None
*/
void R_OCTA_Sys_SyncPeripheral(void);

    
/***************************************************************************
  Group: OCTA driver OS interface functions
*/

/***************************************************************************
  Function: R_OCTA_Sys_Lock

  Description:
  This function locks the OCTA driver access to the specified unit from other threads.
  This function is called from several OCTA APIs.
  
  This function is empty as default.
  
  Customizing Points:
  Please implement the lock process by mutex or semaphore if OCTA API is called from 
  multi-thread.

  Parameter:
  Unit - Instance number.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  
  See <r_octa_Error_t>.
*/
r_octa_Error_t R_OCTA_Sys_Lock(const uint32_t Unit);


/***************************************************************************
  Function: R_OCTA_Sys_Unlock

  Description:
  This function unlocks the OCTA driver access to the specified unit from other threads.
  This function is called from several OCTA APIs.

  This function is empty as default.

  Customizing Points:
  Please implement the unlock process depending on <R_OCTA_Sys_Lock>.

  Parameter:
  Unit - Instance number.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  
  See <r_octa_Error_t>.
*/
r_octa_Error_t R_OCTA_Sys_Unlock(const uint32_t Unit);


/***************************************************************************
  Function: R_OCTA_Sys_Relax

  Description:
  This function relaxes CPU to avoid blocking.
  This function is called from <R_OCTA_DataErase> and <R_OCTA_DataWrite>.

  This function is empty as default.

  Customizing Points:
  Please implement the relaxing or re-scheduling process if necessary.

  Parameter:
  Unit - Instance number.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  
  See <r_octa_Error_t>.
*/
r_octa_Error_t R_OCTA_Sys_Relax(const uint32_t Unit);


#ifdef __cplusplus
}
#endif

#endif /* R_OCTA_SYS_H */

