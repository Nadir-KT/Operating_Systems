/*
****************************************************************************
PROJECT : VOCA driver
============================================================================
DESCRIPTION
Driver for the VOCA macro
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
  Title: VOCA Driver API

  VOCA (Video Output Checker A) driver API.

  VOCA driver controls VOCA H/W macro and accesses only register of VOCA H/W macro.

  An application using VOCA driver should include the following header files.

  * r_typedefs.h
  * r_voca_api.h

*/

#ifndef  R_VOCA_API_H
#define  R_VOCA_API_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
  Section: Global Defines 
*/

/***************************************************************************
  Constants: API Version

  This constant is the value which shows the version information of the VOCA driver.
  It refers at the <R_VOCA_VersionStringGet> function.
  High and Low number of the API version

  Values:
  R_VOCA_VERSION_HI - This constant is the high byte of the version information.
                        It is major version information.
  R_VOCA_VERSION_LO - This constant is the low byte of the version information.
                        It is miner version information.
*/

#define R_VOCA_VERSION_HI  1
#define R_VOCA_VERSION_LO  0

/***************************************************************************
  Constants: Video Output Monitor area reference color range

  The definition of the number of color range Video Output Monitor area.

  Values:
  R_VOCA_REFCOLOR_RANGE_NUM - Number of Video Output Monitor area reference color range.
*/
#define R_VOCA_REFCOLOR_RANGE_NUM  (4u)

/***************************************************************************
  Section: Global Types
*/

/***************************************************************************
  Group: Enumerations and Types
*/

/***************************************************************************
  Enum: r_voca_Error_t

  Discription:
  The type describes the error code of VOCA driver functions.

  Values:
  R_VOCA_ERR_OK                   - No error occurred.
  R_VOCA_ERR_PARAM_INCORRECT      - A parameter provided to a function was incorrect.
  R_VOCA_ERR_RANGE_UNIT           - The unit-number was outside the range.
  R_VOCA_ERR_RANGE_PARAM          - Parameter is the outside the range.
  R_VOCA_ERR_NOT_ACCEPTABLE       - A function was called in an incorrect state.
  R_VOCA_ERR_FATAL_OS             - Fatal error has occurred at OS interface.
  R_VOCA_ERR_FATAL_HW             - Fatal error has occurred at H/W.
  R_VOCA_ERR_LAST                 - Delimiter 
  
*/

typedef enum
{
    R_VOCA_ERR_OK = 0,
    R_VOCA_ERR_PARAM_INCORRECT,
    R_VOCA_ERR_RANGE_UNIT,
    R_VOCA_ERR_RANGE_PARAM,
    R_VOCA_ERR_NOT_ACCEPTABLE,
    R_VOCA_ERR_FATAL_OS,
    R_VOCA_ERR_FATAL_HW,
    R_VOCA_ERR_LAST
} r_voca_Error_t;

/***************************************************************************
  Enum: r_voca_MonitorArea_t

  Discription:
  Selection of Video Monitor Area.

  Values:
  R_VOCA_MONITOR_AREA_0     - Video Output Monitor 0 is assigned.
  R_VOCA_MONITOR_AREA_1     - Video Output Monitor 1 is assigned.
  R_VOCA_MONITOR_AREA_2     - Video Output Monitor 2 is assigned.
  R_VOCA_MONITOR_AREA_3     - Video Output Monitor 3 is assigned.
  R_VOCA_MONITOR_AREA_4     - Video Output Monitor 4 is assigned.
  R_VOCA_MONITOR_AREA_5     - Video Output Monitor 5 is assigned.
  R_VOCA_MONITOR_AREA_6     - Video Output Monitor 6 is assigned.
  R_VOCA_MONITOR_AREA_7     - Video Output Monitor 7 is assigned.
  R_VOCA_MONITOR_AREA_8     - Video Output Monitor 8 is assigned.
  R_VOCA_MONITOR_AREA_9     - Video Output Monitor 9 is assigned.
  R_VOCA_MONITOR_AREA_10    - Video Output Monitor 10 is assigned.
  R_VOCA_MONITOR_AREA_11    - Video Output Monitor 11 is assigned.
  R_VOCA_MONITOR_AREA_12    - Video Output Monitor 12 is assigned.
  R_VOCA_MONITOR_AREA_13    - Video Output Monitor 13 is assigned.
  R_VOCA_MONITOR_AREA_14    - Video Output Monitor 14 is assigned.
  R_VOCA_MONITOR_AREA_15    - Video Output Monitor 15 is assigned.
*/

