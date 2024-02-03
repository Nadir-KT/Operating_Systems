/*
****************************************************************************
PROJECT : BSP - GFX 
FILE    : $Id: r_bsp_sys_gfx_d1m1.c 17130 2020-02-25 12:38:34Z shinya.tomari.wj@renesas.com $
============================================================================
DESCRIPTION
BSP Implementation D1X MANGO Board
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
Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.     

****************************************************************************
*/

/*
  Section: BSP SYS GFX description 
  
  This is the board specific GFX implementation for the BSP. 
        
*/


/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"     /* Renesas basic types, e.g. uint32_t */

#include "r_tick_api.h"
#include "r_dev_api.h"
#include "r_gpio_api.h"
#include "r_xbus_api.h"

#include "r_bsp_api.h"
#include "r_bsp_stdio_api.h"
#include "r_bsp_gfx_api.h"
#include "r_bsp_gfx_sys.h"
#include "r_bsp_board_config.h"



/*******************************************************************************
  Section: Local Defines
*/

/*
The following flags are used in several files.
Thus, please set these flags globally, e.g. in a Makefile.

    USE_VDCE_SERIALRGB  -   Configure VDCE to use Serial RGB on Pins "VDCE1 Out"
    USE_VDCE_OPENLDI    -   Configure VDCE to use Open LDI on Pins "VDCE0 Out"
    USE_VDCE_VODDR      -   Configure VDCE to use VO-DDR on Pins "VDCE0 Out"
*/

/* USE_VDCE_SERIALRGB
    0 = Parallel Video Output (default, if undefined)
    1 = Serial RGB
*/
#ifndef USE_VDCE_SERIALRGB
    #define USE_VDCE_SERIALRGB  (0u)
#endif

/* USE_VDCE_OPENLDI
    0 = Parallel Video Output (default, if undefined)
    1 = Open LDI
*/
#ifndef USE_VDCE_OPENLDI
    #define USE_VDCE_OPENLDI    (0u)
#endif

/* USE_VDCE_VODDR
    0 = Parallel Video Output (default, if undefined)
    1 = VO DDR Video Output
*/
#ifndef USE_VDCE_VODDR
    #define USE_VDCE_VODDR      (0u)
#endif

#if (USE_VDCE_SERIALRGB | USE_VDCE_OPENLDI) & USE_VDCE_VODDR
    #error "VO-DDR must not be enabled together with SerialRGB and OpenLDI"
#endif

/* Module reset control register */
#define LOC_MRSTC           0xFFF8F400u
#define LOC_PROTSMRST       0xFFF8F484u
#define LOC_PROTCMDMRST     0xFFF8F480u

/* Module reset control register bits (only for SDRA) */
#define MRSTC_SDRB0RES      ((uint32_t)(1ul<<12u))


/* SDRA (DBSC2) SDR-SDRAM MemC registers (for D1M1H + D1M1A MANGO) */

#define SDRA_BASE       (0xf2fe0000u)

 #define DBSVCR           (SDRA_BASE + 0x0004u)
 #define DBKIND           (SDRA_BASE + 0x0008u)
 #define DBSTATE          (SDRA_BASE + 0x000cu)
 #define DBEN             (SDRA_BASE + 0x0010u)
 #define DBCMDCNT         (SDRA_BASE + 0x0014u)
 #define DBCKECNT         (SDRA_BASE + 0x0018u)
 #define DBAXICNT         (SDRA_BASE + 0x001cu)
 #define DBCONF           (SDRA_BASE + 0x0020u)
 #define DBTR0            (SDRA_BASE + 0x0030u)
 #define DBTR1            (SDRA_BASE + 0x0034u)
 #define DBTR2            (SDRA_BASE + 0x0038u)
 #define DBRFPDN0         (SDRA_BASE + 0x0040u)
 #define DBRFPDN1         (SDRA_BASE + 0x0044u)
 #define DBRFPDN2         (SDRA_BASE + 0x0048u)
 #define DBRFSTS          (SDRA_BASE + 0x004cu)
 #define DBMRCNT          (SDRA_BASE + 0x0060u)
 #define DBRSET           (SDRA_BASE + 0x0070u)
 #define DBRCHOICE0       (SDRA_BASE + 0x0074u)
 #define DBRCHOICE1       (SDRA_BASE + 0x0078u)
 #define DBTTR            (SDRA_BASE + 0x007cu)
 #define DBPDCNT0         (SDRA_BASE + 0x0108u)

/* SDRA (DBSC2) SDR-SDRAM Transaction Restrictor Registers (for D1M1A MANGO) */

 #define SDRATRCTL        (0xFFFD3000u)
 #define SDRATRINTVL      (0xFFFD3004u)



/* XC1 QoS Registers (D1M1(H) only) */

#define QOS_BASE       (0xffcd2300u)

#define QOS_MODE       (QOS_BASE + 0x0000u)
#define QOS_MAX_COUNT  (QOS_BASE + 0x0004u)
#define QOS_MAX_RACC   (QOS_BASE + 0x0008u)
#define QOS_MAX_WACC   (QOS_BASE + 0x000cu)

#define QOS_PORT_CPU0   0x0
#define QOS_PORT_CPU1   0x1
#define QOS_PORT_VACC0  0x2
#define QOS_PORT_VACC1  0x3
#define QOS_PORT_JCUA   0x4
#define QOS_PORT_GPU2D  0x5
#define QOS_PORT_ETNB   0x6

/* video config */

/* video selection registers */
#define VDCECTL         0xFFC0601Cu

#define VDCECTL_UPDT1   (1ul<<5)
#define VDCECTL_UPDT0   (1ul<<4)
#define VDCECTL_PXSL    (1ul<<3)
#define VDCECTL_VOSL    (1ul<<2)
#define VDCECTL_VISL0   (1ul<<0)

/* RSDS and other video output control register */
#define RSDSCFG                 (0xFFC06020u)

#define RSDSCFG_VODDR_OEN       (1ul<<4)
#define RSDSCFG_OLDI_OEN        (1ul<<5)
#define RSDSCFG_SRGB1_OEN       (1ul<<8)


/* preload buffer (PRLB) control registers */
#define PRL0REG                 0xF200A000u
#define PRL1REG                 0xF200A004u
#define PRL2REG                 0xF200A008u
#define PRL3REG                 0xF200A00Cu
#define PRLnREG_PRLEN           (0x1ul<<0)
#define PRLnREG_PRLID0_EN       (0x0ul<<4)
#define PRLnREG_PRLID1_EN       (0x1ul<<8)
#define PRLnREG_PRLID2_EN       (0x2ul<<12)
#define PRLnREG_PRLID0_DIS      (0xful<<4)
#define PRLnREG_PRLID1_DIS      (0xful<<8)
#define PRLnREG_PRLID2_DIS      (0xful<<12)
#define PRLnREG_PRLISS          (0x2ul<<16)
#define PRLnREG_PRLTO           (0x0ul<<24)
/* Configure PRLB here */
#ifndef PRLB_EN_VDCE0_LAYER0
  #define PRLB_EN_VDCE0_LAYER0    (0u)
#endif
#ifndef PRLB_EN_VDCE0_LAYER1
  #define PRLB_EN_VDCE0_LAYER1    (0u)
#endif
#ifndef PRLB_EN_VDCE0_LAYER2
  #define PRLB_EN_VDCE0_LAYER2    (0u)
