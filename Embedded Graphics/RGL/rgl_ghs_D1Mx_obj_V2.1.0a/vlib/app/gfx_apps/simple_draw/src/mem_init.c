/************************************************************************************************
PROJECT : simple_draw app
FILE    : $Id: mem_init.c 14969 2017-11-02 14:33:54Z florian.zimmermann $
============================================================================ 
DESCRIPTION
Init functions for memory
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
#include "r_config_d1x.h"
#include "r_dev_api.h"
#include "r_cdi_api.h"
#include "r_bsp_stdio_api.h"
#include "mem_init.h"
#include "davehd_driver.h"
#include "r_util_dhd.h"
#include "config.h"
#include "main.h"

#include "r_ddb_api.h"
#include "r_wm_api.h"
/*******************************************************************************
  Section: Local defines
*/
#define ALIGN_UP(ADDR,BLOCK)   ( (ADDR+(BLOCK-1)) & ~(BLOCK-1) )
#define ALIGN_DOWN(ADDR,BLOCK) ( (ADDR) & ~(BLOCK-1) )

#define LOC_VID_HP_BLOCK_SIZE  (0x400u)
#define LOC_VID_HP_BLOCK_NB    (loc_VRAM0_SIZE / LOC_VID_HP_BLOCK_SIZE)


/*******************************************************************************
  Section: Local variables
  
*/


r_cdi_Heap_t       loc_VRAM_heap;
r_cdi_Heap_t       loc_lRAM_heap;
#pragma alignvar(8) 
uint8_t            loc_lRAM_heapMem[LOC_CPU_HP_BLOCK_NB * LOC_CPU_HP_BLOCK_SIZE];
r_cdi_HpBlkIndex_t loc_lRAM_heapIdxList[LOC_CPU_HP_BLOCK_NB];

//r_cdi_HpBlkIndex_t loc_VRAM_heapIdxList[LOC_VID_HP_BLOCK_NB];
uint32_t loc_VRAM0_ADDR          = 0;
uint32_t loc_VRAM0_SIZE          = 0;

/*******************************************************************************
  Section: Local functions

*/
uint32_t loc_VOWE_ImageBuffer_Addr = 0;
uint32_t loc_VOWE_DisplayList_Addr = 0;
uint32_t loc_GPU_Memory_Addr = 0;
/*******************************************************************************
  Section: Global functions
*/