typedef enum {
    R_VOCA_MONITOR_AREA_0     = 0,
    R_VOCA_MONITOR_AREA_1,
    R_VOCA_MONITOR_AREA_2,
    R_VOCA_MONITOR_AREA_3,
    R_VOCA_MONITOR_AREA_4,
    R_VOCA_MONITOR_AREA_5,
    R_VOCA_MONITOR_AREA_6,
    R_VOCA_MONITOR_AREA_7,
    R_VOCA_MONITOR_AREA_8,
    R_VOCA_MONITOR_AREA_9,
    R_VOCA_MONITOR_AREA_10,
    R_VOCA_MONITOR_AREA_11,
    R_VOCA_MONITOR_AREA_12,
    R_VOCA_MONITOR_AREA_13,
    R_VOCA_MONITOR_AREA_14,
    R_VOCA_MONITOR_AREA_15,
    R_VOCA_MONITOR_AREA_LAST
} r_voca_MonAreaNum_t;


/***************************************************************************
  Enum: r_voca_VoCh_t

  Discription:
  Selection of Video channels.
  
  Video channel is different depending on RH850/D1x device.
  For D1M1(H)and D1M1-V2, only Video channel 0 can be selected.
  For D1M1A and D1M2(H), Video channel 0 or 1 can be selected.
  Video channel 0 is VDCE Unit 0 output. Video channel 1 is VDCE Unit 1 output.

  Values:
  R_VOCA_VO_0      - Video channel 0
  R_VOCA_VO_1      - Video channel 1
*/

typedef enum {
    R_VOCA_VO_0     = 0,
    R_VOCA_VO_1,
    R_VOCA_VO_LAST
} r_voca_VoCh_t;

/***************************************************************************
  typedef: r_voca_Param_t

  Description:
  The type describes the VOCA configuration parameter information.

  Struct members:
  HOffset     - Horizontal back porch offset of the Video channel Display area.
                Range is 1-2047.
  VOffset     - Vertical back porch offset of the Video channel Display area.
                Range is 1-2047.
  HSize       - Horizontal size of the Video channel Display area.
                Range is 1-1280.
  VSize       - Vertical size of the Video channel Display area.
                Range is 1-1024.
*/
typedef struct
{
    uint16_t HOffset;
    uint16_t VOffset;
    uint16_t HSize;
    uint16_t VSize;
} r_voca_Param_t;

/***************************************************************************
  typedef: r_voca_MonRefColor_t

  Description:
  The type describes the Video Output Monitor reference color range.
  Set each component as RGB888 format.

  Struct members:
  RUpper      - Video Output Monitor reference color red upper limit. Range is 0 - 255.
  GUpper      - Video Output Monitor reference color green upper limit. Range is 0 - 255.
  BUpper      - Video Output Monitor reference color blue upper limit. Range is 0 - 255.
  RLower      - Video Output Monitor reference color red lower limit. Range is 0 - 255.
  GLower      - Video Output Monitor reference color green lower limit. Range is 0 - 255.
  BLower      - Video Output Monitor reference color blue lower limit. Range is 0 - 255.
*/
typedef struct
{
    uint8_t RUpper;
    uint8_t GUpper;
    uint8_t BUpper;
    uint8_t RLower;
    uint8_t GLower;
    uint8_t BLower;
} r_voca_MonRefColor_t;