#endif
#ifndef PRLB_EN_VDCE0_LAYER3
  #define PRLB_EN_VDCE0_LAYER3    (0u)
#endif
#ifndef PRLB_EN_VDCE0_OIR
  #define PRLB_EN_VDCE0_OIR       (0u)
#endif
#ifndef PRLB_EN_VDCE1_LAYER0
  #define PRLB_EN_VDCE1_LAYER0    (0u)
#endif
#ifndef PRLB_EN_VDCE1_LAYER1
  #define PRLB_EN_VDCE1_LAYER1    (0u)
#endif
#ifndef PRLB_EN_VDCE1_LAYER2
  #define PRLB_EN_VDCE1_LAYER2    (0u)
#endif
#ifndef PRLB_EN_VDCE1_LAYER3
  #define PRLB_EN_VDCE1_LAYER3    (0u)
#endif

/* OpenLDI interface */
#define OLDI0CR0                    (0xF2002000u)
#define OLDI0CR1                    (0xF2002004u)
#define OLDI0CTRCR                  (0xF200200Cu)
#define OLDI0CHCR                   (0xF2002010u)
#define OLDI0SKEWCTR                (0xF2002070u)
                                    
#define OLDI0CR0_LVRES              (0x1ul<<0)
#define OLDI0CR0_LVEN               (0x1ul<<1)
#define OLDI0CR0_LVMD(mode)         ((mode)<<8) /* Possible modes: 0 - 7 */
                                    
#define OLDI0CR1_CLKSTBY(ctrl)      ((ctrl)<<0) /* STBY: Standby Mode, EN: Operating Mode */
#define OLDI0CR1_CH1STBY(ctrl)      ((ctrl)<<2) /* STBY: Standby Mode, EN: Operating Mode */
#define OLDI0CR1_CH2STBY(ctrl)      ((ctrl)<<4) /* STBY: Standby Mode, EN: Operating Mode */
#define OLDI0CR1_CH3STBY(ctrl)      ((ctrl)<<6) /* STBY: Standby Mode, EN: Operating Mode */
#define OLDI0CR1_CH4STBY(ctrl)      ((ctrl)<<8) /* STBY: Standby Mode, EN: Operating Mode */
#define OLDI0CR1_CHnSTBY_EN         (0x3ul)
#define OLDI0CR1_CHnSTBY_STBY       (0x0ul)
                                    
#define OLDI0CTRCR_CTR0SEL(sel)     ((sel)<<0)  /* 0: HSYNC, 1: VSYNC, 2: DE    Attention: Different values allowed for each CTR0 to CTR2! */
#define OLDI0CTRCR_CTR1SEL(sel)     ((sel)<<4)  /* 0: VSYNC, 1: DE,    4: HSYNC */
#define OLDI0CTRCR_CTR2SEL(sel)     ((sel)<<8)  /* 0: DE,    3: HSYNC, 4: VSYNC */
                                    
#define OLDI0CHCR_CH1SEL(ch)        ((ch)<<0)  /* 0: CH1, 1: CH2, 2: CH3, 3: CH4 */
#define OLDI0CHCR_CH2SEL(ch)        ((ch)<<4)  /* 0: CH2, 1: CH3, 2: CH4, 3: CH1 */
#define OLDI0CHCR_CH3SEL(ch)        ((ch)<<8)  /* 0: CH3, 1: CH4, 2: CH1, 3: CH2 */
#define OLDI0CHCR_CH4SEL(ch)        ((ch)<<12) /* 0: CH4, 1: CH1, 2: CH2, 3: CH3 */

#define OLDI0SKEWCTR_SKEW1(skew)    ((skew)<<0)
#define OLDI0SKEWCTR_SKEW2(skew)    ((skew)<<3)
#define OLDI0SKEWCTR_SKEW3(skew)    ((skew)<<6)
#define OLDI0SKEWCTR_SKEW4(skew)    ((skew)<<9)
#define OLDI0SKEWCTR_SKEWC(skew)    ((skew)<<12)
#define OLDI0SKEWCTR_SKEWn_150PS    (3ul) /* +150ps */
#define OLDI0SKEWCTR_SKEWn_100PS    (2ul) /* +100ps */
#define OLDI0SKEWCTR_SKEWn_50PS     (1ul) /* + 50ps */
#define OLDI0SKEWCTR_SKEWn_0PS      (0ul) /*    0ps */
#define OLDI0SKEWCTR_SKEWn_M50PS    (5ul) /* - 50ps */
#define OLDI0SKEWCTR_SKEWn_M100PS   (6ul) /* -100ps */
#define OLDI0SKEWCTR_SKEWn_M150PS   (7ul) /* -150ps */

/* VO-DDR Interface */
#define VODDR0_SYSCNT               (0xF2001000ul)
#define VODDR0_CLKDIV               (0xF2001004ul)
#define VODDR0_TIMCNT1              (0xF200100Cul)

#define VODDR0_SYSCNT_CLK_START     (1ul<<0)
#define VODDR0_SYSCNT_VO1_ENABLE    (1ul<<16)

#define VODDR0_CLKDIV_CLKDIV0(div)  ((div)&0xFFu<<0)  /* Set clock divider of 2..254 of PLLCLK */
#define VODDR0_CLKDIV_CLKDIV1(div)  ((div)&0xFFu<<16) /* Set clock divider of 2..254 of PLLCLK */

#define VODDR0_TIMCNT1_CONFIG0(cyc) ((cyc)<<0)  /* Set delay of 0..7 cycles of PLLCLK */
#define VODDR0_TIMCNT1_CONFIG1(cyc) ((cyc)<<8)  /* Set delay of 0..7 cycles of PLLCLK */
#define VODDR0_TIMCNT1_OUTTIM0(cyc) ((cyc)<<16) /* Set delay of 0..7 cycles of PLLCLK */
#define VODDR0_TIMCNT1_OUTTIM1(cyc) ((cyc)<<24) /* Set delay of 0..7 cycles of PLLCLK */






/*******************************************************************************
  Section: Local Types 
*/

/* XC1 QoS configuration */

typedef struct {
    uint32_t mode; 
    uint32_t max_cnt; 
    uint32_t max_racc;
    uint32_t max_wacc;
} r_xc1_qos_config_t;

/* SDRA-SDRAM configuration values */

typedef struct {
    uint32_t    vDBCONF;     /* SDRAM Address Configuration */
    uint32_t    vDBTR0;      /* SDRAM Timing Configuration (1) */
    uint32_t    vDBTR1;      /* SDRAM Timing Configuration (2) */
    uint32_t    vDBTR2;      /* SDRAM Timing Configuration (3) */
    uint32_t    vDBRFPDN1;   /* Refresh time: (tREF_AT*fRAM/nROWS) - SSCG_factor */
    uint32_t    vDBRFPDN2;   /* Idle- and Busy-thresholds */
} r_sdra_params_t;

/*******************************************************************************
  Section: Local Constants
*/

