/*
****************************************************************************
PROJECT : WM driver
============================================================================ 
DESCRIPTION
Driver for the Window Manager
============================================================================
                            C O P Y R I G H T
============================================================================
                       Copyright (c) 2013 - 2020
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

***************************************************************************
*/

#ifndef R_WM_API_H
#define R_WM_API_H

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Title: WM API
  
  WM (Window Manager) API.

  WM controls VDCE and SPEA drivers via WM support functions, 
  and WM doesn't accesses H/W register directly.

  An application using Window Manager API should include the following header files.

  * r_typedefs.h
  * r_ddb_api.h
  * r_wm_api.h
  * r_cdi_api.h (by default. It can be customiszed. Please see <R_WM_Sys_Heap_Set>.)

*/

/***********************************************************
  Constants: API Version

  High and Low number of the API version

  - R_WM_VERSION_HI
  - R_WM_VERSION_LO
*/
#define R_WM_VERSION_HI  1
#define R_WM_VERSION_LO  21

/***************************************************************************
 Constants: VOCA CLUT entry number

 The number of VOCA CLUT entries
*/
#define R_WM_VOCA_CLUT_NUM   (4u)

/***********************************************************
  Section: Global Types
*/

/***********************************************************
  Enum: r_wm_Error_t

  Description:
  WM driver error code.
  If an error occurs these enumerations give information about the reason.
*/

typedef enum
{
    R_WM_ERR_OK                                                     = 0,
    R_WM_ERR_NG                                                     = 1,
    R_WM_ERR_RANGE_WM                                               = 2,
    R_WM_ERR_PARAM_INCORRECT                                        = 3,
    R_WM_UNIT_LOCKED                                                = 4,
    R_WM_UNIT_NOTLOCKED                                             = 5,
    R_WM_ERR_NO_PHYS_WINDOW                                         = 6,
    R_WM_ERR_MALLOC_FAILED                                          = 7,
    R_WM_ERR_FREE_FAILED                                            = 8,
    R_WM_ERR_EVENT_FAILED                                           = 9,
    R_WM_ERR_VOUT                                                   = 10,
    R_WM_ERR_NOT_DISABLED                                           = 11,
    R_WM_ERR_NOT_DELETED                                            = 12,
    R_WM_ERR_COLORFMT                                               = 13,
    R_WM_ERR_VIN                                                    = 14,
    R_WM_ERR_NOT_SPRITE_WINDOW                                      = 15,
    R_WM_ERR_DISPLAY_TIMING_SET                                     = 16,
    R_WM_ERR_INVALID_WM_UNIT                                        = 17,
    R_WM_ERR_DEV_DEINIT_FAILED                                      = 18,
    R_WM_ERR_NULL_PTR_ARGUMENT                                      = 19,
    R_WM_ERR_SPRITE_NOT_FOUND                                       = 20,
    R_WM_ERR_SYS_LAYER_INIT_FAILURE                                 = 21,
    R_WM_ERR_NOT_CLUT_WIN_FMT                                       = 22,
    R_WM_ERR_NO_SUCH_WINDOW                                         = 23,
    R_WM_ERR_COULD_NOT_ENABLE_SCREEN                                = 24,
    R_WM_ERR_COULD_NOT_DISABLE_SCREEN                               = 25,
    R_WM_ERR_COULD_NOT_SET_SCREEN_BG_COLOR                          = 26,
    R_WM_ERR_COULD_NOT_REGISTER_EVENT                               = 27,
    R_WM_ERR_NOT_FB_WINDOW                                          = 28,
    R_WM_ERR_SYNC_MODE_NOT_POSSIBLE                                 = 29,
    R_WM_ERR_SYS_WIN_ALPHA_SET_FAILED                               = 30,
    R_WM_ERR_SYS_WIN_DELETE_ALL_SPRITES_FAILED                      = 31,
    R_WM_ERR_NOT_SUITABLE_CAPTURE_WINDOW                            = 32,
    R_WM_ERR_COULD_NOT_WRITE_MSG_QUEUE                              = 33,
    R_WM_ERR_SYS_CAPTURE_CREATE_FAILED                              = 34,
    R_WM_ERR_SYS_CAPTURE_DELETE_FAILED                              = 35,
    R_WM_ERR_SYS_CAPTURE_ENABLE_FAILED                              = 36,
    R_WM_ERR_CAPTURE_UNIT_COUNT_EXCEEDED                            = 37,
    R_WM_ERR_VOUT_INTERNAL                                          = 38,
    R_WM_ERR_SCREEN_TIMING_NOT_SET                                  = 39,
    R_WM_ERR_NOT_INITIALIZED                                        = 40,
    R_WM_ERR_NOT_UNINITIALIZED                                      = 41,
    R_WM_ERR_COLOR_KEYING_NOT_SUPPORTED_FOR_LAYER                   = 42,
    R_WM_ERR_ADDRESS_ALIGNMENT                                      = 43,
    R_WM_ERR_DISPLAY_OUTPUT_FORMAT_SET                              = 44,
    R_WM_ERR_SYS_WIN_SWAP_FAILED                                    = 45,
    R_WM_ERR_SYS_WIN_DELETE_FAILED                                  = 46,
    R_WM_ERR_SYS_WIN_EXTERNAL_BUF_SET_FAILED                        = 47,
    R_WM_ERR_SYS_WIN_CREATE_FAILED                                  = 48,
    R_WM_ERR_SYS_WIN_MOVE_FAILED                                    = 49,
    R_WM_ERR_SYS_WIN_GEOMETRY_SET_FAILED                            = 50,
    R_WM_ERR_WIN_SWAP_FAILED                                        = 51,
    R_WM_ERR_SPEA_INTERNAL                                          = 52,
    R_WM_ERR_SYS_WIN_FLAG_UPDATE_FAILED                             = 53,
    R_WM_ERR_COULD_NOT_SET_SCREEN_COLOR_CURVE                       = 54,
    R_WM_ERR_COULD_NOT_SET_SCREEN_GAMMA                             = 55,
    R_WM_ERR_NOT_SUPPORTED                                          = 56,
    R_WM_ERR_SYS_WIN_SCALED_SET_FAILED                              = 57,
    R_WM_ERR_SYS_CAPTURE_MOVE_FAILED                                = 58,
    R_WM_ERR_SYS_CAPTURE_RESIZE_FAILED                              = 59,
    R_WM_ERR_SYS_CAPTURE_SET_VSYNC_FAILED                           = 60,
    R_WM_ERR_SYS_VOCA_CREATE_FAILED                                 = 61,
    R_WM_ERR_SYS_VOCA_DELETE_FAILED                                 = 62,
    R_WM_ERR_SYS_VOCA_ENABLE_FAILED                                 = 63,
    R_WM_ERR_SYS_VOCA_SET_FAILED                                    = 64,
    R_WM_ERR_SYS_ACT_MON_FAILED                                     = 65,
    R_WM_ERR_SYS_DISCOM_CREATE_FAILED                               = 66,
    R_WM_ERR_SYS_DISCOM_DELETE_FAILED                               = 67,
    R_WM_ERR_SYS_DISCOM_ENABLE_FAILED                               = 68,
    R_WM_ERR_SYS_DISCOM_SET_FAILED                                  = 69,
    R_WM_ERR_SYS_DISCOM_GET_FAILED                                  = 70,
    R_WM_ERR_VOCA_NOT_FOUND                                         = 71,
    R_WM_ERR_DISCOM_NOT_FOUND                                       = 72,
    R_WM_ERR_VOCA_INTERNAL                                          = 73,
    R_WM_ERR_DISCOM_INTERNAL                                        = 74,
    R_WM_ERR_LAST
}  r_wm_Error_t;

/***********************************************************
  Enum: r_wm_MsgId_t

  Description:
  The type is used to specify the messages, which are processed by the window manager.

  Members:

  see also <r_wm_Msg_t>.

*/

typedef enum
{
    R_WM_MSG_SCREEN_ENABLE,             /* 0 */
    R_WM_MSG_SCREEN_BG_COLOR_SET,
    R_WM_MSG_SCREEN_COLOR_CURVE_SET,
    R_WM_MSG_SCREEN_GAMMA_SET,

    R_WM_MSG_WIN_CREATE,                /* 4 */
    R_WM_MSG_WIN_DELETE,
    R_WM_MSG_WIN_ENABLE,
    R_WM_MSG_WIN_MOVE,
    R_WM_MSG_WIN_RESIZE,
    R_WM_MSG_WIN_SCALED_SIZE_SET,
    R_WM_MSG_WIN_COLOR_FMT_SET,
    R_WM_MSG_WIN_ALPHA_SET,
    R_WM_MSG_WIN_PREMULT_ALPHA_ENABLE,
    R_WM_MSG_WIN_CLUT_SET,
    R_WM_MSG_WIN_EXTERNAL_BUF_SET,
    R_WM_MSG_WIN_COLOR_KEY_ENABLE,
    R_WM_MSG_WIN_FLAGS_UPDATE,
    R_WM_MSG_WIN_PAINT,
    R_WM_MSG_WIN_DELETE_ALL_SPRITES,
    R_WM_MSG_WIN_SWAP,

    R_WM_MSG_SPRITE_CREATE,             /* 20 */
    R_WM_MSG_SPRITE_DELETE,
    R_WM_MSG_SPRITE_ENABLE,
    R_WM_MSG_SPRITE_MOVE,
    R_WM_MSG_SPRITE_BUF_SET,
    
    R_WM_MSG_CAPT_CREATE,               /* 25 */
    R_WM_MSG_CAPT_DELETE,
    R_WM_MSG_CAPT_ENABLE,
    R_WM_MSG_CAPT_MOVE,
    R_WM_MSG_CAPT_RESIZE,
    R_WM_MSG_CAPT_SCALED_SIZE_SET,
    R_WM_MSG_CAPT_EXT_VSYNC_SET,

    R_WM_MSG_DISCOM_CREATE,             /* 32 */
    R_WM_MSG_DISCOM_DELETE,
    R_WM_MSG_DISCOM_ENABLE,
    R_WM_MSG_DISCOM_CRC_SET,
    
    R_WM_MSG_SCREEN_VOCA_INIT,          /* 36 */
    R_WM_MSG_SCREEN_VOCA_DEINIT,
    R_WM_MSG_SCREEN_VOCA_CREATE,
    R_WM_MSG_SCREEN_VOCA_DELETE,
    R_WM_MSG_SCREEN_VOCA_ENABLE,
    R_WM_MSG_SCREEN_VOCA_EXP_IMG_SET,
    R_WM_MSG_SCREEN_VOCA_CLUT_SET,
    R_WM_MSG_SCREEN_ACT_MON_ENABLE,
    
    R_WM_MSG_EOF,                       /* 44 */
    R_WM_MSG_VBLANK,

    R_WM_MSG_LAST
} r_wm_MsgId_t;


/***********************************************************
  Enum: r_wm_WinMode_t

  Description:
  Window mode.
  
  Members:
  R_WM_WINMODE_FB       - The window is a frame buffer window.
  R_WM_WINMODE_SPRITES  - The window is a Sprite-hosting window.

*/

typedef enum
{
    R_WM_WINMODE_FB,
    R_WM_WINMODE_SPRITES
} r_wm_WinMode_t;

/***********************************************************
  Enum: r_wm_WinStatus_t

  Description:
  Window status.
  
  Members:
  R_WM_WINSTATUS_NOTINITIALIZED - The window has not been created.
  R_WM_WINSTATUS_DISABLED       - The window is invisible on the screen.
  R_WM_WINSTATUS_ENABLED        - The window is visible on the screen.
*/

typedef enum
{
    R_WM_WINSTATUS_NOT_INITIALIZED = 0,
    R_WM_WINSTATUS_DISABLED,
    R_WM_WINSTATUS_ENABLED
} r_wm_WinStatus_t;

/***********************************************************
  Enum: r_wm_WinBufStatus_t

  Description:
  Frame buffer status.

  Members:
  R_WM_WINBUF_FREE            - The buffer can be used for rendering operations 
                                and it is not visible on the screen.
  R_WM_WINBUF_RENDER_STARTED  - Drawing operations have been started on the buffer.
  R_WM_WINBUF_RENDER_FINISHED - The drawing operations in the buffer have been completed 
                                and the buffer is ready to be displayed.
  R_WM_WINBUF_ON_SCREEN       - The buffer is scheduled to be transferred to the screen, 
                                or it is already on the screen.
*/

typedef enum
{
    R_WM_WINBUF_FREE = 0,
    R_WM_WINBUF_RENDER_STARTED,
    R_WM_WINBUF_RENDER_FINISHED,
    R_WM_WINBUF_ON_SCREEN
} r_wm_WinBufStatus_t;

/***********************************************************
  Enum: r_wm_WinBufAllocMode_t

  Description:
  Frame buffer allocation mode.

  Members:
  R_WM_WINBUF_ALLOC_EXTERNAL - External mode. User allocates the frame buffers. 
  R_WM_WINBUF_ALLOC_INTERNAL - Internal mode.
                               <R_WM_WindowCreate> allocates the buffers from heap memory.
*/

typedef enum
{
    R_WM_WINBUF_ALLOC_EXTERNAL,
    R_WM_WINBUF_ALLOC_INTERNAL
} r_wm_WinBufAllocMode_t;

/***********************************************************
  typedef: r_wm_WinBuffer_t

  Description:
  Frame buffer information.

  Struct members:
  Data   - The start address of the frame buffer. It should be 128 Byte alignment.
  Status - The status of the frame buffer. See also <r_wm_WinBufStatus_t>.
*/

typedef struct
{
    void*                  Data;
    r_wm_WinBufStatus_t    Status;
} r_wm_WinBuffer_t;

/***********************************************************
  Enum: r_wm_WinColorFmt_t

  Description:
  AColor format of a window.
  Due to chroma subsampling, two pixels share their u and v data

  Members:
  (A)RGB(w)xyz - The value specifies the number of bits for each colour and the alpha channel.
  
  supported color modes :
  (code)
  R_WM_COLORFMT_RGB565        16bpp                     rrrrrggg gggbbbbb
  R_WM_COLORFMT_ARGB1555      16bpp                     arrrrrgg gggbbbbb
  R_WM_COLORFMT_ARGB4444      16bpp                     aaaarrrr ggggbbbb
  R_WM_COLORFMT_RGB0888       32bpp   -------- rrrrrrrr gggggggg bbbbbbbb
  R_WM_COLORFMT_ARGB8888      32bpp   aaaaaaaa rrrrrrrr gggggggg bbbbbbbb
  R_WM_COLORFMT_RGBA5551      16bpp                     rrrrrggg ggbbbbba
  R_WM_COLORFMT_RGBA8888      32bpp   rrrrrrrr gggggggg bbbbbbbb aaaaaaaa
  R_WM_COLORFMT_YCBCR_444     32bpp   -------- vvvvvvvv yyyyyyyy uuuuuuuu
  R_WM_COLORFMT_YCBCR_422     16bpp   uuuuuuuu yyyyyyyy vvvvvvvv yyyyyyyy
  R_WM_COLORFMT_YUV_YUYV      16bpp   vvvvvvvv yyyyyyyy uuuuuuuu yyyyyyyy
  R_WM_COLORFMT_YUV_UYVY      16bpp   yyyyyyyy vvvvvvvv yyyyyyyy uuuuuuuu
  R_WM_COLORFMT_YUV_YVYU      16bpp   uuuuuuuu yyyyyyyy vvvvvvvv yyyyyyyy
  R_WM_COLORFMT_YUV_VYUY      16bpp   yyyyyyyy uuuuuuuu yyyyyyyy vvvvvvvv
  (end)

  R_WM_COLORFMT_CLUT8          - 8-bit (256 colors) color-lookup index
  R_WM_COLORFMT_CLUT4          - 4-bit (16 colors) color-lookup index
  R_WM_COLORFMT_CLUT1          - 1-bit (2 colors) color lookup index
  R_WM_COLORFMT_RLE24ARGB8888  - 24bit RLE format which is expanded to ARGB8888
  R_WM_COLORFMT_RLE24RGB0888   - 24bit RLE format which is expanded to RGB0888
  R_WM_COLORFMT_RGB0444        - This format is not supported.
  R_WM_COLORFMT_RGBA4444       - This format is not supported.
  R_WM_COLORFMT_RLE18ARGB8888  - This format is not supported.
  R_WM_COLORFMT_RLE18RGB0888   - This format is not supported.
  R_WM_COLORFMT_RLE8CLUT8      - This format is not supported.
  R_WM_COLORFMT_RLE8CLUT4      - This format is not supported.
  R_WM_COLORFMT_RLE8CLUT1      - This format is not supported.

*/

