/*
****************************************************************************
PROJECT : HYPB driver
FILE    : $Id: r_hypb_api.h 15480 2018-01-11 14:07:04Z florian.zimmermann $
============================================================================
DESCRIPTION
Driver for the HYPB(HyperBus Controller) macro
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

DISCLAIMER

LICENSEE has read, understood and accepted the terms and conditions defined in
the license agreement, especially the usage rights. In any case, it is
LICENSEE's responsibility to make sure that any user of the software complies
with the terms and conditions of the signed license agreement.

****************************************************************************
*/

/***************************************************************************
  Title: HYPB Driver API

  HYPB (HyperBus Controller) driver API.

  HYPB driver controls HYPB H/W macro and accesses only register of HYPB H/W macro.

  An application using HYPB driver should include the following header files.

  * r_typedefs.h
  * r_hypb_api.h

*/

/***************************************************************************
  Section: Definition of a term
*/
/***************************************************************************
  Topics: xxxx(@ yyyy)
  It means the xxxx is the member of the structure yyyy.

  Topics: Word
  1 word is 16bits. Word-address is 16bit address. Word-size is 16bit size. 
  
  WordSize=1 means 2Byte. WordSize=2 means 4Byte. 

*/

#ifndef R_HYPB_API_H
#define R_HYPB_API_H

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
  This constant is the value which shows the version information of the HYPB driver.
  It refers at the <R_HYPB_VersionStringGet> function.

  Values:
  R_HYPB_VERSION_HI - This constant is the high byte of the version information.
                      It is major version information.
  R_HYPB_VERSION_LO - This constant is the low byte of the version information.
                      It is minor version information.
*/
#define R_HYPB_VERSION_HI       (1)
#define R_HYPB_VERSION_LO       (1)

/***************************************************************************
  Constants: Internal definition for Command Option

  Description:
  These flags are used to <Command Option>.
  
  Values:
  R_HYPB_CMD_WRITE        - Use Write transaction. 
  R_HYPB_CMD_READ         - Use Read transaction. 
  R_HYPB_CMD_SIZE_16      - Size is 16 bit. 
  R_HYPB_CMD_SIZE_32      - Size is 32 bit.
  R_HYPB_CMD_SIZE_64      - Size is 64 bit.
  R_HYPB_CMD_SIZE_MASK    - Mask for size get.
  R_HYPB_CMD_APPROPRIATE  - Size is appropriate (16/32/64) bit.

*/
#define R_HYPB_CMD_WRITE        (0x0000uL) 
#define R_HYPB_CMD_READ         (0x8000uL) 
#define R_HYPB_CMD_SIZE_16      (0x0000uL) 
#define R_HYPB_CMD_SIZE_32      (0x0001uL) 
#define R_HYPB_CMD_SIZE_64      (0x0002uL) 
#define R_HYPB_CMD_SIZE_MASK    (0x0003uL) 
#define R_HYPB_CMD_APPROPRIATE  (0x0004uL) 

/***************************************************************************
  Constants: Command Option

  Description:
  HYPB command option flags. These flags are used to OpeFlags(@ <r_hypb_CmdTransaction_t>).
  
  Values:
  R_HYPB_CMD_WRITE16   - Use Write transaction. Write size is 16 bit. It is default behavior. 
  R_HYPB_CMD_WRITE_A   - Use Write transaction. Write size is appropriate (16/32/64) bit.
                         This flag can be set with R_HYPB_CMD_SET_DATA.
  R_HYPB_CMD_READ16    - Use Read transaction. Read size is 16 bit.
  R_HYPB_CMD_READ32    - Use Read transaction. Read size is 32 bit.
  R_HYPB_CMD_READ64    - Use Read transaction. Read size is 64 bit.
  R_HYPB_CMD_SET_ADDR  - Update Address member to word-address.
  R_HYPB_CMD_SET_SA    - Update Address member to sector address. 
                         This flag can be set when SectorSize(@ <r_hypb_Config_t>) is not 0.
  R_HYPB_CMD_SET_DATA  - Update WriteData member to write data.
  R_HYPB_CMD_SET_COUNT - Update WriteData member to loop count.
  R_HYPB_CMD_LOOP      - The transaction is executed repeatedly.

*/

#define R_HYPB_CMD_WRITE16      (R_HYPB_CMD_WRITE | R_HYPB_CMD_SIZE_16) 
#define R_HYPB_CMD_WRITE_A      (R_HYPB_CMD_WRITE | R_HYPB_CMD_APPROPRIATE) 
#define R_HYPB_CMD_READ16       (R_HYPB_CMD_READ  | R_HYPB_CMD_SIZE_16) 
#define R_HYPB_CMD_READ32       (R_HYPB_CMD_READ  | R_HYPB_CMD_SIZE_32) 
#define R_HYPB_CMD_READ64       (R_HYPB_CMD_READ  | R_HYPB_CMD_SIZE_64) 
#define R_HYPB_CMD_SET_ADDR     (0x0010uL) 
#define R_HYPB_CMD_SET_SA       (0x0020uL) 
#define R_HYPB_CMD_SET_DATA     (0x0040uL) 
#define R_HYPB_CMD_SET_COUNT    (0x0080uL) 
#define R_HYPB_CMD_LOOP         (0x0100uL) 

