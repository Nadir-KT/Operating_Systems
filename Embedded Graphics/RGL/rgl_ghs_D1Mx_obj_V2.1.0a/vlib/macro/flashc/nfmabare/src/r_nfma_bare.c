/*
****************************************************************************
PROJECT : NFMA driver
FILE    : $Id: r_nfma_bare.c 13286 2017-04-12 15:36:16Z tobyas.hennig $
============================================================================
DESCRIPTION
Driver for NFMA macro - main driver functions
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

#include "r_typedefs.h"
#include "r_dev_api.h"

#include "r_bsp_api.h"
#include "r_bsp_stdio_api.h"

#include "r_nfma_bare_api.h"
#include "r_nfma_bare_sys.h"
#include "r_nfma_bare_reg.h"


static const r_onfi_timing_mode_t timing_modes[ONFI_NUM_TIMING_MODES] =
{
/*  tADL tALH tALS tAR tCEA tCH tCHZ tCLH tCLR tCLS tCOH tCS tDH tDS tFEAT tIR tRC  tREA tREH tRHOH tRHW tRHZ tRLOH tRP  tRR tRST  tWB  tWC tWH tWHR  tWP tWW */
/*    ns   ns   ns  ns   ns  ns   ns  ns   ns   ns   ns  ns  ns  ns  us    ns  ns   ns   ns   ns    ns   ns   ns    ns   ns   us   ns   ns   ns   ns  ns   ns */
    {200, 20, 50,  25, 100, 20, 100, 20,  20,  50,   0,  70, 20, 40, 1,    10, 100, 40,  30,   0,   200, 200, 0,    50,  40, 1000, 200, 100, 30, 120, 50, 100},  /* ONFi1.0 mode 0 */
    {100, 10, 25,  10,  45, 10,  50, 10,  10,  25,  15,  35, 10, 20, 1,    0,  50,  30,  15,  15,   100, 100, 0,    25,  20,  500, 100,  45, 15,  80, 25, 100},  /* ONFi1.0 mode 1 (Fastest mode possible w/ RH850/D1M1A) */
    {100, 10, 15,  10,  30, 10,  50, 10,  10,  15,  15,  25,  5, 15, 1,    0,  35,  25,  15,  15,   100, 100, 0,    17,  20,  500, 100,  35, 15,  80, 17, 100},
    {100, 5,  10,  10,  25,  5,  50,  5,  10,  10,  15,  25,  5, 10, 1,    0,  30,  20,  10,  15,   100, 100, 0,    15,  20,  500, 100,  30, 10,  60, 15, 100},
    { 70, 5,  10,  10,  25,  5,  30,  5,  10,  10,  15,  20,  5, 10, 1,    0,  25,  20,  10,  15,   100, 100, 5,    12,  20,  500, 100,  25, 10,  60, 12, 100},
    { 70, 5,  10,  10,  25,  5,  30,  5,  10,  10,  15,  15,  5,  7, 1,    0,  20,  16,   7,  15,   100, 100, 5,    10,  20,  500, 100,  20,  7,  60, 10, 100}
};


/*******************************************************************************
  Section: Local Functions
*/
static void WaitNFMA_Flags(uint32_t instance, uint32_t statusreg, uint32_t flags);
static void WaitNFMA_Flags(uint32_t instance, uint32_t statusreg, uint32_t flags)
{
    uint32_t NFMAn_BASE;
  
    NFMAn_BASE = R_SYS_NFMA_GetBaseAddr(instance);
  
    while((flags & R_NFMA_READ_REG(32, NFMAn_BASE + statusreg)) != flags);
}

/* Convert a timing value in nanoseconds to counts of CLKB */
static uint32_t ConvNFMA_ClkB_NanoSec(uint32_t ns);
static uint32_t ConvNFMA_ClkB_NanoSec(uint32_t ns)
{
    uint32_t freq_clkb;
    
    freq_clkb = R_SYS_NFMA_GetClkFreqMHz(0);
    return ((freq_clkb * (ns))/1000);
}

/*******************************************************************************
  Section: Global Functions
*/

