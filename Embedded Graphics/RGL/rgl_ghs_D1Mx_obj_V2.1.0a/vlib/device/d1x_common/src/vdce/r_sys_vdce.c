/*
****************************************************************************
PROJECT : VDCE driver
============================================================================
DESCRIPTION
VDCE system functions
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2013 - 2019
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

/******************************************************************************
  Title: Generic VDCE Support Functions

    VDCE driver Support Functions

******************************************************************************/

/*******************************************************************************
  Section: Includes
*/
#include "r_typedefs.h"
#include "r_ddb_api.h"
#include "r_dev_api.h"
#include "r_config_hwversion.h"
#include "r_config_vdce.h"
#include "r_config_vdce_num.h"
#include "r_vdce_api.h"
#include "r_vdce_sys.h"

/*******************************************************************************
  Section: Local Defines
*/

/*
*   The following is sample code, which may not respect the correct value of <Unit>, 
*   if both following conditions are satisfied:
*
*      * Both video channels are active
*      * One of the channel uses RSDS, OpenLDI or SerialRGB
*
*   Please make sure to remove sample code for that Unit that is not required to use 
*   one of the above features.
*/

/*
*   The following flags are used in several files.
*   Thus, these flags are best set globally, e.g. in a Makefile.
*
*       USE_VDCE_SERIALRGB  -   Configure VDCE to use Serial RGB on Pins "VDCE1 Out" [D1M1A],
*                               Configure VDCE to use Serial RGB on Pins "VDCE0 Out" [D1M1-V2]
*       USE_VDCE_OPENLDI    -   Configure VDCE to use Open LDI on Pins "VDCE0 Out" [D1M1A]
*       USE_VDCE_VODDR      -   Configure VDCE to use VO-DDR on Pins "VDCE0 Out" [D1M1A]
*
*       TODO for system integrator:
*       At the moment, for RSDS, OpenLDI and SerialRGB, divider is set for both Units 0 and 1.
*       It should just be set for the Unit currently using the respective mode.
*/

/*
*   USE_VDCE_OPENLDI
*       0 = Parallel Video Output (default, if undefined)
*       1 = Open LDI
*/
#ifndef USE_VDCE_OPENLDI
#define USE_VDCE_OPENLDI                (0u)
#endif

/*
*   USE_VDCE_VODDR
*       0 = Parallel Video Output (default, if undefined)
*       1 = VO DDR Video Output
*/
#ifndef USE_VDCE_VODDR
#define USE_VDCE_VODDR                  (0u)
#endif

/*
*   USE_VDCE_SERIALRGB
*       0 = Parallel Video Output (default, if undefined)
*       1 = Serial RGB
*/
#ifndef USE_VDCE_SERIALRGB
#define USE_VDCE_SERIALRGB              (0u)
#endif

/*
*   USE_VDCE_SERIALRGB_SPEED
*       3 = Triple Speed (default, if undefined)
*       4 = Quadruple Speed
*/
#if USE_VDCE_SERIALRGB
#ifndef USE_VDCE_SERIALRGB_SPEED
#define USE_VDCE_SERIALRGB_SPEED        (3u)
#endif
#endif

#if (USE_VDCE_SERIALRGB | USE_VDCE_OPENLDI) & USE_VDCE_VODDR
    #error "VO-DDR must not be enabled together with SerialRGB and OpenLDI"
#endif

/* Video output pixel clocks exchange register (RH850/D1M2(H) and D1M1A only) */
#define LOC_VDCECTL                     (0xFFC0601Cu)
#define LOC_VDCECTL_VOSL                (1uL << 2)

/* RSDS and other video output control register */
#define LOC_RSDSCFG                     (0xFFC06020u)
#define LOC_FLAG_TO_RSDSCFG(flags)      (((flags) >> R_DDB_DISP_FLAG_RSDS_SHIFT) & 0x0Fu)
                                        /* bit3:ROEN. bit2-1:RPHSL, bit0:RBSW */

#define LOC_RSDSCFG_DISABLE             (0u)

#define LOC_CKSC_IVOEXS_CTL_REG_ADDR    (0xFFF85900u)
#define LOC_PROTCMDD1                   (0xFFF87000u)
#define LOC_CKSC_IVOEXS_CTL_DISABLE     (0u) /* Disable Exchange */
#define LOC_CKSC_IVOEXS_CTL_ENABLE      (1u) /* Enable  Exchange */

#define LOC_PINV45                      (0xFF610B70u)
#define LOC_PINV47                      (0xFF610BF0u)
#define LOC_PPCMD45                     (0xFFC14B6Cu)
#define LOC_PPCMD47                     (0xFFC14BECu)

#define LOC_BIT_PIN0                    (0x00000001u)
#define LOC_BIT_PIN8                    (0x00000100u)

#if USE_VDCE_VODDR

/* Video Output DDR Mode clock divider register (RH850/D1M1A only) */
#define LOC_VODDR0_SYSCNT               (0xF2001000u)
#define LOC_VODDR0_CLKDIV               (0xF2001004u)

#define LOC_SHIFT_16                    (16u)

#define LOC_VODDR0_SYSCNT_CLK_START     (1u << 0)
#define LOC_VODDR0_SYSCNT_VO1_ENABLE    (1u << LOC_SHIFT_16)
#define LOC_VODDR0_SYSCNT_DISABLE       (0u)


/* Set clock divider of 2..254 of PLLCLK */
#define LOC_VODDR0_CLKDIV_CLKDIV0(div)  (((div) & 0xFFu) << 0)
/* Set clock divider of 2..254 of PLLCLK */
#define LOC_VODDR0_CLKDIV_CLKDIV1(div)  (((div) & 0xFFu) << LOC_SHIFT_16)

/* Video Output DDR Mode clock divider mask */
#define LOC_VODDR0_CLKDIV0              (0xFFu)
#define LOC_VODDR0_CLKDIV1              (0xFF0000u)

#endif

/* The definition of the interrupt control. */
#define LOC_INTC_UNIT0_START            (R_DEV_INT_VDCE0ERR)
#define LOC_INTC_UNIT0_END              (R_DEV_INT_VDCE0IRVLINE)
#define LOC_INTC_UNIT1_START            (R_DEV_INT_VDCE1ERR)
#define LOC_INTC_UNIT1_END              (R_DEV_INT_VDCE1S1LOVSYNC)
#define LOC_INTC_UNIT1_OFFSET    ((uint32_t)R_DEV_INT_VDCE1ERR - (uint32_t)R_DEV_INT_VDCE0ERR)

/* VDCE clock divider */
#define LOC_CLOCK_DIVIDER_255           (255u)
#define LOC_CLOCK_DIVIDER_4             (4u)
#define LOC_CLOCK_DIVIDER_2             (2u)
#define LOC_CLOCK_DIVIDER_0             (0u)
#if USE_VDCE_OPENLDI
#define LOC_CLOCK_DIVIDER_7             (7u)
#endif

#define LOC_CLOCK_STOP_REQ_MASK         (0u) /* no mask */
#define LOC_CLOCK_DISABLED              (0u) 


/* VDCE pixel clock divider */
#if USE_VDCE_SERIALRGB
#define LOC_SERIAL_RGB_SPEED_3          (3u)
#define LOC_SERIAL_RGB_SPEED_4          (4u)
#endif

/* Resolution information */
#define LOC_RESOLUTION_320              (320u)
#define LOC_RESOLUTION_480              (480u)
#define LOC_RESOLUTION_1024             (1024u)
#define LOC_RESOLUTION_1280             (1280u)

/***************************************************************************
  Constants: LOC_CPU_FREQ_MAX

  The definition for maximum CPU clock frequency (Hz).
  It does not matter if it is greater than actual value.
*/
#define LOC_CPU_FREQ_MAX                (240000000u)

/***************************************************************************
  Constants: LOC_SYNC_FREQ_MIN

  The definition for minimum syncronization clock frequency (Hz).
  When frequency is unknown (i.e. external VIN clock), this definition value is used.
  It should be enough small value.
*/
#define LOC_SYNC_FREQ_MIN               (1000000u)

/***************************************************************************
  Constants: LOC_1ST_WAIT_CYCLE

  The definition for cycle number that H/W accepts clear instruction.
  The cycle is based on synchronization clock for each interrupt. 

  Simulatuion result : 2 Cycle.
  margin             : 2 Cycle.
  It does not matter if it is greater than actual value.
*/
#define LOC_1ST_WAIT_CYCLE              (4u)