/***************************************************************************
  Section: Global Types
*/
/***************************************************************************
  Group: Enumerations and Types
*/
/***************************************************************************
  Enum: r_hypb_Error_t

  Description:
  HYPB driver error code.

  If an error occurs, these enumerations give information about the reason.

  Values:
  R_HYPB_ERR_OK               - No error has occurred.
  R_HYPB_ERR_NG               - An error has occurred, but no specific error code
                                is defined for it.
  R_HYPB_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  R_HYPB_ERR_RANGE_UNIT       - Unit number is out of range.
  R_HYPB_ERR_RANGE_PARAM      - A parameter is out of range.
  R_HYPB_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_HYPB_ERR_DEVICE           - HYPB driver is not applicable to target d1x device.
  R_HYPB_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_HYPB_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  R_HYPB_ERR_COMMAND          - A command is not supported.
  R_HYPB_ERR_LATENCY          - A latency value is invalid.
  R_HYPB_ERR_PROTECTED        - A process is aborted because of memory protection.
  R_HYPB_ERR_ABORTED          - A process is aborted because of other reason.
  R_HYPB_ERR_TIMEOUT          - Status polling is timeout.
  
*/
typedef enum
{
    R_HYPB_ERR_OK = 0,
    R_HYPB_ERR_NG,
    R_HYPB_ERR_PARAM_INCORRECT,
    R_HYPB_ERR_RANGE_UNIT,
    R_HYPB_ERR_RANGE_PARAM,
    R_HYPB_ERR_NOT_ACCEPTABLE,
    R_HYPB_ERR_DEVICE,
    R_HYPB_ERR_FATAL_OS,
    R_HYPB_ERR_FATAL_HW,
    R_HYPB_ERR_COMMAND,
    R_HYPB_ERR_LATENCY,
    R_HYPB_ERR_PROTECTED,
    R_HYPB_ERR_ABORTED,
    R_HYPB_ERR_TIMEOUT
} r_hypb_Error_t;

/***************************************************************************
  Enum: r_hypb_DeviceType_t

  Description:
  This type describes the control device type.

  Values:
  R_HYPB_DEVICE_FLASH      - Hyper Flash. 
  R_HYPB_DEVICE_RAM        - Hyper RAM. 
                             Note: Hyper RAM should consist of single die.
*/
typedef enum
{
    R_HYPB_DEVICE_FLASH = 0,
    R_HYPB_DEVICE_RAM
} r_hypb_DeviceType_t;

/***************************************************************************
  Enum: r_hypb_OperatingMode_t

  Description:
  This type describes the operating mode.

  Values:
  R_HYPB_MODE_EX_SPACE      - External address space mode. 
                              In case of Hyper Flash, only Read memory access is possible.
                              In case of Hyper RAM, Read/Write memory access is possible.
  R_HYPB_MODE_MANUAL        - Manual mode. 
                              In case of Hyper Flash, Read/Write access to register/memory area
                              is possible.
                              In case of Hyper RAM, Read/Write access to register area is 
                              possible.
*/
typedef enum
{
    R_HYPB_MODE_EX_SPACE = 0,
    R_HYPB_MODE_MANUAL
} r_hypb_OperatingMode_t;

/***************************************************************************
  Enum: r_hypb_ParamSecPos_t

  Description:
  This type describes the position of parameter-sector.

  Values:
  R_HYPB_PARAM_SEC_TOP    - Parameter-sector is mapped into the top of the first sector 
                            of Hyper RAM/flash memory.
  R_HYPB_PARAM_SEC_BOTTOM - Parameter-sector is mapped into the bottom of the last sector 
                            of Hyper RAM/flash memory.
*/
typedef enum 
{
    R_HYPB_PARAM_SEC_TOP    = 0,
    R_HYPB_PARAM_SEC_BOTTOM
} r_hypb_ParamSecPos_t;

/***************************************************************************
  Enum: r_hypb_ProtectionMode_t

  Description:
  This type describes the protection mode of the hyper RAM/flash memory.

  Values:
  R_HYPB_MODE_PROTECT   - Set to the protection mode.
  R_HYPB_MODE_UNPROTECT - Set to the un-protection mode.
*/
typedef enum
{
    R_HYPB_MODE_PROTECT = 0,
    R_HYPB_MODE_UNPROTECT
} r_hypb_ProtectionMode_t;

/***************************************************************************
  Enum: r_hypb_CmdType_t

  Description:
  This type describes the command type that is defined <r_hypb_Command_t>. 

  Values:
  R_HYPB_CMD_TYPE_OPEN            - Command of Open.
  R_HYPB_CMD_TYPE_CLOSE           - Command of Close.
  R_HYPB_CMD_TYPE_READ            - Command of Read.
  R_HYPB_CMD_TYPE_WRITE           - Command of Write.
  R_HYPB_CMD_TYPE_WRITE_BUF_FLASH - Command of WriteBufFlash.
  R_HYPB_CMD_TYPE_ERASE           - Command of Erase.
  R_HYPB_CMD_TYPE_PROTECT         - Command of Protect.
  R_HYPB_CMD_TYPE_READ_CFG_1      - Command of ReadCfg1.
  R_HYPB_CMD_TYPE_READ_CFG_2      - Command of ReadCfg2.
  R_HYPB_CMD_TYPE_WRITE_CFG_1     - Command of WriteCfg1.
  R_HYPB_CMD_TYPE_WRITE_CFG_2     - Command of WriteCfg2.
  R_HYPB_CMD_TYPE_READ_STS        - Command of ReadSts.
  R_HYPB_CMD_TYPE_USER            - Command of user defined.
  R_HYPB_CMD_TYPE_MAX             - The number of defined Command.
  
*/
typedef enum
{
    R_HYPB_CMD_TYPE_OPEN,
    R_HYPB_CMD_TYPE_CLOSE,
    R_HYPB_CMD_TYPE_READ,
    R_HYPB_CMD_TYPE_WRITE,
    R_HYPB_CMD_TYPE_WRITE_BUF_FLASH,
    R_HYPB_CMD_TYPE_ERASE,
    R_HYPB_CMD_TYPE_PROTECT,
    R_HYPB_CMD_TYPE_READ_CFG_1,
    R_HYPB_CMD_TYPE_READ_CFG_2,
    R_HYPB_CMD_TYPE_WRITE_CFG_1,
    R_HYPB_CMD_TYPE_WRITE_CFG_2,
    R_HYPB_CMD_TYPE_READ_STS,
    R_HYPB_CMD_TYPE_USER,
    R_HYPB_CMD_TYPE_MAX
} r_hypb_CmdType_t;

