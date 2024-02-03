/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: app_clock.c 11057 2016-11-10 08:23:04Z shinya.tomari $
============================================================================ 
DESCRIPTION
Sample program for demonstration of the D1MX GFX features
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2014
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

/***********************************************************
  Section: Includes
*/

#include <math.h>        /* sin and cos */
#include "r_typedefs.h"     /* NEC basic types, e.g. uint32_t */
#include "fw_hmial_api.h"
#include "img_format.h"
#include "application.h"
#include "fw_osal_api.h"
#include "fw_vovial_api.h"

#include "davehd_driver.h"
#include "r_drw2d_os.h"
#include "r_drw2d_api.h"
#include "r_config_drw2d.h"
#include "r_drw2d_ctx_dhd.h"

#include "img_format.h"
#include "img.h"
#include "img_drw2d.h"
#include "main.h"
#include "r_config_d1x.h"
#include "app_clock.h"
#include "app_clock_config.h"
#include "app_clock_image.h"
#include "error.h"

extern const Img_t Img_clocktrain;

/***********************************************************
  Section: Local Enumeration
*/

/***********************************************************
  Section: Local Type Definitions
*/

/***********************************************************
  Typedef: Needle_t
  
  This structure lists the properties of a needle.
  
  Members:
  ScaleSideA        - Maximum angle for scale drawing [0; 360]
  ScaleSideB        - Minimum angle for scale drawing [0; 360]
  AngleSideA        - Maximum angle for needle rotation [0; 360]
  AngleSideB        - Minimum angle for needle rotation [0; 360]
  Angle             - Current needle angle
  WidthTop          - Needle top width in pixels
  WidthBase         - Needle base width in pixels
  Style             - Drawing style (see <DrawingStyle_t>)
  Drive             - Needle animation (see <RotationDrive_t>)
  Direction         - Needle rotation direction (see <RotationDirection_t>)
  Length            - Needle length in pixels
  Speed             - Rotation angle added at each frame in degrees
  CenterCoordX      - Horizontal position of the needle base
  CenterCoordY      - Vertical position of the needle base
  CenterRadius      - Width of the needle base drawing
  ScaleRadius       - Radius of the scale drawing
  ScaleWidth        - Width of the scale drawing
  MainColor         - Needle color
*/

typedef struct {
    float64_t                   ScaleSideA;
    float64_t                   ScaleSideB;
    float64_t                   AngleSideA;
    float64_t                   AngleSideB;
    float64_t                   Angle;
    r_drw2d_FixedP_t         WidthTop;
    r_drw2d_FixedP_t         WidthBase;
    uint32_t                 Length;
    uint32_t                 Speed;
    r_drw2d_Point_t          CenterCoord;
    r_drw2d_FixedP_t         CenterRadius;
    r_drw2d_FixedP_t         ScaleRadius;
    r_drw2d_FixedP_t         ScaleWidth;
    r_drw2d_Color_t          MainColor;
} Needle_t;

/***********************************************************
  Section: Local Defines
*/

/***********************************************************
  Define: LOC_APP_CLOCK_THREAD_STACKSIZE

  Size in bytes of the stack used by the application thread
*/

#define LOC_APP_CLOCK_THREAD_STACKSIZE     0x1400


/***********************************************************
  Define: LOC_APP_CLOCK_THREAD_PRIO

  Priority level of the application thread

*/
#define LOC_APP_CLOCK_THREAD_PRIO           0x22


/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Variable: locSurface

  The render surface.
*/

static void       *locSurface;

/***********************************************************
  Variable: locOldData

  Store for old image data pointers.
*/

static void *locOldData[1];

/***********************************************************
  Variable: locQuit

  Signal flag to leave the application.
*/

static volatile uint8_t locQuit;

/***********************************************************
  Variable: loc_DHDDev

  DHD Drawing handle
*/
static struct r_drw2d_DeviceDHD_s locDHDDev;


/***********************************************************

  Variable: loc_Drw2dDev

  Pointer to the Drw2D device handle
*/

static r_drw2d_Device_t locDrw2dDev;


/***********************************************************
  Variable: locRenderThreadId

  Id of the menu thread
*/

static uint32_t locRenderThreadId;


/***********************************************************
  Variable: locStackRenderThread

  d3D Thread local stack

*/
static uint32_t locStackRenderThread[LOC_APP_CLOCK_THREAD_STACKSIZE >> 2];


/***********************************************************
  Variable: locRetVal

  Thread return value;
*/

static void *locRetVal;


