/*
****************************************************************************
PROJECT : BSP - GFX 
FILE    : $Id: r_bsp_sys_gfx.c 16482 2018-12-07 10:16:09Z matthias.nippert@renesas.com $
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
Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.     

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
#include "r_bsp_gfx_api.h"
#include "r_bsp_gfx_sys.h"
#include "r_bsp_board_config.h"

#ifdef BSP_INIT_SFMA
 #include "r_sfma_calib.h"
 #include "r_sfma_api.h"
 #include "r_sfcdb_macronix.h"
 #include "r_bsp_stdio_api.h"
#endif /* BSP_INIT_SFMA */

#ifdef BSP_INIT_NFMA
 #include "r_config_d1x.h"
 #include "r_nfma_init_memcpy.h"
#endif /* BSP_INIT_NFMA */

#ifdef BSP_INIT_HYPB_FLASH_R
 #include "r_hypb_api.h"
 #include "r_hypdb_issi.h"
 #include "r_bsp_stdio_api.h"
#endif /* BSP_INIT_HYPB_FLASH_R */

#ifdef BSP_INIT_HYPB_RAM_RW
 #include "r_hypb_api.h"
 #include "r_hypdb_issi.h"
 #include "r_bsp_stdio_api.h"
#endif /* BSP_INIT_HYPB_RAM_RW */

#ifdef BSP_INIT_OCTA_FLASH_R
 #include "r_octa_api.h"
 #include "r_octacdb_macronix.h"
 #include "r_octa_calib.h"
 #include "r_bsp_stdio_api.h"
#endif /* BSP_INIT_OCTA_FLASH_R */

#ifdef BSP_INIT_OCTA_RAM_RW
 #include "r_octa_api.h"
 #include "r_octacdb_macronix.h"
  #include "r_octa_calib.h"
 #include "r_bsp_stdio_api.h"
#endif /* BSP_INIT_OCTA_RAM_RW */


/*******************************************************************************
  Section: Local Defines
*/

/* Default SMFA config when SFMA initialisation is enabled: SFMA0 active only */
#ifndef BSP_USE_SFMA0
    #define BSP_USE_SFMA0 1
#endif
#ifndef BSP_USE_SFMA1
    #define BSP_USE_SFMA1 0
#endif
#ifndef BSP_USE_SFMA2
    #define BSP_USE_SFMA2 0
#endif
#if (BSP_USE_SFMA1==1) && (BSP_USE_SFMA2==1)
    #error SFMA1 and SFMA2 cannot be used simultaneously
#endif

#ifdef BSP_INIT_SFMA
/* SFMA setup for Macronics MX25L51245G device only

    D1L1 (144 pin) (40MHZ SDR)  1 x MX25L51245G 512Mb: 536,870,912 x 1 bit (64MByte)

    D1L2           (80MHZ DDR)  1 x MX25L51245G 512Mb: 536,870,912 x 1 bit (64MByte)

    D1L2H and above(120MHZ DDR) 2 x MX25L51245G 512Mb: 536,870,912 x 1 bit (128MByte)
    
    D1M1A 
     a) SFMA0: As D1L2H            + SFMA1 8-bit 40MHz
     b) SFMA0 4-bit + SFMA2 4-bit  + SFMA1 8-bit 40MHz
*/

/* Clock regs */
#define CKSC_ISFMAD_CTL             (0xFFF85380u)
#define CKSC_ISFMAS_CTL             (0xFFF85340u)


/*  Default calibration for Serial flash memory (Macronix device only).

    SPBCLK clock phase shift         3 (range 0 - 7, written to the upper 16 bits)
    Sampling point phase shift       1 (range 0 - 7, written to the lower 16 bits)
*/
#define SFMA_SPBCLK_CLOCK_PHASE_SHIFT     (3ul)

/* general setting for most devices */
#define SFMA_SAMPLING_POINT_PHASE_SHIFT   (1ul)
#define SFMA_DEFAULT_CAL                  ((uint32_t)((SFMA_SPBCLK_CLOCK_PHASE_SHIFT << 16) | SFMA_SAMPLING_POINT_PHASE_SHIFT))

/* D1M1 requires a different default cal */
#define SFMA_D1M1_SAMPLING_POINT_PHASE_SHIFT   (2u)
#define SFMA_D1M1_DEFAULT_CAL             ((uint32_t)((SFMA_SPBCLK_CLOCK_PHASE_SHIFT << 16) | SFMA_D1M1_SAMPLING_POINT_PHASE_SHIFT))
#endif /* BSP_INIT_SFMA */


/* HYPB macro */
#define HYPB_CH0 0
#define HYPB_D1MxE_SUPPORT_SIZE     (8*1024*1024)   /* 8MByte */

/* OCTA macro */
#define OCTA_UNIT                   (0u)
#ifndef BSP_OCTA_FLASH_CH
 #define BSP_OCTA_FLASH_CH          (0u)
#endif
#ifndef BSP_OCTA_RAM_CH
 #define BSP_OCTA_RAM_CH            (1u)
#endif

/*******************************************************************************
  Section: Local Constants
*/

