/*
****************************************************************************
PROJECT : VOWE driver
============================================================================
DESCRIPTION
VOWE(Video Output Warping Engine) support functions for d1x_scit
============================================================================
                             C O P Y R I G H T
============================================================================
                            Copyright (c) 2013 - 2019
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

/***************************************************************************
  Title: VOWE Support functions

  Implementation of the VOWE support functions for the D1x device.
*/


/***************************************************************************
  Section: Includes
*/
#include "r_config_vowe.h"
#include "r_typedefs.h"
#include "r_vowe_api.h"
#include "r_vowe_sys.h"
/* display database with display timings */
#include "r_ddb_api.h"
#include "r_vdce_api.h"
#include "r_dev_api.h"

/***************************************************************************
  Section: Local Defines
*/
#define LOC_VDCE_UNIT_0           (0u)

#define LOC_ADJUST_DISPLAY_TIMING (8u)
#define LOC_VOWE_BASE_ADDR_0      (0u)
#define LOC_VDCE_MEM_HEIGHT       (64u) /* VDCE doesn't use this value. */
#define LOC_VDCE_MEM_POSX         (0)   /* used as signed value */
#define LOC_VDCE_MEM_POSY         (0)   /* used as signed value */
#define LOC_VDCE_DISP_POSX        (0)   /* used as signed value */
#define LOC_VDCE_DISP_POSY        (0)   /* used as signed value */

/***************************************************************************
  Section: Local Variables
*/
/***************************************************************************
Variable:loc_StateVariable
This value stores the status of JCUA driver.
*/
#ifdef R_BSP_SYS_INIT_USE
R_VOWE_STATIC r_vowe_Status_t loc_StateVariable[R_VOWE_MACRO_NUM];
#else
R_VOWE_STATIC r_vowe_Status_t loc_StateVariable[R_VOWE_MACRO_NUM] = {R_VOWE_STATE_DEINIT};
#endif

/***************************************************************************
  Section: Local Functions
*/


/***************************************************************************
  Section: Global Functions
*/


/***************************************************************************
  Section: VOWE Driver Support Functions

  Comments see <VOWE Support Functions>
*/

/***********************************************************
  Function: R_JCUA_Sys_InitGlobal

  see: <r_jcua_sys.h>
*/
void R_VOWE_Sys_InitGlobal(void)
{
    uint32_t i;
    
    for (i = 0u; i < R_VOWE_MACRO_NUM; i++)
    {
        loc_StateVariable[i] = R_VOWE_STATE_DEINIT;
    }
}
/***************************************************************************
  Function: R_VOWE_Sys_Init

  see:<r_vowe_sys.h>
*/
r_vowe_Error_t R_VOWE_Sys_Init(const uint32_t Unit)
{
    r_vowe_Error_t error;
    r_vdce_Error_t vdce_result;

    /* Check unit number.*/
    if (Unit == R_VOWE_UNIT0)
    {
        /* The initialization which is peculiar to the device to use */
        /* Adjust display timing. */
        vdce_result = R_VDCE_DisplayTimingAdjust(Unit, LOC_ADJUST_DISPLAY_TIMING);
        if (vdce_result != R_VDCE_ERR_OK)
        {
            error = R_VOWE_ERR_SYS_VDCE;
        }
        else
        {
            error = R_VOWE_ERR_OK;
        }
    }
    else
    {
        error = R_VOWE_ERR_RANGE_UNIT;
    }
    return error;
}


/***************************************************************************
  Function: R_VOWE_Sys_DeInit

  see:<r_vowe_sys.h>
*/
r_vowe_Error_t R_VOWE_Sys_DeInit(const uint32_t Unit)
{
    r_vowe_Error_t error = R_VOWE_ERR_OK;

    /* Check unit number.*/
    if (Unit == R_VOWE_UNIT0)
    {
        /* The de-initialization which is peculiar to the device to use */
    }
    else
    {
        error = R_VOWE_ERR_RANGE_UNIT;
    }
    return error;
}


/***************************************************************************
  Function: R_VOWE_Sys_InterruptEnable

  see:<r_vowe_sys.h>
*/
void R_VOWE_Sys_InterruptEnable(const uint32_t Unit)
{
    /* Check unit number.*/
    if (Unit == R_VOWE_UNIT0)
    {
        /* Interrupt priority set */

        /* Interrupt enable */
        R_DEV_IntEnable(R_DEV_INT_VOWE, (uint8_t)R_TRUE);
    }
    else
    {
        /* Do nothing */
    }
}