/***************************************************************************
  typedef: r_hypb_ParamSec_t

  Description:
  This type describes the parameter-sector information.
  
  Struct members:
  Size - Size of one parameter-sector (Byte).
         This size must be power-of-two value (2^n). 
         If parameter sector is not defined or not used, please set to 0.
  Num  - The number of parameter-sectors.
         The total size of parameter-sector (Size * Num) must be less than 
         SectorSize(@ <r_hypb_Config_t>).
         If parameter sector is not defined or not used, please set to 0.
  Pos  - Position of parameter-sectors. See <r_hypb_ParamSecPos_t>.

  Example:
  MemorySize(@ <r_hypb_Config_t>)=16*1024*1024,
  SectorSize(@ <r_hypb_Config_t>)=256*1024

  (1)Size=4096, Num=8, Pos=R_HYPB_PARAM_SEC_TOP 
  
>  Word-Address          Sector Number
>  ---------------------------------------
>  00000000h~000007FFh - Parameter-Sector #0
>  00000800h~00000FFFh - Parameter-Sector #1
>   :                  -  :  
>  00003800h~00003FFFh - Parameter-Sector #7
>  00004000h~0001FFFFh - Normal Sector #0
>  00020000h~0003FFFFh - Normal Sector #1
>   :                  -  :  

  (2)Size=4096, Num=8, Pos=R_HYPB_PARAM_SEC_BOTTOM 

>  Word-Address          Sector Number
>  ---------------------------------------
>   :                  -  :  
>  007C0000h~007DFFFFh - Normal Sector #62
>  007E0000h~007FBFFFh - Normal Sector #63
>  007FC000h~007FC7FFh - Parameter-Sector #0
>  007FC800h~007FCFFFh - Parameter-Sector #1
>   :                  -  :  
>  007FF800h~007FFFFFh - Parameter-Sector #7

  (3)ParamSec(@ <r_hypb_Config_t>)=R_NULL

>  Word-Address          Sector Number
>  ---------------------------------------
>  00000000h~0001FFFFh - Normal Sector #0
>  00020000h~0003FFFFh - Normal Sector #1
>   :                  -  :  
>  007C0000h~007DFFFFh - Normal Sector #62
>  007E0000h~007FFFFFh - Normal Sector #63

*/ 
typedef struct 
{
    uint32_t                    Size;
    uint32_t                    Num;
    r_hypb_ParamSecPos_t        Pos;
} r_hypb_ParamSec_t;

/***************************************************************************
  typedef: r_hypb_CmdTransaction_t

  Description:
  This type describes the hyper RAM/flash commands.

  Struct members:
  Address   - Address data to set CA[44:16] and CA[02:00].
  WriteData - Write data (only supports a single write transaction of 16 bits.)
  CmdFlags  - Flags data to set CA[47:45]. Please set to MSB 3bit.
  Latency   - Latency in units of clock cycles. 
              Latency is from CA[23:16] access timing to data Read/Write timing. 
              If latency is needed, the range is 3 to 20.
              If no latency is needed, please set to 0. 
  OpeFlags  - Operation Flags. Multiple flags can be set with '|'. 
              See definition of <Command Option>. 

  Note:
  CA means Command-Address. Please see data sheet for detail.

*/
typedef struct 
{
    uint32_t                   Address;
    uint16_t                   WriteData;
    uint8_t                    CmdFlags;
    uint8_t                    Latency;
    uint32_t                   OpeFlags;
} r_hypb_CmdTransaction_t;

/***************************************************************************
  typedef: r_hypb_CmdSequence_t

  Description:
  This type describes the hyper RAM/flash commands.

  Struct members:
  Transaction - Pointer to Command Transaction Array.
                This memory must be kept allocating till <R_HYPB_Close> is finished.
                If the command is not supported, please set to R_NULL. 
  Cycle       - Number of Command Transaction Array. 
                If the command is not supported, please set to 0. 
*/
typedef struct 
{
    r_hypb_CmdTransaction_t*    Transaction;
    uint8_t                     Cycle;
} r_hypb_CmdSequence_t; 

/***************************************************************************
  typedef: r_hypb_Latency_t

  Description:
  This type describes the latency.

  Struct members:
  Code      - Initial latency code to set to Configuration register.
  ClkDelay  - Sets the period from MCS pin assertion to MCK oscillation. 
              Range is 1 to 8 (clock).
              (e.g. It is described as tCSS in data sheet of IS26KL128S.)
  NagDelay  - Sets the period from the time the last MCK edge is sent 
              of a transfer to MCS pin negation. 
              Range is 1 to 8. Actual Delay is set value + 0.5 clk.
              If NegDelay=3 is set, actual delay is 3.5 clock.
              (e.g. It is described as tCSH in data sheet of IS26KL128S.)
  NextAcc   - Sets the period from transaction end to next transaction start.
              Range is 1 to 8 (clock). 
              (e.g. It is described as tCSHI in data sheet of IS26KL128S.)
              
*/

