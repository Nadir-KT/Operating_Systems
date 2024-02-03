/*
****************************************************************************
PROJECT : NFMA driver
FILE    : $Id: r_nfma_bare_api.h 13286 2017-04-12 15:36:16Z tobyas.hennig $
============================================================================
DESCRIPTION
Driver for NFMA macro - driver API
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

#ifndef R_NFMA_BARE_API_H_
#define R_NFMA_BARE_API_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ONFI Timing Mode Structure */
typedef struct {
    uint32_t tADL;
    uint32_t tALH;
    uint32_t tALS;
    uint32_t tAR;
    uint32_t tCEA;
    uint32_t tCH;
    uint32_t tCHZ;
    uint32_t tCLH;
    uint32_t tCLR;
    uint32_t tCLS;
    uint32_t tCOH;
    uint32_t tCS;
    uint32_t tDH;
    uint32_t tDS;
    uint32_t tFEAT;
    uint32_t tIR;
    uint32_t tRC;
    uint32_t tREA;
    uint32_t tREH;
    uint32_t tRHOH;
    uint32_t tRHW;
    uint32_t tRHZ;
    uint32_t tRLOH;
    uint32_t tRP;
    uint32_t tRR;
    uint32_t tRST;
    uint32_t tWB;
    uint32_t tWC;
    uint32_t tWH;
    uint32_t tWHR;
    uint32_t tWP;
    uint32_t tWW;
} r_onfi_timing_mode_t;

/* Number of timing modes supported */
#define ONFI_NUM_TIMING_MODES           (6u)

#define ONFI_DEFAULT_TCCS_NS            (500u)

#define NFMA_DEFAULT_TIMING_MODE        (1u)


/* MT29F4G08 (x8)

  page size     - 2 kByte (+64 Byte ECC)
  block size    - 128 kByte (+ECC) = 64 pages
  device size   - 2x 2048 blocks
  
  -> block # addressing:    ROW BA[17..6]           (12 addresses)
  -> page  # addressing:    ROW BA[17..6]+PA[5..0]  (18 addresses)
  -> page addressing:       COL CA[11..0]           (12 addresses)
*/
#define NFMA_PAGE_SIZE_BYTE             (2048u)
#define NFMA_PAGES_PER_BLOCK            (64u)

#define NFMA_MASK_ROW_BLOCK_NO          (0x0003FFC0)
#define NFMA_MASK_ROW_PAGE_NO           (0x0003FFFF)
#define NFMA_MASK_COL_PAGE_SIZE         (0x000001FF)



typedef struct {
  uint32_t dummy; /* dummy */
} r_nfma_param_t;

typedef struct {
  uint32_t AccessAddr;
  uint32_t WrCount;
  uint32_t *WrData;
} r_nfma_write_param_t;

typedef struct {
  uint32_t AccessAddr;
  uint32_t RdCount;
  uint32_t *RdData;
} r_nfma_read_param_t;

typedef struct {
  uint32_t AccessAddr;
} r_nfma_erase_param_t;


/*
  Function: R_NFMA_Init
  
  Parameter:
            instance    - unit number of NFMA macro
            config      - configuration of NFMA macro <see r_nfma_param_t>
*/
void    R_NFMA_Init(uint32_t  instance, r_nfma_param_t config);

/*
  Function: R_NFMA_PageWrite/Read
  
  Parameter:
            instance          - unit number of NFMA macro
            write/read_config - Note: size in write/read config (write/read_config.Wr/RdCount)
                                      is given in words (= 4bytes = 32bit words).
*/
uint8_t R_NFMA_BlockErase(uint32_t instance, r_nfma_erase_param_t erase_config);
uint8_t R_NFMA_PageWrite(uint32_t instance, r_nfma_write_param_t write_config);
uint8_t R_NFMA_PageRead(    uint32_t instance, r_nfma_read_param_t  read_config);
uint8_t R_NFMA_PageRead_Dma(uint32_t instance, r_nfma_read_param_t  read_config);

/*
  Function: R_NFMA_Read
  
  Parameter:
            instance    - unit number of NFMA macro
            read_config - Note: size in read config (read_config.RdCount)
                          is given in bytes.
*/
uint8_t R_NFMA_Read(    uint32_t instance, r_nfma_read_param_t  read_config);
uint8_t R_NFMA_Read_Dma(uint32_t instance, r_nfma_read_param_t  read_config);


#ifdef __cplusplus
}
#endif

#endif /* R_NFMA_BARE_API_H_ */
