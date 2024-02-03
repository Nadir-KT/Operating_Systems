/*
****************************************************************************
PROJECT : GRAPE
FILE    : $Id: fw_vovial_wm.c 13577 2017-05-29 13:29:20Z matthias.nippert $
AUTHOR  : $Author: matthias.nippert $
============================================================================ 
DESCRIPTION
Grape VOUT & VIN Abstraction Layer (VOVIAL)
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
Purpose: only for testing

DISCLAIMER                                                                   
This software is supplied by Renesas Electronics Corporation and is only     
intended for use with Renesas products. No other uses are authorized. This   
software is owned by Renesas Electronics Corporation and is protected under  
all applicable laws, including copyright laws.                               
THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING  
THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT      
LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE   
AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.          
TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS       
ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE  
FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR   
ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE  
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.                             
Renesas reserves the right, without notice, to make changes to this software 
and to discontinue the availability of this software. By using this software,
you agree to the additional terms and conditions found by accessing the      
following link:                                                              
http://www.renesas.com/disclaimer *                                          
Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.


****************************************************************************
*/
/****************************************************************
  Title: Video Out Application layer module, with window manager
  
  This module manages the initialisation and usage of the 
  video output drivers and of the window manager.
*/

/***************************************************************
  Section: Includes
*/
#include "r_typedefs.h"     /* Renesas basic types, e.g. uint32_t */
#include "fw_osal_api.h"
#include "r_ddb_api.h"      /* display database with display timings */
#include "r_cdi_api.h"
#include "r_wm_api.h"
#include "r_wm_sys.h"
#include "fw_vovial_api.h"
#include "fw_osal_api.h"
#include "r_tick_api.h"
#include "r_config_wm.h"

/***************************************************************
  Section: Local Defines
*/

/***************************************************************
  Define: LOC_DECODE_VOUT

  Decodes a slot from a surface.
*/

#define LOC_DECODE_VOUT(S) (((S) & 0x0000ffffu) - 1)

/***************************************************************
  Define: LOC_CHECK_WM_ERR

  Decodes a slot from a surface.
*/

#define LOC_CHECK_WM_ERR(E) if (R_WM_ERR_OK != (E)) loc_ErrorEndlessLoop()

/***************************************************************
  Define: LOC_DECODE_WIN

  Decodes a window slot from a surface.
*/

#define LOC_DECODE_WIN(S) ((((S) & 0xffff0000u) >> 16) - 1)

/***************************************************************
  Define: LOC_ENCODE_SURF

  Encode a window slot from a surface.
*/

#define LOC_ENCODE_SURF(V, W) ( (((W) + 1) << 16) + (V) + 1)


/***************************************************************
  Define: LOC_WM_MSG_QUEUE_LEN
*/

#define LOC_WM_MSG_QUEUE_LEN 512

/***************************************************************
  Define: LOC_INVALID_SURF_ID

  Corresponds to interface expectation of the NULL pointer
*/

#define LOC_INVALID_SURF_ID 0x00000000u

/***************************************************************
  Define: LOC_MAX_SURFACE_NUM

  Maximum number of surfaces.
*/

#define LOC_MAX_SURFACE_NUM_PER_VOUT 4

/***************************************************************
  Define: LOC_MAX_BUFFER_NUM_SURFACE

  Maximum number of buffers per surface.
*/

#define LOC_MAX_BUFFER_NUM_SURFACE 3


/***************************************************************
  Define: LOC_MAX_DISPLAY_NUM

  Maximum number of displays.
*/

#define LOC_MAX_DISPLAY_NUM 2

/***************************************************************
  Define: LOC_WM_EV_EXEC_THREAD_STACK_SIZE
*/

#define LOC_WM_EV_EXEC_THREAD_STACK_SIZE 1024

/***************************************************************
  Define: CHECK_SURF_ID

  Check if valid surface id.
*/

#define CHECK_SURF_ID(V, W) {if ( ((V) > (LOC_MAX_DISPLAY_NUM - 1)) || ((W) > (LOC_MAX_SURFACE_NUM_PER_VOUT - 1)) ) {loc_ErrorEndlessLoop();}}

/***************************************************************
  Section: Local Types
*/

/* Surface type is an integer with encoded VOUT and local array slot */
typedef uint32_t Surface_t;

typedef struct
{
    r_wm_Window_t Win;
    r_wm_WinBuffer_t Buffers[LOC_MAX_BUFFER_NUM_SURFACE];
    uint32_t Empty;
} WindowSlot_t;

/***************************************************************
  Section: Local Variables
*/

/***************************************************************
  Variable: loc_Windows

  The local storage for windows.
*/

static WindowSlot_t loc_Windows[LOC_MAX_DISPLAY_NUM * LOC_MAX_SURFACE_NUM_PER_VOUT];