typedef enum
{
    R_WM_COLORFMT_RGB565,
    R_WM_COLORFMT_ARGB1555,
    R_WM_COLORFMT_ARGB4444,
    R_WM_COLORFMT_RGB0444,
    R_WM_COLORFMT_RGB0888,
    R_WM_COLORFMT_ARGB8888,
    R_WM_COLORFMT_RGBA5551,
    R_WM_COLORFMT_RGBA4444,
    R_WM_COLORFMT_RGBA8888,
    R_WM_COLORFMT_CLUT8,
    R_WM_COLORFMT_CLUT4,
    R_WM_COLORFMT_CLUT1,
    R_WM_COLORFMT_RLE24ARGB8888,
    R_WM_COLORFMT_RLE18ARGB8888,
    R_WM_COLORFMT_RLE24RGB0888,
    R_WM_COLORFMT_RLE18RGB0888,
    R_WM_COLORFMT_RLE8CLUT8,
    R_WM_COLORFMT_RLE8CLUT4,
    R_WM_COLORFMT_RLE8CLUT1,
    R_WM_COLORFMT_YCBCR_422,
    R_WM_COLORFMT_YCBCR_444,
    R_WM_COLORFMT_YUV_YUYV,
    R_WM_COLORFMT_YUV_UYVY,
    R_WM_COLORFMT_YUV_YVYU,
    R_WM_COLORFMT_YUV_VYUY,
    R_WM_COLORFMT_LAST
} r_wm_WinColorFmt_t;


/***********************************************************
  Enum: r_wm_OutColorFmt_t

  Description:
  The color mode of the video output and option flags. 

  Members:
  (A)RGB(w)xyz - The value specifies the number of bits for each colour.

  supported color modes :
  (code)
  R_WM_OUTCOLORFMT_RGB888             24bit           rrrrrrrrggggggggbbbbbbbb
  R_WM_OUTCOLORFMT_RGB666             18bit           ------rrrrrrggggggbbbbbb
  R_WM_OUTCOLORFMT_RGB565             16bit           --------rrrrrggggggbbbbb
  (end)
  
  supported flags :
  
  R_WM_OUTCOLORFMT_FLAG_ENDIAN   - change endianess of output colour
  R_WM_OUTCOLORFMT_FLAG_SWAP_BR  - swap blue and red channel of output color
  R_WM_OUTCOLORFMT_FLAG_DITHER   - activate dithering applied to output colour
  
  Result of flags is shown as example for RGB666.
  (code)
                                                      MSB                     LSB
  R_WM_OUTCOLORFMT_FLAG_ENDIAN                        ------ rrrrrr gggggg bbbbbb
                                    FLAG=0:           ------ 765432 765432 765432
                                    FLAG=1:           ------ 234567 234567 234567

                                                      MSB                     LSB
  R_WM_OUTCOLORFMT_FLAG_SWAP_BR                       ------ rrrrrr gggggg bbbbbb
                                    FLAG=0:           ------ hgfedc ba9876 543210
                                    FLAG=1:           ------ 543210 ba9876 hgfedc
  (end)
*/

typedef enum
{
    R_WM_OUTCOLORFMT_RGB888 = 0u,
    R_WM_OUTCOLORFMT_RGB666 = 1u,
    R_WM_OUTCOLORFMT_RGB565 = 2u,
    R_WM_OUTCOLORFMT_LAST   = 3u,
    
    /* Optional flags  */
    R_WM_OUTCOLORFMT_FLAG_DITHER  = (int32_t)(1uL << 28u),
    R_WM_OUTCOLORFMT_FLAG_SWAP_BR = (int32_t)(1uL << 29u),
    R_WM_OUTCOLORFMT_FLAG_ENDIAN  = (int32_t)(1uL << 30u),
    /* We do not use the 31th bit if not needed. Saves some signedness castings */
    R_WM_OUTCOLORFMT_FLAG_MASK = (R_WM_OUTCOLORFMT_FLAG_ENDIAN
                                + R_WM_OUTCOLORFMT_FLAG_SWAP_BR
                                + R_WM_OUTCOLORFMT_FLAG_DITHER)
} r_wm_OutColorFmt_t;

/***********************************************************
  typedef: r_wm_ClutEntry_t
  
  Description:
  Defines an entry of the colour lookup table.
  
  Struct members:
  B -   8-bit blue component
  G -   8-bit green component
  R -   8-bit red component
  A -   8-bit alpha component
  
*/

typedef struct
{
    uint8_t B;
    uint8_t G;
    uint8_t R;
    uint8_t A;
} r_wm_ClutEntry_t;

/***********************************************************
  typedef: r_wm_VocaClutEntry_t

  Description:
  Each CLUT has valid level (range) for a pixel. 
  Set as RGB888 format. Each range is 0 to 255.
  
  For one pixel in the monitor area, 
  if 'R' is the value of the upper 4 bits of the red component of the display signal, 
  the 'Diff' is calculated by following formula.
  
  (code)
  if      (R < (RLower >> 4)) Diff = (RLower >> 4) - R
  else if (R > (RUpper >> 4)) Diff = R - (RUpper >> 4)
  else                        Diff = 0
  (end)
  
  The same applies to the green and blue components.
  R_WM_EVENT_VOCA_MISMACTH event is notified when the sum of the differences of all components 
  of all pixels in the monitor is greater than or equal to the Threshold value.
  
  Members:
  RUpper - Upper value of red   component.
  GUpper - Upper value of green component.
  BUpper - Upper value of blue  component.
  RLower - Lower value of red   component.
  GUpper - Lower value of green component.
  BUpper - Lower value of blue  component.
*/

typedef struct
{
    uint8_t             RUpper;
    uint8_t             GUpper;
    uint8_t             BUpper;
    uint8_t             RLower;
    uint8_t             GLower;
    uint8_t             BLower;
} r_wm_VocaClutEntry_t;

/***********************************************************
  typedef: r_wm_Window_t

  forward declaration for <r_wm_Window_t>.
*/
typedef struct r_wm_Window_s r_wm_Window_t; 

/***********************************************************
  typedef: r_wm_Capture_t

  forward declaration for using <r_wm_Capture_t>
*/
typedef struct r_wm_Capture_s   r_wm_Capture_t;

/***********************************************************
  typedef: r_wm_Msg_t

  forward declaration for using <r_wm_Msg_t>
*/
typedef struct r_wm_Msg_s       r_wm_Msg_t;

/***********************************************************
  typedef: r_wm_Voca_t

  forward declaration for using <r_wm_Voca_t>
*/
typedef struct r_wm_Voca_s      r_wm_Voca_t;

/***********************************************************
  typedef: r_wm_Discom_t

  forward declaration for using <r_wm_Discom_t>
*/
typedef struct r_wm_Discom_s    r_wm_Discom_t;

/***********************************************************
  Enum: r_wm_SpriteStatus_t

  Description:
  Status of the Sprite data.

  Members:
  R_WM_SPRITESTATUS_NOT_INITIALIZED - Sprite status is not initialized.
  R_WM_SPRITESTATUS_DISABLED        - Sprite status is disabled.
  R_WM_SPRITESTATUS_ENABLED         - Sprite status is enabled.
  
*/

typedef enum
{
    R_WM_SPRITESTATUS_NOT_INITIALIZED = 0,
    R_WM_SPRITESTATUS_DISABLED,
    R_WM_SPRITESTATUS_ENABLED
} r_wm_SpriteStatus_t;

/***********************************************************
  typedef: r_wm_Sprite_t

  Description:
  Each sprite has a data structure of this type. 
  All sprites are linked in a queue by the window manager.

  Members:
  Window   - Pointer to <r_wm_Window_t> structure which has been created already.
  Next     - Pointer to the next sprite in the queue. This is used in WM internally.
             User should initialize as R_NULL when <R_WM_SpriteCreate> is called.
  Status   - Status of the Sprite. This is used in WM internally.
             User should initialize as R_WM_SPRITESTATUS_NOT_INITIALIZED
             when <R_WM_SpriteCreate> is called.
  Data     - Start address of Sprite source data. It should be 8 Bytes aligned.
  PosX     - X position of Sprite data. Unit is pixel. It should be 8 Bytes aligned.
  PosY     - Y position of Sprite data. Unit is pixel.
  PosZ     - Z position of Sprite data. Z position is the relative position in the Sprite window.
  Width    - Width of Sprite data. Unit is pixel. It should be 8 Bytes aligned.
  Height   - Height of Sprite data. Unit is pixel.
*/

typedef struct r_wm_Sprite_s
{
    struct r_wm_Window_s *Window;
    struct r_wm_Sprite_s *Next;
    r_wm_SpriteStatus_t  Status;
    const void           *Data;
    uint32_t             PosX;
    uint32_t             PosY;
    uint32_t             PosZ;
    uint32_t             Width;
    uint32_t             Height;
} r_wm_Sprite_t;


/***********************************************************
  Enum: r_wm_WinFlags_t

  Description:
  On/Off Switches for different functionalities of a WM window

  Members:
  R_WM_WINFLAG_NONE     - No flags are set.
  R_WM_WINFLAG_V_MIRROR - Flip the output image vertically.
*/

typedef enum
{
    R_WM_WINFLAG_NONE     = 0,
    R_WM_WINFLAG_V_MIRROR = 1,
    R_WM_WINFLAG_MASK     = (R_WM_WINFLAG_V_MIRROR)
} r_wm_WinFlags_t;

/***********************************************************
  typedef: r_wm_Window_t

  Description:
  Each window has a data structure of this type. 
  All windows are linked in a chain by the window manager.
  
  Struct members:
  Status                - Status of the Window. See <r_wm_WinStatus_t>.
                          This is used in WM internally.
                          User should initialize as R_WM_WINSTATUS_NOT_INITIALIZED 
                          when <R_WM_WindowCreate> is called.
  Mode                  - Window mode. See <r_wm_WinMode_t>.
  ColorFmt              - Color format of the window. See <r_wm_WinColorFmt_t>.
  PosX                  - X position of the window. Unit is pixels. Range is -1280 to 1279.
  PosY                  - Y position of the window. Unit is pixels. Range is -1024 to 1023.
  PosZ                  - Z position of the window.
  Pitch                 - Line pitch of a frame buffer. Unit is pixels.
                          It should be 128 bytes aligned. 
  Width                 - Width of the window. Unit is pixels. Range is 3 to 1280.
  Height                - Height of the window. Unit is pixels. Range is 1 to 1024.
  ScaledWidth           - Original width when scaling-up. Unit is pixels. 
                          When scaling-up is disabled, set to 0.
                          When scaling-up is enabled, range is 4 to (Width-1).
  ScaledHeight          - Original height when scaling-up. Unit is pixels. 
                          When scaling-up is disabled, set to 0.
                          When scaling-up is enabled, range is 4 to (Height-1).
  Alpha                 - The constant alpha value of the window. Range is 0 to 255.
  UsePremultipliedAlpha - Pre-multiplied Alpha Mode. 0: Disable, 1: Enable.
  ColorKey.Enabled      - Chromakey Mode. 0: Disable, 1: Enable.
  Flags                 - Additional flags to set certain properties of a window.
  Next                  - Internal pointer to the next window in the chain. 
                          This is used in WM internally. 
                          User should initialize as R_NULL when <R_WM_WindowCreate> is called.

  [used when Mode == R_WM_WINMODE_FB]

  Surface.Fb.Buffer     - Pointer to an array of BufNum <r_wm_WinBuffer_t> elements. 
  Surface.Fb.BufNum     - This is the number of frame buffers of the window. 
  Surface.Fb.BufMode    - Frame buffer allocation mode. See <r_wm_WinBufAllocMode_t>

  [used when Mode == R_WM_WINMODE_SPRITES]

  Surface.SpritesRoot   - Root of Sprite data assigned to the window. 
                          This is used in WM internally. 
                          User should initialize as R_NULL when <R_WM_WindowCreate> is called. 

  [used when ColorFmt == R_WM_COLORFMT_CLUT*]

  ClutNumEntries        - The number of entries of the CLUT assigned to this window.
  Clut                  - Pointer to array of CLUT. It should be 4 Byte aligned.

  [Color ColorKey.Enabled = 1]
  
  ColorKey.In.RgbKey.Red   - Red   component of the target color to replace. 
  ColorKey.In.RgbKey.Green - Green component of the target color to replace. 
  ColorKey.In.RgbKey.Blue  - Blue  component of the target color to replace. 
  ColorKey.In.ClutKey      - This is not supported.
  ColorKey.Out.Red         - Red   component of the replacing color. 
  ColorKey.Out.Green       - Green component of the replacing color. 
  ColorKey.Out.Blue        - Blue  component of the replacing color. 
  ColorKey.Out.Alpha       - Alpha component of the replacing color. 
*/

struct r_wm_Window_s
{
    r_wm_WinStatus_t                Status;
    r_wm_WinMode_t                  Mode;
    r_wm_WinColorFmt_t              ColorFmt;
    int32_t                         PosX;
    int32_t                         PosY;
    uint32_t                        PosZ;
    uint32_t                        Pitch; 
    uint32_t                        Width;
    uint32_t                        Height;
    uint32_t                        ScaledWidth;
    uint32_t                        ScaledHeight;
    
    union
    {
        struct
        {
            r_wm_WinBuffer_t        *Buffer;
            uint8_t                 BufNum;
            r_wm_WinBufAllocMode_t  BufMode;
        } Fb;
        r_wm_Sprite_t               *SpritesRoot;
    } Surface;

    uint8_t                         Alpha;
    uint8_t                         UsePremultipliedAlpha;
    uint32_t                        ClutNumEntries;
    const r_wm_ClutEntry_t*         Clut;
    
    struct
    {
        uint8_t                     Enabled;
        union
        {
            struct
            {
                uint8_t             Red;
                uint8_t             Green;
                uint8_t             Blue;
            } RgbKey;
            uint8_t                 ClutKey;
        } In;
        struct
        {
            uint8_t                 Red;
            uint8_t                 Green;
            uint8_t                 Blue;
            uint8_t                 Alpha;
        } Out;
    } ColorKey;
    
    r_wm_WinFlags_t                 Flags;
    struct r_wm_Window_s            *Next;    /*internal*/
} /*r_wm_Window_t*/;

/***********************************************************
  typedef: r_wm_MsgScreenEnable_t

  Description:
  Message queue data for <R_WM_ScreenEnable> / <R_WM_ScreenDisable>
  
  Struct members:
  Enabled  - 0: Disable 1:Enable
*/

typedef struct
{
    uint32_t Enabled;
} r_wm_MsgScreenEnable_t;

/***********************************************************
  typedef: r_wm_MsgScreenBgColorSet_t

  Description:
  Message queue data for <R_WM_ScreenBgColorSet>
  
  Struct members:
  Red     - Argument "Red"   of R_WM_ScreenBgColorSet
  Green   - Argument "Green" of R_WM_ScreenBgColorSet
  Blue    - Argument "Blue"  of R_WM_ScreenBgColorSet
*/

typedef struct
{
    uint8_t     Red;
    uint8_t     Green;
    uint8_t     Blue;
} r_wm_MsgScreenBgColorSet_t;

/***********************************************************
  typedef: r_wm_MsgScreenColorCurveSet_t

  Description:
  Message queue data for <R_WM_ScreenColorCurveSet>
  
  Struct members:
  NumEntries    - Argument "NumEntries" of R_WM_ScreenColorCurveSet
  ColorCurve    - Argument "ColorCurve" of R_WM_ScreenColorCurveSet
*/

typedef struct
{
    uint32_t               NumEntries;
    const r_wm_ClutEntry_t *ColorCurve;
} r_wm_MsgScreenColorCurveSet_t;

/***********************************************************
  typedef: r_wm_MsgScreenGammaSet_t

  Description:
  Message queue data for <R_WM_ScreenGammaSet>
  
  Struct members:
  GammaRed    - Argument "GammaRed"   of R_WM_ScreenGammaSet
  GammaGreen  - Argument "GammaGreen" of R_WM_ScreenGammaSet
  GammaBlue   - Argument "GammaBlu"e  of R_WM_ScreenGammaSet
*/

typedef struct
{
    uint8_t     GammaRed;
    uint8_t     GammaGreen;
    uint8_t     GammaBlue;
} r_wm_MsgScreenGammaSet_t;

/***********************************************************
  typedef: r_wm_MsgScreenVocaCreate_t

  Description:
  Message queue data for <R_WM_ScreenVocaCreate>
  
  Struct members:
  Voca    - Argument "Voca" of R_WM_ScreenVocaCreate
*/

