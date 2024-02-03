/*
****************************************************************************
PROJECT : DISCOM driver
============================================================================
DESCRIPTION
Driver for the DISCOM macro
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

DISCLAIMER

LICENSEE has read, understood and accepted the terms and conditions defined in
the license agreement, especially the usage rights. In any case, it is
LICENSEE's responsibility to make sure that any user of the software complies
with the terms and conditions of the signed license agreement.

****************************************************************************
*/

/***************************************************************************
  Title: DISCOM Driver API

  DISCOM (Display Output Comparator) driver API.

  DISCOM driver controls DISCOM H/W macro and accesses only register of DISCOM H/W macro.

  An application using DISCOM driver should include the following header files.

  * r_typedefs.h
  * r_discom_api.h

*/

#ifndef R_DISCOM_API_H
#define R_DISCOM_API_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
  Section: Global Defines 
*/

/***************************************************************************
  Constants: API Version

  This constant is the value which shows the version information of the DISCOM driver.
  It refers at the <R_DISCOM_VersionStringGet> function.
  High and Low number of the API version

  Values:
  R_DISCOM_VERSION_HI - This constant is the high byte of the version information.
                        It is major version information.
  R_DISCOM_VERSION_LO - This constant is the low byte of the version information.
                        It is miner version information.
*/

#define R_DISCOM_VERSION_HI 1
#define R_DISCOM_VERSION_LO 0

/***************************************************************************
  Constants: Display Output comparison period

  The definition for Display Output comparison.

  Values:
  R_DISCOM_PERIOD_FRAME    - Flame CRC calculation period.
  R_DISCOM_PERIOD_VBLANK   - Vertical blanking period.
*/
#define R_DISCOM_PERIOD_FRAME       (0u)
#define R_DISCOM_PERIOD_VBLANK      (1u)

/***************************************************************************
  Section: Global Types
*/

/***************************************************************************
  Group: Enumerations and Types
*/

/***************************************************************************
  Enum: r_discom_Error_t

  Discription:
  The type describes the error code of DISCOM driver functions.

  Values:
  R_DISCOM_ERR_OK               - No error occurred.
  R_DISCOM_ERR_PARAM_INCORRECT  - A parameter provided to a function was incorrect.
  R_DISCOM_ERR_RANGE_UNIT       - The unit-number was outside the range.
  R_DISCOM_ERR_RANGE_PARAM      - Parameter is the outside the range.
  R_DISCOM_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_DISCOM_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_DISCOM_ERR_FATAL_HW         - Fatal error has occurred at H/W.
*/

typedef enum
{
    R_DISCOM_ERR_OK = 0,
    R_DISCOM_ERR_PARAM_INCORRECT,
    R_DISCOM_ERR_RANGE_UNIT,
    R_DISCOM_ERR_RANGE_PARAM,
    R_DISCOM_ERR_NOT_ACCEPTABLE,
    R_DISCOM_ERR_FATAL_OS,
    R_DISCOM_ERR_FATAL_HW,
    R_DISCOM_ERR_LAST
} r_discom_Error_t;

/***************************************************************************
  Enum: r_discom_Alpha_t

  Discription:
  This type is used to select the alpha value which is used for CRC code comparison.

  Values:
  R_DISCOM_ALPHA_DEFAULT       - Display Out Comparison default alpha value use.
*/
typedef enum
{
    R_DISCOM_ALPHA_DEFAULT = 0
} r_discom_Alpha_t;

/***************************************************************************
typedef: r_discom_Param_t

  Description:
  TThis structure is used to set DISCOM parameters.

  Struct members:
  HOffset    - Horizontal start position of the rectangular area 
               which the CRC code is calculated. 
               Range is 0-1279.
               Horizontal start position value should be smaller than 'ScreenWidth'.
               'ScreenWidth' is the parameter of R_VDCE_DisplayTimingSet function.
  VOffset    - Vertical start position of the rectangular area 
               which the CRC code is calculated.
               Range is 0-1023.
               Vertical start position value should be smaller than 'ScreenHeight'.
               'ScreenHeight' is the parameter of R_VDCE_DisplayTimingSet function.
  HSize      - Horizontal size of the rectangular area for which the CRC code is calculated.
               Range is 1-1280. The value should be set as follows : 
                'ScreenWidth' >= HOffset+ HSize.
  VSize      - Vertical size of the rectangular area for which the CRC code is calculated.
               Range is 1-1024. The value should be set as follows:
                'ScreenHeight' >= VOffset+ VSize.
  AlphaMode  - Alpha mode to use.
  Alpha      - Default alpha value. Specify only '0xFF'.
*/