/***************************************************************
  Variable: loc_VBLANKCallbacks

  The local storage for window VBLANK callback functions.
*/

/***************************************************************
  Variable: loc_WmEventQueue

  WM event queue storage.
*/

static r_wm_Msg_t loc_WmEventQueue[LOC_WM_MSG_QUEUE_LEN];

/***************************************************************
  Variable: loc_WmEvExecThreadStack

  WM event execute thread stack storage.
*/
static uint8_t loc_WmEvExecThreadStack[LOC_WM_EV_EXEC_THREAD_STACK_SIZE];

/***************************************************************
  Variable: loc_VoutMsgCount

  Count of the messages sent to the vout.
*/
static volatile uint32_t loc_VoutMsgCount[R_WM_DEV_NUM];

/***************************************************************
  Variable: loc_VblankIsr

  VBLANK ISRs.
*/
static void(*loc_VblankIsr[R_WM_DEV_NUM])(void);

static fw_osal_Mutex_t loc_WmMutex;

static r_cdi_Heap_t *loc_CpuHeap_VOVIAL = 0;

/***************************************************************
  Section: Local Functions
*/

/*******************************************************************************
  Function: loc_DevCallback

  A device callback function, for receiving notifications like EOF
*/

void loc_DevCallback(uint32_t Unit, const r_wm_Event_t* Event)
{
    if (Event->Id == R_WM_EVENT_VBLANK)
    {
        if (loc_VblankIsr[Unit] != 0)
        {
            /* execute the user defined ISR */
            loc_VblankIsr[Unit]();
        }
    }
}

void loc_IncMsgCount(uint32_t Unit)
{
    FW_OSAL_MutexLock(&loc_WmMutex);
    ++loc_VoutMsgCount[Unit];
    FW_OSAL_MutexUnlock(&loc_WmMutex);
}

/*******************************************************************************
  Function: loc_ErrorEndlessLoop

  Endless loop in case of an error.
*/
static void loc_ErrorEndlessLoop(void)
{
    for (;;)
    {
    }
}

/*******************************************************************************
  Function: loc_FmtConvVovialToWm

  Converts the VOVIAL->WM color format conversion
*/

static uint32_t loc_FmtConvVovialToWm(fw_vovial_FbFormat_t VovialFmt, r_wm_WinColorFmt_t *WmFmt)
{
	switch (VovialFmt)
	{
	case FW_VOVIAL_RGBA8888:
		*WmFmt = R_WM_COLORFMT_RGBA8888;
		break;
	case FW_VOVIAL_ARGB8888:
		*WmFmt = R_WM_COLORFMT_ARGB8888;
		break;
	case FW_VOVIAL_RGB565:
		*WmFmt = R_WM_COLORFMT_RGB565;
		break;
	case FW_VOVIAL_RLE24ARGB8888:
	    *WmFmt = R_WM_COLORFMT_RLE24ARGB8888;
        break;
	case FW_VOVIAL_RLE24RGB0888:
        *WmFmt = R_WM_COLORFMT_RLE24RGB0888;
        break;
	case FW_VOVIAL_RLE18ARGB8888:
        *WmFmt = R_WM_COLORFMT_RLE18ARGB8888;
        break;
	case FW_VOVIAL_RLE18RGB0888:
        *WmFmt = R_WM_COLORFMT_RLE18RGB0888;
        break;
	case FW_VOVIAL_RLE8CLUT8:
        *WmFmt = R_WM_COLORFMT_RLE8CLUT8;
        break;
	case FW_VOVIAL_RLE8CLUT4:
        *WmFmt = R_WM_COLORFMT_RLE8CLUT4;
        break;
	case FW_VOVIAL_RLE8CLUT1:
        *WmFmt = R_WM_COLORFMT_RLE8CLUT1;
        break;
	case FW_VOVIAL_CLUT8:
        *WmFmt = R_WM_COLORFMT_CLUT8;
        break;
	default:
		return R_FALSE;
	}
	return R_TRUE;
}

