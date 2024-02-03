/************************************************************************************************
PROJECT : simple_draw
FILE    : $Id: sfma.c 16855 2019-08-06 01:12:45Z shinya.tomari.wj@renesas.com $
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
#include "r_bsp_stdio_api.h"   /* R_BSP_STDIO_Printf */
#include "r_sfma_api.h"
#include "r_sfcdb_macronix.h"

#include "config.h"
#include "main.h"

/***********************************************************
  Section: Local Variables
*/
static uint32_t loc_srcData   [(LOC_KING_SIZE / sizeof(uint32_t))];
static uint32_t loc_verifyData[(LOC_KING_SIZE / sizeof(uint32_t))];

/***********************************************************
  Section: Local Defines
*/
#define LOC_SFMA_SECTOR_SIZE  (4096u)
#if (LOC_SFMA_UNIT == 0)
#define LOC_SF_ADDRESS_EX     (0x10000000)
#else
#define LOC_SF_ADDRESS_EX     (0xC0000000)
#endif

/***************************************************************************
  Constant: SF_DEFAULT_CAL

  Default calibration for Serial flash memory (Macronix device only).

    SPBCLK clock phase shift        3 (range 0 - 7, written to the upper 16bits)
    Sampling point phase shift      1 (range 0 - 7, written to the Lower 16 bits)
*/
#define SF_SPBCLK_CLOCK_PHASE_SHIFT    (3u)
#if defined(BSP_USE_SFMA2)
 #define SF_SAMPLING_POINT_PHASE_SHIFT  (0u)
#else
 #define SF_SAMPLING_POINT_PHASE_SHIFT  (1u)
#endif
#define LOC_SFMA_DEFAULT_CAL    ((SF_SPBCLK_CLOCK_PHASE_SHIFT << 16) | SF_SAMPLING_POINT_PHASE_SHIFT)

/***********************************************************
  Variable: g_BlackKingBmp

  the Bitmap (external)
*/
extern const uint8_t g_BlackKingBmp[LOC_KING_SIZE];

/***********************************************************
  Section: Local Functions
*/
static void loc_SFMAConfig(r_sfma_Config_t *Config, r_sfma_Mode_t Mode, r_sfma_DataTransferMode_t Transfer);
/*******************************************************************************
    Function: SetSFMAConfig

    Fills the flash device config and command structure according to the target
    device.
 */

static void loc_SFMAConfig(r_sfma_Config_t *Config, r_sfma_Mode_t Mode, r_sfma_DataTransferMode_t Transfer)
{
    if(Config != R_NULL)
    {
        Config->Mode                        = Mode;
        Config->DataTransferMode            = Transfer;
        Config->AddressMode                 = R_SFMA_ADDRESS_32BIT;
        Config->SerialFlashMemoryPageSize   = 0x100uL; /* 256Byte */
        Config->CacheMode                   = R_SFMA_CACHE_OFF;
        Config->PerformanceEnMode           = R_SFMA_PER_EN_MODE_DISABLE;
        Config->Calibration                 = LOC_SFMA_DEFAULT_CAL;
        
#if defined(BSP_USE_SFMA2) && (LOC_SFMA_UNIT != 1)
        Config->MemoryNum                   = R_SFMA_MEMORY_SINGLE;
#else
        Config->MemoryNum                   = R_SFMA_MEMORY_DUAL;
#endif
#if (LOC_SFMA_UNIT == 1)
        Config->SerialFlashMemoryMaxClock   = (40*1000*1000);
#else
        Config->SerialFlashMemoryMaxClock   = (80*1000*1000);
#endif
        
        if(R_SFMA_MEMORY_DUAL == Config->MemoryNum)
        {
            Config->SerialFlashMemorySectorSize = LOC_SFMA_SECTOR_SIZE * 2;
            Config->SerialFlashMemorySize       = 128*1024*1024;
        }
        else
        {
            Config->SerialFlashMemorySectorSize = LOC_SFMA_SECTOR_SIZE;
            Config->SerialFlashMemorySize       = 64*1024*1024;
        }

        if (R_SFMA_ADDRESS_32BIT == Config->AddressMode)
        {
            if ((40*1000*1000) < Config->SerialFlashMemoryMaxClock)
            {
                if ((Config->DataTransferMode == R_SFMA_SDR_QUAD_IO) ||
                    (Config->DataTransferMode == R_SFMA_DDR_QUAD_IO))
                {
                    /* at 80MHZ with external address space use performance enhance mode */
                    if(R_SFMA_MODE_EX_SPACE == Mode)
                    {
                        Config->Command = &r_sfc_Macronix_32bitAddrCmdPerformanceEnhance_80MHz;
                        Config->PerformanceEnMode = R_SFMA_PER_EN_MODE_ENABLE;
                    }
                    else
                    {
                        Config->Command = &r_sfc_Macronix_32bitAddrQuadCmd_80MHz;
                    }
                }
                else
                {
                    Config->Command = &r_sfc_Macronix_32bitAddrCmd_80MHz;
                }
            }
            else
            {
                if ((Config->DataTransferMode == R_SFMA_SDR_QUAD_IO) ||
                    (Config->DataTransferMode == R_SFMA_DDR_QUAD_IO))
                {
                    Config->Command = &r_sfc_Macronix_32bitAddrQuadCmd_40MHz;
                }
                else
                {
                    Config->Command = &r_sfc_Macronix_32bitAddrCmd_40MHz;
                }
            }
        }
        else
        {
            if ((40*1000*1000) < Config->SerialFlashMemoryMaxClock)
            {
                if ((Config->DataTransferMode == R_SFMA_SDR_QUAD_IO) ||
                    (Config->DataTransferMode == R_SFMA_DDR_QUAD_IO))
                {
                    Config->Command = &r_sfc_Macronix_24bitAddrQuadCmd_80MHz;
                }
                else
                {
                    Config->Command = &r_sfc_Macronix_24bitAddrCmd_80MHz;
                }
            }
            else
            {
                if ((Config->DataTransferMode == R_SFMA_SDR_QUAD_IO) ||
                    (Config->DataTransferMode == R_SFMA_DDR_QUAD_IO))
                {
                    Config->Command = &r_sfc_Macronix_24bitAddrQuadCmd_40MHz;
                }
                else
                {
                    Config->Command = &r_sfc_Macronix_24bitAddrCmd_40MHz;
                }
            }
        }
    }
}