/***************************************************************************
  typedef: r_voca_MonArea_t

  Description:
  The type describes the Video Output Monitor area information.

  Struct members:
  MonAreaNum      - Video Output Monitor area number.Range is 0 - 15.
  MonHStart       - Video Output Monitor area horizontal start position.
                    Range is 0-1279. Horizontal start position value is pixel.
                    Horizontal start position value should be smaller than "HSize".
                    "HSize" is the parameter of R_VOCA_ParamSet function.
  MonVStart       - Video Output Monitor area vertical start position.
                    Range is 0-1023. Vertical start position value is pixel.
                    Vertical start position value should be smaller than "VSize".
                    "VSize" is the parameter of R_VOCA_ParamSet function.
  MonHSize        - Horizontal size of Video Output Monitor area.
                    Range is 1-128. Horizontal start position value is pixel.
                    The value should be set as follows:
                    HSize member of r_voca_Param_t >= MonHStart+ MonHSize.
  MonVSize        - Vertical size of Video Output Monitor area.
                    Range is 1-128. Vertical start position value is pixel.
                    The value should be set as follows:
                    VSize member of r_voca_Param_t >= MonVStart + MonVSize
  RamAddr         - Video Output Monitor reference RAM start address. Range is 0-4095.
  Threshold       - Video Output Monitor m acceptance threshold. Range is 1-262143.
  RefColor        - Video Output Monitor reference color range.
*/
typedef struct
{
    uint16_t MonAreaNum;
    uint16_t MonHStart;
    uint16_t MonVStart;
    uint16_t MonHSize;
    uint16_t MonVSize;
    uint16_t RamAddr;
    uint32_t Threshold;
    r_voca_MonRefColor_t RefColor[R_VOCA_REFCOLOR_RANGE_NUM];
} r_voca_MonArea_t;

/***************************************************************************
  typedef: r_voca_AreaStatus_t

  Description:
  The type describes the status of Video Output Monitor area and Activity Monitor.

  Struct members:
  MonArea        - Video Output Monitor area error information.
>                    0x00000000 : There is no error in Video Output Monitor area
>                    0x00000001 : Video Output Monitor area 0
>                    0x00000002 : Video Output Monitor area 1
>                    0x00000004 : Video Output Monitor area 2
>                    0x00000008 : Video Output Monitor area 3
>                    [...]
>                    0x00001000 : Video Output Monitor area 12
>                    0x00002000 : Video Output Monitor area 13
>                    0x00004000 : Video Output Monitor area 14
>                    0x00008000 : Video Output Monitor area 15
  VoCh           - Video Output Channel error information for Activity Monitor.
>                    0x00000000 : There is no error in Video Output channel
>                    0x00000001 : Video Output channel error 0
>                    0x00000002 : Video Output channel error 1
  SelMon         - The number of the current Video Output Monitor area monitored.
*/
typedef struct {
   uint32_t            MonArea;
   uint32_t            VoCh;
   r_voca_MonAreaNum_t SelMon;
} r_voca_AreaStatus_t;


/***************************************************************************
  Section: Global API functions
*/

/***************************************************************************
  Group: Basic functions
*/

/***************************************************************************
  Function: R_VOCA_Init

  Init VOCA macro unit.
  
  Description:
  This function initializes the driver and the hardware as far as necessary.
  If the function successfully execute, the return code will be <R_VOCA_ERR_OK> and
  the state will be in the Initialized state.

  This function issues a software reset of VOCA unit. So, wait for occurrence of
  interrupt INTVDCE0S0LOVSYNC for VDCE0 in order to the VOCA software reset has been completed.

  If user implements <R_VOCA_Sys_Lock> and <R_VOCA_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.
  
  Parameter:
  Unit - It specifies the VOCA unit number.

  Return value:
  See <r_voca_Error_t>.

  Executable VOCA Status:
  Uninitialized - OK [Status will be changed to "Initialized".]
  Initialized   - NG
  Idle          - NG
  Executing     - NG

  Supported Device:
  RH850/D1M2(H) - OK
  RH850/D1M1A   - OK
  RH850/D1M1-V2 - OK
  RH850/D1M1(H) - OK
*/
r_voca_Error_t R_VOCA_Init(const uint32_t     Unit);