void R_NFMA_Init(uint32_t instance, r_nfma_param_t config)
{
  uint32_t NFMAn_BASE;
  uint32_t dataL;
  const r_onfi_timing_mode_t *mode = mode = &timing_modes[NFMA_DEFAULT_TIMING_MODE];

  NFMAn_BASE = R_SYS_NFMA_GetBaseAddr(instance);

  /* Clear and mask all interrupts */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_INT_STATUS, 0x0);
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_INT_MASK, 0x0);

  dataL  = 0x1<<NFMA_RBFO_AUTO_READ_STAT_EN; /* Auto read status */
  dataL |= 0x0<<NFMA_RBFO_IO_WIDTH;          /* I/O width 8 bit */
  dataL |= 0x0<<NFMA_RBFO_BLOCK_SIZE;
  dataL |= 0x1<<NFMA_RBFO_INT_EN;
  dataL |= 0x1<<NFMA_RBFO_READ_STATUS_EN;
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_CONTROL, dataL);

  /* Is used when NFMA_RBFO_AUTO_READ_STAT_EN is used
     [15:8] is for the READ_STATUS data of the NandFlash (FAIL, FAILC bits)
      [7:0]  is for the READ_STATUS data of the NandFlash (RDY, ARDY bits) 
  */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_STATUS_MASK, 0x140);

  
  /* write MEM_CTRL */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_MEM_CTRL, 0x0|NFMA_RBF_MEM0_WP);

  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_DATA_REG_SIZE, 0x3);

  dataL  = (ConvNFMA_ClkB_NanoSec(mode->tWHR) << NFMA_RBFO_TWHR);
  dataL |= (ConvNFMA_ClkB_NanoSec(mode->tRHW) << NFMA_RBFO_TRHW);
  dataL |= (ConvNFMA_ClkB_NanoSec(mode->tADL) << NFMA_RBFO_TADL);
  dataL |= (ConvNFMA_ClkB_NanoSec(ONFI_DEFAULT_TCCS_NS) << NFMA_RBFO_TCCS);
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_TIME_SEQ_0, dataL);

  dataL  = (ConvNFMA_ClkB_NanoSec(mode->tWW) << NFMA_RBFO_TWW);
  dataL |= (ConvNFMA_ClkB_NanoSec(mode->tRR) << NFMA_RBFO_TRR);
  dataL |= (ConvNFMA_ClkB_NanoSec(mode->tWB) << NFMA_RBFO_TWB);
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_TIME_SEQ_1, dataL);

  dataL  = (ConvNFMA_ClkB_NanoSec(mode->tREH) << NFMA_RBFO_ASYN_TRWH);
  dataL |= (ConvNFMA_ClkB_NanoSec(mode->tREA) << NFMA_RBFO_ASYN_TRWP);
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_TIMINGS_ASYN, dataL);

NFMA_reset:
  /* STATUS.MEM_ST */
  WaitNFMA_Flags(instance, NFMA_STATUS, NFMA_RBF_MEM0_ST);

  /* FIFO_STATE.CF_EMPTY*/
  WaitNFMA_Flags(instance, NFMA_FIFO_STATE, NFMA_RBF_CF_EMPTY);

  /* RESET COMMAND 0xFF00 */
  dataL = 0xFF00;
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_COMMAND, dataL);

  /* STATUS.MEM_ST */
  WaitNFMA_Flags(instance, NFMA_STATUS, NFMA_RBF_MEM0_ST);

  dataL = R_NFMA_READ_REG(32, NFMAn_BASE + NFMA_INT_STATUS);
  if (dataL & NFMA_RBF_TRANS_ERR_FL) {
    R_BSP_STDIO_Printf("NFMA Reset Transfer Error");
    goto NFMA_reset;
  }
}