typedef struct
{
    uint16_t HOffset;
    uint16_t VOffset;
    uint16_t HSize;
    uint16_t VSize;
    r_discom_Alpha_t AlphaMode;
    uint8_t  Alpha;
} r_discom_Param_t;

/***************************************************************************
  Section: Global API Functions
*/

/***************************************************************************
  Group: Basic functions
*/

/***************************************************************************
  Function: R_DISCOM_Init

  Description:
  This function initializes the driver and the hardware as far as necessary.
  If the function successfully execute, the return code will be <R_DISCOM_ERR_OK> and
  the state will be in the Initialized state.
  If user implements <R_DISCOM_Sys_Lock> and <R_DISCOM_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit - It specifies the DISCOM unit number.

  Return value:
  See <r_discom_Error_t>.

  Executable DISCOM Status:
  Uninitialized - OK [Status will be changed to "Initialized".]
  Initialized   - NG
  Idle          - NG
  Executing     - NG

  Supported Device:
  RH850/D1M2(H) - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1A   - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1-V2 - OK (Unit=0 to Unit=1 are available.)
  RH850/D1M1(H) - OK (Unit=0 to Unit=1 are available.)
*/
r_discom_Error_t R_DISCOM_Init(const uint32_t Unit);

/***************************************************************************
  Function: R_DISCOM_DeInit

  Description:
  This function de-initializes the driver and the hardware.
  If the function successfully execute, the return code will be <R_DISCOM_ERR_OK> and
  the state will be in the Uninitialized state.
  If DISCOM unit is already de-initialized status, this function does nothing and
  returns <R_DISCOM_ERR_OK>.
  If user implements <R_DISCOM_Sys_Lock> and <R_DISCOM_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit - It specifies the DISCOM unit number.

  Return value:
  See <r_discom_Error_t>.

  Executable DISCOM Status:
  Uninitialized - OK [Do nothing]
  Initialized   - OK [Status will be changed to "Uninitialized".]
  Idle          - OK [Status will be changed to "Uninitialized".]
  Executing     - NG

  Supported Device:
  RH850/D1M2(H) - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1A   - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1-V2 - OK (Unit=0 to Unit=1 are available.)
  RH850/D1M1(H) - OK (Unit=0 to Unit=1 are available.)
*/
r_discom_Error_t R_DISCOM_DeInit(const uint32_t Unit);

/***************************************************************************
  Function: R_DISCOM_ErrorCallbackSet

  Description:
  This function sets a callback function that is called in case of an error.
  Error notified in this callback can be checked also by return value of each API function,
  so use of callback is not mandatory. The error callback is global for all DISCOM units.
  The error callback is notified during the DISCOM unit is not Uninitialized state.
  The installed error callback can be uninstalled by R_NULL setting in this function.
  And all DISCOM units are deinitialized by R_DISCOM_DeInit, the callback is also uninstalled.
  If user implements <R_DISCOM_Sys_Lock> and <R_DISCOM_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit          - It specifies the DISCOM unit number.
  ErrorCallback - It specifies a function that is called in case an error occurred.
                  Set R_NULL if callback is uninstalled.

  Return value:
  See <r_discom_Error_t>.

  Executable DISCOM Status:
  Uninitialized - NG
  Initialized   - OK
  Idle          - OK
  Executing     - OK

  Supported Device:
  RH850/D1M2(H) - OK
  RH850/D1M1A   - OK
  RH850/D1M1-V2 - OK
  RH850/D1M1(H) - OK
*/
r_discom_Error_t R_DISCOM_ErrorCallbackSet(
                          const uint32_t    Unit, 
                                void(*const ErrorCallback) (const uint32_t  Unit,
                                                            const r_discom_Error_t Error));

/***************************************************************************
  Function: R_DISCOM_ParamSet

  Description:
  This function sets the information of DISCOM configuration parameters.
  If the function successfully execute, the return code will be <R_DISCOM_ERR_OK> and
  the state will be in the Idle state.
  If user implements <R_DISCOM_Sys_Lock> and <R_DISCOM_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit     - It specifies the DISCOM unit number.
  Param    - It specifies the DISCOM configuration parameter.

  Return value:
  See <r_discom_Error_t>.

  Executable DISCOM Status:
  Uninitialized - NG
  Initialized   - OK  [Status will be changed to "Idle".]
  Idle          - OK
  Executing     - NG

  Supported Device:
  RH850/D1M2(H) - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1A   - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1-V2 - OK (Unit=0 to Unit=1 are available.)
  RH850/D1M1(H) - OK (Unit=0 to Unit=1 are available.)
*/
r_discom_Error_t R_DISCOM_ParamSet(const uint32_t                Unit,
                                   const r_discom_Param_t *const Param);

