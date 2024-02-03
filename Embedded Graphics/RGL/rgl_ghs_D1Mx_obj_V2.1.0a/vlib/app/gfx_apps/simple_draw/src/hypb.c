/************************************************************************************************
PROJECT : simple_draw
FILE    : $Id: hypb.c 16240 2018-07-12 12:05:44Z shinya.tomari.wj@renesas.com $
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
#include "r_hypb_api.h"
#include "r_hypdb_issi.h"
#include "main.h"

/***********************************************************
  Section: Local Variables
*/
#ifndef BSP_INIT_HYPB_RAM_RW
static uint32_t loc_srcData   [LOC_KING_SIZE / sizeof(uint32_t)];
static uint32_t loc_verifyData[LOC_KING_SIZE / sizeof(uint32_t)];
#endif

/***********************************************************
  Section: Local Defines
*/

#define LOC_HYPB_UNIT         (0u)
#define LOC_HYPB_EX_ADR       (0x60000000uL)

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
    Function: loc_StartHYPB

    Opens HYPB for writing and writes a pattern to it.
    Afterwards HYPB is closed but opened for reading again, so that
    the application can use it.
 */
void* loc_StartHYPB(void)
{
#ifdef BSP_INIT_HYPB_RAM_RW
    /*************/
    /* Hyper RAM */
    /*************/
    uint32_t*           ex_adr = (uint32_t*)(LOC_HYPB_EX_ADR + 0x00000000);
    uint32_t            i;

    /* fill black king */
    for (i = 0; i < (LOC_KING_SIZE / sizeof(uint32_t)); i++)
    {
        ex_adr[i]  = (uint32_t)g_BlackKingBmp[(i * 4)];
        ex_adr[i] |= ((uint32_t)g_BlackKingBmp[(i * 4) + 1] << 8);
        ex_adr[i] |= ((uint32_t)g_BlackKingBmp[(i * 4) + 2] << 16);
        ex_adr[i] |= ((uint32_t)g_BlackKingBmp[(i * 4) + 3] << 24);
    }
    
#else /* BSP_INIT_HYPB_FLASH_R || BSP_INIT_HYPB_PIN  */
    /***************/
    /* Hyper Flash */
    /***************/
    r_hypb_Error_t      err = R_HYPB_ERR_OK;
    r_hypb_Config_t     config;
    uint32_t            word_adr = 0x00000000;
    uint32_t            word_size;
    uint32_t            i;
    uint32_t*           ex_adr = (uint32_t*)(LOC_HYPB_EX_ADR + word_adr);

    /* fill black king */
    for (i = 0; i < (LOC_KING_SIZE / sizeof(uint32_t)); i++)
    {
        loc_srcData[i] = (uint32_t)g_BlackKingBmp[(i * 4)];
        loc_srcData[i] |= ((uint32_t)g_BlackKingBmp[(i * 4) + 1] << 8);
        loc_srcData[i] |= ((uint32_t)g_BlackKingBmp[(i * 4) + 2] << 16);
        loc_srcData[i] |= ((uint32_t)g_BlackKingBmp[(i * 4) + 3] << 24);
    }

#ifdef BSP_INIT_HYPB_FLASH_R
    err |= R_HYPB_Close(LOC_HYPB_UNIT);
#else /* BSP_INIT_HYPB_PIN */
    err |= R_HYPB_Init(LOC_HYPB_UNIT);
#endif
    /* open for writing */
    config.DeviceType    = R_HYPB_DEVICE_FLASH;
    config.OpeMode       = R_HYPB_MODE_MANUAL;
    config.MemorySize    = R_HYPDB_FLASH_MEMORY_SIZE;
    config.SectorSize    = R_HYPDB_FLASH_SECTOR_SIZE;
    config.WriteAlign    = R_HYPDB_FLASH_WRITE_BUF_SIZE;
    config.RelaxSize     = 0;
    config.ParamSec.Size = 0;
    config.ParamSec.Num  = 0;
    config.ParamSec.Pos  = R_HYPB_PARAM_SEC_TOP;
    config.Command       = &r_hypdb_FlashCmdTbl;
    
    err |= R_HYPB_Open(LOC_HYPB_UNIT, &config);

    /* check if data is already there & program only if data is missing*/
    word_size = sizeof(loc_srcData) / 2;
    err |= R_HYPB_DataRead(LOC_HYPB_UNIT, word_adr, (uint8_t*)loc_verifyData, word_size);
    
    if ((LOC_KING_SIZE / sizeof(uint32_t)) != loc_CompareData32(loc_srcData, loc_verifyData, (LOC_KING_SIZE / sizeof(uint32_t))))
    {
        /* write data to hyper flash */
        err |= R_HYPB_DataProtect(LOC_HYPB_UNIT, word_adr, word_size, R_HYPB_MODE_UNPROTECT);
        
        err |= R_HYPB_DataErase(LOC_HYPB_UNIT, word_adr, word_size, R_NULL);
        
        err |= R_HYPB_DataWrite(LOC_HYPB_UNIT, word_adr, (uint8_t*)loc_srcData, word_size, R_NULL);
       
        /* verify the data is correct */
        err |= R_HYPB_DataRead(LOC_HYPB_UNIT, word_adr, (uint8_t*)loc_verifyData, word_size);
        if ((LOC_KING_SIZE / sizeof(uint32_t)) != loc_CompareData32(loc_srcData, loc_verifyData, (LOC_KING_SIZE / sizeof(uint32_t))))
        {
            err |= R_HYPB_ERR_NG;
        }
    }
    err |= R_HYPB_Close(LOC_HYPB_UNIT);

    /* open again for reading */
    config.OpeMode       = R_HYPB_MODE_EX_SPACE;
    err |= R_HYPB_Open(LOC_HYPB_UNIT, &config);

    if(err != R_HYPB_ERR_OK)
    {
        loc_Error((int32_t)err);
    }
#endif /* BSP_INIT_HYPB_RAM_RW */
    
    return (void *)ex_adr;
}

/*******************************************************************************
    Function: loc_StopHYPB

    Closes and stops HYPB
 */
void loc_StopHYPB(void)
{
#ifdef BSP_INIT_HYPB_PIN
    r_hypb_Error_t err = R_HYPB_ERR_OK;

    err |= R_HYPB_Close(LOC_HYPB_UNIT);
    err |= R_HYPB_DeInit(LOC_HYPB_UNIT);

    if(err != R_HYPB_ERR_OK)
    {
        loc_Error((int32_t)err);
    }
#endif
}