typedef struct
{
    uint8_t                     Code;
    uint8_t                     ClkDelay;
    uint8_t                     NagDelay;
    uint8_t                     NextAcc;
} r_hypb_Latency_t;

/***************************************************************************
  typedef: r_hypb_Command_t

  Description:
  This type describes the hyper RAM/flash commands.

  Struct members:
  Latency       - Latency setting. See <r_hypb_Latency_t>.
  Open          - Command sequence of Open. See <r_hypb_CmdSequence_t>.
                  Following Operation Flags are available.
                    R_HYPB_CMD_WRITE16
  Close         - Command sequence of Close. See <r_hypb_CmdSequence_t>.
                  This command is used in case of manual mode.
                  Following Operation Flags are available.
                    R_HYPB_CMD_WRITE16
  Read          - Command sequence of Read. See <r_hypb_CmdSequence_t>.
                  Following Operation Flags are available.
                    R_HYPB_CMD_WRITE16  / 
                    R_HYPB_CMD_READ16   / 
                    R_HYPB_CMD_READ32   / 
                    R_HYPB_CMD_READ64   / 
                    R_HYPB_CMD_SET_ADDR
  Write         - Command sequence of Write directly. See <r_hypb_CmdSequence_t>.
                  This command is used in case of manual mode.
                  Following Operation Flags are available.
                    R_HYPB_CMD_WRITE16   / 
                    R_HYPB_CMD_WRITE_A   / 
                    R_HYPB_CMD_SET_ADDR  / 
                    R_HYPB_CMD_SET_DATA 
  WriteBufFlash - Command sequence of Write via write Buffer. See <r_hypb_CmdSequence_t>.
                  This command is used in case of manual mode.
                  Following Operation Flags are available.
                    R_HYPB_CMD_WRITE16   / 
                    R_HYPB_CMD_SET_ADDR  / 
                    R_HYPB_CMD_SET_SA    / 
                    R_HYPB_CMD_SET_DATA  / 
                    R_HYPB_CMD_SET_COUNT / 
                    R_HYPB_CMD_LOOP        
  Erase         - Command sequence of Sector Erase. See <r_hypb_CmdSequence_t>.
                  This command is used in case of manual mode.
                  Following Operation Flags are available.
                    R_HYPB_CMD_WRITE16   / 
                    R_HYPB_CMD_SET_ADDR  / 
                    R_HYPB_CMD_SET_SA      
  Protect       - Command sequence of Sector Protect. See <r_hypb_CmdSequence_t>.
                  This command is used in case of manual mode.
                  Following Operation Flags are available.
                    R_HYPB_CMD_WRITE16   / 
                    R_HYPB_CMD_SET_ADDR  / 
                    R_HYPB_CMD_SET_SA    / 
                    R_HYPB_CMD_SET_DATA    
  ReadCfg1      - Command sequence of Read Configuration Register at the first time.
                  This command is used before initial latency code is set.
                  Please take care in latency setting.
                  (e.g. Latency is wrote in Non-Volatile Configuration register
                  in case of IS26KL128S.)
                  See <r_hypb_CmdSequence_t>.
                  Following Operation Flags are available.
                    R_HYPB_CMD_WRITE16   / 
                    R_HYPB_CMD_READ16    
  ReadCfg2      - Command sequence of Read Configuration Register after the second time.
                  This command is used after initial latency code is set.
                  See <r_hypb_CmdSequence_t>.
                  Following Operation Flags are available.
                    R_HYPB_CMD_WRITE16   / 
                    R_HYPB_CMD_READ16    
  WriteCfg1     - Command sequence of Write Configuration Register at the first time.
                  This command is used when initial latency code is set.
                  See <r_hypb_CmdSequence_t>.
                  Following Operation Flags are available.
                    R_HYPB_CMD_WRITE16   / 
                    R_HYPB_CMD_SET_DATA    
  WriteCfg2     - Command sequence of Write Configuration Register after the second time.
                  This command is used after initial latency code is set.
                  See <r_hypb_CmdSequence_t>.
                  Following Operation Flags are available.
                    R_HYPB_CMD_WRITE16   / 
                    R_HYPB_CMD_SET_DATA    
  ReadSts       - Command sequence of Read status Register. See <r_hypb_CmdSequence_t>.
                  This command is used in case of manual mode.
                  Following Operation Flags are available.
                    R_HYPB_CMD_WRITE16   / 
                    R_HYPB_CMD_READ16    
  
*/

typedef struct 
{
    r_hypb_Latency_t            Latency;
    r_hypb_CmdSequence_t        Open;
    r_hypb_CmdSequence_t        Close;
    r_hypb_CmdSequence_t        Read;
    r_hypb_CmdSequence_t        Write;
    r_hypb_CmdSequence_t        WriteBufFlash;
    r_hypb_CmdSequence_t        Erase;
    r_hypb_CmdSequence_t        Protect;
    r_hypb_CmdSequence_t        ReadCfg1;
    r_hypb_CmdSequence_t        ReadCfg2;
    r_hypb_CmdSequence_t        WriteCfg1;
    r_hypb_CmdSequence_t        WriteCfg2;
    r_hypb_CmdSequence_t        ReadSts;
} r_hypb_Command_t;


