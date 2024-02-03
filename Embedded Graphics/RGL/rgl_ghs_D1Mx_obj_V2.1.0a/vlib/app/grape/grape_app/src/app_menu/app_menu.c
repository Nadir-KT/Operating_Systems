/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: app_menu.c 11057 2016-11-10 08:23:04Z shinya.tomari $
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
  Title: Menu Application Module
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
#include "fw_clibal_api.h"

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

#include "img.h"
#include "font.h"
#include "font_data.h"
#include "img_drw2d.h"

#include "write.h"

#include "error.h"
#include "main.h"

#include <math.h>

#include "app_menu.h"

/***********************************************************
  Section: Local Defines
*/

/***********************************************************
  Define: LOC_APP_MENU_THREAD_STACKSIZE

  Size in bytes of the stack used by the application thread
*/

#define LOC_APP_MENU_THREAD_STACKSIZE     0x800


/***********************************************************
  Define: LOC_APP_MENU_THREAD_PRIO

  Priority level of the application thread

*/
#define LOC_APP_MENU_THREAD_PRIO           0x22


/***********************************************************
  Variable: locStackAppMenuThread

  Thread local stack

*/
static uint32_t locStackAppMenuThread[LOC_APP_MENU_THREAD_STACKSIZE >> 2];

/***********************************************************
  Variable: locStackRenderThread

  d3D Thread local stack

*/
static uint32_t locStackRenderThread[LOC_APP_MENU_THREAD_STACKSIZE >> 2];

/***********************************************************
  Variable: locAppMenuThreadId

  Id of the menu thread
*/

static uint32_t locAppMenuThreadId;

/***********************************************************
  Variable: locRenderThreadId

  Id of the menu thread
*/

static uint32_t locRenderThreadId;


/***********************************************************
  Variable: locAppMenuSema

  Local semaphore of the application thread

*/
static fw_osal_Sema_t  locAppMenuSema;


/***********************************************************

  Variable: locSema

  Semaphore to stop the render thread from running.
*/
static fw_osal_Sema_t  locSema;


/***********************************************************

  Variable: locSemaArm

  Variable to arm locSema semaphore (arm == 1, disarm == 0).
*/

static int32_t locSemaArm;


/***********************************************************
  Variable: locQuit

  Signal flag to leave the application.
*/

static volatile uint8_t locQuit;


/***********************************************************
  Variable: locMenuPos

  Current menu position
*/

static int8_t locMenuPos;

/***********************************************************
  Variable: locSurface

  The application's VOVIAL render surface
*/

static void *locSurface;

/***********************************************************
  Variable: locOldData

  Store for old image data pointers.
*/

static void *locOldData[20];

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

  Variable: locDrw2dDev

  Pointer to the Drw2D device handle
*/

static r_drw2d_Device_t locDrw2dDev;

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

