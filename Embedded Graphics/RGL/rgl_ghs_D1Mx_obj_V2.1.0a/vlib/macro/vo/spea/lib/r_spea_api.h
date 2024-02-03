/*
****************************************************************************
PROJECT : VLIB macro driver

============================================================================ 
DESCRIPTION
Generic part of the macro driver
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

DISCLAIMER

LICENSEE has read, understood and accepted the terms and conditions defined in
the license agreement, especially the usage rights. In any case, it is
LICENSEE's responsibility to make sure that any user of the software complies
with the terms and conditions of the signed license agreement.

****************************************************************************
*/

#ifndef R_SPEA_API_H
#define R_SPEA_API_H

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
  Title: SPEA Driver API 
  
  SPEA (Sprite Engine) driver API.

  SPEA driver controls SPEA H/W macro and accesses only register of SPEA H/W macro.

  An application using SPEA driver should include the following header files.

  * r_typedefs.h
  * r_spea_api.h
  
*/

/*******************************************************************************
  Section: Global Constants 
*/

/*******************************************************************************
  Constant:  R_SPEA_VERSION_HI and R_SPEA_VERSION_LO

  Driver version information  
*/
#define R_SPEA_VERSION_HI       1
#define R_SPEA_VERSION_LO       23

/*******************************************************************************
  Section: Global Types
*/

/*******************************************************************************
  Enum: r_spea_Error_t

  SPEA driver error code.
  
  If an error occures these enums give information about the reason.

  Values:
  R_SPEA_ERR_OK             - No error
  R_SPEA_ERR_NG             - Unspecified error
  R_SPEA_ERR_RANGE          - Out of range error 
  R_SPEA_ERR_PARAM          - Wrong parameter
  R_SPEA_ERR_TIME           - Timeout 
  R_SPEA_ERR_ADDR           - Wrong address (not aligned correctly) 
  R_SPEA_ERR_NOT_ACCEPTABLE - Status invalid
*/

typedef enum
{
    R_SPEA_ERR_OK             = 0x00u,
    R_SPEA_ERR_NG             = 0x01u,
    R_SPEA_ERR_RANGE          = 0x02u,
    R_SPEA_ERR_PARAM          = 0x03u,
    R_SPEA_ERR_TIME           = 0x04u, 
    R_SPEA_ERR_ADDR           = 0x05u,
    R_SPEA_ERR_NOT_ACCEPTABLE = 0x06u
} r_spea_Error_t;

/*******************************************************************************
  Enum: r_spea_Color_t

  RLE color modes

  Values:
  R_SPEA_CM_8BPP      - 8 bit per pixel 
  R_SPEA_CM_18BPP     - 18 bit per pixel 
  R_SPEA_CM_24BPP     - 24 bit per pixel 
  R_SPEA_CM_LIMIT     - delimiter
*/

typedef enum
{
    R_SPEA_CM_8BPP    = 0x00,
    R_SPEA_CM_18BPP   = 0x01,
    R_SPEA_CM_24BPP   = 0x02,
    R_SPEA_CM_LIMIT   = 0x03
} r_spea_Color_t;


/*******************************************************************************
  Enum: r_spea_Unit_t

  List of Units in one Sprite engine macro  

  Values:
  R_SPEA_RLE0         - RLE unit 0 
  R_SPEA_RLE1         - RLE unit 1 (Available depending on the RH850/D1x device)
  R_SPEA_RLE2         - RLE unit 2 (Available depending on the RH850/D1x device)
  R_SPEA_RLE3         - RLE unit 3 (Available depending on the RH850/D1x device)
  R_SPEA_SU_OFFSET    - Offset for correct index calculation
  R_SPEA_SU0          - Sprite unit 0 
  R_SPEA_SU1          - Sprite unit 1 
  R_SPEA_SU2          - Sprite unit 2 
  R_SPEA_SU3          - Sprite unit 3 (Available depending on the RH850/D1x device)
  R_SPEA_SU_LAST      - delimiter 
  
*/

typedef enum
{
    R_SPEA_RLE0       = 0x00u,
    R_SPEA_RLE1       = 0x01u,
    R_SPEA_RLE2       = 0x02u,
    R_SPEA_RLE3       = 0x03u,
    R_SPEA_SU_OFFSET  = 0x04u,
    R_SPEA_SU0        = 0x05u,
    R_SPEA_SU1        = 0x06u,
    R_SPEA_SU2        = 0x07u,
    R_SPEA_SU3        = 0x08u,
    R_SPEA_SU_LAST    = 0x09u
} r_spea_Unit_t;


