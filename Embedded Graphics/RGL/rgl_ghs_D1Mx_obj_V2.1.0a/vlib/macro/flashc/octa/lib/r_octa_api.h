/*
****************************************************************************
PROJECT : OCTA driver
FILE    : $Id: r_octa_api.h 15480 2018-01-11 14:07:04Z florian.zimmermann $
============================================================================
DESCRIPTION
Driver for the OCTA(OctaBus Controller) macro
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2021
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
  Title: Octa Driver API

  OCTA (OctaBus Controller) driver API.

  OCTA driver controls OCTA H/W macro and accesses only register of OCTA H/W macro.

  An application using OCTA driver should include the following header files.

  * r_typedefs.h
  * r_octa_api.h

*/

/***************************************************************************
  Section: Definition of a term
*/
/***************************************************************************
  Topics: Word
  1 word is 16bits. Word-address is 16bit address. Word-size is 16bit size. 
  
  Size=1 means 2Byte. Size=2 means 4Byte. 

*/

#ifndef R_OCTA_API_H
#define R_OCTA_API_H

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
  Constants: API Version

  Description:
  This constant is the value which shows the version information of the OCTA driver.
  It refers at the <R_OCTA_VersionStringGet> function.
  
  Values:
  R_OCTA_VERSION_HI - This constant is the high byte of the version information.
                      It is major version information.
  R_OCTA_VERSION_LO - This constant is the low byte of the version information.
                      It is minor version information.
*/
#define R_OCTA_VERSION_HI       (1)
#define R_OCTA_VERSION_LO       (1)

/***************************************************************************
  Constants: Internal definition for Command Option

  Description:
  These flags are used to <Command Option>.
  
  Values:
  R_OCTA_CMD_WRITE        - Use Write transaction. 
  R_OCTA_CMD_READ         - Use Read transaction. 
  R_OCTA_CMD_SIZE_NONE    - Size is none. 
  R_OCTA_CMD_SIZE_8       - 8 bitwise access.
  R_OCTA_CMD_SIZE_16      - 16 bitwise access.
  R_OCTA_CMD_SIZE_MASK    - Mask for size get.

*/
#define R_OCTA_CMD_WRITE        (0x0000uL) 
#define R_OCTA_CMD_READ         (0x8000uL) 
#define R_OCTA_CMD_SIZE_NONE    (0x0000uL) 
#define R_OCTA_CMD_SIZE_8       (0x0001uL) 
#define R_OCTA_CMD_SIZE_16      (0x0002uL) 
#define R_OCTA_CMD_SIZE_MASK    (0x0003uL) 

/***************************************************************************
  Constants: Command Option

  Description:
  OCTA command option flags. These flags are used to OpeFlags(@ <r_octa_CmdTransaction_t>).
  
  Values:
  R_OCTA_CMD_WRITE_NONE - Use Write transaction. No Write size. Only command issue.
  R_OCTA_CMD_WRITE8     - Use Write transaction. 8 bitwise access..
  R_OCTA_CMD_WRITE16    - Use Write transaction. 16 bitwise access.
  R_OCTA_CMD_READ8      - Use Read transaction. 8 bitwise access..
  R_OCTA_CMD_READ16     - Use Read transaction. 16 bitwise access.
  R_OCTA_CMD_NONE       - If the command is not supported, please set to this flag.
*/

#define R_OCTA_CMD_WRITE_NONE  (R_OCTA_CMD_WRITE | R_OCTA_CMD_SIZE_NONE) 
#define R_OCTA_CMD_WRITE8      (R_OCTA_CMD_WRITE | R_OCTA_CMD_SIZE_8) 
#define R_OCTA_CMD_WRITE16     (R_OCTA_CMD_WRITE | R_OCTA_CMD_SIZE_16) 
#define R_OCTA_CMD_READ8       (R_OCTA_CMD_READ  | R_OCTA_CMD_SIZE_8) 
#define R_OCTA_CMD_READ16      (R_OCTA_CMD_READ  | R_OCTA_CMD_SIZE_16) 
#define R_OCTA_CMD_NONE        (0xFFFFuL) 

/***************************************************************************
  Section: Global Types
*/
/***************************************************************************
  Group: Enumerations and Types
*/
/***************************************************************************
  Enum: r_octa_Error_t

  Description:
  OCTA driver error code.

  If an error occurs, these enumerations give information about the reason.

  Values:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_NG               - An error has occurred, but no specific error code
                                is defined for it.
  R_OCTA_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_RANGE_PARAM      - A parameter is out of range.
  R_OCTA_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_OCTA_ERR_DEVICE           - OCTA driver is not applicable to target d1x device.
  R_OCTA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_OCTA_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  R_OCTA_ERR_PROTECTED        - A process is aborted because of memory protection.
  R_OCTA_ERR_TIMEOUT          - Status polling is timeout.
  R_OCTA_ERR_COMMAND          - A command is not supported.
  R_OCTA_ERR_LATENCY          - A latency value is invalid.
*/
typedef enum
{
    R_OCTA_ERR_OK = 0,
    R_OCTA_ERR_NG,
    R_OCTA_ERR_PARAM_INCORRECT,
    R_OCTA_ERR_RANGE_UNIT,
    R_OCTA_ERR_RANGE_PARAM,
    R_OCTA_ERR_NOT_ACCEPTABLE,
    R_OCTA_ERR_DEVICE,
    R_OCTA_ERR_FATAL_OS,
    R_OCTA_ERR_FATAL_HW,
    R_OCTA_ERR_PROTECTED,
    R_OCTA_ERR_TIMEOUT,
    R_OCTA_ERR_COMMAND,
    R_OCTA_ERR_LATENCY
} r_octa_Error_t;