/***********************************************************
  Variables: _Outline{i}_{j}
  
  Local contexts to switch between the default one and the one for shadows.

  {i} is either default or shadow
  {j} is either rc or c
*/

static r_drw2d_RenderContext_t locOutlineDefault_rc;
static r_drw2d_RenderContext_t locOutlineShadow_rc;
static r_drw2d_Context_t locOutlineDefault_c;
static r_drw2d_Context_t locOutlineShadow_c;


/***********************************************************
  Variables: loc{i}LineStyle

  Line styles used for drawing the needles and their shadows.

  {i} is either Needle or Shadow
*/
static r_drw2d_LineStyle_t locNeedleLineStyle;
static r_drw2d_LineStyle_t locShadowLineStyle;


/***********************************************************
  Variable: locNeedle
  
  Array of needles built in this demonstration (see <Needle_t>).
*/

static Needle_t locNeedle[_NEEDLE_NBR];


/***********************************************************
  Variables: locNeedle{Sin/Cos}Temp
  
  Sinus and Cosinus values for angle calculation.
  Variables used for needle and scale drawing.
  Mathematic functions called are sinf and cosf.
*/

static float64_t locNeedleSinTemp;
static float64_t locNeedleCosTemp;


/***********************************************************
  Variables: locNeedle{i}Coord{j}_s
  
  Space coordinates for needle drawing.
  Shifting applied is to match d2_point type.

  {i} is the needle part (Top, Base)
  {j} is the coordinate axis (X->Horizontal, Y->Vertical)
*/

static r_drw2d_Point_t locNeedleCoords_s[2];


/***********************************************************
  Variables: locNeedleWidth{i}_s
  
  Width values for needle drawing.
  Shifting applied is to match d2_width type.

  {i} is the needle part (Top, Base)
*/

static r_drw2d_FixedP_t locNeedleWidthTop_s;


/***********************************************************
  Variables: locNeedleCenterCoord{i}_s
  
  Space coordinates for needle center/base drawing.
  Shifting applied is to match d2_point type.

  {i} is the coordinate axis (X->Horizontal, Y->Vertical)
*/

static r_drw2d_Point_t locNeedleCenterCoord_s;


/***********************************************************
  Variable: locNeedleCenterRadius_s
  
  Radius value for needle center/base drawing.
  Shifting applied is to match d2_width type.
*/

static r_drw2d_FixedP_t locNeedleCenterRadius_s;


/***********************************************************

  Variable: locSema

  Semaphore to stop the render thread from running.
*/

static fw_osal_Sema_t locSema;


/***********************************************************

  Variable: locSemaArm

  Variable to arm locSema semaphore (arm == 1, disarm == 0).
*/

static int32_t locSemaArm;




/***********************************************************
  Section: Local Functions
*/

static void locUpdateClock(int32_t Delta_ms);


/***********************************************************
  Function: locKnobPress
*/

static void locKnobPress(void)
{
    Cmd_t cmd;

    cmd.Cmd = CMD_STOPSTART;
    cmd.Par1 = 0;
    cmd.Par2 = (AppNum - 1);
    CmdSend(&cmd);
}


/***********************************************************
  Function: locKnobLeft
*/

static void locKnobLeft(void)
{
    locUpdateClock(-5*60*1000);
}


/***********************************************************
  Function: locKnobRight
*/

static void locKnobRight(void)
{
    locUpdateClock(5*60*1000);
}


/***********************************************************
  Function: locNeedleCalc

  Draw one needle in Vector style.

  Parameters:
  NeedleNo   - Needle number
  XOff       - Needle x offset
  YOff       - Needle y offset

  Returns:
  void
*/
static void locNeedleCalc(uint32_t NeedleNo, uint32_t XOff, uint32_t YOff)
{
    /***********************/
    /* Calculation Section */
    /***********************/

    /* calculate Sinus & Cosinus */
    locNeedleSinTemp = sinf(_DEGREE(locNeedle[NeedleNo].Angle));
    locNeedleCosTemp = cosf(_DEGREE(locNeedle[NeedleNo].Angle));

    /* calulate needle center coodinates */
    locNeedleCenterCoord_s.X = R_DRW2D_2X(locNeedle[NeedleNo].CenterCoord.X + XOff);
    locNeedleCenterCoord_s.Y = R_DRW2D_2X(locNeedle[NeedleNo].CenterCoord.Y + YOff);

    /* calulate needle coodinates */
    locNeedleCoords_s[0].X  = R_DRW2D_2X(locNeedle[NeedleNo].CenterCoord.X +
                          (locNeedleCosTemp * locNeedle[NeedleNo].Length) +
                          XOff);

    locNeedleCoords_s[0].Y  = R_DRW2D_2X(locNeedle[NeedleNo].CenterCoord.Y -
                          (locNeedleSinTemp * locNeedle[NeedleNo].Length) +
                          YOff);

    locNeedleCoords_s[1].X = R_DRW2D_2X(locNeedle[NeedleNo].CenterCoord.X -
                         (locNeedleCosTemp * (locNeedle[NeedleNo].Length/_TAIL_REDUC)) +
                         XOff);

    locNeedleCoords_s[1].Y = R_DRW2D_2X(locNeedle[NeedleNo].CenterCoord.Y +
                         (locNeedleSinTemp * (locNeedle[NeedleNo].Length/_TAIL_REDUC)) +
                         YOff);

    locNeedleWidthTop_s   = R_DRW2D_2X(locNeedle[NeedleNo].WidthTop);

    /* calulate needle center coodinates */
    locNeedleCenterRadius_s = R_DRW2D_2X(locNeedle[NeedleNo].CenterRadius);
}


