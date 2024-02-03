/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: app_tutorial.c 11057 2016-11-10 08:23:04Z shinya.tomari $
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

#include "r_typedefs.h"
#include "r_config_d1x.h"

#include "fw_hmial_api.h"
#include "fw_osal_api.h"
#include "fw_vovial_api.h"

#ifdef R_DRW2D_SYS_DHD
#include "davehd_driver.h"
#include "r_cdi_api.h"
#include "r_util_dhd.h"
#include "r_drw2d_os.h"
#include "r_drw2d_api.h"
#include "r_config_drw2d.h"
#include "r_drw2d_ctx_dhd.h"
#else
#include "r_drw2d_os.h"
#include "r_drw2d_api.h"
#include "r_config_drw2d.h"
#include "r_drw2d_ctx_cpu.h"
#endif

#include "img_format.h"
#include "img.h"
#include "img_drw2d.h"

#include "application.h"
#include "app_tutorial_image.h"
#include "main.h"
#include "error.h"


/***********************************************************
  Section: Local defines
*/

#define LOC_BG_COLOR        0xff000000

#define LOC_TEXT_POSX      45
#define LOC_TEXT_POSY      90


/***********************************************************
  Define: LOC_APP_TUTORIAL_THREAD_STACKSIZE

  Size in bytes of the stack used by the application thread
*/

#define LOC_APP_TUTORIAL_THREAD_STACKSIZE     0x1400


/***********************************************************
  Define: LOC_APP_TUTORIAL_THREAD_PRIO

  Priority level of the application thread

*/
#define LOC_APP_TUTORIAL_THREAD_PRIO           0x22


/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Variable: locSurface

  The application's drawing surface 1.
*/
static void *locSurface;

/***********************************************************
  Variable: locOldText

  Store for old image data pointer.
*/

static void *locOldText;

/***********************************************************
  Variable: locStackAppTutorialThread

  d3D Thread local stack

*/
static uint32_t locStackAppTutorialThread[LOC_APP_TUTORIAL_THREAD_STACKSIZE >> 2];


/***********************************************************
  Variable: locAppTutorialThreadId

  Id of the test thread
*/
static uint32_t locAppTutorialThreadId;


/***********************************************************
  Variable: locQuit

  Signal flag to leave the application.
*/
static volatile uint8_t locQuit;

/***********************************************************
  Variable: locImplDev

  DHD Drawing handle
*/
#ifdef R_DRW2D_SYS_DHD
static struct r_drw2d_DeviceDHD_s locImplDev;
#else
static struct r_drw2d_DeviceCPU_s locImplDev;
#endif

/***********************************************************

  Variable: loc_Drw2dDev

  Pointer to the Drw2D device handle
*/
static r_drw2d_Device_t locDrw2dDev;


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
  Variable: locTexture

  DRW2D texture structure for the text texture.
*/
static r_drw2d_Texture_t locTexture;


/***********************************************************
  Variable: loc_Color

  Initial color of the lower text.
*/
static r_drw2d_Color_t loc_Color = 0x000033ff;


/***********************************************************
  Variable: locRndSeed

  seed for the locRndColor function
*/
static uint32_t locRndSeed = 78163;


/***********************************************************
  Variable: locRetVal

  Thread return value;
*/
static void *locRetVal;


/***********************************************************
  Section: Local Functions
*/

/************************************************************
  Function: locRndColor
  
  Returns a pseudo random color for the forms.
*/
static uint32_t locRndColor(void) {
    uint32_t x;
    
    x = ((locRndSeed>>16) + 3715436908ul ) ^ 0x1fd8dae7;
    locRndSeed += x;
    
    return locRndSeed;
}


/***********************************************************
  Function: locKnobPress

  KNOB pressed callback function.

  The application returns to the menu.

  Parameters:
  void

  Returns:
  void
*/
static void locKnobPress(void) {
    Cmd_t cmd;

    cmd.Cmd  = CMD_STOPSTART;
    cmd.Par1 = 0;
    cmd.Par2 = (AppNum - 1);
    CmdSend(&cmd);
}


/***********************************************************
  Function: locKnobRotation

  KNOB right/left callback function.

  The color of forms changed.

  Parameters:
  void

  Returns:
  void
*/
static void locKnobRotation(void)
{
    loc_Color = (r_drw2d_Color_t) locRndColor();
}