/***************************************************************************
  Enum: r_octa_DeviceType_t

  Description:
  This type describes the control device type.

  Values:
  R_OCTA_DEVICE_FLASH      - Octa Flash. 
  R_OCTA_DEVICE_RAM        - Octa RAM. 
*/
typedef enum
{
    R_OCTA_DEVICE_FLASH = 0,
    R_OCTA_DEVICE_RAM
} r_octa_DeviceType_t;

/***************************************************************************
  Enum: r_octa_OperatingMode_t

  Description:
  This type describes the operating mode.

  Values:
  R_OCTA_MODE_EX_SPACE      - External address space mode.
                              Read/Write access to memory-mapped space is possible for Octa RAM/flash memory.
  R_OCTA_MODE_MANUAL        - Manual mode. 
                              In case of Octa flash, Read/Write access to register area and
                              Write access to memory is possible.
                              In case of Octa RAM, Read/Write access to register area is possible.
*/
typedef enum
{
    R_OCTA_MODE_EX_SPACE = 0,
    R_OCTA_MODE_MANUAL
} r_octa_OperatingMode_t;

/***************************************************************************
  Enum: r_octa_DataTransferMode_t

  Description:
  This type describes the data transfer mode.

  Values:
  R_OCTA_MODE_SPI      - Single I/O STR (Single Transfer Rate), 1 bit per cycle mode. 
                         In case of Octa Flash, Read/Write to memory/register access is possible.
                         In case of Octa RAM, it is not supported.
  R_OCTA_MODE_OPI      - Octa I/O STR (Single Transfer Rate), 8 bit per cycle mode. 
                         In case of Octa Flash, Read/Write to memory/register access is possible.
                         In case of Octa RAM, it is not supported.
  R_OCTA_MODE_DOPI     - Octa I/O DTR (Double Transfer Rate), 16 bit per cycle mode. 
                         In case of Octa Flash, Read/Write to memory/register access is possible.
                         In case of Octa RAM, Read/Write to memory/register access is possible.
*/
typedef enum
{
    R_OCTA_MODE_SPI = 0,
    R_OCTA_MODE_OPI,
    R_OCTA_MODE_DOPI
} r_octa_DataTransferMode_t;

/***************************************************************************
  Enum: r_octa_AddressMode_t

  Description:
  This type describes the format of address output to the Octa flash memory.
  In case of Octa RAM, please set to R_OCTA_ADDRESS_32BIT.

  Values:
  R_OCTA_ADDRESS_24BIT - 24 bit address output.
  R_OCTA_ADDRESS_32BIT - 32 bit address output.
*/
typedef enum
{
    R_OCTA_ADDRESS_24BIT = 0,
    R_OCTA_ADDRESS_32BIT
} r_octa_AddressMode_t;


/***************************************************************************
  Enum: r_octa_ProtectionMode_t

  Description:
  This type describes the protection mode of the Octa flash memory.

  Values:
  R_OCTA_MODE_PROTECT   - Set to the protection mode.
  R_OCTA_MODE_UNPROTECT - Set to the un-protection mode.
*/
typedef enum
{
    R_OCTA_MODE_PROTECT = 0,
    R_OCTA_MODE_UNPROTECT
} r_octa_ProtectionMode_t;


/***************************************************************************
  Enum: r_octa_Reg_t

  Description:
  This type describes the register of the Octa RAM/flash memory.

  Values:
  R_OCTA_STATUS_REG  - Status Register.
  R_OCTA_CONFIG_REG  - Configuration Register.
  R_OCTA_CONFIG2_REG - Configuration2 Register.
  R_OCTA_NONE_REG    - Un used.
*/
typedef enum
{
    R_OCTA_STATUS_REG = 0,
    R_OCTA_CONFIG_REG,
    R_OCTA_CONFIG2_REG,
    R_OCTA_NONE_REG
} r_octa_Reg_t;

/***************************************************************************
  Enum: r_octa_PreCycle_t

  Description:
  This type describes the pre-cycle mode of the Octa RAM/flash memory.
  This type is used when the data transfer mode is R_OCTA_MODE_DOPI.

  Values:
  R_OCTA_PRECYCLE_OFF - pre-cycle mode off.
  R_OCTA_PRECYCLE_ON  - pre-cycle mode on.
*/
typedef enum
{
    R_OCTA_PRECYCLE_OFF = 0,
    R_OCTA_PRECYCLE_ON
} r_octa_PreCycle_t;

/***************************************************************************
  Enum: r_octa_LowPeriod_t

  Description:
  This type describes the setting of the period for issuing command 
  when the device chip select pull down. (from the last SCLK low to CS high)
  Its unit is MCLK cycle.
  In case of DOPI mode, Actual Cycle is set value - 0.5 clk.
  If Low=3 is set, actual delay is 2.5 clock (for DOPI mode).
  (e.g. It is described as tSLCH in data sheet of MX25LW51245G.)

  Values:
  R_OCTA_LOWPERIOD_2  - 2 cycles.
  R_OCTA_LOWPERIOD_3  - 3 cycles.
  R_OCTA_LOWPERIOD_4  - 4 cycles.
  R_OCTA_LOWPERIOD_5  - 5 cycles.
*/
typedef enum
{
    R_OCTA_LOWPERIOD_2 = 0,
    R_OCTA_LOWPERIOD_3,
    R_OCTA_LOWPERIOD_4,
    R_OCTA_LOWPERIOD_5
} r_octa_LowPeriod_t;