/***********************************************************
  Function: locNeedleVectorDraw

  Draw one needle in Vector style.

  Parameters:
  Dev        - R_DRW2D device structure
  NeedleNo   - Needle number

  Returns:
  void
*/
static void locNeedleVectorDraw(r_drw2d_Device_t  Dev, uint32_t NeedleNo)
{
    /*******************/
    /* Drawing Section */
    /*******************/

    /* drawing needle */
    R_DRW2D_DrawLines(Dev, locNeedleCoords_s, 2);

    /* drawing needle center */
    R_DRW2D_DrawEllipse(Dev, locNeedleCenterCoord_s, locNeedleCenterRadius_s, locNeedleCenterRadius_s);
}


/***********************************************************
  Function: locDrawNeedles

  Draw all needles in Vector style.

  Parameters:
  Dev        - R_DRW2D device structure

  Returns:
  void
*/
static void locDrawNeedles(r_drw2d_Device_t  Dev)
{
    uint32_t i;

    R_DRW2D_ContextSelect(locDrw2dDev, locOutlineShadow_c);
    for (i = 0; i < _NEEDLE_NBR; i++)
    {
       locNeedleCalc(i, 4, 2);

       locNeedleLineStyle.Width = locNeedleWidthTop_s + R_DRW2D_2X(1);
       R_DRW2D_CtxLineStyle(locDrw2dDev, &locShadowLineStyle);
       locNeedleVectorDraw(locDrw2dDev, i);
    }

    R_DRW2D_ContextSelect(locDrw2dDev, locOutlineDefault_c);
    for (i = 0;i < _NEEDLE_NBR; i++)
    {
       locNeedleCalc(i, 0, 0);

       locNeedleLineStyle.Width = locNeedleWidthTop_s;
       R_DRW2D_CtxLineStyle(locDrw2dDev, &locNeedleLineStyle);
       R_DRW2D_CtxFgColor(Dev, 0xFF000000 | locNeedle[i].MainColor);
       locNeedleVectorDraw(locDrw2dDev, i);
    }

    /* drawing clock center */
    R_DRW2D_CtxFgColor(Dev, 0xFF000000);
    R_DRW2D_DrawEllipse(Dev, locNeedleCenterCoord_s, R_DRW2D_2X(2), R_DRW2D_2X(2));

    R_DRW2D_ContextSelect(locDrw2dDev, 0);
}


/*******************************************************************************
  Function: locSetRenderTarget

  Sets the target for the subsequent render commands to a current draw buffer
  of a window (its back-buffer)
*/
static void* locSetRenderTarget(void* Win)
{
    r_drw2d_Framebuffer_t fb;
    fw_vovial_FbFormat_t fmt;

    /* Init framebuffer struct */
    fb.Flags = (r_drw2d_FramebufferFlags_t)0;
    fb.Handle = 0; /* not used currently */
    FW_VOVIAL_GetBufferInfo(Win, &fmt, &fb.Buffer.Pitch, &fb.Buffer.Size.Width, &fb.Buffer.Size.Height);
    fb.Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
    fb.Buffer.Data = FW_VOVIAL_GetFrameBuffer(Win);
    R_DRW2D_FramebufferSet(locDrw2dDev, &fb);

    return fb.Buffer.Data;
}


/***********************************************************
  Function: locVBlank

  VBlank callback function, called when a VBlank interrupt
  occurs.
*/
static void locVBlank(void)
{
    if(locSemaArm == 1)
    {
        FW_OSAL_SemaSignal(&locSema);
    }
}


/***********************************************************
  Function: locRenderThread
*/