static uint32_t loc_FmtConvWmToVovial(r_wm_WinColorFmt_t WmFmt, fw_vovial_FbFormat_t *VovialFmt)
{
	switch (WmFmt)
	{
	case R_WM_COLORFMT_RGBA8888:
		*VovialFmt = FW_VOVIAL_RGBA8888;
		break;
	case R_WM_COLORFMT_ARGB8888:
		*VovialFmt = FW_VOVIAL_ARGB8888;
		break;
	case R_WM_COLORFMT_RGB565:
		*VovialFmt = FW_VOVIAL_RGB565;
		break;
	case R_WM_COLORFMT_RLE24ARGB8888:
        *VovialFmt = FW_VOVIAL_RLE24ARGB8888;
        break;
	case R_WM_COLORFMT_RLE24RGB0888:
        *VovialFmt = FW_VOVIAL_RLE24RGB0888;
        break;
	case R_WM_COLORFMT_RLE18ARGB8888:
        *VovialFmt = FW_VOVIAL_RLE18ARGB8888;
        break;
	case R_WM_COLORFMT_RLE18RGB0888:
        *VovialFmt = FW_VOVIAL_RLE18RGB0888;
        break;
	case R_WM_COLORFMT_RLE8CLUT8:
        *VovialFmt = FW_VOVIAL_RLE8CLUT8;
        break;
	case R_WM_COLORFMT_RLE8CLUT4:
        *VovialFmt = FW_VOVIAL_RLE8CLUT4;
        break;
	case R_WM_COLORFMT_RLE8CLUT1:
        *VovialFmt = FW_VOVIAL_RLE8CLUT1;
        break;
	case R_WM_COLORFMT_CLUT8:
        *VovialFmt = FW_VOVIAL_CLUT8;
        break;
	default:
		return R_FALSE;
	}
	return R_TRUE;
}

/*******************************************************************************
  Function: loc_EventExecuteThreadStart

  The WM event execute thread.
*/

void* loc_WmEvExecLoop(void* Arg)
{
    uint32_t i;
    for(;;)
    {
        /* we are greedy: as soon as there are some messages, process them */
        for (i = 0; i < R_WM_DEV_NUM; ++i)
        {
            if (loc_VoutMsgCount[i] > 0)
            {   
                FW_OSAL_MutexLock(&loc_WmMutex);
                loc_VoutMsgCount[i] = 0;
                FW_OSAL_MutexUnlock(&loc_WmMutex);
                R_WM_FrameEndMark(i, 0);
                R_WM_FrameWait(i, 0);
                /*
                    be nice to other threads because otherwise,
                    on some operating systems, they could not get scheduled
                */
            }
        }
        FW_OSAL_ThreadSleep(0);
    }
}


void loc_WM_ErrorCb(uint32_t Unit, r_wm_Error_t Error)
{
    __asm("nop");
    while(1);
}

/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: FW_VOVIAL_Init
  
  For details see vovial.h
*/

void FW_VOVIAL_Init(int32_t Vout, const char *DisplayId, void *CpuHeap, void *VidHeap)
{
    int32_t iv;
    int32_t iw;
    r_wm_Error_t wm_err;
	
    /* initialize the window structure slots */
    for (iv = 0; iv < LOC_MAX_DISPLAY_NUM; ++iv)
    {
        for (iw = 0; iw < LOC_MAX_SURFACE_NUM_PER_VOUT; ++iw)
        {
            loc_Windows[(iv * LOC_MAX_SURFACE_NUM_PER_VOUT) + iw].Win.Status = R_WM_WINSTATUS_NOT_INITIALIZED;
            loc_Windows[(iv * LOC_MAX_SURFACE_NUM_PER_VOUT) + iw].Empty = R_TRUE;
        }
    }

    loc_CpuHeap_VOVIAL = CpuHeap;

    FW_OSAL_MutexCreate(&loc_WmMutex);
    
    R_WM_ErrorCallbackSet(Vout, &loc_WM_ErrorCb);
    
    /* init the screen */
    wm_err = R_WM_DevInit(Vout, loc_WmEventQueue, LOC_WM_MSG_QUEUE_LEN, &loc_DevCallback, CpuHeap, VidHeap);
    LOC_CHECK_WM_ERR(wm_err);

    wm_err = R_WM_ScreenTimingSetByName(Vout, (const int8_t*)DisplayId);
    LOC_CHECK_WM_ERR(wm_err);

    /* NOTE for D1L2 usage:
       Actually D1Lx devices implement RGB666 VDCE video output i/f only.
       For simplified wiring of the display on the D1Lx Mango Board,
       we set-up RGB888 i/f here anyway. Please adapt on custom boards. */
    wm_err = R_WM_ScreenColorFormatSet(Vout, (R_WM_OUTCOLORFMT_RGB888));
    LOC_CHECK_WM_ERR(wm_err);

    /* enable the screen */
    wm_err = R_WM_ScreenEnable(Vout);
    LOC_CHECK_WM_ERR(wm_err);

    wm_err = R_WM_ScreenBgColorSet(Vout, 0x00, 0x00, 0);
    LOC_CHECK_WM_ERR(wm_err);

    FW_OSAL_ThreadCreate(&loc_WmEvExecLoop, R_NULL, loc_WmEvExecThreadStack, LOC_WM_EV_EXEC_THREAD_STACK_SIZE, 45);
}