/***************************************************************************
  Enum: r_octa_HighPeriod_t

  Description:
  This type describes the setting of the period for the device chip select
  pull up after the command is finished. (from the last SCLK low to CS high)
  Its unit is MCLK cycle.
  In case of DOPI mode, Actual Cycle is set value - 0.5 clk.
  If High=3 is set, actual delay is 2.5 clock (for DOPI mode).
  (e.g. It is described as tCLSH in data sheet of MX25LW51245G.)

  Values:
  R_OCTA_HIGHPERIOD_2  - 2 cycles.
  R_OCTA_HIGHPERIOD_3  - 3 cycles.
  R_OCTA_HIGHPERIOD_4  - 4 cycles.
  R_OCTA_HIGHPERIOD_5  - 5 cycles.
  R_OCTA_HIGHPERIOD_6  - 6 cycles.
  R_OCTA_HIGHPERIOD_7  - 7 cycles.
  R_OCTA_HIGHPERIOD_8  - 8 cycles.
  R_OCTA_HIGHPERIOD_9  - 9 cycles.
*/
typedef enum
{
    R_OCTA_HIGHPERIOD_2 = 0,
    R_OCTA_HIGHPERIOD_3,
    R_OCTA_HIGHPERIOD_4,
    R_OCTA_HIGHPERIOD_5,
    R_OCTA_HIGHPERIOD_6,
    R_OCTA_HIGHPERIOD_7,
    R_OCTA_HIGHPERIOD_8,
    R_OCTA_HIGHPERIOD_9
} r_octa_HighPeriod_t;

/***************************************************************************
  Enum: r_octa_BetweenPeriod_t

  Description:
  This type describes the setting of the period between two commands.
  (from CS high to the next CS low). Its unit is MCLK cycle.
  (e.g. It is described as tSHSL in data sheet of MX25LW51245G.)

  Values:
  R_OCTA_BETWEENPERIOD_2  - 2 cycles.
  R_OCTA_BETWEENPERIOD_5  - 5 cycles.
  R_OCTA_BETWEENPERIOD_7  - 7 cycles.
  R_OCTA_BETWEENPERIOD_9  - 9 cycles.
  R_OCTA_BETWEENPERIOD_11 - 11 cycles.
  R_OCTA_BETWEENPERIOD_13 - 13 cycles.
  R_OCTA_BETWEENPERIOD_15 - 15 cycles.
  R_OCTA_BETWEENPERIOD_17 - 17 cycles.
*/
typedef enum
{
    R_OCTA_BETWEENPERIOD_2 = 0,
    R_OCTA_BETWEENPERIOD_5,
    R_OCTA_BETWEENPERIOD_7,
    R_OCTA_BETWEENPERIOD_9,
    R_OCTA_BETWEENPERIOD_11,
    R_OCTA_BETWEENPERIOD_13,
    R_OCTA_BETWEENPERIOD_15,
    R_OCTA_BETWEENPERIOD_17
} r_octa_BetweenPeriod_t;

/***************************************************************************
  Enum: r_octa_StateType_t

  Description:
  This type describes the state type of the register.

  Values:
  R_OCTA_TYPE_WEL   - State of Write enable latch.
  R_OCTA_TYPE_WIP   - State of Write In Progress.
  R_OCTA_TYPE_BP    - State of Block Protect.
  R_OCTA_TYPE_DTM   - State of Data Transfer mode.
  R_OCTA_TYPE_PCM   - State of Pre-cycle mode.
  R_OCTA_TYPE_DOS   - State of DOS.
  R_OCTA_TYPE_DL    - State of Dummy Length.

*/
typedef enum
{
    R_OCTA_TYPE_WEL = 0,
    R_OCTA_TYPE_WIP,
    R_OCTA_TYPE_BP,
    R_OCTA_TYPE_PCM,
    R_OCTA_TYPE_DTM,
    R_OCTA_TYPE_DOS,
    R_OCTA_TYPE_DL
} r_octa_StateType_t;

/***************************************************************************
  typedef: r_octa_CmdTransaction_t

  Description:
  This type describes the Octa RAM/flash commands.

  Struct members:
  Cmd           - Command for Octa RAM/flash memory.
                  In case of Octa flash memory, Cmd is set 1st byte 
                  for SPI register commands or 1st byte ~ 2nd byte
                  for OPI register commands.
                  In case of Octa RAM, Cmd is set command address.
                  If the command is not supported, please set to 0xFFFF.
  Address       - Address of the command for Octa RAM/flash memory.
                  In case of Octa flash memory, Address is set 3rd byte~6th byte for 
                  OPI register commands.
                  In case of Octa RAM, Address is set Row address and Column address.
                  If the address is not defined, please set to 0.
  CmdLength     - Command length of the command. Its unit is byte.
                  If the command is not supported, please set to 0.
  AddressLength - Address length of the command. Its unit is byte.
                  If the address is not defined, please set to 0.
  DummyLength   - Dummy length of the command. Its unit is clock cycle.
                  The range is 3 to 20.
                  In case of Octa flash memory, Set Dummy Cycle.
                  In case of Octa RAM, Set Latency Counter.
                  If dummy length is not needed, please set to 0.
  DataLength    - Data length of the command.
                  If data length is not needed, please set to 0.
  OpeFlags      - Operation Flags. Multiple flags can be set with '|'. 
                  See definition of <Command Option>. 

*/
typedef struct 
{
    uint16_t                   Cmd;
    uint32_t                   Address;
    uint8_t                    CmdLength;
    uint8_t                    AddressLength;
    uint8_t                    DummyLength;
    uint8_t                    DataLength;
    uint32_t                   OpeFlags;
} r_octa_CmdTransaction_t;