static const r_dev_PinConfig_t loc_D1M1_D1M1H_VoDefaultPinConfig[] = 
/*  Port Pin Func      Dir      Feedback     Pull        OpenDrain   HiCurr   HiSpeed  InputType */
{
    /* VDCE0 Output 24bit parallel, HSync+VSync */
    {44, 0,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 Out Data 23 */
    {44, 1,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 2,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 3,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 4,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 5,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 6,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 7,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 8,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 9,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 10, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 11, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 Out Data 12 */
    {45, 0,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 Out CLKP */
/*   45, 1,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1,*//* VDCE0 Out CLKN */
    {45, 1,  4u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 Out Tcon3 DE (shared on same pin as CLKN) */
    {45, 2,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 Out Data 11 */
    {45, 3,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 4,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 5,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 6,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 7,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 8,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 9,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 10, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 11, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 12, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 13, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 Out Data 0 */
    {43, 0,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 Out TCON2 HS */
/*   43, 0,  2u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1,*//* VDCE0 Out Tcon3 DE (shared on same pin as TCON2 HS) */
    {43, 1,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 Out TCON0 VS */
    {42, 11, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 Out TCON3 DE */
    
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1} 
};

static const r_dev_PinConfig_t loc_D1M1_D1M1H_ViDefaultPinConfig[] = 
/*  Port Pin Func      Dir      Feedback     Pull        OpenDrain   HiCurr   HiSpeed  InputType */
{
    /* VDCE0 Input ITU on P42_[10:0] AF2_In; BT656 from CVBS source via SAA71xx video converter */
    {42, 0,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* VDCE0 In ITU DATA7 */
    {42, 1,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {42, 2,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {42, 3,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {42, 4,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {42, 5,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {42, 6,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {42, 7,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* VDCE0 In ITU DATA0 */
    {42, 8,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* VDCE0 In ITU CLK */
    {42, 9,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* VDCE0 In ITU HSYNC */
    {42, 10, 2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* VDCE0 In ITU VSYNC */
    
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1} 
};

#if (USE_VDCE_OPENLDI == 1) && (USE_VDCE_SERIALRGB == 0)
static const r_dev_PinConfig_t loc_D1M1A_VoOpenLdiPinConfig[] = 
/*  Port Pin Func      Dir      Feedback     Pull        OpenDrain   HiCurr   HiSpeed  InputType */
{
    /* VDCE0 Output 24bit OpenLDI */
    {45, 0,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch0_p (clk_p) */
    {45, 1,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch0_n (clk_n) */
    {45, 2,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch1_p */
    {45, 3,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch1_n */
    {45, 4,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch2_p */
    {45, 5,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch2_n */
    {45, 6,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch3_p */
    {45, 7,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch3_n */
    {45, 8,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch4_p */
    {45, 9,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch4_n */
    /* Special for R7F701460 POD that emulates D1M1A with different pin-out */
    /* Replacement pin config of OpenLDI at R7F701460 POD
        that replaces P45_8 and P45_9 of D1M1A real device
        by P44_10 and P44_11 (also channels 1-4 are shifted) */
    {44, 10, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch1_p */
    {44, 11, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch1_n */
    
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1} 
};
#endif /* (USE_VDCE_OPENLDI == 1) && (USE_VDCE_SERIALRGB == 0) */


#if (USE_VDCE_SERIALRGB == 1) && (USE_VDCE_OPENLDI == 0)
static const r_dev_PinConfig_t loc_D1M1A_VoSerialRgbPinConfig[] = 
/*  Port Pin Func      Dir      Feedback     Pull        OpenDrain   HiCurr   HiSpeed  InputType */
{
    /* VDCE0 Output SerialRGB */
    {42, 11, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE1 Out TCON3 DE */
    {43, 0,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE1 Out TCON2 HS */
    {43, 1,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE1 Out TCON0 VS */
    {44, 0,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE1 Out Data 7 */
    {44, 1,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 2,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 3,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 4,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 5,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 6,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 7,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE1 Out Data 0 */
    {44, 8,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE1 Out CLKP */
    
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1} 
};
#endif /* (USE_VDCE_SERIALRGB == 1) && (USE_VDCE_OPENLDI == 0) */

#if (USE_VDCE_SERIALRGB == 1) && (USE_VDCE_OPENLDI == 1)
static const r_dev_PinConfig_t loc_D1M1A_VoOpenLdiSerialRgbPinConfig[] = 
/*  Port Pin Func      Dir      Feedback     Pull        OpenDrain   HiCurr   HiSpeed  InputType */
{
    /* VDCE0 Output 24bit OpenLDI */
    {45, 0,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch0_p (clk_p) */
    {45, 1,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch0_n (clk_n) */
    {45, 2,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch1_p */
    {45, 3,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch1_n */
    {45, 4,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch2_p */
    {45, 5,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch2_n */
    {45, 6,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch3_p */
    {45, 7,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch3_n */
    {45, 8,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch4_p */
    {45, 9,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch4_n */
    /* Special for R7F701460 POD that emulates D1M1A with different pin-out */
    /* Replacement pin config of OpenLDI at R7F701460 POD
        that replaces P45_8 and P45_9 of D1M1A real device
        by P44_10 and P44_11 (also channels 1-4 are shifted) */
    {44, 10, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch1_p */
    {44, 11, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE0 OpenLDI ch1_n */
    
    /* VDCE0 Output Serial RGB */
    {42, 11, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE1 Out TCON3 DE */
    {43, 0,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE1 Out TCON2 HS */
    {43, 1,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE1 Out TCON0 VS */
    {44, 0,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE1 Out Data 7 */
    {44, 1,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 2,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 3,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 4,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 5,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 6,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 7,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE1 Out Data 0 */
    {44, 8,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VDCE1 Out CLKP */
    
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1} 
};
#endif /* (USE_VDCE_SERIALRGB == 1) && (USE_VDCE_OPENLDI == 1) */


#if USE_VDCE_VODDR
static const r_dev_PinConfig_t loc_D1M1A_VoVoDdrPinConfig[] = 
/*  Port Pin Func      Dir      Feedback     Pull        OpenDrain   HiCurr   HiSpeed  InputType */
{
    /* VDCE0 Output 24bit VO-DDR + VDCE1 Output 24bit VO-DDR */
    /* VODDR 24bit parallel, HSync+VSync, CLK0+CLK1 */
    {44, 0,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VODDR Out Data 23 */
    {44, 1,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 2,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 3,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 4,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 5,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 6,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 7,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 8,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 9,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 10, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {44, 11, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VODDR Out Data 12 */
    {45, 0,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VODDR Out OUT0_CLK */
    {45, 1,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VODDR Out OUT1_CLK */
    {45, 2,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VODDR Out Data 11 */
    {45, 3,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 4,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 5,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 6,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 7,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 8,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 9,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 10, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 11, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 12, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {45, 13, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VODDR Out Data 0 */
    {43, 0,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VODDR Out TCON2 HS */
/*   43, 0,  2u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1,     VODDR Out TCON3 DE (shared on same pin as TCON2 HS) */
    {43, 1,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VODDR Out TCON0 VS */
    {42, 11, 1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* VODDR Out TCON3 DE */
    
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1} 
};
#endif /* USE_VDCE_VODDR */


#if defined (BSP_INIT_QOS_SDRAM) || defined (BSP_INIT_QOS_VRAM)

/* XC1 QoS Configuration for maximum speed at master port */ 
static const r_xc1_qos_config_t qos_config_max[] = 
{
    0x3u,   /* Threshold mode */
    0x7u,   /* Initial cycle count: 8 cycles */
    0x7u,   /* 8 read bursts permitted within 8 cycles */
    0x7u,   /* 8 write bursts permitted within 8 cycles */
};
#endif

#ifdef BSP_INIT_QOS_SDRAM
/* XC1 QoS Configuration for D/AVE HD SDRAM access (assuming single layer VDCE pixel clock of 30MHz) */
/* This mode roughly divides the bandwidth between: D/AVE HD -> 33% read, 33% write
                                                    VDCE: 33% read
    At 100MHz SDRAM clock, this scheme locks maximum VDCE pixel clock at ~30MHz.
    For other pixel clocks / layers, different values should be used!
*/
static const r_xc1_qos_config_t qos_config_dhd_sdram[] = 
{
    0x3u,   /* Threshold mode */
    0x7fu,  /* Initial cycle count: 96 cycles */
    0x0u,   /* 1 read burst permitted within 96 cycles */
    0x7u,   /* 8 write burst permitted within 96 cycles (D/AVE HD write is limited by read ratio) */    
};
#endif

#ifdef BSP_INIT_QOS_VRAM
/* XC1 QoS Configuration for D/AVE HD VRAM access (assuming VDCE pixel clock of 30MHz) */
static const r_xc1_qos_config_t qos_config_dhd_vram[] = 
{
    0x3u,
    0x5fu,
    0x0u,   
    0x7u,       
};
#endif


/*******************************************************************************
  SDRA-SDRAM configuration values
  
  These are the configuration values specific to Micron SDRAM "MT48LC4M32B2P-6A"
  running with 100MHz.
  
  Init SDRA(DBSC2) SDR-SDRAM Memc of D1M1H Mango with 16 MByte
  (SDRA supports up to 128 MByte, but D1M1H Mango Board only implements 16 MByte.)
  1 piece of external Micron SDRAM "MT48LC4M32B2P".
  (1x 4Mx32bit = 1x 128Mbit = 16 MByte)
  Use the register based setup.
*/
static const r_sdra_params_t MT48LC4M32B2P_100MHz = {
    /* DBCONF */ 0x01110a02u,   /* SDRAM Address Configuration
                                   ===========================
                                   - 32-bit width, 12-bit row, 8-bit col

                                   - BA[1:0] = A[11:10]

                                   - MA[11:0] = A[23:12] -> RAS
                                   - MA[ 7:0] = A[ 9:2]  -> CAS
                                 */
    /* DBTR0 */  0x02040501u,   /* SDRAM Timing Configuration (1)
                                   ==============================
                                   - CL   = 2 clk
                                   - tRAS = 5 clk
                                   - tRFC = 6 clk
                                   - tRCD = 2 clk
                                 */
    /* DBTR1 */  0x00010102u,   /* SDRAM Timing Configuration (2)
                                   ==============================
                                   - CKEH = 1 clk                                           ???  testing 1
                                   - tRP  = 2 clk
                                   - tRRD = 2 clk
                                   - tWR  = 2 clk                                           ??? different value mapping, changed
                                 */
    /* DBTR2 */  0x01050504u,   /* SDRAM Timing Configuration (3)
                                   ==============================
                                   - tRTP = 2 clk
                                   - tRC  = 6 clk
                                   - RDWR = 6 clk       (BL=Burst Length + CL=CAS Latency)  ??? would be 6
                                   - WRRD = 5 clk       (BL=Burst Length)                   ??? would be 4
                                 */ 
    /* DBRFPDN1*/ 0x00000173u,  /* Refresh time of 16ms: 
                                          (tREF_AT*fRAM/nROWS)-SSCG_factor
                                          (16ms*100MHz/4096rows) - 5%SSCG = 0x173; 
                                          0x186 would be 16ms without SSCG
                                          With SSCG 0x186 corresponds to 16.8ms.
                                 */
    /* DBRFPDN2*/ 0x0aaa002Du,  /* LV1TH: tREFI  <= LV1TH <= tREFI x 9         
                                          tREFI  =  tREF/ (#rows*tCLK) = 16ms/(4096*10ns)=390,6
                                          391    <= LV1TH <= 3515
                                          0x0aaa == 2730
                                   LV0TH: LV0TH =  (tRP + tRCD + tWR + 3) x 5 = 45
                                          0x02d =  45
                                 */
};



/*******************************************************************************
  SDRA-SDRAM configuration values
  
  These are the configuration values specific to 2x Micron SDRAM "MT48LC16M16A2P-6A:G"
  running with 120MHz.
  
  Init SDRA(DBSC2) SDR-SDRAM Memc of D1M1A Mango with 2 x 32 MByte
  (SDRA supports up to 128 MByte, but D1M1A Mango Board only implements 64 MByte.)
  2 pieces of external Micron SDRAM "MT48LC4M32B2P".
  (2x 16Mx16bit = 1x 512Mbit = 64 MByte)
  Use the register based setup.
*/
static const r_sdra_params_t MT48LC16M16A2P_120MHz = {
    /* DBCONF */ 0x011a0c02u,   /* SDRAM Address Configuration
                                   ===========================
                                   - 32-bit width, 13-bit row, 10-bit col

                                   - BA[1:0] = A[13:12]

                                   - MA[11:0] = A[25:14] -> RAS
                                   - MA[ 7:0] = A[11:2]  -> CAS
                                 */
    /* DBTR0 */  0x02040701u,   /* SDRAM Timing Configuration (1)
                                   ==============================
                                   - CL   = 2 clk
                                   - tRAS = 6 clk       (42ns / 8.33ns = 5.04clk)
                                   - tRFC = 8 clk       (60ns / 8.33ns = 7.2clk)
                                   - tRCD = 3 clk       (18ns / 8.33ns = 2.16clk)
                                 */
    /* DBTR1 */  0x00010102u,   /* SDRAM Timing Configuration (2)
                                   ==============================
                                   - CKEH = 1 clk
                                   - tRP  = 3 clk       (18ns)
                                   - tRRD = 2 clk       (12ns)
                                   - tWR  = 2 clk       (1 clk + 6ns)
                                 */
    /* DBTR2 */  0x01050504u,   /* SDRAM Timing Configuration (3)
                                   ==============================
                                   - tRTP = 2 clk
                                   - tRC  = 7 clk       (60ns)
                                   - RDWR = 6 clk       (BL=Burst Length + CL=CAS Latency + 1)  Would be 7 acc to formula. Hand optimised.
                                   - WRRD = 5 clk       (BL=Burst Length + 1)
                                 */ 
    /* DBRFPDN1*/ 0x000000EAu,  /* Refresh time of 16ms: 
                                          (tREF_AT*fRAM/nROWS) - SSCG_factor
                                          SSCG is downspread only, so there is no correction needed.
                                          (16ms*120MHz/8192rows) - 0%SSCG = 0xEA
                                 */
    /* DBRFPDN2*/ 0x0555002Du,  /* LV1TH: tREFI  <= LV1TH <= tREFI x 9         
                                          tREFI  =  tREF/ (#rows*tCLK) = 16ms/(4096*8.33ns)=0xEA
                                          234    <= LV1TH <= 2106
                                          0x0555 == 1365
                                   LV0TH: LV0TH =  (tRP + tRCD + tWR + 3) x 5 = 45
                                          0x02d =  45
                                 */
};
/*******************************************************************************
  Section: Local Functions declarations 
*/

void loc_SysGfxInitD1M1(void);
void loc_SysLowInitD1M1(void);

void loc_InitVideoIOPortD1M1(uint32_t *VoPinsAddr, uint32_t *ViPinsAddr);
void loc_InitVideoIOPortD1M1A(uint32_t *VoPinsAddr, uint32_t *ViPinsAddr);

static void loc_InitSDRAMemMangoRaw(const r_sdra_params_t *params);
static void loc_SDRA_SelectIoDriveStrength(void);
#if defined BSP_SDRA_QOS_WRITE_RESTRICT || defined BSP_SDRA_QOS_READ_RESTRICT
static void loc_InitSDRATransactionRestrictor(void);
#endif

/*******************************************************************************
  Function: loc_InitVideoIOPortD1M1 / loc_InitVideoIOPortD1M1A
  
  Init port pins of Video I/O and select a video configuration.
*/

void loc_InitVideoIOPortD1M1(uint32_t *VoPinsAddr, uint32_t *ViPinsAddr)
{
    uint32_t val;
    
    *VoPinsAddr = (uint32_t) &(loc_D1M1_D1M1H_VoDefaultPinConfig[0]);
    *ViPinsAddr = (uint32_t) &(loc_D1M1_D1M1H_ViDefaultPinConfig[0]);
    
    val = 0;
    /* video input selection VDCE0 */
#if 1
    /* select BT656 on 2nd alternative input on VDCE0 Input
        (i.e. ITU port pins P42_[10..0] */
    val &= ~VDCECTL_PXSL;
    val &= ~VDCECTL_VISL0;
#else
    /* select RGB on 3rd alternative input on VDCE0 Input
        (i.e. RGB666 port pins P42_[15..0] + P44_[5..1] */
    val &= ~VDCECTL_PXSL;
    val |= VDCECTL_VISL0;
#endif
    R_DEV_WRITE_REG(32, VDCECTL, ((val & 0x3Fu) | 0x10000u));

    
    /* video output assignment */
     /* nothing to select for D1M1(H) */
}


void loc_InitVideoIOPortD1M1A(uint32_t *VoPinsAddr, uint32_t *ViPinsAddr)
{
    uint32_t val;
    uint32_t chSelValD1M1A = 0;
    (void)chSelValD1M1A;
    
    /* D1M1A default VO/VI interface is same as for D1M1(H) */
    *VoPinsAddr = (uint32_t) &(loc_D1M1_D1M1H_VoDefaultPinConfig[0]);
    *ViPinsAddr = (uint32_t) &(loc_D1M1_D1M1H_ViDefaultPinConfig[0]);
    R_DEV_WRITE_REG(32, RSDSCFG, 0x0);


#if USE_VDCE_OPENLDI
  #if USE_VDCE_SERIALRGB == 0
    /* VO0 use interface: OpenLDI
       VO1 use interface: N/A
       VI  use interface: N/A
    */
    *VoPinsAddr = (uint32_t) &(loc_D1M1A_VoOpenLdiPinConfig[0]);
    *ViPinsAddr = 0;
    R_DEV_WRITE_REG(32, RSDSCFG, RSDSCFG_OLDI_OEN);
  #else
    /* Combined Port setting of OpenLDI + SerialRGB will be set below */
  #endif /* USE_VDCE_SERIALRGB == 0 */
  
    /* Set all channels to operating mode */
    R_DEV_WRITE_REG(32, OLDI0CR1,       OLDI0CR1_CLKSTBY(OLDI0CR1_CHnSTBY_EN) | 
                                        OLDI0CR1_CH1STBY(OLDI0CR1_CHnSTBY_EN) |
                                        OLDI0CR1_CH2STBY(OLDI0CR1_CHnSTBY_EN) |
                                        OLDI0CR1_CH3STBY(OLDI0CR1_CHnSTBY_EN) |
                                        OLDI0CR1_CH4STBY(OLDI0CR1_CHnSTBY_EN) );
    /* Set CTRL0 to HSYNC, CTRL1 to VSYNC, CTRL2 to DE */
    R_DEV_WRITE_REG(32, OLDI0CTRCR,     OLDI0CTRCR_CTR0SEL(0ul) |
                                        OLDI0CTRCR_CTR1SEL(0ul) |
                                        OLDI0CTRCR_CTR2SEL(0ul) );
    /* Set CH0 to output CH0, CH1 to CH1, CH2 to CH2, CH3 to CH3 */
    chSelValD1M1A = (OLDI0CHCR_CH1SEL(0ul) | OLDI0CHCR_CH2SEL(0ul)
                   | OLDI0CHCR_CH3SEL(0ul) | OLDI0CHCR_CH4SEL(0ul) );
    if (R_DEV_REVISION_EMU == R_DEV_GetVersion())
    {
#if 1
        R_BSP_STDIO_Printf("Use native OpenLDI channel assignment of R7F701460 POD.\n");
        R_BSP_STDIO_Printf(" Do not use OpenLDI channel re-assignment to fit to D1M1A assignment now.\n");
#else
        /* OpenLDI workaround for R7F701460 POD */
        /* Reassign channel setting:
            Set CH1 to output CH4, CH2 to CH1, CH3 to CH2, CH4 to CH3 */
        /* On Exchange Adapter select switch SW for [P45_8, P45_9]
           to "o" : OpenLDI emulation [pin assign to P44_10, P44_11] */
        R_BSP_STDIO_Printf("OpenLDI workaround for R7F701460 POD by channel re-assignment.\n");
        R_BSP_STDIO_Printf(" On Exchange set switch SW for [P45_8, P45_9] to 'o': OpenLDI emulation\n");
        chSelValD1M1A =   ((chSelValD1M1A + 0x0003) & 0x0003)
                        | ((chSelValD1M1A + 0x0030) & 0x0030)
                        | ((chSelValD1M1A + 0x0300) & 0x0300)
                        | ((chSelValD1M1A + 0x3000) & 0x3000);
#endif
    }
    R_DEV_WRITE_REG(32, OLDI0CHCR,      chSelValD1M1A);

    /* Set SKEW of all channels to 0ps */
    R_DEV_WRITE_REG(32, OLDI0SKEWCTR,   OLDI0SKEWCTR_SKEWC(OLDI0SKEWCTR_SKEWn_0PS) |
                                        OLDI0SKEWCTR_SKEW1(OLDI0SKEWCTR_SKEWn_0PS) |
                                        OLDI0SKEWCTR_SKEW2(OLDI0SKEWCTR_SKEWn_0PS) |
                                        OLDI0SKEWCTR_SKEW3(OLDI0SKEWCTR_SKEWn_0PS) |
                                        OLDI0SKEWCTR_SKEW4(OLDI0SKEWCTR_SKEWn_0PS) );
    /* Activate outputs; Start Operation; Select Mode 0 */
    R_DEV_WRITE_REG(32, OLDI0CR0,       OLDI0CR0_LVRES|OLDI0CR0_LVEN|OLDI0CR0_LVMD(0ul) );
#endif /* USE_VDCE_OPENLDI */

#if USE_VDCE_VODDR
    /* VO0 use interface: VO-DDR
       VO1 use interface: VO-DDR
       VI  use interface: N/A
    */
    *VoPinsAddr = (uint32_t) &(loc_D1M1A_VoVoDdrPinConfig[0]);
    *ViPinsAddr = 0;

    /* Set DDR output timing */
    R_DEV_WRITE_REG(32, VODDR0_TIMCNT1, VODDR0_TIMCNT1_CONFIG0(0ul) | 
                                        VODDR0_TIMCNT1_CONFIG1(0ul) |
                                        VODDR0_TIMCNT1_OUTTIM0(0ul) |
                                        VODDR0_TIMCNT1_OUTTIM1(0ul) );
    
    /* Set multiplexer to supply VDCE0 and VDCE1 blocks with VODDR_SYSCLK */
    R_DEV_WRITE_REG(32, RSDSCFG, RSDSCFG_VODDR_OEN);

    #if 0 /* Set by r_sys_vdce.c :: loc_Vdce_Pixel_SetClockM1A */
        /* Enable VO1 and start clock generation */
        R_DEV_WRITE_REG(32, VODDR0_SYSCNT,  VODDR0_SYSCNT_CLK_START | 
                                            VODDR0_SYSCNT_VO1_ENABLE);
    #endif
#endif /* USE_VDCE_VODDR */


#if USE_VDCE_SERIALRGB
  /* As there is only one <VoPinsAddr>, OpenLDI and SerialRGB Pin Configuration will 
     be merged into one array if both switches are active.  */
  #if USE_VDCE_OPENLDI
    /* VO0 use interface: OpenLDI
       VO1 use interface: SerialRGB
       VI  use interface: N/A
    */
    *VoPinsAddr = (uint32_t) &(loc_D1M1A_VoOpenLdiSerialRgbPinConfig[0]);
    R_DEV_WRITE_REG(32, RSDSCFG, RSDSCFG_OLDI_OEN | RSDSCFG_SRGB1_OEN);
  #else
    /* VO0 use interface: N/A
       VO1 use interface: SerialRGB
       VI  use interface: N/A
    */
    *VoPinsAddr = (uint32_t) &(loc_D1M1A_VoSerialRgbPinConfig[0]);
    R_DEV_WRITE_REG(32, RSDSCFG, RSDSCFG_SRGB1_OEN);
  #endif /* USE_VDCE_OPENLDI */
#endif /* USE_VDCE_SERIALRGB */


    val = 0;
    /* video input selection VDCE0 */    
#if 1
    /* select BT656 on 2nd alternative input on VDCE0 Input
        (i.e. ITU port pins P42_[10..0] */
    val &= ~VDCECTL_VISL0;
#else
    /* select RGB on 3rd alternative input on VDCE0 Input
        (i.e. RGB666 port pins P42_[15..0] + P44_[5..1] */
    val |= VDCECTL_VISL0;
#endif

    /* video output assignment */
#if 1
    /* staight assignment */
    /* VDCE0 video channel 0 to VDCE0_VO port pins */
    /* VDCE1 video channel 1 to VDCE1_VO port pins */
    val &= ~VDCECTL_VOSL;
#else
    /* swapped channel assignment */
    /* VDCE0 video channel 0 to VDCE1_VO port pins */
    /* VDCE1 video channel 1 to VDCE0_VO port pins */
    val |= VDCECTL_VOSL;
#endif
    R_DEV_WRITE_REG(32, VDCECTL, ((val & 0x35u) | 0x10000u));
}



/*******************************************************************************
  Function: loc_InitSDRAMemMangoRaw
  
  Init SDRA(DBSC2) SDR-SDRAM Memc of D1M1H/D1M1A Mango with provided configuration.
  (SDRA supports up to 128 MByte)
  Use the register based setup.
*/
static void loc_InitSDRAMemMangoRaw(const r_sdra_params_t *params)
{
    uint32_t regval;
    uint32_t i;

    /* reset release MRSTC */   
    /* - Release reset for SDRA */
    regval  = R_DEV_READ_REG(32, LOC_MRSTC);
    regval |= MRSTC_SDRB0RES;
    R_DEV_WRITE_PROT_REG(LOC_PROTCMDMRST, LOC_MRSTC, regval);
    while (0 != R_DEV_READ_REG(32, LOC_PROTSMRST))  /* Read PROTSMRST */
    {
        /* PROTSMRST - register protection sequence fail */
    }
    
    #if defined BSP_SDRA_QOS_WRITE_RESTRICT || defined BSP_SDRA_QOS_READ_RESTRICT
    loc_InitSDRATransactionRestrictor();
    #endif

    R_DEV_WRITE_REG(32,DBPDCNT0,0x23000000);
    R_DEV_WRITE_REG(32,DBPDCNT0,0x23000001);

    R_DEV_WRITE_REG(32,DBCMDCNT,0x00000003);
    
    /* Wait 200 us (0.2 ms)
       - SDRA Initialization sequence step 4 (UM section 15.4.2) 
        R_TICK_WaitMS(0, 1); */
    for(i = 0; i < 48000; i++)
    {
        R_DEV_Nop();    /* => 48000 CPU cycles x 4.167ns */
    }
    
    R_DEV_WRITE_REG(32,DBCONF,params->vDBCONF); /* SDRAM Address Configuration */
    R_DEV_WRITE_REG(32,DBTR0, params->vDBTR0);  /* SDRAM Timing Configuration (1) */
    R_DEV_WRITE_REG(32,DBTR1, params->vDBTR1);  /* SDRAM Timing Configuration (2) */
    R_DEV_WRITE_REG(32,DBTR2, params->vDBTR2);  /* SDRAM Timing Configuration (3) */ 
    
    R_DEV_WRITE_REG(32,DBKIND,0x00000001);  /* Set SDR SDRAM operation */
    
    R_DEV_WRITE_REG(32,DBCMDCNT,0x00000002);/* Execute "Precharge All" */
    /* Wait 20 ns (0.00002 ms)
        - MT48LC4M32B2P DS SDRAM init sequence step 7
          Wait at least tRP time (min. 20ns wait after PRECHARGE ALL command)
        R_TICK_WaitMS(0, 1); */
    for(i = 0; i < 5; i++)
    {
        R_DEV_Nop();    /* => 5 CPU cycles x 4.167ns */
    }
    
    
    R_DEV_WRITE_REG(32,DBCMDCNT,0x00000004);/* Execute "Refresh" */
    /* Wait 70 ns (0.00007 ms)
        - MT48LC4M32B2P DS SDRAM init sequence step 9
          Wait at least tRFC time (min. 70ns wait after Issue an AUTO REFRESH command)
        R_TICK_WaitMS(0, 1); */
    for(i = 0; i < 17; i++)
    {
        R_DEV_Nop();    /* => 17 CPU cycles x 4.167ns */
    }
    
        
    R_DEV_WRITE_REG(32,DBCMDCNT,0x00000004);/* Execute "Refresh" */
    /* Wait 70 ns (0.00007 ms)
        - MT48LC4M32B2P DS SDRAM init sequence step 11
          Wait at least tRFC time (min. 70ns wait after Issue an AUTO REFRESH command)
        R_TICK_WaitMS(0, 1); */
    for(i = 0; i < 17; i++)
    {
        R_DEV_Nop();    /* => 17 CPU cycles x 4.167ns */
    }
    
    
    R_DEV_WRITE_REG(32,DBMRCNT,0x00000022); /* Execute "Mode Register Set" */
    /* Wait 20 ns (0.00002 ms)
        - MT48LC4M32B2P DS SDRAM init sequence step 13
          Wait at least tMRD time
          (tMRD = 2x tCK = 2x 6..10ns = min. 20ns wait after mode register programming)
        R_TICK_WaitMS(0, 1); */
    for(i = 0; i < 5; i++)
    {
        R_DEV_Nop();    /* => 5 CPU cycles x 4.167ns */
    }
    
    
    R_DEV_WRITE_REG(32,DBEN,0x00000001);    /* Enable SDRAM data operations */
    
    R_DEV_WRITE_REG(32,DBRFPDN1,params->vDBRFPDN1);  /* Refresh time: (tREF_AT/fRAM/nROWS)-SSCG_factor */
    R_DEV_WRITE_REG(32,DBRFPDN2,params->vDBRFPDN2);  /* Level 1 and Level 0 thrensholds */
    R_DEV_WRITE_REG(32,DBRFPDN0,0x00010000);         /* - Enable auto refresh
                                                        - No power-down
                                                      */
}


#if defined BSP_SDRA_QOS_WRITE_RESTRICT || defined BSP_SDRA_QOS_READ_RESTRICT
/*******************************************************************************
  Function: loc_InitSDRATransactionRestrictor
  
  Set SDRA Transaction Restrictor via define BSP_SDRA_QOS_WRITE_RESTRICT

  When enabled, the restrictor will pause either the read or write path in order 
  increase throughput for the other path.
  
  Examplarily for write restriction:
  - After processing <WTOVFL> read words, one write transaction is accepted during a period of <ITVLTIM> XCCLK Cycles.
  - If there are no read transactions for a specific timeout of <IDLTIM> XCCLK cycles, 
    the writes will be unrestricted.
  - So, write will only be restricted if read transactions are pending. 
  - The restrictor will not split burts if above conditions are met within a burst access.
    It will wait for their completion before changing states.
  
*/
void loc_InitSDRATransactionRestrictor(void)
{
  /* Set other define to zero */
  #ifdef BSP_SDRA_QOS_WRITE_RESTRICT
    #define BSP_SDRA_QOS_READ_RESTRICT  (0)
    #define LOC_RCTL                    (1)
  #else
    #define BSP_SDRA_QOS_WRITE_RESTRICT (0)
    #define LOC_RCTL                    (2)
  #endif

    uint32_t regval = 0;

    /* WTOVFL: Number of read operations before allowing single write operation WTOVFL = 2*ReadTransactions */
    regval |= ((BSP_SDRA_QOS_WRITE_RESTRICT) <<  1u);   /* WTOVFL: (0..127) */

    /* RTOVFL: Number of write operations before allowing single read operation. RTOVFL = 2*WriteTransactions */
    regval |= ((BSP_SDRA_QOS_READ_RESTRICT) <<  9u);    /* RTOVFL: (0..127) */

    /* RCTL: Restrictor mode */
    regval |= ((LOC_RCTL) <<  16u);                     /* RCTL: 0b01 = write restrict; 0b10 = read restrict */

    /* IDLTIM: Timeout before switching back to unrestricted mode. IDLTIM = XCCLK Cycles */
    /*     High values may waste time that could already be used by restricted masters to 'catch up', 
           while unrestricted master is not requesting. Low values may cause a stead but slowly 
           requesting unrestricted master to be interrupted unintentionally. */
    regval |= (0x80u << 24u);                           /* IDLTIM: (0..255) */
    R_DEV_WRITE_REG(32, SDRATRCTL,   regval);
    
    /* ITVLTIM: Timeslot to accept single request of restricted path. ITVLTIM = XCCLK Cycles */
    /*          If no single request is coming, unrestricted path is blocked fro ITVLTIM. Choose small value. */
    regval = 0x8u;                                      /* ITVLTIM: (0..255) */
    R_DEV_WRITE_REG(32, SDRATRINTVL, regval);
}
#endif /* BSP_SDRA_QOS_WRITE_RESTRICT || defined BSP_SDRA_QOS_READ_RESTRICT */



/*******************************************************************************
  Function: loc_SDRA_SelectIoDriveStrength
  
  SDRA(DBSC2) SDR-SDRAM Memc of D1M1A
  - Select driver strength of I/O buffers
*/
/* SDRA/SFMA I/O drive strength control Register (DSCTRL) */
#define DSCTRL_REG                  0xFFC06054u
#define DSCTRL_SDRA_BIT_CTLMASK     0x00003FFCuL
#define DSCTRL_SDRA_BIT_CTL5(val)   ((uint32_t)((val)<<12u))
#define DSCTRL_SDRA_BIT_CTL4(val)   ((uint32_t)((val)<<10u))
#define DSCTRL_SDRA_BIT_CTL3(val)   ((uint32_t)((val)<<8u))
#define DSCTRL_SDRA_BIT_CTL2(val)   ((uint32_t)((val)<<6u))
#define DSCTRL_SDRA_BIT_CTL1(val)   ((uint32_t)((val)<<4u))
#define DSCTRL_SDRA_BIT_CTL0(val)   ((uint32_t)((val)<<2u))

static void loc_SDRA_SelectIoDriveStrength(void)
{
    uint32_t regval = 0;
    uint32_t strength;
    /*
        strength = 0:   output buffer 25ohm/120MHz/3.3V
                         - IBIS model P2HUF120_CM0_3V_33VN
                         - Same as D1M1H V1.2.
        strength = 1:   output buffer 33ohm/120MHz/3.3V
                         - IBIS model P2HUF090_CM0_3V_33VN
        strength = 2:   output buffer 50ohm/120MHz/3.3V
                         - IBIS model P2HUF060_CM0_3V_33VN
        strength = 3:   output buffer 100ohm/120MHz/3.3V
                         - IBIS model P2HUF030_CM0_3V_33VN
    */
    strength = 0;
    
    regval = R_DEV_READ_REG(32, DSCTRL_REG);
    regval &= ~(DSCTRL_SDRA_BIT_CTLMASK);
    regval |= DSCTRL_SDRA_BIT_CTL5(strength);   /* ctrl group 5: Add[12:0], BA[1:0], CS, RAS, CAS, WE signals */
    regval |= DSCTRL_SDRA_BIT_CTL4(strength);   /* ctrl group 4: Data[31:24], DQM[3] signals */
    regval |= DSCTRL_SDRA_BIT_CTL3(strength);   /* ctrl group 3: Data[23:16], DQM[2] signals */
    regval |= DSCTRL_SDRA_BIT_CTL2(strength);   /* ctrl group 2: Data[15:8], DQM[1] signals */
    regval |= DSCTRL_SDRA_BIT_CTL1(strength);   /* ctrl group 1: Data[7:0], DQM[0] signals */
    regval |= DSCTRL_SDRA_BIT_CTL0(strength);   /* ctrl group 0: CLK, CKE signals */
    
    R_DEV_WRITE_REG(32, DSCTRL_REG, regval);
}


#if defined (BSP_INIT_QOS_SDRAM) || defined (BSP_INIT_QOS_VRAM)

/*******************************************************************************
  Function: loc_XC1_ConfigureQoS
  
  Configure XC1 arbiter to control bandwidth distribution in D1M1
  - Bandwidth distribution is static, and therefore requires to be adjusted
    according to the use-case.
*/

static void loc_XC1_ConfigureQoS(uint32_t port, r_xc1_qos_config_t *config)
{
    uint32_t addr_offset = 0x10 * port;
    
    R_DEV_WRITE_REG(32, QOS_MODE      + addr_offset, config->mode);
    R_DEV_WRITE_REG(32, QOS_MAX_COUNT + addr_offset, config->max_cnt);  
    R_DEV_WRITE_REG(32, QOS_MAX_RACC  + addr_offset, config->max_racc); 
    R_DEV_WRITE_REG(32, QOS_MAX_WACC  + addr_offset, config->max_wacc);
}
#endif
#ifdef BSP_INIT_QOS_SDRAM
static void loc_InitXC1QosSDRAM(void){
    loc_XC1_ConfigureQoS(QOS_PORT_CPU0, (r_xc1_qos_config_t *)&qos_config_max[0]);
    loc_XC1_ConfigureQoS(QOS_PORT_CPU1, (r_xc1_qos_config_t *)&qos_config_max[0]);
    loc_XC1_ConfigureQoS(QOS_PORT_VACC0,(r_xc1_qos_config_t *)&qos_config_max[0]);
    loc_XC1_ConfigureQoS(QOS_PORT_VACC1,(r_xc1_qos_config_t *)&qos_config_max[0]);
    loc_XC1_ConfigureQoS(QOS_PORT_JCUA, (r_xc1_qos_config_t *)&qos_config_max[0]);
    loc_XC1_ConfigureQoS(QOS_PORT_GPU2D,(r_xc1_qos_config_t *)&qos_config_dhd_sdram[0]);
    loc_XC1_ConfigureQoS(QOS_PORT_ETNB, (r_xc1_qos_config_t *)&qos_config_max[0]);
}
#endif
#ifdef BSP_INIT_QOS_VRAM
static void loc_InitXC1QosVRAM(void){
    loc_XC1_ConfigureQoS(QOS_PORT_CPU0, (r_xc1_qos_config_t *)&qos_config_max[0]);
    loc_XC1_ConfigureQoS(QOS_PORT_CPU1, (r_xc1_qos_config_t *)&qos_config_max[0]);
    loc_XC1_ConfigureQoS(QOS_PORT_VACC0,(r_xc1_qos_config_t *)&qos_config_max[0]);
    loc_XC1_ConfigureQoS(QOS_PORT_VACC1,(r_xc1_qos_config_t *)&qos_config_max[0]);
    loc_XC1_ConfigureQoS(QOS_PORT_JCUA, (r_xc1_qos_config_t *)&qos_config_max[0]);
    loc_XC1_ConfigureQoS(QOS_PORT_GPU2D,(r_xc1_qos_config_t *)&qos_config_dhd_vram[0]);
    loc_XC1_ConfigureQoS(QOS_PORT_ETNB, (r_xc1_qos_config_t *)&qos_config_max[0]);
}
#endif

/*******************************************************************************
  Function: loc_SysGfxInitD1M1
  
    Board and device specific init 
*/
void loc_SysGfxInitD1M1(void) 
{
    /* get the device type  */
    r_dev_Device_t device = R_DEV_GetDev();

    if (0 != (R_BSP_Capabilities() & R_BSP_HAS_SDRAM))
    {
        if ((device == R_DEV_R7F701441)
            || (device == R_DEV_R7F701418))
        {
            /* SDRAM controller (SDRA) in D1M1A
               w/ 2x 16-bit SDRAM
               Micron MT48LC16M16A2P-6A:G size: 64 MByte (2x 32 MByte) */
            loc_SDRA_SelectIoDriveStrength();
            loc_InitSDRAMemMangoRaw( &MT48LC16M16A2P_120MHz );
        }
        else
        {
            /* SDRAM controller (SDRA) in D1M1H
               w/ 1x 32-bit SDRAM
               Micron MT48LC4M32B2P size: 16 MByte */
            loc_InitSDRAMemMangoRaw( &MT48LC4M32B2P_100MHz );
        }

#ifdef BSP_INIT_QOS_SDRAM
        loc_InitXC1QosSDRAM(); /* Configure QoS so that D/AVE HD doesn't distort video output (SDRAM case) */
#endif
    }

#ifdef BSP_INIT_QOS_VRAM
    loc_InitXC1QosVRAM();   /* Configure QoS so that D/AVE HD doesn't distort video output (VRAM case) */
#endif

    /* Enable the preload buffer PRLB for D1M1A or D1M1-V2 */
    #ifndef BSP_PRLB_DISABLE
        if ((device == R_DEV_R7F701441)
            || (device == R_DEV_R7F701442)
            || (device == R_DEV_R7F701418))
        {
            uint32_t u32_temp;
            
            u32_temp = PRLnREG_PRLEN | PRLnREG_PRLISS | PRLnREG_PRLTO
                #if 1 == PRLB_EN_VDCE0_LAYER0
                    | PRLnREG_PRLID0_EN
                #else
                    | PRLnREG_PRLID0_DIS
                #endif
                #if 1 == PRLB_EN_VDCE1_LAYER0
                    | PRLnREG_PRLID1_EN
                #else
                    | PRLnREG_PRLID1_DIS
                #endif
                    | PRLnREG_PRLID2_DIS
              ; /* All OR'ed into <u32_temp> */
            R_DEV_WRITE_REG(32, PRL0REG, u32_temp|PRLnREG_PRLEN);
            
            u32_temp = PRLnREG_PRLEN | PRLnREG_PRLISS | PRLnREG_PRLTO
                #if 1 == PRLB_EN_VDCE0_LAYER1
                    | PRLnREG_PRLID0_EN
                #else
                    | PRLnREG_PRLID0_DIS
                #endif
                #if 1 == PRLB_EN_VDCE1_LAYER3
                    | PRLnREG_PRLID1_EN
                #else
                    | PRLnREG_PRLID1_DIS
                #endif
                #if 1 == PRLB_EN_VDCE0_OIR
                    | PRLnREG_PRLID2_EN
                #else
                    | PRLnREG_PRLID2_DIS
                #endif
              ; /* All OR'ed into <u32_temp> */
            R_DEV_WRITE_REG(32, PRL1REG, u32_temp|PRLnREG_PRLEN);
            
            
            u32_temp = PRLnREG_PRLEN | PRLnREG_PRLISS | PRLnREG_PRLTO
                #if 1 == PRLB_EN_VDCE0_LAYER2
                    | PRLnREG_PRLID0_EN
                #else
                    | PRLnREG_PRLID0_DIS
                #endif
                #if 1 == PRLB_EN_VDCE1_LAYER2
                    | PRLnREG_PRLID1_EN
                #else
                    | PRLnREG_PRLID1_DIS
                #endif
                    | PRLnREG_PRLID2_DIS
              ; /* All OR'ed into <u32_temp> */
            R_DEV_WRITE_REG(32, PRL2REG, u32_temp|PRLnREG_PRLEN);
            
            
            u32_temp = PRLnREG_PRLEN | PRLnREG_PRLISS | PRLnREG_PRLTO
                #if 1 == PRLB_EN_VDCE0_LAYER3
                    | PRLnREG_PRLID0_EN
                #else
                    | PRLnREG_PRLID0_DIS
                #endif
                #if 1 == PRLB_EN_VDCE1_LAYER1
                    | PRLnREG_PRLID1_EN
                #else
                    | PRLnREG_PRLID1_DIS
                #endif
                    | PRLnREG_PRLID2_DIS
              ; /* All OR'ed into <u32_temp> */
            R_DEV_WRITE_REG(32, PRL3REG, u32_temp|PRLnREG_PRLEN);
        }
    #endif /*BSP_PRLB_DISABLE*/
    
    
}

/*******************************************************************************
  Function: R_BSP_SYS_GFX_LowInitD1M1

    Board and device specific init
*/
void loc_SysLowInitD1M1(void)
{
    loc_SysGfxInitD1M1();
}