/***************************************************************************
  Constants: LOC_2ND_WAIT_CYCLE

  The definition for cycle number that H/W completes clear instruction.
  The cycle is based on synchronization clock for each interrupt.

  Simulatuion result : 4 Cycle.
  margin             : 4 Cycle.
  It does not matter if it is greater than actual value.
*/
#define LOC_2ND_WAIT_CYCLE              (8u)

/*
*   The definition of the device version.
*
*      If this definition is not defined, VDCE works as H/W device version.
*      If this definition is defined, VDCE works as specified device version for debug.
*
*     Values:
*       R_DEV_R7F701403 - VDCE works as RH850/D1L2x.
*       R_DEV_R7F701407 - VDCE works as RH850/D1M1(H).
*       R_DEV_R7F701410 - VDCE works as RH850/D1M2
*       R_DEV_R7F701412 - VDCE works as RH850/D1M2H
*/
/* #define LOC_VDCE_FORCE_DEV_NUM       (R_DEV_R7F701403) */

/***************************************************************************
  Constants: LOC_WAIT_CLEAR_COUNT
  
  The definition for maximum loop counter that H/W completes clear instruction.
  
  Simulatuion result : It took max 7 Loops in case of 1.88MHz pixel clock and 240MHz CPU clock.
*/
#define LOC_WAIT_CLEAR_COUNT         (16u)

/****************************************************************************
  Section: Local Variables
*/
/*******************************************************************************
  Variable: loc_LayerState

  This value stores the Layer state of the VDCE driver.
*/
#ifdef R_BSP_SYS_INIT_USE
R_VDCE_STATIC r_vdce_State_t loc_LayerState[R_VDCE_MACRO_NUM][R_VDCE_SYS_MAX_LAYER_NUM];
#else
R_VDCE_STATIC r_vdce_State_t loc_LayerState[R_VDCE_MACRO_NUM][R_VDCE_SYS_MAX_LAYER_NUM] =
{
    {
        R_VDCE_STATE_UNINITIALIZED,
        R_VDCE_STATE_UNINITIALIZED,
        R_VDCE_STATE_UNINITIALIZED,
        R_VDCE_STATE_UNINITIALIZED,
        R_VDCE_STATE_UNINITIALIZED
    }
#if (R_VDCE_MACRO_NUM > 1)
    , {
        R_VDCE_STATE_UNINITIALIZED,
        R_VDCE_STATE_UNINITIALIZED,
        R_VDCE_STATE_UNINITIALIZED,
        R_VDCE_STATE_UNINITIALIZED,
        R_VDCE_STATE_UNINITIALIZED
    }
#endif
};
#endif /* R_BSP_RGL_INIT_USE */

/*******************************************************************************
  Variable: loc_CaptState

  This value stores the Capture state of the VDCE driver.
*/
#ifdef R_BSP_RGL_INIT_USE
R_VDCE_STATIC r_vdce_State_t loc_CaptState[R_VDCE_MACRO_NUM];
#else
R_VDCE_STATIC r_vdce_State_t loc_CaptState[R_VDCE_MACRO_NUM] =
{
    R_VDCE_STATE_UNINITIALIZED
#if (R_VDCE_MACRO_NUM > 1)
    ,
    R_VDCE_STATE_UNINITIALIZED
#endif
};
#endif /* R_BSP_RGL_INIT_USE */

/*******************************************************************************
  Section: Local functions
*/
/* list of static functions: */
R_VDCE_STATIC uint32_t loc_CalcClockDiv(const uint32_t   Clkin,
                                        const uint32_t   Clkout) ;

R_VDCE_STATIC void loc_SetClkDotCk0(const r_dev_CksSrc_t SrcId, 
                                    const uint32_t       Div);

R_VDCE_STATIC void loc_SetClkDotCk1(const r_dev_CksSrc_t SrcId, 
                                    const uint32_t       Div);

R_VDCE_STATIC void loc_SetClkVdce0Ck(const r_dev_CksSrc_t SrcId);

R_VDCE_STATIC void loc_SetClkVdce1Ck(const r_dev_CksSrc_t SrcId);

R_VDCE_STATIC void loc_SetRsdsCk(const r_dev_CksSrc_t    SrcId, 
                                 const uint32_t          Div);

R_VDCE_STATIC uint32_t loc_PixelSetClockM2(const uint32_t    Unit,
                                           const uint32_t    Freq,
                                           const uint32_t    Flags);

R_VDCE_STATIC uint32_t loc_PixelSetClockM1A(const uint32_t   Unit,
                                            const uint32_t   Freq,
                                            const uint32_t   OtherUnitActive);

R_VDCE_STATIC uint32_t loc_PixelSetClockL2M1(const uint32_t      Freq);

R_VDCE_STATIC r_vdce_Error_t loc_IntcCtrl(const uint32_t         Unit,
                                          const r_dev_IntSel_t   IntSel,
                                          const uint8_t          Enable);

R_VDCE_STATIC r_dev_Device_t loc_GetDev(void);

R_VDCE_STATIC r_vdce_Error_t loc_IntcCtrlAll(const uint32_t      Unit,
                                             const uint8_t       Enable);

R_VDCE_STATIC r_dev_IntSel_t loc_ConvertToIntSel(const uint32_t         Unit,
                                                 const r_vdce_IntType_t IntcType);