/*******************************************************************************
  Enum: r_spea_UnitCapbs_t

  Capability per Unit

  Values:
  R_SPEA_SPEACAPBS_RLE    - select RLE unit
  R_SPEA_SPEACAPBS_SPRITE - select Sprite unit
  R_SPEA_SPEACAPBS_LAST   - delimiter

*/
typedef enum
{
    R_SPEA_SPEACAPBS_RLE    = 0x00u,
    R_SPEA_SPEACAPBS_SPRITE = 0x01u,
    R_SPEA_SPEACAPBS_LAST   = 0x02u
} r_spea_UnitCapbs_t;



/*******************************************************************************
  Struct: r_spea_SpriteConfig_t

  Configuration data for a Sprite data

  Members:
  Enable              - 1: Sprite enabled/visible, 0: disabled/not visible
  VSync               - The signal to be used for register update. 0: VUPDATE0  1: VUPDATE1 
  SrcAddr             - Start address of the Sprite data. It should be 8 byte-aligned.
  Height              - Height of the Sprite data in pixels.
  Width               - Width  of the Sprite data in pixels. It should be multiple of 8 Bytes.
  PosX                - X Position in pixel on the virtual layer. It should be multiple of 8 Bytes.
  PosY                - Y Position in pixel on the virtual layer. 
*/
typedef struct 
{
    uint8_t            Enable; 
    uint8_t            VSync; 
    uint32_t           SrcAddr;
    uint16_t           Height; 
    uint16_t           Width; 
    uint16_t           PosX; 
    uint16_t           PosY; 
} r_spea_SpriteConfig_t; 


/*******************************************************************************
  Struct: r_spea_RleConfig_t

  Configuration data for a RLE unit

  Members:
  VirtStar            - Start address on the virtual layer. It should be fixed to 0x30000000.
  MemStart            - Start address assigned for RLE compressed data. 
                        It should be 8 byte-aligned.
  ColorMode           - Bits per pixel of RLE data from the following. see <r_spea_Color_t> 
  VideoOutID          - Enable / Disable of RLE engine. Only lower 4 bits are valid.
                        For RLE engine0,  0:enable, 3-15:disable, 1-2: setting prohibit 
                        For RLE engine1,  1:enable, 3-15:disable, 0,2: setting prohibit 
*/

typedef struct 
{
    uint32_t          VirtStart; 
    uint32_t          MemStart; 
    r_spea_Color_t    ColorMode; 
    uint8_t           VideoOutID; 
} r_spea_RleConfig_t;

/*******************************************************************************
  Section: Global API Functions
*/

/*******************************************************************************
  Function: R_SPEA_GetVersionStr
   
  Description: 
  Get the driver version number in human readable form (string). 

  Parameters:
  void
  
  Returns:
  Pointer to receive string destination
  
*/

const int8_t * R_SPEA_GetVersionStr(void);

/*******************************************************************************
  Function: R_SPEA_GetMajorVersion

  Description:
  Get the driver major version number. 

  Parameters:
  void
  
  Returns:
  Major version number
  
*/
uint16_t R_SPEA_GetMajorVersion(void); 

/*******************************************************************************
  Function: R_SPEA_GetMinorVersion

  Description:
  Get the driver minor version number. 

  Parameters:
  void
  
  Returns:
  Minor version number
  
*/
uint16_t R_SPEA_GetMinorVersion(void); 

/*******************************************************************************
  Function: R_SPEA_Init

  Description:
  This function initializes the SPEA driver.
  This function calls <R_SPEA_SYS_HardwareInit> to initialized other than SPEA H/W.
  If the function successfully executes, the return code will be R_SPEA_ERR_OK and 
  the state of the SPEA unit will be in the Idle state.

  Parameters:
  Unit          - SPEA unit number.
 
  Returns:
  see: <r_spea_Error_t>
*/

r_spea_Error_t R_SPEA_Init(const uint32_t Unit);


/*******************************************************************************
  Function: R_SPEA_DeInit

  Description:
  This function de-initializes the SPEA driver.
  This function calls <R_SPEA_SYS_HardwareDeInit> to de-initialize other than SPEA H/W.
  If the function successfully executes, the return code will be R_SPEA_ERR_OK and the 
  state of the SPEA unit will be in the DeInit state.

  Parameters:
  Unit       - SPEA unit number.

  Returns:
  see: <r_spea_Error_t>
*/

r_spea_Error_t R_SPEA_DeInit(const uint32_t Unit);


