/*
**************************************************************************** 
PROJECT : JCUA driver
============================================================================ 
DESCRIPTION
JCUA support functions
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
  Title: JCUA Driver support functions

  JCUA (JPEG Codec Unit A) driver support functions

  The generic JCUA driver uses these functions. They have to be implemented 
  within the driver library for a concrete device. (e.g. D1L, D1M)

  Support functions are functions that are not part of the driver itself 
  but they must be provided to integrate the driver on a particular board.
*/

#ifndef R_JCUA_SYS_H
#define R_JCUA_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
  Section: Global Types
*/
/***************************************************************************
  typedef: r_jcua_State_t

  This enumerator indicates a main status of JCUA driver.

  R_JCUA_STATE_UNINITIALIZED          - This is a status that the JCUA driver is not initialized.
  R_JCUA_STATE_INITIALIZED            - This is a status that the operation mode of JCUA driver is
                                        not set after the JCUA driver is initialized.
  R_JCUA_STATE_IDLE                   - This is a status that the operation mode of JCUA drive is
                                        set to a decoding mode. The decoding is not performed yet.
  R_JCUA_STATE_EXECUTING              - This is a status that the JCUA driver is performing a 
                                        decoding.
  R_JCUA_STATE_PAUSED                 - This is a status that a pause caused by input division or 
                                        output division is occurring after JCUA driver performed a 
                                        decoding. When a decoding is restarted, the status transition 
                                        to a decoding
                                        status occurs.
  R_JCUA_STATE_UNKNOWN                - This is the status when the JCUA unit number is invalid.
*/

typedef enum
{
    R_JCUA_STATE_UNINITIALIZED        = 0x00,
    R_JCUA_STATE_INITIALIZED          = 0x01,
    R_JCUA_STATE_IDLE                 = 0x02,
    R_JCUA_STATE_EXECUTING            = 0x03,
    R_JCUA_STATE_PAUSED               = 0x04
} r_jcua_State_t;

/***************************************************************************
  Section: Global Functions
*/
/***************************************************************************
  Group: JCUA driver Basic interface functions
*/

/***************************************************************************
  Function: R_JCUA_Sys_Init

  Description:
  This function initializes environment-dependent part.
  This function is called from <R_JCUA_Init>.

  This function executes the following processing.

  * Bus reset for JCUA H/W macro (refer to OPC(Operating Precautions) 
    item #12 "JCUA reset operation").
  >         -> JCCMD.BRST must not be used. (JCU macro internal reset not sufficient)
  >         -> JCSWRST.JCUA0RES must be used instead. (JCU macro external reset)
  >            Refer to D1x UM section 44.6 "Software Reset Processing".
  >              Use the following software reset procedure to reset the JCUA unit:
  >              0. Wait for 30us.
  >              1. Set JCSWRST.JCUA0RES bit to 1, to generate the software reset condition.
  >              2. Read JCSWRST.JCUA0RES bit until bit is set to 1, to wait for software
                 reset is active.
  >              3. Set JCSWRST.JCUA0RES bit to 0, to release software reset condition.
  >              By this sequence all JCUA registers with exception of the JCSWRST register 
                 are initialized by a software reset.

  * Enable interrupt for JCUA H/W macro.
  > Call R_JCUA_Sys_InterruptEnable(Unit)

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  But this function is using the TICK driver or OSTM driver for wait process. 
  If you don't want to use TICK driver or OSTM driver, please modify wait process.

  Parameter:
  Unit     - Instance number.
  
  Return value:
  R_JCUA_ERR_OK               - No error has occurred.
  R_JCUA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_JCUA_ERR_FATAL_HW         - Fatal error has occurred at H/W.

  See <r_jcua_Error_t>. 
*/

r_jcua_Error_t R_JCUA_Sys_Init(const uint32_t Unit);

/***************************************************************************
  Function: R_JCUA_Sys_DeInit

  Description:
  This function de-initializes environment-dependent part.
  This function is called from <R_JCUA_DeInit>.

  This function executes the following processing.

  * Disable interrupt for JCUA H/W macro.
  > Call R_JCUA_Sys_InterruptDisable(Unit)

  * Close the H/W timer for the wait process. (if necessary)

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  But this function is using the TICK driver or OSTM driver for wait process. 
  If you don't want to use TICK driver or OSTM driver, please modify wait process.

  Parameter:
  Unit     - Instance number.
  
  Return value:
  R_JCUA_ERR_OK               - No error has occurred.
  R_JCUA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_JCUA_ERR_FATAL_HW         - Fatal error has occurred at H/W.

  See <r_jcua_Error_t>. 
*/

