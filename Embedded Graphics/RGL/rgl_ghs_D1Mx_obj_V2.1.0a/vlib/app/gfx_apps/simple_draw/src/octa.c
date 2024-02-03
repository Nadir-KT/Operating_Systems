/************************************************************************************************
PROJECT : simple_draw
FILE    : $Id: octa.c 16240 2018-07-12 12:05:44Z shinya.tomari.wj@renesas.com $
============================================================================ 
DESCRIPTION
============================================================================
                            C O P Y R I G H T
============================================================================
                       Copyright (c) 2017
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
#include "r_octa_api.h"
#include "r_octacdb_macronix.h"
#include "main.h"

/***********************************************************
  Section: Local Variables
*/
#ifndef BSP_INIT_OCTA_RAM_RW
static uint32_t loc_srcData   [(LOC_KING_SIZE / sizeof(uint32_t))];
#endif

/***********************************************************
  Section: Local Defines
*/

#define LOC_OCTA_UNIT           (0u)

#ifndef BSP_OCTA_FLASH_CH
 #define BSP_OCTA_FLASH_CH      (0u)
#endif
#ifndef BSP_OCTA_RAM_CH
 #define BSP_OCTA_RAM_CH        (1u)
#endif
#if (BSP_OCTA_FLASH_CH == 0)
 #define LOC_OCTA_FLASH_EX_ADR  (0x60000000uL)
#else
 #define LOC_OCTA_FLASH_EX_ADR  (0x70000000uL)
#endif
#if (BSP_OCTA_RAM_CH == 0)
 #define LOC_OCTA_RAM_EX_ADR    (0x60000000uL)
#else
 #define LOC_OCTA_RAM_EX_ADR    (0x70000000uL)
#endif

/*******************************************************************************
  Section: external function protorype
*/

extern uint32_t loc_CompareData32(uint32_t a[], uint32_t b[], uint32_t length32);

/***********************************************************
  Variable: g_BlackKingBmp

  the Bitmap (external)
*/
extern const uint8_t g_BlackKingBmp[LOC_KING_SIZE];

/*******************************************************************************
    Function: loc_StartOCTA

    Opens OCTA for writing and writes a pattern to it.
    Afterwards OCTA is closed but opened for reading again, so that
    the application can use it.
 */
