/************************************************************************************************
PROJECT : simple_capture
FILE    : $Id: main.c 17203 2020-06-03 02:39:46Z shinya.tomari.wj@renesas.com $
============================================================================ 
DESCRIPTION
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

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"         /* Renesas basic types, e.g. uint32_t */
#include "r_bsp_api.h"          /* Board support package */
#include "r_bsp_stdio_api.h"    /* Printf */
#include "r_ddb_api.h"
#include "r_bsp_hmi_api.h"      /* Board support for knob input device */
#include "r_cdi_api.h"

#include "r_wm_api.h"
#include "r_vdce_api.h"
#include "r_tick_api.h"
#include "wm.h"
#include "mem_init.h"
#include "r_dev_api.h"

#include "config.h"
#include <string.h>

/***********************************************************
  Section: Local Defines
*/

#define LOC_SEL_DIM     8

#define LOC_ON_SPRITE       0
#define LOC_OFF_SPRITE      1
#define LOC_RESUME_SPRITE   2
#define LOC_PAUSE_SPRITE    3
#define LOC_SEL_SPRITE      4

/***********************************************************
  Section: Global Variables
*/

/***********************************************************
  Variable: g_*Btn
  
  External definitions of the button bitmaps.
*/
extern const unsigned char g_OnBtn[LOC_BMP_SIZE];
extern const unsigned char g_OffBtn[LOC_BMP_SIZE];
extern const unsigned char g_PauseBtn[LOC_BMP_SIZE];
extern const unsigned char g_ResumeBtn[LOC_BMP_SIZE];
extern const unsigned char g_Sel[LOC_SEL_DIM * LOC_SEL_DIM * 4];

extern uint32_t loc_ContensAddr;
extern uint32_t loc_ContensSize;

