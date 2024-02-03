/************************************************************************************************
PROJECT : simple_draw
FILE    : $Id: jcua.c 15225 2017-11-21 12:01:29Z matthias.nippert $
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

#include "r_typedefs.h"        /* Renesas basic types, e.g. uint32_t */
#include "r_bsp_api.h"         /* Board support package */
#include "r_jcua_api.h"
#include "r_ddb_api.h"
#include "r_wm_api.h"
#include "r_tick_api.h"
#include "r_bsp_stdio_api.h"

#include "config.h"
#include "main.h"

/***********************************************************
  Section: Local Variables
*/
static uint32_t loc_CallbackReached;
static uint32_t loc_InterruptFactor;

#pragma alignvar(8)
static const uint8_t loc_JpegData[] = {
#include "NormalData_YUV420_240x320.TXT"
};

extern r_wm_Window_t loc_WmWindow;

/***********************************************************
  Section: Local Defines
*/
#define LOC_BYTE_PER_PIXEL  (4)

/***********************************************************
  Section: Local Functions
*/
/*******************************************************************************
    Function: SetSFMAConfig

    Fills the flash device config and command structure according to the target
    device.
 */
void loc_JpegCallbackFunction(uint32_t Unit, r_jcua_CallbackReason_t Factor, uint32_t Param)
{
    loc_CallbackReached = 1;
    loc_InterruptFactor |= Factor;
    return;
}

void loc_JcuaRun(void)
{
    r_jcua_Error_t         error;
    r_jcua_FrameBuffer_t   frame_buffer;
    r_jcua_DecodeSetting_t option;
    r_wm_Error_t           wmErr;

    /* Initialize JCUA driver. */
    error = R_JCUA_Init(LOC_JCUA_UNIT);

    /* Open the decoder. */
    if (R_JCUA_ERR_OK == error)
    {
       error = R_JCUA_DecoderOpen(LOC_JCUA_UNIT, loc_JpegCallbackFunction, 0);

       /* Setup frame buffer information. */
       frame_buffer.Address = R_WM_WindowNewDrawBufGet(LOC_WM_UNIT, &loc_WmWindow);
       frame_buffer.Size    = LOC_DISPLAY_STRIDE * LOC_DISPLAY_HEIGHT * LOC_BYTE_PER_PIXEL;
       frame_buffer.Stride  = LOC_DISPLAY_STRIDE * LOC_BYTE_PER_PIXEL;
       frame_buffer.Format  = R_JCUA_OUTPUT_FORMAT_ARGB8888;
       frame_buffer.Swap    = R_JCUA_SWAP_LONG;
       option.OptionFlag    = R_JCUA_DECODE_OPTION_NONE;
       option.ImgInfo       = R_NULL;

       /* Start JPEG decode.*/
       loc_CallbackReached = 0;
       if (R_JCUA_ERR_OK == error)
       {
           error = R_JCUA_DecoderStart(LOC_JCUA_UNIT, loc_JpegData, &frame_buffer, &option);
           if (R_JCUA_ERR_OK == error)
           {
               /* Wait decode complete. */
               while (loc_CallbackReached == 0)
               {
               };

               wmErr = R_WM_WindowSwap(LOC_WM_UNIT, &loc_WmWindow);
               if(R_WM_ERR_OK != wmErr)
               {
                  loc_Error(1);
               }
               wmErr = R_WM_FrameEndMark(LOC_WM_UNIT, 0);
               if(R_WM_ERR_OK != wmErr)
               {
                  loc_Error(1);
               }
               wmErr = R_WM_FrameWait(LOC_WM_UNIT, 0);
               if(R_WM_ERR_OK != wmErr)
               {
                  loc_Error(1);
               }

               R_TICK_WaitMS(0, 8000);
           }
           else
           {
               /* Close the decoder. */
               error = R_JCUA_DecoderClose(LOC_JCUA_UNIT);
           }
       }
       R_BSP_STDIO_Printf("Closing  JCUA\n");
       /* Close */
       error = R_JCUA_DecoderClose(LOC_JCUA_UNIT);
       
       /* De-initialize Unit 0 of JCUA driver. */
       error = R_JCUA_DeInit(LOC_JCUA_UNIT);
    }
}