void* loc_StartOCTA(void)
{
#ifdef BSP_INIT_OCTA_RAM_RW
    uint32_t*           ex_adr = (uint32_t*)(LOC_OCTA_RAM_EX_ADR + 0x00000000);
    uint32_t            i;

    /* fill black king */
    for (i = 0; i < (LOC_KING_SIZE / sizeof(uint32_t)); i++)
    {
        ex_adr[i] = (uint32_t)g_BlackKingBmp[(i * 4)];
        ex_adr[i] |= ((uint32_t)g_BlackKingBmp[(i * 4) + 1] << 8);
        ex_adr[i] |= ((uint32_t)g_BlackKingBmp[(i * 4) + 2] << 16);
        ex_adr[i] |= ((uint32_t)g_BlackKingBmp[(i * 4) + 3] << 24);
    }
    
#else /* BSP_INIT_OCTA_FLASH_R || BSP_INIT_OCTA_PIN  */
    r_octa_Error_t      err = R_OCTA_ERR_OK;
    r_octa_Config_t     config;
    uint32_t            byte_adr = 0x00000000;
    uint32_t            byte_size;
    uint32_t            i;
    uint32_t*           ex_adr = (uint32_t*)(LOC_OCTA_FLASH_EX_ADR + 0x00000000);

    /* fill black king */
    for (i = 0; i < (LOC_KING_SIZE / sizeof(uint32_t)); i++)
    {
        loc_srcData[i] = (uint32_t)g_BlackKingBmp[(i * 4)];
        loc_srcData[i] |= ((uint32_t)g_BlackKingBmp[(i * 4) + 1] << 8);
        loc_srcData[i] |= ((uint32_t)g_BlackKingBmp[(i * 4) + 2] << 16);
        loc_srcData[i] |= ((uint32_t)g_BlackKingBmp[(i * 4) + 3] << 24);
    }

    /* Set open parameter */
    config.DeviceType         = R_OCTA_DEVICE_FLASH;
    config.OpeMode            = R_OCTA_MODE_EX_SPACE;
    config.DataTransferMode   = R_OCTA_MODE_DOPI;
    config.AddressMode        = R_OCTA_ADDRESS_32BIT;
    config.MemorySize         = R_OCTADB_FLASH_MEMORY_SIZE;
    config.SectorSize         = R_OCTADB_FLASH_SECTOR_SIZE;
    config.PageSize           = R_OCTADB_FLASH_PAGE_SIZE;
    config.Command            = &r_octacdb_FlashCmdTbl;
    config.RelaxSize          = 0;
    config.PreCycle           = R_OCTA_PRECYCLE_OFF;
    config.DQSDelay.EnableCnt = R_OCTADB_FLASH_DQS_EN_CNT_DOPI;
    config.DQSDelay.Delay     = 0x17;
    config.CalAddress         = 0xFFFFFFFF;

#ifdef BSP_INIT_OCTA_PIN
    err |= R_OCTA_Init(LOC_OCTA_UNIT);
    err |= R_OCTA_Open(LOC_OCTA_UNIT, BSP_OCTA_FLASH_CH, &config);
#endif

    /* check if data is already there & program only if data is missing*/
    if ((LOC_KING_SIZE / sizeof(uint32_t)) != loc_CompareData32(loc_srcData, ex_adr, (LOC_KING_SIZE / sizeof(uint32_t))))
    {
        err |= R_OCTA_Close(LOC_OCTA_UNIT, BSP_OCTA_FLASH_CH);
        
        /* Open for writing */
        config.OpeMode  = R_OCTA_MODE_MANUAL;
        err |= R_OCTA_Open(LOC_OCTA_UNIT, BSP_OCTA_FLASH_CH, &config);
        
        byte_size = sizeof(loc_srcData);
        
        /* write data to octa flash */
        err |= R_OCTA_DataProtect(LOC_OCTA_UNIT, BSP_OCTA_FLASH_CH, R_OCTA_MODE_UNPROTECT);
        
        err |= R_OCTA_DataErase(LOC_OCTA_UNIT, BSP_OCTA_FLASH_CH, byte_adr, byte_size);
        
        err |= R_OCTA_DataWrite(LOC_OCTA_UNIT, BSP_OCTA_FLASH_CH, byte_adr, (uint8_t*)loc_srcData, byte_size);

        err |= R_OCTA_Close(LOC_OCTA_UNIT, BSP_OCTA_FLASH_CH);
        
        /* open again for reading */
        config.OpeMode = R_OCTA_MODE_EX_SPACE;
        err |= R_OCTA_Open(LOC_OCTA_UNIT, BSP_OCTA_FLASH_CH, &config);
        
        if ((LOC_KING_SIZE / sizeof(uint32_t)) != loc_CompareData32(loc_srcData, ex_adr, (LOC_KING_SIZE / sizeof(uint32_t))))
        {
            err |= R_OCTA_ERR_NG;
        }
    }
    if(err != R_OCTA_ERR_OK)
    {
        loc_Error((int32_t)err);
    }
#endif
    return (void*)ex_adr;
}

/*******************************************************************************
    Function: loc_StopOCTA

    Closes and stops OCTA
 */
void loc_StopOCTA(void)
{
#ifdef BSP_INIT_OCTA_PIN
    r_octa_Error_t err = R_OCTA_ERR_OK;

    err |= R_OCTA_Close(LOC_OCTA_UNIT, BSP_OCTA_FLASH_CH);
    err |= R_OCTA_DeInit(LOC_OCTA_UNIT);

    if(err != R_OCTA_ERR_OK)
    {
        loc_Error((int32_t)err);
    }
#endif
}