static const r_dev_PinConfig_t loc_D1Mx_SfmaDefaultPinConfig[] = 
/*  Port Pin Func      Dir      Feedback     Pull        OpenDrain   HiCurr   HiSpeed  InputType */
{
    /* SFMA - serial Flash memory pins */
    {21, 0,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* O:  SFMA0CLK */
    {21, 1,  1u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* O:  SFMA0SSL */
    {21, 2,  1u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* IO: SFMA0IO[0..]0 */
    {21, 3,  1u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {21, 4,  1u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {21, 5,  1u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* IO: SFMA0IO[..3]0 */
    {21, 6,  1u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* IO: SFMA0IO[0..]1 */
    {21, 7,  1u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {21, 8,  1u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {21, 9,  1u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* IO: SFMA0IO[..3]1 */
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1} 
};

#if BSP_USE_SFMA1 == 1
static const r_dev_PinConfig_t loc_D1M1A_SfmaDefaultPinConfig[] = 
/*  Port Pin Func      Dir      Feedback     Pull        OpenDrain   HiCurr   HiSpeed  InputType */
{
    /* SFMA1 - serial Flash memory pins */
    {42, 0,  6u, R_DEV_PIN_OUT,    0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* O:  SFMA1CLK */
    {42, 1,  6u, R_DEV_PIN_OUT,    0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* O:  SFMA1SSL */
    {42, 2,  6u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* IO: SFMA1IO[0..]0 */
    {42, 3,  6u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {42, 4,  6u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {42, 5,  6u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* IO: SFMA1IO[..3]0 */
    {42, 6,  6u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* IO: SFMA1IO[0..]1 */
    {42, 7,  6u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {42, 8,  6u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1},
    {42, 9,  6u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* IO: SFMA1IO[..3]1 */
    
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1} 
};

static const r_dev_PinConfig_t loc_D1MxE_SfmaDefaultPinConfig[] = 
/*  Port Pin Func      Dir      Feedback     Pull        OpenDrain   HiCurr   HiSpeed  InputType */
{
    /* SFMA1 - serial Flash memory pins */
    {22, 0,  1u, R_DEV_PIN_OUT,    0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* O:  SFMA1CLK */
    {22, 1,  1u, R_DEV_PIN_OUT,    0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* O:  SFMA1SSL */
    {22, 2,  1u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* IO: SFMA1IO[0..]0 */
    {22, 3,  1u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {22, 4,  1u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {22, 5,  1u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* IO: SFMA1IO[..3]0 */
    {22, 6,  1u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* IO: SFMA1IO[0..]1 */
    {22, 7,  1u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {22, 8,  1u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {22, 9,  1u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* IO: SFMA1IO[..3]1 */
    
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1} 
};
#endif

#if BSP_USE_SFMA2 == 1
static const r_dev_PinConfig_t loc_D1M1A_Sfma2PinConfig[] = 
/*  Port Pin Func      Dir      Feedback     Pull        OpenDrain   HiCurr   HiSpeed  InputType */
{
    /* SFMA1 - serial Flash memory pins */
    {22,10,  4u, R_DEV_PIN_OUT   , 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* O:  SFMA2CLK */
    {45,13,  5u, R_DEV_PIN_OUT   , 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* O:  SFMA2SSL */
    {21, 6,  4u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* IO: SFMA2IO[0..]0 */
    {21, 7,  4u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {21, 8,  4u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {21, 9,  4u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* IO: SFMA2IO[..3]0 */
    
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1} 
};
#endif

#if defined(BSP_INIT_HYPB_FLASH_R) || defined(BSP_INIT_HYPB_RAM_RW) || defined(BSP_INIT_HYPB_PIN) \
 || defined(BSP_INIT_OCTA_FLASH_R) || defined(BSP_INIT_OCTA_RAM_RW) || defined(BSP_INIT_OCTA_PIN)

static const r_dev_PinConfig_t loc_D1M1A_HyperOctaDefaultPinConfig[] = 
/*  Port Pin Func      Dir      Feedback     Pull        OpenDrain   HiCurr   HiSpeed  InputType */
{
    /* HYPB / OCTA bus serial Flash/RAM memory pins */
    {21, 0,  3u, R_DEV_PIN_OUT,    0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* O: MCK */
    {21, 1,  3u, R_DEV_PIN_OUT,    0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* O: MCS */
    {21, 2,  3u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* IO: NDQO[0..] */
    {21, 3,  3u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {21, 4,  3u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {21, 5,  3u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {21, 6,  3u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {21, 7,  3u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {21, 8,  3u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {21, 9,  3u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* IO: NDQO[..7] */
    {22,10,  3u, R_DEV_PIN_DIRECT, 1u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* O: MDQS */
    
    /* for OCTA mcp  */
    {45,13,  5u, R_DEV_PIN_OUT   , 0u, R_DEV_PIN_PULLNO, 0u, 0u, 1u, R_DEV_PIN_CMOS1}, /* O:  SFMA2SSL */
    
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1} 
};
#endif


static const r_xbus_Cfg_t xbus_cfg = 
{
    0,
    0,
    {{R_XBUS_AM_CACHE,0x1u,0x98000000u, 0x07F00000u}, /* Cached SDRAM (original addr: 0x58000000) mirror 128MB  */
    {R_XBUS_AM_CACHE, 0x1u,0xa0000000u, 0x1FF00000u},  /* Cached SFMA mirror (0xa000_0000 - 0xBFff_ffff)*/
    {R_XBUS_AM_DIRECT,0x1u,0x40000000u, 0x3FF00000u}, /* (disabled) buffered SDRAM + HYPB/OCTA (0x4000_0000 - 0x7FFF_FFFF) */
    {R_XBUS_AM_DIRECT,0x1u,0x3FCE8000u, 0x00300000u}}  /* (disabled) buffered VRAM */
};

/*******************************************************************************
  Section: Local Types 
*/

/*******************************************************************************
  Section: Local Functions declarations (board & device specific) 
*/
extern void loc_SysGfxInitD1M1(void); 
extern void loc_SysLowInitD1M1(void); 
extern void loc_InitVideoIOPortD1M1(uint32_t *VoPinsAddr, uint32_t *ViPinsAddr);
extern void loc_InitVideoIOPortD1M1A(uint32_t *VoPinsAddr, uint32_t *ViPinsAddr);

extern void loc_SysGfxInitD1M2(void); 
extern void loc_SysLowInitD1M2(void); 
extern void loc_InitVideoIOPortD1M2(uint32_t *VoPinsAddr, uint32_t *ViPinsAddr);

/*******************************************************************************
  Section: Local Functions declarations 
*/
static void loc_DisableVOPixelClocks(void);
static void loc_InitVideoIOPort(void);
static void loc_SFMA_SelectP21P22IoDriveStrength(void);

#ifdef BSP_INIT_SFMA
 void loc_SetupSFMA(uint32_t Unit);
#endif

#ifdef BSP_INIT_HYPB_FLASH_R
 void loc_SetupHyperBus_ExMem_FlashRead(void);
#endif /* BSP_INIT_HYPB_FLASH_R */

#ifdef BSP_INIT_HYPB_RAM_RW
 void loc_SetupHyperBus_ExMem_RamReadWrite(void);
#endif /* BSP_INIT_HYPB_RAM_RW */


#ifdef BSP_INIT_OCTA_FLASH_R
 void loc_SetupOctaBus_ExMem_FlashRead(void);
#endif /* BSP_INIT_HYPB_FLASH_R */

#ifdef BSP_INIT_OCTA_RAM_RW
 void loc_SetupOctaBus_ExMem_RamReadWrite(void);
#endif /* BSP_INIT_HYPB_RAM_RW */


static void loc_InitGfxBus(void); 


/*******************************************************************************
  Function: loc_DisableVOPixelClocks
  
  Disable the video output pixel clock as default.
*/

static void loc_DisableVOPixelClocks(void)
{
    r_dev_ClkSelConfig_t ClkSelCfg;
    
    ClkSelCfg.Cks       = R_DEV_CKS_VDCE0CK;
    ClkSelCfg.SrcId     = R_DEV_CKS_SRC_DISABLED;
    ClkSelCfg.Div       = 0;
    ClkSelCfg.StpReqMsk = 0;
    R_DEV_ClkIdSet(&ClkSelCfg);

    if ( 0 != (R_BSP_Capabilities() & R_BSP_HAS_VOUT1))
    {    
        ClkSelCfg.Cks       = R_DEV_CKS_VDCE1CK;
        R_DEV_ClkIdSet(&ClkSelCfg);
    }
}


/*******************************************************************************
  Function: loc_InitVideoIOPort
  
  Init port pins of Video I/O and select a video configuration.
*/

static void loc_InitVideoIOPort(void)
{
    uint32_t vo_pins_addr = 0; 
    uint32_t vi_pins_addr = 0; 
    
    /* get the device type and configure the pins */
    r_dev_Device_t device = R_DEV_GetDev();
    
    if (device == R_DEV_R7F701441)
    {
        /* D1M1A */  
        loc_InitVideoIOPortD1M1A(&vo_pins_addr, &vi_pins_addr);
    }
    else if ((device >= R_DEV_R7F701408) && (device <= R_DEV_R7F701412))
    {
        /* D1M2(H) */  
        loc_InitVideoIOPortD1M2(&vo_pins_addr, &vi_pins_addr);
    }
    else 
    {
        /* D1M1(H) and D1M1-V2 */
        loc_InitVideoIOPortD1M1(&vo_pins_addr, &vi_pins_addr);
    }
    
    if (0 != vo_pins_addr)
    {
        R_DEV_PinInit((const r_dev_PinConfig_t *) vo_pins_addr);
    }
    if (0 != vi_pins_addr)
    {
        R_DEV_PinInit((const r_dev_PinConfig_t *) vi_pins_addr);
    }
}

/*******************************************************************************
  Function: loc_SFMA_SelectP21P22IoDriveStrength
  
  Port P21 + P22 (SFMA / HyperBus / OctaBus) of D1M1A
  - Select driver strength of I/O buffers
*/
/* SDRA/SFMA I/O drive strength control Register (DSCTRL) */
#define DSCTRL_REG     0xFFC06054u
#define DSCTRL_SFMA_BIT_CTLMASK      0x003F0000uL
#define DSCTRL_SFMA2_CLK_BIT(val)    ((uint32_t)((val)<<20u))
#define DSCTRL_SFMA0_DAT_BIT(val)    ((uint32_t)((val)<<18u))
#define DSCTRL_SFMA0_CLK_BIT(val)    ((uint32_t)((val)<<16u))

static void loc_SFMA_SelectP21P22IoDriveStrength(void)
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
    regval &= ~(DSCTRL_SFMA_BIT_CTLMASK);
    regval |= DSCTRL_SFMA2_CLK_BIT(strength);   /* SFMA 2: clock signal  / HYPB+OCTA: MDQS signal  */
    regval |= DSCTRL_SFMA0_DAT_BIT(strength);   /* SFMA 0: data signals  / HYPB+OCTA: data signals */
    regval |= DSCTRL_SFMA0_CLK_BIT(strength);   /* SFMA 0: clock signal  / HYPB+OCTA: clock signal */
    
    R_DEV_WRITE_REG(32, DSCTRL_REG, regval);
}


/*******************************************************************************
  Function: loc_InitGfxBus
  
  Configure the Gfx bus (cache) 
*/
static void loc_InitGfxBus(void)
{
    /* Init XBus Bridge */
    if (R_XBUS_ERR_OK == R_XBUS_Init(0)) 
    {
        R_XBUS_Config(0u, (r_xbus_Cfg_t *)&xbus_cfg);
    }
}

#ifdef BSP_INIT_SFMA
/***********************************************************
  Function: loc_SetupSFMA

  SFMA setup and calibration procedure
*/

void loc_SetupSFMA(uint32_t Unit)
{
    r_sfma_Config_t Config;
    r_sfma_Error_t sfma_error;
    r_dev_Device_t device;

#ifdef SFMA_USE_BSP_CALIB
    sf_CalConfig_t CalConfig;
    uint32_t f_phclk;
    uint32_t f_b;
    uint32_t u32_temp;
    sf_Error_t sf_error;
#endif /* SFMA_USE_BSP_CALIB */

    /* common SFMA config */
    Config.AddressMode = R_SFMA_ADDRESS_32BIT;
    Config.SerialFlashMemoryPageSize = 256;
    Config.SerialFlashMemoryMaxClock = 80000000;
    Config.SerialFlashMemorySectorSize = 4096;
    Config.CacheMode = R_SFMA_CACHE_OFF;
    device = R_DEV_GetDev();

    /* D1M1 requires a different cal */
    if(device == R_DEV_R7F701405) /* D1M1 */
    {
        Config.Calibration = SFMA_D1M1_DEFAULT_CAL;
    }
    else
    {
        Config.Calibration = SFMA_DEFAULT_CAL;        
    }

    Config.PerformanceEnMode = R_SFMA_PER_EN_MODE_ENABLE;

    if((device == R_DEV_R7F701441) || (device == R_DEV_R7F701418) || (device == R_DEV_R7F701442)) /* D1M1A and newer */
    {
        Config.SerialFlashMemorySize = ((64 * 1024 * 1024) * 2); /* 128 MByte */
        Config.MemoryNum = R_SFMA_MEMORY_DUAL;
        Config.Command = &r_sfc_Macronix_32bitAddrCmdPerformanceEnhance_80MHz;
        
            /* SFMA0 and SFMA2 share pins. 
               Therefore, SFMA0 can only be used in 4-bit mode when SFMA2 is also active.
               SFMA2 only supports 4-bit mode. */
        if( ((0 == Unit) && (BSP_USE_SFMA2 == 1))
            /* SFMA2 only supports 4-bit mode. */
            || (2 == Unit) )
        {
            Config.SerialFlashMemorySize = ((64 * 1024 * 1024) * 1); /* 64 MByte */
            Config.MemoryNum = R_SFMA_MEMORY_SINGLE;
        }
            /* SFMA1 is limited to 40MHz instead of 80MHz DDR mode*/
        if (1 == Unit)
        {
            Config.SerialFlashMemoryMaxClock = 40000000;
            Config.Command = &r_sfc_Macronix_32bitAddrCmdPerformanceEnhance_40MHz;
        }
    }
    else if(device >= R_DEV_R7F701403) /* D1L2H and above */
    {
        Config.SerialFlashMemorySize = ((64 * 1024 * 1024) * 2); /* 128 MByte */
        Config.MemoryNum = R_SFMA_MEMORY_DUAL;
        Config.Command = &r_sfc_Macronix_32bitAddrCmdPerformanceEnhance_80MHz;
    }
    else if(device >= R_DEV_R7F701402) /* D1L2 and above */
    {
        Config.SerialFlashMemorySize = ((64 * 1024 * 1024) * 1); /* 64 MByte */
        Config.MemoryNum = R_SFMA_MEMORY_SINGLE;
        Config.Command = &r_sfc_Macronix_32bitAddrCmdPerformanceEnhance_80MHz;
    }
    else /* D1L1 */
    {
        /* Calibration procedure not supported for D1L1 */
        Config.AddressMode = R_SFMA_ADDRESS_32BIT;
        Config.SerialFlashMemoryPageSize = 256;
        Config.SerialFlashMemoryMaxClock = 40000000;
        Config.SerialFlashMemorySectorSize = 4096;
        Config.SerialFlashMemorySize = ((64 * 1024 * 1024) * 1); /* 64 MByte */
        Config.MemoryNum = R_SFMA_MEMORY_SINGLE;
        Config.Command = &r_sfc_Macronix_32bitAddrCmdPerformanceEnhance_40MHz;
        Config.CacheMode = R_SFMA_CACHE_OFF;

        /* config to external address space quad IO */
        Config.Mode = R_SFMA_MODE_EX_SPACE;
        Config.DataTransferMode = R_SFMA_SDR_QUAD_IO;

        /* Init the SFMA */
        sfma_error = R_SFMA_Init(Unit);
        if(R_SFMA_ERR_OK == sfma_error)
        {
            /* open */
            sfma_error = R_SFMA_Open(Unit, &Config);
            if(R_SFMA_ERR_OK == sfma_error)
            {
                /* set address range */
                sfma_error = R_SFMA_AccessAddressSet(Unit, 0x00000000UL, R_SFMA_ACCESS_RANGE_64MB);
                if(R_SFMA_ERR_OK == sfma_error)
                {
                    R_BSP_STDIO_Printf("SFMA configured for D1L1\n");
                }
            }
            else
            {
                /* error */
                R_BSP_STDIO_Printf("SFMA %u configuration error\n", Unit);
            }
        }
        else
        {
            /* error */
            R_BSP_STDIO_Printf("SFMA %u configuration error\n", Unit);
        }

        return;
    }
    
    
#ifdef SFMA_USE_BSP_CALIB
    /* Data write & erase is done in SPI single IO mode only */
    Config.Mode = R_SFMA_MODE_SPI_OPERATING;
    Config.DataTransferMode = R_SFMA_SDR_SINGLE_IO;

    /* configure the calibration data */
    /* we will write the minimum data we can but this is dependent of the sector erase size */
    if(Config.MemoryNum == R_SFMA_MEMORY_DUAL)
    {
        /* 2 devices */
        CalConfig.TestSize = Config.SerialFlashMemorySectorSize * 2;
    }
    else
    {
        /* 1 device */
        CalConfig.TestSize = Config.SerialFlashMemorySectorSize;
    }

    /* base memory of the Serial flash data */
    CalConfig.BaseMemoryAddr = 0x10000000uL;

    /* test area right at the top */
    CalConfig.TestStartAddressOs = Config.SerialFlashMemorySize - CalConfig.TestSize;

    /* Write data to flash for preparation*/
    R_BSP_STDIO_Printf("SFMA Calibration:\n");

    /* init the calibration variables */
    SF_CalibrationInit(Unit, &CalConfig);

    /* disable */
    Config.PerformanceEnMode = R_SFMA_PER_EN_MODE_DISABLE;

    /* perform a data write to the SFMA */
    sf_error = SF_CalibrationWriteData(Unit, &Config);

    /* write ok, so run the calibration routine */
    if(sf_error == SF_ERR_OK)
    {
        /* Base clock Freq */
        f_phclk = R_DEV_ClkFrequencyHz(R_DEV_CKS_SFMA) * (R_DEV_READ_REG(32, CKSC_ISFMAD_CTL) + 1);
        f_b = R_DEV_ClkFrequencyHz(R_DEV_CKS_SFMA);
    
        /* PLL0 or 1 */
        u32_temp = R_DEV_READ_REG(32, CKSC_ISFMAS_CTL);
        if(u32_temp == 0x01)
        {
            R_BSP_STDIO_Printf(" - f_phclk (PLL0) '%dHz' \n", f_phclk);
        }
        else
        {
            R_BSP_STDIO_Printf(" - f_phclk (PLL1) '%dHz'\n", f_phclk);
        }
    
        R_BSP_STDIO_Printf(" - f_b '%dHz'\n", f_b);
    
        /* config to external address space quad IO */
        Config.Mode = R_SFMA_MODE_EX_SPACE;
        Config.DataTransferMode = R_SFMA_DDR_QUAD_IO;
    
        /* enable */
        Config.PerformanceEnMode = R_SFMA_PER_EN_MODE_ENABLE;

        sf_error = SF_CalibrationRun(Unit, &Config, f_phclk, f_b);
    }
    else
    {
        /* nothing, catch below */
    }

    /* pass or fail */
    if(sf_error == SF_ERR_OK)
    {
        R_BSP_STDIO_Printf(" - Passed\n\n");
    }
    else
    {
        R_BSP_STDIO_Printf(" - failed\n\n");
    }

#else /* SFMA_USE_BSP_CALIB */

    /* config to external address space quad IO */
    Config.Mode = R_SFMA_MODE_EX_SPACE;
    Config.DataTransferMode = R_SFMA_DDR_QUAD_IO;

    /* Init the SFMA */
    sfma_error = R_SFMA_Init(Unit);
    if(R_SFMA_ERR_OK == sfma_error)
    {
        /* open */
        sfma_error = R_SFMA_Open(Unit, &Config);
        if(R_SFMA_ERR_OK == sfma_error)
        {
            /* set address range 128MByte */
            if(Config.SerialFlashMemorySize == (128 *1024 *1024))
            {
                sfma_error = R_SFMA_AccessAddressSet(Unit, 0x00000000UL, R_SFMA_ACCESS_RANGE_128MB);
            }
            else /* 64MByte */
            {
                sfma_error = R_SFMA_AccessAddressSet(Unit, 0x00000000UL, R_SFMA_ACCESS_RANGE_64MB);
            }

            if(R_SFMA_ERR_OK == sfma_error)
            {
                R_BSP_STDIO_Printf("SFMA %u configured using default calibration\n", Unit);
            }
        }
        else
        {
            /* error */
            R_BSP_STDIO_Printf("SFMA %u configuration error\n", Unit);
        }
    }
    else
    {
        /* error */
        R_BSP_STDIO_Printf("SFMA %u configuration error\n", Unit);
    }
#endif /* SFMA_USE_BSP_CALIB */
}
#endif /* BSP_INIT_SFMA */


/***********************************************************
  Function: loc_SetupOctaBus_ExMem_FlashRead

  OCTA initialization for OctaFlash 
   - External Address Space Read mode
   - OctaFlash device: MX25LM51245G Macronix (64MByte) (latency set to 8)
*/
#ifdef BSP_INIT_OCTA_FLASH_R
void loc_SetupOctaBus_ExMem_FlashRead(void)
{
    r_octa_Error_t          result;
    r_octa_Config_t         config;
#ifdef OCTA_USE_BSP_CALIB
    uint8_t                 delay;
#endif /*OCTA_USE_BSP_CALIB */
    
    /* Precondition:
       Clock Controller CKSC_ISFMAS_CTL clock selector output
       have to be configured to 160 MHz before calling this function.
       (This will result in 80MHz/DDR operation of OCTA.)
    */

    /* Init */
    result = R_OCTA_Init(OCTA_UNIT);
    if (result != R_OCTA_ERR_OK)
    {
        R_BSP_STDIO_Printf("OCTA init error\n");
    }
    /* Open */
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
    
#ifdef OCTA_USE_BSP_CALIB
    
    R_OCTA_DeInit(OCTA_UNIT);
    
    config.CalAddress         = 0x00000000;
    /* Execute preparration calibration */
    result = OCTA_CalibrationPreparation(OCTA_UNIT, BSP_OCTA_FLASH_CH, &config);
    if (result == R_OCTA_ERR_OK)
    {
        result = OCTA_Calibration(OCTA_UNIT, BSP_OCTA_FLASH_CH, &config);
        if (result == R_OCTA_ERR_OK)
        {
            R_OCTA_Init(OCTA_UNIT);
            result = R_OCTA_GetCal(OCTA_UNIT, BSP_OCTA_FLASH_CH, &delay);
            if (result == R_OCTA_ERR_OK)
            {
                config.DQSDelay.Delay = delay;
            }
            else
            {
                R_BSP_STDIO_Printf("OCTA get calibration error\n");
            }
        }
        else
        {
            R_BSP_STDIO_Printf("OCTA calibration error\n");
        }
    }
    else
    {
        R_BSP_STDIO_Printf("OCTA preparation calibration error\n");
    }
#endif /* OCTA_USE_BSP_CALIB */

    result = R_OCTA_Open(OCTA_UNIT, BSP_OCTA_FLASH_CH, &config);
    if (result != R_OCTA_ERR_OK)
    {
        R_BSP_STDIO_Printf("OCTA open error\n");
    }
}
#endif /* BSP_INIT_OCTA_FLASH_R */


/***********************************************************
  Function: loc_SetupOctaBus_ExMem_RamReadWrite

  OCTA initialization for OctaRAM
   - External Address Space Read+Write mode
   - OctaRAM device: JSC64SSU8AGDY (latency 4; fixed latency)
*/
#ifdef BSP_INIT_OCTA_RAM_RW
void loc_SetupOctaBus_ExMem_RamReadWrite(void)
{
    r_octa_Error_t          result;
    r_octa_Config_t         config;
#ifdef OCTA_USE_BSP_CALIB
    uint8_t                 delay;
#endif /*OCTA_USE_BSP_CALIB */
    
    /* Precondition:
       Clock Controller CKSC_ISFMAS_CTL clock selector output
       have to be configured to 160 MHz before calling this function.
       (This will result in 80MHz/DDR operation of OCTA.)
    */

    /* Init - Must only be called once. If Flash is also activated, it will be called there */
  #ifndef BSP_INIT_OCTA_FLASH_R
    result = R_OCTA_Init(OCTA_UNIT);
    if (result != R_OCTA_ERR_OK)
    {
        R_BSP_STDIO_Printf("OCTA init error\n");
    }
  #endif
    
    /* Open */
    config.DeviceType         = R_OCTA_DEVICE_RAM;
    config.OpeMode            = R_OCTA_MODE_EX_SPACE;
    config.DataTransferMode   = R_OCTA_MODE_DOPI;
    config.AddressMode        = R_OCTA_ADDRESS_32BIT;
    config.MemorySize         = R_OCTADB_RAM_MEMORY_SIZE;
    config.SectorSize         = 0;
    config.PageSize           = 0;
    config.Command            = &r_octacdb_RamCmdTabl;
    config.RelaxSize          = 0;
    config.PreCycle           = R_OCTA_PRECYCLE_OFF;
    config.DQSDelay.EnableCnt = R_OCTADB_RAM_DQS_EN_CNT;
    config.DQSDelay.Delay     = 0x17;
    config.CalAddress         = 0xFFFFFFFFu;

#ifdef OCTA_USE_BSP_CALIB
    
    R_OCTA_DeInit(OCTA_UNIT);
    
    config.CalAddress         = 0x00000000;
    /* Execute preparration calibration */
    result = OCTA_CalibrationPreparation(OCTA_UNIT, BSP_OCTA_RAM_CH, &config);
    if (result == R_OCTA_ERR_OK)
    {
        result = OCTA_Calibration(OCTA_UNIT, BSP_OCTA_RAM_CH, &config);
        if (result == R_OCTA_ERR_OK)
        {
            R_OCTA_Init(OCTA_UNIT);
            result = R_OCTA_GetCal(OCTA_UNIT, BSP_OCTA_RAM_CH, &delay);
            if (result == R_OCTA_ERR_OK)
            {
                config.DQSDelay.Delay = delay;
            }
            else
            {
                R_BSP_STDIO_Printf("OCTA get calibration error\n");
            }
        }
        else
        {
            R_BSP_STDIO_Printf("OCTA calibration error\n");
        }
    }
    else
    {
        R_BSP_STDIO_Printf("OCTA preparation calibration error\n");
    }
#endif /* OCTA_USE_BSP_CALIB */

    result = R_OCTA_Open(OCTA_UNIT, BSP_OCTA_RAM_CH, &config);
    if (result != R_OCTA_ERR_OK)
    {
        R_BSP_STDIO_Printf("OCTA open error\n");
    }
}
#endif /* BSP_INIT_OCTA_RAM_RW */


/***********************************************************
  Function: loc_SetupHyperBus_ExMem_FlashRead

  HYPB initialization for HyperFlash 
   - External Address Space Read mode
   - HyperFlash device: S26KLxxxx (latency set to 8)
*/
#ifdef BSP_INIT_HYPB_FLASH_R
void loc_SetupHyperBus_ExMem_FlashRead(void)
{
    r_hypb_Error_t          result;
    r_hypb_Config_t         config;

    /* Precondition:
       Clock Controller CKSC_ISFMAS_CTL clock selector output
       have to be configured to 160 MHz before calling this function.
       (This will result in 80MHz/DDR operation of HYPB.)
    */

    /* Init */
    result = R_HYPB_Init(HYPB_CH0);
    if (result != R_HYPB_ERR_OK)
    {
        R_BSP_STDIO_Printf("HYPB init error\n");
    }
    
    /* Open */
    config.DeviceType    = R_HYPB_DEVICE_FLASH;
    config.OpeMode       = R_HYPB_MODE_EX_SPACE;
    config.MemorySize    = R_HYPDB_FLASH_MEMORY_SIZE;
    config.SectorSize    = R_HYPDB_FLASH_SECTOR_SIZE;
    config.WriteAlign    = R_HYPDB_FLASH_WRITE_BUF_SIZE;
    config.RelaxSize     = 0;
    config.ParamSec.Size = 0;
    config.ParamSec.Num  = 0;
    config.ParamSec.Pos  = R_HYPB_PARAM_SEC_TOP;
    config.Command       = &r_hypdb_FlashCmdTbl;
    
    result = R_HYPB_Open(HYPB_CH0, &config);
    if (result != R_HYPB_ERR_OK)
    {
        R_BSP_STDIO_Printf("HYPB open error\n");
    }
}
#endif /* BSP_INIT_HYPB_FLASH_R */


/***********************************************************
  Function: loc_SetupHyperBus_ExMem_RamReadWrite

  HYPB initialization for HyperRAM
   - External Address Space Read+Write mode
   - HyperRAM device: IS66WVH16M8BLL (latency 4; fixed latency)
*/
#ifdef BSP_INIT_HYPB_RAM_RW
void loc_SetupHyperBus_ExMem_RamReadWrite(void)
{
    r_hypb_Error_t          result;
    r_hypb_Config_t         config;

    /* Precondition:
       Clock Controller CKSC_ISFMAS_CTL clock selector output
       have to be configured to 160 MHz before calling this function.
       (This will result in 80MHz/DDR operation of HYPB.)
    */

    /* Init */
    result = R_HYPB_Init(HYPB_CH0);
    if (result != R_HYPB_ERR_OK)
    {
        R_BSP_STDIO_Printf("HYPB init error\n");
    }
    
    /* Open */
    config.DeviceType    = R_HYPB_DEVICE_RAM;
    config.OpeMode       = R_HYPB_MODE_EX_SPACE;
    config.MemorySize    = R_HYPDB_RAM_MEMORY_SIZE;
    config.SectorSize    = 0;
    config.WriteAlign    = 0;
    config.RelaxSize     = 0;
    config.ParamSec.Size = 0;
    config.ParamSec.Num  = 0;
    config.ParamSec.Pos  = R_HYPB_PARAM_SEC_TOP;
    config.Command       = &r_hypdb_RamCmdTabl;

    result = R_HYPB_Open(HYPB_CH0, &config);
    if (result != R_HYPB_ERR_OK)
    {
        R_BSP_STDIO_Printf("HYPB open error\n");
    }
}
#endif /* BSP_INIT_HYPB_RAM_RW */


/*******************************************************************************
  Function: R_BSP_SYS_GFX_LowInit
  
  See: r_bsp_gfx_sys.h for details    
*/
void R_BSP_SYS_GFX_LowInit(void)
{
    /* get the device type  */
    r_dev_Device_t device = R_DEV_GetDev();
    
    if ((device >= R_DEV_R7F701408) && (device <= R_DEV_R7F701412))
    {
        loc_SysLowInitD1M2();
    }
    else 
    {
        loc_SysLowInitD1M1();
    }
    
}

/*******************************************************************************
  Function: R_BSP_SYS_GFX_Init
  
  See: r_bsp_gfx_sys.h for details    
*/
void R_BSP_SYS_GFX_Init(void) 
{
    volatile uint8_t io_state; 
    /* get the device type  */
    r_dev_Device_t device = R_DEV_GetDev();
    
    loc_DisableVOPixelClocks();
    loc_InitVideoIOPort();

    /* Store original IO state (enabled or disabled) */
    io_state = R_DEV_PinOutStateGet(); 
    #ifdef BSP_FORCE_SFLASH_IO
        /* Force Serial Flash IO to override the global IO disable setting */
        R_DEV_PinOutStateSet(1); 
    #endif
    R_DEV_PinInit(loc_D1Mx_SfmaDefaultPinConfig);
    if ((device == R_DEV_R7F701441)
        || (device == R_DEV_R7F701442))
    {
#if BSP_USE_SFMA1==1
        if (R_DEV_REVISION_EMU == R_DEV_GetVersion())
        {
            /* use R7F701460 POD */
            R_DEV_PinInit(loc_D1MxE_SfmaDefaultPinConfig);
        }
        else
        {
            R_DEV_PinInit(loc_D1M1A_SfmaDefaultPinConfig);
        }
#endif
#if BSP_USE_SFMA2 == 1
        R_DEV_PinInit(loc_D1M1A_Sfma2PinConfig);
#endif
        loc_SFMA_SelectP21P22IoDriveStrength();
    }

    /* restore previous IO state */
    R_DEV_PinOutStateSet(io_state); 

    loc_InitGfxBus();

    if ((device >= R_DEV_R7F701408) && (device <= R_DEV_R7F701412))
    {
        loc_SysGfxInitD1M2();
    }
    else 
    {
        loc_SysGfxInitD1M1();
    }
    
#ifdef BSP_INIT_SFMA 
    #if BSP_USE_SFMA0
        loc_SetupSFMA(0);
    #endif
    #if BSP_USE_SFMA1
        loc_SetupSFMA(1);
    #endif
    #if BSP_USE_SFMA2
        loc_SetupSFMA(2);
    #endif
#endif /* BSP_INIT_SFMA */

#if defined(BSP_INIT_HYPB_FLASH_R) || defined(BSP_INIT_HYPB_RAM_RW) || defined(BSP_INIT_HYPB_PIN)
    if ((device == R_DEV_R7F701441)
        || (device == R_DEV_R7F701442)
        || (device == R_DEV_R7F701418))
    {
        R_DEV_PinInit(loc_D1M1A_HyperOctaDefaultPinConfig);

 #ifdef BSP_INIT_HYPB_FLASH_R
        loc_SetupHyperBus_ExMem_FlashRead();
 #endif /* BSP_INIT_HYPB_FLASH_R */

 #ifdef BSP_INIT_HYPB_RAM_RW
        loc_SetupHyperBus_ExMem_RamReadWrite();
 #endif /* BSP_INIT_HYPB_RAM_RW */
    }
#endif /* defined(BSP_INIT_HYPB_FLASH_R) || defined(BSP_INIT_HYPB_RAM_RW) */

#if defined(BSP_INIT_OCTA_FLASH_R) || defined(BSP_INIT_OCTA_RAM_RW) || defined(BSP_INIT_OCTA_PIN)
    if ((device == R_DEV_R7F701441)
        || (device == R_DEV_R7F701442)
        || (device == R_DEV_R7F701418))
    {
        R_DEV_PinInit(loc_D1M1A_HyperOctaDefaultPinConfig);

 #ifdef BSP_INIT_OCTA_FLASH_R
        loc_SetupOctaBus_ExMem_FlashRead();
 #endif /* BSP_INIT_HYPB_FLASH_R */

 #ifdef BSP_INIT_OCTA_RAM_RW
        loc_SetupOctaBus_ExMem_RamReadWrite();
 #endif /* BSP_INIT_OCTA_RAM_RW */
    }
#endif /* defined(BSP_INIT_OCTA_FLASH_R) || defined(BSP_INIT_OCTA_RAM_RW) */

#ifdef BSP_INIT_NFMA
    if (device == R_DEV_R7F701441)
    {
        NF_Init(NF_UNIT0);
        NF_MemCpy(NF_UNIT0, NF_FLASH_COPY_DST, NF_FLASH_COPY_START, NF_FLASH_COPY_SIZE);
    }
#endif /* BSP_INIT_NFMA */
}

