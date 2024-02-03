/*
****************************************************************************
PROJECT : BSP - Raw NAND flash routines
FILE    : $Id: r_nfma_init_memcpy.c 12943 2017-03-03 17:12:19Z tobyas.hennig $
============================================================================
DESCRIPTION
Main functions of the NFMA init and memcopy
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
Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.

****************************************************************************
*/

/*******************************************************************************
  Title: NFMA init and memcopy application

*/

#ifdef BSP_INIT_NFMA

/*******************************************************************************
  Section Includes
*/

#include "r_typedefs.h"     /* Renesas basic types, e.g. uint32_t */

#include "r_nfma_bare_api.h"

#include "r_dev_api.h"
#include "r_tick_api.h"
#include "r_bsp_stdio_api.h"

#include "r_gpio_api.h"
#include "r_nfma_init_memcpy.h"


/***********************************************************
    Section: Local Constants
*/

static const r_dev_PinConfig_t loc_D1M1A_NandDefaultPinConfig[] = 
/*  Port Pin Func      Dir      Feedback     Pull        OpenDrain   HiCurr   HiSpeed  InputType */
{
    /* NFMA default pins */
    {42,  0, 5u, R_DEV_PIN_OUT,    0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* NFMA_CE  out */
    {42,  1, 5u, R_DEV_PIN_OUT,    0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* NFMA_CLE out */
    {42,  2, 5u, R_DEV_PIN_OUT,    0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* NFMA_ALE out */
    {42,  3, 5u, R_DEV_PIN_OUT,    0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* NFMA_REB out */
    {42,  4, 5u, R_DEV_PIN_OUT,    0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* NFMA_WEB out */
    {42,  5, 5u, R_DEV_PIN_IN,     0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* NFMA_RB0  in */
    {42,  6, 5u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* NFMA_DI/DO[0..] in/out */
    {42,  7, 5u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, 
    {42,  8, 5u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, 
    {42,  9, 5u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, 
    {42, 10, 5u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, 
    {42, 11, 5u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, 
    {42, 12, 5u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, 
    {42, 13, 5u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* NFMA_DI/DO[..7] in/out */
    
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1} 
};

/***************************************************************************
  Section: Local functions
*/

/***************************************************************************
  Section: Global functions
*/

/***************************************************************************
  Function: NF_Init

  see: <r_nfma_init_memcpy.h>
*/
void NF_Init(uint32_t Unit)
{
    r_nfma_param_t nfma_config;
    
    R_BSP_STDIO_Printf("NFMA0 Init.\n");
    R_DEV_PinInit(loc_D1M1A_NandDefaultPinConfig);
    
    nfma_config.dummy = 0; /* dummy */
    R_NFMA_Init(Unit, nfma_config);
}

/***************************************************************************
  Function: NF_MemCpy

  see: <r_nfma_init_memcpy.h>
*/

void NF_MemCpy(uint32_t Unit, uint32_t Dst, uint32_t Src, uint32_t len)
{
    r_nfma_read_param_t nfma_read_config;
    
    R_BSP_STDIO_Printf("NFMA0 MemCpy %d MB to 0x%x.\n", len/1024/1024, Dst);
    
    nfma_read_config.AccessAddr = Src;
    nfma_read_config.RdCount    = len;
    nfma_read_config.RdData     = (uint32_t*)Dst;
    R_NFMA_Read_Dma(Unit, nfma_read_config);
}


#endif /* BSP_INIT_NFMA */