/***************************************************************************
  Function: R_VOCA_DeInit

  Description:
  This function de-initializes the driver and the hardware.
  If the function successfully execute, the return code will be <R_VOCA_ERR_OK> and
  the state will be in the Uninitialized state.
  If VOCA unit is already de-initialized status, this function does nothing and
  returns <R_VOCA_ERR_OK>.
  If user implements <R_VOCA_Sys_Lock> and <R_VOCA_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit - It specifies the VOCA unit number.

  Return value:
  See <r_voca_Error_t>.

  Executable VOCA Status:
  Uninitialized - OK [Do nothing]
  Initialized   - OK [Status will be changed to "Uninitialized".]
  Idle          - OK [Status will be changed to "Uninitialized".]
  Executing     - NG

  Supported Device:
  RH850/D1M2(H) - OK
  RH850/D1M1A   - OK
  RH850/D1M1-V2 - OK
  RH850/D1M1(H) - OK
*/
r_voca_Error_t R_VOCA_DeInit(const uint32_t Unit);

/***************************************************************************
  Function: R_VOCA_ErrorCallbackSet

  Description:
  This function sets a callback function that is called in case of an error.
  Error notified in this callback can be checked also by return value of each API function,
  so use of callback is not mandatory. The error callback is global for all VOCA units.
  The error callback is notified during the VOCA unit is not Uninitialized state.
  The installed error callback can be uninstalled by R_NULL setting in this function.
  And all VOCA units are deinitialized by R_VOCA_DeInit, the callback is also uninstalled.
  If user implements <R_VOCA_Sys_Lock> and <R_VOCA_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit          - It specifies the VOCA unit number.
  ErrorCallback - It specifies a function that is called in case an error occurred.
                  Set R_NULL if callback is uninstalled.

  Return value:
  See <r_voca_Error_t>.

  Executable VOCA Status:
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
r_voca_Error_t R_VOCA_ErrorCallbackSet(const uint32_t Unit, 
                                void(*const ErrorCallback) (const uint32_t  Unit,
                                                            const r_voca_Error_t Error));

/***************************************************************************
  Function: R_VOCA_ParamSet

  Description:
  This function sets the display timing information of horizontal / vertical back porch offset
  and horizontal / vertical size for a Video channel.
  If the function successfully execute, the return code will be <R_VOCA_ERR_OK> and
  the state will be in the Idle state.
  If user implements <R_VOCA_Sys_Lock> and <R_VOCA_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit     - It specifies the VOCA unit number.
  VoCh     - It specifies the Video channel.
  Param    - It specifies the VOCA configuration parameter.

  Return value:
  See <r_voca_Error_t>.

  Executable VOCA Status:
  Uninitialized - NG
  Initialized   - OK [Status will be changed to "Idle".]
  Idle          - OK
  Executing     - NG

  Supported Device:
  RH850/D1M2(H) - OK
  RH850/D1M1A   - OK
  RH850/D1M1-V2 - OK
  RH850/D1M1(H) - OK
*/
r_voca_Error_t R_VOCA_ParamSet(const uint32_t                Unit,
                               const r_voca_VoCh_t           VoCh,
                               const r_voca_Param_t  *const  Param);

