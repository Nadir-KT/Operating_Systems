/*
****************************************************************************
PROJECT : NFMA driver
FILE    : $Id: r_nfma_bare_reg.h 13286 2017-04-12 15:36:16Z tobyas.hennig $
============================================================================
DESCRIPTION
Driver for NFMA macro - registers
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

#ifndef NFMA_REG_H_
#define NFMA_REG_H_

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
  Macro: R_NFMA_WRITE_REG

  Write NFMA register with predefined size.
  SIZE allowed values are 8, 16, 32
*/
#ifdef R_DBG_PRINT_NFMA_REGS
    #define R_NFMA_WRITE_REG(SIZE, ADDR, VALUE) R_DBG_PrintRegCmd((SIZE), (ADDR), (VALUE));  *((volatile uint##SIZE##_t*)(ADDR)) = (VALUE)
#else
    #define R_NFMA_WRITE_REG(SIZE, ADDR, VALUE)  (*((volatile uint##SIZE##_t*)(ADDR)) = (VALUE))
#endif



/*******************************************************************************
  Macro: R_NFMA_READ_REG

  Read NFMA register with predefined size.
  SIZE allowed values are 8, 16, 32
*/

#ifdef R_DBG_PRINT_NFMA_REGS
#define R_NFMA_READ_REG(SIZE, ADDR) R_DBG_PrintRegRead##SIZE##(ADDR)
#else
#define R_NFMA_READ_REG(SIZE, ADDR)  (*((volatile uint##SIZE##_t*)((ADDR)))) 
#endif




#define NFMA_COMMAND          (0x000)  
#define NFMA_CONTROL          (0x004)  
#define NFMA_STATUS           (0x008)  
#define NFMA_STATUS_MASK      (0x00C)  
#define NFMA_INT_MASK         (0x010)  
#define NFMA_INT_STATUS       (0x014)  
#define NFMA_ECC_CTRL         (0x018)  
#define NFMA_ECC_OFFSET       (0x01C)  
#define NFMA_ECC_STAT         (0x020)  
#define NFMA_ADDR0_COL        (0x024)  
#define NFMA_ADDR0_ROW        (0x028)  
#define NFMA_ADDR1_COL        (0x02C)  
#define NFMA_ADDR1_ROW        (0x030)  
#define NFMA_FIFO_DATA        (0x038)  
#define NFMA_DATA_REG         (0x03C)  
#define NFMA_DATA_REG_SIZE    (0x040)  
#define NFMA_DEV0_PTR         (0x044)  
#define NFMA_DMA_ADDR_L       (0x064)  
#define NFMA_DMA_ADDR_H       (0x068)  
#define NFMA_DMA_CNT          (0x06C)
#define NFMA_DMA_CTRL         (0x070)
#define NFMA_BBM_CTRL         (0x074)
#define NFMA_MEM_CTRL         (0x080)
#define NFMA_DATA_SIZE        (0x084)
#define NFMA_TIMINGS_ASYN     (0x088)
#define NFMA_TIME_SEQ_0       (0x090)
#define NFMA_TIME_SEQ_1       (0x094)
#define NFMA_TIME_GEN_SEQ_0   (0x098)
#define NFMA_TIME_GEN_SEQ_1   (0x09C)
#define NFMA_TIME_GEN_SEQ_2   (0x0A0)
#define NFMA_FIFO_INIT        (0x0B0)
#define NFMA_FIFO_STATE       (0x0B4)
#define NFMA_GEN_SEQ_CTRL     (0x0B8)
#define NFMA_MLUN             (0x0BC)
#define NFMA_DEV0_SIZE        (0x0C0)
#define NFMA_DMA_TRIG_TLVL    (0x114)
#define NFMA_CMD_MARK         (0x124)
#define NFMA_LUN_STATUS0      (0x128)
#define NFMA_LUN_STATUS1      (0x12C)
#define NFMA_TIME_GEN_SEQ_3   (0x134)
#define NFMA_ECC_CNT          (0x14C)