typedef struct
{
    r_wm_Voca_t   *Voca;
} r_wm_MsgScreenVocaCreate_t;

/***********************************************************
  typedef: r_wm_MsgScreenVocaDelete_t

  Description:
  Message queue data for <R_WM_ScreenVocaDelete>
  
  Struct members:
  Voca    - Argument "Voca" of R_WM_ScreenVocaDelete
*/

typedef struct
{
    r_wm_Voca_t   *Voca;
} r_wm_MsgScreenVocaDelete_t;

/***********************************************************
  typedef: r_wm_MsgScreenVocaEnable_t

  Description:
  Message queue data for <R_WM_ScreenVocaEnable> / <R_WM_ScreenVocaDisable>
  
  Struct members:
  Voca     - Argument "Voca" of R_WM_ScreenVocaEnable / R_WM_ScreenVocaDisable
  Enabled  - 0: Disable 1:Enable
*/

typedef struct
{
    r_wm_Voca_t   *Voca;
    uint32_t      Enabled;
} r_wm_MsgScreenVocaEnable_t;

/***********************************************************
  typedef: r_wm_MsgScreenVocaExpImgSet_t

  Description:
  Message queue data for <R_WM_ScreenVocaExpImgSet>
  
  Struct members:
  Voca      - Argument "Voca"      of R_WM_ScreenVocaExpImgSet
  Threshold - Argument "Threshold" of R_WM_ScreenVocaExpImgSet
  RamAddr   - Argument "RamAddr"   of R_WM_ScreenVocaExpImgSet
  Size      - Argument "ExpSize"   of R_WM_ScreenVocaExpImgSet
  ExpImg    - Argument "ExpImg"    of R_WM_ScreenVocaExpImgSet
*/

typedef struct
{
    r_wm_Voca_t         *Voca;
    uint32_t            Threshold;
    uint16_t            RamAddr; 
    uint16_t            ExpSize;
    const uint16_t      *ExpImg;
} r_wm_MsgScreenVocaExpImgSet_t;

/***********************************************************
  typedef: r_wm_MsgScreenVocaClutSet_t

  Description:
  Message queue data for <R_WM_ScreenVocaClutSet>
  
  Struct members:
  Voca      - Argument "Voca"       of R_WM_ScreenVocaClutSet
  Threshold - Argument "NumEntries" of R_WM_ScreenVocaClutSet
  RamAddr   - Argument "Clut"       of R_WM_ScreenVocaClutSet
*/

typedef struct
{
    r_wm_Voca_t                *Voca;
    uint8_t                    NumEntries;
    const r_wm_VocaClutEntry_t *Clut;
} r_wm_MsgScreenVocaClutSet_t;

/***********************************************************
  typedef: r_wm_MsgScreenActMonEnable_t

  Description:
  Message queue data for <R_WM_ScreenActivityMonEnable> / <R_WM_ScreenActivityMonDisable>
  
  Struct members:
  UpperTime - Argument "UpperTime" of R_WM_ScreenActivityMonEnable
  LowerTime - Argument "LowerTime" of R_WM_ScreenActivityMonEnable
  Enabled   - 0: Disable 1:Enable
*/

typedef struct
{
    uint32_t      UpperTime;
    uint32_t      LowerTime;
    uint32_t      Enabled;
} r_wm_MsgScreenActMonEnable_t;

/***********************************************************
  typedef: r_wm_MsgWinCreateData_t

  Description:
  Message queue data for <R_WM_WindowCreate>
  
  Struct members:
  Win    - Argument "Window" of R_WM_WindowCreate
*/

typedef struct
{
    r_wm_Window_t   *Win;
} r_wm_MsgWinCreateData_t;

/***********************************************************
  typedef: r_wm_MsgWinDeleteData_t

  Description:
  Message queue data for <R_WM_WindowDelete>
  
  Struct members:
  Win    - Argument "Window" of R_WM_WindowDelete
*/

typedef struct
{
    r_wm_Window_t   *Win;
} r_wm_MsgWinDeleteData_t;

/***********************************************************
  typedef: r_wm_MsgWinEnableData_t

  Description:
  Message queue data for <R_WM_WindowEnable> / <R_WM_WindowDisable>
  
  Struct members:
  Win     - Argument "Window" of R_WM_WindowEnable / R_WM_WindowDisable
  Enabled - 0: Disable 1:Enable
*/

typedef struct
{
    r_wm_Window_t   *Win;
    uint32_t        Enabled;
} r_wm_MsgWinEnableData_t;

/***********************************************************
  typedef: r_wm_MsgWinMoveData_t

  Description:
  Message queue data for <R_WM_WindowMove>
  
  Struct members:
  Win    - Argument "Window" of R_WM_WindowMove
  PosX   - Argument "PosX"   of R_WM_WindowMove
  PosY   - Argument "PosY"   of R_WM_WindowMove
  PosZ   - Argument "PosZ"   of R_WM_WindowMove
*/

typedef struct
{
    r_wm_Window_t  *Win;
    int32_t        PosX;
    int32_t        PosY;
    int32_t        PosZ;
} r_wm_MsgWinMoveData_t;

/***********************************************************
  typedef: r_wm_MsgWinResizeData_t

  Description:
  Message queue data for <R_WM_WindowResize>
  
  Struct members:
  Win    - Argument "Window" of R_WM_WindowResize
  Pitch  - Argument "Pitch"  of R_WM_WindowResize
  Width  - Argument "Width"  of R_WM_WindowResize
  Height - Argument "Height" of R_WM_WindowResize
*/

typedef struct
{
    r_wm_Window_t   *Win;
    uint32_t        Pitch;
    uint32_t        Width;
    uint32_t        Height;
} r_wm_MsgWinResizeData_t;

/***********************************************************
  typedef: r_wm_MsgWinScaledSizeSetData_t

  Description:
  Message queue data for <R_WM_WindowScaledSizeSet>
  
  Struct members:
  Win    - Argument "Window"       of R_WM_WindowScaledSizeSet
  Width  - Argument "ScaledWidth"  of R_WM_WindowScaledSizeSet
  Height - Argument "ScaledHeight" of R_WM_WindowScaledSizeSet
*/

typedef struct
{
    r_wm_Window_t   *Win;
    uint32_t        ScaledWidth;
    uint32_t        ScaledHeight;
} r_wm_MsgWinScaledSizeSetData_t;

/***********************************************************
  typedef: r_wm_MsgWinColorFmtSet_t

  Description:
  Message queue data for <R_WM_WindowColorFmtSet>
  
  Struct members:
  Win       - Argument "Window"   of R_WM_WindowColorFmtSet
  ColorFmt  - Argument "ColorFmt" of R_WM_WindowColorFmtSet
*/

typedef struct
{
    r_wm_Window_t       *Win;
    r_wm_WinColorFmt_t  ColorFmt;
} r_wm_MsgWinColorFmtSet_t;

/***********************************************************
  typedef: r_wm_MsgWinAlphaData_t

  Description:
  Message queue data for <R_WM_WindowAlphaSet>
  
  Struct members:
  Win    - Argument "Window" of R_WM_WindowAlphaSet
  Win    - Argument "Alpha"  of R_WM_WindowAlphaSet
*/

typedef struct
{
    r_wm_Window_t   *Win;
    uint8_t         Alpha;
} r_wm_MsgWinAlphaData_t;

/***********************************************************
  typedef: r_wm_MsgWinPremultAlphaEnableData_t

  Description:
  Message queue data for <R_WM_WindowPremultipliedAlphaEnable> / 
  <R_WM_WindowPremultipliedAlphaDisable>
  
  Struct members:
  Win     - Argument "Window" of R_WM_WindowPremultipliedAlphaEnable / 
            R_WM_WindowPremultipliedAlphaEnable
  Enabled - 0: Disable 1:Enable
*/

typedef struct
{
    r_wm_Window_t   *Win;
    uint8_t         Enabled;
} r_wm_MsgWinPremultAlphaEnableData_t;

/***********************************************************
  typedef: r_wm_MsgWinExternalBufSetData_t

  Description:
  Message queue data for <R_WM_WindowExternalBufSet>
  
  Struct members:
  Win         - Argument "Window"      of R_WM_WindowExternalBufSet
  Buf         - Argument "Buf"         of R_WM_WindowExternalBufSet
  BufNum      - Argument "BufNum"      of R_WM_WindowExternalBufSet
  ColorFormat - Argument "ColorFormat" of R_WM_WindowExternalBufSet
*/

typedef struct
{
    r_wm_Window_t             *Win;
    r_wm_WinBuffer_t          *Buf;
    uint32_t                  BufNum;
    r_wm_WinColorFmt_t        ColorFormat;
} r_wm_MsgWinExternalBufSetData_t;

/***********************************************************
  typedef: r_wm_MsgWinClutSetData_t

  Description:
  Message queue data for <R_WM_WindowClutSet>
  
  Struct members:
  Win        - Argument "Window"     of R_WM_WindowClutSet
  NumEntries - Argument "NumEntries" of R_WM_WindowClutSet
  Clut       - Argument "Clut"       of R_WM_WindowClutSet
*/

typedef struct
{
    r_wm_Window_t           *Win;
    uint32_t                NumEntries;
    const r_wm_ClutEntry_t  *Clut;
} r_wm_MsgWinClutSetData_t;

/***********************************************************
  typedef: r_wm_MsgWinColorKeyEnableData_t

  Description:
  Message queue data for <R_WM_WindowColorKeyEnable> / <R_WM_WindowColorKeyDisable>
  
  Struct members:
  Win     - Argument "Window" of R_WM_WindowColorKeyEnable / R_WM_WindowColorKeyDisable
  Enabled - 0: Disable 1:Enable
*/

typedef struct
{
    r_wm_Window_t   *Win;
    uint32_t        Enabled;
} r_wm_MsgWinColorKeyEnableData_t;

/***********************************************************
  typedef: r_wm_MsgWinFlagsUpdateData_t

  Description:
  Message queue data for <R_WM_WindowVerticalMirrorEnable> / <R_WM_WindowVerticalMirrorDisable>
  
  Struct members:
  Win        - Argument "Window" of R_WM_WindowVerticalMirrorEnable  / 
               R_WM_WindowVerticalMirrorDisable
  SetFlags   - Set flag. See <r_wm_WinFlags_t>.
  ClearFlags - Clear flag. See <r_wm_WinFlags_t>.
*/

typedef struct
{
    r_wm_Window_t    *Win;
    r_wm_WinFlags_t  SetFlags;
    r_wm_WinFlags_t  ClearFlags;
} r_wm_MsgWinFlagsUpdateData_t;

/***********************************************************
  typedef: r_wm_MsgWinSwapData_t

  Description:
  Message queue data for <R_WM_WindowSwap>
  
  Struct members:
  Win    - Argument "Window" of R_WM_WindowSwap
*/

typedef struct
{
    r_wm_Window_t   *Win;
} r_wm_MsgWinSwapData_t;

/***********************************************************
  typedef: r_wm_MsgWinDeleteAllSprites_t

  Description:
  Message queue data for <R_WM_WindowDeleteAllSprites>
  
  Struct members:
  Win    - Argument "Window" of R_WM_WindowDeleteAllSprites
*/

typedef struct
{
    r_wm_Window_t   *Win;
} r_wm_MsgWinDeleteAllSprites_t;

/***********************************************************
  typedef: r_wm_MsgEofData_t

  Description:
  Message queue data for <R_WM_FrameEndMark>
  
  Struct members:
  Id    - Argument "Id" of R_WM_FrameEndMark
*/

typedef struct
{
    uint32_t    Id;
} r_wm_MsgEofData_t;

/***********************************************************
  typedef: r_wm_MsgSpriteCreateData_t

  Description:
  Message queue data for <R_WM_SpriteCreate>
  
  Struct members:
  Sprite    - Argument "Sprite" of R_WM_SpriteCreate
*/

typedef struct
{
    r_wm_Sprite_t   *Sprite;
} r_wm_MsgSpriteCreateData_t;

/***********************************************************
  typedef: r_wm_MsgSpriteDeleteData_t

  Description:
  Message queue data for <R_WM_SpriteDelete>
  
  Struct members:
  Sprite    - Argument "Sprite" of R_WM_SpriteDelete
*/

typedef struct
{
    r_wm_Sprite_t   *Sprite;
} r_wm_MsgSpriteDeleteData_t;

/***********************************************************
  typedef: r_wm_MsgSpriteEnableData_t

  Description:
  Message queue data for <R_WM_SpriteEnable> / <R_WM_SpriteDisable>
  
  Struct members:
  Sprite    - Argument "Sprite" of R_WM_SpriteEnable / R_WM_SpriteDisable
  Enabled   - 0: Disable 1:Enable
*/

typedef struct
{
    r_wm_Sprite_t   *Sprite;
    uint32_t        Enabled;
} r_wm_MsgSpriteEnableData_t;

/***********************************************************
  typedef: r_wm_MsgSpriteMoveData_t

  Description:
  Message queue data for <R_WM_SpriteMove>
  
  Struct members:
  Sprite    - Argument "Sprite" of R_WM_SpriteMove
  PosX      - Argument "PosX"   of R_WM_SpriteMove
  PosY      - Argument "PosY"   of R_WM_SpriteMove
  PosZ      - Argument "PosZ"   of R_WM_SpriteMove
*/

typedef struct
{
    r_wm_Sprite_t   *Sprite;
    uint32_t        PosX;
    uint32_t        PosY;
    uint32_t        PosZ;
} r_wm_MsgSpriteMoveData_t;

/***********************************************************
  typedef: r_wm_MsgSpriteBufSetData_t

  Description:
  Message queue data for <R_WM_SpriteBufSet>
  
  Struct members:
  Sprite    - Argument "Sprite" of R_WM_SpriteBufSet
  Buf       - Argument "Buffer" of R_WM_SpriteBufSet
*/

typedef struct
{
    r_wm_Sprite_t   *Sprite;
    const void      *Buf;
} r_wm_MsgSpriteBufSetData_t;

/***********************************************************
  typedef: r_wm_MsgCaptCreateData_t

  Description:
  Message queue data for <R_WM_CaptureCreate>
  
  Struct members:
  Capt    - Argument "Capture" of R_WM_CaptureCreate
*/

typedef struct
{
    r_wm_Capture_t  *Capt;
} r_wm_MsgCaptCreateData_t;

/***********************************************************
  typedef: r_wm_MsgCaptDeleteData_t

  Description:
  Message queue data for <R_WM_CaptureDelete>
  
  Struct members:
  Capt    - Argument "Capture" of R_WM_CaptureDelete
*/

typedef struct
{
    r_wm_Capture_t  *Capt;
} r_wm_MsgCaptDeleteData_t;

/***********************************************************
  typedef: r_wm_MsgCaptEnableData_t

  Description:
  Message queue data for <R_WM_CaptureEnable> / <R_WM_CaptureDisable>
  
  Struct members:
  Capt    - Argument "Capture" of R_WM_CaptureEnable / R_WM_CaptureDisable
  Enabled - 0: Disable 1:Enable
*/

typedef struct
{
    r_wm_Capture_t  *Capt;
    uint32_t        Enabled;
} r_wm_MsgCaptEnableData_t;

/***********************************************************
  typedef: r_wm_MsgCaptMoveData_t

  Description:
  Message queue data for <R_WM_CaptureMove>
  
  Struct members:
  Capt    - Argument "Capture" of R_WM_CaptureMove. 
  StartX  - Argument "StartX"  of R_WM_CaptureMove.
  StartY1 - Argument "StartY1" of R_WM_CaptureMove.
*/

typedef struct
{
    r_wm_Capture_t  *Capt;
    uint32_t        StartX;
    uint32_t        StartY1;
} r_wm_MsgCaptMoveData_t;

/***********************************************************
  typedef: r_wm_MsgCaptResizeData_t

  Description:
  Message queue data for <R_WM_CaptureResize>
  
  Struct members:
  Capt    - Argument "Capture" of R_WM_CaptureResize. 
  Width   - Argument "Width"   of R_WM_CaptureResize. 
  Height  - Argument "Height"  of R_WM_CaptureResize. 
*/

typedef struct
{
    r_wm_Capture_t  *Capt;
    uint32_t        Width;
    uint32_t        Height;
} r_wm_MsgCaptResizeData_t;