/*******************************************************************************
    Function: loc_CompareData32

    Compare two 32-bit arrays.
    Returns index of first mismatch or length if arrays are equal.
 */
uint32_t loc_CompareData32(uint32_t a[], uint32_t b[], uint32_t length32)
{
    uint32_t i;

    for(i = 0; i < length32; i++)
    {
        if ( a[i] != b[i] )
        {
            return i;
        }
    }
    return length32;
}
 
/**/

typedef struct {
    uint8_t manufacturer;
    uint8_t type;
    uint8_t capacity;
    char    name[50];
} serialMemoryIDs_t;


serialMemoryIDs_t IDs[] = {
    {0xC2, 0x20, 0x1A, "SFMA: Macronix MX25L51245G SerialFlash"},
    {0xC2, 0x27, 0x1B, "SFMA: Macronix MX66L1G85G  SerialFlash"},
    {0xC2, 0x30, 0x1B, "SFMA: Macronix MX66L1G83G  SerialFlash"},
    
    /*The SFMA I/F cannot identify below IDs, therefore, they are commented out*/
  #if 0
    {0x01, 0x7E, 0x1A, "HYPB: Spansion S26KL512S   HyperFLASH"},
    {0x03, 0x00, 0x00, "HYPB: ISSI IS67WVH16M8BLL  HyperFLASH"},

    {0xC2, 0x85, 0x3A, "OCTA: Macronix MX25LM51245G OctaFLASH"},
    {0xC2, 0x86, 0x3A, "OCTA: Macronix MX25LW51245G OctaFLASH"},
  #endif
};

void loc_Identify(void)
{
    r_sfma_Error_t err = R_SFMA_ERR_OK;
    uint8_t ManufacturerID;
    uint8_t MemoryType;
    uint8_t Capacity;
    uint32_t i;
    
    R_BSP_STDIO_Printf("\nIdentifying serial memory:\n");

    err |= R_SFMA_Init(LOC_SFMA_UNIT);
    err |= R_SFMA_JEDECRead(LOC_SFMA_UNIT, &ManufacturerID, &MemoryType, &Capacity);
    err |= R_SFMA_DeInit(LOC_SFMA_UNIT);
    
    R_BSP_STDIO_Printf("Current ID:\n Manufacturer: 0x%02x; Type: 0x%02x; Capacity: 0x%02x\n\n", ManufacturerID, MemoryType, Capacity);
    
    
    R_BSP_STDIO_Printf("Some known IDs:\n");
    for (i=0; i<(sizeof(IDs)/sizeof(serialMemoryIDs_t)); i++) {
        R_BSP_STDIO_Printf(" 0x%02x; 0x%02x; 0x%02x; %s\n", IDs[i].manufacturer, IDs[i].type, IDs[i].capacity, IDs[i].name);
    }
    
    R_BSP_STDIO_Printf("\nIf the current ID doesn't match any known SFMA ID, SW may require adaption.\n");
    R_BSP_STDIO_Printf("\nIf the current ID is 0xFF, connected memory is most likely OCTA or HYPB. SW may require adaption.\n");
}
 