/***************************************************************************
  Function: R_VOCA_ActiveMonitorEnable

  Description:
  This function enables Activity Monitor for the specified Video channel.
  MaxTime and MinTime of the parameters set the upper and lower detection time
  for Activity Monitor for Video channel.
  If the function successfully execute, the return code will be <R_VOCA_ERR_OK> and
  the state will be in the Executing state.
  This function works when at least one Video Output Monitor is enabled for
  the respective Video channel.
  If user implements <R_VOCA_Sys_Lock> and <R_VOCA_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit     - It specifies the VOCA unit number.
  VoCh     - It specifies the Video channel.
  MaxTime  - It specifies the upper detection time for Activity Monitor in 0.033 ms units.
              >0: 0 ms
              >1: 0.033 ms
              >2: 0.067 ms
              >3: 0.1 ms
              >...
              >4094: 136.467 ms
  MinTime  = It specifies the lower detection time for Activity Monitor in 0.033 ms units.
              >0: 0 ms
              >1: 0.033 ms
              >2: 0.067 ms
              >3: 0.1 ms
              >...
              >4094: 136.467 
              >4095: 136.5 ms

  Return value:
  See <r_voca_Error_t>.

  Executable VOCA Status:
  Uninitialized - NG
  Initialized   - NG
  Idle          - OK [Status will be changed to "Executing".]
  Executing     - OK

  Supported Device:
  RH850/D1M2(H) - OK
  RH850/D1M1A   - OK
  RH850/D1M1-V2 - OK
  RH850/D1M1(H) - OK
*/
r_voca_Error_t R_VOCA_ActiveMonitorEnable(const uint32_t        Unit,
                                          const r_voca_VoCh_t   VoCh,
                                          const uint16_t        MaxTime,
                                          const uint16_t        MinTime);

/***************************************************************************
  Function: R_VOCA_ActiveMonitorDisable

  Description:
  This function disables Activity Monitor for the specified Video channel.
  If the function successfully executes, the return code will be R_VOCA_ERR_OK.
  VOCA unit status will become Idle state if all Video channels has been disabled
  after the execution of this function.
  If user implements <R_VOCA_Sys_Lock> and <R_VOCA_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit     - It specifies the VOCA unit number.
  VoCh     - It specifies the Video channel.

  Return value:
  See <r_voca_Error_t>.

  Executable VOCA Status:
  Uninitialized - NG
  Initialized   - OK
  Idle          - OK
  Executing     - OK [Status will be changed to "Idle".]

  Supported Device:
  RH850/D1M2(H) - OK
  RH850/D1M1A   - OK
  RH850/D1M1-V2 - OK
  RH850/D1M1(H) - OK
*/
r_voca_Error_t R_VOCA_ActiveMonitorDisable(const uint32_t        Unit,
                                           const r_voca_VoCh_t   VoCh);

/***************************************************************************
  Function: R_VOCA_VideoOutputCheckEnable

  Description:
  This function enables monitoring for the specified Video Output Monitor area.
  If the function successfully execute, the return code will be <R_VOCA_ERR_OK> and
  the state will be in the Executing state.
  If user implements <R_VOCA_Sys_Lock> and <R_VOCA_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit          - It specifies the VOCA unit number.
  MonAreaNum    - It specifies the Video Output Monitor area number.
                  It can be set multiple flags with OR operation.
                   > 0x00000001 : Video Output Monitor area 0
                   > 0x00000002 : Video Output Monitor area 1
                   > 0x00000004 : Video Output Monitor area 2
                   > 0x00000008 : Video Output Monitor area 3
                   > 0x00000010 : Video Output Monitor area 4
                   > 0x00000020 : Video Output Monitor area 5
                   > 0x00000040 : Video Output Monitor area 6
                   > 0x00000080 : Video Output Monitor area 7
                   > 0x00000100 : Video Output Monitor area 8
                   > 0x00000200 : Video Output Monitor area 9
                   > 0x00000400 : Video Output Monitor area 10
                   > 0x00000800 : Video Output Monitor area 11
                   > 0x00001000 : Video Output Monitor area 12
                   > 0x00002000 : Video Output Monitor area 13
                   > 0x00004000 : Video Output Monitor area 14
                   > 0x00008000 : Video Output Monitor area 15

  Return value:
  See <r_voca_Error_t>.

  Executable VOCA Status:
  Uninitialized - NG
  Initialized   - NG
  Idle          - OK [Status will be changed to "Executing".]
  Executing     - OK

  Supported Device:
  RH850/D1M2(H) - OK
  RH850/D1M1A   - OK
  RH850/D1M1-V2 - OK
  RH850/D1M1(H) - OK
*/
r_voca_Error_t R_VOCA_VideoOutputCheckEnable(const uint32_t  Unit,
                                             const uint32_t  MonArea);