/***************************************************************************
  Function: R_DISCOM_Enable

  Description:
  This function enables the display output comparison start.
  If the function successfully execute, the return code will be <R_DISCOM_ERR_OK> and
  the state will be in the Executing state.
  If user implements <R_DISCOM_Sys_Lock> and <R_DISCOM_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit     - It specifies the DISCOM unit number.

  Return value:
  See <r_discom_Error_t>.

  Executable DISCOM Status:
  Uninitialized - NG
  Initialized   - NG
  Idle          - OK [Status will be changed to "Executing".]
  Executing     - OK

  Supported Device:
  RH850/D1M2(H) - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1A   - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1-V2 - OK (Unit=0 to Unit=1 are available.)
  RH850/D1M1(H) - OK (Unit=0 to Unit=1 are available.)
*/
r_discom_Error_t R_DISCOM_Enable(const uint32_t Unit);

/***************************************************************************
  Function: R_DISCOM_Disable

  Description:
  This function disables the display output comparison.
  If the function successfully execute, the return code will be <R_DISCOM_ERR_OK> and
  the state will be in the Idle state.
  If user implements <R_DISCOM_Sys_Lock> and <R_DISCOM_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit     - It specifies the DISCOM unit number.

  Return value:
  See <r_discom_Error_t>.

  Executable DISCOM Status:
  Uninitialized - NG
  Initialized   - OK
  Idle          - OK
  Executing     - OK [Status will be changed to "Idle".]

  Supported Device:
  RH850/D1M2(H) - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1A   - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1-V2 - OK (Unit=0 to Unit=1 are available.)
  RH850/D1M1(H) - OK (Unit=0 to Unit=1 are available.)
*/
r_discom_Error_t R_DISCOM_Disable(const uint32_t Unit);

/***************************************************************************
  Function: R_DISCOM_StatusGet

  Description:
  This function gets the compare result of the CRC code.
  The compare result of the CRC code can be got after the end of the valid period of Display area.
  See RH850/D1L/D1M Group User's Manual: Hardware for the detail 
  about the timing of the compare result of the CRC code.

  Parameter:
  Unit     - It specifies the DISCOM unit number.
  State    - It specified the pointer to the compare result of the CRC code.
                0 : Compared CRC codes match.
                1 : Compared CRC codes do not match

  Return value:
  See <r_discom_Error_t>.

  Executable DISCOM Status:
  Uninitialized - NG
  Initialized   - OK
  Idle          - OK
  Executing     - OK

  Supported Device:
  RH850/D1M2(H) - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1A   - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1-V2 - OK (Unit=0 to Unit=1 are available.)
  RH850/D1M1(H) - OK (Unit=0 to Unit=1 are available.)
*/
r_discom_Error_t R_DISCOM_StatusGet(const uint32_t Unit, uint32_t *const State);

/***************************************************************************
  Function: R_DISCOM_StatusClear

  Description:
  This function clears the compare result of the CRC code.
  This function processes the error interrupt factor of DISCOM.
  This function doesn't call <R_DISCOM_Sys_Lock> and <R_DISCOM_Sys_Unlock>.
  User should control not to re-enter the same DISCOM unit.

  Parameter:
  Unit     - It specifies the DISCOM unit number.

  Return value:
  See <r_discom_Error_t>.

  Executable DISCOM Status:
  Uninitialized - NG
  Initialized   - OK
  Idle          - OK
  Executing     - OK

  Supported Device:
  RH850/D1M2(H) - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1A   - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1-V2 - OK (Unit=0 to Unit=1 are available.)
  RH850/D1M1(H) - OK (Unit=0 to Unit=1 are available.)
*/
r_discom_Error_t R_DISCOM_StatusClear(const uint32_t Unit);

/***************************************************************************
  Function: R_DISCOM_CrcSet

  Description:
  This function sets the expectation value of the CRC code.
  The CRC code is generated a 32-bit CRC code by using the following CRC polynomial (IEEE802.3).
  
  > x32+ x26+ x23+ x22+ x16+ x12+ x11+ x10+ x8+ x7+ x5+ x4+ x2+ x+1
  
  If the function successfully executes, the return code will be <R_DISCOM_ERR_OK>.
  If user implements <R_DISCOM_Sys_Lock> and <R_DISCOM_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit     - It specifies the DISCOM unit number.
  Crc      - It specifies the Expected CRC code value of the selected graphics data of
             rectangular area.
  Flags    - It specifies the Display Output comparison timing flags. See R_DISCOM_PERIOD_.

  Return value:
  See <r_discom_Error_t>.

  Executable DISCOM Status:
  Uninitialized - NG
  Initialized   - OK
  Idle          - OK
  Executing     - OK

  Supported Device:
  RH850/D1M2(H) - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1A   - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1-V2 - OK (Unit=0 to Unit=1 are available.)
  RH850/D1M1(H) - OK (Unit=0 to Unit=1 are available.)
*/
r_discom_Error_t R_DISCOM_CrcSet(const uint32_t Unit, const uint32_t Crc, const uint32_t Flags);