/***************************************************************************
  typedef: r_octa_RegInfo_t

  Description:
  This type describes register information of the Octa RAM/flash memory.

  Struct members:
  Reg             - See <r_octa_Reg_t>
  Address         - Address of the command for Octa RAM/flash memory.
                    In case of Octa flash memory, Address is set 2nd byte~5th byte for 
                    SPI register commands or Address is set 3rd byte~6th byte for 
                    OPI register commands.
                    In case of Octa RAM, Address is set Row address and Column address.
                    If the address is not defined, please set to 0.
  AddressLength   - Address length of the command. Its unit is byte.
                    If the address is not defined, please set to 0.
  BitPosition     - Bit position of status register or configuration register.
*/
typedef struct
{
    r_octa_Reg_t           Reg;
    uint32_t               Address;
    uint8_t                AddressLength;
    uint16_t               BitPosition;
} r_octa_RegInfo_t;

/***************************************************************************
  typedef: r_octa_RegSetParam_t

  Description:
  This type describes register information and set parameter of the Octa RAM/flash memory.

  Struct members:
  Reg             - See <r_octa_Reg_t>
  Address         - Address of the command for Octa RAM/flash memory.
                    In case of Octa flash memory, Address is set 2nd byte~5th byte for 
                    SPI register commands or Address is set 3rd byte~6th byte for 
                    OPI register commands.
                    In case of Octa RAM, Address is set Row address and Column address.
                    If the address is not defined, please set to 0.
  AddressLength   - Address length of the command. Its unit is byte.
                    If the address is not defined, please set to 0.
  BitMask         - Bit mask of status register or configuration register.
  BitSet          - Set point to a register.
*/
typedef struct
{
    r_octa_Reg_t           Reg;
    uint32_t               Address;
    uint8_t                AddressLength;
    uint16_t               BitMask;
    uint16_t               BitSet;
} r_octa_RegSetParam_t;

/***************************************************************************
  typedef: r_octa_Timing_t

  Description:
  This type describes the Chip Select timing.

  Struct members:
  Low     - Set the period for issuing command when the device chip select pull down. 
            See <r_octa_LowPeriod_t>.
  High    - Set the period for the device chip select pull up after the command is finished.
            See <r_octa_HighPeriod_t>.
  Between - Set the period between two commands.
            See <r_octa_BetweenPeriod_t>.
*/

typedef struct
{
    r_octa_LowPeriod_t          Low;
    r_octa_HighPeriod_t         High;
    r_octa_BetweenPeriod_t      Between;
} r_octa_Timing_t;

/***************************************************************************
  typedef: r_octa_Command_t

  Description:
  This type describes the Octa RAM/flash commands.

  Struct members:
  BlockProtect        - Block Protection information.
  WriteEnableLatch    - Write Enable Latch.
  WriteInProgress     - Write in progress information.
  PreCycle            - Pre-cycle enable information.
  FlashTransferType   - Transfer type information of connected Octa flash memory.
  DOS                 - DOS (DQS on STR mode) information. DQS is "Data Strobe Signal".
  DummyCycle          - Dummy cycle setting information.
  ReadTiming          - Read timing setting of connected Octa RAM/flash memory.
                        See <r_octa_Timing_t>.
  WriteTiming         - Write timing setting of connected Octa RAM/flash memory.
                        See <r_octa_Timing_t>.
  CfgTiming           - Configuration timing setting of connected Octa RAM/flash memory.
                        See <r_octa_Timing_t>.
  ReadSPI3B           - Read SPI 3 byte address command.
  ReadSPI4B           - Read SPI 4 byte address command.
  ReadOPI             - Read OPI command.
  ReadDOPI            - Read DTR (Double Transfer Rate) OPI command.
  WriteSPI3B          - Write SPI 3 byte address command.
  WriteSPI4B          - Write SPI 4 byte address command.
  WriteOPI            - Write OPI command.
  WriteDOPI           - Write DTR (Double Transfer Rate) OPI command.
  WriteBufInitialSPI  - Write buffer Initial SPI command.
  WriteBufInitialOPI  - Write buffer Initial OPI command.
  WriteBufContinueSPI - Write buffer Continue SPI command.
  WriteBufContinueOPI - Write buffer Continue OSPI command.
  WriteBufConfirmSPI  - Write buffer Confirm SPI command.
  WriteBufConfirmOPI  - Write buffer Confirm OPI command.
  WriteEnableSPI      - Write Enable SPI command.
  WriteEnableOPI      - Write Enable OPI command.
  EraseSPI3B          - Erase SPI 3 byte address command.
  EraseSPI4B          - Erase SPI 4 byte address command.
  EraseOPI            - Erase OPI command.
  ReadStsSPI          - Read Status SPI command.
  ReadStsOPI          - Read Status OPI command.
  ReadCfgSPI          - Read Configuration SPI command.
  ReadCfgOPI          - Read Configuration OPI command.
  ReadCfg2SPI         - Read Configuration2 SPI command.
  ReadCfg2OPI         - Read Configuration2 OPI command.
  WriteStsCfgSPI      - Write Status / Configuration SPI command.
  WriteStsOPI         - Write Status OPI command.
  WriteCfgOPI         - Write Configuration OPI command.
  WriteCfg2SPI        - Write Configuration2 SPI command.
  WriteCfg2OPI        - Write Configuration2 OPI command.
  ResetEnableSPI      - Reset Enable SPI command.
  ResetEnableOPI      - Reset Enable OPI command.
  ResetSPI            - Reset SPI command.
  ResetOPI            - Reset OPI command.
  ReadIDSPI           - Read ID SPI command.
  ReadIDOPI           - Read ID OPI command.
  
*/

