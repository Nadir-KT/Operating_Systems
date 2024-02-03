/***********************************************************************************************
PROJECT : simple_draw
FILE   : $Id: config.h 17203 2020-06-03 02:39:46Z shinya.tomari.wj@renesas.com $
============================================================================ 
DESCRIPTION
config of the simple_draw
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
#ifndef CONFIG_H_
#define CONFIG_H_


#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Section: Global Defines
*/

/*******************************************************************************
  Define: USE_HDMI0_INTERFACE

  Use HDMI output instead of QVGA display.
  Note: Normal HDMI Computer Monitors won't work with: LOC_CAPTURE_SYNCHRONIZED = 1 
        in case of 50Hz input.
*/
/* #define USE_HDMI0_INTERFACE */

/*******************************************************************************
  Define: LOC_CAPTURE_SYNCHRONIZED
  
  Synchronized:       Single Buffer
    Both input and output use the same buffer. 
    When configured to use the right VI->VO delay, no tearing is visible. 
    If tearing is visible, it will alway be the same line!
        
    Default Delay is zero.
    Recommended Delay for above settings is 70 Lines.

  Un-Synchronized:    Triple Buffer 
    Both input and output run on their own frequency.
    Duplicate VI/VO VBLANK events, are ignored until the next opposite VO/VI event.
    Tearing should never occur in this configuration.
 */
#ifndef LOC_CAPTURE_SYNCHRONIZED
  #define LOC_CAPTURE_SYNCHRONIZED            0
#endif

/*******************************************************************************
  Define: LOC_DEINTERLACE_MODE
  
  De-Interlace mode:
  0     - R_WM_CAPMODE_BOB_TOP    Only top field is written to capture buffer.
  1     - R_WM_CAPMODE_BOB_BOTTOM Only bottom field is written to capture buffer.
  2     - R_WM_CAPMODE_WEAVE      Output by interweaving top field and bottom field alternately.
  Other - R_WM_CAPMODE_NONE       Both top field and bottom field is written to same capture buffer.

  Note: WEAVE mode works with Single Buffer and USE_HDMI0_INTERFACE is defined. 
 */
#define LOC_DEINTERLACE_BOB_TOP         0
#define LOC_DEINTERLACE_BOB_BOTTOM      1
#define LOC_DEINTERLACE_WEAVE           2
#define LOC_DEINTERLACE_BOB_BOTH        3

#ifndef LOC_DEINTERLACE_MODE
  #define LOC_DEINTERLACE_MODE          LOC_DEINTERLACE_BOB_BOTTOM
#endif

/*******************************************************************************
  Define: Display infos

  LOC_DISPLAY_NAME     - display string
  LOC_DISPLAY_WIDTH    - display width
  LOC_DISPLAY_HEIGHT   - display height
  LOC_DISPLAY_BPP      - bytes per pixel
*/

#ifndef USE_HDMI0_INTERFACE
  #define LOC_CAPTURE_DISPLAY_NAME      "NEC_QVGA_TOUCH_240x320"
  #define LOC_DISPLAY_BPP               4
  #define LOC_DISPLAY_WIDTH             240
  #define LOC_DISPLAY_HEIGHT            320
#else
  #define LOC_CAPTURE_DISPLAY_NAME      "VESA_640x480_60Hz"
  #define LOC_DISPLAY_BPP               4
  #define LOC_DISPLAY_WIDTH             640
  #define LOC_DISPLAY_HEIGHT            480
#endif

#define LOC_SECOND_DISPLAY_NAME         "VESA_640x480_60Hz"


/*******************************************************************************
  Define: Capture configuration
*/
/* Real dimensions of the camera signal */
#define LOC_CAMERA_WIDTH                    720
#if (USE_VIN_NTSC == 0)
  #define LOC_CAMERA_HEIGHT                 568 /* reduce 8 line to delete noise in weave mode */
  #define LOC_CAPTURE_INPUT_STARTX          288
  #define LOC_CAPTURE_INPUT_STARTY          24
#else
  #define LOC_CAMERA_HEIGHT                 480
  #define LOC_CAPTURE_INPUT_STARTX          276
  #define LOC_CAPTURE_INPUT_STARTY          16
#endif

/* Pixels to be captured */
/* Use full input pixels */
/* this is the doubled width, because every pixel is captured twice when using ITU656; see VDCE API doc */
#define LOC_CAPTURE_INPUT_WIDTH             (LOC_CAMERA_WIDTH * 2)
#define LOC_CAPTURE_INPUT_HEIGHT            (LOC_CAMERA_HEIGHT / 2) /* Interlaced video has halfed vertical resolution */
#define LOC_CAPTURE_INPUT_DELAY             70