/*******************************************************************************
  Function: SetupMemManager

    See: main.h for details
*/
void loc_SetupMemManager(void)
{
    uint32_t x;
    r_dev_Device_t dev;

    dev = R_DEV_GetDev();

    /* ================================
       Get device dependant memory info
       ================================
    */
    /* D1M1(H) */
    if((dev == R_DEV_R7F701404) ||
       (dev == R_DEV_R7F701405) ||
       (dev == R_DEV_R7F701406) ||
       (dev == R_DEV_R7F701407) ||
       (dev == R_DEV_R7F701442) ||
       (dev == R_DEV_R7F701418))
    {
        loc_VRAM0_ADDR = D1M1_VRAM0;
        loc_VRAM0_SIZE = D1M_VRAM0_SIZE;
    }
    /* D1M2(H) */
    else if((dev == R_DEV_R7F701408) ||
            (dev == R_DEV_R7F701410) ||
            (dev == R_DEV_R7F701411) ||
            (dev == R_DEV_R7F701412))
    {
        loc_VRAM0_ADDR = D1M2_VRAM0;
        loc_VRAM0_SIZE = D1M_VRAM0_SIZE + D1M2_VRAM1_SIZE;
    }
    /* D1M1A */
    else if(dev == R_DEV_R7F701441 ||
            dev == R_DEV_R7F701460)
    {
        loc_VRAM0_ADDR = D1M1A_VRAM0;
        loc_VRAM0_SIZE = D1M1A_VRAM0_SIZE + D1M1A_VRAM1_SIZE;
    }
    else
    {
        loc_Error(ERR_NG);
    }
    
    /* ============================== 
       Distribute memory among macros 
       ==============================
    */
    /* You may also use the CDI together with the R_CDI_AllocAlign function to 
       reserve the reqired buffers. But as the buffers set up here are completely 
       static during runtime, we can do a fixed assignment. 
    */
    
  #if LOC_VOWE_ENABLE == 1
    /* 1st is VOWE display list. We reserve space for two display lists. */
    /* Size is set to multiple of 1024 byte because VOWE image buffer has the alignment restriction. */
    if ( loc_VRAM0_SIZE > (LOC_VOWE_DL_BUF_SIZE*2u) )
    {
        loc_VOWE_DisplayList_Addr = (loc_VRAM0_ADDR + loc_VRAM0_SIZE) - (LOC_VOWE_DL_BUF_SIZE*2u);
        loc_VRAM0_SIZE -= LOC_VOWE_DL_BUF_SIZE*2u;
        R_BSP_STDIO_Printf("VOWE DL:  0x%08X / size %d\n", loc_VOWE_DisplayList_Addr, LOC_VOWE_DL_BUF_SIZE*2u);
    }
    else
    {
        R_BSP_STDIO_Printf("Not enough memory space for VOWE display list! "
             "Please consider changing to Ringbuffer mode, to reduce the ringbuffer size or to reduce display resolution.\n");
        loc_Error(ERR_NG);
    }

    /* 2nd is VOWE image buffer. 
       Start address must be multiple of 1024 byte in case of ring buffer mode.
       Size should be 2^n. */
    if ( loc_VRAM0_SIZE > LOC_VOWE_WORK_BUFF_SIZE)
    {
        loc_VOWE_ImageBuffer_Addr = (loc_VRAM0_ADDR + loc_VRAM0_SIZE) - LOC_VOWE_WORK_BUFF_SIZE;
        loc_VRAM0_SIZE -= LOC_VOWE_WORK_BUFF_SIZE;
        R_BSP_STDIO_Printf("VOWE Buf: 0x%08X / size %d\n", loc_VOWE_ImageBuffer_Addr, LOC_VOWE_WORK_BUFF_SIZE);
    }
    else
    {
        R_BSP_STDIO_Printf("Not enough memory space for VOWE buffer! "
             "Please consider changing to Ringbuffer mode, to reduce the ringbuffer size or to reduce display resolution.\n");
        loc_Error(ERR_NG);
    }
  #endif /* LOC_VOWE_ENABLE */
    
    /* 3rd is GPU2D. GPU is happy with almost any alignment, so it comes last.*/
    if ( loc_VRAM0_SIZE > DHD_MEMORY_SIZE)
    {
        loc_GPU_Memory_Addr = (loc_VRAM0_ADDR + loc_VRAM0_SIZE) - DHD_MEMORY_SIZE;
        loc_VRAM0_SIZE -= DHD_MEMORY_SIZE;
        R_BSP_STDIO_Printf("GPU2D:    0x%08X / size %d\n", loc_GPU_Memory_Addr, DHD_MEMORY_SIZE);
    }
    else
    {
        R_BSP_STDIO_Printf("Not enough space for GPU memory! "
             "Please consider changing VOWE to Ringbuffer mode, to reduce the ringbuffer size or to reduce display resolution.\n");
        loc_Error(ERR_NG);
    }
    
    /* ============================== 
       Initialise CDI Memory Managers 
       ==============================
    */
    x = R_CDI_InitHeapManager((uint32_t)loc_lRAM_heapMem,
                              &loc_lRAM_heap,
                              loc_lRAM_heapIdxList, /* Example: Here we use the optional parameter HeapIdxList */
                              LOC_CPU_HP_BLOCK_NB,
                              LOC_CPU_HP_BLOCK_SIZE);
    if (x == 1)
    {
         loc_Error(ERR_NG);
    }

    x = R_CDI_InitHeapManager(loc_VRAM0_ADDR,
                              &loc_VRAM_heap,
                              0,                    /* Example: Here we don't use the optional parameter HeapIdxList */
                              LOC_VID_HP_BLOCK_NB,
                              LOC_VID_HP_BLOCK_SIZE);
    if (x == 1)
    {
         loc_Error(ERR_NG);
    }
    R_BSP_STDIO_Printf("VRAM CDI: 0x%08X / size %d\n", loc_VRAM0_ADDR, ALIGN_DOWN(loc_VRAM0_SIZE,LOC_VID_HP_BLOCK_SIZE));


}