/*******************************************************************************
  Function: R_SPEA_SetSprite

  Description:
  This function sets configuration for the specified Sprite data.
  Selectable Sprite unit number is depending on the RH850/D1x device.

  Parameters:
  Unit          - SPEA unit number. 
  SpEng         - Sprite unit number. Range is 0 - 3.
  SpNum         - Index number of Sprite data. Range is 0 - 15.
  SpConf        - Pointer to sprite configuration data. See <r_spea_SpriteConfig_t> 
  DispBpp       - Bits per pixel for the color format. 
                  Range is 1 - 32. It should be a power of 2.
 
  Returns:
  see: <r_spea_Error_t>
*/

r_spea_Error_t R_SPEA_SetSprite(const uint32_t                Unit, 
                                const uint8_t                 SpEng, 
                                const uint8_t                 SpNum, 
                                const r_spea_SpriteConfig_t*        SpConf,
                                const uint8_t                 DispBpp);


/*******************************************************************************
  Function: R_SPEA_SetRle

  Description:
  This function sets configuration of the specified RLE engine.
  Selectable RLE unit number is depending on the RH850/D1x device.
  
  VideoOutID is the member of RleConf which always make sure to assign different VideoOutID 
  for two RLE engines. This function checks two VideoOutID and returns with an error if 
  they are the same. Note that default VideoOutID value of all RLE engines is 0.
  
  Parameters:
  Unit           - RLE unit number. Range is 0 - 3.
  RleNum         - RLE engine number. Range is 0 - 1.
  RleConf        - Pointer to RLE configuration data. See <r_spea_RleConfig_t> 
  
  Returns:
  see: <r_spea_Error_t>
*/

r_spea_Error_t R_SPEA_SetRle(const uint32_t                Unit, 
                             const uint8_t                 RleNum, 
                             const r_spea_RleConfig_t*     RleConf);

/*******************************************************************************
  Function: R_SPEA_UnitEnable

  Description:
  This function enables or disables the specified RLE unit.
  Selectable RLE unit number is depending on the RH850/D1x device.

  Parameters:
  Unit          - RLE unit number. Range is 0 - 3.
  SpUnit        - RLE unit number from the following. see <r_spea_Unit_t>
                    R_SPEA_RLE0 /
                    R_SPEA_RLE1 /
                    R_SPEA_RLE2 /
                    R_SPEA_RLE3
  Enable        - 0: disable, 1: enable
 
  Returns:
  see: <r_spea_Error_t>
*/

r_spea_Error_t R_SPEA_UnitEnable(const uint32_t            Unit, 
                                 const r_spea_Unit_t       SpUnit, 
                                 const uint8_t             Enable);

/*******************************************************************************
  Function: R_SPEA_SpriteEnable

  Description:
  This function enables or disables the blit of Sprite data.
  Selectable Sprite unit number is depending on the RH850/D1x device.

  Parameters:
  Unit          - SPEA unit number.
  SpEng         - Sprite unit number. Range is 0 - 3.
  SpNum         - Index number of Sprite data. Range is 0 - 15.
  Enable        - 0: disable, 1: enable
 
  Returns:
  see: <r_spea_Error_t>
*/

r_spea_Error_t R_SPEA_SpriteEnable(const uint32_t      Unit, 
                                   const uint8_t       SpEng, 
                                   const uint8_t       SpNum, 
                                   const uint8_t       Enable);


/*******************************************************************************
  Function: R_SPEA_SetSpritePos

  Description:
  This function updates the blit position of the specified Sprite data.
  Selectable Sprite unit number is depending on the RH850/D1x device.

  Parameters:
  Unit          - SPEA unit number.
  SpEng         - Sprite unit number. Range is 0 - 3.
  SpNum         - Index number of Sprite data. Range is 0 - 15.
  PosX          - Horizontal position in pixel 
  PosY          - Vertical position in pixel 
  DispBpp       - Bits per pixel for color format. 
                  Range is 1 - 32. It should be a power of 2.
  
  Returns:
  see: <r_spea_Error_t>
*/

r_spea_Error_t R_SPEA_SetSpritePos(const uint32_t      Unit, 
                                   const uint8_t       SpEng, 
                                   const uint8_t       SpNum, 
                                   const uint16_t      PosX,
                                   const uint16_t      PosY,
                                   const uint8_t       DispBpp);