/***************************************************************************
  Function: R_VOCA_VideoOutputCheckDisable

  Description:
  This function disables monitoring for the specified Video Output Monitor area.
  If the function successfully executes, the return code will be R_VOCA_ERR_OK.
  VOCA unit status will become Idle state if all Video Output Monitors has been disabled
  after the execution of this function.
  If user implements <R_VOCA_Sys_Lock> and <R_VOCA_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit          - It specifies the VOCA unit number.
  MonAreaNum    - It specifies the Video Output Monitor area number.
                  It can be set multiple flags with OR operation.
                   > 0x00000001 : Video Output Monitor area 0
                   > 0x00000002 : Video Output Monitor area 1
                   > 0x00000004 : Video Output Monitor area 2
                   > 0x00000008 : Video Output Monitor area 3
                   > 0x00000010 : Video Output Monitor area 4
                   > 0x00000020 : Video Output Monitor area 5
                   > 0x00000040 : Video Output Monitor area 6
                   > 0x00000080 : Video Output Monitor area 7
                   > 0x00000100 : Video Output Monitor area 8
                   > 0x00000200 : Video Output Monitor area 9
                   > 0x00000400 : Video Output Monitor area 10
                   > 0x00000800 : Video Output Monitor area 11
                   > 0x00001000 : Video Output Monitor area 12
                   > 0x00002000 : Video Output Monitor area 13
                   > 0x00004000 : Video Output Monitor area 14
                   > 0x00008000 : Video Output Monitor area 15

  Return value:
  See <r_voca_Error_t>.

  Executable VOCA Status:
  Uninitialized - NG
  Initialized   - OK
  Idle          - OK
  Executing     - OK [Status will be changed to "Idle".]

  Supported Device:
  RH850/D1M2(H) - OK
  RH850/D1M1A   - OK
  RH850/D1M1-V2 - OK
  RH850/D1M1(H) - OK
*/
r_voca_Error_t R_VOCA_VideoOutputCheckDisable(const uint32_t  Unit,
                                              const uint32_t  MonArea);