/***********************************************************
  typedef: r_wm_MsgCaptScaledSizeData_t

  Description:
  Message queue data for <R_WM_CaptureScaledSizeSet>
  
  Struct members:
  Capt         - Argument "Capture"      of R_WM_CaptureScaledSizeSet. 
  ScaledWidth  - Argument "ScaledWidth"  of R_WM_CaptureScaledSizeSet. 
  ScaledHeight - Argument "ScaledHeight" of R_WM_CaptureScaledSizeSet. 
*/

typedef struct
{
    r_wm_Capture_t  *Capt;
    uint32_t        ScaledWidth;
    uint32_t        ScaledHeight;
} r_wm_MsgCaptScaledSizeData_t;

/***********************************************************
  typedef: r_wm_MsgCaptExtVsyncData_t

  Description:
  Message queue data for <R_WM_CaptureExtVsyncSet>
  
  Struct members:
  Capt         - Argument "Capture"     of R_WM_CaptureExtVsyncSet. 
  VsyncMaskUs  - Argument "VsyncMaskUs" of R_WM_CaptureExtVsyncSet. 
  VsyncLackUs  - Argument "VsyncLackUs" of R_WM_CaptureExtVsyncSet. 
  HsyncCycle   - Argument "HsyncCycle"  of R_WM_CaptureExtVsyncSet. 
*/

typedef struct
{
    r_wm_Capture_t  *Capt;
    uint32_t        VsyncMaskUs;
    uint32_t        VsyncLackUs;
    uint16_t        HsyncCycle;
} r_wm_MsgCaptExtVsyncData_t;

/***********************************************************
  typedef: r_wm_MsgDiscomCreateData_t

  Description:
  Message queue data for <R_WM_DiscomCreate>
  
  Struct members:
  Discom  - Argument "Discom" of R_WM_DiscomCreate
*/

typedef struct
{
    r_wm_Discom_t   *Discom;
} r_wm_MsgDiscomCreateData_t;

/***********************************************************
  typedef: r_wm_MsgDiscomDeleteData_t

  Description:
  Message queue data for <R_WM_DiscomDelete>
  
  Struct members:
  Discom  - Argument "Discom" of R_WM_DiscomDelete
*/

typedef struct
{
    r_wm_Discom_t   *Discom;
} r_wm_MsgDiscomDeleteData_t;

/***********************************************************
  typedef: r_wm_MsgDiscomEnableData_t

  Description:
  Message queue data for <R_WM_DiscomEnable> / <R_WM_DiscomDisable>
  
  Struct members:
  Discom  - Argument "Discom" of R_WM_DiscomEnable / R_WM_DiscomDisable
  Enabled - 0: Disable 1:Enable
*/

typedef struct
{
    r_wm_Discom_t   *Discom;
    uint32_t        Enabled;
} r_wm_MsgDiscomEnableData_t;

/***********************************************************
  typedef: r_wm_MsgDiscomCrcSetData_t

  Description:
  Message queue data for <R_WM_DiscomCrcSet>
  
  Struct members:
  Discom  - Argument "Discom" of R_WM_DiscomCrcSet 
  ExpCrc  - Argument "ExpCrc" of R_WM_DiscomCrcSet 
*/

typedef struct
{
    r_wm_Discom_t   *Discom;
    uint32_t        ExpCrc;
} r_wm_MsgDiscomCrcSetData_t;

/***********************************************************
  typedef: r_wm_MsgGenericData

  Description:
  Message queue data for each asyncronous functions

  Struct members:
  ScrEnable             - Message of R_WM_ScreenEnable / Disable
  ScrBgColorSet         - Message of R_WM_ScreenBgColorSet
  ScrColorCurveSet      - Message of R_WM_ScreenColorCurveSet
  ScrGammaSet           - Message of R_WM_ScreenGammaSet
  ScrVocaCreate         - Message of R_WM_ScreenVocaCreate
  ScrVocaDelete         - Message of R_WM_ScreenVocaDelete
  ScrVocaEnable         - Message of R_WM_ScreenVocaEnable / Disable
  ScrVocaExpImgSet      - Message of R_WM_ScreenVocaExpImgSet
  ScrVocaClutSet        - Message of R_WM_ScreenVocaClutSet
  ScrActMonEnable       - Message of R_WM_ScreenActivityMonEnable / Disable
  WinCreate             - Message of R_WM_WindowCreate
  WinDelete             - Message of R_WM_WindowDelete
  WinEnable             - Message of R_WM_WindowEnable / Disable
  WinMove               - Message of R_WM_WindowMove
  WinResize             - Message of R_WM_WindowResize
  WinScaledSizeSet      - Message of R_WM_WindowScaledSizeSet
  WinAlphaSet           - Message of R_WM_WindowAlphaSet
  WinPremultAlphaEnable - Message of R_WM_WindowPremultipliedAlphaEnable / Disable
  WinClutSet            - Message of R_WM_WindowClutSet
  WinColorFmtSet        - Message of R_WM_WindowColorFmtSet
  WinFlags              - Message of R_WM_WindowVerticalMirrorEnable / Disable
  WinSwap               - Message of R_WM_WindowSwap
  WinExternalBufSet     - Message of R_WM_WindowExternalBufSet
  WinColorKeyEnable     - Message of R_WM_WindowColorKeyEnable / Disable
  WinDeleteAllSprites   - Message of R_WM_WindowDeleteAllSprites
  SpriteCreate          - Message of R_WM_SpriteCreate
  SpriteDelete          - Message of R_WM_SpriteDelete
  SpriteEnable          - Message of R_WM_SpriteEnable / Disable
  SpriteMove            - Message of R_WM_SpriteMove
  SpriteBufSet          - Message of R_WM_SpriteBufSet
  CaptCreate            - Message of R_WM_CaptureCreate
  CaptDelete            - Message of R_WM_CaptureDelete
  CaptEnable            - Message of R_WM_CaptureEnable / Disable
  CaptMove              - Message of R_WM_CaptureMove
  CaptResize            - Message of R_WM_CaptureResize
  CaptScaledSizeSet     - Message of R_WM_CaptureScaledSizeSet
  CaptExtVsyncSet       - Message of R_WM_CaptureExtVsyncSet
  DiscomCreate          - Message of R_WM_DiscomCreate
  DiscomDelete          - Message of R_WM_DiscomDelete
  DiscomEnable          - Message of R_WM_DiscomEnable / Disable
  DiscomCrcSet          - Message of R_WM_DiscomCrcSet
  Eof                   - Message of R_WM_FrameEndMark
*/

typedef union
{
    r_wm_MsgScreenEnable_t              ScrEnable;
    r_wm_MsgScreenBgColorSet_t          ScrBgColorSet;
    r_wm_MsgScreenColorCurveSet_t       ScrColorCurveSet;
    r_wm_MsgScreenGammaSet_t            ScrGammaSet;
    r_wm_MsgScreenVocaCreate_t          ScrVocaCreate;
    r_wm_MsgScreenVocaDelete_t          ScrVocaDelete;
    r_wm_MsgScreenVocaEnable_t          ScrVocaEnable;
    r_wm_MsgScreenVocaExpImgSet_t       ScrVocaExpImgSet;
    r_wm_MsgScreenVocaClutSet_t         ScrVocaClutSet;
    r_wm_MsgScreenActMonEnable_t        ScrActMonEnable;
    r_wm_MsgWinCreateData_t             WinCreate;
    r_wm_MsgWinDeleteData_t             WinDelete;
    r_wm_MsgWinEnableData_t             WinEnable;
    r_wm_MsgWinMoveData_t               WinMove;
    r_wm_MsgWinResizeData_t             WinResize;
    r_wm_MsgWinScaledSizeSetData_t      WinScaledSizeSet;
    r_wm_MsgWinAlphaData_t              WinAlphaSet;
    r_wm_MsgWinPremultAlphaEnableData_t WinPremultAlphaEnable;
    r_wm_MsgWinClutSetData_t            WinClutSet;
    r_wm_MsgWinColorFmtSet_t            WinColorFmtSet;
    r_wm_MsgWinFlagsUpdateData_t        WinFlags;
    r_wm_MsgWinSwapData_t               WinSwap;
    r_wm_MsgWinExternalBufSetData_t     WinExternalBufSet;
    r_wm_MsgWinColorKeyEnableData_t     WinColorKeyEnable;
    r_wm_MsgWinDeleteAllSprites_t       WinDeleteAllSprites;
    r_wm_MsgSpriteCreateData_t          SpriteCreate;
    r_wm_MsgSpriteDeleteData_t          SpriteDelete;
    r_wm_MsgSpriteEnableData_t          SpriteEnable;
    r_wm_MsgSpriteMoveData_t            SpriteMove;
    r_wm_MsgSpriteBufSetData_t          SpriteBufSet;
    r_wm_MsgCaptCreateData_t            CaptCreate;
    r_wm_MsgCaptDeleteData_t            CaptDelete;
    r_wm_MsgCaptEnableData_t            CaptEnable;
    r_wm_MsgCaptMoveData_t              CaptMove;
    r_wm_MsgCaptResizeData_t            CaptResize;
    r_wm_MsgCaptScaledSizeData_t        CaptScaledSizeSet;
    r_wm_MsgCaptExtVsyncData_t          CaptExtVsyncSet;
    r_wm_MsgDiscomCreateData_t          DiscomCreate;
    r_wm_MsgDiscomDeleteData_t          DiscomDelete;
    r_wm_MsgDiscomEnableData_t          DiscomEnable;
    r_wm_MsgDiscomCrcSetData_t          DiscomCrcSet;
    r_wm_MsgEofData_t                   Eof;
} r_wm_MsgGenericData;  

/***********************************************************
  typedef: r_wm_Msg_s

  Description:
  Message queue information. This structure is used in WM driver internally.
  User does not need to be aware of the content of the message.
  User should prepare the buffer for message queue in <R_WM_DevInit>.
  The structure size (one message queue size) is 20 Bytes.

  Struct members:
  Id            - See <r_wm_MsgId_t>.
  Data          - A union to hold event specific data. See <r_wm_MsgGenericData>.
*/

struct r_wm_Msg_s
{
    r_wm_MsgId_t            Id;
    r_wm_MsgGenericData     Data;
};

/***********************************************************
  typedef: r_wm_EventId_t

  Description:
  The external event information. 
  
  Members:
  R_WM_EVENT_VBLANK             - V-sync at layer 0 interrupt notification.
  R_WM_EVENT_SCANLINE           - Scanline interrupt at layer 3 notification.
  R_WM_EVENT_VI_VBLANK          - Video Input V-sync interrupt notification.
  R_WM_EVENT_VI_OVERFLOW        - Video Input overflow interrupt notification.
  R_WM_EVENT_LAYER0_UNDERFLOW   - Video output layer 0 underflow.
  R_WM_EVENT_LAYER1_UNDERFLOW   - Video output layer 1 underflow.
  R_WM_EVENT_LAYER2_UNDERFLOW   - Video output layer 2 underflow.
  R_WM_EVENT_LAYER3_UNDERFLOW   - Video output layer 3 underflow.
  R_WM_EVENT_LAYER1_VBLANK      - V-sync at layer 1 interrupt notification.
  R_WM_EVENT_OIR_VBLANK         - V-sync at OIR layer interrupt notification. 
                                  Only WM Unit 0 is supported.
  R_WM_EVENT_OIR_SCANLINE       - Scanline interrupt at OIR layer notification.
                                  Only WM Unit 0 is supported.
  R_WM_EVENT_DISCOM_MISMACTH    - Mismatch occurs in one DISCOM device.
  R_WM_EVENT_VOCA_MISMACTH      - Mismatch occurs in Voca monitor area.
  R_WM_EVENT_ACT_MON_ERROR      - Error detected by activity monitor.
*/

typedef enum
{
    R_WM_EVENT_VBLANK                   = 0x0,
    R_WM_EVENT_SCANLINE                 = 0x1,
    R_WM_EVENT_VI_VBLANK                = 0x2,
    R_WM_EVENT_VI_OVERFLOW              = 0x3,
    R_WM_EVENT_LAYER0_UNDERFLOW         = 0x4,
    R_WM_EVENT_LAYER1_UNDERFLOW         = 0x5,
    R_WM_EVENT_LAYER2_UNDERFLOW         = 0x6,
    R_WM_EVENT_LAYER3_UNDERFLOW         = 0x7,
    R_WM_EVENT_LAYER1_VBLANK            = 0x8,
    R_WM_EVENT_OIR_VBLANK               = 0x9,
    R_WM_EVENT_OIR_SCANLINE             = 0xA,
    R_WM_EVENT_DISCOM_MISMACTH          = 0xB,
    R_WM_EVENT_VOCA_MISMACTH            = 0xC,
    R_WM_EVENT_ACT_MON_ERROR            = 0xD,
    R_WM_EVENT_LAST
} r_wm_EventId_t;

/***********************************************************
  typedef: r_wm_Event_t

  Description:
  The external event information. 
  
  Member:
  Id    - Event ID. See <r_wm_EventId_t>.
  Data  - The value depending on event ID.
          In case of R_WM_EVENT_DISCOM_MISMACTH, 
          Data means Discom unit number where mismatch occurs.
          In case of R_WM_EVENT_VOCA_MISMACTH,
          Data means Voca monitor area index number where mismatch occurs.
          In case of other event, Data has no meanings.
*/

typedef struct 
{
    r_wm_EventId_t  Id;
    uint32_t        Data;
} r_wm_Event_t;

/***********************************************************
  typedef: r_wm_CapMode_t

  Description:
  Input video signal format and optional flags.

  Members:
  One of these six flags must be selected or the function call will fail :

  R_WM_CAPMODE_YUV_ITU656 - Select ITU-R BT.656 mode
  R_WM_CAPMODE_YUV_8BIT   - Select ITU-R BT.601 mode with external sync
  R_WM_CAPMODE_YUV_16BIT  - Select YCbCr422 mode with external sync
  R_WM_CAPMODE_RGB_16BPP  - Select RGB565 mode with external sync
  R_WM_CAPMODE_RGB_18BPP  - Select RGB666 mode with external sync
  R_WM_CAPMODE_RGB_24BPP  - Select RGB888 mode with external sync

  The following flags are optional and can be omitted :

  R_WM_CAPMODE_DITHER           - If this flag is off, dithering is disabled.
                                  If this flag is on, dithering is enabled. 
                                  Dithering is effective when Window color format is 
                                  R_WM_COLORFMT_RGB565.
  R_WM_CAPMODE_YUV_Y_UV_INVERT  - If this flag is off, capturing order is Y->Cb->Y->Cr. 
                                  If this flag is on , capturing order is Cb->Y->Cr->Y. 
                                  This flag is effective in case of ITU-R BT.656 or 
                                  ITU-R BT.601 input.
                                  See INP_H_POS description in H/W User's manual.
  R_WM_CAPMODE_VSYNC_INVERT     - If this flag is off, V-sync is positive polarity. 
                                  If this flag is on , V-sync is negative polarity.
  R_WM_CAPMODE_HSYNC_INVERT     - If this flag is off, H-sync is positive polarity. 
                                  If this flag is on , H-sync is negative polarity.
  R_WM_CAPMODE_DATA_CLK_INVERT  - If this flag is off, data capturing timing is falling edge. 
                                  If this flag is on , data capturing timing is rising edge. 
  R_WM_CAPMODE_VSYNC_CLK_INVERT - If this flag is off, 
                                  V-sync signal capturing timing is falling edge. 
                                  If this flag is on , 
                                  V-sync signal capturing timing is rising edge. 
  R_WM_CAPMODE_HSYNC_CLK_INVERT - If this flag is off, 
                                  H-sync signal capturing timing is falling edge. 
                                  If this flag is on , 
                                  H-sync signal capturing timing is rising edge. 
  R_WM_CAPMODE_H_MIRRORING      - If this flag is off, horizontal mirroring is disabled.
                                  If this flag is on , horizontal mirroring is enabled.
  R_WM_CAPMODE_V_MIRRORING      - If this flag is off, vertical mirroring is disabled.
                                  If this flag is on , vertical mirroring is enabled.
  R_WM_CAPMODE_FIXED_VSYNC      - If this flag is off, synchronous V-sync of connecting layer is 
                                  external input V-sync.
                                  If this flag is on, synchronous V-sync of connecting layer is 
                                  fixed as internally generated free-running V-sync signal.
  R_WM_CAPMODE_BIG_ENDIAN       - If this flag is off, incoming data words are little endian.
                                  If this flag is on , incoming data words are big endian.
  R_WM_CAPMODE_DE_MODE          - If this flag is off, DE mode is disabled. Hsync signal is 
                                  used to capture.
                                  If this flag is on, DE mode is enabled. DE signal is used 
                                  instead of Hsync signal. DE signal should be input to 
                                  VDCE0_VI_HSYNC/VDCE1_VI_HSYNC port. 
                                  R_WM_CAPMODE_HSYNC_INVERT or R_WM_CAPMODE_HSYNC_CLK_INVERT 
                                  flag is valid for DE signal.
                                  DE mode can be selected in case of YCbCr422/RGB565/RGB666/
                                  RGB888 input mode.
  R_WM_CAPMODE_PAL              - If this flag is off, input signal is assumed as 
                                  NTSC (525 Lines/59.94 Hz) system.
                                  If this flag is on, input signal is assumed as 
                                  PAL (625 Lines/50.00 Hz) system.
                                  This flag is valid in case of BT.656 or BT.601 input mode.
  R_WM_CAPMODE_EAV              - If this flag is off, SAV code is converted to Hsync signal.
                                  If this flag is on, EAV code is converted to Hsync signal.
                                  This flag is valid in case of BT.656 input mode.
  R_WM_CAPMODE_SYNC_ONLY        - If this flag is off, input data is written to capture buffer.
                                  If this flag is on, input data is not written to capture 
                                  buffer. Only clock, Vsync Hsync are input. This mode is used 
                                  when video data is not captured but the video is output in 
                                  synchronization with the external Vsync input. 
  R_WM_CAPMODE_BOB_TOP          - If this flag is off, bob de-interlacing mode is disabled.
                                  If this flag is on , bob de-interlacing mode by top field is 
                                  enabled. Only top field is written to capture buffer.
                                  If this flag is on , once every two frames is written to capture 
                                  buffer in case of progressive video input.
  R_WM_CAPMODE_BOB_BOTTOM       - If this flag is off, bob de-interlacing mode is disabled.
                                  If this flag is on , bob de-interlacing mode by top field is 
                                  enabled. Only bottom field is written to capture buffer.
                                  This flag is effective in case of interlace video input. 
  R_WM_CAPMODE_WEAVE            - If this flag is off, weave de-interlacing mode is disabled.
                                  If this flag is on,  weave de-interlacing mode is enabled.
                                  Output by interweaving top field and bottom field alternately.
                                  This flag is effective in case of interlace video input. 
                                  Only single frame buffer is supported, no multi-buffering.
*/