/* Some tested devices */
/* Canon DSC Camera in NTSC-Mode:  720x480 (240 interleaved) StartXY: 16x20 60Hz */
/* Canon DSC Camera in PAL-Mode:   720x576 (288 interleaved) StartXY: 16x20 50Hz */
/* ACME Camera Module in PAL-Mode: 720x576 (288 interleaved) StartXY: 16x24 50Hz */

/* Down-scaling of the input video. */
#ifndef USE_HDMI0_INTERFACE
  #define LOC_CAPTURE_BUFFER_WIDTH            240 /* Scale-down */
  #define LOC_CAPTURE_BUFFER_HEIGHT           200 /* Scale-down */
#else
  #if (LOC_DEINTERLACE_MODE == LOC_DEINTERLACE_WEAVE)
    #define LOC_CAPTURE_BUFFER_WIDTH          LOC_DISPLAY_WIDTH        /* Scale-down (LOC_DISPLAY_WIDTH < LOC_CAMERA_WIDTH) */
    #define LOC_CAPTURE_BUFFER_HEIGHT         LOC_DISPLAY_HEIGHT       /* Scale-down (LOC_DISPLAY_HEIGHT <= LOC_CAMERA_HEIGHT) */
  #else
    #define LOC_CAPTURE_BUFFER_WIDTH          LOC_DISPLAY_WIDTH        /* Scale-down (LOC_DISPLAY_WIDTH < LOC_CAMERA_WIDTH) */
    #define LOC_CAPTURE_BUFFER_HEIGHT         LOC_CAPTURE_INPUT_HEIGHT /* as-is */
  #endif
#endif
#define LOC_CAPTURE_BUFFER_STRIDE             ALIGN_UP(LOC_CAPTURE_BUFFER_WIDTH, 32)

/* Video input layout excluding special handling of ITU565 */
#ifndef USE_HDMI0_INTERFACE
  #define LOC_CAPTURE_WINDOW_WIDTH            LOC_CAPTURE_BUFFER_WIDTH
  #define LOC_CAPTURE_WINDOW_HEIGHT           LOC_CAPTURE_BUFFER_HEIGHT
  #define LOC_CAPTURE_WINDOW_POSY             LOC_DISPLAY_HEIGHT - LOC_CAPTURE_BUFFER_HEIGHT
#else
  #define LOC_CAPTURE_WINDOW_WIDTH            LOC_CAPTURE_BUFFER_WIDTH
  #define LOC_CAPTURE_WINDOW_HEIGHT           LOC_DISPLAY_HEIGHT /* Scale-up (LOC_DISPLAY_HEIGHT <= LOC_CAMERA_HEIGHT) */
  #define LOC_CAPTURE_WINDOW_POSY             0
#endif

/* Window containign the buttons to control capturing */
#define LOC_CTRL_WINDOW_WIDTH               240
#define LOC_CTRL_WINDOW_STRIDE              2048    /* D1Mx HW specific */
#define LOC_CTRL_WINDOW_HEIGHT              120

/*******************************************************************************
  Memory setup
*/

/* bitmap width */
#define LOC_BMP_WIDTH   64
/* bitmap height */
#define LOC_BMP_HEIGHT  32
/* bitmap size */
#define LOC_BMP_SIZE (LOC_BMP_WIDTH * LOC_BMP_HEIGHT * 4)

/*******************************************************************************
  Define: HEAP_SIZE

  Drawing engine driver CPU heap memory.

  Byte size of the drawing engine driver local heap. It
  is used for internal structures and for preparation of
  the drawing engine command lists. Its minimum size depends 
  on  the size of local command lists, number of drawing
  contexts and number of video memory blocks.
*/
#define LOC_HEAP_SIZE           (LOC_CPU_HP_BLOCK_NB * LOC_CPU_HP_BLOCK_SIZE)

/*******************************************************************************
  Define: CPU_HP_BLOCK_NB

  Number of blocks the CPU heap is divided into
  Heap Memory is allocated blockwise.
*/
#define LOC_CPU_HP_BLOCK_NB     0x800

/*******************************************************************************
  Define: CPU_HP_BLOCK_SIZE

  Size of one CPU heap block 
*/
#define LOC_CPU_HP_BLOCK_SIZE   0x40


/*******************************************************************************
  Define: ERRN_NG
  
  Error Constant set to -1
  Is used as return value when an error occurs.
*/
#define ERR_NG -1


/*******************************************************************************
  Some tool functions
*/
#define MIN(a,b)    ( (a>b) ? b : a )
#define MAX(a,b)    ( (a<b) ? b : a )
#define ALIGN_UP(x,a)  ( (x+a-1) & ~(a-1) )




#define LOC_JCUA_UNIT (0u)
#define LOC_SFMA_UNIT (0u)



#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H_ */