/*******************************************************************************
  Function: FW_VOVIAL_DeInit

  For details see vovial.h
*/

void FW_VOVIAL_DeInit(int32_t Vout)
{
    r_wm_Error_t wm_err;

    /* deinitialize the screen */
    wm_err = R_WM_DevDeinit(Vout);
    LOC_CHECK_WM_ERR(wm_err);
}


int32_t loc_GetSpriteLayerPitch(r_wm_WinColorFmt_t Format)
{
    int32_t Pitch;

    switch(Format)
    {
    case R_WM_COLORFMT_RGB565:
    case R_WM_COLORFMT_ARGB1555:
    case R_WM_COLORFMT_ARGB4444:
    case R_WM_COLORFMT_RGB0444:
    case R_WM_COLORFMT_RGB0888:
    case R_WM_COLORFMT_ARGB8888:
    case R_WM_COLORFMT_RGBA5551:
    case R_WM_COLORFMT_RGBA4444:
    case R_WM_COLORFMT_RGBA8888:
        Pitch = 2048;
        break;
    case R_WM_COLORFMT_CLUT8:
    case R_WM_COLORFMT_CLUT4:
    case R_WM_COLORFMT_CLUT1:
        Pitch = 8192;
        break;
    default:
        Pitch = 0;
        break;
    }

    return Pitch;
}


/*******************************************************************************
  Function: FW_VOVIAL_CreateSurface
  
  For details see: <vovial.h>
*/