/***************************************************************************
  typedef: r_hypb_Config_t

  Description:
  This type describes the configuration of the unit.

  Struct members:
  DeviceType - Control device type. See <r_hypb_DeviceType_t>.
  OpeMode    - Operating mode. See <r_hypb_OperatingMode_t>.
  MemorySize - Total memory byte-size of connected hyper RAM/flash memory.
               RH850/D1M1A supports up to 16 Mbytes. 
               MemorySize should be multiple of SectorSize if SectorSize is defined.
               (e.g. This size is 16 MBytes (16*1024*1024), when IS26KL128S 
               is connected.)
  SectorSize - Sector byte-size of connected hyper RAM/flash memory.
               Sector size must be power-of-two value (2^n). 
               This size is used in case of manual mode.
               This size is used when R_HYPB_CMD_SET_SA flag is included in the command.
               If Sector is not defined, please set to 0.
               (e.g. This size is 256 Kbytes when IS26KL128S is connected.)
  WriteAlign - Alignment byte-address for WriteBufFlash command.
               Write align size must be power-of-two value (2^n). The range is 8 to 2048.
               This alignment is used in case of manual mode and it is used by <R_HYPB_DataWrite>.
               WriteBufFlash command sequence is closed in units of this alignment.
               If not required, please set to 0.
               (e.g. This size is 512 bytes (Write buffer line size) when IS26KL128S 
               is connected.)
  RelaxSize  - Relax byte-size. Relax size must be multiple of 8-byte.
               This size is used in case of manual mode.
               Some APIs call <R_HYPB_Sys_Relax> in units of this size.
               If not required, please set to 0.
  ParamSec   - Parameter sector information. See <r_hypb_ParamSec_t>.
               This information is used in case of manual mode.
  Command    - Command setting. See <r_hypb_Command_t>.
               This memory must be kept allocating till <R_HYPB_Close> is finished.
*/
typedef struct
{
    r_hypb_DeviceType_t         DeviceType;
    r_hypb_OperatingMode_t      OpeMode;
    uint32_t                    MemorySize;
    uint32_t                    SectorSize;
    uint32_t                    WriteAlign;
    uint32_t                    RelaxSize;
    r_hypb_ParamSec_t           ParamSec;
    r_hypb_Command_t*           Command;
} r_hypb_Config_t;


/***************************************************************************
  Section: Global API functions
*/

/***************************************************************************
  Function: R_HYPB_Init

  Description:
  This function initializes the HYPB driver.
  
  This function calls <R_HYPB_Sys_Init> to initialize environment-dependent 
  setting. 

  This function calls <R_HYPB_Sys_Lock> and <R_HYPB_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_HYPB_Sys_Lock> and <R_HYPB_Sys_Unlock>.

  Parameter:
  Unit - Instance number.

  Return value:
  R_HYPB_ERR_OK               - No error has occurred.
  R_HYPB_ERR_RANGE_UNIT       - Unit number is out of range.
  R_HYPB_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_HYPB_ERR_DEVICE           - HYPB driver is not applicable to target device.
  R_HYPB_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  
  See <r_hypb_Error_t>.

  Executable Status:
  Uninitialized - OK (Status will be changed to *Initialized*.)
  Initialized   - NG 
  Idle          - NG 
  Executing     - NG 

  Executable Connected Device:
  Hyper Flash   - OK 
  Hyper RAM     - OK 

*/
r_hypb_Error_t R_HYPB_Init(const uint32_t Unit);


/***************************************************************************
  Function: R_HYPB_DeInit

  Description:
  This function de-initializes the HYPB driver.

  This function calls <R_HYPB_Sys_DeInit> to de-initialize environment-dependent 
  setting. 
  
  This function calls <R_HYPB_Sys_Lock> and <R_HYPB_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_HYPB_Sys_Lock> and <R_HYPB_Sys_Unlock>.

  Parameter:
  Unit - Instance number.

  Return value:
  R_HYPB_ERR_OK               - No error has occurred.
  R_HYPB_ERR_RANGE_UNIT       - Unit number is out of range.
  R_HYPB_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_HYPB_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  
  See <r_hypb_Error_t>.

  Executable Status:
  Uninitialized - NG 
  Initialized   - OK (Status will be changed to *Uninitialized*.)
  Idle          - NG 
  Executing     - NG 

  Executable Connected Device:
  Hyper Flash   - OK 
  Hyper RAM     - OK 

*/
r_hypb_Error_t R_HYPB_DeInit(const uint32_t Unit);