/***************************************************************************
  Function: R_VOWE_Sys_InterruptDisable

  see:<r_vowe_sys.h>
*/
void R_VOWE_Sys_InterruptDisable(const uint32_t Unit)
{
    /* Check unit number.*/
    if (Unit == R_VOWE_UNIT0)
    {
        /* Interrupt disable */
        R_DEV_IntEnable(R_DEV_INT_VOWE, (uint8_t)R_FALSE); 
    }
    else
    {
        /* Do nothing */
    }
}


/***************************************************************************
  Function: R_VOWE_Sys_BaseAddrGet

  see:<r_vowe_sys.h>
*/
uint32_t R_VOWE_Sys_BaseAddrGet(const uint32_t Unit)
{
    uint32_t       base_addr;

    /* Check unit number.*/
    if (Unit == R_VOWE_UNIT0)
    {
        base_addr = R_VOWE_BASE_ADDR;
    }
    else
    {
        base_addr = LOC_VOWE_BASE_ADDR_0;
    }
    return base_addr;
}


/***************************************************************************
  Function: R_VOWE_Sys_ExtBaseAddrGet

  see:<r_vowe_sys.h>
*/
uint32_t R_VOWE_Sys_ExtBaseAddrGet(const uint32_t Unit)
{
    uint32_t       ext_base_addr;

    /* Check unit number.*/
    if (Unit == R_VOWE_UNIT0)
    {
        ext_base_addr = R_VOWE_EXT_BASE_ADDR;
    }
    else
    {
        ext_base_addr = LOC_VOWE_BASE_ADDR_0;
    }
    return ext_base_addr;
}