uint8_t R_NFMA_BlockErase(uint32_t instance, r_nfma_erase_param_t erase_config)
{
  uint32_t NFMAn_BASE;
  uint32_t dataL;
  uint32_t block;

  NFMAn_BASE = R_SYS_NFMA_GetBaseAddr(instance);

NFMA_erase:
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_INT_STATUS, 0x0);
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_INT_MASK, 0x0);

  /* write ADDR0_ROW */
  block = (erase_config.AccessAddr * 4 / (NFMA_PAGE_SIZE_BYTE * NFMA_PAGES_PER_BLOCK));
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_ADDR0_ROW, ((block*NFMA_PAGES_PER_BLOCK) & NFMA_MASK_ROW_BLOCK_NO));
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_ADDR0_COL, (0x0 & NFMA_MASK_COL_PAGE_SIZE));  /* ADDR0_COL not used by PageErase (SEQ_14) */
  
  /* write MEM_CTRL */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_MEM_CTRL, 0x0);

  /* Erase BLOCK COMMAND 0x00D0600E */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_COMMAND, 0x00D0600E);

  /* STATUS.MEM_ST */
  WaitNFMA_Flags(instance, NFMA_STATUS, NFMA_RBF_MEM0_ST);

  dataL = R_NFMA_READ_REG(32, NFMAn_BASE + NFMA_INT_STATUS);
  if (dataL & NFMA_RBF_TRANS_ERR_FL)
  {
    R_BSP_STDIO_Printf("NFMA Erase Transfer Error");
    goto NFMA_erase;
  }

  if (dataL & NFMA_RBF_STAT_ERR_INT0_FL)
  {
    R_BSP_STDIO_Printf("NFMA Erase Error");
    goto NFMA_erase;
  }
  return 0x0;
}

uint8_t R_NFMA_PageWrite(uint32_t instance, r_nfma_write_param_t write_config)
{
  uint32_t NFMAn_BASE;
  uint32_t dataL;
  uint32_t count;
  uint32_t page;

  NFMAn_BASE = R_SYS_NFMA_GetBaseAddr(instance);

  if( 1>write_config.WrCount ) return 0x1;

NFMA_write:
  /* FIFO flush */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_FIFO_INIT, 0x1);
  WaitNFMA_Flags(instance, NFMA_FIFO_STATE, NFMA_RBF_DF_W_EMPTY);

  /* Clear and mask all interrupts */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_INT_STATUS, 0x0);
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_INT_MASK, 0x0);

  /* write ADDR0_COL , write ADDR0_ROW, write DATA_SIZE */
  page = (write_config.AccessAddr * 4 / NFMA_PAGE_SIZE_BYTE);
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_ADDR0_ROW, (page & NFMA_MASK_ROW_PAGE_NO));
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_ADDR0_COL, (write_config.AccessAddr & NFMA_MASK_COL_PAGE_SIZE));

  /* Write the Number of Required Bytes */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_DATA_SIZE, (write_config.WrCount * 4));
  
  
  /* write MEM_CTRL */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_MEM_CTRL, 0x0);

  /* Program PAGE COMMAND 0x0010800c */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_COMMAND, 0x0010800c);

  /* FIFO_STATE.CF_EMPTY*/
  WaitNFMA_Flags(instance, NFMA_FIFO_STATE, NFMA_RBF_CF_EMPTY);

  /* Write Data to FIFO */
  for (count = 0; count < write_config.WrCount; count++)
  {
    /* TODO: Add a timeout to this loop */
    while ((R_NFMA_READ_REG(32, NFMAn_BASE + NFMA_FIFO_STATE) & NFMA_RBF_DF_W_FULL) != 0)
    {
        /* Nothing */
    }
    R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_FIFO_DATA, *(write_config.WrData+count));
  }

  /* STATUS.MEM_ST */
  WaitNFMA_Flags(instance, NFMA_STATUS, NFMA_RBF_MEM0_ST);

  dataL = R_NFMA_READ_REG(32, NFMAn_BASE + NFMA_INT_STATUS);
  if (dataL & NFMA_RBF_TRANS_ERR_FL)
  {
    R_BSP_STDIO_Printf("NFMA Write Transfer Error");
    goto NFMA_write;
  }

  if (dataL & NFMA_RBF_STAT_ERR_INT0_FL)
  {
    R_BSP_STDIO_Printf("NFMA Write Error");
    goto NFMA_write;
  }

  return 0x0;
}