static void locKnobPress(void)
{
    Cmd_t cmd;

    if (locMenuPos < AppNum)
    {
        cmd.Cmd  = CMD_STOPSTART;
        cmd.Par1 = AppNum - 1;
        cmd.Par2 = locMenuPos;
    }
    else
    {
        cmd.Cmd  = CMD_EXIT;;
        cmd.Par1 = 0;
        cmd.Par2 = 0;
    }
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

static void locKnobRight(void)
{
    locMenuPos++;
    if (locMenuPos > AppNum-2)
    {
        locMenuPos = 0;
    }
    FW_OSAL_SemaSignal(&locAppMenuSema);
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

static void locKnobLeft(void)
{
    locMenuPos--;
    if (locMenuPos < 0)
    {
        locMenuPos = AppNum-2;
    }
    FW_OSAL_SemaSignal(&locAppMenuSema);
}


/*******************************************************************************
  Function: loc_SetRenderTarget

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
    fb.Buffer.PixelFormat = LOC_DRW2D_FRAMEBUFFER_FORMAT;
    fb.Buffer.Data = FW_VOVIAL_GetFrameBuffer(Win);
    R_DRW2D_FramebufferSet(locDrw2dDev, &fb);

    return fb.Buffer.Data;
}


#ifdef R_DRW2D_SYS_DHD
/***********************************************************
  Function: locDrawIconMenuBackGround

  Render the icon menu backrgound.

  Parameters:
  Dev        - D/AVE 2D device structure
  Width      - Width of the background
  Height     - Height of the background
  PosY       - Y postion where to put the background

  Returns:
  void
*/
static void locDrawIconMenuBackGround(
    int32_t Width,
    int32_t Height,
    int32_t PosY )
{
    r_drw2d_Rect_t rect;

    R_DRW2D_CtxBlendMode(locDrw2dDev, R_DRW2D_BLENDMODE_SRC);
    R_DRW2D_CtxFillMode(locDrw2dDev, R_DRW2D_FILLMODE_SOLID);
    R_DRW2D_CtxFgColor(locDrw2dDev, 0xFF000000);
    rect.Pos.X = R_DRW2D_2X(0);
    rect.Pos.Y = R_DRW2D_2X(PosY);
    rect.Size.Height = R_DRW2D_2X(Height);
    rect.Size.Width = R_DRW2D_2X(Width);
    R_DRW2D_DrawRect(locDrw2dDev, &rect);

    R_DRW2D_CtxFgColor(locDrw2dDev, 0xFF777777);
    rect.Pos.X = R_DRW2D_2X(0);
    rect.Pos.Y = R_DRW2D_2X(PosY);
    rect.Size.Height = R_DRW2D_2X(2);
    rect.Size.Width = R_DRW2D_2X(Width);
    R_DRW2D_DrawRect(locDrw2dDev, &rect);

    R_DRW2D_CtxFgColor(locDrw2dDev, 0xFFCCCCCC);
    rect.Pos.X = R_DRW2D_2X(0);
    rect.Pos.Y = R_DRW2D_2X(PosY+Height-2);
    rect.Size.Height = R_DRW2D_2X(2);
    rect.Size.Width = R_DRW2D_2X(Width);
    R_DRW2D_DrawRect(locDrw2dDev, &rect);
}


/***********************************************************
  Function: locDrawDemoTextMenuBackGround

  Render the text menu background.

  Parameters:
  Width      - Width of the background
  Height     - Height of the background
  PosY       - Y position where to put the background
  Color      - Color of the background

  Returns:
  void
*/

static void locDrawDemoTextMenuBackGround(
    int32_t Width,
    int32_t Height,
    int32_t PosY,
    uint32_t Color)
{
    r_drw2d_Rect_t rect;

    R_DRW2D_CtxBlendMode(locDrw2dDev, R_DRW2D_BLENDMODE_SRC);
    R_DRW2D_CtxFillMode(locDrw2dDev, R_DRW2D_FILLMODE_SOLID);
    R_DRW2D_CtxFgColor(locDrw2dDev, Color);
    rect.Pos.X = R_DRW2D_2X(0);
    rect.Pos.Y = R_DRW2D_2X(PosY);
    rect.Size.Height = R_DRW2D_2X(Height);
    rect.Size.Width = R_DRW2D_2X(Width);
    R_DRW2D_DrawRect(locDrw2dDev, &rect);
}

#else

/***********************************************************
  Function: locDrawBackGroundDrw2dCPU

  Render the whole background for the Drw2D CPU
  implementation, which only provides a subset of the Drw2D
  features.

  Parameters:
  Width      - Width of the background
  Height     - Height of the background

  Returns:
  void
*/

static void locDrawBackGroundDrw2dCPU(
    int32_t Width,
    int32_t Height)
{
    r_drw2d_IntRect_t rect;

    rect.Pos.X = 0;
    rect.Pos.Y = 0;
    rect.Size.Width  = Width;
    rect.Size.Height = TOP_TEXT_HEIGHT;

    R_DRW2D_CtxBgColor(locDrw2dDev, COLOR_GREY);
    R_DRW2D_CtxClipRect(locDrw2dDev, &rect);
    R_DRW2D_FramebufferClear(locDrw2dDev);

    rect.Pos.X = 0;
    rect.Pos.Y = TOP_TEXT_HEIGHT;
    rect.Size.Width  = Width;
    rect.Size.Height = Height - BOTTOM_TEXT_HEIGHT;

    R_DRW2D_CtxBgColor(locDrw2dDev, 0xff000000);
    R_DRW2D_CtxClipRect(locDrw2dDev, &rect);
    R_DRW2D_FramebufferClear(locDrw2dDev);

    rect.Pos.X = 0;
    rect.Pos.Y = Height - BOTTOM_TEXT_HEIGHT;
    rect.Size.Width  = Width;
    rect.Size.Height = BOTTOM_TEXT_HEIGHT;

    R_DRW2D_CtxBgColor(locDrw2dDev, COLOR_GREY);
    R_DRW2D_CtxClipRect(locDrw2dDev, &rect);
    R_DRW2D_FramebufferClear(locDrw2dDev);

    rect.Pos.X = 0;
    rect.Pos.Y = 0;
    rect.Size.Width  = Width;
    rect.Size.Height = Height;

    R_DRW2D_CtxClipRect(locDrw2dDev, &rect);
}
#endif

/***********************************************************
  Function: locIconMenuRender

  Render the icon menus.

  Parameters:

  Returns:
  void
*/

static void locRenderIcon(const Img_t *icon, uint32_t X, uint32_t Y, uint32_t frame)
{
    r_drw2d_Rect_t rect;
    r_drw2d_Rect_t src_rect;
    r_drw2d_Texture_t tex;
    float32_t resize = 0;

    if(frame != 0)
    {
        resize = sinf(((frame%51)/25.0)*M_PI) * 3;
    }

    rect.Pos.X = R_DRW2D_2X(X-resize);
    rect.Pos.Y = R_DRW2D_2X(Y-resize);
    rect.Size.Height = R_DRW2D_2X(MENUICON_HEIGHT+2*resize);
    rect.Size.Width = R_DRW2D_2X(MENUICON_WIDTH+2*resize);

    IMG_GetRect(icon, &src_rect);
    IMG_GetTexture(icon, &tex, R_DRW2D_TEX_BILINEAR);

    R_DRW2D_CtxBlendMode(locDrw2dDev, R_DRW2D_BLENDMODE_SRC);
    R_DRW2D_CtxFillMode(locDrw2dDev, R_DRW2D_FILLMODE_TEXTURE);
    R_DRW2D_CtxTextureSet(locDrw2dDev, 0, &tex);
    R_DRW2D_TextureBlit(locDrw2dDev, &src_rect, &rect);
}


/***********************************************************
  Function: locIconMenuRender

  Render the icon menus.

  Parameters:

  Returns:
  void
*/

static void locIconMenuRender(uint32_t frame)
{
    int32_t   i;
    int32_t   j;
    int32_t   ypos=16;
    int32_t   xpos;
    int32_t   num_icons;
    void *icon;
    int32_t   Width = 240;
    int32_t   Height = 320;

    SelectFont(&FontArial19);
    i     = MENUICON_HEIGHT + 4;
    ypos  = GetFontHeight() * 2;

    /* Draw Menu Icon Background */
#ifdef R_DRW2D_SYS_DHD
    locDrawDemoTextMenuBackGround(Width, TOP_TEXT_HEIGHT, TOP_TEXT_POSY, COLOR_GREY);
    locDrawIconMenuBackGround(Width, (Height - (BOTTOM_TEXT_HEIGHT + TOP_TEXT_HEIGHT)), TOP_TEXT_HEIGHT);
    locDrawDemoTextMenuBackGround(Width, BOTTOM_TEXT_HEIGHT, (Height - BOTTOM_TEXT_HEIGHT), COLOR_GREY);
#else
    locDrawBackGroundDrw2dCPU(Width, Height);
#endif
    num_icons = Width / (MENUICON_WIDTH + MENUICON_SPACE_ICON);

    /* Start after the app menu */
    for (i = 0, j = 0; i < (AppNum - 1); i++)
    {

        /* Number of demos icons per line */
        if (i > num_icons)
        {
            if ((i % (num_icons + 1)) == 0 )
            {
                ypos += MENUICON_HEIGHT + 4;
                j += (num_icons + 1);
            }
        }

        xpos = MENUICON_SPACE_ICON + (MENUICON_WIDTH + 4) * (i - j);

        /* Select icon */
        if (i > (AppNum - 1))
        {
            icon = 0;
        } else {
            if (AppList[i]->Icon)
            {
                icon         = *AppList[i]->Icon->Data;
            } else {
                icon = 0;
            }
        }

        if (icon !=0 )
        {
            if (i == locMenuPos)
            {
                locRenderIcon(AppList[i]->Icon, xpos, ypos, frame);
                Write(locDrw2dDev, ((Width - GetTextWidth(AppList[i]->Name))/2), ((32 - GetFontHeight())/2), 0xff000000, AppList[i]->Name);
            }
            else
            {
                locRenderIcon(AppList[i]->Icon, xpos, ypos, 0);
            }
        }
    }
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
  Function: locAppMenuThread

  The application main thread, which is triggered by the
  KNOB interrupts.

  Parameters:
  Arg        - Thread parameter

  Returns:
  ==0        - OK
  !=0        - NG
*/

static void *locAppMenuThread(void *Arg)
{
    FW_CLIBAL_PrintF("Started Menu App with %d apps...\n", AppNum-1);
    while (!locQuit)
    {
        FW_OSAL_SemaWait(&locAppMenuSema);
    }
    FW_OSAL_ThreadExit((void*)666);

    return 0;
}


/*******************************************************************************
  Function: loc_RenderThreadStart

  The render thread entry point.
*/
static void *locRenderThreadStart(void* Arg)
{
    uint32_t i=0;

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

        locIconMenuRender(i);

        R_DRW2D_GpuFinish(locDrw2dDev, R_DRW2D_FINISH_WAIT);
        FW_VOVIAL_SwapFrameBuffer(locSurface, i);
        ++i;

        locSemaArm = 1;
        FW_OSAL_SemaWait(&locSema);
    }
    FW_OSAL_ThreadExit((void*)42);

    return 0;
}


/***********************************************************
  Function: locInit

  Menu application init function.

  The function creates the required surfaces and contextes.
  It informs the generic D2D framework to use the application's
  drawing function and surface.

  Parameters:
  void

  Returns:
  void
*/

static void locInit(void)
{
    uint32_t i;
    r_drw2d_Error_t err;

    locSemaArm = 0;
    locQuit    = 0;
    locMenuPos = 0;
    FW_OSAL_SemaCreate(&locAppMenuSema);
    FW_OSAL_SemaCreate(&locSema);

    locSurface = FW_VOVIAL_CreateSurface(LOC_VOVIAL_UNIT, LOC_VOVIAL_FORMAT, 0, 0, 0, LOC_DISPLAY_STRIDE, LOC_DISPLAY_WIDTH, LOC_DISPLAY_HEIGHT, 0xff, 2, 0);

    i = FW_VOVIAL_SelectSurface(locSurface);
    if(i != 0)
    {
        Error(ERR_VOVIAL);
    }

    err = R_DRW2D_Init();
    err |= R_DRW2D_Open(0, R_DRW2D_UNIT_DHD0, &locImplDev, &locDrw2dDev);
    if(err != R_DRW2D_ERR_OK)
    {
        Error(ERR_DRW2D);
    }

    /* Setup Textures used in the Menu */
    for (i = 0; i < (AppNum - 1); i++)
    {
        locOldData[i] = Init2DImg((Img_t*)AppList[i]->Icon);
    }
    locOldData[i] = Init2DImg((Img_t*)FontArial19.Texture);

    FW_VOVIAL_RegisterVBlankIsr(locSurface, &locVBlank);

    locRenderThreadId = FW_OSAL_ThreadCreate(locRenderThreadStart, 0,
                                               locStackRenderThread,
                                               LOC_APP_MENU_THREAD_STACKSIZE,
                                               LOC_APP_MENU_THREAD_PRIO);

    locAppMenuThreadId = FW_OSAL_ThreadCreate(locAppMenuThread, 0,
                                           locStackAppMenuThread, 
                                           LOC_APP_MENU_THREAD_STACKSIZE, 
                                           LOC_APP_MENU_THREAD_PRIO);
}


/***********************************************************
  Function: locDeInit

  Test application deinit function.

  The function releases all created resources. It informs
  the generic D2D framework not to use the application's
  surface or drawing function anymore.

  Parameters:
  void

  Returns:
  void
*/

static void locDeInit(void)
{
    uint32_t i;
    /* Signal thread to quit */
    locQuit = 1;

    FW_OSAL_SemaSignal(&locAppMenuSema);

    FW_OSAL_ThreadJoin(locAppMenuThreadId, &locRetVal);
    FW_OSAL_ThreadJoin(locRenderThreadId, &locRetVal);

    for (i = 0; i < (AppNum - 1); i++)
    {
        Deinit2DImg((Img_t*)AppList[i]->Icon, locOldData[i]);
    }
    Deinit2DImg((Img_t*)FontArial19.Texture, locOldData[i]);

    i = R_DRW2D_Close(locDrw2dDev);
    if(i != 0)
    {
        Error(ERR_DRW2D);
    }
    i = R_DRW2D_Exit();
    if(i != 0)
    {
        Error(ERR_DRW2D);
    }

    FW_VOVIAL_DeleteSurface(locSurface);

    FW_OSAL_SemaDestroy(&locAppMenuSema);
    FW_OSAL_SemaDestroy(&locSema);
}



/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Variable: locAppMenuControl

  Control function structure.

  List of callback functions for each control element.
*/

static const fw_hmial_Control_t locAppMenuControl = {
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
  Variable: AppMenu
*/

const App_t AppMenu = {
    0,
    "Menu Application",
    "Menu for application launch",
    &locAppMenuControl,
    0,
    locInit,
    locDeInit
};