/***************************************************************************
  Function: R_VOWE_Sys_VDCEnable

  see:<r_vowe_sys.h>
*/
r_vowe_Error_t R_VOWE_Sys_VDCEnable(const uint32_t             Unit,
                                    const r_vowe_BufferMode_t  WorkBufferMode,
                                    const uint8_t*       const VdceBufferAddr,
                                    const uint32_t             VdceBufferSize,
                                    const uint32_t             Stride,
                                    const r_vowe_ColorFormat_t ColorFormat,
                                    const uint32_t             SourceWidth,
                                    const uint32_t             SourceHeight,
                                    const uint32_t             RingBufferDelay,
                                    const r_vowe_DestMode_t    DestMode)
{
    uint32_t          ring_buffer_height;
    int32_t           pixel_size;
    r_vdce_Format_t   vdce_format;
    r_vdce_OirMode_t  vdce_oir_mode;
    r_vowe_Error_t    error = R_VOWE_ERR_OK;
    r_vdce_Error_t    vdce_result;

    /* Check unit number.*/
    if (Unit == R_VOWE_UNIT0)
    {
        /* Set address with typecast to adjust VDCE I/F */
        vdce_result = R_VDCE_OirBaseSet(Unit, (uint32_t)VdceBufferAddr); /* PRQA S 306 */
        if (vdce_result == R_VDCE_ERR_OK)
        {
            if (ColorFormat == R_VOWE_FORMAT_RGB565)
            {
                vdce_format = R_VDCE_RGB565;
                pixel_size = R_VOWE_SIZE_OF_RGB565;
            }
            else
            {
                vdce_format = R_VDCE_ARGB8888;
                pixel_size = R_VOWE_SIZE_OF_ARGB8888;
            }

            if (DestMode == R_VOWE_DEST_MODE_SCREEN_SHOTS)
            {
                vdce_oir_mode = R_VDCE_OIR_MODE_SCREEN_SHOTS;
            }
            else
            {
                vdce_oir_mode = R_VDCE_OIR_MODE_NORMAL;
            }
            /* Select buffer mode */
            if (WorkBufferMode == R_VOWE_FRAME_BUFFER_MODE)
            {
                /* Setup the Frame Buffer Mode. */
                vdce_result = R_VDCE_OirRingBufferDisable(Unit);
            }
            else
            {
                /* Setup the Ring Buffer Mode. */
                ring_buffer_height = VdceBufferSize / Stride / (uint32_t)pixel_size;
                vdce_result = R_VDCE_OirRingBufferEnable(Unit, ring_buffer_height);
                if (vdce_result == R_VDCE_ERR_OK)
                {
                    vdce_result = R_VDCE_OirVSyncDelaySet(Unit, RingBufferDelay);
                }
                else
                {
                    /* Do nothing */
                }
            }
            if (vdce_result == R_VDCE_ERR_OK)
            {
                /* Setup the memory size */
                vdce_result = R_VDCE_OirMemGeometrySet(Unit, Stride, LOC_VDCE_MEM_HEIGHT);
                if (vdce_result == R_VDCE_ERR_OK)
                {
                    vdce_result = R_VDCE_OirViewPortSet(
                                        Unit, 
                                        LOC_VDCE_MEM_POSX, LOC_VDCE_MEM_POSY, 
                                        LOC_VDCE_DISP_POSX, LOC_VDCE_DISP_POSY,
                                        (int32_t)SourceWidth,
                                        (((int32_t)SourceHeight) + R_VOWE_SOURCE_HEIGHT_MARGIN));
                    if (vdce_result == R_VDCE_ERR_OK)
                    {
                        vdce_result = R_VDCE_OirFormatSet(Unit, vdce_format);
                        if (R_VDCE_ERR_OK == vdce_result)
                        {
                            vdce_result = R_VDCE_OirModeSet(Unit, vdce_oir_mode);
                            if (vdce_result == R_VDCE_ERR_OK)
                            {
                                vdce_result = R_VDCE_OirEnable(Unit);
                            }
                            /* R_VDCE_OirModeSet */
                            else
                            {
                                /* Do nothing */
                            }
                        }
                        /* R_VDCE_OirFormatSet */
                        else
                        {
                            /* Do nothing */
                        }
                    }
                    /* R_VDCE_OirViewPortSet */
                    else
                    {
                        /* Do nothing */
                    }
                }
                /* R_VDCE_OirMemGeometrySet */
                else
                {
                    /* Do nothing */
                }
            }
            /* Select buffer mode */
            else
            {
                /* Do nothing */
            }
        }
        /* R_VDCE_OirBaseSet */
        else
        {
            /* Do nothing */
        }
        if (vdce_result != R_VDCE_ERR_OK)
        {
            error = R_VOWE_ERR_SYS_VDCE;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        error = R_VOWE_ERR_RANGE_UNIT;
    }
    return error;
}


/***************************************************************************
  Function: R_VOWE_Sys_VDCDisable

  see:<r_vowe_sys.h>
*/
r_vowe_Error_t R_VOWE_Sys_VDCDisable(const uint32_t Unit)
{
    r_vowe_Error_t error;
    r_vdce_Error_t vdce_result;

    /* Check unit number.*/
    if (Unit == R_VOWE_UNIT0)
    {
        vdce_result = R_VDCE_OirDisable(LOC_VDCE_UNIT_0);
        if (vdce_result != R_VDCE_ERR_OK)
        {
            error = R_VOWE_ERR_SYS_VDCE;
        }
        else
        {
            error = R_VOWE_ERR_OK;
        }
    }
    else
    {
        error = R_VOWE_ERR_RANGE_UNIT;
    }
    return error;
}

/***************************************************************************
  Function: R_VOWE_Sys_StateSet

  see:<r_vowe_sys.h>
*/
r_vowe_Error_t R_VOWE_Sys_StateSet(const uint32_t       Unit,
                                   const r_vowe_Status_t State)
{
    r_vowe_Error_t error = R_VOWE_ERR_OK;

    if (Unit < R_VOWE_MACRO_NUM)
    {
        loc_StateVariable[Unit] = State;
    }
    else
    {
        error = R_VOWE_ERR_RANGE_UNIT;
    }
    return error;
}

/***************************************************************************
  Function: R_VOWE_Sys_StateGet

  see:<r_vowe_sys.h>
*/
r_vowe_Status_t R_VOWE_Sys_StateGet(const uint32_t Unit)
{
    r_vowe_Status_t state;

    if (Unit < R_VOWE_MACRO_NUM)
    {
        state = loc_StateVariable[Unit];
    }
    else
    {
        state = R_VOWE_STATE_DEINIT;
    }
    return state;
}


/***************************************************************************
  Group: VOWE OS interface
*/

/***************************************************************************
  Function: R_VOWE_Sys_Lock

  see:<r_vowe_sys.h>
*/
r_vowe_Error_t R_VOWE_Sys_Lock(const uint32_t Unit)
{
    r_vowe_Error_t error = R_VOWE_ERR_OK;

    /* Check unit number.*/
    if (Unit == R_VOWE_UNIT0)
    {
        /* Please implement the lock process by mutex or semaphore */
        /* if VOWE API is called from multi-thread. */
    }
    else
    {
        error = R_VOWE_ERR_RANGE_UNIT;
    }
    return error;
}


/***************************************************************************
  Function: R_VOWE_Sys_Unlock

  see:<r_vowe_sys.h>
*/
r_vowe_Error_t R_VOWE_Sys_Unlock(const uint32_t Unit)
{
    r_vowe_Error_t error = R_VOWE_ERR_OK;

    /* Check unit number.*/
    if (Unit == R_VOWE_UNIT0)
    {
        /* Please implement the unlock process depending on R_VOWE_Sys_Lock. */
    }
    else
    {
        error = R_VOWE_ERR_RANGE_UNIT;
    }
    return error;
}