typedef enum
{
    R_WM_CAPMODE_NONE             = 0,
    R_WM_CAPMODE_YUV_ITU656       = (int32_t)(1uL << 0),
    R_WM_CAPMODE_YUV_8BIT         = (int32_t)(1uL << 1),
    R_WM_CAPMODE_YUV_16BIT        = (int32_t)(1uL << 2),
    R_WM_CAPMODE_RGB_16BPP        = (int32_t)(1uL << 3),
    R_WM_CAPMODE_RGB_18BPP        = (int32_t)(1uL << 4),
    R_WM_CAPMODE_RGB_24BPP        = (int32_t)(1uL << 5),
    R_WM_CAPMODE_DITHER           = (int32_t)(1uL << 6),
    R_WM_CAPMODE_YUV_Y_UV_INVERT  = (int32_t)(1uL << 10),
    R_WM_CAPMODE_VSYNC_INVERT     = (int32_t)(1uL << 11),
    R_WM_CAPMODE_HSYNC_INVERT     = (int32_t)(1uL << 12),
    R_WM_CAPMODE_DATA_CLK_INVERT  = (int32_t)(1uL << 15),
    R_WM_CAPMODE_VSYNC_CLK_INVERT = (int32_t)(1uL << 16),
    R_WM_CAPMODE_HSYNC_CLK_INVERT = (int32_t)(1uL << 17),
    R_WM_CAPMODE_H_MIRRORING      = (int32_t)(1uL << 18),
    R_WM_CAPMODE_V_MIRRORING      = (int32_t)(1uL << 19),
    R_WM_CAPMODE_FIXED_VSYNC      = (int32_t)(1uL << 20),
    R_WM_CAPMODE_BIG_ENDIAN       = (int32_t)(1uL << 21),
    R_WM_CAPMODE_DE_MODE          = (int32_t)(1uL << 22),
    R_WM_CAPMODE_PAL              = (int32_t)(1uL << 23),
    R_WM_CAPMODE_EAV              = (int32_t)(1uL << 24),
    R_WM_CAPMODE_SYNC_ONLY        = (int32_t)(1uL << 25),
    
    /* Bits 0-24 mirror the VDCE data type <r_vdce_CapMode_t>. 
       The following entries are additional. */
    R_WM_CAPMODE_BOB_TOP          = (int32_t)(1uL << 28),
    R_WM_CAPMODE_BOB_BOTTOM       = (int32_t)(1uL << 29),
    R_WM_CAPMODE_WEAVE            = (int32_t)(1uL << 30)
} r_wm_CapMode_t;

/***********************************************************
  typedef: r_wm_Capture_s

  Description:
  The type describes the settings of a video capturing surface. 
  The video capturing surface is always associated with a window surface. 

  Members:
  Window       - Pointer to <r_wm_Window_t> structure which has been created already. 
  Mode         - Video input type and capturing mode.
  StartX       - X position of capturing start. Unit is pixels. 
                 When DE mode is disabled, reference point is Hsync signal.
                 StartX pixels are skipped after Hsync signal. The range is 16 to 2011.
                 If setting value is 0 to 15, this function rounds up to 16.
                 When DE mode is enabled (see R_WM_CAPMODE_DE_MODE), the reference point is 
                 DE signal. StartX pixels are skipped after DE signal. The range of 0 to 2011.
  StrideX      - The stride of the frame buffer. Unit is pixels. It should be 128 bytes aligned.
  StartY1      - Y position of capturing start. Unit is pixels. 
                 When DE mode is disabled, reference point is Vsync signal.
                 (StartY + 1) lines are skipped after Vsync signal. The range is 4 to 2035.
                 If setting value is 0 to 3, this function rounds up to 4.
                 When DE mode is enabled (see R_WM_CAPMODE_DE_MODE), the reference point is 
                 DE signal after Vsync signal. StartY pixels are skipped after 1st DE signal. 
                 The range of 0 to 2035.
  StartY2      - This value is not used.
  Width        - Width of capturing video data. Unit is pixels. Range is 4 to 2011.
                 It should be 4 pixels aligned.
                 When Width is greater than 1024 pixels, horizontal image size must be scaled-down 
                 to 1024 pixels or less. 
  Height       - Height of capturing video data. Unit is pixels. 
                 Range is 4 to 1024. It should be 4 pixels aligned.
  ScaledWidth  - Frame buffer width when scaling-down. Unit is pixels. 
                 When scaling-down is disabled, set to 0.
                 When scaling-down is enabled, range is 4 to (Width-1).
                 It should be 4 pixels aligned.
  ScaledHeight - Frame buffer height when scaling-down. Unit is pixels. 
                 When scaling-down is disabled, set to 0.
                 When scaling-down is enabled, range is 4 to (Height-1).
                 It should be 4 pixels aligned.
  Delay        - Delay from video input V-sync to Layer output V-sync 
  CapUnit      - The WM unit number to be used for capturing. 
                 If another Unit is to be used than the assigned Window, 
                 make sure that this unit is already initialized.
  Next         - Internal pointer to the next capture in the chain. 
                 This is used in WM internally. 
                 User should initialize as R_NULL when <R_WM_CaptureCreate> is called.

*/

struct r_wm_Capture_s
{
    r_wm_Window_t           *Window;
    r_wm_CapMode_t          Mode;
    uint32_t                StartX;
    uint32_t                StrideX;
    uint32_t                StartY1;
    uint32_t                StartY2;
    uint32_t                Width;
    uint32_t                Height;
    uint32_t                ScaledWidth;
    uint32_t                ScaledHeight;
    uint32_t                Delay;
    uint32_t                CapUnit;
    struct r_wm_Capture_s   *Next;
};

/***********************************************************
  typedef: r_wm_WinCapbs_t

  Description: 
  Selectable window type.
  See also <R_WM_WindowCapabilitiesSet>.
    
  Members:
  R_WM_WINCAPBS_RLE      - RLE window is selectable.
  R_WM_WINCAPBS_SPRITES  - Sprite window is selectable.
*/

typedef enum
{
    R_WM_WINCAPBS_RLE     = 0,
    R_WM_WINCAPBS_SPRITES = 1,
    R_WM_WINCAPBS_LAST    = 2
} r_wm_WinCapbs_t;

/***********************************************************
  typedef: r_wm_Voca_t

  Description:
  Each Voca monitor area has a data structure of this type. 
  
  Members:
  Threshold - Acceptable mismatch (difference) of a Monitor Area. The range is 1 to 0x3FFFF.
              See description of <r_wm_VocaClutEntry_t> for the detail.
  AreaNum   - Monitor area index number. The range is 0 to 15.
              Monitor area is shared by 2 WM unit.
  PosX      - X position of Monitor area. Unit is pixel. 
  PosY      - Y position of Monitor area. Unit is pixel.
  Width     - Width of Monitor area. Unit is pixel. The range is 1 to 128.
  Height    - Height of Monitor area. Unit is pixel. The range is 1 to 128.
  RamAddr   - Internal RAM start index of a monitor area. The range is 0 to 4095.
              The expected image should be set to RAM by <R_WM_ScreenVocaExpImgSet> 
              after <R_WM_ScreenVocaCreate>.
  ExpSize   - The number of expected image array. 
              The range is 0 to 2048.
  ExpImg    - Pointer to update expected image array. 
              User should prepare the number of arrays specified by 'Size'.
              If Size is 0, set R_NULL.
              1 address has 8pixel information.
              Data format is 8 x 2bit pixel value each 16bit. Place sequentially in top-down 
              format.
              
>             example:
>               Data[0]=0x1B00
>                 first  pixel value is 0  (0x1B00 & 0xC000) 
>                 second pixel value is 1  (0x1B00 & 0x3000) 
>                 third  pixel value is 2  (0x1B00 & 0x0C00) 
>                 forth  pixel value is 3  (0x1B00 & 0x0300) 
>                 fifth to eighth pixel value is 0 
>               
>               bit       |15,14 |13,12 | ... | 3, 2 | 1, 0 | 
>               ----------+------+------+-----+------+------+ 
>               ExpImg[0] | pix0 | pix1 | ... | pix6 | pix7 | 
>               ExpImg[1] | pix8 | pix9 | ... | pix14| pix15| 
>                          :
  Clut      - CLUT value of a Monitor Area. Each CLUT has valid level (range) for a pixel.
              See <r_wm_VocaClutEntry_t>.
  Next      - Internal pointer to the next monitor area in the chain. 
              This is used in WM internally. 
              User should initialize as R_NULL when <R_WM_ScreenVocaCreate> is called.
*/

struct r_wm_Voca_s
{
    uint32_t              Threshold;
    uint16_t              AreaNum;
    uint16_t              PosX;
    uint16_t              PosY;
    uint16_t              Width;
    uint16_t              Height;
    uint16_t              RamAddr;
    uint16_t              ExpSize;
    const uint16_t        *ExpImg;
    r_wm_VocaClutEntry_t  Clut[R_WM_VOCA_CLUT_NUM];
    struct r_wm_Voca_s    *Next;
};

/***********************************************************
  typedef: r_wm_Discom_t

  Description:
  Each Discom device has a data structure of this type. 
  
  Members:
  ExpCrc     - Expected CRC data.
  PosX       - X position of compare area. Unit is pixel. 
  PosY       - Y position of compare area. Unit is pixel.
  Width      - Width of compare area. Unit is pixel. 
  Height     - Height of compare area. Unit is pixel. 
  DiscomUnit - Discom unit number. The range is 0 - 3.
               0-1: Discom unit connected to WM unit 0.
               2-3: Discom unit connected to WM unit 1.
  Next       - Internal pointer to the next monitor area in the chain. 
               This is used in WM internally. 
               User should initialize as R_NULL when <R_WM_DiscomCreate> is called.
*/
struct r_wm_Discom_s
{
    uint32_t             ExpCrc;
    uint16_t             PosX;
    uint16_t             PosY;
    uint16_t             Width;
    uint16_t             Height;
    uint8_t              DiscomUnit;
    struct r_wm_Discom_s *Next;
};

/***********************************************************
  Section: Global API functions
*/

/***********************************************************
  Group: Device

  The section describes driver functions, 
  which are required for general use of the driver, 
  but which are related to a specific functionality of the macro itself.
*/

/***********************************************************
  Function: R_WM_DevInit

  Init WM unit.
  
  Description:
  This function initializes the driver and the hardware as far as necessary. 
  The driver makes sure, that the driver is set into a default configuration.
  WM unit status will become Initialized state after the execution of this function.
  
  Parameter:
  Unit          -   WM unit number
  MsgQueue      -   This is a pointer to Size elements of the <r_wm_Msg_t> type
                    The array is used by the window manager to store the window manager requests
  Size          -   The number specified the number of elements in the message queue
                    The maximum size is 1024.
  EventCb       -   External event notification callback function.
                    If this is R_NULL, no callback will be invoked upon event detection.
  CpuHeap       -   Pointer to your managed cpu heap. It can be set as R_NULL, when frame buffer 
                    is allocated by external mode only.
  VidHeap       -   Pointer to your managed video heap. It can be set as R_NULL, when frame buffer 
                    is allocated by external mode only.
  
  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_DevInit(const uint32_t      Unit,
                          r_wm_Msg_t *const   MsgQueue, 
                          const uint32_t      Size,
                          void                (*EventCb)(uint32_t          Unit, 
                                                         const r_wm_Event_t *Event),
                          void *const         CpuHeap,
                          void *const         VidHeap);

/***********************************************************
  Function: R_WM_DevEventRegister

  Description:
  This function enables the notification of specified event.
  Once the event is enabled, it cannot be disabled until <R_WM_DevDeinit> is called.
  Callback function to notify events are installed by <R_WM_DevInit>.
  
  Because V-sync signal is not stable while WM unit is Uninitialized state or Initialized state, 
  it is recommended to enable these events while WM unit is Display Initialized state or Display 
  Active state.
  
  Regarding R_WM_EVENT_SCANLINE event, the line position that triggers the event is set to the 
  default value by function <R_WM_ScreenTimingSet> or <R_WM_ScreenTimingSetByName>. And, this 
  function can be changed the trigger position.
  
  Regarding R_WM_EVENT_OIR_SCANLINE, this function can be set the trigger position.
  
  Regarding R_WM_EVENT_VI_VBLANK and R_WM_EVENT_VI_OVERFLOW, these events occurs when the capture 
  is enabled. Specify the unit number of the capture side to Unit.
  
  Parameter:
  Unit      -   See description of <R_WM_DevInit>
  EventId   -   Specifies the event of the event to be registered. See <r_wm_EventId_t>.
  Arg       -   Specifies the scanline number on which the event will trigger.
                This argument is valid when EventId is R_WM_EVENT_SCANLINE or 
                R_WM_EVENT_OIR_SCANLINE. The range is 0 to 2047.
  
  Returns:
  See <r_wm_Error_t>.
*/
r_wm_Error_t R_WM_DevEventRegister(const uint32_t        Unit,
                                   const r_wm_EventId_t  EventId,
                                   const uint32_t        Arg);

/***********************************************************
  Function: R_WM_DevDeinit

  DeInit WM unit.

  Description:
  This function de-initializes the driver and the hardware.
  WM unit status will become Uninitialized state after executing this function.
  If WM unit is already de-initialized status, this function does nothing and returns R_WM_ERR_OK.
  All created windows should be deleted by <R_WM_WindowDelete> before this function is called.
  All created sprites should be deleted by <R_WM_SpriteDelete> or <R_WM_WindowDeleteAllSprites>.
  
  Parameter:
  Unit -  See description of <R_WM_DevInit>
  
  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_DevDeinit(const uint32_t Unit);

/***********************************************************
  Function: R_WM_DevInfoGet

  Description:
  This function gets information of window parameters.

  Parameter:
  Unit      - See description of <R_WM_DevInit>
  LayerNum  - pointer to a variable to return number of usable layers.
  PitchMax  - pointer to a variable to return maximum pitch of a layer. 
              If 0 is returned then a pitch is not supported.
  WidthMax  - pointer to a variable to return maximum width of a layer.
  HeightMax - pointer to a variable to return maximum height of a layer.

  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_DevInfoGet(const uint32_t  Unit,
                             uint32_t *const LayerNum,
                             uint32_t *const PitchMax,
                             uint32_t *const WidthMax,
                             uint32_t *const HeightMax);

/***********************************************************
  Function: R_WM_GetVersionString

  Description:
  This function returns version string of the WM driver.
  
  Parameter:
  None.
  
  Returns:
  version string of this WM driver
*/
const int8_t *R_WM_GetVersionString(void);