uint8_t R_NFMA_PageRead(uint32_t instance, r_nfma_read_param_t read_config)
{
  uint32_t NFMAn_BASE;
  uint32_t dataL;
  uint32_t count;
  uint32_t page;

  NFMAn_BASE = R_SYS_NFMA_GetBaseAddr(instance);

  if( 1>read_config.RdCount ) return 0x1;

NFMA_read:
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_FIFO_INIT, 0x1);
  WaitNFMA_Flags(instance, NFMA_FIFO_STATE, NFMA_RBF_DF_W_EMPTY);

  /* Clear and mask all interrupts */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_INT_STATUS, 0x0);
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_INT_MASK, 0x0);

  /* write ADDR0_COL , write ADDR0_ROW */
  page = (read_config.AccessAddr * 4 / NFMA_PAGE_SIZE_BYTE);
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_ADDR0_ROW, (page & NFMA_MASK_ROW_PAGE_NO));
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_ADDR0_COL, (read_config.AccessAddr & NFMA_MASK_COL_PAGE_SIZE));

  /* Read the Number of Required Bytes */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_DATA_SIZE, (read_config.RdCount * 4));

  
  /* write MEM_CTRL */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_MEM_CTRL, 0x0|(0x1<<NFMA_RBFO_MEM0_WP));

  /* Read PAGE COMMAND 0x3000002A */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_COMMAND, 0x3000002A);

  /* FIFO_STATE.CF_EMPTY, FIFO_STATE.DF_R_EMPTY*/
  WaitNFMA_Flags(instance, NFMA_FIFO_STATE, NFMA_RBF_CF_EMPTY);

  /* STATUS.MEM_ST */
  WaitNFMA_Flags(instance, NFMA_STATUS, NFMA_RBF_MEM0_ST);

  dataL = R_NFMA_READ_REG(32, NFMAn_BASE + NFMA_INT_STATUS);
  if (dataL & NFMA_RBF_TRANS_ERR_FL)
  {
    R_BSP_STDIO_Printf("NFMA Read Transfer Error");
    goto NFMA_read;
  }

  /* Read Data from FIFO */
  for (count = 0; count < read_config.RdCount; count++)
  {
    while ((R_NFMA_READ_REG(32, NFMAn_BASE + NFMA_FIFO_STATE) & NFMA_RBF_DF_R_EMPTY) != 0)
    {

    }
    dataL = R_NFMA_READ_REG(32, NFMAn_BASE + NFMA_FIFO_DATA);
    *(read_config.RdData+count) = dataL;
  }

  return 0x0;
}


uint8_t R_NFMA_PageRead_Dma(uint32_t instance, r_nfma_read_param_t read_config)
{
  uint32_t NFMAn_BASE;
  uint32_t page;

  NFMAn_BASE = R_SYS_NFMA_GetBaseAddr(instance);

  if( 1>read_config.RdCount ) return 0x1;

  /* Clear and mask all interrupts */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_INT_STATUS, 0x0);
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_INT_MASK, 0x0);

  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_FIFO_INIT, 0x1);
  WaitNFMA_Flags(instance, NFMA_FIFO_STATE, NFMA_RBF_DF_W_EMPTY);

  /* write ADDR0_COL , write ADDR0_ROW */
  page = (read_config.AccessAddr * 4 / NFMA_PAGE_SIZE_BYTE);
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_ADDR0_ROW, (page & NFMA_MASK_ROW_PAGE_NO));
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_ADDR0_COL, (read_config.AccessAddr & NFMA_MASK_COL_PAGE_SIZE));

  /* Write the Number of Required Bytes */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_DATA_SIZE, (read_config.RdCount * 4));

  /* write MEM_CTRL */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_MEM_CTRL, 0x0|(0x1<<NFMA_RBFO_MEM0_WP));

  /* setup DMA */
  /* Write the address of the first descriptor to the DMA_ADDR register.
     And the size of data to be transfered to the DMA_CNT register. */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_DMA_ADDR_H, 0x0);
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_DMA_ADDR_L, (uint32_t)read_config.RdData);
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_DMA_CNT,    (read_config.RdCount * 4));
  /* Set DMA_START bit to start DMA when the command sequence
     is sent to the NAND Flash memory.
     mode: register managed mode / single transfer(addr inc) */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_DMA_CTRL,   0x88);

  /* Read PAGE COMMAND 0x3000002A, but w/ DMA -> 0x3000006A */
  R_NFMA_WRITE_REG(32, NFMAn_BASE + NFMA_COMMAND, 0x3000006A);

  /* When the controller is ready for further work,
     the appropriate DMA_INT_FL bit is set and the interrupt is activated. */
  WaitNFMA_Flags(instance, NFMA_INT_STATUS, NFMA_INTSTAT_DMA_INT_FL);

  return 0x0;
}



