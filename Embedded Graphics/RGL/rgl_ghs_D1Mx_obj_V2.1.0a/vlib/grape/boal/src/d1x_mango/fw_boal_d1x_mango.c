/*
****************************************************************************
PROJECT : GRAPE
FILE    : $Id: fw_boal_d1x_mango.c 11006 2016-11-07 16:32:44Z michael.golczewski $
AUTHOR  : $Author: michael.golczewski $
============================================================================ 
DESCRIPTION
GRAPE Board Abstraction Layer (BOAL)
============================================================================
                            C O P Y R I G H T                                    
============================================================================
                           Copyright (c) 2011
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

/*******************************************************************************
  Title: BOAL dummy module
  
  This module manages the BOAL functions.
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "fw_boal_api.h"
#include "r_bsp_api.h"
#include "r_sfma_api.h"
#include "r_sfcdb_macronix.h"

/*******************************************************************************
  Section: Local Defines
*/
/***************************************************************************
  Constant: SF_DEFAULT_CAL

  Default calibration for Serial flash memory (Macronix device only).

    SPBCLK clock phase shift        3 (range 0 - 7, written to the upper 16bits)
    Sampling point phase shift      1 (range 0 - 7, written to the Lower 16 bits)
*/
#define SF_SPBCLK_CLOCK_PHASE_SHIFT    (3u)
#define SF_SAMPLING_POINT_PHASE_SHIFT  (1u)
#define LOC_SFMA_DEFAULT_CAL    ((SF_SPBCLK_CLOCK_PHASE_SHIFT << 16) | SF_SAMPLING_POINT_PHASE_SHIFT)


#define LOC_SFMA_SECTOR_SIZE  (4096u)
#define LOC_SFMA_UNIT       0

/*******************************************************************************
  Section: Local Variables
*/


/*******************************************************************************
  Section: Local Functions
*/
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
        Config->MemoryNum                   = R_SFMA_MEMORY_DUAL;
        Config->DataTransferMode            = Transfer;
        Config->AddressMode                 = R_SFMA_ADDRESS_32BIT;
        Config->SerialFlashMemoryPageSize   = 0x100uL; /* 256Byte */
        Config->SerialFlashMemoryMaxClock   = (80*1000*1000);
        Config->CacheMode                   = R_SFMA_CACHE_OFF;
        Config->PerformanceEnMode           = R_SFMA_PER_EN_MODE_DISABLE;
        Config->Calibration                 = LOC_SFMA_DEFAULT_CAL;

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


/***********************************************************
  Function: SetupSFMA

  Setup SFMA Macro for extended address space access.
*/
static void SetupSFMA()
{
    r_sfma_Error_t err;
    r_sfma_Config_t cfg;
    
    /* Initializes the SFMA driver */
    err = R_SFMA_Init(LOC_SFMA_UNIT);
    if(err != R_SFMA_ERR_OK)
    {
        return;
    }

    /* Set the configuration data */
    loc_SFMAConfig(&cfg, R_SFMA_MODE_EX_SPACE, R_SFMA_SDR_QUAD_IO);

    /* Opens the SFMA driver at the external address space operating mode */
    err = R_SFMA_Open(LOC_SFMA_UNIT, &cfg);
    if(err != R_SFMA_ERR_OK)
    {
        return;
    }

    /* Set address range for mapped access */
    err = R_SFMA_AccessAddressSet(LOC_SFMA_UNIT, 0x02000000, R_SFMA_ACCESS_RANGE_128MB);
    if(err != R_SFMA_ERR_OK)
    {
        return;
    }
}

/***********************************************************
  Function: TeardownSFMA

  De-Init SFMA Macro
*/
static void TeardownSFMA()
{
    r_sfma_Error_t err;

    err = R_SFMA_Close(LOC_SFMA_UNIT);
    if(err != R_SFMA_ERR_OK)
    {
        return;
    }

    err = R_SFMA_DeInit(LOC_SFMA_UNIT);
    if(err != R_SFMA_ERR_OK)
    {
        return;
    }
}


/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: FW_BOAL_LowLevelInit
  
  See: boal.h for details
*/

void FW_BOAL_LowLevelInit(void) 
{
    R_BSP_Init();
    SetupSFMA();
}


/*******************************************************************************
  Function: FW_BOAL_LowLevelDeInit
  
  See: boal.h for details
*/

void FW_BOAL_LowLevelDeInit(void) 
{
    TeardownSFMA();
    R_BSP_DeInit();
}


/*******************************************************************************
  Function: FW_BOAL_OsInit
  
  See: boal.h for details
*/

void FW_BOAL_OsLevelInit(void) 
{
}


/*******************************************************************************
  Function: FW_BOAL_OsDeInit
  
  See: boal.h for details
*/

void FW_BOAL_OsLevelDeInit(void) 
{
}


