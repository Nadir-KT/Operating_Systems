/************************************************************************************************
PROJECT : simple_draw app
FILE    : $Id: wm.c 17203 2020-06-03 02:39:46Z shinya.tomari.wj@renesas.com $
============================================================================ 
DESCRIPTION
============================================================================
                            C O P Y R I G H T
============================================================================
                       Copyright (c) 2013 - 2014
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
#include "r_typedefs.h"     /* Renesas basic types, e.g. uint32_t */
#include "r_ddb_api.h"
#include "r_cdi_api.h"
#include "r_wm_api.h"

#include "r_bsp_stdio_api.h"

#include "config.h"
#include "wm.h"

#include <strings.h>

/*******************************************************************************
  Section: Local defines
*/
#define WM_MSG_QUEUE_LEN 32


/*******************************************************************************
  Section: Local variables
  
*/
static r_wm_Msg_t loc_WmMsgQueue[2][WM_MSG_QUEUE_LEN];

extern r_cdi_Heap_t loc_lRAM_heap;
extern r_cdi_Heap_t loc_VRAM_heap;

/*******************************************************************************
  Section: Local functions

*/
uint32_t vo_vblank_cnt = 0;
uint32_t vi_vblank_cnt = 0;

static void loc_WM_EventHandler(uint32_t Unit, const r_wm_Event_t* Event)
{
    switch (Event->Id)
    {
    case R_WM_EVENT_VBLANK:
        vo_vblank_cnt++;
        break;
    case R_WM_EVENT_VI_VBLANK:
        vi_vblank_cnt++;
        break;
    }
}

static void loc_WM_ErrorHandler(uint32_t Unit, r_wm_Error_t Error)
{
    R_BSP_STDIO_Printf("Error in WM%d: %d\n", Unit, Error);
    while (Error != 0) { /* Forever */ };
}

/*******************************************************************************
  Function: loc_InitWM

    See: main.h for details
*/

#ifdef LOC_VIN_TEST
void loc_InitVinWmUnit(void)
{
    R_WM_ErrorCallbackSet(LOC_CAPTURE_OUT_WM_UNIT, &loc_WM_ErrorHandler);
    
    R_WM_DevInit(LOC_CAPTURE_OUT_WM_UNIT, wm_msg_queue, WM_MSG_QUEUE_LEN, 0);
    R_WM_ScreenTimingSetByName(LOC_CAPTURE_OUT_WM_UNIT, (const int8_t*)LOC_DISPLAY_NAME);
    R_WM_ScreenBgColorSet(LOC_CAPTURE_OUT_WM_UNIT, 0, 0, 0);
    R_WM_ScreenEnable(LOC_CAPTURE_OUT_WM_UNIT);
}
#endif

/*******************************************************************************
  Function: loc_InitWM

    See: main.h for details
*/

void loc_InitWmUnit(uint32_t Unit, const int8_t* DisplayName)
{
    R_WM_DevInit(Unit, loc_WmMsgQueue[Unit], WM_MSG_QUEUE_LEN, &loc_WM_EventHandler, &loc_lRAM_heap, &loc_VRAM_heap);
    R_WM_ErrorCallbackSet(Unit, &loc_WM_ErrorHandler);
    R_WM_ScreenTimingSetByName(Unit, (const int8_t*)DisplayName);    
    R_WM_ScreenEnable(Unit);
    R_WM_ScreenBgColorSet(Unit, 0, 0, 0);
    
    R_BSP_STDIO_Printf("VOUT%u: Connect display '%s'\n", Unit, DisplayName);
}

void *loc_CreateWindow  (   uint32_t            Unit,
                            r_wm_WinMode_t      Mode,
                            uint32_t            PosX,
                            uint32_t            PosY,
                            uint32_t            PosZ,
                            uint32_t            Width,
                            uint32_t            Height,
                            uint32_t            Pitch,
                            uint32_t            ScaledWidth,
                            uint32_t            ScaledHeight,
                            r_wm_WinColorFmt_t  ColorFmt,
                            r_wm_Window_t*      Window
                        )
{
    /* clear */
    memset(Window, 0, sizeof(r_wm_Window_t));

    Window->Mode      = Mode;
    Window->ColorFmt  = ColorFmt;
    Window->Alpha     = 0xff;
    Window->PosX      = PosX;
    Window->PosY      = PosY;
    Window->PosZ      = PosZ;
    Window->Width     = Width;
    Window->Height    = Height;
    Window->Pitch     = Pitch;
    Window->ScaledWidth  = ScaledWidth;
    Window->ScaledHeight = ScaledHeight;

    if(Mode == R_WM_WINMODE_FB)
    {
#if ((LOC_CAPTURE_SYNCHRONIZED == 1) || (LOC_DEINTERLACE_MODE == LOC_DEINTERLACE_WEAVE))
        Window->Surface.Fb.BufNum    = 1;
#else
        Window->Surface.Fb.BufNum    = 3;
#endif
        Window->Surface.Fb.Buffer    = 0;
        Window->Surface.Fb.BufMode   = R_WM_WINBUF_ALLOC_INTERNAL;
    }

    R_WM_WindowCreate(Unit, Window);
    R_WM_WindowEnable(Unit, Window);
    return Window;
}