/***************************************************************************
  Function: R_DISCOM_CrcGet

  Description:
  This function gets the current value of the CRC code.
  The current value of the CRC code can be got after the end of the valid period of Display area.
  See RH850/D1L/D1M Group User's Manual: Hardware for the detail about the timing of
  CRC calculation.
  If the function successfully executes, the return code will be <R_DISCOM_ERR_OK>.

  Parameter:
  Unit     - It specifies the DISCOM unit number.
  Crc      - It specifies the pointer to the current CRC code value of
             the selected graphics data of rectangular area.

  Return value:
  See <r_discom_Error_t>.

  Executable DISCOM Status:
  Uninitialized - NG
  Initialized   - OK
  Idle          - OK
  Executing     - OK

  Supported Device:
  RH850/D1M2(H) - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1A   - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1-V2 - OK (Unit=0 to Unit=1 are available.)
  RH850/D1M1(H) - OK (Unit=0 to Unit=1 are available.)
*/
r_discom_Error_t R_DISCOM_CrcGet(const uint32_t Unit, uint32_t *const Crc);

/***************************************************************************
  Function: R_DISCOM_VersionStringGet

  Description:
  This function returns version string of the DISCOM driver.
  Note that this function can't call from interrupt handler.

  Parameter:
  None.

  Return value:
  Pointer of version string.

  Executable DISCOM Status:
  Uninitialized - OK
  Initialized   - OK
  Idle          - OK
  Executing     - OK

  Supported Device:
  RH850/D1M2(H) - OK
  RH850/D1M1A   - OK
  RH850/D1M1-V2 - OK
  RH850/D1M1(H) - OK
*/
const uint8_t *R_DISCOM_VersionStringGet(void);

/***************************************************************************
  Function: R_DISCOM_VersionStringGet

  Description:
  This function returns the major and minor version of the H/W macro.
  Note that this function can't call from interrupt handler.

  Parameter:
  Major - The macro version as a major version.
  Minor - The macro version as a minor version.

  Return value:
  See <r_discom_Error_t>.

  Executable DISCOM Status:
  Uninitialized - OK
  Initialized   - OK
  Idle          - OK
  Executing     - OK

  Supported Device:
  RH850/D1M2(H) - OK
  RH850/D1M1A   - OK
  RH850/D1M1-V2 - OK
  RH850/D1M1(H) - OK
*/
r_discom_Error_t R_DISCOM_MacroVersionGet(uint32_t *const Major, uint32_t *const Minor);

/***************************************************************************
  Function: R_DISCOM_IntEnable

  Description:
  This function enables DISCOM interrupt.
  If the function successfully executes, the return code will be <R_DISCOM_ERR_OK>.
  If user implements <R_DISCOM_Sys_Lock> and <R_DISCOM_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit     - It specifies the DISCOM unit number.

  Return value:
  See <r_discom_Error_t>.

  Executable DISCOM Status:
  Uninitialized - NG
  Initialized   - OK
  Idle          - OK
  Executing     - OK

  Supported Device:
  RH850/D1M2(H) - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1A   - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1-V2 - OK (Unit=0 to Unit=1 are available.)
  RH850/D1M1(H) - OK (Unit=0 to Unit=1 are available.)
*/
r_discom_Error_t R_DISCOM_IntEnable(const uint32_t Unit);

/***************************************************************************
  Function: R_DISCOM_IntDisable

  Description:
  This function disables DISCOM interrupt.
  If the function successfully executes, the return code will be <R_DISCOM_ERR_OK>.
  If user implements <R_DISCOM_Sys_Lock> and <R_DISCOM_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit     - It specifies the DISCOM unit number.

  Return value:
  See <r_discom_Error_t>.

  Executable DISCOM Status:
  Uninitialized - NG
  Initialized   - OK
  Idle          - OK
  Executing     - OK

  Supported Device:
  RH850/D1M2(H) - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1A   - OK (Unit=0 to Unit=3 are available.)
  RH850/D1M1-V2 - OK (Unit=0 to Unit=1 are available.)
  RH850/D1M1(H) - OK (Unit=0 to Unit=1 are available.)
*/
r_discom_Error_t R_DISCOM_IntDisable(const uint32_t Unit);

#ifdef __cplusplus
}
#endif

#endif /* R_DISCOM_API_H  */