typedef struct 
{
    r_octa_RegInfo_t            BlockProtect;
    r_octa_RegInfo_t            WriteEnableLatch;
    r_octa_RegInfo_t            WriteInProgress;
    r_octa_RegInfo_t            PreCycle;
    r_octa_RegInfo_t            FlashTransferType;
    r_octa_RegInfo_t            DOS;
    r_octa_RegSetParam_t        DummyCycle;
    r_octa_Timing_t             ReadTiming;
    r_octa_Timing_t             WriteTiming;
    r_octa_Timing_t             CfgTiming;
    r_octa_CmdTransaction_t     ReadSPI3B;
    r_octa_CmdTransaction_t     ReadSPI4B;
    r_octa_CmdTransaction_t     ReadOPI;
    r_octa_CmdTransaction_t     ReadDOPI;
    r_octa_CmdTransaction_t     WriteSPI3B;
    r_octa_CmdTransaction_t     WriteSPI4B;
    r_octa_CmdTransaction_t     WriteOPI;
    r_octa_CmdTransaction_t     WriteDOPI;
    r_octa_CmdTransaction_t     WriteBufInitialSPI;
    r_octa_CmdTransaction_t     WriteBufInitialOPI;
    r_octa_CmdTransaction_t     WriteBufContinueSPI;
    r_octa_CmdTransaction_t     WriteBufContinueOPI;
    r_octa_CmdTransaction_t     WriteBufConfirmSPI;
    r_octa_CmdTransaction_t     WriteBufConfirmOPI;
    r_octa_CmdTransaction_t     WriteEnableSPI;
    r_octa_CmdTransaction_t     WriteEnableOPI;
    r_octa_CmdTransaction_t     EraseSPI3B;
    r_octa_CmdTransaction_t     EraseSPI4B;
    r_octa_CmdTransaction_t     EraseOPI;
    r_octa_CmdTransaction_t     ReadStsSPI;
    r_octa_CmdTransaction_t     ReadStsOPI;
    r_octa_CmdTransaction_t     ReadCfgSPI;
    r_octa_CmdTransaction_t     ReadCfgOPI;
    r_octa_CmdTransaction_t     ReadCfg2SPI;
    r_octa_CmdTransaction_t     ReadCfg2OPI;
    r_octa_CmdTransaction_t     WriteStsCfgSPI;
    r_octa_CmdTransaction_t     WriteStsOPI;
    r_octa_CmdTransaction_t     WriteCfgOPI;
    r_octa_CmdTransaction_t     WriteCfg2SPI;
    r_octa_CmdTransaction_t     WriteCfg2OPI;
    r_octa_CmdTransaction_t     ResetEnableSPI;
    r_octa_CmdTransaction_t     ResetEnableOPI;
    r_octa_CmdTransaction_t     ResetSPI;
    r_octa_CmdTransaction_t     ResetOPI;
    r_octa_CmdTransaction_t     ReadIDSPI;
    r_octa_CmdTransaction_t     ReadIDOPI;
} r_octa_Command_t;

/***************************************************************************
  typedef: r_octa_DQSDelay_t

  Description:
  This type describes the DQS (Data Strobe Signal) delay.

  Struct members:
  EnableCnt  - DQS enable counter.
               During read operation with DQS clock input (RAM/OPI/DOPI mode), 
               DQS clock will transition from high-impedance to zero after command/address 
               phase is finished. 
               To prevent using such invalid DQS clock, user can adjust these configuration 
               to guarantee data correctness.
               The range is 0 to 15. Actual enable DQS clock input is set value + 1 clock.
               If EnableCnt=0 is set, Enable DQS clock input at 1st SCLK.
               If EnableCnt=1 is set, Enable DQS clock input at 2nd SCLK.
               If EnableCnt=2 is set, Enable DQS clock input at 3rd SCLK.
               If EnableCnt=3 is set, Enable DQS clock input at 4th SCLK.
               ...
               If EnableCnt=15 is set, Enable DQS clock input at 16th SCLK.
               
               Reference DQS enable counter settings (when SCLK frequency < 120 MHz)
               
>               ----------------------------------------------------------------
>               DQS             RAM               Flash OPI       Flash DOPI
>                       --------------------------------------------------------
>               delay   Pre-cycle  Pre-cycle      Pre-cycle    Pre-cycle  Pre-cycle
>               [SCLK]     ON         OFF            ON           ON         OFF
>               ----------------------------------------------------------------
>               0-0.5      3          3-4           7-8           5          5-6
>               0.5-1.5    4           5             8            6          6-7
>               1.5-2.5    5          5-6            9            7          7-8
>               ----------------------------------------------------------------
  Delay    - DQS delay value.
             Delay value = SCLK output pad delay + SCLK wire bonding + DQS wire bonding + DQS input pad delay
             (Memory device delay has considered in this table, maximum 2 SCLK cycle)
*/

typedef struct
{
    uint8_t          EnableCnt;
    uint8_t          Delay;
} r_octa_DQSDelay_t;

