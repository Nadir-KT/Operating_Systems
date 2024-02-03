/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: app_drw2dcpu.c 11057 2016-11-10 08:23:04Z shinya.tomari $
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
#include "r_config_d1x.h"
#include "fw_hmial_api.h"
#include "application.h"
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

#include "app_drw2dcpu_image.h"
#include "main.h"
#include "error.h"

#include <math.h>


/***********************************************************
  Section: Local Defines
*/

/***********************************************************
  Define: LOC_BGCOLOR

  Background color of the display.
*/

#define LOC_BGCOLOR                       0xff888888


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

#define LOC_CYLINDER_POS_Y (LOC_DISPLAY_HEIGHT - 30)


/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Variable: locSurface

  The application's drawing surface 1.
*/

static void       *locSurface;


/***********************************************************
  Variable: locOldData*

  Pointer to keep the original image data.
*/

static void       *locOldDataCylinder;
static void       *locOldDataMarble;


/***********************************************************
  Variable: locPosX

  Position of the sprite.
*/

static int32_t     locPosX;

/***********************************************************
  Variable: locMarble*

  Position of the marble.
*/
static float32_t locMarbleX;
static float32_t locMarbleY;

/***********************************************************
  Variable: locMarbleVel*

  Velocity of the marble.
*/
static float32_t locMarbleVelX;
static float32_t locMarbleVelY;


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
  Variable: locImplDev

  Drw2D Implementation Drawing handle
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
    locPosX += 10;
    if(locPosX > LOC_DISPLAY_WIDTH - Img_cylinder.Width) {
        locPosX = LOC_DISPLAY_WIDTH - Img_cylinder.Width;
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
    locPosX -= 10;
    if(locPosX < 0) {
        locPosX = 0;
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
    fb.Buffer.PixelFormat = LOC_DRW2D_FRAMEBUFFER_FORMAT;
    fb.Buffer.Data = FW_VOVIAL_GetFrameBuffer(Win);
    R_DRW2D_FramebufferSet(locDrw2dDev, &fb);

    return fb.Buffer.Data;
}


static void locMarbleCheckBoundaryCollision()
{
    if(locMarbleX < 0)
    {
        locMarbleX = 0;
        locMarbleVelX *= -1;
    }
    else if(locMarbleX > LOC_DISPLAY_WIDTH - Img_marble.Width)
    {
        locMarbleX = LOC_DISPLAY_WIDTH - Img_marble.Width;
        locMarbleVelX *= -1;
    }

    if(locMarbleY < 0)
    {
        locMarbleY = 0;
        locMarbleVelY *= -1;
    }
    else if(locMarbleY > LOC_DISPLAY_HEIGHT - Img_marble.Height)
    {
        locMarbleY = LOC_DISPLAY_HEIGHT - Img_marble.Height;
        locMarbleVelY *= -1;
    }
}


static void locMarbleCheckCylinderCollision()
{
    float32_t temp;
    const int32_t m_top = locMarbleY;
    const int32_t m_bottom = locMarbleY + Img_marble.Height;
    const int32_t m_left = locMarbleX;
    const int32_t m_right = locMarbleX + Img_marble.Width;
    const int32_t c_top = LOC_CYLINDER_POS_Y;
    const int32_t c_bottom = LOC_CYLINDER_POS_Y + Img_cylinder.Height;
    const int32_t c_left = locPosX;
    const int32_t c_right = locPosX + Img_cylinder.Width;
    const float32_t distf = ((locMarbleX + Img_marble.Width/2.0f) - (locPosX + Img_cylinder.Width/2.0f))/50.0f*3.1459f;

    if( ((m_bottom > c_top) && (m_bottom < c_bottom)) &&
        (
                ((m_right > c_left) && (m_right < c_right)) ||
                ((m_left < c_right) && (m_left > c_left))
        )
        )
    {
        locMarbleY -= m_bottom - c_top;
        locMarbleVelY *= -1;

        temp          = cosf(distf) * locMarbleVelX - sinf(distf) * locMarbleVelY;
        locMarbleVelY = sinf(distf) * locMarbleVelX + cosf(distf) * locMarbleVelY;
        locMarbleVelX = temp;
    }

    if( ((m_top < c_bottom) && (m_top > c_top)) &&
        (
                ((m_right > c_left) && (m_right < c_right)) ||
                ((m_left < c_right) && (m_left > c_left))
        )
        )
    {
        locMarbleY = c_bottom;
        locMarbleVelY *= -1;

        temp          = cosf(distf) * locMarbleVelX - sinf(distf) * locMarbleVelY;
        locMarbleVelY = sinf(distf) * locMarbleVelX + cosf(distf) * locMarbleVelY;
        locMarbleVelX = -temp;
    }
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
    R_DRW2D_FramebufferClear(locDrw2dDev);

    /* Render cylinder */
    R_DRW2D_CtxBlendMode(locDrw2dDev, R_DRW2D_BLENDMODE_SRC);
    IMG_Blit(locDrw2dDev, &Img_cylinder, locPosX, LOC_CYLINDER_POS_Y);

    R_DRW2D_CtxBlendMode(locDrw2dDev, R_DRW2D_BLENDMODE_SRC_OVER);
    IMG_Blit(locDrw2dDev, &Img_marble, (int32_t) locMarbleX, (int32_t) locMarbleY);

    locMarbleX += locMarbleVelX;
    locMarbleY += locMarbleVelY;

    locMarbleCheckBoundaryCollision();
    locMarbleCheckCylinderCollision();
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
    R_DRW2D_CtxTransformMode(locDrw2dDev, R_DRW2D_TRANSFORM_NONE);

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

    locPosX       = LOC_DISPLAY_WIDTH/2 - Img_cylinder.Width/2;
    locQuit       = 0;
    locMarbleX    = LOC_DISPLAY_WIDTH/2 - Img_marble.Width/2;
    locMarbleY    = LOC_DISPLAY_HEIGHT*0.2;
    locMarbleVelX = 0.0f;
    locMarbleVelY = 2.0f;

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

    locOldDataCylinder = Init2DImg((Img_t*) &Img_cylinder);
    locOldDataMarble = Init2DImg((Img_t*) &Img_marble);

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

    Deinit2DImg((Img_t*) &Img_cylinder, locOldDataCylinder);
    Deinit2DImg((Img_t*) &Img_marble, locOldDataMarble);

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

static const fw_hmial_Control_t locAppDrw2dCPUControl = {
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

const App_t AppDrw2dCPU = {
    &Img_icon_drw2dcpu,
    "Drw2D Blit App",
    "Shows the Drw2D texture blit feature",
    &locAppDrw2dCPUControl,
    0,
    locInit,
    locDeInit
};