/* register bit field offset */
#define NFMA_RBFO_CMD_SEQ            (0x00)  /* Command Code                              */
#define NFMA_RBFO_INPUT_SEL          (0x06)  /* Input module selection flag               */
#define NFMA_RBFO_DATA_SEL           (0x07)  /* Data / FIFO selection flag:               */
#define NFMA_RBFO_CMD_0              (0x08)  /* Code of the first command in a sequence   */
#define NFMA_RBFO_CMD_1_3            (0x10)  /* Code of the second command in a sequence. */
#define NFMA_RBFO_CMD_2              (0x18)  /* Code of the third command in a sequence.  */

#define NFMA_RBFO_MEM0_ST            (0x00)  /* Device 0 Status Flag                                       */
#define NFMA_RBFO_CTRL_STAT          (0x08)  /* The main controller status bit                             */
#define NFMA_RBFO_AUTO_READ_STAT_EN  (0x17)  /* Auto read status                                           */
#define NFMA_RBFO_IO_WIDTH           (0x0C)  /* NAND Flash I/O width                                       */
#define NFMA_RBFO_BLOCK_SIZE         (0x06)  /* Overwritten later                                          */
#define NFMA_RBFO_INT_EN             (0x04)  /* Enable interrupts                                          */
#define NFMA_RBFO_READ_STATUS_EN     (0x00)  /* Read status                                                */
#define NFMA_RBFO_MEM0_WP            (0x08)  /* The WP line state of the first device in the selected bank */

#define NFMA_RBFO_TWHR               (0x18)  /* WE# high to RE# low time */
#define NFMA_RBFO_TRHW               (0x10)  /* RE# high to WE# low time */
#define NFMA_RBFO_TADL               (0x08)  /* ALE to data start time */
#define NFMA_RBFO_TCCS               (0x00)  /* Change column setup. */
#define NFMA_RBFO_TWW                (0x10)  /* WP# transition to command cycle delay */
#define NFMA_RBFO_TRR                (0x08)  /* Read high to Read low. */
#define NFMA_RBFO_TWB                (0x00)  /* tWB delay */
#define NFMA_RBFO_ASYN_TRWH          (0x04)  /* RE# or WE# high hold time */
#define NFMA_RBFO_ASYN_TRWP          (0x00)  /* RE# or WE# pulse width */

#define NFMA_RBF_MEM0_ST             (1u<<0)  /* Device 0 Status Flag                                           */
#define NFMA_RBF_CTRL_STAT           (1u<<8)  /* The main controller status bit                                 */
#define NFMA_RBF_CF_EMPTY            (1u<<2)  /* Command FIFO empty flag                                        */
#define NFMA_RBF_DF_W_FULL           (1u<<1)  /* Data FIFO full state bit                                       */
#define NFMA_RBF_DF_W_EMPTY          (1u<<7)  /* Data FIFO Empty state bit                                      */
#define NFMA_RBF_DF_R_EMPTY          (1u<<0)  /* Data FIFO empty state bit                                      */
#define NFMA_RBF_TRANS_ERR_FL        (1u<<4)  /* The transfer on the slave interface error                      */
#define NFMA_RBF_STAT_ERR_INT0_FL    (1u<<16) /* Most recently finished operation on the Memory device 0 failed */
#define NFMA_RBF_MEM0_WP             (1u<<8)  /* The WP line state of the first device in the selected bank     */

#define NFMA_INTSTAT_DMA_INT_FL      (1u<<3)  /* DMA Transfer ended      (DMA_INT_FL bit     in INT_STATUS register) */
#define NFMA_INTSTAT_CMD_END_INT_FL  (1u<<1)  /* Transfer sequence ended (CMD_END_INT_FL bit in INT_STATUS register) */

#define NFMA_DMA_DMA_READY           (1u<<0)  /* DMA ready flag (set when dma transfer is completed) */

#define NAND_RBF_FAIL                (1u<<0)  /* Program/Erase/Read Error  */
#define NAND_RBF_FAILC               (1u<<1)  /*                           */
#define NAND_RBF_REWRITE             (1u<<3)  /* Rewrite Recommended       */
#define NAND_RBF_ARDY                (1u<<5)  /*                           */
#define NAND_RBF_RDY                 (1u<<6)  /* Device Ready              */
#define NAND_RBF_WP                  (1u<<7)  /* Protected                 */


#ifdef __cplusplus
}
#endif

#endif /* NFMA_REG_H_ */
