/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: app_simplemt.c 11057 2016-11-10 08:23:04Z shinya.tomari $
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
#include "r_typedefs.h"
#include "r_config_d1x.h"
#include "fw_hmial_api.h"
#include "application.h"
#include "fw_osal_api.h"
#include "fw_vovial_api.h"
#include "fw_clibal_api.h"
#include "r_cdi_api.h"

#include "davehd_driver.h"
#include "r_util_dhd.h"
#include "r_drw2d_os.h"
#include "r_drw2d_api.h"
#include "r_config_drw2d.h"
#include "r_drw2d_ctx_dhd.h"

#include "img.h"
#include "img_drw2d.h"

#include "error.h"
#include "main.h"

#include <math.h>

#include "app_simplemt_image.h"
#include "app_simplemt.h"

/***********************************************************
  Section: Local Defines
*/

#define LOC_NUM_THREADS     4
#define LOC_STACK_SIZE      4096
#define LOC_BAR_HEIGHT      15
#define LOC_BAR_WIDTH       180
#define LOC_BGCOLOR         0xff000080
#define LOC_COLOR_1         0xffdd0000
#define LOC_COLOR_2         0xff00ff00
#define LOC_COLOR_3         0xff0000ff
#define LOC_COLOR_4         0xffffff00
#define LOC_COLOR_5         0xff00ffff
#define LOC_COLOR_6         0xffff00ff
#define LOC_RND_SEED        78163

/***********************************************************
  Section: Local Types
*/
typedef struct {
  int32_t               YPos;       /* Y position of the thread's bar */
  uint32_t              BarColor;   /* color of the bar */
  int32_t               Sleep;      /* Sleep time */
  void                  *Stack;     /* pointer to the Stack */
  uint32_t              Thread;     /* Thread ID */
  r_drw2d_Framebuffer_t *Fb;        /* Framebuffer */
  uint32_t              Number;     /* Application thread number */
} locThreadData_t;


/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Variable: locDrawingSurface

  Drawing surface for the threads.
*/
static void             *locDrawingSurface;

/***********************************************************
  Variable: locDrawingFb

  Drw2D frame buffer structure used as the thread's render target.
*/
static r_drw2d_Framebuffer_t locDrawingFb;

/***********************************************************
  Variable: locSurface

  Surface for a simple background color.
*/
static void             *locSurface;

/***********************************************************
  Variable: locThreadData

  Needed data for every thread.
*/
static locThreadData_t  locThreadData[LOC_NUM_THREADS];

/***********************************************************
  Variable: locEndThreads

  Stop signal for the threads (when set to 1).
*/
static volatile int     locEndThreads = 0;

/***********************************************************
  Variable: locBarColors

  Colors for the thread's bars.
*/
static int              locBarColors[] = {  LOC_COLOR_1, LOC_COLOR_2, LOC_COLOR_3,
                                            LOC_COLOR_4, LOC_COLOR_5, LOC_COLOR_6 };

/***********************************************************
  Variable: locRndSeed

  Seed for the random number generation.
*/
static unsigned int     locRndSeed = LOC_RND_SEED;

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

extern r_cdi_Heap_t loc_lRAM_heap;

/***********************************************************
  Section: Local Functions
*/

/************************************************************
  Function: locRnd

  Return a pseudo random number for generating the thread sleeping time
*/
static unsigned int locRnd(void)
{
    unsigned int x;
    x = ( (locRndSeed>>16) + 3715436908ul ) ^ 0x1fd8dae7;
    locRndSeed += x;
    return locRndSeed;
}

/*******************************************************************************
  Function: locSetRenderTarget

  Fills a DRW2D frame buffer structure with the frame buffer data of a given
  window.
*/
static void locGetDrw2dFramebuffer(r_drw2d_Framebuffer_t *Fb, void* Win)
{
    fw_vovial_FbFormat_t fmt;

    /* Init framebuffer struct */
    Fb->Flags = (r_drw2d_FramebufferFlags_t) 0;
    Fb->Handle = 0; /* not used currently */
    FW_VOVIAL_GetBufferInfo(Win, &fmt, &Fb->Buffer.Pitch, &Fb->Buffer.Size.Width, &Fb->Buffer.Size.Height);
    Fb->Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
    Fb->Buffer.Data = FW_VOVIAL_GetFrameBuffer(Win);
}