/***************************************************************************
  Function: R_HYPB_Open

  Description:
  This function opens the HYPB driver.
  
  This function will access the Configuration register to set latency code and 
  parameter-sector information. This process is dependent on <R_HYPB_Sys_ConfigRegSet>.
  
  At the first access, ReadCfg1(@ <r_hypb_Command_t>) and WriteCfg1(@ <r_hypb_Command_t>) 
  are used to access to Configuration register.
  
  After the second time, ReadCfg2(@ <r_hypb_Command_t>) and WriteCfg2(@ <r_hypb_Command_t>) 
  are used. 
  
  If ReadCfg1/ReadCfg2 or WriteCfg1/WriteCfg2 is not supported, this function dose
  not update the Configuration register. In this case, this function assumes that latency 
  code and parameter-sector set by argument have already set to register.
  
  After Configuration register is updated, in case of manual mode, this function executes 
  Open(@ <r_hypb_Command_t>) command if it is supported.
  
  In case of external address space mode, Read(@ r_hypb_Command_t) command is set to H/W 
  and it enables access from external address space.
  
  When R_HYPB_MODE_EX_SPACE is specified, this function will fail and return R_HYPB_ERR_COMMAND 
  or R_HYPB_ERR_LATENCY under following conditions.
  - Read(@ <r_hypb_Command_t>) command is not supported.
  - Read(@ <r_hypb_Command_t>) transaction cycle is not 1.
  - Read(@ <r_hypb_Command_t>) latency cycles are less than 8. 
  - Read(@ <r_hypb_Command_t>) latency cycles are not even value. 
  
  This function calls <R_HYPB_Sys_Lock> and <R_HYPB_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_HYPB_Sys_Lock> and <R_HYPB_Sys_Unlock>.
  
  Parameter:
  Unit    - Instance number.
  Config  - Pointer to the <r_hypb_Config_t> structure.

  Return value:
  R_HYPB_ERR_OK               - No error has occurred.
  R_HYPB_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  R_HYPB_ERR_RANGE_UNIT       - Unit number is out of range.
  R_HYPB_ERR_RANGE_PARAM      - A parameter is out of range.
  R_HYPB_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_HYPB_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_HYPB_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  R_HYPB_ERR_COMMAND          - A command is not supported.
  R_HYPB_ERR_LATENCY          - A latency value is invalid.
  
  See <r_hypb_Error_t>.

  Executable Status:
  Uninitialized - NG 
  Initialized   - OK (Status will be changed to *Idle* or *Executing*.)
  Idle          - NG 
  Executing     - NG 

  Status will be changed to *Idle* if R_HYPB_MODE_MANUAL is specified. 
  
  Status will be changed to *Executing* if R_HYPB_MODE_EX_SPACE is specified. 

  Executable Connected Device:
  Hyper Flash   - OK 
  Hyper RAM     - OK 

*/
r_hypb_Error_t R_HYPB_Open(const uint32_t                Unit,
                           const r_hypb_Config_t * const Config);


/***************************************************************************
  Function: R_HYPB_Close

  Description:
  This function closes the HYPB driver.

  In case of manual mode, this function executes Close(@ <r_hypb_Command_t>) 
  command if it is supported.
  In case of external address space mode, this function disables access from 
  the external space.
  
  This function calls <R_HYPB_Sys_Lock> and <R_HYPB_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_HYPB_Sys_Lock> and <R_HYPB_Sys_Unlock>.

  Parameter:
  Unit    - Instance number.

  Return value:
  R_HYPB_ERR_OK               - No error has occurred.
  R_HYPB_ERR_RANGE_UNIT       - Unit number is out of range.
  R_HYPB_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_HYPB_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_HYPB_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  
  See <r_hypb_Error_t>.

  Executable Status:
  Uninitialized - NG 
  Initialized   - NG 
  Idle          - OK (Status will be changed to *Initialized*.) 
  Executing     - OK (Status will be changed to *Initialized*.) 

  Executable Connected Device:
  Hyper Flash   - OK 
  Hyper RAM     - OK 
  
*/
r_hypb_Error_t R_HYPB_Close(const uint32_t Unit);


/***************************************************************************
  Function: R_HYPB_DataProtect

  Description:
  This function sets the protection mode of the hyper flash memory.

  This function executes Protect(@ <r_hypb_Command_t>) command.
  
  This function assumes that Protect command can be set protection mode for each 
  sector (or parameter-sector).
  
  This function controls sectors in following range.
  
  Start sector - Sector (or parameter-sector) that WordAddr is belonged. 
  End sector   - Sector (or parameter-sector) that (WordAddr + WordSize - 1) is belonged. 
  
  This function executes Protect command without checking the protecting status of sector.

  The write or erase command to hyper flash memory will fail if target sector 
  is protected.
  
  This function will fail and return R_HYPB_ERR_COMMAND under following conditions.
  - SectorSize(@ <r_hypb_Config_t>) = 0.
  - Protect(@ <r_hypb_Command_t>) command is not supported.

  This function calls <R_HYPB_Sys_Lock> and <R_HYPB_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_HYPB_Sys_Lock> and <R_HYPB_Sys_Unlock>.

  Parameter:
  Unit     - Instance number.
  WordAddr - Protect control start word-address of the hyper flash memory.
  WordSize - Data word-size to protect.
  Mode     - Protection mode. See <r_hypb_ProtectionMode_t>.

  Return value:
  R_HYPB_ERR_OK               - No error has occurred.
  R_HYPB_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  R_HYPB_ERR_RANGE_UNIT       - Unit number is out of range.
  R_HYPB_ERR_RANGE_PARAM      - A parameter is out of range.
  R_HYPB_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_HYPB_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_HYPB_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  R_HYPB_ERR_COMMAND          - A command is not supported.
  
  See <r_hypb_Error_t>.

  Executable Status:
  Uninitialized - NG 
  Initialized   - NG 
  Idle          - OK 
  Executing     - NG 

  Executable Connected Device:
  Hyper Flash   - OK 
  Hyper RAM     - NG 

*/
r_hypb_Error_t R_HYPB_DataProtect(const uint32_t                Unit,
                                  const uint32_t                WordAddr, 
                                  const uint32_t                WordSize,
                                  const r_hypb_ProtectionMode_t Mode);