uint8_t R_NFMA_Read(uint32_t instance, r_nfma_read_param_t read_config)
{
    r_nfma_read_param_t page_read_config;
    uint32_t byteCount = 0;
    uint32_t err = 0;
    
    /* Error check: Access Address to be aligned to one Page */
    if ((read_config.AccessAddr % NFMA_PAGE_SIZE_BYTE) != 0x0)
    {
        /* Access Address is not aligned to one Page */
        err = 1;
    }    
    
    /* Slit read data request into page read requests
        and process page read requests one after each other. */
    while (byteCount < read_config.RdCount)
    {
        page_read_config.AccessAddr = (read_config.AccessAddr + byteCount/4);
        page_read_config.RdData     = (read_config.RdData + byteCount/4);
        if ((read_config.RdCount - byteCount) > (NFMA_PAGE_SIZE_BYTE))
        {
            page_read_config.RdCount    = (NFMA_PAGE_SIZE_BYTE/4);
        } else {
            page_read_config.RdCount    = ((read_config.RdCount - byteCount)/4);
        }
        
        err = R_NFMA_PageRead(instance, page_read_config);
        if (0 != err)
        {
            return err;
        }
        
        byteCount += (page_read_config.RdCount * 4);
    }
    
    /* Error check: Number of read data */
    page_read_config.RdData     = (read_config.RdData + byteCount/4);
    if (page_read_config.RdData != (read_config.RdData + read_config.RdCount/4))
    {
        /* Size of read data does not match to read data size request.  */
        err = 1;
    }
    
    return err;
}


uint8_t R_NFMA_Read_Dma(uint32_t instance, r_nfma_read_param_t read_config)
{
    r_nfma_read_param_t page_read_config;
    uint32_t byteCount = 0;
    uint32_t err = 0;
    
    /* Error check: Access Address to be aligned to one Page */
    if ((read_config.AccessAddr % NFMA_PAGE_SIZE_BYTE) != 0x0)
    {
        /* Access Address is not aligned to one Page */
        err = 1;
    }    
    
    /* Slit read data request into page read requests
        and process page read requests one after each other. */
    while (byteCount < read_config.RdCount)
    {
        page_read_config.AccessAddr = (read_config.AccessAddr + byteCount/4);
        page_read_config.RdData     = (read_config.RdData + byteCount/4);
        if ((read_config.RdCount - byteCount) > (NFMA_PAGE_SIZE_BYTE))
        {
            page_read_config.RdCount    = (NFMA_PAGE_SIZE_BYTE/4);
        } else {
            page_read_config.RdCount    = ((read_config.RdCount - byteCount)/4);
        }
        
        err = R_NFMA_PageRead_Dma(instance, page_read_config);
        if (0 != err)
        {
            return err;
        }
        
        byteCount += (page_read_config.RdCount * 4);
    }
    
    /* Error check: Number of read data */
    page_read_config.RdData     = (read_config.RdData + byteCount/4);
    if (page_read_config.RdData != (read_config.RdData + read_config.RdCount/4))
    {
        /* Size of read data does not match to read data size request.  */
        err = 1;
    }
    
    return err;
}