/*******************************************************************************
  Function: locSetRenderTarget

  Sets the target for the subsequent render commands to a current draw buffer
  of a window (its back-buffer)
*/
static void* locSetRenderTarget(r_drw2d_Device_t Dev, void* Win)
{
    r_drw2d_Framebuffer_t fb;

    locGetDrw2dFramebuffer(&fb, Win);
    R_DRW2D_FramebufferSet(Dev, &fb);

    return fb.Buffer.Data;
}

/************************************************************
  Function: locDrawingThread
*/
static void *locDrawingThread(void *data)
{
    locThreadData_t     *threadData = (locThreadData_t*)data;
    int32_t             barwidth = 1;
    r_drw2d_Error_t     err;
    r_drw2d_DeviceDHD_t dhddev;
    r_drw2d_Device_t    dev;
    r_drw2d_Rect_t      bar;
    const int32_t       max_barwidth = threadData->Fb->Buffer.Size.Width;

    bar.Pos.X = R_DRW2D_2X(0);
    bar.Pos.Y = R_DRW2D_2X(threadData->YPos);
    bar.Size.Height = R_DRW2D_2X(LOC_BAR_HEIGHT);

    err = R_DRW2D_Open(0, R_DRW2D_UNIT_DHD0, &dhddev, &dev);
    if(err != R_DRW2D_ERR_OK)
    {
        Error(ERR_DRW2D);
    }

    R_DRW2D_FramebufferSet(dev, threadData->Fb);
    R_DRW2D_CtxFgColor(dev, threadData->BarColor);

    while(1)
    {
        /* Draw half transparent full bar */
        R_DRW2D_CtxFgColor(dev, 0x80808080);
        bar.Size.Width = R_DRW2D_2X(max_barwidth);
        R_DRW2D_DrawRect(dev, &bar);

        /* Draw growing bar */
        bar.Size.Width = R_DRW2D_2X(barwidth);
        R_DRW2D_CtxFgColor(dev, threadData->BarColor);
        R_DRW2D_DrawRect(dev, &bar);

        barwidth = (barwidth + 1) % max_barwidth;

        R_DRW2D_GpuFinish(dev, R_DRW2D_FINISH_NOWAIT);

        /* Thread goes to Sleep */
        if(threadData->Sleep)
        {
          FW_OSAL_ThreadSleep(threadData->Sleep);
        }

        R_DRW2D_GpuFinish(dev, R_DRW2D_FINISH_WAIT);

        /* deinit device and Thread if necessary */
        if(locEndThreads)
        {
            R_DRW2D_Close(dev);
            FW_OSAL_ThreadExit(0);
        }
    }
}


/***********************************************************
  Function: locThreadsInit
*/
static void locThreadsInit(void)
{
   int i;
   int bar_ypos;

   bar_ypos = 0;
   locEndThreads = 0;

   locDrawingSurface = FW_VOVIAL_CreateSurface(
           LOC_VOVIAL_UNIT,
           FW_VOVIAL_ARGB8888,
           20,
           20,
           1,
           LOC_DISPLAY_STRIDE,
           LOC_DISPLAY_WIDTH-40,
           LOC_DISPLAY_HEIGHT-40,
           0xff,
           1,
           0);
   if (!locDrawingSurface)
   {
       Error(ERR_NG);
   }

   /* Prepare the thread frame buffer */
   FW_VOVIAL_SwapFrameBuffer(locDrawingSurface, 0);
   locGetDrw2dFramebuffer(&locDrawingFb, locDrawingSurface);
   R_DRW2D_FramebufferSet(locDrw2dDev, &locDrawingFb);

   R_DRW2D_CtxBgColor(locDrw2dDev, 0x00000000);
   R_DRW2D_FramebufferClear(locDrw2dDev);
   R_DRW2D_GpuFinish(locDrw2dDev, R_DRW2D_FINISH_WAIT);

   FW_VOVIAL_EnableSurface(locDrawingSurface);


   /* Prepare the threads */
   for(i=0; i<LOC_NUM_THREADS; i++)
   {
       locThreadData[i].BarColor = locBarColors[i];
       locThreadData[i].Fb = &locDrawingFb;
       locThreadData[i].Number = i;

       /* set a random sleeping time for each Thread */
       do
       {
           locThreadData[i].Sleep = (locRnd()%7) * 2 + i;
       }
       while (locThreadData[i].Sleep <= 0);

       locThreadData[i].YPos = bar_ypos;
       bar_ypos += LOC_BAR_HEIGHT * 2;

       /* setup the Stack for each Thread */
       locThreadData[i].Stack = (void*) R_CDI_Alloc(&loc_lRAM_heap, LOC_STACK_SIZE);

       /* set the Thread ID for each Thread */
       if(locThreadData[i].Stack)
       {
           FW_CLIBAL_PrintF("Starting thread %d...\n", i);
           locThreadData[i].Thread = FW_OSAL_ThreadCreate(locDrawingThread,
                   &locThreadData[i], locThreadData[i].Stack, LOC_STACK_SIZE, 36);
       }
   }
}