/***********************************************************
  Group: Screen

  A screen is a physical video output unit. 
  There is no frame-buffer associated with a screen, 
  but a screen has a background color.
*/

/***********************************************************
  Function: R_WM_ScreenTimingSet

  Description:
  This function sets display area and timings. 
  The setting values will be active after executing <R_WM_ScreenEnable>.
  WM unit status will become Display Initialized state after execution this function.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Timing - The timing parameter contains all the information 
           to drive the display see the description of <r_ddb_Timing_t> for details.

  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_ScreenTimingSet(const uint32_t              Unit,
                                  const r_ddb_Timing_t *const Timing);

/***********************************************************
  Function: R_WM_ScreenTimingSetByName

  Description:
  This function sets display area and timings from specified name (identifier).
  This function gets the r_ddb_Timing_t parameter from data base and execute 
  <R_WM_ScreenTimingSet>.
  
  The new values will be active after executing <R_WM_ScreenEnable>.
  WM unit status will become Display Initialized state after execution this function.
  
  Parameter:
  Unit - See description of <R_WM_DevInit>
  Name - The name is an identifier, which is used to lookup 
          for the correct screen timing in the display data base (see: <r_ddb_Timing_t>).

  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_ScreenTimingSetByName(const uint32_t        Unit,
                                        const int8_t   *const Name);

/***********************************************************
  Function: R_WM_ScreenColorFormatSet

  Description:
  This function sets the color format of the video output signal.
  If user doesn't call this function, RGB888 with no optional flags is the default.

  Parameter:
    Unit   - See description of <R_WM_DevInit>
    OutFmt - The color format of the video output. <r_wm_OutColorFmt_t>
              R_WM_OUTCOLORFMT_RGB888 /
              R_WM_OUTCOLORFMT_RGB666 /
              R_WM_OUTCOLORFMT_RGB565
             Optional flags can be available with OR operation.
              R_WM_OUTCOLORFMT_FLAG_ENDIAN  / 
              R_WM_OUTCOLORFMT_FLAG_SWAP_BR / 
              R_WM_OUTCOLORFMT_FLAG_DITHER

  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_ScreenColorFormatSet(const uint32_t            Unit,
                                       const r_wm_OutColorFmt_t  OutFmt);

/***********************************************************
  Function: R_WM_ScreenBgColorSet

  Description:
  This function set the screen background color that is seen if no window (or a transparent one) 
  is on top of it.
  If user doesn't call this function, black (Red = 0, Green = 0, Blue = 0) is set as default.
  
  Parameter:
  Unit           - See description of <R_WM_DevInit>
  Red/Green/Blue - The individual color components of the background color.

  Return value:
  See <r_wm_Error_t>. 
*/
r_wm_Error_t R_WM_ScreenBgColorSet(const uint32_t Unit,
                                   const uint8_t  Red, 
                                   const uint8_t  Green, 
                                   const uint8_t  Blue);

/***********************************************************
  Function: R_WM_ScreenColorCurveSet

  Description:
  This function sets gamma correction with color curve reference points.
  Using this curve, each RGB color channel is individually corrected according to the given curve.
  See WM User's manual for the detail.
  
  The instance of ColorCurve must be hold until <R_WM_FrameWait> corresponding to this function is 
  completed. 
  This function will overwrite the settings of <R_WM_ScreenGammaSet>.
  
  Parameter:
  Unit          - See description of <R_WM_DevInit>
  NumEntries    - The number of entries in the Color Curve table. Set to 33 fixed.
  ColorCurve    - Pointer to Color curve table.
                  The head shall point to the starting address of an array of 33 elements of
                  type <r_wm_ClutEntry_t>. Element A of this structure is not used.
  
  Return value:
  See <r_wm_Error_t>. 
*/
r_wm_Error_t R_WM_ScreenColorCurveSet(const uint32_t                Unit,
                                      const uint32_t                NumEntries,
                                      const r_wm_ClutEntry_t *const ColorCurve);

/***************************************************************************
  Function: R_WM_ScreenGammaSet

  Description:
    This function sets the gamma correction with gamma value.
    Each RGB color channel is individually corrected according to the given gamma value.
    See WM User's manual for the detail.
    
    This function will overwrite the settings of <R_WM_ScreenColorCurveSet>.
    
  Parameter:
    Unit        - See description of <R_WM_DevInit>
    GammaRed    - Gamma factor for red. Range is 32-255 which corresponds to 
                  gamma = [0.25(=32/128) ... 1.99(=255/128)]. 
                  128 is the default value. It means gamma = 1.0.
    GammaGreen  - Gamma factor for green. Range is same as GammaRed.
    GammaBlue   - Gamma factor for blue. Range is same as GammaRed.

  Return value:
  See <r_wm_Error_t>. 
*/
r_wm_Error_t R_WM_ScreenGammaSet(const uint32_t  Unit,
                                 const uint8_t   GammaRed,
                                 const uint8_t   GammaGreen,
                                 const uint8_t   GammaBlue);

/***********************************************************
  Function: R_WM_ScreenEnable

  Description:
  This function enables the screen.
  WM unit status will become Display Active state after asynchronous execution is complete.

  Parameter:
  Unit - See description of <R_WM_DevInit>

  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_ScreenEnable(const uint32_t Unit);

/***********************************************************
  Function: R_WM_ScreenDisable

  Description:
  This function disables the screen.
  WM unit status will become Display Initialized state after asynchronous execution is complete.

  Parameter:
  Unit - See description of <R_WM_DevInit>

  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_ScreenDisable(const uint32_t Unit);

/***********************************************************
  Function: R_WM_ScreenVocaInit

  Description:
  This function setups VOCA H/W for specified WM unit.
  
  When WM unit 0 is specified, WM unit 0 should be in Display Active state.
  When WM unit 1 is specified, both WM unit 0 and WM unit 1 should be in Display Active state.
  That is, the message queue of <R_WM_ScreenEnable> should also be completed.
  
  This function can be executed when of all Video monitor areas and activity monitors, 
  including another WM unit, are disabled.
  
  VOCA requires a front porch of 1 pixel or more. 
  If front porch is set to 0 by <R_WM_ScreenTimingSet> or <R_WM_ScreenTimingSetByName>, 
  this function fails.

  Parameter:
  Unit - See description of <R_WM_DevInit>

  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_ScreenVocaInit(const uint32_t Unit);

/***********************************************************
  Function: R_WM_ScreenVocaDeInit

  Description:
  This function disables all VOCA monitoring (Video output monitor and Activity monitor) 
  and deletes all created VOCA monitor areas of specified WM unit.
  
  Parameter:
  Unit - See description of <R_WM_DevInit>

  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_ScreenVocaDeInit(const uint32_t Unit);

/***********************************************************
  Function: R_WM_ScreenVocaCreate
  
  Description:
  This function creates a Video output monitor area to specified WM unit.
  16 monitor areas can be created in all WM units.
  The instance of <r_wm_Voca_t> structure must be hold until <R_WM_ScreenVocaDelete> 
  is completed.
  
  Even when calling this function with Unit = 1 specified, WM Unit 0 must be initialized 
  in advance by <R_WM_DevInit>.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Voca   - <r_wm_Voca_t> structure pointer.
           The structure has to be filled by the application before calling the function.
  
  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_ScreenVocaCreate(const uint32_t       Unit, 
                                   r_wm_Voca_t   *const Voca);

/***********************************************************
  Function: R_WM_ScreenVocaDelete
  
  Description:
  This function deletes a Video output monitor area.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Voca   - <r_wm_Voca_t> structure pointer.
  
  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_ScreenVocaDelete(const uint32_t       Unit, 
                                   r_wm_Voca_t   *const Voca);

/***********************************************************
  Function: R_WM_ScreenVocaEnable
  
  Description:
  This function enables a Video output monitor.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Voca   - <r_wm_Voca_t> structure pointer.
  
  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_ScreenVocaEnable(const uint32_t       Unit, 
                                   r_wm_Voca_t   *const Voca);

/***********************************************************
  Function: R_WM_ScreenVocaDisable
  
  Description:
  This function disables a Video output monitor.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Voca   - <r_wm_Voca_t> structure pointer.
  
  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_ScreenVocaDisable(const uint32_t       Unit, 
                                    r_wm_Voca_t   *const Voca);

/***********************************************************
  Function: R_WM_ScreenVocaExpImgSet
  
  Description:
  This function sets the expected image to internal RAM.
  This function also updates the Threshold and RamAddr for specified monitor area.

  The Expected Image data is 2bit CLUT format. Each pixel is 2bit.
  Internla RAM is 8KBytes i.e. 32768 pixel
  The instance of ExpImg must be hold until <R_WM_FrameWait> or <R_WM_FrameExecuteVoca> 
  corresponding to this function is completed.
  
  This function is valid only once per Vsync in one monitor area. 
  If this function is executed more than once, the last setting becomes effective. 
  
  Parameter:
  Unit      - See description of <R_WM_DevInit>
  Voca      - <r_wm_Voca_t> structure pointer.
  Threshold - Acceptable mismatch (difference) of a Monitor Area. 
              Same as Threshold of <r_wm_Voca_t>.
  RamAddr   - Internal RAM start index to update. Same as RamAddr of <r_wm_Voca_t>.
              This function overwrites RAM from index #(RamAddr) to index #(RamAddr + ExpSize - 1).
  ExpSize   - The number of expected image array to update. Same as ExpSize of <r_wm_Voca_t>.
              The range is 1 to 2048.
              If ExpSize is 0, only Threshold and RamAddr for specified monitor area is updated.
  ExpImg    - Pointer to update expected image array. 
              Same as ExpImg of <r_wm_Voca_t>.
              
  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_ScreenVocaExpImgSet(const uint32_t        Unit, 
                                      r_wm_Voca_t    *const Voca,
                                      const uint32_t        Threshold,
                                      const uint16_t        RamAddr, 
                                      const uint16_t        ExpSize, 
                                      const uint16_t *const ExpImg);

/***********************************************************
  Function: R_WM_ScreenVocaClutSet
  
  Description:
  This function changes the CLUT data of VOCA.
  The instance of Clut must be hold until <R_WM_FrameWait> or <R_WM_FrameExecuteVoca> 
  corresponding to this function is completed.

  This function is valid only once per Vsync in one monitor area. 
  If this function is executed more than once, the last setting becomes effective. 
  
  Parameter:
  Unit       - See description of <R_WM_DevInit>
  Voca       - <r_wm_Voca_t> structure pointer.
  NumEntries - Number of Clut entries. The range is 1 to 4.
               This function updates Clut #0 to #(NumEntries - 1)
  Clut       - Pointer to array of Clut.
               User should prepare the number of arrays specified by 'NumEntries'.
  
  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_ScreenVocaClutSet(const uint32_t                    Unit, 
                                    r_wm_Voca_t                *const Voca,
                                    const uint8_t                     NumEntries,
                                    const r_wm_VocaClutEntry_t *const Clut);

/***********************************************************
  Function: R_WM_ScreenActivityMonEnable
  
  Description:
  This function enables activity monitor of VOCA.
  Activity monitor notifies R_WM_EVENT_ACT_MON_ERROR 
  when one Video Output does not finish in the interval from LowerTime to UpperTime.
  
  LowerTime and UpperTime can be set in (100/3 = 33.3...) microseconds units. 
  WM driver rounds the setting value to the nearest value.
  
  Activity monitor can work when at least one Video Output Monitor is enabled by 
  <R_WM_ScreenVocaEnable> for the respective WM unit.
  
  Parameter:
  Unit      - See description of <R_WM_DevInit>
  UpperTime - Maximum detection time. Unit is microseconds.
              The range is 0 to 136467.
  LowerTime - Minimum detection time. Unit is microseconds.
              The range is 0 to 136467.
  
  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_ScreenActivityMonEnable(const uint32_t       Unit, 
                                          const uint32_t       UpperTime,
                                          const uint32_t       LowerTime);

/***********************************************************
  Function: R_WM_ScreenActivityMonDisable
  
  Description:
  This function disables activity monitor.
  
  Parameter:
  Unit      - See description of <R_WM_DevInit>
  
  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_ScreenActivityMonDisable(const uint32_t       Unit);


/***********************************************************
  Group: Windows
  
  Description:
  A window is an area with graphical content, which is placed on a screen. 
  Windows may cover the whole screen, but they could also cover part of the screen only. 

  It is possible to stack windows and to define transparency for a window. 
  If overlapping windows have the same Z position, 
  it is not defined, which window will be on the top. 

  A video output can have hardware support for windows (e.g. hardware layers). 

  Note:
  If an OS is used the Window functions internally use semaphores. 
  This must be taken into account if they are called by an interrupt service routine.
*/