/***************************************************************************
  typedef: r_octa_Config_t

  Description:
  This type describes the configuration of the channel.

  Struct members:
  DeviceType       - Control device type. See <r_octa_DeviceType_t>.
  OpeMode          - Operating mode. See <r_octa_OperatingMode_t>.
  DataTransferMode - Data transfer mode. See <r_octa_DataTransferMode_t>.
  AddressMode      - Address mode. See <r_octa_AddressMode_t>.
  MemorySize       - Total memory byte-size of connected Octa RAM/flash memory.
                     RH850/D1M1A supports 64 MByte.
                     MemorySize should be multiple of SectorSize if SectorSize is defined.
                     (e.g. This size is 64 MBytes (64*1024*1024), when MX25LW51245G 
                     is connected.)
  SectorSize       - Erase Sector byte-size of connected Octa flash memory.
                     Sector size must be power-of-two value (2^n).
                     If not required, please set to 0.
                     (e.g. This size is 4 Kbytes when MX25LW51245G is connected.)
  PageSize         - Page byte-size of connected Octa flash memory.
                     Page byte-size must be power-of-two value (2^n).
                     If not required, please set to 0.
                     (e.g. This size is 256 bytes when MX25LW51245G is connected.)
  Command          - Command setting. See <r_octa_Command_t>.
                     This memory must be kept allocating till <R_OCTA_Close> is finished.
  RelaxSize        - Relax byte-size.
                     Relax size must be multiple of 4-byte. This size is used in case of manual mode.
                     Some APIs call R_OCTA_Sys_Relax in units of this size. If not required, please set to 0.
  PreCycle         - Pre-cycle mode. See <r_octa_PreCycle_t>.
                     This mode is used when the data transfer mode is R_OCTA_MODE_DOPI.
  DQSDelay         - DQS delay. See <r_octa_DQSDelay_t>.
  CalAddress       - Calibration Start address.
                     This specifies the start address for executing the calibration.
                     The calibration is adjusted by actually reading / writing to 
                     the connected OCTA RAM / flash memory while changing the DQS delay value.
                     This driver isn't executed the calibration.
                     If you want to be executed the calibration, please calibrate with
                     data read / data write after calling the R_OCTA_Open function by
                     setting calibration start address to CalAddress, DQS delay value to DQSDelay.Delay.
                     CalAddress must be 32 bit address aligned.
                     If not required, please set to 0xFFFFFFFF.
*/
typedef struct
{
    r_octa_DeviceType_t         DeviceType;
    r_octa_OperatingMode_t      OpeMode;
    r_octa_DataTransferMode_t   DataTransferMode;
    r_octa_AddressMode_t        AddressMode;
    uint32_t                    MemorySize;
    uint32_t                    SectorSize;
    uint32_t                    PageSize;
    r_octa_Command_t*           Command;
    uint32_t                    RelaxSize;
    r_octa_PreCycle_t           PreCycle;
    r_octa_DQSDelay_t           DQSDelay;
    uint32_t                    CalAddress;
} r_octa_Config_t;


/***************************************************************************
  Section: Global API functions
*/

/***************************************************************************
  Function: R_OCTA_Init

  Description:
  This function initializes the OCTA driver.
  
  This function calls <R_OCTA_Sys_Init> to initialize environment-dependent 
  setting. 

  This function calls <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock>.

  Parameter:
  Unit - Instance number.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_OCTA_ERR_DEVICE           - OCTA driver is not applicable to target device.
  R_OCTA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  
  See <r_octa_Error_t>.

  Executable Status:
  Uninitialized - OK (Status will be changed to *Initialized*.)
  Initialized   - NG 
  Idle          - NG 
  Executing     - NG 

  Executable Connected Device:
  Octa Flash   - OK 
  Octa RAM     - OK 

*/
r_octa_Error_t R_OCTA_Init(const uint32_t Unit);


/***************************************************************************
  Function: R_OCTA_DeInit

  Description:
  This function de-initializes the OCTA driver.
  This function is executed after executing R_OCTA_Close on all channels.

  This function calls <R_OCTA_Sys_DeInit> to de-initialize environment-dependent 
  setting. 
  
  This function calls <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock>.

  Parameter:
  Unit - Instance number.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_OCTA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  
  See <r_octa_Error_t>.

  Executable Status:
  Uninitialized - NG 
  Initialized   - OK (Status will be changed to *Uninitialized*.)
  Idle          - NG 
  Executing     - NG 

  Executable Connected Device:
  Octa Flash   - OK 
  Octa RAM     - OK 

*/
r_octa_Error_t R_OCTA_DeInit(const uint32_t Unit);


/***************************************************************************
  Function: R_OCTA_Open

  Description:
  This function opens the OCTA driver.
  
  This function opens the OCTA driver with the operating mode and device type that is specified.
  
  This function opens for each specified channel device.
  
  In case of external address space mode, Read command and Write command are set to H/W
  and it enables access from memory-map space. For Octa RAM can be read / write directly.
  For Octa flash memory can be read directly.
  Read command and Write command to be set are different depending on the data transfer mode.
  
  In case of manual mode, the function enables access the status/configuration register
  for Octa RAM/flash memory. And enables Write access to memory for Octa flash memory.

  This function calls <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock>.
  
  Parameter:
  Unit    - Instance number.
  Channel - Device Channel number.
  Config  - Pointer to the <r_octa_Config_t> structure.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_RANGE_PARAM      - A parameter is out of range.
  R_OCTA_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_OCTA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_OCTA_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  R_OCTA_ERR_PROTECTED        - A process is aborted because of memory protection.
  R_OCTA_ERR_COMMAND          - A command is not supported.
  R_OCTA_ERR_LATENCY          - A latency value is invalid.
  
  See <r_octa_Error_t>.

  Executable Status:
  Uninitialized - NG 
  Initialized   - OK (Status will be changed to *Idle* or *Executing*.)
  Idle          - NG 
  Executing     - NG 

  Status will be changed to *Idle* if R_OCTA_MODE_MANUAL is specified. 
  
  Status will be changed to *Executing* if R_OCTA_MODE_EX_SPACE is specified. 

  Executable Connected Device:
  Octa Flash   - OK 
  Octa RAM     - OK 

*/
r_octa_Error_t R_OCTA_Open(const uint32_t                Unit,
                           const uint32_t                Channel,
                           const r_octa_Config_t * const Config);