/***********************************************************
  Function: locThreadsDeInit
*/
static void locThreadsDeInit(void)
{
    int i;
    locEndThreads = 1;
    for(i=0; i<LOC_NUM_THREADS; i++)
    {
        void *retval;

        /* wait until Thread terminated */
        FW_OSAL_ThreadJoin(locThreadData[i].Thread, &retval);
        locThreadData[i].Thread = 0;

        /* free Stack memory */
        R_CDI_Free((uint32_t)locThreadData[i].Stack, &loc_lRAM_heap);
    }

    FW_VOVIAL_DisableSurface(locDrawingSurface);
    FW_VOVIAL_DeleteSurface(locDrawingSurface);
}


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


/************************************************************
  Function: locKnobLeftRight
*/
static void locKnobLeftRight(void)
{
    int i;

    for(i = 0; i < LOC_NUM_THREADS; i++)
    {
        do
        {
            locThreadData[i].Sleep = (locRnd()%7) * 2;
        }
        while (locThreadData[i].Sleep <= 0);
    }
}


/***********************************************************
  Function: locInit
*/
static void locInit(void)
{
    r_drw2d_Error_t err;

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
            1,
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

    FW_VOVIAL_EnableSurface(locSurface);

    /* Clear background surface */
    locSetRenderTarget(locDrw2dDev, locSurface);
    R_DRW2D_CtxBgColor(locDrw2dDev, LOC_BGCOLOR);
    R_DRW2D_CtxFgColor(locDrw2dDev, 0xff00ffff);
    R_DRW2D_FramebufferClear(locDrw2dDev);
    R_DRW2D_GpuFinish(locDrw2dDev, R_DRW2D_FINISH_WAIT);

    /* initialize the threads */
    locThreadsInit();
}

/***********************************************************
  Function: locDeInit
*/

static void locDeInit(void)
{
    /* Signal thread to quit */
    locThreadsDeInit();

    R_DRW2D_Close(locDrw2dDev);

    R_DRW2D_Exit();

    FW_VOVIAL_DisableSurface(locSurface);
    FW_VOVIAL_DeleteSurface(locSurface);
}

/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Variable: locAppFontControl

  Control function structure.

  List of callback functions for each control element.
*/

static const fw_hmial_Control_t locAppD2ThreadControl = {
    locKnobPress,           /* KNOB press callback */
    0,                      /* KNOB release callback  */
    locKnobLeftRight,       /* KNOB right callback */
    locKnobLeftRight,       /* KNOB left callback  */
    0,                      /* BUTTON up press callback */
    0,                      /* BUTTON up release callback */
    0,                      /* BUTTON down press callback  */
    0,                      /* BUTTON down release callback */
    0,                      /* BUTTON middle press callback */
    0,                      /* BUTTON middle release callback */
    0,                      /* BUTTON left press callback */
    0,                      /* BUTTON left release callback */
    0,                      /* BUTTON right press callback */
    0                       /* BUTTON right release callback */
};

/***********************************************************
  Section: Global Variables

  Description see: <application.h>
*/

const App_t AppSimpleMt = {
    &Img_icon_demosimplemt,
    "Multithreading Simple App",
    "Demonstrate mutithreading in 2d graphics",
    &locAppD2ThreadControl,
    0,
    locInit,
    locDeInit,
};