/*******************************************************************************
  Function: locSetRenderTarget

  Sets the target for the subsequent render commands to a current draw buffer
  of a window (its back-buffer).

  Parameters:
  Win - VOVIAL surface (window).

  Returns:
  A pointer to the buffer.
*/
static void* locSetRenderTarget(void* Win)
{
    r_drw2d_Framebuffer_t fb;
    fw_vovial_FbFormat_t fmt;

    /* Init framebuffer struct */
    fb.Flags = (r_drw2d_FramebufferFlags_t) 0;
    fb.Handle = 0; /* not used currently */
    FW_VOVIAL_GetBufferInfo(Win, &fmt, &fb.Buffer.Pitch, &fb.Buffer.Size.Width, &fb.Buffer.Size.Height);
    fb.Buffer.PixelFormat = LOC_DRW2D_FRAMEBUFFER_FORMAT;
    fb.Buffer.Data = FW_VOVIAL_GetFrameBuffer(Win);
    R_DRW2D_FramebufferSet(locDrw2dDev, &fb);

    return fb.Buffer.Data;
}


/***********************************************************
  Function: locRender

  Tutorial application render function.

  Returns:
  void
*/
static void locRender()
{
    r_drw2d_EffectStage_t effect;
    r_drw2d_Rect_t srcRect, dstRect;

    /* Clear entire screen */
    R_DRW2D_FramebufferClear(locDrw2dDev);

    /* Setup a modulate effect for the texture's color channels */
    effect.Name = R_DRW2D_EFFECT_MODULATE;
    effect.Args[0].Source = R_DRW2D_EFFECT_SOURCE_TEXTURE_UNIT;
    effect.Args[0].Param.Color.Source.TextureUnit = 0;
    effect.Args[0].Param.Color.Operand = R_DRW2D_EFFECT_COLOR_OPERAND_RGBA;
    effect.Args[1].Source = R_DRW2D_EFFECT_SOURCE_CONSTANT_COLOR;
    effect.Args[1].Param.Color.Source.ConstantColor = 0xff0033ff;
    effect.Args[1].Param.Color.Operand = R_DRW2D_EFFECT_COLOR_OPERAND_RGBA;

    /* Setup the source rectangle for the whole texture */
    srcRect.Pos.X = R_DRW2D_2X(0);
    srcRect.Pos.Y = R_DRW2D_2X(0);
    srcRect.Size.Width = R_DRW2D_2X(Img_tutorial_text.Width);
    srcRect.Size.Height = R_DRW2D_2X(Img_tutorial_text.Height);

    /* Setup the destination rectangle */
    dstRect.Pos.X = R_DRW2D_2X(LOC_TEXT_POSX);
    dstRect.Pos.Y = R_DRW2D_2X(LOC_TEXT_POSY);
    dstRect.Size.Width = R_DRW2D_2X(Img_tutorial_text.Width);
    dstRect.Size.Height = R_DRW2D_2X(Img_tutorial_text.Height);

    /* Turn of transformation */
    R_DRW2D_CtxTransformMode(locDrw2dDev, R_DRW2D_TRANSFORM_NONE);

    /* Enable effect stage */
    R_DRW2D_CtxEffectsSet(locDrw2dDev, &effect, 1);

    /* Render the upper left texture */
    R_DRW2D_TextureBlit(locDrw2dDev, &srcRect, &dstRect);

    effect.Args[1].Param.Color.Source.ConstantColor = loc_Color;
    dstRect.Pos.X = R_DRW2D_2X(  LOC_DISPLAY_WIDTH
                               - LOC_TEXT_POSX
                               - Img_tutorial_text.Width);
    dstRect.Pos.Y = R_DRW2D_2X(  LOC_DISPLAY_HEIGHT
                               - LOC_TEXT_POSY
                               - Img_tutorial_text.Height);

    /* Update the effect stage */
    R_DRW2D_CtxEffectsUpdate(locDrw2dDev, R_DRW2D_EFFECT_MODULATE, 0, 2, effect.Args);

    /* Render the lower right texture*/
    R_DRW2D_TextureBlit(locDrw2dDev, &srcRect, &dstRect);

    /* Disable the effect stage */
    R_DRW2D_CtxEffectsDelete(locDrw2dDev);

    /* Turn on transformations */
    R_DRW2D_CtxTransformMode(locDrw2dDev, R_DRW2D_TRANSFORM_2D);
}

/***********************************************************
  Function: locVBlank

  VBlank callback function, called when a VBlank interrupt
  occurs.
*/
static void locVBlank()
{
    if(locSemaArm == 1)
    {
        FW_OSAL_SemaSignal(&locSema);
    }
}