/***************************************************************************
  Function: R_VOCA_StatusGet

  Description:
  This function gets the result of Video Output Monitor area error and Activity Monitor error.
  This function gets the number of the current Video Output Monitor area monitored.

  Parameter:
  Unit     - It specifies the VOCA unit number.
  State    - It specified the pointer to the status of Video Output Monitor area error
             and Activity Monitor error information.

  Return value:
  See <r_voca_Error_t>.

  Executable VOCA Status:
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
r_voca_Error_t R_VOCA_StatusGet(const uint32_t                     Unit,
                                      r_voca_AreaStatus_t   *const State);

/***************************************************************************
  Function: R_VOCA_StatusClear

  Description:
  This function clears the result of Video Output Monitor area error and Activity Monitor error.
  This function processes the error interrupt factor of VOCA.
  This function doesn't call <R_VOCA_Sys_Lock> and <R_VOCA_Sys_Unlock>.
  User should control not to re-enter the same VOCA unit.

  Parameter:
  Unit     - It specifies the VOCA unit number.

  Return value:
  See <r_voca_Error_t>.

  Executable VOCA Status:
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
r_voca_Error_t R_VOCA_StatusClear(const uint32_t      Unit);

/***************************************************************************
  Function: R_VOCA_MonitorAreaSet

  Description:
  This function sets the information of the start position, the size,
  the reference RAM start address, the reference color upper and lower limit value for
  Video Output Monitor area.
  If the function successfully executes, the return code will be <R_VOCA_ERR_OK>.
  If user implements <R_VOCA_Sys_Lock> and <R_VOCA_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit     - It specifies the VOCA unit number.
  VoCh     - It specifies the Video channel.
  MonArea  - It specifies the Video Output Monitor area information.

  Return value:
  See <r_voca_Error_t>.

  Executable VOCA Status:
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
r_voca_Error_t R_VOCA_MonitorAreaSet(const uint32_t                Unit,
                                     const r_voca_VoCh_t           VoCh,
                                     const r_voca_MonArea_t *const MonArea);

/***************************************************************************
  Function: R_VOCA_ColorRamSet

  Description:
  This function sets the reference colors of the 2-bpp CLUT2 indices.
  If the function successfully executes, the return code will be <R_VOCA_ERR_OK>.
  The total number of all monitored areas is up to 32,768 pixels.
  Following equations must be satisfied. Otherwise this function will return error.
  
    > (Offset * 8 + Size) <= 32,768 pixels.
  
  If user implements <R_VOCA_Sys_Lock> and <R_VOCA_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit     - It specifies the VOCA unit number.
  Offset   - It specifies the Video Output reference RAM first address.
             Range is 0 - 4095.
  Size     - It specifies the Video Output Monitor reference RAM size. Unit is pixel.
             Range is 1 - 16384.
  Table    - It specifies the 2-bpp CLUT2 indices of the reference colors.

  Return value:
  See <r_voca_Error_t>.

  Executable VOCA Status:
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
r_voca_Error_t R_VOCA_ColorRamSet(const uint32_t          Unit,
                                  const uint16_t          Offset,
                                  const uint16_t          Size,
                                  const uint16_t   *const Table);

/***************************************************************************
  Function: R_VOCA_VersionStringGet

  Description:
  This function returns version string of the VOCA driver.
  Note that this function can't call from interrupt handler.

  Parameter:
  None.

  Return value:
  Pointer of version string.

  Executable VOCA Status:
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
const uint8_t *R_VOCA_VersionStringGet(void);

/***************************************************************************
  Function: R_VOCA_VersionStringGet

  Description:
  This function returns the major and minor version of the H/W macro.
  Note that this function can't call from interrupt handler.

  Parameter:
  Major - The macro version as a major version.
  Minor - The macro version as a minor version.

  Return value:
  See <r_voca_Error_t>.

  Executable VOCA Status:
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
r_voca_Error_t R_VOCA_MacroVersionGet(uint32_t *const Major, uint32_t *const Minor);

/***************************************************************************
  Function: R_VOCA_IntEnable

  Description:
  This function enables VOCA interrupt.
  If the function successfully executes, the return code will be <R_VOCA_ERR_OK>.
  If user implements <R_VOCA_Sys_Lock> and <R_VOCA_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit     - It specifies the VOCA unit number.

  Return value:
  See <r_voca_Error_t>.

  Executable VOCA Status:
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
r_voca_Error_t R_VOCA_IntEnable(const uint32_t Unit);

/***************************************************************************
  Function: R_VOCA_IntDisable

  Description:
  This function disables VOCA interrupt.
  If the function successfully executes, the return code will be <R_VOCA_ERR_OK>.
  If user implements <R_VOCA_Sys_Lock> and <R_VOCA_Sys_Lock> to prevent multiple executions, 
  this function will become re-entrant.
  Note that this function can't call from interrupt handler.

  Parameter:
  Unit     - It specifies the VOCA unit number.

  Return value:
  See <r_voca_Error_t>.

  Executable VOCA Status:
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
r_voca_Error_t R_VOCA_IntDisable(const uint32_t Unit);

#ifdef __cplusplus
}
#endif


#endif /* _R_VOCA_API_H */