/***************************************************************************
  Function: R_HYPB_DataErase

  Description:
  This function erases the data in the hyper flash memory.

  This function executes Erase(@ <r_hypb_Command_t>) command.

  This function assumes that Erase command can be erased in a unit of sector (or 
  parameter-sector).
  
  This function erases the sectors in following range.
  
  Start sector - Sector (or parameter-sector) that WordAddr is belonged. 
  End sector   - Sector (or parameter-sector) that (WordAddr + WordSize - 1) is belonged. 
  
  This function will fail and return R_HYPB_ERR_COMMAND under following conditions.
  - SectorSize(@ <r_hypb_Config_t>) = 0.
  - Erase(@ <r_hypb_Command_t>) command is not supported.
  - Multiple sectors are specified and ReadSts(@ <r_hypb_Command_t>) command is 
    not supported.

  This function calls <R_HYPB_Sys_Lock> and <R_HYPB_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_HYPB_Sys_Lock> and <R_HYPB_Sys_Unlock>.

  *[Wait mode]*

  If ReadSts(@ <r_hypb_Command_t>) command is supported, this function waits to finish the 
  erasing process and checks the result. 

  Actual erased size is stored to ResultSize.

  Result check process is dependent on <R_HYPB_Sys_StatusRegCheck>. 

  *[No-Wait mode]*
  
  If ReadSts(@ <r_hypb_Command_t>) command is not supported, this function does not wait to 
  finish the erasing process and dose not check the result. 

  After one Erase command sequence is finished, this function will return R_HYPB_ERR_OK 
  and the size that is set to command is stored to ResultSize.

  Parameter:
  Unit         - Instance number.
  WordAddr     - Erase start word-address of the hyper flash memory.
  WordSize     - Data word-size to erase.
  ResultSize   - Pointer to variable that receives the word-size erased by this function.
                 If not required, please set to R_NULL.
  
  Return value:
  R_HYPB_ERR_OK               - No error has occurred.
  R_HYPB_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  R_HYPB_ERR_RANGE_UNIT       - Unit number is out of range.
  R_HYPB_ERR_RANGE_PARAM      - A parameter is out of range.
  R_HYPB_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_HYPB_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_HYPB_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  R_HYPB_ERR_COMMAND          - A command is not supported.
  R_HYPB_ERR_PROTECTED        - A process is aborted because of memory protection.
  R_HYPB_ERR_ABORTED          - A process is aborted because of other reason.
  R_HYPB_ERR_TIMEOUT          - Status polling is timeout.
  
  See <r_hypb_Error_t>.

  Executable Status:
  Uninitialized - NG 
  Initialized   - NG 
  Idle          - OK 
  Executing     - NG 

  Executable Connected Device:
  Hyper Flash   - OK 
  Hyper RAM     - NG 

*/
r_hypb_Error_t R_HYPB_DataErase(const uint32_t        Unit, 
                                const uint32_t        WordAddr, 
                                const uint32_t        WordSize, 
                                      uint32_t* const ResultSize );

/***************************************************************************
  Function: R_HYPB_DataWrite

  Description:
  This function writes data to the hyper flash memory.

  In order to write data to Hyper Flash, the data of the sector must have been 
  erased previously.
  
  This function executes WriteBufFlash(@ <r_hypb_Command_t>) or Write(@ <r_hypb_Command_t>) 
  command under following condiion. 

  - If Write command is supported, Write command is used only.
  - If Write command is not supported, WriteBufFlash command is used only.
  - If both commands are not supported, this function will fail and return R_HYPB_ERR_COMMAND.

  When Write command is used and Write command doesn't have R_HYPB_CMD_WRITE_A option, 
  this function has the possibility that the processing takes time.
  Therefore, <R_HYPB_Sys_Relax> is executed in units of RelaxSize(@ r_hypb_Config_t).

  This function calls <R_HYPB_Sys_Lock> and <R_HYPB_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_HYPB_Sys_Lock> and <R_HYPB_Sys_Unlock>.

  *[Wait mode]*

  If ReadSts(@ <r_hypb_Command_t>) command is supported, this function waits to finish the 
  writing process and checks the result. 

  Actual written size is stored to ResultSize.

  Result check process is dependent on <R_HYPB_Sys_StatusRegCheck>. 

  *[No-Wait mode]*

  If ReadSts(@ <r_hypb_Command_t>) command is not supported, this function does not wait to 
  finish the writing process and dose not check the result. 
  
  If WriteBufFlash command is used, this function will return R_HYPB_ERR_OK after only one 
  WriteBufFlash command sequence is issued. 

  If Write command is used and Write command has R_HYPB_CMD_WRITE_A option, 
  this function will return R_HYPB_ERR_OK after only one Write command sequence is issued. 
  
  If Write command is used and Write command doesn't have R_HYPB_CMD_WRITE_A option, 
  Write command is executed continuously without status check.
  
  The size that is set to command is stored to ResultSize.

  Parameter:
  Unit        - Instance number.
  WordAddr    - The parameter specifies the write word-address of the hyper flash memory.
  Buf         - This is a pointer to the buffer stored write data.
  WordSize    - The parameter specifies the data word-size to write.
  ResultSize  - Pointer to variable that receives the word-size written by this function.
                If not required, please set to R_NULL.

  Return value:
  R_HYPB_ERR_OK               - No error has occurred.
  R_HYPB_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  R_HYPB_ERR_RANGE_UNIT       - Unit number is out of range.
  R_HYPB_ERR_RANGE_PARAM      - A parameter is out of range.
  R_HYPB_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_HYPB_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_HYPB_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  R_HYPB_ERR_COMMAND          - A command is not supported.
  R_HYPB_ERR_PROTECTED        - A process is aborted because of memory protection.
  R_HYPB_ERR_ABORTED          - A process is aborted because of other reason.
  R_HYPB_ERR_TIMEOUT          - Status polling is timeout.
  
  See <r_hypb_Error_t>.
  
  Executable Status:
  Uninitialized - NG 
  Initialized   - NG 
  Idle          - OK 
  Executing     - NG 

  Executable Connected Device:
  Hyper Flash   - OK 
  Hyper RAM     - NG 
  
*/
r_hypb_Error_t R_HYPB_DataWrite(const uint32_t        Unit,
                                const uint32_t        WordAddr,
                                const uint8_t*        Buf,
                                const uint32_t        WordSize,
                                      uint32_t* const ResultSize);