/***********************************************************
  Function: R_WM_WindowCapabilitiesSet

  Description:
  This function configures the selectable window type, RLE window or Sprite window for each layer 
  set.
  One layer of WM Unit 0 and one layer of WM Unit 1 have the same configuration.
  Attention: The layer order of WM unit 1 is different from the layer order of WM unit 0.
  
  This function is available depending on RH850/D1x device. 
  
  For both RLE and Sprite settings, Frame buffer type window can be created in the layer.
  
  Default configuration is as follows.
    Capability0: RLE window
    Capability1: Sprite window
    Capability2: Sprite window
    Capability3: Sprite window

  All WM units should be in Uninitialized state before using this function.

  Parameter:
  Capability0  - Select window type for WM unit0 Layer0 and WM unit1 Layer0.
                   R_WM_WINCAPBS_RLE / R_WM_WINCAPBS_SPRITES
                 See <r_wm_WinCapbs_t>.
  Capability1  - Select window type for WM unit0 Layer1 and WM unit1 Layer3.
  Capability2  - Select window type for WM unit0 Layer2 and WM unit1 Layer2.
  Capability3  - select window type for WM unit0 Layer3 and WM unit1 Layer1.
  
  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_WindowCapabilitiesSet(const r_wm_WinCapbs_t Capability0,
                                        const r_wm_WinCapbs_t Capability1,
                                        const r_wm_WinCapbs_t Capability2,
                                        const r_wm_WinCapbs_t Capability3);

/***********************************************************
  Function: R_WM_WindowCreate

  Description:
  This function creates a window as specified in the Window parameter on screen Unit.
  User should initialize all members of parameter of Window structure before this function is 
  called.
  For example: 
  (code)
  memset(&Window, 0, sizeof(r_wm_Window_t))
  (end)
  
  The instance of Window structure must be hold until <R_WM_WindowDelete> is completed.
  When external frame buffer mode is selected, the instance of buffer information pointed by
  Window->Surface.Fb.Buffer must be hold until <R_WM_WindowDelete> is completed.

  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Window - This is a pointer to an <r_wm_Window_t> structure. 
           The structure has to be filled by the application 
           before calling the function.
  
  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_WindowCreate(const uint32_t       Unit,
                               r_wm_Window_t *const Window);

/***********************************************************
  Function: R_WM_WindowDelete

  Description:
  The function deletes the specified window.
  All internally allocated data buffer will be freed by this function.
  
  All created sprites data in the window should be deleted by <R_WM_SpriteDelete> or 
  <R_WM_WindowDeleteAllSprites> before this function is called.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Window - This is a window's structure <r_wm_Window_t> pointer
  
  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_WindowDelete(const uint32_t       Unit,
                               r_wm_Window_t *const Window);

/***********************************************************
  Function: R_WM_WindowEnable

  Description:
  This function enables the window. 
  The window will be visible on the screen after calling this function.

  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Window - This is a window's structure <r_wm_Window_t> pointer

  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_WindowEnable(const uint32_t       Unit,
                               r_wm_Window_t *const Window);

/***********************************************************
  Function: R_WM_WindowDisable

  Description:
  This function disables the window. The window will be invisible on the screen 
  after calling this function.

  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Window - This is a Window's structure <r_wm_Window_t> pointer

  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_WindowDisable(const uint32_t       Unit,
                                r_wm_Window_t *const Window);

/***********************************************************
  Function: R_WM_WindowMove

  Description:
  This function moves the window to the specified position.
  PosX and PosY should be set to a value greater than or equal to 0 in case of RLE window type.
  If Window position is out of screen, WM disable the layer internally.
  
  When PosZ is changed from previous value, all window will be re-constructed and changes layer 
  assignment of the Windows. 

  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Window - This is a window's structure <r_wm_Window_t> pointer
  PosX   - Specifies the new X position of the window on the screen. 
           Unit is pixels. Range is -1280 to 1279.
  PosY   - Specifies the new Y position of the window on the screen. 
           Unit is pixels. Range is -1024 to 1023.
  PosZ   - Specifies the new Z position of the window.
    
  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_WindowMove(const uint32_t       Unit,
                             r_wm_Window_t *const Window,
                             const int32_t        PosX,
                             const int32_t        PosY,
                             const int32_t        PosZ);

/***********************************************************
  Function: R_WM_WindowResize

  Description:
  This function changes the geometry of the window.
  The window can be dynamically resized until the life time of the window.

  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Window - This is a window's structure <r_wm_Window_t> pointer
  Pitch  - Specifies the distance in pixels between subsequent rows in the framebuffer memory. 
           Range is from (1024 / bpp) to (261120 / bpp).
  Width  - Specifies the window width in pixels. Range is 3 - 1280.
           When scaling-up is disabled, this value is also window frame buffer width.
  Height - Specifies the window height in pixels. Range is 1 - 1024.
           When scaling-up is disabled, this value is also window frame buffer height.

  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_WindowResize(const uint32_t        Unit,
                               r_wm_Window_t  *const Window,
                               const uint32_t        Pitch,
                               const uint32_t        Width,
                               const uint32_t        Height);

/***********************************************************
  Function: R_WM_WindowScaledSizeSet

  Description:
  This function changes the scaled size of the window.
  The window can be dynamically resized until the life time of the window.

  Parameter:
  Unit         - See description of <R_WM_DevInit>
  Window       - This is a window's structure <r_wm_Window_t> pointer
  ScaledWidth  - Original width when scaling-up. Unit is pixels. 
                 When scaling-up is disabled, set to 0.
                 When scaling-up is enabled, range is 4 to (Width-1).
  ScaledHeight - Original height when scaling-up. Unit is pixels. 
                 When scaling-up is disabled, set to 0.
                 When scaling-up is enabled, range is 4 to (Height-1).

  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_WindowScaledSizeSet(const uint32_t        Unit,
                                      r_wm_Window_t  *const Window,
                                      const uint32_t        ScaledWidth,
                                      const uint32_t        ScaledHeight);

/***********************************************************
  Function: R_WM_WindowColorFmtSet

  Description:
  This function changes the color format of the window.
  Color format should be changed to same bpp when window is enabled status. 
  Otherwise, need to care about frame buffer size and pitch condition.

  Parameter:
  Unit     - See description of <R_WM_DevInit>
  Window   - Pointer to the window's structure <r_wm_Window_t>
  ColorFmt - Window color format, see the description of <r_wm_WinColorFmt_t>

  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_WindowColorFmtSet(const uint32_t            Unit,
                                    r_wm_Window_t *const      Window,
                                    const r_wm_WinColorFmt_t  ColorFmt);

/***********************************************************
  Function: R_WM_WindowAlphaSet

  Description:
  This function changes the constant alpha value of the window.
  The alpha value multiplies the whole Pixel in the Window.
  
  If specified window is assigned to layer 0, constant alpha is not effective.
  If color format of specified window is YCbCr type, constant alpha is not effective.
  If Chromakey feature is enabled, execution of this function will fail.

  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Window - This is a window's structure <r_wm_Window_t> pointer
  Alpha  - New alpha value of the window. Range is 0 to 255
  
  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_WindowAlphaSet(const uint32_t        Unit,
                                 r_wm_Window_t  *const Window,
                                 const uint8_t         Alpha);

/***********************************************************
  Function: R_WM_WindowPremultipliedAlphaEnable

  Description:
  This function enables the window's pre-multiplied alpha mode.
  
  If specified window is assigned to layer 0, pre-multiplied alpha is not effective.
  If color format of specified window is YCbCr type or no alpha channel format, 
  pre-multiplied alpha value is not effective.

  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Window - This is a window's structure <r_wm_Window_t> pointer
  
  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_WindowPremultipliedAlphaEnable(const uint32_t       Unit,
                                                r_wm_Window_t *const Window);

/***********************************************************
  Function: R_WM_WindowPremultipliedAlphaDisable

  Description:
  This function disables the window's pre-multiplied alpha mode.

  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Window - This is a window's structure <r_wm_Window_t> pointer
  
  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_WindowPremultipliedAlphaDisable(const uint32_t       Unit,
                                                  r_wm_Window_t *const Window);

/***********************************************************
  Function: R_WM_WindowVerticalMirrorEnable

  Description:
  This function enables window's vertical mirroring.

  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Window - This is a window's structure <r_wm_Window_t> pointer
  
  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_WindowVerticalMirrorEnable(const uint32_t       Unit,
                                             r_wm_Window_t *const Window);

/***********************************************************
  Function: R_WM_WindowVerticalMirrorDisable

  Description:
  This function disables window's vertical mirroring.

  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Window - This is a window's structure <r_wm_Window_t> pointer
  
  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_WindowVerticalMirrorDisable(const uint32_t        Unit,
                                              r_wm_Window_t  *const Window);

/***********************************************************
  Function: R_WM_WindowSwap

  Description:
  This function swaps the window.
  
  If the window is a multi-buffer window, the buffer whose status is Render Started is switched 
  to the screen surface.
  <R_WM_WindowNewDrawBufGet> should be called before using this function in case of multi frame 
  buffer.  
  
  If the window is a single-buffer window, this function re-sets the frame buffer address holding 
  with Window->Surface.Fb.Buffer[0].Data.
  User can change the frame buffer address with modifying this parameter before this function is 
  called.
  
  Parameter:
  Unit       - See description of <R_WM_DevInit>
  Window     - Pointer to the window's structure <r_wm_Window_t>
    
  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_WindowSwap(const uint32_t       Unit,
                             r_wm_Window_t *const Window);

/***********************************************************
  Function: R_WM_WindowNewDrawBufGet

  Description:
  This function gives information about the next free buffer of the specified window.
  
  This function also changes the buffer status. See User's manual for the detail.
  In normal sequence, this function should be called after asynchronous process of 
  <R_WM_WindowCreate> is completed.

  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Window - This is a window's structure <r_wm_Window_t> pointer

  Return value:
  not R_NULL - Address of the next frame buffer, which can be used for drawing operations.
  R_NULL     - No free buffer is available or parameter is error.
*/

void *R_WM_WindowNewDrawBufGet(const uint32_t       Unit,
                               r_wm_Window_t *const Window);

/***********************************************************
  Function: R_WM_WindowVisibleBufGet

  Description:
  This function gives information about current On Screen buffer of the specified window.
  This function is used by WM porting layer.

  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Window - This is a window's structure <r_wm_Window_t> pointer

  Return value:
  not R_NULL - Address of the frame buffer which is currently on screen 
               (or scheduled to be on screen).
  R_NULL     - On Screen buffer is not found or parameter is error.
*/

void *R_WM_WindowVisibleBufGet(const uint32_t      Unit,
                               r_wm_Window_t const *Window);

/***********************************************************
  Function: R_WM_WindowCurrentDrawBufGet

  Description:
  This function gives information about the current Render Started frame buffer of the specified 
  window.

  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Window - This is a window's structure <r_wm_Window_t> pointer

  Return value:
  not R_NULL - Address of the frame buffer whose status is Render Started.
  R_NULL     - Render Started buffer is not found or parameter is error.
*/
void *R_WM_WindowCurrentDrawBufGet(const uint32_t       Unit,
                                   r_wm_Window_t *const Window);

/***********************************************************
  Function: R_WM_WindowExternalBufSet

  Description:
  This function dynamically changes the frame buffers for windows with externally allocated 
  buffers.
  
  If the window frame buffer was internal allocate mode, this function frees the all internal 
  buffers and management area. Frame buffer allocate mode is changed to external mode.

  One of the following status conditions shall be set as new external buffer.
  - All buffer status are Free.
  - One buffer status is Render Finished. The other buffer status are Free.

  The instance of buffer information pointed by Buf must be hold until <R_WM_WindowDelete> is 
  completed.
  
  Parameter:
  Unit        - See description of <R_WM_DevInit>
  Window      - Pointer to the window's structure <r_wm_Window_t>
  Buf         - Buffer array
  BufNum      - Number of buffers in the array
  ColorFormat - Color format of the buffers

  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_WindowExternalBufSet(const uint32_t            Unit,
                                       r_wm_Window_t *const      Window,
                                       r_wm_WinBuffer_t *const   Buf,
                                       const uint32_t            BufNum,
                                       const r_wm_WinColorFmt_t  ColorFormat);

/***********************************************************
  Function: R_WM_WindowColorKeyEnable

  Description:
  This function enables the chromakey (color keying) of the specified window.
  
  Target color (before change) and replaced color (after change) should be set to window 
  structure when <R_WM_WindowCreate> is called.

  If chromakey is set to enable, the constant alpha setting will be disabled.
  If specified window is assigned to layer 0, this function will fail.
  The chromakey is supported when window color format is RGB type.

  Parameters:
  Unit    - See description of <R_WM_DevInit>
  Window  - This is a window's structure <r_wm_Window_t> pointer

  Returns value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_WindowColorKeyEnable(const uint32_t       Unit, 
                                       r_wm_Window_t *const Window);

/***********************************************************
  Function: R_WM_WindowColorKeyDisable

  Description:
  This function disables the chromakey (color keying) of the selected Window.
  
  If user disables the chroma key, constant alpha settings will be effective.
  If specified window is assigned to layer 0, this function will fail.
  
  Parameters:
  Unit    - See description of <R_WM_DevInit>
  Window  - window's structure <r_wm_Window_t> pointer

  Returns value:
  See <r_wm_Error_t>.
*/
r_wm_Error_t R_WM_WindowColorKeyDisable(const uint32_t       Unit, 
                                        r_wm_Window_t *const Window);

/***********************************************************
  Function: R_WM_WindowClutSet
  
  Description:
  This function sets color look-up table (CLUT).
  
  The instance of CLUT must be hold until <R_WM_FrameWait> corresponding to this function 
  is completed.
  This function is applicable for the windows with CLUT color modes specified.
  
  Parameters:
  Unit        - See description of <R_WM_DevInit>
  Window      - window's structure <r_wm_Window_t> pointer
  NumEntries  - Specifies the number of entries in the CLUT.
  Clut        - Specifies the pointer to CLUT. It must be 32 bits aligned. See <r_wm_ClutEntry_t>. 
*/

r_wm_Error_t R_WM_WindowClutSet(const uint32_t                 Unit,
                                r_wm_Window_t *const           Window,
                                const uint32_t                 NumEntries,
                                const r_wm_ClutEntry_t  *const Clut);

/***********************************************************
  Function: R_WM_WindowDeleteAllSprites

  Description:
  This function disables and deletes all Sprite data associated to the window.
  
  Parameters:
  Unit        - See description of <R_WM_DevInit>
  Window      - window's structure <r_wm_Window_t> pointer
  
  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_WindowDeleteAllSprites(const uint32_t       Unit,
                                         r_wm_Window_t *const Window);

/***********************************************************
  Group: Message Queue
*/

/***********************************************************
  Function: R_WM_FrameEndMark

  Description:
  This function marks end for message queue with specified Id.
  
  User should adjust this function to call only once during 1 V-sync period 
  when message queue is executed with <R_WM_FrameExecuteNext>.
  
  Parameters:
  Unit  - See description of <R_WM_DevInit>
  Id    - Specifies the ID of the frame up to which all the requests should be executed.
  
  Return value:
  See <r_wm_Error_t>.
*/
                                
r_wm_Error_t R_WM_FrameEndMark(const uint32_t Unit, 
                               const uint16_t Id);

/***********************************************************
  Function: R_WM_FrameWait

  Description:
  This function executes following sequence.
  - Before processing the message queue, this function waits until the Hardware is ready to be 
    reconfigured. The point of time of this "Scanline Event" can be configured with the function 
    <R_WM_Sys_DevEventRegister>. 
  - After processing the queue, this function waits for the "VSync Event" until the Hardware 
    accepted the new configuration.
  - Please note, if the "Scanline Event" already occurred for the current frame, this function 
    will wait for the next event, thus it will skip one frame.
    The same applies to the "VSync Event". If processing the message queue took too much time to 
    miss the current event, this function will wait for the next event and skip one frame.
    
  Default sequence:
  The call to <R_WM_FrameWait> consists of 
  - WAIT (for "Scanline Event")
  - EXEC (process the message queue for VDCE/SPEA)
  - WAIT (for "VSync Event")
  - EXEC (process the message queue for DISCOM)
  - EXEC (process the message queue for VOCA)
  
  (code)
  VSync      ScanLine        VSync      ScanLine         VSync
    |            |             |            |             |   
    | DRW |      |             | DRW |      |             |   
    |     | WAIT |             |     | WAIT |             |   
    |            | EXEC |      |            | EXEC |      |   
    |            |      | WAIT |            |      | WAIT |   
  (end)

  Problematic sequence:
  Case 1 - The "Scanline Event" is configured to occur while drawing is still active.
  The execution of the Message Queue will happen on the following "Scanline Event".
  
  (code)
  VSync      ScanLine        VSync      ScanLine         VSync
    |            |             |            |             |   
    | DRW --------> |          |            |             |   
    |            |  | WAIT ---------------> |             |   
    |            |             |            | EXEC |      |   
    |            |             |            |      | WAIT |   
  (end)
  
  Case 2 - The "Scanline Event" is configured to occur too close to the "Vsync Event".
  Control will be given back to user application only after the following "VSync Event"
  
  (code)
  VSync            ScanLine  VSync            ScanLine   VSync
    |                  |       |                  |       |
    | DRW |            |       |                  |       |
    |     | WAIT ----> |       |                  |       |
    |                  | EXEC --> |               |       |
    |                  |       |  | WAIT ---------------> |
  (end)
  
  Parameters:
  Unit  - See description of <R_WM_DevInit>
  Id    - ID of the frame up to which all the requests should be executed

  Return value:
  See <r_wm_Error_t>.
  
  Remarks:
  This function blocks until all the requests scheduled up to the specified frame delimiter
  are executed, and the corresponding VDCE H/W update is finished.
  
*/
r_wm_Error_t R_WM_FrameWait(const uint32_t Unit, 
                            const uint16_t Id);

/***********************************************************
  Function: R_WM_FrameExecuteNext

  Description:
  This function executes jobs in message queue.
  R_WM_FrameExecuteNext doesn't wait for any interrupt.
  
  If R_WM_FrameExecuteNext function is used, user should control similar to <R_WM_FrameWait> 
  with using interrupt.
  
  When the job of <R_WM_FrameEndMark> is found, update processing for the next frame is executed.
  User should adjust this update process so that it will be executed slightly before V-sync event.
  It should be especially careful when there is control over Sprite, RLE, and CLUT.  
  
  Parameters:
  Unit  - See description of <R_WM_DevInit>

  Return value:
  0 ~ 0xFFFF                             - The encountered frame-end ID.
  0x80000000u                            - Error occurs during job execution.
  0x80000000u | R_WM_ERR_INVALID_WM_UNIT - Unit number is outside the range.
  0x80000000u | R_WM_ERR_NOT_INITIALIZED - WM driver is not initialized.
  0x80000000u | R_WM_ERR_EVENT_FAILED    - Received an unexpected number of messages.
  
  Remarks:
  This function can be used for implementing the custom drawing mechanism, apart from the
  synchronous one provided by default.
*/
uint32_t R_WM_FrameExecuteNext(const uint32_t Unit);

/***********************************************************
  Function: R_WM_FrameExecuteVoca

  Description:
  This function executes jobs for VOCA in message queue.
  This function doesn't wait for any interrupt.
  
  Parameters:
  Unit  - See description of <R_WM_DevInit>

  Return value:
  See <r_wm_Error_t>.
  
  Remarks:
  This function can be used for implementing the custom drawing mechanism, apart from the
  synchronous one provided by default.
*/
r_wm_Error_t R_WM_FrameExecuteVoca(const uint32_t Unit);

/***********************************************************
  Function: R_WM_FrameExecuteDiscom

  Description:
  This function executes jobs for DISCOM in message queue.
  This function doesn't wait for any interrupt.
  
  Parameters:
  Unit  - See description of <R_WM_DevInit>

  Return value:
  See <r_wm_Error_t>.
  
  Remarks:
  This function can be used for implementing the custom drawing mechanism, apart from the
  synchronous one provided by default.
*/
r_wm_Error_t R_WM_FrameExecuteDiscom(const uint32_t Unit);

