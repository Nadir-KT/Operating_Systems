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
#include "main.h"

#include <strings.h>
/*******************************************************************************
  Section: Local defines
*/
#define WM_MSG_QUEUE_LEN 32


/*******************************************************************************
  Section: Local variables
  
*/
static r_wm_Msg_t wm_msg_queue[WM_MSG_QUEUE_LEN];


extern r_wm_Discom_t loc_Discom;
extern uint32_t      loc_DiscomCrcVal;
extern uint32_t      loc_DiscomMismatch;
extern uint32_t      loc_VocaMismatch;

/*******************************************************************************
  Section: Local functions

*/

static void loc_WM_ErrorCb(uint32_t Unit, r_wm_Error_t Error)
{
    loc_Error(1);
}

static void loc_WM_EventCb(uint32_t Unit, const r_wm_Event_t* Event)
{
    if (Event->Id == R_WM_EVENT_DISCOM_MISMACTH)
    {
        R_WM_DiscomCrcGet(LOC_WM_UNIT, &loc_Discom, &loc_DiscomCrcVal);
        loc_DiscomMismatch++;
    }
#if LOC_VOCA_ENABLE == 1 
    else if (Event->Id == R_WM_EVENT_VOCA_MISMACTH)
    {
        loc_VocaMismatch++;
    }
#endif
}

/*******************************************************************************
  Function: loc_InitWM

    See: main.h for details
*/

void loc_InitWM(r_cdi_Heap_t *Cpu, r_cdi_Heap_t *Vid)
{
    r_wm_Error_t wmErr;
    wmErr = R_WM_DevInit(LOC_WM_UNIT, 
                         wm_msg_queue, 
                         WM_MSG_QUEUE_LEN, 
                         &loc_WM_EventCb, 
                         (void*)Cpu, 
                         (void*)Vid);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }

    wmErr = R_WM_ErrorCallbackSet(LOC_WM_UNIT, &loc_WM_ErrorCb);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }

    wmErr = R_WM_DevEventRegister(LOC_WM_UNIT, R_WM_EVENT_DISCOM_MISMACTH, 0);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }

#if LOC_VOCA_ENABLE == 1 
    wmErr = R_WM_DevEventRegister(LOC_WM_UNIT, R_WM_EVENT_VOCA_MISMACTH, 0);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
#endif
    
    wmErr = R_WM_ScreenTimingSetByName(LOC_WM_UNIT, (const int8_t*)LOC_DISPLAY_NAME);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }

    wmErr = R_WM_ScreenColorFormatSet(LOC_WM_UNIT, (R_WM_OUTCOLORFMT_RGB888));
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }

    wmErr = R_WM_ScreenEnable(LOC_WM_UNIT);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
    R_WM_ScreenBgColorSet(LOC_WM_UNIT, 0, 0, 0);
    if(R_WM_ERR_OK != wmErr)
    {
        loc_Error(1);
    }
}

void *loc_CreateWindow(r_wm_WinMode_t      Mode,
                          uint32_t            PosX,
                          uint32_t            PosY,
                          uint32_t            PosZ,
                          uint32_t            Width,
                          uint32_t            Height,
                          uint32_t            Pitch,
                          r_wm_WinColorFmt_t  ColorFmt,
                          r_wm_Window_t*      Window)
{
    r_wm_Error_t wmErr;

    /* clear */
    memset(Window, 0, sizeof(r_wm_Window_t));

    Window->Mode      = Mode;
    Window->Status    = R_WM_WINSTATUS_NOT_INITIALIZED;
    Window->ColorFmt  = ColorFmt;
    Window->Alpha     = 0xff;
    Window->PosX      = PosX;
    Window->PosY      = PosY;
    Window->PosZ      = PosZ;
    Window->Width     = Width;
    Window->Height    = Height;
    Window->Pitch     = Pitch;

    if(Mode == R_WM_WINMODE_FB)
    {
        
        Window->Surface.Fb.BufNum    = 2;
        Window->Surface.Fb.Buffer    = 0;
        Window->Surface.Fb.BufMode   = R_WM_WINBUF_ALLOC_INTERNAL;
    }

    wmErr = R_WM_WindowCreate(LOC_WM_UNIT, Window);
    if(R_WM_ERR_OK != wmErr)
    {
        if (R_WM_ERR_MALLOC_FAILED == wmErr)
        {
            R_BSP_STDIO_Printf("Not enough space for Window Framebuffers! "
                "Please consider changing VOWE to Ringbuffer mode to reduce the ringbuffer size or reduce display resolution.\n");
        }
        loc_Error(1);
    }
    wmErr = R_WM_WindowEnable(LOC_WM_UNIT, Window);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }

    return Window;
}