/*******************************************************************************
    Function: loc_StartSFMA

    Opens SFMA for writing and writes a pattern to it.
    Afterwards SFMA is closed but opened for reading again, so that
    the application can use it.
 */
void* loc_StartSFMA(void)
{
    r_sfma_Error_t err = R_SFMA_ERR_OK;
    uint32_t sf_adr    = 0x00000000;
    r_sfma_Config_t cfg;
    uint32_t i;

    /* fill black king */
    for(i = 0; i < (LOC_KING_SIZE / sizeof(uint32_t)); i++)
    {
        loc_srcData[i]  = (uint32_t)g_BlackKingBmp[(i * 4)];
        loc_srcData[i] |= ((uint32_t)g_BlackKingBmp[(i * 4) + 1] << 8);
        loc_srcData[i] |= ((uint32_t)g_BlackKingBmp[(i * 4) + 2] << 16);
        loc_srcData[i] |= ((uint32_t)g_BlackKingBmp[(i * 4) + 3] << 24);
    }

    err |= R_SFMA_Init(LOC_SFMA_UNIT);

    /* open for writing */
    loc_SFMAConfig(&cfg, R_SFMA_MODE_SPI_OPERATING, R_SFMA_DDR_SINGLE_IO);
    err |= R_SFMA_Open(LOC_SFMA_UNIT, &cfg);
    err |= R_SFMA_ProtectionModeSet(LOC_SFMA_UNIT, R_SFMA_MODE_UNPROTECT);
    
    /* check if data is already there & program only if data is missing*/
    err |= R_SFMA_DataRead(LOC_SFMA_UNIT, sf_adr, (uint8_t*)loc_verifyData, sizeof(loc_srcData));
    if ((LOC_KING_SIZE / sizeof(uint32_t)) != loc_CompareData32(loc_srcData, loc_verifyData, (LOC_KING_SIZE / sizeof(uint32_t))))
    {
        /* write data to serial flash */
        err |= R_SFMA_DataErase(LOC_SFMA_UNIT, sf_adr, sizeof(loc_srcData));
        err |= R_SFMA_DataWrite(LOC_SFMA_UNIT, sf_adr, (uint8_t*)loc_srcData, sizeof(loc_srcData));
    
        /* verify the data is correct */
        err |= R_SFMA_DataRead(LOC_SFMA_UNIT, sf_adr, (uint8_t*)loc_verifyData, sizeof(loc_srcData));
        if ((LOC_KING_SIZE / sizeof(uint32_t)) != loc_CompareData32(loc_srcData, loc_verifyData, (LOC_KING_SIZE / sizeof(uint32_t))))
        {
            err |= R_SFMA_ERR_NG;
        }
    }
    err |= R_SFMA_Close(LOC_SFMA_UNIT);

    /* open again for reading */
    loc_SFMAConfig(&cfg, R_SFMA_MODE_EX_SPACE, R_SFMA_DDR_QUAD_IO);
    err |= R_SFMA_Open(LOC_SFMA_UNIT, &cfg);
    err |= R_SFMA_AccessAddressSet(LOC_SFMA_UNIT, 0, R_SFMA_ACCESS_RANGE_128MB);
    
    for(i = 0; i<(LOC_KING_SIZE / sizeof(uint32_t)); i++)
    {
        ((uint8_t*)loc_verifyData)[i] = ((uint8_t*)(LOC_SF_ADDRESS_EX + sf_adr))[i];
    }
    if ((LOC_KING_SIZE / sizeof(uint32_t)) != loc_CompareData32(loc_srcData, loc_verifyData, (LOC_KING_SIZE / sizeof(uint32_t))))
    {
        R_BSP_STDIO_Printf("SFMA Verification failed!");
        err |= R_SFMA_ERR_NG;
    }

    if(err != R_SFMA_ERR_OK)
    {
        loc_Error((int32_t)err);
    }
    return (void *)(LOC_SF_ADDRESS_EX + sf_adr);
}

/*******************************************************************************
    Function: loc_StopSFMA

    Closes and stops SFMA
 */
void loc_StopSFMA(void)
{
    r_sfma_Error_t err = R_SFMA_ERR_OK;

    err |= R_SFMA_Close(LOC_SFMA_UNIT);
    err |= R_SFMA_DeInit(LOC_SFMA_UNIT);

    if(err != R_SFMA_ERR_OK)
    {
        loc_Error((int32_t)err);
    }
}