/***********************************************************
  Group: Video Capture

  The window manager offers the possibility to create surfaces 
  for the video capturing.
*/

/***********************************************************
  Function: R_WM_CaptureCreate

  Description:
  This function creates a video capture surface inside a specific frame buffer type window.
  For each WM unit, only one video capture surface can be created.

  When video capture is used, following window color format is supported.
  - R_WM_COLORFMT_RGB0888
  - R_WM_COLORFMT_ARGB8888
  - R_WM_COLORFMT_RGB565
  
  The instance of capture structure must be hold until <R_WM_CaptureDelete> is completed

  Create a video capture surface inside a specific window on the screen Unit. 

  Parameter:
  Unit    - See description of <R_WM_DevInit>
  Capture - This is a pointer to an <r_wm_Capture_t> structure. 
            The structure has to be filled by the application before calling the function.

  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_CaptureCreate(const uint32_t        Unit,
                                r_wm_Capture_t *const Capture);

/***********************************************************
  Function: R_WM_CaptureDelete

  Description:
  This function deletes the specified capturing surface.
  
  Parameter:
  Unit    - See description of <R_WM_DevInit>.
  Capture - This is a pointer to the capturing surface's structure <r_wm_Capture_t>.

  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_CaptureDelete(const uint32_t        Unit,
                                r_wm_Capture_t *const Capture);

/***********************************************************
  Function: R_WM_CaptureEnable

  Description:
  This function enables the video capturing surface and start the video capturing.
  <R_WM_WindowEnable> is also needed to control window during capturing.
  
  Parameter:
  Unit    - See description of <R_WM_DevInit>
  Capture - This is a pointer to the capturing surface's structure <r_wm_Capture_t>.

  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_CaptureEnable(const uint32_t        Unit,
                                r_wm_Capture_t *const Capture);

/***********************************************************
  Function: R_WM_CaptureDisable

  Description:
  This function disables the video capturing.
  After this function call, the last frame will remain in the frame buffer.
  
  Parameter:
  Unit    - See description of <R_WM_DevInit>
  Capture - This is a pointer to the capturing surface's structure <r_wm_Capture_t>.

  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_CaptureDisable(const uint32_t        Unit,
                                 r_wm_Capture_t *const Capture);

/***********************************************************
  Function: R_WM_CaptureMove

  Description:
  This function moves the capturing start position.
  Dynamic changes can cause momentary screen disruptions. 
  To avoid this, change to Disabled state by <R_WM_CaptureDisable> and then call this function.
  
  Parameter:
  Unit    - See description of <R_WM_DevInit>
  Capture - This is a pointer to the capturing surface's structure <r_wm_Capture_t>.
  StartX  - X position of capturing start. Unit is pixels. 
            This is same as StartX in <r_wm_Capture_t>.
  StartY1 - Y position of capturing start. Unit is pixels. 
            This is same as StartY1 in <r_wm_Capture_t>.
  
  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_CaptureMove(const uint32_t        Unit,
                              r_wm_Capture_t *const Capture,
                              const uint32_t        StartX,
                              const uint32_t        StartY1);

/***********************************************************
  Function: R_WM_CaptureResize

  Description:
  This function resizes the capturing signal size.
  If scaling-down is disabled, Width and Height is also capture buffer size.
  Dynamic changes can cause momentary screen disruptions. 
  To avoid this, change to Disabled state by <R_WM_CaptureDisable> and then call this function.
  
  Parameter:
  Unit    - See description of <R_WM_DevInit>
  Capture - This is a pointer to the capturing surface's structure <r_wm_Capture_t>.
  Width   - Width of capturing video data. Unit is pixels.
            This is same as Width in <r_wm_Capture_t>.
  Height  - Height of capturing video data. Unit is pixels.
            This is same as Height in <r_wm_Capture_t>.

  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_CaptureResize(const uint32_t        Unit,
                                r_wm_Capture_t *const Capture,
                                const uint32_t        Width,
                                const uint32_t        Height);

/***********************************************************
  Function: R_WM_CaptureScaledSizeSet

  Description:
  This function resize the scaling-down size. (It is capture buffer size.)
  
  Parameter:
  Unit         - See description of <R_WM_DevInit>
  Capture      - This is a pointer to the capturing surface's structure <r_wm_Capture_t>.
  ScaledWidth  - Frame buffer width when scaling-down. Unit is pixels. 
                 This is same as ScaledWidth in <r_wm_Capture_t>.
  ScaledHeight - Frame buffer height when scaling-down. Unit is pixels. 
                 This is same as ScaledHeight in <r_wm_Capture_t>.

  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_CaptureScaledSizeSet(const uint32_t        Unit,
                                       r_wm_Capture_t *const Capture,
                                       const uint32_t        ScaledWidth,
                                       const uint32_t        ScaledHeight);

/***********************************************************
  Function: R_WM_CaptureExtVsyncSet

  Description:
  This function sets the Hsync cycle of input signal and Vsync protection.
  
  Hsync cycle is used to determine the field of the interlace signal.
  In case of NTSC signal input, the default value is 1716.
  In case of PAL signal input, the default value is 1728.
  If HsyncCycle is set as 0 or this function is not called, 
  the default value is selected automatically.
  NTSC or PAL is selected by R_WM_CAPMODE_PAL flag.

  Vsync protections are effective when external Vsync synchronization is selected 
  (R_WM_CAPMODE_FIXED_VSYNC is off). Vsync protections are disabled by default.
  Vsync protection timing is made by PixelClock set by R_WM_ScreenTimingSet.
  The maximum value of VsyncMaskUs and VsyncLackUs is (8,388,480 / PixelClock[MHz])[usec]
  If setting value is over the range, this function assumes that the maximum value is set.

  Parameter:
  Unit         - See description of <R_WM_DevInit>
  Capture      - This is a pointer to the capturing surface's structure <r_wm_Capture_t>.
  HsyncCycle   - Horizontal cycle of input signal. Unit is cycle (pixel). The range is 4 to 2046.
                 If 0 is set, WM driver set automatically.
  VsyncMaskUs  - Prevent Vsync coming faster than VsyncMaskUs [usec].
                 If non-0 value is set, vsync masking is enabled. 
                 If 0 is set, vsync masking is disabled.
  VsyncLackUs  - Compensate Vsync coming slower than VsyncLackUs [usec]. 
                 If non-0 value is set, vsync compensation is enabled. 
                 If 0 is set, vsync compensation is disabled.

  Return value:
  See <r_wm_Error_t>. 
*/

r_wm_Error_t R_WM_CaptureExtVsyncSet(const uint32_t             Unit,
                                     r_wm_Capture_t      *const Capture,
                                     const uint16_t             HsyncCycle,
                                     const uint32_t             VsyncMaskUs,
                                     const uint32_t             VsyncLackUs);

/***********************************************************
  Function: R_WM_CapBufGet

  Description:
  This function gets the Free buffer to be assigned for video capture buffer.
  This function is called from WM porting layer.
  This function changes the buffer status.
  
  Parameter:
  Unit    - See description of <R_WM_DevInit>
  Window  - This is a pointer to the parent window handling the capturing.

  Returns:
  not R_NULL - Pointer to frame buffer information of Free buffer.
  R_NULL     - Free buffer is not found or parameter is error.
*/
r_wm_WinBuffer_t *R_WM_Cap_CapBufGet(const uint32_t       Unit, 
                                     r_wm_Window_t *const Window);

/**********************************************************
  Function: R_WM_CapDispBufGet

  Description:
  This function gets the Render finished buffer to be assigned for on-screen buffer.
  This function is called from WM porting layer.
  This function changes the buffer status.
  
  Parameter:
  Unit    - See description of <R_WM_DevInit>
  Window  - This is a pointer to the parent window handling the capturing.

  Returns:
  not R_NULL - Pointer to frame buffer information of Render finished buffer.
  R_NULL     - Render finished buffer is not found or parameter is error.
*/
r_wm_WinBuffer_t *R_WM_Cap_DispBufGet(const uint32_t       Unit, 
                                      r_wm_Window_t *const Window);

/***********************************************************
  Group: Sprites
*/

/***********************************************************
  Function: R_WM_SpriteCreate

  Description:
  This function creates a Sprite data to sprite window.
  
  The Sprite data will not be visible until it is enabled.
  Up to 32 Sprite data can be created per window, but up to 16 Sprite data can be visible 
  per window in descending order of Sprite->PosZ.
  
  The instance of sprite structure must be hold until <R_WM_SpriteDelete> or 
  <R_WM_WindowDeleteAllSprites> is completed.
  
  Parameter:
  Unit      -   See description of <R_WM_DevInit>
  Sprite    -   <r_wm_Sprite_t> structure pointer
                The structure has to be filled by the application before calling the function.

  Return value:
  See <r_wm_Error_t>.
*/
r_wm_Error_t R_WM_SpriteCreate(const uint32_t       Unit, 
                               r_wm_Sprite_t *const Sprite);

/***********************************************************
  Function: R_WM_SpriteEnable
  
  Description:
  This function enables the specified Sprite data.
  The Sprite data becomes visible on the screen.
  Up to 16 Sprite data can be visible per window in descending order of Sprite->PosZ.

  Parameter:
  Unit      -   See description of <R_WM_DevInit>
  Sprite    -   <r_wm_Sprite_t> structure pointer

  Return value:
  See <r_wm_Error_t>.
*/
r_wm_Error_t R_WM_SpriteEnable(const uint32_t       Unit, 
                               r_wm_Sprite_t *const Sprite);

/***********************************************************
  Function: R_WM_SpriteDisable
  
  Description:
  This function disables the specified Sprite.
  The Sprite data becomes not visible on the screen.
  
  Parameter:
  Unit      -   See description of <R_WM_DevInit>
  Sprite    -   <r_wm_Sprite_t> structure pointer

  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_SpriteDisable(const uint32_t       Unit, 
                                r_wm_Sprite_t *const Sprite);

/***********************************************************
  Function: R_WM_SpriteMove
  
  Description:
  This function moves the Sprite to the specified X/Y and Z-order location.
  
  Z position is the relative position of the Sprite data in the sprite window. 
  The one with the larger value takes precedence.

  Parameter:
  Unit    - See description of <R_WM_DevInit>
  Sprite  - <r_wm_Sprite_t> structure pointer
  PosX    - Specifies the new Sprite X position. Unit is pixels.
  PosY    - Specifies the new Sprite X position. Unit is pixels.
  PosZ    - Specifies the new Sprite Z position.
  
  Return value:
  See <r_wm_Error_t>.
  
  Remarks:
  The PosZ specifies the relative position of the sprite in the sprites Z-order.
  Changing the PosZ of one sprite can yield SPEA registers writes for all the sprites
  defined within a window.
*/

r_wm_Error_t R_WM_SpriteMove(const uint32_t       Unit,
                             r_wm_Sprite_t *const Sprite,
                             const uint32_t       PosX,
                             const uint32_t       PosY,
                             const uint32_t       PosZ);

/***********************************************************
  Function: R_WM_SpriteBufSet
  
  Description:
  This function changes the Sprite data source address.
  
  Parameter:
  Unit    - See description of <R_WM_DevInit>
  Sprite  - <r_wm_Sprite_t> structure pointer
  Buffer  - Specifies the new address stored the sprite source data.
            The address should be 8 Bytes alignment.
  
  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_SpriteBufSet(const uint32_t       Unit, 
                               r_wm_Sprite_t *const Sprite, 
                               const void *const    Buffer);

/***********************************************************
  Function: R_WM_SpriteDelete
  
  Description:
  This function removes the specified Sprite data from the host window.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Sprite - <r_wm_Sprite_t> structure pointer
  
  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_SpriteDelete(const uint32_t       Unit, 
                               r_wm_Sprite_t *const Sprite);

/***********************************************************
  Group: General
*/

/***********************************************************
  Function: R_WM_ErrorCallbackSet

  Description:
  This function sets the error callback function.
  
  Callback is notified when an error occurs in WM, VDCE or SPEA driver.
  Because this callback is the only way to get asynchronous control error details, 
  it is recommended to set the callback.
  
  Regarding the callback parameter Unit and Error, Unit has different meaning depending on Error.
  R_WM_ERR_VOUT_INTERNAL - VDCE unit number (For capture control across two VDCE units, 
                                             the VDCE unit may not match the WM unit.)
  R_WM_ERR_SPEA_INTERNAL - SPEA unit number (It is always 0.)
  R_WM_ERR_VOCA_INTERNAL - VOCA unit number (It is always 0.)
  others                 - WM unit number   
  
  Following error is notified to Unit = 0.
  - WM Unit is error
  - Error of function without WM unit in argument.
  
  Callback with Unit = 0 is notified to the function installed to Unit = 0.
  Callback with Unit = 1 is notified to the function installed to Unit = 1.
  
  Error callback is notified during WM unit is not de-initialized state.
  The installed error callback can be uninstalled by R_WM_DevInit or R_NULL setting 
  by this function.
  
  Parameter:
  Unit    - See description of <R_WM_DevInit>
  ErrorCb - Error callback function

  Return value:
  See <r_wm_Error_t>. 
*/
r_wm_Error_t R_WM_ErrorCallbackSet(const uint32_t  Unit,
                                   void            (*ErrorCb)(uint32_t     Unit,
                                                              r_wm_Error_t Error));

/***********************************************************
  Function: R_WM_ErrorHandler

  Description:
  This function is the driver's central error handler. 
  
  This function is internal functions. Application does not need to use this function.
  WM Porting layer also calls this function to execute error callback set by R_WM_ErrorCallbackSet.
  
  Unit   -   See description of <R_WM_DevInit>
  Error  -   See <r_wm_Error_t>.

  Return value:
  None.

*/
void R_WM_ErrorHandler(const uint32_t      Unit, 
                       const r_wm_Error_t  Error);

/***********************************************************
  Function: R_WM_ColorFmtBitsPerPixGet

  Description:
  This function returns the bits per pixel (1 - 32) for the specified format.
  
  Parameter:
  Format  - Color format. See <r_wm_WinColorFmt_t>.
  
  Return value:
  not 0 - Bits per pixel of the specified format.
  0     - Unsupported format is specified.
*/
uint8_t R_WM_ColorFmtBitsPerPixGet(const r_wm_WinColorFmt_t Format);

/***********************************************************
  Group: DISCOM (Display Output Comparator)
*/

/***********************************************************
  Function: R_WM_DiscomCreate
  
  Description:
  This function creates a Discom device.
  A Discom device can be created per one Discom Unit.
  Two Discom device can be created per one WM unit.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Discom - <r_wm_Discom_t> structure pointer.
           The structure has to be filled by the application before calling the function.
  
  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_DiscomCreate(const uint32_t       Unit, 
                               r_wm_Discom_t *const Discom);

/***********************************************************
  Function: R_WM_DiscomDelete
  
  Description:
  This function deletes a Discom device.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Discom - <r_wm_Discom_t> structure pointer.
  
  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_DiscomDelete(const uint32_t       Unit, 
                               r_wm_Discom_t *const Discom);

/***********************************************************
  Function: R_WM_DiscomEnable
  
  Description:
  This function enables a Discom device.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Discom - <r_wm_Discom_t> structure pointer.
  
  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_DiscomEnable(const uint32_t       Unit, 
                               r_wm_Discom_t *const Discom);

/***********************************************************
  Function: R_WM_DiscomDisable
  
  Description:
  This function disables a Discom device.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Discom - <r_wm_Discom_t> structure pointer.
  
  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_DiscomDisable(const uint32_t       Unit, 
                                r_wm_Discom_t *const Discom);

/***********************************************************
  Function: R_WM_DiscomCrcSet
  
  Description:
  This function changes expected CRC of Discom device.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Discom - <r_wm_Discom_t> structure pointer.
  ExpCrc - Expected CRC
  
  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_DiscomCrcSet(const uint32_t       Unit, 
                               r_wm_Discom_t *const Discom,
                               const uint32_t       ExpCrc);

/***********************************************************
  Function: R_WM_DiscomCrcGet
  
  Description:
  This function gets the latest calculated CRC value.
  
  Parameter:
  Unit   - See description of <R_WM_DevInit>
  Discom - <r_wm_Discom_t> structure pointer.
  Crc    - The latest calculated CRC value.
  
  Return value:
  See <r_wm_Error_t>.
*/

r_wm_Error_t R_WM_DiscomCrcGet(const uint32_t       Unit, 
                               r_wm_Discom_t *const Discom,
                               uint32_t      *const Crc);


#ifdef __cplusplus
}
#endif


#endif /* R_WM_API_H */