/***************************************************************************
  Function: R_OCTA_Close

  Description:
  This function closes the OCTA driver.

  In case of external address space mode, this function disables access from 
  the memory-map space.
  
  This function calls <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock>.

  Parameter:
  Unit    - Instance number.
  Channel - Device Channel number.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_OCTA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_OCTA_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  
  See <r_octa_Error_t>.

  Executable Status:
  Uninitialized - NG 
  Initialized   - NG 
  Idle          - OK (Status will be changed to *Initialized*.) 
  Executing     - OK (Status will be changed to *Initialized*.) 

  Executable Connected Device:
  Octa Flash   - OK 
  Octa RAM     - OK 
  
*/
r_octa_Error_t R_OCTA_Close(const uint32_t Unit,
                            const uint32_t Channel);


/***************************************************************************
  Function: R_OCTA_DataProtect

  Description:
  This function sets the protection mode of the Octa flash memory.
  
  The write and erase access to Octa Flash memory is prohibited if protection is set.

  This function calls <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock>.

  Parameter:
  Unit     - Instance number.
  Channel  - Device Channel number.
  Mode     - Protection mode. See <r_octa_ProtectionMode_t>.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_RANGE_PARAM      - A parameter is out of range.
  R_OCTA_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_OCTA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_OCTA_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  
  See <r_octa_Error_t>.

  Executable Status:
  Uninitialized - NG 
  Initialized   - NG 
  Idle          - OK 
  Executing     - NG 

  Executable Connected Device:
  Octa Flash   - OK 
  Octa RAM     - NG 

*/
r_octa_Error_t R_OCTA_DataProtect(const uint32_t                Unit,
                                  const uint32_t                Channel,
                                  const r_octa_ProtectionMode_t Mode);


/***************************************************************************
  Function: R_OCTA_DataErase

  Description:
  This function erases the data in the Octa flash memory.
  
  This function erases the data in a unit of sector. Therefore this function
  erases data of the sector including the size from the address.
  
  This function erases the sectors in following range.
  
  Start sector - Sector that Addr is belonged. 
  End sector   - Sector that (Addr + Size - 1) is belonged. 
  
  In the case of Octa flash memory compatible with RWW (Read-While-Write), 
  it can be executed while reading into the memory map space in external address space mode.
  
  This function has the possibility that the processing takes time.
  Therefore, <R_OCTA_Sys_Relax> is sometimes executed.
  
  This function calls <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock>.


  Parameter:
  Unit     - Instance number.
  Channel  - Device Channel number.
  Addr     - Erase start address of the Octa flash memory.
  Size     - Data size to erase.
  
  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_RANGE_PARAM      - A parameter is out of range.
  R_OCTA_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_OCTA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_OCTA_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  R_OCTA_ERR_PROTECTED        - A process is aborted because of memory protection.
  R_OCTA_ERR_TIMEOUT          - Status polling is timeout.
  R_OCTA_ERR_COMMAND          - A command is not supported.
  
  See <r_octa_Error_t>.

  Executable Status:
  Uninitialized - NG 
  Initialized   - NG 
  Idle          - OK 
  Executing     - OK 

  Executable Connected Device:
  Octa Flash   - OK 
  Octa RAM     - NG 

*/
r_octa_Error_t R_OCTA_DataErase(const uint32_t        Unit, 
                                const uint32_t        Channel,
                                const uint32_t        Addr, 
                                const uint32_t        Size );


/***************************************************************************
  Function: R_OCTA_DataWriteRWW

  Description:
  This function writes data to the Octa Flash memory by RWW (Read-While-Write) function.
  
  RWW function means read data one bank while another bank is programing or erasing.

  This function can be executed for Octa flash memory with supporting RWW.
  If Octa flash memory is not supported RWW, does not guarantee the operation.
  
  This function is executed WriteBufInitial, WriteBufContinue and WriteBufConfirm
  command. These commands to be set are different depending on the data transfer mode.
  
  In DTR OPI, the starting address given must be even address (A0=0) and data byte number must be even.
  
  This function has the possibility that the processing takes time.
  Therefore, <R_OCTA_Sys_Relax> is sometimes executed.
  
  This function calls <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock>.


  Parameter:
  Unit     - Instance number.
  Channel  - Device Channel number.
  Addr     - The parameter specifies the write address of the Octa flash memory.
             If the data transfer mode is R_OCTA_MODE_DOPI, Addr given must be even address.
  Buf      - This is a pointer to the buffer stored write data.
  Size     - The parameter specifies the data byte-size to write.
             If the data transfer mode is R_OCTA_MODE_DOPI, Size given must be even data size.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_RANGE_PARAM      - A parameter is out of range.
  R_OCTA_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_OCTA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_OCTA_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  R_OCTA_ERR_PROTECTED        - A process is aborted because of memory protection.
  R_OCTA_ERR_TIMEOUT          - Status polling is timeout.
  R_OCTA_ERR_COMMAND          - A command is not supported.
  
  See <r_octa_Error_t>.
  
  Executable Status:
  Uninitialized - NG 
  Initialized   - NG 
  Idle          - NG 
  Executing     - OK 

  Executable Connected Device:
  Octa Flash   - OK 
  Octa RAM     - NG 
  
*/
r_octa_Error_t R_OCTA_DataWriteRWW(const uint32_t        Unit,
                                const uint32_t        Channel,
                                const uint32_t        Addr,
                                const uint8_t*        Buf,
                                const uint32_t        Size);