r_jcua_Error_t R_JCUA_Sys_DeInit(const uint32_t Unit);

/***************************************************************************
  Function: R_JCUA_Sys_InterruptEnable

  Description:
  This function is enable interrupt request.
  This function is called from <R_JCUA_DecoderStart> and <R_JCUA_DecoderContinue>.

  This function executes the following processing.
  * Enables the JEDI and JDTI interrupt.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  Unit     - Instance number.
  
  Return value:
  None. 
*/
void R_JCUA_Sys_InterruptEnable(const uint32_t Unit);

/***************************************************************************
  Function: R_JCUA_Sys_InterruptDisable

  Description:
  This function is disable interrupt request.
  This function is called from several JCUA APIs.

  This function executes the following processing.
  * Disables the JEDI and JDTI interrupt.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  Unit     - Instance number.
  
  Return value:
  None.
*/
void R_JCUA_Sys_InterruptDisable(const uint32_t Unit);

/***************************************************************************
  Function: R_JCUA_Sys_BaseAddrGet

  Description:
  This function gives back the base address of JCUA H/W macro's register.
  This function is called from several JCUA APIs.

  This function executes the following processing.
  * Returns the base address of JCUA H/W macro's register.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.

  Parameter:
  Unit     - Instance number.

  Return value:
  uint32_t - base address of macro instance (Unit).
*/
uint32_t R_JCUA_Sys_BaseAddrGet(const uint32_t Unit);
   
/***************************************************************************
  Function: R_JCUA_Sys_TimerStart

  Description:
  This function starts time-out measurement.
  This function is called from <R_JCUA_DecoderStart> and <R_JCUA_IsrStop>.
  
  This function executes the following processing.
  * Starts the timer for time-out measurement.
  * Calls <R_JCUA_IsrTimeOut> calllback function when time-out occurs. (This process is executed 
    by loc_OstmCallback.)

  Note:
  Please note that this function is called from <R_JCUA_IsrStop>.
  This may be called from ISR depending on the system.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  But this function is using the OSTM driver for time-out measurement. 
  If you don't want to use OSTM driver, please modify time-out measurement process.

  Parameter:
  Unit     - Instance number.
  IsHeader - The parameter specified the decoding part. 
              R_TRUE  : JPEG Header part 
              R_FALSE : JPEG Image data part

  Return value:
  R_JCUA_ERR_OK               - No error has occurred.
  R_JCUA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_JCUA_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  R_JCUA_ERR_FATAL_HW         - Fatal error has occurred at H/W.

  See <r_jcua_Error_t>. 
*/
r_jcua_Error_t R_JCUA_Sys_TimerStart(const uint32_t Unit, const uint32_t IsHeader);
    
/***************************************************************************
  Function: R_JCUA_Sys_TimerStop

  Description:
  This function stops time-out measurement.
  This function is called from <R_JCUA_IsrFinish> and <R_JCUA_IsrStop>.

  This function executes the following processing.
  * Stops the timer for time-out measurement.

  Note:
  Please note that this function is called from <R_JCUA_IsrFinish> and <R_JCUA_IsrStop>.
  These may be called from ISR depending on the system.
  
  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  But this function is using the OSTM driver for time-out measurement. 
  If you don't want to use OSTM driver, please modify time-out measurement process.

  Parameter:
  Unit     - Instance number.

  Return value:
  R_JCUA_ERR_OK               - No error has occurred.
  R_JCUA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_JCUA_ERR_FATAL_HW         - Fatal error has occurred at H/W.

  See <r_jcua_Error_t>. 
*/
r_jcua_Error_t R_JCUA_Sys_TimerStop(const uint32_t Unit);
   
/***************************************************************************
  Function: R_JCUA_Sys_TimerPause

  Description:
  This function suspends time-out measurement (pause).
  This function stores the remaining counter until time-out.
  This function is called from <R_JCUA_IsrFinish>.

  This function executes the following processing.
  * Pauses the timer for time-out measurement.

  Note:
  Please note that this function is called from <R_JCUA_IsrFinish>.
  This may be called from ISR depending on the system.
  
  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  But this function is using the OSTM driver for time-out measurement. 
  If you don't want to use OSTM driver, please modify time-out measurement process.

  Parameter:
  Unit     - Instance number.

  Return value:
  R_JCUA_ERR_OK               - No error has occurred.
  R_JCUA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_JCUA_ERR_FATAL_HW         - Fatal error has occurred at H/W.

  See <r_jcua_Error_t>. 
*/
r_jcua_Error_t R_JCUA_Sys_TimerPause(const uint32_t Unit);
   