void *FW_VOVIAL_CreateSurface(int32_t Vout, fw_vovial_FbFormat_t Format, 
                           int32_t PosX, int32_t PosY, int32_t PosZ,
                           int32_t Pitch, int32_t Width, int32_t Height,
                           int32_t Alpha, int32_t BufNum, uint32_t Flags)
{
    int32_t i,j;
    r_wm_Window_t* win = 0;

    /* find the next available window slot */
    for (i = 0; i < LOC_MAX_SURFACE_NUM_PER_VOUT; ++i)
    {
        if (loc_Windows[(Vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + i].Empty == R_TRUE)
        {
            break;
        }
    }
    if (i == LOC_MAX_SURFACE_NUM_PER_VOUT)
    {
        /* no available slot found */
        return (void*)LOC_INVALID_SURF_ID;
    }

    loc_Windows[(Vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + i].Empty = R_FALSE;
    win = &loc_Windows[(Vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + i].Win;

    if (R_FALSE == loc_FmtConvVovialToWm(Format, &win->ColorFmt))
    {
        return (void*)LOC_INVALID_SURF_ID; /* color format not supported */
    }

    if (0u != (Flags & FW_VOVIAL_CRFLAG_SPRITE))
    {
        win->Mode = R_WM_WINMODE_SPRITES;
        win->Pitch = loc_GetSpriteLayerPitch(win->ColorFmt);

        if(0 == win->Pitch)
        {
            return (void*)LOC_INVALID_SURF_ID; /* color format not supported for sprite layer */
        }
    }
    else
    {
        win->Mode = R_WM_WINMODE_FB;
        win->Pitch = Pitch;
    }

    win->PosX = PosX;
    win->PosY = PosY;
    win->PosZ = PosZ;
    win->Width = Width;
    win->Height = Height;

    /* Setup the buffer structure */
    if ( (0u != (Flags & FW_VOVIAL_CRFLAG_NO_ALLOC)) ||
         (0u != (Flags & FW_VOVIAL_CRFLAG_RLE)) ||
         (0u != (Flags & FW_VOVIAL_CRFLAG_SPRITE)) )
    {
        win->Surface.Fb.Buffer = loc_Windows[(Vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + i].Buffers;
        for(j = 0; j < LOC_MAX_BUFFER_NUM_SURFACE; ++j)
        {
            win->Surface.Fb.Buffer[j].Data = (void*) 0x0;
            win->Surface.Fb.Buffer[j].Status = R_WM_WINBUF_FREE;
        }
        win->Surface.Fb.BufMode = R_WM_WINBUF_ALLOC_EXTERNAL;
    }
    else
    {
        win->Surface.Fb.Buffer = (r_wm_WinBuffer_t*) R_NULL;
        win->Surface.Fb.BufMode = R_WM_WINBUF_ALLOC_INTERNAL;
    }

    win->Surface.Fb.BufNum = BufNum;
    win->Alpha = Alpha;
    if (R_WM_WindowCreate(Vout, win) == R_WM_ERR_OK)
    {
        return (void*)LOC_ENCODE_SURF(Vout, i);
    }
    /* an error has occured */
    return (void*)LOC_INVALID_SURF_ID;
}


/*******************************************************************************
  Function: FW_VOVIAL_CreateCaptureSurface
  
  For details see <vovial.h>
*/

void *FW_VOVIAL_CreateCaptureSurface(int32_t Vin, fw_vovial_FbFormat_t Format, int32_t Bufnum,
                                  int32_t SrcPitch, int32_t SrcWidth, int32_t SrcHeight, int32_t SrcMode,
                                  int32_t CropWidth, int32_t CropHeight, int32_t CropXOff, int32_t CropYOff)
{
    return (void*)0;
}

/*******************************************************************************
  Function: FW_VOVIAL_SelectSurface

  For details see <vovial.h>
*/
int32_t FW_VOVIAL_SelectSurface(void *Id)
{
    /* not applicable */
    return 0;
}

/*******************************************************************************
  Function: FW_VOVIAL_EnableSurface

  For details see <vovial.h>
*/

void FW_VOVIAL_EnableSurface(void *Id)
{
    uint32_t vout;
    uint32_t win;
    r_wm_Error_t wm_err;

    vout = LOC_DECODE_VOUT((uint32_t)Id);
    win = LOC_DECODE_WIN((uint32_t)Id);

    wm_err = R_WM_WindowEnable(vout, &loc_Windows[(vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + win].Win);
    LOC_CHECK_WM_ERR(wm_err);

    loc_IncMsgCount(vout);
}

/*******************************************************************************
  Function: FW_VOVIAL_EnableCaptureSurface

  For details see <vovial.h>
*/

void FW_VOVIAL_EnableCaptureSurface(void *Id)
{
}

/*******************************************************************************
  Function: FW_VOVIAL_DisableSurface

  For details see <vovial.h>
*/

void FW_VOVIAL_DisableSurface(void *Id)
{
    uint32_t vout;
    uint32_t win;
    r_wm_Error_t wm_err;

    vout = LOC_DECODE_VOUT((uint32_t)Id);
    win = LOC_DECODE_WIN((uint32_t)Id);

    wm_err = R_WM_WindowDisable(vout, &loc_Windows[(vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + win].Win);
    LOC_CHECK_WM_ERR(wm_err);

    loc_IncMsgCount(vout);
}


/*******************************************************************************
  Function: FW_VOVIAL_DisableCaptureSurface

  For details see <vovial.h>
*/

void FW_VOVIAL_DisableCaptureSurface(void *Id)
{
}

/*******************************************************************************
  Function: FW_VOVIAL_DeleteSurface

  For details see <vovial.h>
*/

void FW_VOVIAL_DeleteSurface(void *Id)
{
    uint32_t vout;
    uint32_t win;
    r_wm_Error_t wm_err;

    vout = LOC_DECODE_VOUT((uint32_t)Id);
    win = LOC_DECODE_WIN((uint32_t)Id);

    wm_err = R_WM_WindowDisable(vout, &loc_Windows[(vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + win].Win);
    LOC_CHECK_WM_ERR(wm_err);
    wm_err = R_WM_WindowDelete(vout, &loc_Windows[(vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + win].Win);
    LOC_CHECK_WM_ERR(wm_err);
	
    loc_Windows[(vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + win].Win.Status = R_WM_WINSTATUS_NOT_INITIALIZED;
    loc_Windows[(vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + win].Empty = R_TRUE;

    R_WM_FrameEndMark(vout, 0);
    R_WM_FrameWait(vout, 0);
}


/*******************************************************************************
  Function: FW_VOVIAL_GetNativeWindow

  For details see <vovial.h>
*/

void* FW_VOVIAL_GetNativeWindow(void *Id)
{
    uint32_t vout;
    uint32_t win;

    vout = LOC_DECODE_VOUT((uint32_t)Id);
    win = LOC_DECODE_WIN((uint32_t)Id);

    return (void*)&loc_Windows[(vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + win].Win;
}


void* FW_VOVIAL_GetNativeDisplay(void *Id)
{
    return R_NULL;
}


/*******************************************************************************
  Function: FW_VOVIAL_DeleteCaptureSurface

  For details see <vovial.h>
*/

void FW_VOVIAL_DeleteCaptureSurface(void *Id)
{
}


/*******************************************************************************
  Function: FW_VOVIAL_MoveSurface

  For details see <vovial.h>
*/

void FW_VOVIAL_MoveSurface(void *Id, uint32_t PosX, uint32_t PosY, uint32_t PosZ)
{
    uint32_t vout;
    uint32_t win;
    r_wm_Error_t wm_err;

    vout = LOC_DECODE_VOUT((uint32_t)Id);
    win = LOC_DECODE_WIN((uint32_t)Id);

    wm_err = R_WM_WindowMove(vout, &loc_Windows[(vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + win].Win, PosX, PosY, PosZ);
    loc_IncMsgCount(vout);
    LOC_CHECK_WM_ERR(wm_err);
}


/*******************************************************************************
  Function: FW_VOVIAL_GetFrameCount

  For details see <vovial.h>
*/

uint32_t FW_VOVIAL_GetFrameCount( void *Id )
{
    return 0;
}


/*******************************************************************************
  Function: FW_VOVIAL_GetBufferInfo
  
  For details see vovial.h
*/

void FW_VOVIAL_GetBufferInfo(void *Id, 
                          fw_vovial_FbFormat_t *Format, 
                          int32_t *Pitch,
                          int32_t *Width, 
                          int32_t *Height)
{
    uint32_t i_vout;
    uint32_t i_win;
    r_wm_Window_t* win;

    i_vout = LOC_DECODE_VOUT((uint32_t)Id);
    i_win = LOC_DECODE_WIN((uint32_t)Id);

    CHECK_SURF_ID(i_vout, i_win);

    win = &loc_Windows[(i_vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + i_win].Win;

    loc_FmtConvWmToVovial(win->ColorFmt, Format);
    *Pitch = win->Pitch;
    *Width = win->Width;
    *Height = win->Height;
}


/*******************************************************************************
  Function: FW_VOVIAL_GetFrameBufferIdx
  
  For details see vovial.h
*/

int32_t  FW_VOVIAL_GetFrameBufferIdx(void *Id)
{
    return 0;
}


/*******************************************************************************
  Function: FW_VOVIAL_GetCaptureWindowSurface

  For details see <vovial.h>
*/

void* FW_VOVIAL_GetCaptureWindowSurface(void *Id)
{
    /* ? */
    return (void*)0;
}


/*******************************************************************************
  Function: FW_VOVIAL_GetFrameBuffer
  
  For details see vovial.h
*/

void* FW_VOVIAL_GetFrameBuffer(void* Id)
{
    uint32_t vout;
    uint32_t win;
    void* fb = R_NULL;

    vout = LOC_DECODE_VOUT((uint32_t)Id);
    win = LOC_DECODE_WIN((uint32_t)Id);

    while (!fb)
    {
        fb = R_WM_WindowNewDrawBufGet(vout, &loc_Windows[(vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + win].Win);
    }
    return fb;
}


/*******************************************************************************
  Function: FW_VOVIAL_SwapFrameBuffer
  
  For details see vovial.h
*/

void FW_VOVIAL_SwapFrameBuffer(void *Id, uint32_t FrameCount)
{
    uint32_t vout;
    uint32_t win;
    r_wm_Error_t wm_err;

    vout = LOC_DECODE_VOUT((uint32_t)Id);
    win = LOC_DECODE_WIN((uint32_t)Id);

    wm_err = R_WM_WindowSwap(vout, &loc_Windows[(vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + win].Win);
    LOC_CHECK_WM_ERR(wm_err);
    loc_IncMsgCount(vout);
}


/*******************************************************************************
  Function: FW_VOVIAL_RegisterVBlankIsr
  
  For details see vovial.h
*/

void FW_VOVIAL_RegisterVBlankIsr(void *Id, void (*Isr)(void)) 
{
    uint32_t vout;

    vout = LOC_DECODE_VOUT((uint32_t)Id);

    loc_VblankIsr[vout] = Isr;

    R_WM_DevEventRegister(vout, R_WM_EVENT_VBLANK, 0);
} 


/*******************************************************************************
  Function: FW_VOVIAL_SetSurfaceAlpha

  For details see <vovial.h>
*/

void FW_VOVIAL_SetSurfaceAlpha(void *Id, uint32_t Alpha)
{
    uint32_t vout;
    uint32_t win;
    r_wm_Error_t wm_err;

    vout = LOC_DECODE_VOUT((uint32_t)Id);
    win = LOC_DECODE_WIN((uint32_t)Id);

    wm_err = R_WM_WindowAlphaSet(vout, &loc_Windows[(vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + win].Win, Alpha);
    loc_IncMsgCount(vout);
    LOC_CHECK_WM_ERR(wm_err);
}


/*******************************************************************************
  Function: FW_VOVIAL_GetSurfaceFps
  
  For details see vovial.h
*/

float32_t FW_VOVIAL_GetSurfaceFps(void *Id)
{
    return 0;
}


/*******************************************************************************
  Function: FW_VOVIAL_SetSurfacePerfinfo

  For details see <vovial.h>
*/

void FW_VOVIAL_SetSurfacePerfinfo(void *Id, 
                               fw_vovial_Performance_t Index, 
                               uint32_t Value)
{
    /* not applicable */
}


/*******************************************************************************
  Function: FW_VOVIAL_GetSurfacePerfinfo

  For details see <vovial.h>
*/

uint32_t FW_VOVIAL_GetSurfacePerfinfo(void *Id, 
                                   fw_vovial_Performance_t Index)
{
    /* not applicable */
    return 0;
}


/*******************************************************************************
  Function: FW_VOVIAL_GetCurFrameBuffer
  
  For details see vovial.h
*/

void* FW_VOVIAL_GetCurFrameBuffer(void *Id)
{
    uint32_t vout;
    uint32_t win;
    void* fb = R_NULL;

    vout = LOC_DECODE_VOUT((uint32_t)Id);
    win = LOC_DECODE_WIN((uint32_t)Id);

    while (0 == fb)
    {
        fb = R_WM_WindowCurrentDrawBufGet(vout, &loc_Windows[(vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + win].Win);
    }
    return fb;
}


/*******************************************************************************
  Function: FW_VOVIAL_SetVisibleBuffer

  For details see vovial.h
*/

void FW_VOVIAL_SetVisibleBuffer(void *Id, void *Buffer)
{
}


/*******************************************************************************
  Function: FW_VOVIAL_SetVisibleBuffer

  For details see vovial.h
*/
/*******************************************************************************
  Function: FW_VOVIAL_GetVisibleBuffer

  For details see vovial.h
*/

void* FW_VOVIAL_GetVisibleBuffer(void *Id)
{
    uint32_t vout;
    uint32_t win;
    void* fb = R_NULL;

    vout = LOC_DECODE_VOUT((uint32_t)Id);
    win = LOC_DECODE_WIN((uint32_t)Id);

    while (0 == fb)
    {
        fb = R_WM_WindowVisibleBufGet(vout, &loc_Windows[(vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + win].Win);
    }
    return fb;
}

/*******************************************************************************
  Function: FW_VOVIAL_ResizeSurface
*/

void FW_VOVIAL_ResizeSurface(void *Id, uint32_t Pitch, uint32_t Width, uint32_t Height )
{
    /* todo */
}

/***********************************************************
  Function: loc_ConvSpriteStatusToWM

  Converts a fw_vovial_SpriteStatus_t sprite status to a
  r_wm_SpriteStatus_t sprite status.

  Parameters:
   SpriteStatus - The sprite status to convert.

  Returns:
   The corresponding WM sprite status.
*/
r_wm_SpriteStatus_t loc_ConvSpriteStatusToWM(fw_vovial_SpriteStatus_t SpriteStatus)
{
    r_wm_SpriteStatus_t ret;

    switch(SpriteStatus)
    {
    case FW_VOVIAL_SPRITESTATUS_NOT_INITIALIZED:
        ret = R_WM_SPRITESTATUS_NOT_INITIALIZED;
        break;
    case FW_VOVIAL_SPRITESTATUS_DISABLED:
        ret = R_WM_SPRITESTATUS_DISABLED;
        break;
    case FW_VOVIAL_SPRITESTATUS_ENABLED:
        ret = R_WM_SPRITESTATUS_ENABLED;
        break;
    default:
        ret = R_WM_SPRITESTATUS_NOT_INITIALIZED;
        break;
    }

    return ret;
}

/***********************************************************
  Function: FW_VOVIAL_CreateSprite
*/
int32_t FW_VOVIAL_CreateSprite(fw_vovial_Sprite_t *Sprite)
{
    uint32_t vout;
    uint32_t win;
    r_wm_Sprite_t *wm_sprite;

    vout = LOC_DECODE_VOUT((uint32_t) Sprite->Surface);
    win  = LOC_DECODE_WIN ((uint32_t) Sprite->Surface);

    wm_sprite = R_CDI_Alloc(loc_CpuHeap_VOVIAL, sizeof(*wm_sprite));

    if(wm_sprite == 0)
    {
        return -1;
    }
    else
    {
        Sprite->Intern = (void*) wm_sprite;
    }

    wm_sprite->Window = &loc_Windows[(vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + win].Win;
    wm_sprite->Status = loc_ConvSpriteStatusToWM(Sprite->Status);

    wm_sprite->Data = Sprite->Data;
    wm_sprite->PosX = Sprite->PosX;
    wm_sprite->PosY = Sprite->PosY;
    wm_sprite->PosZ = Sprite->PosZ;
    wm_sprite->Width = Sprite->Width;
    wm_sprite->Height = Sprite->Height;

    R_WM_SpriteCreate(vout, wm_sprite);

    Sprite->Status = FW_VOVIAL_SPRITESTATUS_DISABLED;

    return 0;
}

/***********************************************************
  Function: FW_VOVIAL_EnableSprite
*/
void FW_VOVIAL_EnableSprite(fw_vovial_Sprite_t *Sprite)
{
    uint32_t vout;
    r_wm_Sprite_t *wm_sprite = (r_wm_Sprite_t*) Sprite->Intern;

    vout = LOC_DECODE_VOUT((uint32_t) Sprite->Surface);

    R_WM_SpriteEnable(vout, wm_sprite);

    Sprite->Status = FW_VOVIAL_SPRITESTATUS_ENABLED;
}


/***********************************************************
  Function: FW_VOVIAL_DisableSprite
*/
void FW_VOVIAL_DisableSprite(fw_vovial_Sprite_t *Sprite)
{
    uint32_t vout;
    r_wm_Sprite_t *wm_sprite = (r_wm_Sprite_t*) Sprite->Intern;

    vout = LOC_DECODE_VOUT((uint32_t) Sprite->Surface);

    R_WM_SpriteDisable(vout, wm_sprite);

    Sprite->Status = FW_VOVIAL_SPRITESTATUS_DISABLED;
}


/***********************************************************
  Function: FW_VOVIAL_MoveSprite
*/
void FW_VOVIAL_MoveSprite    (   fw_vovial_Sprite_t  *Sprite,
                                 uint32_t            PosX,
                                 uint32_t            PosY,
                                 uint32_t            PosZ
                             )
{
    uint32_t vout;
    r_wm_Sprite_t *wm_sprite = (r_wm_Sprite_t*) Sprite->Intern;

    vout = LOC_DECODE_VOUT((uint32_t) Sprite->Surface);

    R_WM_SpriteMove(vout, wm_sprite, PosX, PosY, PosZ);

    Sprite->PosX = PosX;
    Sprite->PosY = PosY;
    Sprite->PosZ = PosZ;
}


/***********************************************************
  Function: FW_VOVIAL_SetBufferAddr
*/
int32_t FW_VOVIAL_SetBufferAddr(void *Id, int32_t BufNum, void **Addresses)
{
    int32_t i;
    uint32_t i_vout;
    uint32_t i_win;
    r_wm_Window_t* win;
    r_wm_WinBuffer_t *bufs;

    i_vout = LOC_DECODE_VOUT((uint32_t)Id);
    i_win = LOC_DECODE_WIN((uint32_t)Id);

    win = &loc_Windows[(i_vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + i_win].Win;
    bufs = loc_Windows[(i_vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + i_win].Buffers;

    if(R_WM_WINBUF_ALLOC_INTERNAL == win->Surface.Fb.BufMode)
    {
        return -1;
    }

    for(i = 0; i < BufNum; ++i)
    {

        bufs[i].Data = Addresses[i];
        bufs[i].Status = R_WM_WINBUF_FREE;
    }

    R_WM_WindowExternalBufSet(i_vout, win, bufs, BufNum, win->ColorFmt);

    return 0;
}


/***********************************************************
  Function: FW_VOVIAL_SetBufferAddr
*/
int32_t FW_VOVIAL_SetClut(void *Id, uint32_t Size, void *Data)
{
    uint32_t i_vout;
    uint32_t i_win;
    r_wm_Window_t* win;
    
    i_vout = LOC_DECODE_VOUT((uint32_t)Id);
    i_win = LOC_DECODE_WIN((uint32_t)Id);

    win = &loc_Windows[(i_vout * LOC_MAX_SURFACE_NUM_PER_VOUT) + i_win].Win;

    R_WM_WindowClutSet(i_vout, win, Size, (r_wm_ClutEntry_t*)Data);

    return 0;
}


/***********************************************************
  Function: FW_VOVIAL_SetSpriteBuffer
*/
void FW_VOVIAL_SetSpriteBuffer(fw_vovial_Sprite_t *Sprite, void *Buffer)
{
    uint32_t vout;
    r_wm_Sprite_t *wm_sprite = (r_wm_Sprite_t*) Sprite->Intern;

    vout = LOC_DECODE_VOUT((uint32_t) Sprite->Surface);

    R_WM_SpriteBufSet(vout, wm_sprite, Buffer);

    Sprite->Data = Buffer;
}


/***********************************************************
  Function: FW_VOVIAL_DeleteSprite
*/
void FW_VOVIAL_DeleteSprite(fw_vovial_Sprite_t *Sprite)
{
    uint32_t vout;
    r_wm_Sprite_t *wm_sprite = (r_wm_Sprite_t*) Sprite->Intern;

    vout = LOC_DECODE_VOUT((uint32_t) Sprite->Surface);

    R_WM_SpriteDelete(vout, wm_sprite);

    R_CDI_Free((uint32_t) wm_sprite, loc_CpuHeap_VOVIAL);

    Sprite->Intern = 0; /* Make sure that subsequent dereferencing attracts attention. */
}