static const r_dev_PinConfig_t loc_Vi0ItuCapute_PinConfig[] = 
/*  Port Pin Func      Dir      Feedback     Pull        OpenDrain   HiCurr   HiSpeed  InputType */
{
    {42, 0,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* VDCE0 In ITU DATA7 */
    {42, 1,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {42, 2,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {42, 3,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {42, 4,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {42, 5,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {42, 6,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {42, 7,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* VDCE0 In ITU DATA0 */
    {42, 8,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_SCHMITT1}, /* VDCE0 In ITU CLK (Select SCHMITT1 to remove noise) */
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1} 
};
static const r_dev_PinConfig_t loc_Vi1ItuCapute_PinConfig[] = 
{
    {43, 2,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* VDCE1 In ITU DATA7 */
    {43, 3,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {43, 4,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {43, 5,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {43, 6,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {43, 7,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {43, 8,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {43, 9,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* VDCE1 In ITU DATA0 */
    {43, 10, 2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_SCHMITT1}, /* VDCE1 In ITU CLK (Select SCHMITT1 to remove noise) */
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1} 
};

/***********************************************************
  Section: Local Variables
*/

/***********************************************************

  Variable: loc_CtrlWindow

  The window hosting capture controls.
*/
static r_wm_Window_t loc_CtrlWindow;

/***********************************************************

  Variable: loc_CaptureWindow

  The window which displays the video input stream.
*/
static r_wm_Window_t loc_CaptureWindow;
r_wm_Window_t *p_CaptureWindow = &loc_CaptureWindow;

/***********************************************************

  Variable: loc_CaptureDevice

  The capture device object.
*/
static r_wm_Capture_t loc_CaptureDevice = {0};

/***********************************************************

  Variable: loc_Sprites

  The sprite objects for the capture control buttons.
*/
static r_wm_Sprite_t loc_Sprites[5] = {0};

/***********************************************************

  Variable: loc_SelState

  Selector left/right (0/1) state.
*/
static uint32_t loc_SelState;

/***********************************************************

  Variable: loc_CtrlState

  Control states for on/off (0) and pause/continue (1).
*/
static uint32_t loc_CtrlState[2];

typedef enum { KNOB_NO_EVENT, KNOB_MOVE_EVENT, KNOB_PRESS_EVENT } loc_KnobEvent_t;

static loc_KnobEvent_t loc_KnobEvent;

static uint32_t loc_WM_UNIT = 0;
static uint32_t loc_CAPT_UNIT = 0;

/***********************************************************
  Section: Local Functions
*/


/***********************************************************
 Function: loc_UpdateScreen

 Block until the screen is updated.
*/

void loc_UpdateScreen(void)
{
    R_WM_FrameEndMark(loc_WM_UNIT, 0);
    R_WM_FrameWait(loc_WM_UNIT, 0);
}

/***********************************************************
  Section: Local Functions
*/

void loc_CreateSprite   (   uint32_t    Ordinal,
                            uint32_t    PosX,
                            uint32_t    PosY,
                            uint32_t    PosZ,
                            uint32_t    Width,
                            uint32_t    Height,
                            void*       Data
                        )
{
    loc_Sprites[Ordinal].Window = &loc_CtrlWindow;
    loc_Sprites[Ordinal].PosX = PosX;
    loc_Sprites[Ordinal].PosY = PosY;
    loc_Sprites[Ordinal].PosZ = PosZ;
    loc_Sprites[Ordinal].Width = Width;
    loc_Sprites[Ordinal].Height = Height;
    loc_Sprites[Ordinal].Data = Data;
    R_WM_SpriteCreate(loc_WM_UNIT, &loc_Sprites[Ordinal]);
    R_WM_SpriteEnable(loc_WM_UNIT,  &loc_Sprites[Ordinal]);
}

void loc_UpdateCtrl(void)
{
    if (loc_SelState == 0)
    {
        if (loc_CtrlState[0] == 0)
        {
            /* window on state */
            R_WM_SpriteEnable(loc_WM_UNIT,  &loc_Sprites[LOC_OFF_SPRITE]);
            R_WM_WindowEnable(loc_WM_UNIT,  &loc_CaptureWindow);
        }
        else
        {
            /* window off state */
            R_WM_SpriteDisable(loc_WM_UNIT, &loc_Sprites[LOC_OFF_SPRITE]);
            R_WM_WindowDisable(loc_WM_UNIT, &loc_CaptureWindow);
        }
    }
    else
    {
        if (loc_CtrlState[1] == 0)
        {
            /* stream on state */
            R_WM_SpriteEnable(loc_WM_UNIT,  &loc_Sprites[LOC_PAUSE_SPRITE]);
            R_WM_CaptureEnable(loc_WM_UNIT, &loc_CaptureDevice);
        }
        else
        {
            /* stream paused state */
            R_WM_SpriteDisable(loc_WM_UNIT,  &loc_Sprites[LOC_PAUSE_SPRITE]);
            R_WM_CaptureDisable(loc_WM_UNIT, &loc_CaptureDevice);
        }
    }
}


void loc_UpdateSel(void)
{
    R_WM_SpriteMove (   loc_WM_UNIT,
                        &loc_Sprites[LOC_SEL_SPRITE],
                        (((LOC_CTRL_WINDOW_WIDTH / 2) - LOC_SEL_DIM) / 2) + (loc_SelState * (LOC_CTRL_WINDOW_WIDTH / 2)),
                        loc_Sprites[LOC_SEL_SPRITE].PosY,
                        loc_Sprites[LOC_SEL_SPRITE].PosZ
                    );
}

static void loc_KnobTurnLeft(void)
{
    loc_KnobEvent = KNOB_MOVE_EVENT;
}

static void loc_KnobPress(void)
{
    loc_KnobEvent = KNOB_PRESS_EVENT;
}

static void loc_KnobTurnRight(void)
{
    loc_KnobEvent = KNOB_MOVE_EVENT;
}

/*******************************************************************************
  Function: loc_App

  The render loop
*/
static void loc_App(void)
{
    void* addr_OnBtn;
    void* addr_OffBtn;
    void* addr_PauseBtn;
    void* addr_ResumeBtn;
    uint32_t scaled_width  = 0;
    uint32_t scaled_height = 0;
    
    uint32_t sprite_y = (LOC_CTRL_WINDOW_HEIGHT - LOC_BMP_HEIGHT) / 2;
    
    if (loc_ContensAddr != 0)
    {
        /* Copy contetns from CodeFlash to VRAM/SDRAM */
        addr_OnBtn     = (void*)(loc_ContensAddr + LOC_BMP_SIZE * 0);
        addr_OffBtn    = (void*)(loc_ContensAddr + LOC_BMP_SIZE * 1);
        addr_PauseBtn  = (void*)(loc_ContensAddr + LOC_BMP_SIZE * 2);
        addr_ResumeBtn = (void*)(loc_ContensAddr + LOC_BMP_SIZE * 3);
        
        memcpy(addr_OnBtn     , g_OnBtn,     LOC_BMP_SIZE);
        memcpy(addr_OffBtn    , g_OffBtn,    LOC_BMP_SIZE);
        memcpy(addr_PauseBtn  , g_PauseBtn,  LOC_BMP_SIZE);
        memcpy(addr_ResumeBtn , g_ResumeBtn, LOC_BMP_SIZE);
    }
    else
    {
        addr_OnBtn     = (void*)g_OnBtn;
        addr_OffBtn    = (void*)g_OffBtn;
        addr_PauseBtn  = (void*)g_PauseBtn;
        addr_ResumeBtn = (void*)g_ResumeBtn;
    }
    
    
    loc_InitWmUnit(loc_WM_UNIT, (const int8_t*)LOC_CAPTURE_DISPLAY_NAME);
    
    /* Use the other VIDEO channel for capturing. So it must be initialised. */
    if (loc_WM_UNIT != loc_CAPT_UNIT)
    {
        R_BSP_STDIO_Printf("VOUT%u: Second VOUT must be initialized in order to use the capture function. It is not used by the application.\n", loc_CAPT_UNIT);
        loc_InitWmUnit(loc_CAPT_UNIT, (const int8_t*)LOC_SECOND_DISPLAY_NAME);
        R_WM_FrameEndMark(loc_CAPT_UNIT, 0);
        R_WM_FrameWait(loc_CAPT_UNIT, 0);
    }
    R_WM_ScreenBgColorSet(loc_WM_UNIT, 255, 0, 0);
    
#if (LOC_CAPTURE_BUFFER_WIDTH < LOC_CAPTURE_WINDOW_WIDTH)
    scaled_width  = LOC_CAPTURE_BUFFER_WIDTH;
#endif
#if (LOC_CAPTURE_BUFFER_HEIGHT < LOC_CAPTURE_WINDOW_HEIGHT)
    scaled_height = LOC_CAPTURE_BUFFER_HEIGHT;
#endif
    
    loc_CreateWindow    (   loc_WM_UNIT,
                            R_WM_WINMODE_FB,
                            0,
                            LOC_CAPTURE_WINDOW_POSY,
                            1,
                            LOC_CAPTURE_WINDOW_WIDTH, 
                            LOC_CAPTURE_WINDOW_HEIGHT, 
                            LOC_CAPTURE_BUFFER_STRIDE,
                            scaled_width,
                            scaled_height,
                            R_WM_COLORFMT_RGB0888,
                            &loc_CaptureWindow
                        );
    loc_UpdateScreen();
    
    /* create the capture interface */
    loc_CaptureDevice.Mode          =   (r_wm_CapMode_t)(R_WM_CAPMODE_YUV_ITU656 
                                                      |  R_WM_CAPMODE_YUV_Y_UV_INVERT
                                                      |  R_WM_CAPMODE_DATA_CLK_INVERT
                                                      |  R_WM_CAPMODE_EAV);
#if (LOC_DEINTERLACE_MODE == LOC_DEINTERLACE_BOB_TOP)
    loc_CaptureDevice.Mode         |=   R_WM_CAPMODE_BOB_TOP;
#endif
#if (LOC_DEINTERLACE_MODE == LOC_DEINTERLACE_BOB_BOTTOM)
    loc_CaptureDevice.Mode         |=   R_WM_CAPMODE_BOB_BOTTOM;
#endif
#if (LOC_DEINTERLACE_MODE == LOC_DEINTERLACE_WEAVE)
    loc_CaptureDevice.Mode         |=   R_WM_CAPMODE_WEAVE;
#endif
#if (USE_VIN_NTSC == 0)
    loc_CaptureDevice.Mode         |=   R_WM_CAPMODE_PAL;
#endif
  
#if LOC_CAPTURE_SYNCHRONIZED == 1
    R_BSP_STDIO_Printf("\nUsing synchronous capturing with a single buffer (FPS_out == FPS_in).\n\n");
#else
    R_BSP_STDIO_Printf("\nUsing asynchronous capturing with triple buffering (FPS_out != FPS_in).\n\n");
    loc_CaptureDevice.Mode         |=   R_WM_CAPMODE_FIXED_VSYNC;
#endif
    loc_CaptureDevice.StartX        =   LOC_CAPTURE_INPUT_STARTX;
    loc_CaptureDevice.StrideX       =   LOC_CAPTURE_BUFFER_STRIDE;
    loc_CaptureDevice.StartY1       =   LOC_CAPTURE_INPUT_STARTY;
    loc_CaptureDevice.StartY2       =   0;
    loc_CaptureDevice.Width         =   LOC_CAPTURE_INPUT_WIDTH;
    loc_CaptureDevice.Height        =   LOC_CAPTURE_INPUT_HEIGHT;
    loc_CaptureDevice.Window        =   &loc_CaptureWindow;
    loc_CaptureDevice.Delay         =   LOC_CAPTURE_INPUT_DELAY;
    loc_CaptureDevice.CapUnit       =   loc_CAPT_UNIT;
    
#if (LOC_CAPTURE_INPUT_WIDTH > LOC_CAPTURE_BUFFER_WIDTH)
    loc_CaptureDevice.ScaledWidth   =   LOC_CAPTURE_BUFFER_WIDTH;
#else
    loc_CaptureDevice.ScaledWidth   =   0;
#endif
#if (LOC_DEINTERLACE_MODE == LOC_DEINTERLACE_WEAVE)
  #if (LOC_CAPTURE_INPUT_HEIGHT > (LOC_CAPTURE_BUFFER_HEIGHT / 2))
    loc_CaptureDevice.ScaledHeight  =   LOC_CAPTURE_BUFFER_HEIGHT / 2;  /* top + bottom */
  #else
    loc_CaptureDevice.ScaledHeight  =   0;
  #endif
#else
  #if (LOC_CAPTURE_INPUT_HEIGHT > LOC_CAPTURE_BUFFER_HEIGHT)
    loc_CaptureDevice.ScaledHeight  =   LOC_CAPTURE_BUFFER_HEIGHT;
  #else
    loc_CaptureDevice.ScaledHeight  =   0;
  #endif
#endif
    R_WM_CaptureCreate(loc_WM_UNIT, &loc_CaptureDevice);
#if (USE_VIN_NTSC == 0)
    R_WM_CaptureExtVsyncSet(loc_WM_UNIT, &loc_CaptureDevice, 0, 19500, 20500);
#else
    R_WM_CaptureExtVsyncSet(loc_WM_UNIT, &loc_CaptureDevice, 0, 16500, 16700);
#endif
    R_WM_CaptureEnable(loc_WM_UNIT, &loc_CaptureDevice);

    loc_UpdateScreen();
    
    /* create the UI buttons */ 
    loc_CreateWindow    (   loc_WM_UNIT,
                            R_WM_WINMODE_SPRITES,
                            0,
                            0,
                            2,
                            LOC_CTRL_WINDOW_WIDTH,
                            LOC_CTRL_WINDOW_HEIGHT,
                            LOC_CTRL_WINDOW_STRIDE,
                            0,
                            0,
                            R_WM_COLORFMT_ARGB8888,
                            &loc_CtrlWindow
                        );
    loc_UpdateScreen();
    
    loc_CreateSprite    (   LOC_ON_SPRITE,
                            ((LOC_CTRL_WINDOW_WIDTH / 2) - LOC_BMP_WIDTH) / 2,
                            sprite_y,
                            LOC_ON_SPRITE,
                            LOC_BMP_WIDTH,
                            LOC_BMP_HEIGHT,
                            addr_OnBtn
                        );
    loc_CreateSprite    (   LOC_OFF_SPRITE,
                            ((LOC_CTRL_WINDOW_WIDTH / 2) - LOC_BMP_WIDTH) / 2,
                            sprite_y,
                            LOC_OFF_SPRITE,
                            LOC_BMP_WIDTH,
                            LOC_BMP_HEIGHT,
                            addr_OffBtn
                        );
    loc_CreateSprite    (   LOC_RESUME_SPRITE,
                            (LOC_CTRL_WINDOW_WIDTH / 2) + loc_Sprites[0].PosX,
                            sprite_y,
                            LOC_RESUME_SPRITE,
                            LOC_BMP_WIDTH,
                            LOC_BMP_HEIGHT,
                            addr_ResumeBtn
    );
    loc_CreateSprite    (   LOC_PAUSE_SPRITE,
                            (LOC_CTRL_WINDOW_WIDTH / 2) + loc_Sprites[0].PosX,
                            sprite_y,
                            LOC_PAUSE_SPRITE,
                            LOC_BMP_WIDTH,
                            LOC_BMP_HEIGHT,
                            addr_PauseBtn
    );
    loc_CreateSprite    (   LOC_SEL_SPRITE,
                            ((LOC_CTRL_WINDOW_WIDTH / 2) - LOC_SEL_DIM) / 2,
                            sprite_y + 40,
                            LOC_SEL_SPRITE,
                            LOC_SEL_DIM,
                            LOC_SEL_DIM,
                            (void*)g_Sel
    );
              
    loc_UpdateScreen();

    R_WM_DevEventRegister(loc_WM_UNIT, R_WM_EVENT_VBLANK, 0);
    R_WM_DevEventRegister(loc_WM_UNIT, R_WM_EVENT_VI_VBLANK, 0);
    
    for (;;)
    {
        if (loc_KnobEvent != KNOB_NO_EVENT)
        {
            if (loc_KnobEvent == KNOB_MOVE_EVENT)
            {
                /* toggle the selector */
                loc_SelState ^= 1;
                loc_UpdateSel();
            }
            else
            {
                /* toggle the corresponding ctrl */
                loc_CtrlState[loc_SelState] ^= 1;
                loc_UpdateCtrl();
            }
            loc_KnobEvent = KNOB_NO_EVENT;
            loc_UpdateScreen();
        }
    }
}

/*******************************************************************************
  Function: loc_Error

  This function is called when error occurs
  The application hangs in there.
*/
void loc_Error(int32_t Err)
{
    while (Err)
    {
    }
}

/*******************************************************************************
  Function: main

  entry after startup
*/

int32_t main(void)
{
    R_BSP_Init();
    
    R_BSP_SetButtonCallback(BSP_BTN_LEFT_TURN, loc_KnobTurnLeft);
    R_BSP_SetButtonCallback(BSP_BTN_RIGHT_TURN, loc_KnobTurnRight);
    R_BSP_SetButtonCallback(BSP_BTN_OK_PRESS, loc_KnobPress);
    loc_SetupMemManager(&loc_WM_UNIT, &loc_CAPT_UNIT);
    
    if (0 == loc_CAPT_UNIT)
    {
        R_DEV_PinInit(loc_Vi0ItuCapute_PinConfig);
    }
    else
    {
        R_DEV_PinInit(loc_Vi1ItuCapute_PinConfig);
    }
    
    if ( (0 == loc_WM_UNIT) && (1 == loc_CAPT_UNIT) )
    {
        R_BSP_STDIO_Printf("Combining VI1 (ITU) with VO0 (HDMI). Default HDMI-DE not available.\nPlease connect P45_1 (U35.Pin20) to JP98-2.\n\n");
    }
    
    loc_App();
    return(0);
}