static void *locRenderThread(void *Arg)
{
    uint32_t i=0;
    uint32_t time = FW_OSAL_TimeGet();

    // Clear surface before enabling it
    locSetRenderTarget(locSurface);
    R_DRW2D_CtxBgColor(locDrw2dDev, 0xff000000);
    R_DRW2D_FramebufferClear(locDrw2dDev);
    R_DRW2D_GpuFinish(locDrw2dDev, R_DRW2D_FINISH_WAIT);

    FW_VOVIAL_SwapFrameBuffer(locSurface, i);
    ++i;

    /* Make sure that buffer has been already swapped */
    locSemaArm = 1;
    FW_OSAL_SemaWait(&locSema);

    FW_VOVIAL_EnableSurface(locSurface);

    while (!locQuit)
    {
        locSetRenderTarget(locSurface);

        locUpdateClock(FW_OSAL_TimeGet()-time);
        time = FW_OSAL_TimeGet();

        R_DRW2D_CtxBlendMode(locDrw2dDev, R_DRW2D_BLENDMODE_SRC);
        IMG_Blit(locDrw2dDev, &Img_clocktrain, 0, 0);

        locDrawNeedles(locDrw2dDev);

        R_DRW2D_GpuFinish(locDrw2dDev, R_DRW2D_FINISH_WAIT);
        FW_VOVIAL_SwapFrameBuffer(locSurface, i);

        locSemaArm = 1;
        FW_OSAL_SemaWait(&locSema);

        ++i;
    }
    FW_OSAL_ThreadExit((void*)42);

    return 0;
}


/***********************************************************
  Function: locUpdateClock

  Parameters:
  Delta_ms          - Value added to current clock time in milliseconds
*/

static void locUpdateClock(int32_t Delta_ms)
{
    static uint32_t time = 45*60*1000;
    uint32_t i;
    uint32_t sec;
    uint32_t min;
    uint32_t h;

    if((Delta_ms < 0) && (time < (Delta_ms*(-1))))
    {
        time += 12*60*60*1000;
    }
    time += Delta_ms;
    sec  = (time/1000)%60;
    min  = ((time/1000)/60)%60;
    h    = ((time/1000)/60/60)%12;

    locNeedle[0].Angle = (360.0/60.0/60.0/12.0)*((h*60+min)*60+sec);    /* Turn Hours */
    locNeedle[1].Angle = (360.0/60.0/60.0)*(min*60+sec);	/* Turn Minutes */
    locNeedle[2].Angle = (360.0/60.0)*sec;  /* Turn Seconds */

    /* Move 12 o'clock to the upper middle */
    for(i=0; i < _NEEDLE_NBR; ++i)
    {
        if(locNeedle[i].Angle >= 90.0)
        {
            locNeedle[i].Angle = 450.0 - locNeedle[i].Angle;
        }
        else
        {
            locNeedle[i].Angle = 90 - locNeedle[i].Angle;
        }
    }
}


/***********************************************************
  Function: locInitDRW2DContext

  Initialize the render contexts for the clock.
*/

static void locInitDRW2DContext()
{
    r_drw2d_Error_t err;

    /* Initialize basic drawing configurations */
    err = R_DRW2D_ContextInit(locDrw2dDev, &locOutlineDefault_rc, &locOutlineDefault_c);
    err |= R_DRW2D_ContextSelect(locDrw2dDev, locOutlineDefault_c);

    locNeedleLineStyle.IsClosed = 0;
    locNeedleLineStyle.LineCap = R_DRW2D_LINECAP_BUTT;
    locNeedleLineStyle.LineJoin = R_DRW2D_LINEJOIN_NONE;
    locNeedleLineStyle.MiterLimit = 1;
    locNeedleLineStyle.Width = R_DRW2D_2X(1);

    err |= R_DRW2D_CtxLineStyle(locDrw2dDev, &locNeedleLineStyle);
    err |= R_DRW2D_CtxFgColor(locDrw2dDev, COLOR_WHITE);
    err |= R_DRW2D_CtxBgColor(locDrw2dDev, COLOR_BLUE);
    err |= R_DRW2D_CtxFillMode(locDrw2dDev, R_DRW2D_FILLMODE_SOLID);
    err |= R_DRW2D_CtxBlendMode(locDrw2dDev, R_DRW2D_BLENDMODE_SRC);

    if(err != R_DRW2D_ERR_OK)
    {
        Error(ERR_DRW2D);
    }

    /* Initialize the outline context for shadows*/
    err = R_DRW2D_ContextInit(locDrw2dDev, &locOutlineShadow_rc, &locOutlineShadow_c);
    err |= R_DRW2D_ContextSelect(locDrw2dDev, locOutlineShadow_c);

    locShadowLineStyle.IsClosed = 0;
    locShadowLineStyle.LineCap = R_DRW2D_LINECAP_BUTT;
    locShadowLineStyle.LineJoin = R_DRW2D_LINEJOIN_NONE;
    locShadowLineStyle.MiterLimit = 1;
    locShadowLineStyle.Width = R_DRW2D_2X(3);

    err |= R_DRW2D_CtxLineStyle(locDrw2dDev, &locShadowLineStyle);
    err |= R_DRW2D_CtxFgColor(locDrw2dDev, 0x33000000 | COLOR_BLACK);
    err |= R_DRW2D_CtxBgColor(locDrw2dDev, COLOR_BLUE);
    err |= R_DRW2D_CtxFillMode(locDrw2dDev, R_DRW2D_FILLMODE_SOLID);
    err |= R_DRW2D_CtxBlendMode(locDrw2dDev, R_DRW2D_BLENDMODE_SRC_OVER);

    if(err != R_DRW2D_ERR_OK)
    {
        Error(ERR_DRW2D);
    }

    /* Restore the default context */
    err = R_DRW2D_ContextSelect(locDrw2dDev, 0);

    if(err != R_DRW2D_ERR_OK)
    {
        Error(ERR_DRW2D);
    }
}