/***************************************************************************
  Function: R_OCTA_DataWrite

  Description:
  This function writes data to the Octa Flash memory.
  
  In order to write data to Octa Flash, the data of the sector must have been 
  erased previously.
  
  This function executes Write command. Write commands to be set are different 
  depending on the data transfer mode.

  In DTR OPI, the starting address given must be even address (A0=0) and data byte number must be even.
  
  This function has the possibility that the processing takes time.
  Therefore, <R_OCTA_Sys_Relax> is sometimes executed.
  
  This function calls <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock>.


  Parameter:
  Unit     - Instance number.
  Channel  - Device Channel number.
  Addr     - The parameter specifies the write address of the Octa flash memory.
             This parameter aligns in the page size of the Octa flash memory.
             If the data transfer mode is R_OCTA_MODE_DOPI, Addr given must be even address.
  Buf      - This is a pointer to the buffer stored write data.
  Size     - The parameter specifies the data byte-size to write.
             If the data transfer mode is R_OCTA_MODE_DOPI, Size given must be even data size.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_RANGE_PARAM      - A parameter is out of range.
  R_OCTA_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_OCTA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_OCTA_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  R_OCTA_ERR_PROTECTED        - A process is aborted because of memory protection.
  R_OCTA_ERR_TIMEOUT          - Status polling is timeout.
  
  See <r_octa_Error_t>.
  
  Executable Status:
  Uninitialized - NG 
  Initialized   - NG 
  Idle          - OK 
  Executing     - NG 

  Executable Connected Device:
  Octa Flash   - OK 
  Octa RAM     - NG 
  
*/
r_octa_Error_t R_OCTA_DataWrite(const uint32_t        Unit,
                                const uint32_t        Channel,
                                const uint32_t        Addr,
                                const uint8_t*        Buf,
                                const uint32_t        Size);

/***************************************************************************
  Function: R_OCTA_UserCmdIssue

  Description:
  This function executes user command sequence.

  This function executes when user read / write the status/configuration register
  and read device ID for Octa RAM/flash memory.

  Read register in OCTA RAM, the byte order is as following.
    byte1, byte0, byte3, byte2
  Read register in OCTA flash memory, the byte order is as following.
    byte0, byte1, byte2, byte3

  This function calls <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock>.

  Parameter:
  Unit           - Instance number.
  Channel        - Device Channel number.
  CmdTransaction - Command sequence. See <r_octa_CmdTransaction_t>.
  Buf            - This is a pointer to the Buffer to store the read/write data.
                   If read transaction is not existed in command sequence, 
                   please set to R_NULL.
  BufSize        - Byte-size of data that can be stored to Buf.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_RANGE_PARAM      - A parameter is out of range.
  R_OCTA_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_OCTA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_OCTA_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  R_OCTA_ERR_COMMAND          - A command is not supported.
  
  See <r_octa_Error_t>.
  
  Executable Status:
  Uninitialized - NG 
  Initialized   - NG 
  Idle          - OK 
  Executing     - NG 

  Executable Connected Device:
  Octa Flash   - OK 
  Octa RAM     - OK 
*/
r_octa_Error_t R_OCTA_UserCmdIssue(const uint32_t                       Unit,
                                   const uint32_t                       Channel,
                                   const r_octa_CmdTransaction_t* const CmdTransaction,
                                         uint8_t*                 const Buf,
                                   const uint32_t                       BufSize);

/***************************************************************************
  Function: R_OCTA_GetCal

  Description:
  This function receives the DQS (Data Strobe Signal) delay value.
  The DQS delay value uses to adjusting the calibration.
  
  This function uses to get the DQS delay value after executing the calibration.
  The calibration is adjusted by actually reading / writing to the connected 
  OCTA RAM / flash memory while changing the DQS delay value.
  The DQS delay value was received in this function is sets to DQSDelay.Delay
  @ <r_octa_Config_t> of argument for R_OCTA_Open function.
   
  This function calls <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_OCTA_Sys_Lock> and <R_OCTA_Sys_Unlock>.

  Parameter:
  Unit    - Instance number.
  Channel - Device Channel number.
  Delay   - Pointer to the DQS delay value.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_RANGE_UNIT       - Unit number is out of range.
  R_OCTA_ERR_RANGE_PARAM      - A parameter is out of range.
  R_OCTA_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  R_OCTA_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_OCTA_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  
  See <r_octa_Error_t>.

  Executable Status:
  Uninitialized - NG
  Initialized   - OK 
  Idle          - OK 
  Executing     - OK 

  Executable Connected Device:
  Octa Flash   - OK 
  Octa RAM     - OK 
*/
r_octa_Error_t R_OCTA_GetCal(const uint32_t         Unit,
                             const uint32_t         Channel,
                                   uint8_t* const   Delay);

/***************************************************************************
  Function: R_OCTA_VersionStringGet

  Description:
  This function returns the version string of this OCTA driver.

  Parameter:
  None

  Return value:
  Pointer of string.

  Executable Status:
  Uninitialized - OK 
  Initialized   - OK 
  Idle          - OK 
  Executing     - OK 

  Executable Connected Device:
  Octa Flash   - OK 
  Octa RAM     - OK
*/
const uint8_t* R_OCTA_VersionStringGet(void);

/***************************************************************************
  Function: R_OCTA_MacroVersionGet

  Description:
  This function returns the major and minor version of the H/W macro.

  Parameter:
  Major - Pointer to store the major version.
  Minor - Pointer to store the minor version.

  Return value:
  R_OCTA_ERR_OK               - No error has occurred.
  R_OCTA_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  
  See <r_octa_Error_t>.

  Executable Status:
  Uninitialized - OK 
  Initialized   - OK 
  Idle          - OK 
  Executing     - OK 

  Executable Connected Device:
  Octa Flash   - OK 
  Octa RAM     - OK
*/
r_octa_Error_t R_OCTA_MacroVersionGet(uint32_t * const Major,
                                      uint32_t * const Minor);


#ifdef __cplusplus
}
#endif

#endif /* R_OCTA_API_H */

