/************************************************************************************************
PROJECT : Octa calibration routine
FILE    : $Id: r_octa_calib.c 13492 2017-05-18 03:54:06Z shinobu.uehara $
============================================================================
DESCRIPTION
Main funcitons of the OCTA calibration application
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
Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.

****************************************************************************
*/

/*******************************************************************************
  Title: Serial Flash calibration application

*/

#ifdef OCTA_USE_BSP_CALIB

/*******************************************************************************
  Section Includes
*/

#include "r_typedefs.h"     /* Renesas basic types, e.g. uint32_t */

#include "r_octa_api.h"
#include "r_config_octa.h"
#include "r_octacdb_macronix.h"

#include "r_dev_api.h"
#include "r_tick_api.h"
#include "r_bsp_stdio_api.h"

#include "r_gpio_api.h"
#include "r_octa_calib.h"


/***********************************************************
    Constants

*/
/*******************************************************************************
  Constant: LOC_CAL_SIZE

  Calibration pattern size.
*/
#define LOC_CAL_SIZE             (16uL / sizeof(uint32_t))

/***************************************************************************
  Constant: LOC_EXTERNAL_BASE_~

  Definitions to the base address of external address space.
  
  LOC_EXTERNAL_BASE_ADDR0   - Base address for Channel0.
  LOC_EXTERNAL_BASE_ADDR1   - Base address for Channel1.
*/
#define LOC_EXTERNAL_BASE_ADDR0      (0x60000000uL)
#define LOC_EXTERNAL_BASE_ADDR1      (0x70000000uL)

/*******************************************************************************
  Variable: loc_CalPtr

  Calibration pattern to use for the calibration data.
*/
static uint32_t loc_CalPtr[LOC_CAL_SIZE] =
                                {0xF700F708u, 0x00FFF700u, 0x000800FFu, 0xFFFF0000u};

/*******************************************************************************
  Variable: loc_ExBaseAddr

  The base address of external address space for dedicated macro.
*/
static uint32_t loc_ExBaseAddr[2] = {LOC_EXTERNAL_BASE_ADDR0, LOC_EXTERNAL_BASE_ADDR1};


