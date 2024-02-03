/*
****************************************************************************
PROJECT : DDB (Display DataBase) driver
============================================================================ 
DESCRIPTION
database for display timings (ddb)
DESCRIPTION
Driver for the VOWE(Video Output Warping Engine) macro
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

#ifndef R_DDB_API_H_
#define R_DDB_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Title: Display Data Base

  The header file r_ddb_api.h contains the declarations related to 
  display(s).
  
  In the module r_ddb_timing.c, a list of displays are defined.
  This list is not exhaustive and the user can add his own 
  display if it is missing.
*/


/*******************************************************************************
  Section: Global Defines 
  
  R_DDB_DISP_FLAG_RSDS_SHIFT - Bit start position of RSDS flags. See <r_ddb_TimingFlags_t>. 
*/
#define R_DDB_DISP_FLAG_RSDS_SHIFT    (6u)


/*******************************************************************************
  Section: Enumerations 
*/

/*******************************************************************************
  Enum: r_ddb_TimingFlags_t

  DDB Timing Flags.
  List of all available Timing Flags of the Display DataBase

  Values:
  R_DDB_DISP_FLAG_NONE          - empty flag
  R_DDB_DISP_FLAG_VOEN          - This flag is not supported. (Ignore even if set.)
  R_DDB_DISP_FLAG_CSYNC         - This flag is not supported. (Ignore even if set.) 
  R_DDB_DISP_FLAG_NEGCLK        - display requires neg. DCLK (data out with falling edge,
                                                             data stable with rising edge of VOnCLK)
  R_DDB_DISP_FLAG_HSYNC_ACTHI   - use active high level for HSYNC
  R_DDB_DISP_FLAG_VSYNC_ACTHI   - use active high level for VSYNC
  R_DDB_DISP_FLAG_DESYNC_ACTHI  - use active high level for DATE ENABLE SYNC
  R_DDB_DISP_FLAG_RSDS_RBSW     - display requires bit swap (off:even bit first, on:odd bit first)
  R_DDB_DISP_FLAG_RSDS_PHSEL90  - display requires phase shift of 90-degree (default)
  R_DDB_DISP_FLAG_RSDS_PHSEL180 - display requires phase shift of 180-degree 
  R_DDB_DISP_FLAG_RSDS_PHSEL270 - display requires phase shift of 270-degree 
  R_DDB_DISP_FLAG_RSDS_PHSEL360 - display requires phase shift of 360/0-degree 
  R_DDB_DISP_FLAG_RSDS_ENABLE   - display requires RSDS mode 

  Note:
  DO NOT change the bit position relation from R_DDB_DISP_FLAG_RSDS_RBSW to 
  R_DDB_DISP_FLAG_RSDS_ENABLE.
  
  R_DDB_DISP_FLAG_RSDS_RBSW and R_DDB_DISP_FLAG_RSDS_PHSELxx are valid when 
  R_DDB_DISP_FLAG_RSDS_ENABLE is set.
*/

typedef enum
{
    R_DDB_DISP_FLAG_NONE          = (0uL << 0u),
    R_DDB_DISP_FLAG_VOEN          = (1uL << 0u),
    R_DDB_DISP_FLAG_CSYNC         = (1uL << 1u),
    R_DDB_DISP_FLAG_NEGCLK        = (1uL << 2u),
    R_DDB_DISP_FLAG_HSYNC_ACTHI   = (1uL << 3u),
    R_DDB_DISP_FLAG_VSYNC_ACTHI   = (1uL << 4u),
    R_DDB_DISP_FLAG_DESYNC_ACTHI  = (1uL << 5u),
    R_DDB_DISP_FLAG_RSDS_RBSW     = (1uL << (R_DDB_DISP_FLAG_RSDS_SHIFT + 0u)),
    R_DDB_DISP_FLAG_RSDS_PHSEL90  = (0uL << (R_DDB_DISP_FLAG_RSDS_SHIFT + 1u)),
    R_DDB_DISP_FLAG_RSDS_PHSEL180 = (1uL << (R_DDB_DISP_FLAG_RSDS_SHIFT + 1u)),
    R_DDB_DISP_FLAG_RSDS_PHSEL270 = (2uL << (R_DDB_DISP_FLAG_RSDS_SHIFT + 1u)),
    R_DDB_DISP_FLAG_RSDS_PHSEL360 = (3uL << (R_DDB_DISP_FLAG_RSDS_SHIFT + 1u)),
    R_DDB_DISP_FLAG_RSDS_ENABLE   = (1uL << (R_DDB_DISP_FLAG_RSDS_SHIFT + 3u)) 
} r_ddb_TimingFlags_t;

/*******************************************************************************
  Section: Structures
*/

/*******************************************************************************
  Typedef: r_ddb_SubTiming_t

  Sub-Timing information of horizontal / vertical display line.
 
> 
>           +-----------+
>  SYNC     |           |
>  ---------+           +------------------------------------
>                        
>                                +----------------------+
>  DATA EN                       |                      |
>  ------------------------------+                      +----
>                       
>    Front      Sync      Back
>  <-Porch-><---Width--><-Porch->
>  <---- BlankWidth ------------>
>  <------------------------- Total ------------------------>
>
>  BlankWidth = Front Porch + Sync Width + Back Porch
> 
  
  Members:
  Total          - length of total line
  BlankWidth     - length of blanking period
  FrontPorch     - length of front porch
  SyncWidth      - length of active sync pulse
*/     

typedef struct
{
    uint32_t Total;
    uint32_t BlankWidth;
    uint32_t FrontPorch;
    uint32_t SyncWidth;
} r_ddb_SubTiming_t;


/*******************************************************************************
  typedef: r_ddb_Timing_t

  Timing information of display

  Members:
  Name           - display name as unique idendifier a specific display
  ScreenWidth    - width of screen (in pixel)
  ScreenHeight   - height of screen (in pixel)
  H              - horizontal sub-timing see: <r_ddb_SubTiming_t>
  V              - vertical sub-timing see: <r_ddb_SubTiming_t>
  Flags          - Timing Flags see: <r_ddb_TimingFlags_t>
  PixelClock     - pixel clock frequency (in MHz)
*/     

typedef struct
{
    const char          *Name;
    uint32_t            ScreenWidth;
    uint32_t            ScreenHeight;
    r_ddb_SubTiming_t   H;
    r_ddb_SubTiming_t   V;
    uint32_t            Flags;
    uint32_t            PixelClock;
} r_ddb_Timing_t;


/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_DDB_GetDisplayTiming

  Get a timing struct from the database.
  
  Parameters:
  DisplayId    - character string that describes the timing in the database
    
  Returns:
  timing       - timing struct that contains display timing information
                 see <r_ddb_Timing_t> for details 
*/
r_ddb_Timing_t *R_DDB_GetDisplayTiming(const char *DisplayId);


#ifdef __cplusplus
}
#endif

#endif /* R_DDB_API_H_ */