/***********************************************************
  Function: locAppTestThread

  The application's main thread. FW_VOVIAL_SwapFrameBuffer
  will stop the loop's execution until a VSync occurs.

  Parameters:
  Arg        - Thread parameter

  Returns:
  ==0        - OK
  !=0        - NG
*/
static void *locAppTestThread(void *Arg)
{
    uint32_t i = 0;

    /* Set to textured fill mode */
    R_DRW2D_CtxFillMode(locDrw2dDev, R_DRW2D_FILLMODE_TEXTURE);

    /* Initially set background color */
    R_DRW2D_CtxBgColor(locDrw2dDev, LOC_BG_COLOR);

    /* Initially set blend mode to SRC_OVER */
    R_DRW2D_CtxBlendMode(locDrw2dDev, R_DRW2D_BLENDMODE_SRC_OVER);

    /* Initially set texture. We only have one in this app. */
    R_DRW2D_CtxTextureSet(locDrw2dDev, 0, &locTexture);

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

    while(!locQuit)
    {
        locSetRenderTarget(locSurface);

        locRender();

        R_DRW2D_GpuFinish(locDrw2dDev, R_DRW2D_FINISH_WAIT);
        FW_VOVIAL_SwapFrameBuffer(locSurface, i);
        ++i;

        FW_OSAL_SemaWait(&locSema);
    }

    FW_OSAL_ThreadExit((void*) 666);

    return 0;
}


/***********************************************************
  Function: locInit

  Tutorial application init function.

  The function creates the required surfaces and initializes
  Drw2D. It also sets up the primitive's colors.

  Parameters:
  void

  Returns:
  void
*/
static void locInit(void) {
    r_drw2d_Error_t err;

    locSemaArm   = 0;
    locQuit      = 0;

    locSurface = FW_VOVIAL_CreateSurface(
            LOC_VOVIAL_UNIT,
            LOC_VOVIAL_FORMAT,
            0,
            0,
            0,
            LOC_DISPLAY_STRIDE,
            LOC_DISPLAY_WIDTH,
            LOC_DISPLAY_HEIGHT,
            0xff,
            2,
            0);
    if(locSurface == 0)
    {
        Error(ERR_VOVIAL);
    }

    err = R_DRW2D_Init();
    if(err != R_DRW2D_ERR_OK)
    {
        Error(ERR_DRW2D);
    }

    err = R_DRW2D_Open(0, R_DRW2D_UNIT_DHD0, &locImplDev, &locDrw2dDev);
    if(err != R_DRW2D_ERR_OK)
    {
        Error(ERR_DRW2D);
    }

    locOldText = Init2DImg((Img_t*) &Img_tutorial_text);

    IMG_GetTexture(&Img_tutorial_text, &locTexture, R_DRW2D_TEX_NONE);

    FW_OSAL_SemaCreate(&locSema);

    FW_VOVIAL_RegisterVBlankIsr(locSurface, locVBlank);

    locAppTutorialThreadId = FW_OSAL_ThreadCreate(locAppTestThread, 0,
                                           locStackAppTutorialThread,
                                           LOC_APP_TUTORIAL_THREAD_STACKSIZE,
                                           LOC_APP_TUTORIAL_THREAD_PRIO);
}


/***********************************************************
  Function: locDeInit

  Test application deInit function.

  The function releases all created resources and joins the
  app's thread by setting locQuit to 1, which will cause
  the main thread to end.

  Parameters:
  void

  Returns:
  void
*/
static void locDeInit(void)
{
    /* Signal thread to quit */
    locQuit = 1;

    FW_OSAL_ThreadJoin(locAppTutorialThreadId, &locRetVal);

    Deinit2DImg((Img_t*) &Img_tutorial_text, locOldText);

    FW_OSAL_SemaDestroy(&locSema);

    R_DRW2D_Close(locDrw2dDev);
    R_DRW2D_Exit();

    FW_VOVIAL_DeleteSurface(locSurface);
}


/***********************************************************
  Section: Local Variables
*/


/***********************************************************
  Variable: locAppTutorialControl

  Control function structure.

  List of callback functions for each control element.
*/
static const fw_hmial_Control_t locAppTutorialControl = {
    locKnobPress,    /* KNOB press callback */
    0,               /* KNOB release callback  */
    locKnobRotation, /* KNOB right callback */
    locKnobRotation, /* KNOB left callback  */
    0,               /* BUTTON up press callback */
    0,               /* BUTTON up release callback */
    0,               /* BUTTON down press callback  */
    0,               /* BUTTON down release callback */
    0,               /* BUTTON middle press callback */
    0,               /* BUTTON middle release callback */
    0,               /* BUTTON left press callback */
    0,               /* BUTTON left release callback */
    0,               /* BUTTON right press callback */
    0                /* BUTTON right release callback */
};


/***********************************************************
  Section: Global Variables

  Description see: <app_tutorial.h>
*/


/***********************************************************
  Variable: AppTutorial
*/
const App_t AppTutorial = {
    &Img_icon_tutorial,
    "Tutorial App",
    "Simple rendering",
    &locAppTutorialControl,
    0,
    locInit,
    locDeInit
};