/***************************************************************************
  Function: R_HYPB_DataRead

  Description:
  This function reads data from the hyper flash memory.

  This function will fail and return R_HYPB_ERR_COMMAND under following conditions.
  - Read(@ <r_hypb_Command_t>) command is not supported.

  This function has the possibility that the processing takes time.
  Therefore, <R_HYPB_Sys_Relax> is executed in units of RelaxSize(@ r_hypb_Config_t).

  This function calls <R_HYPB_Sys_Lock> and <R_HYPB_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_HYPB_Sys_Lock> and <R_HYPB_Sys_Unlock>.

  Parameter:
  Unit     - Instance number.
  WordAddr - The parameter specifies the read word-address of the hyper flash memory.
  Buf      - This is a pointer to the buffer to store the read data. 
  WordSize - The parameter specifies the data word-size to read.

  Return value:
  R_HYPB_ERR_OK               - No error has occurred.
  R_HYPB_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  R_HYPB_ERR_RANGE_UNIT       - Unit number is out of range.
  R_HYPB_ERR_RANGE_PARAM      - A parameter is out of range.
  R_HYPB_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_HYPB_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_HYPB_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  R_HYPB_ERR_COMMAND          - A command is not supported.
  
  See <r_hypb_Error_t>.
  
  Executable Status:
  Uninitialized - NG 
  Initialized   - NG 
  Idle          - OK 
  Executing     - NG 

  Executable Connected Device:
  Hyper Flash   - OK 
  Hyper RAM     - NG 
  
*/
r_hypb_Error_t R_HYPB_DataRead(const uint32_t       Unit,
                               const uint32_t       WordAddr,
                                     uint8_t* const Buf,
                               const uint32_t       WordSize);

/***************************************************************************
  Function: R_HYPB_UserCmdIssue

  Description:
  This function executes user command sequence.

  Following <Command Option> are available.
  - R_HYPB_CMD_WRITE16
  - R_HYPB_CMD_READ16
  - R_HYPB_CMD_READ32
  - R_HYPB_CMD_READ64
  
  If read transaction is existed in command sequence, read data will be stored to 
  Buf. Read size is 16bit, 32bit or 64bit. It depends on <Command Option>.
  If BufSize is less than Read size, specified size by BufSize is stored to Buf.

  e.g. When R_HYPB_CMD_READ64 is specified and BufSize = 8, stored data is as follows. 
>        Buf[0] = 1st WordData[15:08] 
>        Buf[1] = 1st WordData[07:00] 
>         :          :
>        Buf[6] = 4th WordData[15:08] 
>        Buf[7] = 4th WordData[07:00] 

  This function calls <R_HYPB_Sys_Lock> and <R_HYPB_Sys_Unlock> internally.
  If this function is executed from the interrupt handler, please be careful of 
  specification of <R_HYPB_Sys_Lock> and <R_HYPB_Sys_Unlock>.

  Parameter:
  Unit        - Instance number.
  CmdSequence - Command sequence. See <r_hypb_CmdSequence_t>.
  Buf         - This is a pointer to the Buffer to store the read data.
                If read transaction is not existed in command sequence, 
                please set to R_NULL.
  BufSize     - Byte-size of data that can be stored to Buf.

  Return value:
  R_HYPB_ERR_OK               - No error has occurred.
  R_HYPB_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  R_HYPB_ERR_RANGE_UNIT       - Unit number is out of range.
  R_HYPB_ERR_RANGE_PARAM      - A parameter is out of range.
  R_HYPB_ERR_NOT_ACCEPTABLE   - A function was called in an incorrect state.
  R_HYPB_ERR_FATAL_OS         - Fatal error has occurred at OS interface.
  R_HYPB_ERR_FATAL_HW         - Fatal error has occurred at H/W.
  R_HYPB_ERR_COMMAND          - A command is not supported.
  
  See <r_hypb_Error_t>.
  
  Executable Status:
  Uninitialized - NG 
  Initialized   - NG 
  Idle          - OK 
  Executing     - NG 

  Executable Connected Device:
  Hyper Flash   - OK 
  Hyper RAM     - OK 
*/
r_hypb_Error_t R_HYPB_UserCmdIssue(const uint32_t                    Unit,
                                   const r_hypb_CmdSequence_t* const CmdSequence,
                                         uint8_t *             const Buf,
                                   const uint32_t                    BufSize);

/***************************************************************************
  Function: R_HYPB_VersionStringGet

  Description:
  This function returns the version string of this HYPB driver.

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
  Hyper Flash   - OK 
  Hyper RAM     - OK
*/
const uint8_t* R_HYPB_VersionStringGet(void);

/***************************************************************************
  Function: R_HYPB_MacroVersionGet

  Description:
  This function returns the major and minor version of the H/W macro.

  Parameter:
  Major - Pointer to store the major version.
  Minor - Pointer to store the minor version.

  Return value:
  R_HYPB_ERR_OK               - No error has occurred.
  R_HYPB_ERR_PARAM_INCORRECT  - A parameter is incorrect.
  
  See <r_hypb_Error_t>.

  Executable Status:
  Uninitialized - OK 
  Initialized   - OK 
  Idle          - OK 
  Executing     - OK 

  Executable Connected Device:
  Hyper Flash   - OK 
  Hyper RAM     - OK
*/
r_hypb_Error_t R_HYPB_MacroVersionGet(uint32_t * const Major,
                                      uint32_t * const Minor);


#ifdef __cplusplus
}
#endif

#endif /* R_HYPB_API_H */

