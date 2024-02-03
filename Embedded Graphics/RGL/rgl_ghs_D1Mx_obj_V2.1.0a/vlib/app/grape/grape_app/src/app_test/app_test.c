/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: app_test.c 11057 2016-11-10 08:23:04Z shinya.tomari $
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
  Title: Test Application Module
*/

/***********************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "fw_hmial_api.h"
#include "application.h"
#include "fw_osal_api.h"
#include "fw_vovial_api.h"

#include "davehd_driver.h"
#include "r_cdi_api.h"
#include "r_util_dhd.h"
#include "r_drw2d_os.h"
#include "r_drw2d_api.h"
#include "r_config_drw2d.h"
#include "r_drw2d_ctx_dhd.h"

#include "img_format.h"
#include "img.h"
#include "img_drw2d.h"

#include "app_test_image.h"
#include "main.h"
#include "error.h"


/***********************************************************
  Section: Local Defines
*/

/***********************************************************
  Define: LOC_BGCOLOR

  Background color of the display.
*/

#define LOC_BGCOLOR                       0xff333333


/***********************************************************
  Define: LOC_APP_TEST_THREAD_STACKSIZE

  Size in bytes of the stack used by the application thread
*/

#define LOC_APP_TEST_THREAD_STACKSIZE     0x1400


/***********************************************************
  Define: LOC_APP_TEST_THREAD_PRIO

  Priority level of the application thread

*/
#define LOC_APP_TEST_THREAD_PRIO           0x22


/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Variable: locSurface

  The application's drawing surface 1.
*/

static void       *locSurface;


/***********************************************************
  Variable: locOldData

  Pointer to keep the original image data.
*/

static void       *locOldData;


/***********************************************************
  Variable: locPosX

  Position of the sprite.
*/

static int32_t     locPosX;


/***********************************************************
  Variable: locStackAppTestThread

  d3D Thread local stack

*/
static uint32_t locStackAppTestThread[LOC_APP_TEST_THREAD_STACKSIZE >> 2];


/***********************************************************
  Variable: locAppTestThreadId

  Id of the test thread
*/

static uint32_t locAppTestThreadId;


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
  Variable: locRetVal

  Thread return value;
*/

static void *locRetVal;


/***********************************************************
  Section: Local Functions
*/


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
  Function: locKnobRight

  KNOB right callback function.

  The sprite position is increased.

  Parameters:
  void

  Returns:
  void
*/

static void locKnobRight(void) {
    locPosX++;
    if(locPosX > LOC_DISPLAY_WIDTH) {
        locPosX = 0;
    }
}


/***********************************************************
  Function: locKnobLeft

  KNOB left callback function.

  The sprite position is decreased.

  Parameters:
  void

  Returns:
  void
*/
static void locKnobLeft(void) {
    locPosX--;
    if(locPosX < 0) {
        locPosX = LOC_DISPLAY_WIDTH;
    }
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
    fb.Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
    fb.Buffer.Data = FW_VOVIAL_GetFrameBuffer(Win);
    R_DRW2D_FramebufferSet(locDrw2dDev, &fb);

    return fb.Buffer.Data;
}


/***********************************************************
  Function: locRender

  Test application render function.

  Parameters:
  void

  Returns:
  void
*/
static void locRender()
{
    static int32_t posx = -50;
    r_drw2d_Point_t p;
    r_drw2d_FixedP_t rad;

    R_DRW2D_FramebufferClear(locDrw2dDev);

    /* Draw circle */
    p.X = R_DRW2D_2X(posx); p.Y = R_DRW2D_2X(100);
    rad = R_DRW2D_2X(50);
    R_DRW2D_CtxFillMode(locDrw2dDev, R_DRW2D_FILLMODE_SOLID);
    R_DRW2D_DrawEllipse(locDrw2dDev, p, rad, rad);

    /* Render alien */
    R_DRW2D_CtxBlendMode(locDrw2dDev, R_DRW2D_BLENDMODE_SRC_OVER);
    IMG_Blit(locDrw2dDev, &Img_invaders_alien, locPosX, 90);

    posx++;
    if ((posx-50) > LOC_DISPLAY_WIDTH) {
        posx = -50;
    }
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

  The application's main thread.

  Parameters:
  Arg        - Thread parameter

  Returns:
  ==0        - OK
  !=0        - NG
*/
static void *locAppTestThread(void *Arg)
{
    uint32_t i = 0;

    /* Initially set foreground and background color */
    R_DRW2D_CtxFgColor(locDrw2dDev, 0xffffffff);
    R_DRW2D_CtxBgColor(locDrw2dDev, LOC_BGCOLOR);

    // Clear surface before enabling it
    locSetRenderTarget(locSurface);
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

        locSemaArm = 1;
        FW_OSAL_SemaWait(&locSema);
    }

    FW_OSAL_ThreadExit((void*) 666);

    return 0;
}


/***********************************************************
  Function: locInit

  Test application init function.

  The function creates the required surfaces and initializes
  Drw2D.
  Afterwards, the space invaders alien image is initialized
  and the app's main thread is created.

  Parameters:
  void

  Returns:
  void
*/
static void locInit(void)
{
    r_drw2d_Error_t err;

    locSemaArm = 0;

    locPosX      = 0;
    locQuit      = 0;

    locSurface = FW_VOVIAL_CreateSurface(
            LOC_VOVIAL_UNIT,
            FW_VOVIAL_ARGB8888,
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

    err = R_DRW2D_Open(0, R_DRW2D_UNIT_DHD0, &locDHDDev, &locDrw2dDev);
    if(err != R_DRW2D_ERR_OK)
    {
        Error(ERR_DRW2D);
    }

    locOldData = Init2DImg((Img_t*) &Img_invaders_alien);

    FW_OSAL_SemaCreate(&locSema);

    FW_VOVIAL_RegisterVBlankIsr(locSurface, locVBlank);

    locAppTestThreadId = FW_OSAL_ThreadCreate(locAppTestThread, 0,
                                           locStackAppTestThread,
                                           LOC_APP_TEST_THREAD_STACKSIZE,
                                           LOC_APP_TEST_THREAD_PRIO);
}


/***********************************************************
  Function: locDeInit

  Test application deinit function.

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

    FW_OSAL_ThreadJoin(locAppTestThreadId, &locRetVal);

    Deinit2DImg((Img_t*) &Img_invaders_alien, locOldData);

    R_DRW2D_Close(locDrw2dDev);
    R_DRW2D_Exit();

    FW_VOVIAL_DeleteSurface(locSurface);

    FW_OSAL_SemaDestroy(&locSema);
}



/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Variable: locAppTestControl

  Control function structure.

  List of callback functions for each control element.
*/

static const fw_hmial_Control_t locAppTestControl = {
    locKnobPress,    /* KNOB press callback */
    0,               /* KNOB release callback  */
    locKnobRight,    /* KNOB right callback */
    locKnobLeft,     /* KNOB left callback  */
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

  Description see: <app_test.h>
*/

/***********************************************************
  Variable: AppTest
*/

const App_t AppTest = {
    &Img_icon_test,
    "Test App",
    "Play field for the framework development",
    &locAppTestControl,
    0,
    locInit,
    locDeInit
};