/***************************************************************************
  Section: Global functions
*/
/***********************************************************
  Function: OCTA_CalibrationPreparation

  see: <octa_cal.h>
*/
r_octa_Error_t OCTA_CalibrationPreparation(uint32_t Unit, uint32_t Channel, r_octa_Config_t * Config)
{
    r_octa_Error_t  result     = R_OCTA_ERR_OK;
    uint32_t        cnt;
    uint32_t*       ex_addr;
    uint32_t        calbuf[LOC_CAL_SIZE];
    uint32_t        calptr = R_FALSE;
    r_octa_DataTransferMode_t tmp_DTM;
    uint32_t        delay;
    
    ex_addr = (uint32_t*)(loc_ExBaseAddr[Channel] + Config->CalAddress);
    
    if (Config->DeviceType == R_OCTA_DEVICE_FLASH)
    {
        /* Change the SPI mode. for preparation calibration. */
        tmp_DTM = Config->DataTransferMode;
        Config->DataTransferMode = R_OCTA_MODE_SPI;
    }
    
    /* Init OCTA */
    result = R_OCTA_Init(Unit);
    if (result == R_OCTA_ERR_OK)
    {
        result = R_OCTA_Open(Unit, Channel, Config);
    }
    
    if (result == R_OCTA_ERR_OK)
    {
        /* verify data : data pattern all 0xFFFFFFFF */
        for (cnt = 0; cnt < LOC_CAL_SIZE ; cnt++)
        {
            calbuf[cnt] = ex_addr[cnt];
            if (calbuf[cnt] != 0xFFFFFFFFuL)
            {
                if (Config->DeviceType == R_OCTA_DEVICE_RAM)
                {
                    for (cnt = 0; cnt < LOC_CAL_SIZE; cnt++)
                    {
                        ex_addr[cnt] = 0xFFFFFFFFuL;
                    }
                }
                break;
            }
        }
        
        result =  R_OCTA_Close(Unit, Channel);
    }

    /* Execute the preparation calibration */
    if (result == R_OCTA_ERR_OK)
    {
        for (delay = 0; delay <= 0xFF; delay++)
        {
            Config->DQSDelay.Delay     = (uint8_t)delay;
            
            /* Write the calibration pattern */
            if (Config->DeviceType == R_OCTA_DEVICE_FLASH)
            {
                /* for Flash */
                Config->OpeMode        = R_OCTA_MODE_MANUAL;
                result = R_OCTA_Open(Unit, Channel, Config);
                if (result == R_OCTA_ERR_OK)
                {
                    result =  R_OCTA_DataProtect(Unit, Channel, R_OCTA_MODE_UNPROTECT);
                }
                
                if (result == R_OCTA_ERR_OK)
                {
                    result =  R_OCTA_DataErase(Unit, Channel, Config->CalAddress, 4096);
                }
                
                if (result == R_OCTA_ERR_OK)
                {
                    result =  R_OCTA_DataWrite(Unit, Channel, Config->CalAddress, (uint8_t*)loc_CalPtr, (LOC_CAL_SIZE * sizeof(uint32_t)));
                }
            }
            else
            {
                /* for RAM */
                result = R_OCTA_Open(Unit, Channel, Config);
                if (result == R_OCTA_ERR_OK)
                {
                    for (cnt = 0; cnt < LOC_CAL_SIZE; cnt++)
                    {
                        ex_addr[cnt] = loc_CalPtr[cnt];
                    }
                }
            }
            
            if (result == R_OCTA_ERR_OK)
            {
                result =  R_OCTA_Close(Unit, Channel);
            }
            
            /* Read and Check calibration pattern */
            if (result == R_OCTA_ERR_OK)
            {
                Config->OpeMode        = R_OCTA_MODE_EX_SPACE;
                result = R_OCTA_Open(Unit, Channel, Config);
                if (result == R_OCTA_ERR_OK)
                {
                    for (cnt = 0; cnt < LOC_CAL_SIZE ; cnt++)
                    {
                        calbuf[cnt] = ex_addr[cnt];
                        if (calbuf[cnt] != loc_CalPtr[cnt])
                        {
                            break;
                        }
                    }
                    result =  R_OCTA_Close(Unit, Channel);
                }
            }
            
            if (result == R_OCTA_ERR_OK)
            {
                /* calibration pattern OK */
                if (cnt >= LOC_CAL_SIZE)
                {
                    calptr = R_TRUE;
                    break;
                }
            }
        }
    }
    
    if (result == R_OCTA_ERR_OK)
    {
        /* Preparation calibration all NG */
        if (calptr != R_TRUE)
        {
            result = R_OCTA_ERR_NG;
        }
    }
    
    if (Config->DeviceType == R_OCTA_DEVICE_FLASH)
    {
        /* Store the Data Transfer mode. */
        Config->DataTransferMode = tmp_DTM;
    }
    
    if (result == R_OCTA_ERR_OK)
    {
        result =  R_OCTA_DeInit(Unit);
    }
    
    if (result != R_OCTA_ERR_OK)
    {
        R_OCTA_Close(Unit, Channel);
        R_OCTA_DeInit(Unit);
    }

    return result;
}

/***********************************************************
  Function: OCTA_Calibration

  see: <octa_cal.h>
*/
r_octa_Error_t OCTA_Calibration(uint32_t Unit, uint32_t Channel, r_octa_Config_t * Config)
{
    r_octa_Error_t  result     = R_OCTA_ERR_OK;
    uint8_t         delay;
    
    /* Init OCTA */
    result = R_OCTA_Init(Unit);
    if (result == R_OCTA_ERR_OK)
    {
        /* Set coarse delay value */
        result = OCTA_CalibrationCalcDelayValue(Unit, Channel, Config, 0x00, LOC_DELAY_PART_COARSE);
        if (result == R_OCTA_ERR_OK)
        {
            /* Get current calibration value */
            result = R_OCTA_GetCal(Unit, Channel, &delay);
            if (result == R_OCTA_ERR_OK)
            {
                /* Set fine delay value */
                result = OCTA_CalibrationCalcDelayValue(Unit, Channel, Config, delay, LOC_DELAY_PART_FINE);
            }
        }
        else
        {
            /* Set coarse + fine delay value */
            result = OCTA_CalibrationCalcDelayValue(Unit, Channel, Config, 0x00,
                                                         (LOC_DELAY_PART_COARSE | LOC_DELAY_PART_FINE));

        }
    }

    R_OCTA_Close(Unit, Channel);
    R_OCTA_DeInit(Unit);

    return result;
}