/*******************************************************************************
  Function: loc_CalcClockDiv
*/
R_VDCE_STATIC uint32_t loc_CalcClockDiv(const uint32_t   Clkin,
                                        const uint32_t   Clkout)
{
    uint32_t   divider;
    uint32_t   div_frq;
    uint32_t   div_frq_next;
    
    /* Check Disable  */
    if (Clkout == LOC_CLOCK_DISABLED)
    {
        divider = LOC_CLOCK_DIVIDER_0;
    }
    else
    {
        /* calc near divider */
        divider = Clkin / Clkout;
        if (divider != LOC_CLOCK_DIVIDER_0)
        {
            /* test if next divider is better, if yes choose next divider */
            div_frq      = Clkin / divider;
            div_frq_next = Clkin / (divider + 1u);
            if ((div_frq - Clkout) > (Clkout - div_frq_next))
            {
                divider += 1u;
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* Do nothing */
        }
        if (divider < LOC_CLOCK_DIVIDER_2)
        {
            divider = LOC_CLOCK_DIVIDER_2;
        }
        else if (divider > LOC_CLOCK_DIVIDER_255)
        {
            divider = LOC_CLOCK_DIVIDER_255;
        }
        else
        {
            /* Do nothing */
        }
    }

    return divider;
}

R_VDCE_STATIC void loc_SetClkDotCk0(const r_dev_CksSrc_t SrcId, const uint32_t Div)
{
    r_dev_ClkSelConfig_t  clk_sel_cfg;
    
    clk_sel_cfg.Cks       = R_DEV_CKS_DOTCK0;
    clk_sel_cfg.SrcId     = SrcId;
    clk_sel_cfg.Div       = (uint16_t)Div;
    clk_sel_cfg.StpReqMsk = LOC_CLOCK_STOP_REQ_MASK;
    
    (void)R_DEV_ClkIdSet(&clk_sel_cfg);
    /* R_DEV_ClkIdSet does not return error as default. */
}

R_VDCE_STATIC void loc_SetClkDotCk1(const r_dev_CksSrc_t SrcId, const uint32_t Div)
{
    r_dev_ClkSelConfig_t  clk_sel_cfg;
    
    clk_sel_cfg.Cks       = R_DEV_CKS_DOTCK1;
    clk_sel_cfg.SrcId     = SrcId;
    clk_sel_cfg.Div       = (uint16_t)Div;
    clk_sel_cfg.StpReqMsk = LOC_CLOCK_STOP_REQ_MASK;
    
    (void)R_DEV_ClkIdSet(&clk_sel_cfg);
    /* R_DEV_ClkIdSet does not return error as default. */
}

R_VDCE_STATIC void loc_SetClkVdce0Ck(const r_dev_CksSrc_t SrcId)
{
    r_dev_ClkSelConfig_t  clk_sel_cfg;
    
    clk_sel_cfg.Cks       = R_DEV_CKS_VDCE0CK;
    clk_sel_cfg.SrcId     = SrcId;
    clk_sel_cfg.Div       = LOC_CLOCK_DIVIDER_0;
    clk_sel_cfg.StpReqMsk = LOC_CLOCK_STOP_REQ_MASK;
    
    (void)R_DEV_ClkIdSet(&clk_sel_cfg);
    /* R_DEV_ClkIdSet does not return error as default. */
}

R_VDCE_STATIC void loc_SetClkVdce1Ck(const r_dev_CksSrc_t SrcId)
{
    r_dev_ClkSelConfig_t  clk_sel_cfg;
    
    clk_sel_cfg.Cks       = R_DEV_CKS_VDCE1CK;
    clk_sel_cfg.SrcId     = SrcId;
    clk_sel_cfg.Div       = LOC_CLOCK_DIVIDER_0;
    clk_sel_cfg.StpReqMsk = LOC_CLOCK_STOP_REQ_MASK;
    
    (void)R_DEV_ClkIdSet(&clk_sel_cfg);
    /* R_DEV_ClkIdSet does not return error as default. */
}

R_VDCE_STATIC void loc_SetRsdsCk(const r_dev_CksSrc_t SrcId, const uint32_t Div)
{
    r_dev_ClkSelConfig_t  dot_clk_cfg;
    r_dev_ClkSelConfig_t  rsds_clk_cfg;
    uint32_t              clk;
    
    /* Get another clk */
    if (SrcId == R_DEV_CKS_SRC_DOTCK0)
    {
        dot_clk_cfg.Cks = R_DEV_CKS_DOTCK1;
    }
    else
    {
        dot_clk_cfg.Cks = R_DEV_CKS_DOTCK0;
    }
    /* Check that another DOTCLK is already avtive */
    clk = R_DEV_ClkFrequencyHz(dot_clk_cfg.Cks);
    if (clk == LOC_CLOCK_DISABLED)
    {
       /* another DOTCK is needed before RSDS clk is changed  */
        dot_clk_cfg.SrcId     = R_DEV_CKS_SRC_PLL2;
        dot_clk_cfg.Div       = (uint16_t)Div;
        dot_clk_cfg.StpReqMsk = LOC_CLOCK_STOP_REQ_MASK;

        (void)R_DEV_ClkIdSet(&dot_clk_cfg);
        /* R_DEV_ClkIdSet does not return error as default. */
    }
    else
    {
        /* Do nothing */
    }
    
    /* Set C_ISO_RSDSCLK */
    rsds_clk_cfg.Cks       = R_DEV_CKS_RSDSCK;
    rsds_clk_cfg.SrcId     = SrcId;
    rsds_clk_cfg.Div       = LOC_CLOCK_DIVIDER_0;
    rsds_clk_cfg.StpReqMsk = LOC_CLOCK_STOP_REQ_MASK;
    (void)R_DEV_ClkIdSet(&rsds_clk_cfg);

    /* Restore another DOTCK  */
    if (clk == LOC_CLOCK_DISABLED)
    {
        dot_clk_cfg.Div = LOC_CLOCK_DIVIDER_0;
        (void)R_DEV_ClkIdSet(&dot_clk_cfg);
        /* R_DEV_ClkIdSet does not return error as default. */
    }
    else
    {
        /* Do nothing */
    }

    
}

/*******************************************************************************
  Function: loc_PixelSetClockM2
*/
R_VDCE_STATIC uint32_t loc_PixelSetClockM2(const uint32_t   Unit,
                                           const uint32_t   Freq,
                                           const uint32_t   Flags)
{
    uint32_t                divider;
    uint32_t                clk_in;
    uint32_t                rsds_cfg_val = LOC_RSDSCFG_DISABLE;

    /* get VDCE clock */
    clk_in = R_DEV_BusFrequencyHz(R_DEV_CLK_PLL2);
    if ((Flags & (uint32_t)R_DDB_DISP_FLAG_RSDS_ENABLE) != 0u)
    {
        /* divider by 4 for pixel clock, when RSDS is used */
        clk_in /= LOC_CLOCK_DIVIDER_4;
    }
    else
    {
        /* Do nothing */
    }
    
    /* Calculate VDCE clock divider */
    divider = loc_CalcClockDiv(clk_in, Freq);

    /* Set DOTCLK0 / DOTCLK1 */
    if (Unit == R_VDCE_UNIT0)
    {
        loc_SetClkDotCk0(R_DEV_CKS_SRC_PLL2, divider);
    }
    else
    {
        loc_SetClkDotCk1(R_DEV_CKS_SRC_PLL2, divider);
    }

    if (divider != LOC_CLOCK_DIVIDER_0)
    {
        /* Check RSDS */
        if ((Flags & (uint32_t)R_DDB_DISP_FLAG_RSDS_ENABLE) != 0u)
        {
            if (Unit == R_VDCE_UNIT0)
            {
                /* Set VDCE0CLK - divider by 4 for pixel clock, when RSDS is used */
                loc_SetClkVdce0Ck(R_DEV_CKS_SRC_DOTCK0_4);

                /* Set C_ISO_RSDSCLK  */
                loc_SetRsdsCk(R_DEV_CKS_SRC_DOTCK0, divider);
            }
            else
            {
                /* Set VDCE1CLK - divider by 4 for pixel clock, when RSDS is used */
                loc_SetClkVdce1Ck(R_DEV_CKS_SRC_DOTCK1_4);

                /* Set C_ISO_RSDSCLK  */
                loc_SetRsdsCk(R_DEV_CKS_SRC_DOTCK1, divider);
            }
            
            /* Sets RSDSCFG register */
            rsds_cfg_val = LOC_FLAG_TO_RSDSCFG(Flags);
        }
        else
        {
            /* Set VDCE0CLK / VDCE1CLK */
            if (Unit == R_VDCE_UNIT0)
            {
                loc_SetClkVdce0Ck(R_DEV_CKS_SRC_DOTCK0);
            }
            else
            {
                /* Set VDCE1CLK */
                loc_SetClkVdce1Ck(R_DEV_CKS_SRC_DOTCK1);
            }
        }
    }
    else
    {
        /* Do nothing */
    }

    /*
    *   If VDCE0 and VDCE1 signalling is cross-changed, also change
    *   to clock supply of the output macro
    */
    if ((R_DEV_READ_REG(32, LOC_VDCECTL) & LOC_VDCECTL_VOSL) == 0u)
    {
        /* RSDSCFG is changed when Unit0->VO0 output */
        if (Unit == R_VDCE_UNIT0)
        {
            R_DEV_WRITE_REG(32, LOC_RSDSCFG, rsds_cfg_val);
        }
        else
        {
            /* Do nothing */
        }
        
        /* Disable exchange of VDCE0_VO_CLK and VDCE1_VO_CLK */
        /* Set by macro function with do-while loop  */
        R_DEV_WRITE_PROT_REG( /* PRQA S 2214 */
            LOC_PROTCMDD1, LOC_CKSC_IVOEXS_CTL_REG_ADDR, LOC_CKSC_IVOEXS_CTL_DISABLE);
    }
    else
    {
        /* RSDSCFG is changed when Unit1->VO0 output */
        if (Unit == R_VDCE_UNIT1)
        {
            R_DEV_WRITE_REG(32, LOC_RSDSCFG, rsds_cfg_val);
        }
        else
        {
            /* Do nothing */
        }
        
        /* Enable exchange of VDCE0_VO_CLK and VDCE1_VO_CLK */
        /* Set by macro function with do-while loop */
        R_DEV_WRITE_PROT_REG( /* PRQA S 2214 */
            LOC_PROTCMDD1, LOC_CKSC_IVOEXS_CTL_REG_ADDR, LOC_CKSC_IVOEXS_CTL_ENABLE);
    }
    
    return (divider != LOC_CLOCK_DIVIDER_0) ? (clk_in / divider) : LOC_CLOCK_DISABLED;
}

/*******************************************************************************
  Function: loc_CheckOpenLdi
*/
#if USE_VDCE_OPENLDI
R_VDCE_STATIC int32_t  loc_CheckOpenLdi(const uint32_t   Unit,
                                        const uint32_t   VdceCtl,
                                        uint32_t *const  ClockIn)
{
    int32_t         result;
    
    if ((VdceCtl & LOC_VDCECTL_VOSL) == 0u)
    {
        if (Unit == R_VDCE_UNIT0)
        {
            /* VDCE0 -> VO0 */
            result = R_TRUE;
        }
        else
        {
            /* VDCE1 -> VO1 */
            result = R_FALSE;
        }
    }
    else
    {
        if (Unit == R_VDCE_UNIT0)
        {
            /* VDCE0 -> VO1 */
            result = R_FALSE;
        }
        else
        {
            /* VDCE1 -> VO0 */
            result = R_TRUE;
        }
    }
    
    if (result == R_TRUE)
    {
        *ClockIn /= LOC_CLOCK_DIVIDER_7;
    }
    else
    {
        /* Do nothing */
    }
    
    return result;
}
#endif /* USE_VDCE_OPENLDI */

/*******************************************************************************
  Function: loc_CheckSerialRgb
*/
#if USE_VDCE_SERIALRGB
R_VDCE_STATIC void     loc_CheckSerialRgb(const uint32_t   Unit,
                                          const uint32_t   VdceCtl,
                                          uint32_t *const  ClockIn)
{
    int32_t         result;
    
    if ((VdceCtl & LOC_VDCECTL_VOSL) == 0u)
    {
        if (Unit == R_VDCE_UNIT0)
        {
            /* VDCE0 -> VO0 */
            result = R_FALSE;
        }
        else
        {
            /* VDCE1 -> VO1 */
            result = R_TRUE;
        }
    }
    else
    {
        if (Unit == R_VDCE_UNIT0)
        {
            /* VDCE0 -> VO1 */
            result = R_TRUE;
        }
        else
        {
            /* VDCE1 -> VO0 */
            result = R_FALSE;
        }
    }
    if (result == R_TRUE)
    {
#if (USE_VDCE_SERIALRGB_SPEED == 3)
        *ClockIn /= LOC_SERIAL_RGB_SPEED_3;
#elif (USE_VDCE_SERIALRGB_SPEED == 4)
        *ClockIn /= LOC_SERIAL_RGB_SPEED_4;
#endif /* USE_VDCE_SERIALRGB_SPEED */
    }
    else
    {
    }
}
#endif /* USE_VDCE_SERIALRGB */


/*******************************************************************************
  Function: loc_SetVoddr
*/
#if USE_VDCE_VODDR  /* 0=LVTTL, 1=VODDR */
R_VDCE_STATIC int32_t  loc_SetVoddr(const uint32_t   Unit,
                                    const uint32_t   Divider,
                                    const uint32_t   OtherUnitActive)
{
    int32_t               result = R_TRUE;
    uint32_t              voddr_clk_div;
    uint32_t              divider0;
    uint32_t              divider1;
    
    /* Check Disable */
    if (Divider == LOC_CLOCK_DIVIDER_0)
    {
        R_DEV_WRITE_REG(32, LOC_VODDR0_SYSCNT, LOC_VODDR0_SYSCNT_DISABLE);
    }
    else
    {
        if (OtherUnitActive == (uint32_t)R_FALSE)
        {
            voddr_clk_div = R_DEV_READ_REG(32, LOC_VODDR0_CLKDIV);
            divider0 = voddr_clk_div & LOC_VODDR0_CLKDIV0;
            divider1 = (voddr_clk_div & LOC_VODDR0_CLKDIV1) >> LOC_SHIFT_16;

            if (Unit == R_VDCE_UNIT0) 
            {
                divider0 = Divider;
            }
            else 
            {
                divider1 = Divider;
            }

            R_DEV_WRITE_REG(32, LOC_VODDR0_CLKDIV, 
                            LOC_VODDR0_CLKDIV_CLKDIV0(divider0) | 
                            LOC_VODDR0_CLKDIV_CLKDIV1(divider1));
            
            R_DEV_WRITE_REG(32, LOC_VODDR0_SYSCNT, LOC_VODDR0_SYSCNT_DISABLE);
        }
        /* Other Unit is already running and we are configuring this Unit: Start VO-DDR */
        else
        {
            voddr_clk_div = R_DEV_READ_REG(32, LOC_VODDR0_CLKDIV);
            divider0 = voddr_clk_div & LOC_VODDR0_CLKDIV0;
            divider1 = (voddr_clk_div & LOC_VODDR0_CLKDIV1) >> LOC_SHIFT_16;
            if (Unit == R_VDCE_UNIT0)
            {
                divider0 = Divider;
            }
            else
            {
                divider1 = Divider;
            }

            /* Check if divider for Unit0 and Unit1 have the correct proportion */

            /* fVDCE0 <= fVDCE1 => div0 >= div1 */
            /* divider0 must be an integer multiple of divider1 */
            /* divider0 & divider1 must be even*/
            if ((divider0 < divider1) ||
                (divider1 == LOC_CLOCK_DIVIDER_0) ||
                (((divider0 / divider1) * divider1) != divider0) ||
                ((divider0 % 2) == 1u) || ((divider1 % 2) == 1u))
            {
                /* Proportion mismatch! */
                result = R_FALSE;
            }
            else
            {
                R_DEV_WRITE_REG(32, LOC_VODDR0_CLKDIV, 
                                LOC_VODDR0_CLKDIV_CLKDIV0(divider0) |
                                LOC_VODDR0_CLKDIV_CLKDIV1(divider1));
                R_DEV_WRITE_REG(32, LOC_VODDR0_SYSCNT, 
                                LOC_VODDR0_SYSCNT_CLK_START | LOC_VODDR0_SYSCNT_VO1_ENABLE);
            }
        }
    }

    return result;
}
#endif /* USE_VDCE_VODDR */

/*******************************************************************************
  Function: loc_PixelSetClockM1A
*/
R_VDCE_STATIC uint32_t loc_PixelSetClockM1A(const uint32_t   Unit,
                                            const uint32_t   Freq,
                                            const uint32_t   OtherUnitActive)
{
    uint32_t              divider;
    uint32_t              clk_in;
    uint32_t              ret;
    uint32_t              vdce_ctl;
#if USE_VDCE_VODDR
    int32_t               voddr_res = R_TRUE;
#endif
#if USE_VDCE_OPENLDI
    int32_t               open_ldi  = R_FALSE;
#endif
    
    /* Get VDCECTL register */
    vdce_ctl = R_DEV_READ_REG(32, LOC_VDCECTL);
    
    /* Get base clock */
    clk_in = R_DEV_BusFrequencyHz(R_DEV_CLK_PLL1);
#if USE_VDCE_OPENLDI
    open_ldi = loc_CheckOpenLdi(Unit, vdce_ctl, &clk_in);
#endif
#if USE_VDCE_SERIALRGB
    loc_CheckSerialRgb(Unit, vdce_ctl, &clk_in);
#endif
    
    /* Get divider */
    divider = loc_CalcClockDiv(clk_in, Freq);
    
#if USE_VDCE_VODDR
    voddr_res = loc_SetVoddr(Unit, divider, OtherUnitActive);
    if (voddr_res == R_FALSE)
    {
        ret = LOC_CLOCK_DISABLED;
    }
    else
#endif /* USE_VDCE_VODDR */
    {
        if (Unit == R_VDCE_UNIT0)
        {
#if USE_VDCE_OPENLDI
            if (open_ldi == R_TRUE)
            {
                if (Freq == LOC_CLOCK_DISABLED)
                {
                    /* D1M1A needs to stop the source clock of both OLDICLK and IVOEXS0_OUTCLK 
                       so both clocks stop at the same timing.
                    */
                    loc_SetClkDotCk0(R_DEV_CKS_SRC_DISABLED, divider);
                    loc_SetClkVdce0Ck(R_DEV_CKS_SRC_DOTCK0_7);
                }
                else
                {
                    /* Set DOTCLK0 & VDCE0CLK */
                    loc_SetClkDotCk0(R_DEV_CKS_SRC_PLL1, divider);
                    loc_SetClkVdce0Ck(R_DEV_CKS_SRC_DOTCK0_7);
                }
            }
            else
#endif /* USE_VDCE_OPENLDI */
            {
                /* Set DOTCLK0 & VDCE0CLK */
                loc_SetClkDotCk0(R_DEV_CKS_SRC_PLL1, divider);
                loc_SetClkVdce0Ck(R_DEV_CKS_SRC_DOTCK0);
            }
        }
        else
        {
#if USE_VDCE_OPENLDI
            if (open_ldi == R_TRUE)
            {
                if (Freq == LOC_CLOCK_DISABLED)
                {
                    /* D1M1A needs to stop the source clock of both OLDICLK and IVOEXS0_OUTCLK 
                       so both clocks stop at the same timing.
                    */
                    loc_SetClkDotCk1(R_DEV_CKS_SRC_DISABLED, divider);
                    loc_SetClkVdce1Ck(R_DEV_CKS_SRC_DOTCK1_7);
                }
                else
                {
                    /* Set DOTCLK1 & VDCE1CLK */
                    loc_SetClkDotCk1(R_DEV_CKS_SRC_PLLFIX, divider);
                    loc_SetClkVdce1Ck(R_DEV_CKS_SRC_DOTCK1_7);
                }
            }
            else
#endif /* USE_VDCE_OPENLDI */
            {
                /* Set DOTCLK1 & VDCE1CLK */
                /* R_DEV_CKS_SRC_PLLFIX is actually same as R_DEV_CKS_SRC_PLL1 */
                loc_SetClkDotCk1(R_DEV_CKS_SRC_PLLFIX, divider);
                loc_SetClkVdce1Ck(R_DEV_CKS_SRC_DOTCK1);
            }
        }
        
        /*
        *   If VDCE0 and VDCE1 signalling is cross-changed,
        *   also change to clock supply of the output macro
        */
        if ((vdce_ctl & LOC_VDCECTL_VOSL) == 0u)
        {
            /* Disable exchange of VDCE0_VO_CLK and VDCE1_VO_CLK */
            /* Set by macro function with do-while loop */
            R_DEV_WRITE_PROT_REG(  /* PRQA S 2214 */
                LOC_PROTCMDD1, LOC_CKSC_IVOEXS_CTL_REG_ADDR, LOC_CKSC_IVOEXS_CTL_DISABLE);
        }
        else
        {
            /* Enable exchange of VDCE0_VO_CLK and VDCE1_VO_CLK */
            /* Set by macro function with do-while loop */
            R_DEV_WRITE_PROT_REG(  /* PRQA S 2214 */
                LOC_PROTCMDD1, LOC_CKSC_IVOEXS_CTL_REG_ADDR, LOC_CKSC_IVOEXS_CTL_ENABLE);
        }

        ret = (divider != LOC_CLOCK_DIVIDER_0) ? (clk_in / divider) : LOC_CLOCK_DISABLED;
    }

    return ret;
}

/*******************************************************************************
  Function: loc_PixelSetClockL2M1
  
  set VDCE pixel clock (in MHz)
*/
R_VDCE_STATIC uint32_t loc_PixelSetClockL2M1(const uint32_t Freq)
{
    uint32_t             divider;
    uint32_t             clk_in;

    /* get VDCE clock */
    clk_in = R_DEV_BusFrequencyHz(R_DEV_CLK_PLL0PIX);
    R_DBG_PRINT(R_DBG_MSG_CODE, "Set VDCE clock divider");

    /* for D1M1-V2 */
#if USE_VDCE_SERIALRGB    
#if USE_VDCE_SERIALRGB_SPEED == 3
    clk_in /= LOC_SERIAL_RGB_SPEED_3;
#elif USE_VDCE_SERIALRGB_SPEED == 4
    clk_in /= LOC_SERIAL_RGB_SPEED_4;
#endif
#endif /* USE_VDCE_SERIALRGB */

    divider = loc_CalcClockDiv(clk_in, Freq);
    
    /* Set DOTCLK0 */
    loc_SetClkDotCk0(R_DEV_CKS_SRC_PLL0PIX, divider);
    
    /* Set VDCE0CLK */
    loc_SetClkVdce0Ck(R_DEV_CKS_SRC_DOTCK0);
    
    return (divider != LOC_CLOCK_DIVIDER_0) ? (clk_in / divider) : LOC_CLOCK_DISABLED;
}

/*******************************************************************************
  Function: loc_IntcCtrlAll
*/
R_VDCE_STATIC r_vdce_Error_t loc_IntcCtrlAll(const uint32_t Unit,
                                             const uint8_t  Enable)
{
    r_vdce_Error_t ret;
    uint16_t       i;
    r_dev_IntSel_t unit0_intc_sel[] =
    {
        R_DEV_INT_VDCE0ERR       ,
        R_DEV_INT_VDCE0S0VIVSYNC ,
        R_DEV_INT_VDCE0S0LOVSYNC ,
        R_DEV_INT_VDCE0GR3VLINE  ,
        R_DEV_INT_VDCE0S0VFIELD  ,
        R_DEV_INT_VDCE0S1LOVSYNC ,
        R_DEV_INT_VDCE0OIRVIVSYNC,
        R_DEV_INT_VDCE0OIRLOVSYNC,
        R_DEV_INT_VDCE0IRVLINE   
    };
    r_dev_IntSel_t unit1_intc_sel[] =
    {
        R_DEV_INT_VDCE1ERR       ,
        R_DEV_INT_VDCE1S0VIVSYNC ,
        R_DEV_INT_VDCE1S0LOVSYNC ,
        R_DEV_INT_VDCE1GR3VLINE  ,
        R_DEV_INT_VDCE1S0VFIELD  ,
        R_DEV_INT_VDCE1S1LOVSYNC
    };

    ret = R_VDCE_ERR_OK;

    if (Unit < R_VDCE_MACRO_NUM)
    {
        if (Unit == R_VDCE_UNIT0)
        {
            /* Configure all interrupts on the device level */
            for (i = 0u; i < (sizeof(unit0_intc_sel) / sizeof(r_dev_IntSel_t)); i++)
            {
                R_DEV_IntClearFlag(unit0_intc_sel[i]);
                R_DEV_IntEnable(unit0_intc_sel[i], Enable);
            }
            /* GR3VBLANK is enabled independently, only disable is executed here. */
            if (Enable == (uint8_t)R_FALSE)
            {
                R_DEV_IntClearFlag(R_DEV_INT_VDCE0GR3VBLANK);
                R_DEV_IntEnable(R_DEV_INT_VDCE0GR3VBLANK, Enable);
            }
            else
            {
                /* Do nothing */
            }
        }
        /* Unit == R_VDCE_UNIT1 */
        else
        {
            /* Configure all interrupts on the device level */
            for (i = 0u; i < (sizeof(unit1_intc_sel) / sizeof(r_dev_IntSel_t)); i++)
            {
                R_DEV_IntClearFlag(unit1_intc_sel[i]);
                R_DEV_IntEnable(unit1_intc_sel[i], Enable);
            }
            /* GR3VBLANK is enabled independently, only disable is executed here. */
            if (Enable == (uint8_t)R_FALSE)
            {
                R_DEV_IntClearFlag(R_DEV_INT_VDCE1GR3VBLANK);
                R_DEV_IntEnable(R_DEV_INT_VDCE1GR3VBLANK, Enable);
            }
            else
            {
                /* Do nothing */
            }
        }
    }
    else
    {
        ret = R_VDCE_ERR_RANGE_UNIT;
    }

    return ret;
}

/*******************************************************************************
  Function: loc_IntcCtrl
*/
R_VDCE_STATIC r_vdce_Error_t loc_IntcCtrl(const uint32_t       Unit,
                                          const r_dev_IntSel_t IntSel,
                                          const uint8_t        Enable)
{
    r_vdce_Error_t ret;
    r_dev_IntSel_t int_sel;

    ret = R_VDCE_ERR_OK;

    if (Unit < R_VDCE_MACRO_NUM)
    {
        if (Unit == R_VDCE_UNIT0)
        {
            int_sel = IntSel;
            if ((int_sel >= LOC_INTC_UNIT0_START) && (int_sel <= LOC_INTC_UNIT0_END))
            {
                R_DEV_IntClearFlag(int_sel);
                R_DEV_IntEnable(int_sel, Enable);
            }
            else
            {
                ret = R_VDCE_ERR_RANGE_PARAM;
            }
        }
        else /* Unit == R_VDCE_UNIT1 */
        {
            /* Calculate Unit1 intterrupt with type cast */
            int_sel = (r_dev_IntSel_t)((uint32_t)IntSel + LOC_INTC_UNIT1_OFFSET); /* PRQA S 4394 */
            if ((int_sel >= LOC_INTC_UNIT1_START) && (int_sel <= LOC_INTC_UNIT1_END))
            {
                R_DEV_IntClearFlag(int_sel);
                R_DEV_IntEnable(int_sel, Enable);
            }
            else
            {
                ret = R_VDCE_ERR_RANGE_PARAM;
            }
        }
    }
    else
    {
        ret = R_VDCE_ERR_RANGE_UNIT;
    }

    return ret;
}

/*******************************************************************************
  Function: loc_GetDev
*/
R_VDCE_STATIC r_dev_Device_t loc_GetDev(void)
{
    r_dev_Device_t  device;

    /* Modify device number if debug. */
#ifdef LOC_VDCE_FORCE_DEV_NUM
    device = LOC_VDCE_FORCE_DEV_NUM;
#else
    device = R_DEV_GetDev();
#endif
    return device;
}

/*******************************************************************************
  Function: loc_ConvertToIntSel
*/
R_VDCE_STATIC r_dev_IntSel_t loc_ConvertToIntSel(const uint32_t          Unit,
                                                 const r_vdce_IntType_t IntcType)
{
    r_dev_IntSel_t  intc_sel;

    switch (IntcType)
    {
        case R_VDCE_INTC_VBLANK:
            intc_sel = R_DEV_INT_VDCE0S0LOVSYNC;
            break;

        case R_VDCE_INTC_VBLANK_1:
            intc_sel = R_DEV_INT_VDCE0S1LOVSYNC;
            break;

        case R_VDCE_INTC_VBLANK_DELAY:
            intc_sel = R_DEV_INT_VDCE0GR3VBLANK;
            break;

        case R_VDCE_INTC_SCANLINE:
            intc_sel = R_DEV_INT_VDCE0GR3VLINE;
            break;

        case R_VDCE_INTC_OIR_SCANLINE:
            intc_sel = R_DEV_INT_VDCE0IRVLINE;
            break;

        case R_VDCE_INTC_OIR_VBLANK:
            intc_sel = R_DEV_INT_VDCE0OIRLOVSYNC;
            break;

        case R_VDCE_INTC_OIR_VSCYNC_WRITE:
            intc_sel = R_DEV_INT_VDCE0OIRVIVSYNC;
            break;

        case R_VDCE_INTC_CAP_VBLANK:
            intc_sel = R_DEV_INT_VDCE0S0VIVSYNC;
            break;

        case R_VDCE_INTC_CAP_END_OF_FIELD:
            intc_sel = R_DEV_INT_VDCE0S0VFIELD;
            break;

        case R_VDCE_INTC_ERROR:
            /* fall through */
        case R_VDCE_INTC_NO_VI_VSYNC_SIGNAL:
            /* fall through */
        case R_VDCE_INTC_NO_VI_VSYNC_SIGNAL_1:
            /* fall through */
        case R_VDCE_INTC_ERR_OIR_UNDERFLOW:
            /* fall through */
        case R_VDCE_INTC_ERR_LAYER0_UNDERFLOW:
            /* fall through */
        case R_VDCE_INTC_ERR_LAYER1_UNDERFLOW:
            /* fall through */
        case R_VDCE_INTC_ERR_LAYER2_UNDERFLOW:
            /* fall through */
        case R_VDCE_INTC_ERR_LAYER3_UNDERFLOW:
            /* fall through */
        case R_VDCE_INTC_ERR_CAP_WRITE_OVERFLOW:
            /* fall through */
        default: 
            intc_sel = R_DEV_INT_LAST;
            break;
    }
    if ((intc_sel < R_DEV_INT_LAST) && (Unit == R_VDCE_UNIT1))
    {
        /* Calculate Unit1 intterrupt with type cast */
        intc_sel = (r_dev_IntSel_t)((uint32_t)intc_sel + LOC_INTC_UNIT1_OFFSET);  /* PRQA S 4394 */
    }
    else
    {
        /* Do nothing */
    }

    return intc_sel;
}

/*******************************************************************************
  Section: VDCE Macro Driver Support Functions

  Comments see: <r_vdce_sys.h>
*/

/*******************************************************************************
  Function: R_VDCE_Sys_InitGlobal
*/
void R_VDCE_Sys_InitGlobal(void)
{
    uint32_t    unit;
    uint32_t    layer;
    
    for (unit = 0u; unit < R_VDCE_MACRO_NUM; unit++)
    {
        for (layer = 0u; layer < R_VDCE_SYS_MAX_LAYER_NUM; layer++)
        {
            loc_LayerState[unit][layer] = R_VDCE_STATE_UNINITIALIZED;
        }
        
        loc_CaptState[unit] = R_VDCE_STATE_UNINITIALIZED;
    }
}

/*******************************************************************************
  Function: R_VDCE_Sys_Init
*/
r_vdce_Error_t R_VDCE_Sys_Init(const uint32_t Unit)
{
    return R_VDCE_ERR_OK;
}

/*******************************************************************************
  Function: R_VDCE_Sys_DeInit
*/
r_vdce_Error_t R_VDCE_Sys_DeInit(const uint32_t Unit)
{
    return R_VDCE_ERR_OK;
}

/*******************************************************************************
  Function: R_VDCE_Sys_BaseAddrGet
*/
uint32_t R_VDCE_Sys_BaseAddrGet(const uint32_t Unit)
{
    uint32_t        ret;
    r_dev_Device_t  device  =  R_DEV_GetDev(); 

    if (Unit == R_VDCE_UNIT0)
    {
        ret = R_VDCE_BASE_ADDR0;
    }
    else if (Unit == R_VDCE_UNIT1)
    {
        /* RH850/D1M2x and RH850/D1M1A */
        if (((device >= R_DEV_R7F701408) && (device <= R_DEV_R7F701412)) ||
            (device == R_DEV_R7F701441))
        {
            ret = R_VDCE_BASE_ADDR1;
        }
        else
        {
            ret = 0u;
        }
    }
    else
    {
        ret = 0u;
    }

    return ret;
}

/*******************************************************************************
  Function: R_VDCE_Sys_PixelClockSet
*/
uint32_t R_VDCE_Sys_PixelClockSet(const uint32_t Unit,
                                  const uint32_t Clock,
                                  const uint32_t OtherUnitActive,
                                  const uint32_t Flags)
{
    uint32_t       ret;
    r_dev_Device_t device;

    if (Unit >= R_VDCE_MACRO_NUM)
    {
        ret = LOC_CLOCK_DISABLED;
    }
    else
    {
        /* activate VDCE pixel clock */
        /*  - pixel clock generator depends on the current device */
        device = R_DEV_GetDev();
        if (((device >= R_DEV_R7F701401) && (device <= R_DEV_R7F701407)) ||
            (device == R_DEV_R7F701442) || (device == R_DEV_R7F701418))
        {
            ret = loc_PixelSetClockL2M1(Clock);
        }
        else if (device == R_DEV_R7F701441)
        {
            ret = loc_PixelSetClockM1A(Unit, Clock, OtherUnitActive);
        }
        else
        {
            ret = loc_PixelSetClockM2(Unit, Clock, Flags);
        }
    }

    return ret;
}

/*******************************************************************************
  Function: R_VDCE_Sys_IntcInit
*/
r_vdce_Error_t R_VDCE_Sys_IntcInit(const uint32_t Unit)
{
    return loc_IntcCtrlAll(Unit, (uint8_t)R_TRUE);
}

/*******************************************************************************
  Function: R_VDCE_Sys_IntcDeinit
*/
r_vdce_Error_t R_VDCE_Sys_IntcDeinit(const uint32_t Unit)
{
    return loc_IntcCtrlAll(Unit, (uint8_t)R_FALSE);
}

/***************************************************************************
  Function: R_VDCE_Sys_IntcEnable
*/
r_vdce_Error_t R_VDCE_Sys_IntcEnable(const uint32_t       Unit,
                                     const r_dev_IntSel_t IntSel)
{
    return loc_IntcCtrl(Unit, IntSel, (uint8_t)R_TRUE);
}

/***************************************************************************
  Function: R_VDCE_Sys_IntcDisable
*/
r_vdce_Error_t R_VDCE_Sys_IntcDisable(const uint32_t       Unit,
                                      const r_dev_IntSel_t IntSel)
{
    return loc_IntcCtrl(Unit, IntSel, (uint8_t)R_FALSE);
}

/*******************************************************************************
  Function: R_VDCE_Sys_IntcPrepareClearCheck
*/
uint32_t R_VDCE_Sys_IntcPrepareClearCheck(const uint32_t SyncFreq)
{
    uint32_t   rate;
    uint32_t   sync_freq;
    uint32_t   cpu_cycle;
    uint32_t   i;

    /* Use default value if SyncFreq = 0. */
    sync_freq = SyncFreq;
    if (sync_freq == 0u)
    {
        sync_freq = LOC_SYNC_FREQ_MIN;
    }
    else
    {
        /* Do nothing */
    }

    /* Calculate rate to convert from Syncronization clock to CPU clock */
    rate = (LOC_CPU_FREQ_MAX + (sync_freq - 1u)) / sync_freq;

    /* 1st : Wait for INT_STAx to change 0->1 by NOP instruction. */
    /*       This change will not be caught by software. */
    cpu_cycle = rate * LOC_1ST_WAIT_CYCLE;
    for (i = 0u; i < cpu_cycle; i++)
    {
        R_DEV_Nop();
    }

    /* 2nd : Wait for INT_STAx to change 1->0 by Register Polling */
    /*       This function returns time-out count of register polling. */
    cpu_cycle = rate * LOC_2ND_WAIT_CYCLE;

    return cpu_cycle;
}

/***************************************************************************
  Function: R_VDCE_Sys_IntcClearCheck
*/
r_vdce_Error_t R_VDCE_Sys_IntcClearCheck(const uint32_t          Unit,
                                         const r_vdce_IntType_t IntcType)
{
    r_vdce_Error_t ret = R_VDCE_ERR_FATAL_HW;
    r_dev_IntSel_t intc_sel;
    uint32_t       i;
    uint8_t        intc_pending_flag;

    intc_sel = loc_ConvertToIntSel(Unit, IntcType);
    if (intc_sel >= R_DEV_INT_LAST)
    {
        ret = R_VDCE_ERR_RANGE_PARAM;
    }
    else
    {
        for (i = 0u; i < LOC_WAIT_CLEAR_COUNT; i++)
        {
            intc_pending_flag = R_DEV_IntGetFlag(intc_sel);
            if (intc_pending_flag == 0u)
            {
                ret = R_VDCE_ERR_OK;
                break;
            }
            else
            {
                /* Do nothing */
            }
        }
    }
    return ret;
}

/*******************************************************************************
  Function: R_VDCE_Sys_PortInit
*/
r_vdce_Error_t R_VDCE_Sys_PortInit(const uint32_t  Unit)
{
    return R_VDCE_ERR_OK;
}

/*******************************************************************************
  Function: R_VDCE_Sys_PortDeInit
*/
r_vdce_Error_t R_VDCE_Sys_PortDeInit(const uint32_t  Unit)
{
    return R_VDCE_ERR_OK;
}

/*******************************************************************************
  Function: R_VDCE_Sys_HsyncActLevelSet
*/
uint32_t R_VDCE_Sys_HsyncActLevelSet(const uint32_t Unit, const int32_t Level)
{
    /* HS Sync inversion is handled inside VDCE macro */
    return (uint32_t)R_FALSE;
}

/*******************************************************************************
  Function: R_VDCE_Sys_VsyncActLevelSet

*/
uint32_t R_VDCE_Sys_VsyncActLevelSet(const uint32_t Unit, const int32_t Level)
{
    /* VS Sync inversion is handled inside VDCE macro */
    return (uint32_t)R_FALSE;
}

/*******************************************************************************
  Function: R_VDCE_Sys_ClockActEdgeSet
*/
uint32_t R_VDCE_Sys_ClockActEdgeSet(const uint32_t Unit,
                                    const uint32_t Flags) 
{
    uint32_t        val;
    uint32_t        ret;
    uint32_t        vdce_ctl;
    uint32_t        pin_num;
    r_dev_Device_t  device; 

    if (Unit < R_VDCE_MACRO_NUM)
    {
        if ((Flags & (uint32_t)R_DDB_DISP_FLAG_RSDS_ENABLE) == 0u)
        {
            /* Get VDCECTL register */
            vdce_ctl = R_DEV_READ_REG(32, LOC_VDCECTL);
            if ((vdce_ctl & LOC_VDCECTL_VOSL) == 0u)
            {
                if (Unit == R_VDCE_UNIT0)
                {
                    pin_num = LOC_PINV45;
                }
                else
                {
                    pin_num = LOC_PINV47;
                }
            }
            else
            {
                if (Unit == R_VDCE_UNIT0)
                {
                    pin_num = LOC_PINV47;
                }
                else
                {
                    pin_num = LOC_PINV45;
                }
            }
            /* Check P47 with device */
            if (pin_num == LOC_PINV45)
            {
                /* Use port logic (PINV) to invert polarity of VDCE0_VO_CLKP signal on P45_0 */
                val = R_DEV_READ_REG(16, LOC_PINV45);
                if ((Flags & (uint32_t)R_DDB_DISP_FLAG_NEGCLK) == 0u)
                {
                    val &= ~(uint32_t)LOC_BIT_PIN0;
                }
                else
                {
                    val |= LOC_BIT_PIN0;
                }
                /* Set by macro function with do-while loop */
                R_DEV_WRITE_PORT_PROT_REG(LOC_PPCMD45, LOC_PINV45, val);  /* PRQA S 2214 */
                
                ret = (uint32_t)R_TRUE;
            }
            else
            {
                /* Check device that has P47  */
                device  =  R_DEV_GetDev();
                if ((device >= R_DEV_R7F701408) && (device <= R_DEV_R7F701412))
                {
                    /* Use port logic (PINV) to invert polarity of VDCE1_VO_CLK signal on P47_8 */
                    val = R_DEV_READ_REG(16, LOC_PINV47);
                    if ((Flags & (uint32_t)R_DDB_DISP_FLAG_NEGCLK) == 0u)
                    {
                        val &= ~(uint32_t)LOC_BIT_PIN8;
                    }
                    else
                    {
                        val |= LOC_BIT_PIN8;
                    }
                    /* Set by macro function with do-while loop */
                    R_DEV_WRITE_PORT_PROT_REG(LOC_PPCMD47, LOC_PINV47, val); /* PRQA S 2214 */
                    
                    ret = (uint32_t)R_TRUE;
                }
                else
                {
                    ret = (uint32_t)R_FALSE;
                }
            }
        }
        else
        {
            /* In case of RSDS, control by Phase shift with RSDSCFG  */
            ret = (uint32_t)R_TRUE;
        }
    }
    else
    {
        ret = (uint32_t)R_FALSE;
    }

    /* Clock inversion is NOT handled inside VDCE macro */
    return ret;
}

/*******************************************************************************
  Function: R_VDCE_Sys_StateSet
*/
r_vdce_Error_t R_VDCE_Sys_StateSet(const uint32_t       Unit,
                                   const uint32_t       LayerNr,
                                   const r_vdce_State_t State)
{
    r_vdce_Error_t      ret = R_VDCE_ERR_OK;
    uint32_t            i;

    if (Unit < R_VDCE_MACRO_NUM)
    {
        if (LayerNr == R_VDCE_SYS_LAYER_ALL)
        {
            for (i = 0u; i < R_VDCE_SYS_MAX_LAYER_NUM; i++)
            {
                loc_LayerState[Unit][i] = State;
            }

            loc_CaptState[Unit] = State;
        }
        /* R_VDCE_SYS_LAYER_CAPT and R_VDCE_LAYER_INPUT are one */
        /* to have replaced capture function with LayerNr. */
        /* When using capture function, it uses R_VDCE_SYS_LAYER_CAPT. */
        /* When using in Matrix, it uses R_VDCE_LAYER_INPUT. */
        
        /* R_VDCE_LAYER_INPUT or R_VDCE_SYS_LAYER_CAPT */
        else if ((LayerNr == R_VDCE_LAYER_INPUT) ||
                 (LayerNr == R_VDCE_SYS_LAYER_CAPT))
        {
            loc_CaptState[Unit] = State;
        }
        else if (LayerNr < R_VDCE_SYS_MAX_LAYER_NUM)
        {
            loc_LayerState[Unit][LayerNr] = State;
        }
        else
        {
            ret = R_VDCE_ERR_RANGE_LAYER;
        }
    }
    else
    {
        ret = R_VDCE_ERR_RANGE_UNIT;
    }
    return ret;
}

/*******************************************************************************
  Function: R_VDCE_Sys_StateGet
*/
r_vdce_State_t R_VDCE_Sys_StateGet(const uint32_t       Unit,
                                   const uint32_t       LayerNr)
{
    uint32_t i;
    r_vdce_State_t state = R_VDCE_STATE_UNINITIALIZED;
    
    if (Unit < R_VDCE_MACRO_NUM)
    {
        if (LayerNr == R_VDCE_SYS_LAYER_ALL)
        {
            for (i = 0u; i < R_VDCE_SYS_MAX_LAYER_NUM; i++)
            {
                if (state < loc_LayerState[Unit][i])
                {
                    state = loc_LayerState[Unit][i];
                }
            }

            if (state < loc_CaptState[Unit])
            {
                state = loc_CaptState[Unit];
            }
        }
        /* R_VDCE_SYS_LAYER_CAPT and R_VDCE_LAYER_INPUT are one */
        /* to have replaced capture function with LayerNr. */
        /* When using capture function, it uses R_VDCE_SYS_LAYER_CAPT. */
        /* When using in Matrix, it uses R_VDCE_LAYER_INPUT. */
        else if ((LayerNr == R_VDCE_LAYER_INPUT) ||
                 (LayerNr == R_VDCE_SYS_LAYER_CAPT))
        {
            state = loc_CaptState[Unit];
        }
        else if (LayerNr < R_VDCE_SYS_MAX_LAYER_NUM)
        {
            state = loc_LayerState[Unit][LayerNr];
        }
        else
        {
            /* Unkown layer */
            state = R_VDCE_STATE_UNINITIALIZED;
        }
    }
    else
    {
        /* Unkown unit */
        state = R_VDCE_STATE_UNINITIALIZED;
    }
    return state;
}

/*******************************************************************************
  Function: R_VDCE_Sys_DesyncActLevelSet
*/
uint32_t R_VDCE_Sys_DesyncActLevelSet(const uint32_t Unit, const int32_t Level) 
{
    /* DE Sync inversion is handled inside VDCE macro */
    return R_FALSE;
}

/*******************************************************************************
  Section: VDCE Macro Driver OS interface

  Comments see: <vdce_sys.h>
*/

/*******************************************************************************
  Function: R_VDCE_Sys_Lock
*/
r_vdce_Error_t R_VDCE_Sys_Lock(const uint32_t Unit)
{
    r_vdce_Error_t  ret;
    
    if (Unit < R_VDCE_MACRO_NUM)
    {
        /* Please implement the lock process by mutex or semaphore */
        /* if VDCE API is called from multi-thread. */
        
        ret = R_VDCE_ERR_OK;
    }
    else
    {
        ret = R_VDCE_ERR_RANGE_UNIT;
    }
    return ret;
}

/*******************************************************************************
  Function: R_VDCE_Sys_Unlock
*/
r_vdce_Error_t R_VDCE_Sys_Unlock(const uint32_t Unit)
{
    r_vdce_Error_t  ret;
    
    if (Unit < R_VDCE_MACRO_NUM)
    {
        /* Please implement the unlock process depending on R_VDCE_Sys_Lock. */
        
        ret = R_VDCE_ERR_OK;
    }
    else
    {
        ret = R_VDCE_ERR_RANGE_UNIT;
    }
    return ret;
}

/*******************************************************************************
  Function: R_VDCE_Sys_AllLock
*/
r_vdce_Error_t R_VDCE_Sys_AllLock(void)
{
    r_vdce_Error_t  ret;
    uint32_t        unit;
    uint32_t        unit2;
    
    for (unit = 0u; unit < R_VDCE_MACRO_NUM; unit++)
    {
        ret = R_VDCE_Sys_Lock(unit);
        
        if (ret != R_VDCE_ERR_OK)
        {
            /* Error Recovery */
            for (unit2 = 0u; unit2 < unit; unit2++)
            {
                (void)R_VDCE_Sys_Unlock(unit2);
            }
            break;
        }
    }
    
    return ret;
}

/*******************************************************************************
  Function: R_VDCE_Sys_AllUnlock
*/
r_vdce_Error_t R_VDCE_Sys_AllUnlock(void)
{
    r_vdce_Error_t  ret;
    uint32_t        unit;
    
    for (unit = 0u; unit < R_VDCE_MACRO_NUM; unit++)
    {
        ret = R_VDCE_Sys_Unlock(unit);
        if (ret != R_VDCE_ERR_OK)
        {
            break;
        }
    }
    
    return ret;
}

/*******************************************************************************
  Function: R_VDCE_Sys_VIChannelCheck
*/
r_vdce_Error_t R_VDCE_Sys_VIChannelCheck(const uint32_t Unit)
{
    r_vdce_Error_t  ret    = R_VDCE_ERR_OK;
    r_dev_Device_t  device = loc_GetDev();

    /* RH850/D1Lx */
    if (device <= R_DEV_R7F701403)
    {
        ret = R_VDCE_ERR_NOT_ACCEPTABLE;
    }
    /*
    *  device <= R_DEV_R7F701407     - RH850/D1M1(H)
    *  device = R_DEV_R7F701418
    *  device = R_DEV_R7F701442      - RH850/D1M1-V2
    */
    else if ((device <= R_DEV_R7F701407) ||
             (device == R_DEV_R7F701418) ||
             (device == R_DEV_R7F701442))
    {
        /* All units are available */
    }
    /* RH850/D1M2 */
    else if (device <= R_DEV_R7F701410)
    {
        if (Unit == R_VDCE_UNIT0)
        {
            ret = R_VDCE_ERR_RANGE_UNIT;
        }
        else
        {
            /* Do nothing */
        }
    }
    /* RH850/D1M2H */
    else if (device <= R_DEV_R7F701412)
    {
        /* All units are available */
    }
    /* RH850/D1M1A */
    else if (device == R_DEV_R7F701441)
    {
        if (Unit == R_VDCE_UNIT1)
        {
            ret = R_VDCE_ERR_RANGE_UNIT;
        }
        else
        {
            /* Do nothing */
        }
    }
    /* Unknown device */
    else
    {
        ret = R_VDCE_ERR_NOT_ACCEPTABLE;
    }
    return ret;
}

/***********************************************************
  Function: R_VDCE_Sys_MaxResolutionGet
*/
r_vdce_Error_t R_VDCE_Sys_MaxResolutionGet(uint32_t *ResolutionHmax,
                                           uint32_t *ResolutionVmax)
{
    r_vdce_Error_t    ret     =  R_VDCE_ERR_OK;
    r_dev_Device_t    device  =  loc_GetDev(); 

    if ((ResolutionHmax != R_NULL) && (ResolutionVmax != R_NULL))
    {
        /* RH850/D1Lx */
        if (device <= R_DEV_R7F701403)
        {
            *ResolutionHmax = LOC_RESOLUTION_480;
            *ResolutionVmax = LOC_RESOLUTION_320;
        }
        /*
        *  device <= R_DEV_R7F701407     - RH850/D1M1(H)
        *  device = R_DEV_R7F701418
        *  device = R_DEV_R7F701442     - RH850/D1M1-V2
        */
        else if ((device <= R_DEV_R7F701407) ||
                 (device == R_DEV_R7F701418) ||
                 (device == R_DEV_R7F701442))
        {
            *ResolutionHmax = LOC_RESOLUTION_1024;
            *ResolutionVmax = LOC_RESOLUTION_1024;
        }
        /*
        *  device <= R_DEV_R7F701412      - RH850/D1M2(H)
        *  device == R_DEV_R7F701441      - RH850/D1M1A
        */
        else if ((device <= R_DEV_R7F701412) ||
                 (device == R_DEV_R7F701441))
        {
            *ResolutionHmax = LOC_RESOLUTION_1280;
            *ResolutionVmax = LOC_RESOLUTION_1024;
        }
        /* Unknown device */
        else
        {
            ret = R_VDCE_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        ret = R_VDCE_ERR_PARAM_INCORRECT;
    }

    return ret;
}

/***********************************************************
  Function: R_VDCE_Sys_DeviceInfoGet
*/
r_vdce_Error_t R_VDCE_Sys_DeviceInfoGet(r_vdce_DeviceInfo_t *DevInfo)
{
    r_vdce_Error_t ret    = R_VDCE_ERR_OK;
    r_dev_Device_t device = loc_GetDev(); 

    if (DevInfo != R_NULL)
    {
        /* Set default */
        DevInfo->Device         = device;
        DevInfo->VIWithDataEn   = R_FALSE;
        DevInfo->SerialRGBEn    = R_FALSE;
        DevInfo->GammaCorrectEn = R_TRUE;
        DevInfo->EnlargementEn  = R_TRUE;

        /* RH850/D1Lx */
        if (device <= R_DEV_R7F701403)
        {
            DevInfo->GammaCorrectEn = R_FALSE;
            DevInfo->EnlargementEn  = R_FALSE;
        }
        /* RH850/D1M1(H) */
        else if (device <= R_DEV_R7F701407)
        {
        }
        /* RH850/D1M2x */
        else if ((device <= R_DEV_R7F701412) ||
                 (device == R_DEV_R7F701418))
        {
            DevInfo->VIWithDataEn   = R_TRUE;
        }
        /* RH850/D1M1A, RH850/D1M1-V2 */
        else if ((device == R_DEV_R7F701441) ||
                 (device == R_DEV_R7F701442))
        {
            DevInfo->VIWithDataEn   = R_TRUE;
            DevInfo->SerialRGBEn    = R_TRUE;
        }
        else
        {
            ret = R_VDCE_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        ret = R_VDCE_ERR_PARAM_INCORRECT;
    }

    return ret;
}
