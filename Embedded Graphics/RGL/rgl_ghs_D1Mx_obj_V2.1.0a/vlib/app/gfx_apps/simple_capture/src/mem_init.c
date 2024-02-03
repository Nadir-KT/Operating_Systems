/************************************************************************************************
PROJECT : simple_draw app
FILE    : $Id: mem_init.c 16855 2019-08-06 01:12:45Z shinya.tomari.wj@renesas.com $
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
#include "r_bsp_api.h"
#include "r_cdi_api.h"
#include "mem_init.h"
#include "config.h"

#include "r_ddb_api.h"
#include "r_wm_api.h"
#include "r_wm_sys.h"

#define LOC_VID_HP_BLOCK_SIZE  (1024u)
#define LOC_VID_HP_BLOCK_NB    (loc_VRAM0_SIZE / LOC_VID_HP_BLOCK_SIZE)
#define ALIGN(ADDR,BLOCK)      ( (ADDR+(BLOCK-1)) & ~(BLOCK-1) )
#define VRAM_HEAP_BASE         ALIGN((loc_VRAM0), LOC_VID_HP_BLOCK_SIZE)

r_cdi_Heap_t       loc_VRAM_heap;
r_cdi_Heap_t       loc_lRAM_heap;
#pragma alignvar(8)
uint8_t            loc_lRAM_heapMem[LOC_CPU_HP_BLOCK_NB * LOC_CPU_HP_BLOCK_SIZE];
r_cdi_HpBlkIndex_t loc_lRAM_heapIdxList[LOC_CPU_HP_BLOCK_NB];
//r_cdi_HpBlkIndex_t loc_VRAM_heapIdxList[LOC_VID_HP_BLOCK_NB];
uint32_t loc_VRAM0               = 0;
uint32_t loc_VRAM0_SIZE          = 0;

uint32_t loc_ContensAddr         = 0;
uint32_t loc_ContensSize         = 0;

/*******************************************************************************
  Section: Local functions

*/

/*******************************************************************************
  Section: Global functions
*/

/*******************************************************************************
  Function: SetupMemManager

    See: main.h for details
*/
void loc_SetupMemManager(uint32_t *WmUnit, uint32_t *CaptUnit)
{
    uint32_t x;
    r_dev_Device_t dev;

    dev = R_DEV_GetDev();

    /* D1M1(H), D1M1-V2 */
    if((dev == R_DEV_R7F701404) ||
       (dev == R_DEV_R7F701405) ||
       (dev == R_DEV_R7F701406) ||
       (dev == R_DEV_R7F701407) ||
       (dev == R_DEV_R7F701442) ||
       (dev == R_DEV_R7F701418))
    {
        R_BSP_MemoryAddressAndSize(R_BSP_MEM_VRAM, &loc_VRAM0, &loc_VRAM0_SIZE);
        /* Reduce for Contents buffer */
        loc_ContensAddr  = loc_VRAM0 + loc_VRAM0_SIZE - (LOC_BMP_SIZE * 4);
        loc_VRAM0_SIZE  -= (LOC_BMP_SIZE * 4);
        
        *WmUnit = 0;
        *CaptUnit = 0;
        
      #if LOC_CAPTURE_SYNCHRONIZED == 0
        /* Triple Buffering: VRAM will be not enough, change to SDRAM */
        if (0 == (R_BSP_Capabilities() & R_BSP_HAS_SDRAM))
        {
        }
        else
        {
            R_BSP_MemoryAddressAndSize(R_BSP_MEM_SDRAM, &loc_VRAM0, &loc_VRAM0_SIZE);
            R_BSP_MemoryAddressAndSize(R_BSP_MEM_VRAM, &loc_ContensAddr, &loc_ContensSize);
        }
      #endif
    }
    /* D1M1A */
    else if((dev == R_DEV_R7F701441) ||
            (dev == R_DEV_R7F701460))
    {
      #ifndef USE_HDMI0_INTERFACE
        *WmUnit = 0;    /* Also for LVTTL we use Vo0, as D1M1A's Vo1 is needs some HW preparation*/
      #else
        *WmUnit = 0;
      #endif
        *CaptUnit = 0;

      #if LOC_CAPTURE_SYNCHRONIZED == 0
        /* Triple Buffering: VRAM will be not enough */
        R_BSP_MemoryAddressAndSize(R_BSP_MEM_SDRAM, &loc_VRAM0, &loc_VRAM0_SIZE);
        R_BSP_MemoryAddressAndSize(R_BSP_MEM_VRAM, &loc_ContensAddr, &loc_ContensSize);
      #else
        R_BSP_MemoryAddressAndSize(R_BSP_MEM_VRAM, &loc_VRAM0, &loc_VRAM0_SIZE);
        R_BSP_MemoryAddressAndSize(R_BSP_MEM_SDRAM, &loc_ContensAddr, &loc_ContensSize);
      #endif
    }
    /* D1M2(H) */
    else if((dev == R_DEV_R7F701408) ||
            (dev == R_DEV_R7F701410) ||
            (dev == R_DEV_R7F701411) ||
            (dev == R_DEV_R7F701412))
    {
        R_BSP_MemoryAddressAndSize(R_BSP_MEM_VRAM, &loc_VRAM0, &loc_VRAM0_SIZE);
        R_BSP_MemoryAddressAndSize(R_BSP_MEM_SDRAM, &loc_ContensAddr, &loc_ContensSize);

    #ifndef USE_HDMI0_INTERFACE
        *WmUnit = 1;
    #else
        *WmUnit = 0;
    #endif
        *CaptUnit = 1;
    }
    else
    {
        /* Unsupported device */
        while(1);
    }

    x = R_CDI_InitHeapManager((uint32_t)loc_lRAM_heapMem,
                              &loc_lRAM_heap,
                              loc_lRAM_heapIdxList,
                              LOC_CPU_HP_BLOCK_NB,
                              LOC_CPU_HP_BLOCK_SIZE);
    if (x == 1)
    {
         while(1);
    }

    x = R_CDI_InitHeapManager(VRAM_HEAP_BASE,
                              &loc_VRAM_heap,
                              0,
                              LOC_VID_HP_BLOCK_NB,
                              LOC_VID_HP_BLOCK_SIZE);
    if (x == 1)
    {
         while(1);
    }
}