/***************************************************************************
  Function: OCTA_CalibrationCalcDelayValue

  see: <octa_cal.h>
*/
static r_octa_Error_t OCTA_CalibrationCalcDelayValue(uint32_t Unit, uint32_t Channel,
                                                          r_octa_Config_t * Config,
                                                          uint32_t          Delay,
                                                          uint32_t          Mode)
{
    r_octa_Error_t  result     = R_OCTA_ERR_OK;
    uint32_t        cnt;
    uint32_t*       ex_addr;
    uint32_t        calbuf[LOC_CAL_SIZE];
    uint32_t        calptr = R_FALSE;
    uint32_t        curptr = R_FALSE;
    uint32_t        preptr = R_FALSE;
    uint32_t        inc = 0x1;
    uint32_t        times;
    uint32_t        delay;
    uint32_t        maxlen = 0, curlen = 0;
    uint32_t        cur_maxdelay = 0, cur_mindelay = 0;
    uint32_t        maxdelay = 0, mindelay = 0;
    
    ex_addr = (uint32_t*)(loc_ExBaseAddr[Channel] + Config->CalAddress);
    
    if (Mode == (LOC_DELAY_PART_FINE | LOC_DELAY_PART_COARSE))
    {
        times = 0xFFuL;   /* 256 times */
    }
    else if (Mode & LOC_DELAY_PART_COARSE)
    {
        inc   = 0x10;
        times = 0xF0;
    }
    else
    {
        times = (Delay + 0x0FuL);    /* 16 times */
    }
    
    /* Execute the calcuration of delay value */
    Config->OpeMode        = R_OCTA_MODE_EX_SPACE;
    for (delay = Delay; delay <= times; delay += inc)
    {
        Config->DQSDelay.Delay     = (uint8_t)delay;
        
        /* Read and Check calibration pattern */
        result = R_OCTA_Open(Unit, Channel, Config);
        if (result == R_OCTA_ERR_OK)
        {
            for (cnt = 0; cnt < LOC_CAL_SIZE ; cnt++)
            {
                calbuf[cnt] = ex_addr[cnt];
                if (calbuf[cnt] != loc_CalPtr[cnt])
                {
                    /* calibration pattern NG */
                    calptr = R_FALSE;
                    break;
                }
            }
            /* calibration pattern OK */
            if (cnt >= LOC_CAL_SIZE)
            {
                calptr = R_TRUE;
            }
            
            /* set pattern result */
            preptr = curptr;
            curptr = calptr;

            if (curptr == R_TRUE)
            {
                if (preptr == R_TRUE)
                {
                    curlen++;
                    cur_maxdelay = delay;
                }
                else
                {
                    curlen = 1;
                    cur_maxdelay = delay;
                    cur_mindelay = delay;
                }
                
                /* update max / min delay */
                if (curlen > maxlen)
                {
                    maxlen   = curlen;
                    maxdelay = cur_maxdelay;
                    mindelay = cur_mindelay;
                }
            }
            else
            {
                /* reset curlen */
                curlen = 0;
            }
        }
        
        if (result == R_OCTA_ERR_OK)
        {
            result =  R_OCTA_Close(Unit, Channel);
        }
    }
    
    /* Calcurate delay value */
    if (result == R_OCTA_ERR_OK)
    {
        if (maxlen != 0)
        {
            delay = (maxdelay + mindelay) / 2;
            delay &= times;
            Config->DQSDelay.Delay     = (uint8_t)delay;
            
            /* set calibration value */
            result = R_OCTA_Open(Unit, Channel, Config);
            if (result == R_OCTA_ERR_OK)
            {
                result =  R_OCTA_Close(Unit, Channel);
            }
        }
        else
        {
            result = R_OCTA_ERR_NG;
        }
    }
    
    return result;
}

#endif /* OCTA_USE_BSP_CALIB */