/*******************************************************************************
  Function: R_SPEA_UnitUpdateActive

  Description:
  This function gets the information if a register update is ongoing or finished for the 
  specified RLE / Sprite unit.
  Selectable RLE / Sprite unit number is depending on the RH850/D1x device.  

  If error occurs in this function, the return code is 0 and following error code is notified 
  by error callback.
  R_SPEA_ERR_RANGE          - Parameter is invalid.
  R_SPEA_ERR_NOT_ACCEPTABLE - SEPA unit status is invalid.

  Parameters:
  Unit          - RLE unit number in case of RLE unit control. Range is 0 - 3.
                  SPEA unit number in case of Sprite unit control. Range is only 0.
  SpUnit        - RLE / Sprite unit number from followings. see <r_spea_Unit_t>
                    R_SPEA_RLE0 /
                    R_SPEA_RLE1 /
                    R_SPEA_RLE2 /
                    R_SPEA_RLE3 /
                    R_SPEA_SU0  /
                    R_SPEA_SU1  /
                    R_SPEA_SU2  /
                    R_SPEA_SU3

  Returns:
  == 0 - Update finished or update is not started. 
  != 0 - Register update in progress.
*/

uint32_t R_SPEA_UnitUpdateActive(const uint32_t Unit,  const r_spea_Unit_t SpUnit);  

/*******************************************************************************
  Function: R_SPEA_UpdateUnit

  Description:
  This function sets the register update request for the specified RLE or Sprite unit.
  If the function successfully executes, the return code will be R_SPEA_ERR_OK and the state 
  of the SPEA unit will be in the Updating state.
  The actual register update will be executed after VUPDATE0 or VUPDATE1 signal occurs.
  
  Parameters:
  Unit          - RLE unit number in case of RLE unit control. Range is 0 - 3.
                  SPEA unit number in case of Sprite unit control. Range is only 0.
  SpUnit        - RLE / Sprite unit number from followings. see <r_spea_Unit_t>
                    R_SPEA_RLE0 /
                    R_SPEA_RLE1 /
                    R_SPEA_RLE2 /
                    R_SPEA_RLE3 /
                    R_SPEA_SU0  /
                    R_SPEA_SU1  /
                    R_SPEA_SU2  /
                    R_SPEA_SU3
  VSync         - VUPDATE0 or VUPDATE1 to be used for update. 0:VUPDATE0, 1:VUPDATE1

  Returns:
  see: <r_spea_Error_t>
*/

r_spea_Error_t R_SPEA_UpdateUnit(const uint32_t       Unit, 
                                 const r_spea_Unit_t  SpUnit, 
                                 const uint8_t        VSync);  

/*******************************************************************************
  Function: R_SPEA_SetErrorCallback

  Description:
  This function installs the Error callback function.
  When some error occurs in SPEA driver, SPEA driver invokes the callback function.
  The error callback is notified during SPEA unit is not DeInit state.
  The installed error callback can be uninstalled by <R_SPEA_Init> or R_NULL setting in this 
  function.

  Since there is no asynchronous processing in SPEA driver, it is possible to detect an error by 
  checking the return code of each API function.

  Parameters:
  ErrorCallback - Pointer to a user function
  
  Returns:
  void
*/


void R_SPEA_SetErrorCallback(void (*ErrorCallback )(uint32_t Unit, uint32_t Error));

/*******************************************************************************
  Function: R_SPEA_UnitCapabilitiesSet

  Description:
  This function selects the valid Sprite unit number and RLE unit number.
  The validity of this function is depending on RH850/D1x device.
  Calling this function is only possible with VDCE/VOWE disabled.
  
  Parameter:
  Unit0 - Selects RLE unit 0 or Sprite unit3
          Selected unit is connected to VDCE0 Scaler0 and VDCE1 Scaler0.
  Unit1 - Selects RLE unit 1 or Sprite unit 0
          Selected unit is connected to VDCE0 Scaler1 and VDCE1 Image Synthesizer3.
  Unit2 - Selects RLE unit 2 or Sprite unit 1
          Selected unit is connected to VDCE0 Image Synthesizer2 and VDCE1 Image
          Synthesizer2.
  Unit3 - Selects RLE unit 3 or Sprite unit 2
          Selected unit is connected to VDCE0 Image Synthesizer3 and VDCE1 Scaler1.

  Return value:
  See <r_spea_Error_t>.

  Supported Device:
  RH850/D1M2(H) - NG 
  RH850/D1M2    - NG 
  RH850/D1M1A   - OK 
  RH850/D1M1-V2 - OK 
  RH850/D1M1(H) - NG 
  RH850/D1L2(H) - NG 
*/

r_spea_Error_t R_SPEA_UnitCapabilitiesSet(const r_spea_UnitCapbs_t Unit0,
                                          const r_spea_UnitCapbs_t Unit1,
                                          const r_spea_UnitCapbs_t Unit2,
                                          const r_spea_UnitCapbs_t Unit3);

#ifdef __cplusplus
}
#endif


#endif /* R_SPEA_API_H  */