/***************************************************************************
  Function: R_JCUA_Sys_TimerResume

  Description:
  This function resumes time-out measurement.
  The time-out value is the remaining count value stored at <R_JCUA_Sys_TimerPause>.
  This function is called from <R_JCUA_DecoderContinue>.

  This function executes the following processing.
  * Resumes the timer for time-out measurement.

  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  But this function is using the OSTM driver for time-out measurement. 
  If you don't want to use OSTM driver, please modify time-out measurement process.

  Parameter:
  Unit     - Instance number.

  Return value:
  R_JCUA_ERR_OK               - No error has occurred.
  R_JCUA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_JCUA_ERR_FATAL_HW         - Fatal error has occurred at H/W.

  See <r_jcua_Error_t>. 
*/
r_jcua_Error_t R_JCUA_Sys_TimerResume(const uint32_t Unit);

/***************************************************************************
  Function: R_JCUA_Sys_StateSet
  
  Description:
  This function is used to change the state of JCUA driver
  
  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  
  Parameters:
  Unit     - Instance number
  state    - requested state of JCUA driver
  
  Return value:
  R_JCUA_ERR_OK               - No error has occurred
  
  See <r_jcua_Error_t>
*/ 
r_jcua_Error_t R_JCUA_Sys_StateSet(const uint32_t Unit, const r_jcua_State_t State);

/***************************************************************************
  Function: R_JCUA_Sys_StateGet
  
  Description:
  This function is used to get the state of JCUA driver
  
  Customizing Points:
  It is NOT necessary to modify this function in general use-case.
  
  Parameter:
  Unit     - Instance number
  
  Return value:
  R_JCUA_STATE_UNINITIALIZED  - JCUA driver is not initialized.
  R_JCUA_STATE_INITIALIZED    - Decompression mode of JCUA driver is not set after the JCUA 
                                driver is initialized.
  R_JCUA_STATE_IDLE           - Decompression mode of JCUA drive is set to a decoding mode. 
                                The decoding is not performed yet.
  R_JCUA_STATE_EXECUTING      - JCUA driver is performing a decoding.
  R_JCUA_STATE_PAUSED         - A pause caused by input division or output division is 
                                occurring after JCUA driver performed a decoding

  see <r_jcua_State_t>
*/
r_jcua_State_t R_JCUA_Sys_StateGet(const uint32_t Unit);

/***************************************************************************
  Function: R_JCUA_Sys_InitGlobal
  
  Description:
  Initializes the global variables in JCUA porting layer.
  
  If R_BSP_SYS_INIT_USE is defined, user must call this function before calling JCUA APIs. 
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
void R_JCUA_Sys_InitGlobal(void);

/***************************************************************************
  Group: JCUA driver OS interface functions
*/

/***************************************************************************
  Function: R_JCUA_Sys_Lock

  Description:
  This function locks the JCUA driver access to the specified unit from other threads.
  This function is called from several JCUA APIs.
  
  This function is empty as default.

  Customizing Points:
  Please implement the lock process by mutex or semaphore if JCUA API is called from 
  multi-thread.

  Parameter:
  Unit     - Instance number.
  
  Return value:
  R_JCUA_ERR_OK               - No error has occurred.
  R_JCUA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_JCUA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.

  See <r_jcua_Error_t>. 
*/

r_jcua_Error_t R_JCUA_Sys_Lock(const uint32_t Unit);

/***************************************************************************
  Function: R_JCUA_Sys_Unlock

  Description:
  This function unlocks the JCUA driver access to the specified unit from other threads.
  This function is called from several JCUA APIs.

  This function is empty as default.

  Customizing Points:
  Please implement the unlock process depending on <R_JCUA_Sys_Lock>.

  Parameter:
  Unit     - Instance number.
  
  Return value:
  R_JCUA_ERR_OK               - No error has occurred.
  R_JCUA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_JCUA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.

  See <r_jcua_Error_t>. 
*/

r_jcua_Error_t R_JCUA_Sys_Unlock(const uint32_t Unit);

#ifdef __cplusplus
}
#endif

#endif /* R_JCUA_SYS_H */