/***********************************************************
  Function: locInit
*/

static void locInit(void)
{
    r_drw2d_Error_t err;

    locQuit    = 0;
    locSemaArm = 0;

    locSurface = FW_VOVIAL_CreateSurface(0, FW_VOVIAL_ARGB8888, 0, 0, 0, LOC_DISPLAY_STRIDE, LOC_DISPLAY_WIDTH, LOC_DISPLAY_HEIGHT, 0xff, 2, 0);
    if(locSurface == 0)
    {
        Error(ERR_VOVIAL);
    }

    err = R_DRW2D_Init();
    err |= R_DRW2D_Open(0, R_DRW2D_UNIT_DHD0, &locDHDDev, &locDrw2dDev);
    if(err != R_DRW2D_ERR_OK)
    {
        Error(ERR_DRW2D);
    }

    locInitDRW2DContext();

    /* Setup background texture */
    locOldData[0] = Init2DImg((Img_t*)&Img_clocktrain);

    /* Initialize needle information */
    _NEEDLE_INIT(0);
    _NEEDLE_INIT(1);
    _NEEDLE_INIT(2);

    FW_OSAL_SemaCreate(&locSema);

    FW_VOVIAL_RegisterVBlankIsr(locSurface, &locVBlank);

    locRenderThreadId = FW_OSAL_ThreadCreate(locRenderThread, 0,
                                                   locStackRenderThread,
                                                   LOC_APP_CLOCK_THREAD_STACKSIZE,
                                                   LOC_APP_CLOCK_THREAD_PRIO);
}


/***********************************************************
  Function: locDeInit
*/

static void locDeInit(void)
{
    /* Signal thread to quit */
    locQuit = 1;

    FW_OSAL_ThreadJoin(locRenderThreadId, &locRetVal);

    Deinit2DImg((Img_t*)&Img_clocktrain, locOldData[0]);

    R_DRW2D_Close(locDrw2dDev);
    R_DRW2D_Exit();

    FW_VOVIAL_DeleteSurface(locSurface);

    FW_OSAL_SemaDestroy(&locSema);
}



/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Variable: locAppClockControl

  Control function structure.

  List of callback functions for each control element.
*/

static const fw_hmial_Control_t locAppClockControl = {
    locKnobPress,         /* KNOB press callback */
    0,                    /* KNOB release callback  */
    locKnobRight,         /* KNOB right callback */
    locKnobLeft,          /* KNOB left callback  */
    0,                    /* BUTTON up press callback */
    0,                    /* BUTTON up release callback */
    0,                    /* BUTTON down press callback  */
    0,                    /* BUTTON down release callback */
    0,                    /* BUTTON middle press callback */
    0,                    /* BUTTON middle release callback */
    0,                    /* BUTTON left press callback */
    0,                    /* BUTTON left release callback */
    0,                    /* BUTTON right press callback */
    0                     /* BUTTON right release callback */
};

/***********************************************************
  Section: Global Variables

  Description see: <app_clock.h>
*/

const App_t AppClock = {
    &Img_icon_clock,
    "Clock",
    "Demonstrates an analog Clock",
    &locAppClockControl,
    0,
    locInit,
    locDeInit
};

