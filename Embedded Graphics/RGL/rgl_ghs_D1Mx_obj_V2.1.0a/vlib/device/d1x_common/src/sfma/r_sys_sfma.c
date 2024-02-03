/*
****************************************************************************
PROJECT : SFMA driver
FILE    : $Id: r_sys_sfma.c 16482 2018-12-07 10:16:09Z matthias.nippert@renesas.com $
============================================================================
DESCRIPTION
SFMA support functions for d1x
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


/***************************************************************************
  Title: SFMA Driver support functions

  Implementation of the SFMA support functions for the D1x device.

*/

/***************************************************************************
  Section: Includes
*/
#include "r_typedefs.h"
#include "r_sfma_api.h"
#include "r_sfma_sys.h"
#include "r_config_sfma.h"
#include "r_dev_api.h"


/***************************************************************************
  Section: Local Defines
*/

/*******************************************************************************
  Constant: LOC_SYS_SFMA_CKDLY

  The offset value of the CKDLY register from the base address.
*/
#define LOC_SYS_SFMA_CKDLY              (0x00000050uL)

/*******************************************************************************
  Constant: LOC_SYS_SFMA_SPODLY

  The offset value of the SPODL register from the base address.
*/
#define LOC_SYS_SFMA_SPODLY             (0x00000068uL)

/*******************************************************************************
  Constant: LOC_SYS_SFMA_CLOCK_ADJUSTED_LIMIT

  The limit value which needs the adjustment of the phase relationship.
*/
#define LOC_SYS_SFMA_CLOCK_ADJUSTED_LIMIT       (60 * 1000 * 1000)

/***************************************************************************
  Constant: LOC_SYS_SFMA_DRCR

  The offset value of the DRCR register from the base address.
*/
#define LOC_SYS_SFMA_DRCR               (0x0c)


/***************************************************************************
  Constant: LOC_SYS_SFMA_CKDLY_MASK

  Mask for the CKDLY register.
*/
#define LOC_SYS_SFMA_CKDLY_MASK         (0xFFF8FFF8uL)


/***************************************************************************
  Constant: LOC_SFMA_DEFAULT_CAL

  Default calibration for Serial flash memory.

    SPBCLK clock phase shift        3 (Upper 16bits)
    Sampling point phase shift      1 (Lower 16 bits)
*/

#define SPBCLK_CLOCK_PHASE_SHIFT    (3u)
#define SAMPLING_POINT_PHASE_SHIFT  (1u)
#define LOC_SFMA_DEFAULT_CAL        ((SPBCLK_CLOCK_PHASE_SHIFT << 16) | SAMPLING_POINT_PHASE_SHIFT)

/***************************************************************************
  Constant: LOC_SYS_SFMA

*/
#define LOC_SYS_SFMA_UNIT       (3u)

/***************************************************************************
  Constant: LOC_SYS_DEV_NUM

  Number of device information resource.
  It should be simultaneous operation number.
  SFMA0 and (SFMA1 or SFMA2)
*/
#define LOC_SYS_DEV_NUM         (2u)

/***************************************************************************
  Constant: LOC_SYS_SFMA

  The Unit number for SFMA.
*/
#define LOC_SYS_SFMA0  (0u)
#define LOC_SYS_SFMA1  (1u)
#define LOC_SYS_SFMA2  (2u)

/***************************************************************************
  Constant: LOC_SYS_SFMA_MAX_MACRO_NUM

  The number of unit for SFMA.
*/
#define LOC_SYS_SFMA_MAX_MACRO_NUM        (1u)
#define LOC_SYS_SFMA_MAX_MACRO_NUM_D1M1A  (3u)
#define LOC_SYS_SFMA_MAX_MACRO_NUM_D1M1V2 (2u)

/***************************************************************************
  Constant: LOC_SYS_SFMA_MAX_CLK

  The maximum serial clock SPBCLK.
  SFMA1 for D1M1A is maximum 40MHz.
  The other are maximum 80MHz.
*/
#define LOC_SYS_SFMA_MAX_CLK_40MHz        ( 40 * 1000 * 1000)
#define LOC_SYS_SFMA_MAX_CLK_80MHz        ( 80 * 1000 * 1000)
#define LOC_SYS_SFMA_MAX_CLK_120MHz       (120 * 1000 * 1000)

/***************************************************************************
  Constant: LOC_XCRAMCFG0

  Bus Switch for external memory interfaces.
*/
#define LOC_XCRAMCFG0           0xFFC06050u
/* Bus Switch for SFMA1 and SFMA2 control register bits */
#define LOC_XCRAMCFG0_SFMASEL   ((uint32_t)(1ul<<9u))

/***************************************************************************
  Section: Local Variables
*/
/*******************************************************************************
  Variable: r_sfma_Dev

  This value stores the setting information of SFMA driver.
  SFMA1 and SFMA2 use same resource.
*/
static r_sfma_Dev_t   r_sfma_Dev[LOC_SYS_DEV_NUM];

/*******************************************************************************
  Variable: loc_sfma_base

  This value stores the base address of SFMA driver.
*/
static uint32_t loc_sfma_base[LOC_SYS_SFMA_UNIT] = {R_SFMA_BASE_ADD0, R_SFMA_BASE_ADD1, R_SFMA_BASE_ADD1};

/***************************************************************************
  Section: SFMA driver support functions

  Comments see: <r_sfma_sys.h>
*/

/***************************************************************************
  Function: R_SFMA_Sys_NumberOfUnitGet

  see: <r_sfma_sys.h>
*/
uint32_t R_SFMA_Sys_NumberOfUnitGet(void)
{
    static uint32_t loc_sfma_NumOfUnit = 0;
    r_dev_Device_t device;

    if (loc_sfma_NumOfUnit == 0)
    {
        device = R_DEV_GetDev();
        if (device == R_DEV_R7F701441)    /* RH850/D1M1A */
        {
            loc_sfma_NumOfUnit = LOC_SYS_SFMA_MAX_MACRO_NUM_D1M1A;
        }
        else if ((device == R_DEV_R7F701442) /* RH850/D1M1-V2 */
              || (device == R_DEV_R7F701418))
        {
            loc_sfma_NumOfUnit = LOC_SYS_SFMA_MAX_MACRO_NUM_D1M1V2;
        }
        else
        {
            loc_sfma_NumOfUnit = LOC_SYS_SFMA_MAX_MACRO_NUM;
        }
    }

    return loc_sfma_NumOfUnit;
}

/***************************************************************************
  Function: R_SFMA_Sys_ExclusiveUnitGet

  see: <r_sfma_sys.h>
*/
uint32_t R_SFMA_Sys_ExclusiveUnitGet(const uint32_t Unit)
{
    uint32_t  ex_unit;
    
        /* Unit => ex_unit            */
        /* -------------------------- */
        /*  0   =>    3 (range over)  */
        /*  1   =>    2               */
        /*  2   =>    1               */
    
    if (Unit == LOC_SYS_SFMA1)
    {
        ex_unit = LOC_SYS_SFMA2;
    }
    else if (Unit == LOC_SYS_SFMA2)
    {
        ex_unit = LOC_SYS_SFMA1;
    }
    else /* Unit == LOC_SYS_SFMA0 */
    {
        ex_unit = LOC_SYS_SFMA_UNIT;
    }
    
    return ex_unit;
}

/***************************************************************************
  Function: R_SFMA_Sys_SwapUnitGet

  see: <r_sfma_sys.h>
*/
uint32_t R_SFMA_Sys_SwapUnitGet(const uint32_t Unit)
{
    uint32_t  swap_unit   = LOC_SYS_SFMA0;
    uint32_t  reg_val     = 0;
    r_dev_Device_t device = R_DEV_GetDev();
    
    if (Unit == LOC_SYS_SFMA0)
    {
        if (device >= R_DEV_R7F701441)
        {
            reg_val = R_DEV_READ_REG(32, LOC_XCRAMCFG0);
            if ((reg_val & LOC_XCRAMCFG0_SFMASEL) == 0)
            {
                swap_unit = LOC_SYS_SFMA1;
            }
            else
            {
                swap_unit = LOC_SYS_SFMA2;
            }
        }
    }
    return swap_unit;
}

/***************************************************************************
  Function: R_SFMA_Sys_GetDevInfo

  see: <r_sfma_sys.h>
*/
r_sfma_Dev_t* R_SFMA_Sys_GetDevInfo(const uint32_t Unit)
{
    r_sfma_Dev_t* p_dev; 
    
    if (Unit == LOC_SYS_SFMA0)
    {
        p_dev = &r_sfma_Dev[LOC_SYS_SFMA0];
    }
    else /* (Unit == LOC_SYS_SFMA1) || (Unit == LOC_SYS_SFMA2) */
    {
        p_dev = &r_sfma_Dev[LOC_SYS_SFMA1];
    }
    return p_dev;
}

/***************************************************************************
  Function: R_SFMA_Sys_Init

  see: <r_sfma_sys.h>
*/
r_sfma_Error_t R_SFMA_Sys_Init(const uint32_t  Unit)
{
    r_sfma_Error_t sys_err = R_SFMA_ERR_OK;

    return sys_err;
}


/***************************************************************************
  Function: R_SFMA_Sys_DeInit

  see: <r_sfma_sys.h>
*/

r_sfma_Error_t R_SFMA_Sys_DeInit(const uint32_t  Unit)
{
    r_sfma_Error_t sys_err = R_SFMA_ERR_OK;

    return sys_err;
}

/***************************************************************************
  Function: R_SFMA_Sys_BaseAddrGet

  see: <r_sfma_sys.h>
*/
uint32_t R_SFMA_Sys_BaseAddrGet(const uint32_t Unit)
{
    uint32_t addr;
    uint32_t num_of_unit;

    num_of_unit = R_SFMA_Sys_NumberOfUnitGet();
    if (Unit >= num_of_unit)
    {
        addr = 0;
    }
    else
    {
        addr = loc_sfma_base[Unit];
    }
    return addr;
}

/***************************************************************************
  Function: R_SFMA_Sys_ClockGet

  see: <r_sfma_sys.h>
*/
uint32_t R_SFMA_Sys_ClockGet(const uint32_t Unit)
{
    uint32_t clk;
    uint32_t num_of_unit;

    num_of_unit = R_SFMA_Sys_NumberOfUnitGet();
    if (Unit >= num_of_unit)
    {
        clk = 0;
    }
    else
    {
        clk = R_DEV_ClkFrequencyHz(R_DEV_CKS_SFMA);
        
        /* SFMA1 clock is divided by 2. */
        if (Unit == 1)
        {
            if (R_DEV_GetVersion() != R_DEV_REVISION_EMU)
            {
                clk = clk / 2;
            }
        }
    }

    return clk;
}


/***************************************************************************
  Function: R_SFMA_Sys_CalibSetting

  see: <r_sfma_sys.h>
*/
r_sfma_Error_t R_SFMA_Sys_CalibSetting(const uint32_t Unit,
                                       const r_sfma_Config_t * const Config)
{
    uint32_t base;
    r_sfma_Error_t sys_err = R_SFMA_ERR_OK;

    if (Config != R_NULL)
    {
        base = R_SFMA_Sys_BaseAddrGet(Unit);

        if (Config->SerialFlashMemoryMaxClock >= LOC_SYS_SFMA_CLOCK_ADJUSTED_LIMIT)
        {
            /* check for valid calibration values */
            if(0 == (LOC_SYS_SFMA_CKDLY_MASK & Config->Calibration))
            {
                R_SFMA_WRITE_REG(32, (base+LOC_SYS_SFMA_CKDLY),  0xa5000000u + Config->Calibration); /* Clock delay */
                R_SFMA_WRITE_REG(32, (base+LOC_SYS_SFMA_SPODLY), 0xa5000000u); /* No Data delay */
            }
            else
            {
                /* error use a default */
                R_SFMA_WRITE_REG(32, (base+LOC_SYS_SFMA_CKDLY),  0xa5000000u + (LOC_SFMA_DEFAULT_CAL)); /* Clock delay */
                R_SFMA_WRITE_REG(32, (base+LOC_SYS_SFMA_SPODLY), 0xa5000000u); /* No Data delay */

                /* error calibration value */
                sys_err = R_SFMA_ERR_PARAM_INCORRECT;
            }
        }
        else
        {
            /* low frequency setting */

            /* clock delay */
            R_SFMA_WRITE_REG(32, (base+LOC_SYS_SFMA_CKDLY),  0xa5000000u);

            /* fixed 2ns delay  */
            R_SFMA_WRITE_REG(32, (base+LOC_SYS_SFMA_SPODLY), 0xa5001111u);
        }
    }
    else
    {
        sys_err = R_SFMA_ERR_PARAM_INCORRECT;
    }

    return sys_err;
}


/***************************************************************************
  Function: R_SFMA_Sys_Close

  see: <r_sfma_sys.h>
*/
r_sfma_Error_t R_SFMA_Sys_Close(const uint32_t Unit)
{
    uint32_t base;
    r_sfma_Error_t sys_err = R_SFMA_ERR_OK;
    uint32_t regval;

    /* terminate any transfers from external address space */
    base = R_SFMA_Sys_BaseAddrGet(Unit);

    regval = R_SFMA_READ_REG(32, base + LOC_SYS_SFMA_DRCR);
    R_SFMA_WRITE_REG(32, (base + LOC_SYS_SFMA_DRCR),   (regval | 0x01000000uL)); /* set negate */

    return sys_err;
}


/***************************************************************************
  Function: R_SFMA_Sys_DDREnable

  see: <r_sfma_sys.h>
*/
uint8_t R_SFMA_Sys_DDREnable(void)
{
    uint32_t device;
    uint8_t  ret;

    device = R_DEV_GetDev();
    if (device == R_DEV_R7F701401)
    {
        ret = R_FALSE;
    }
    else
    {
        ret = R_TRUE;
    }
    return ret;
}

/***************************************************************************
  Function: R_SFMA_Sys_BusWidth8BitEnable

  see: <r_sfma_sys.h>
*/
uint8_t R_SFMA_Sys_BusWidth8BitEnable(const uint32_t Unit)
{
    uint32_t device;
    uint8_t  ret;

    device = R_DEV_GetDev();
    if ((device == R_DEV_R7F701401) ||
        (device == R_DEV_R7F701402))
    {
        ret = R_FALSE;
    }
    else
    {
        if (Unit == LOC_SYS_SFMA2)
        {
            ret = R_FALSE;
        }
        else
        {
            ret = R_TRUE;
        }
    }
    return ret;
}

/***************************************************************************
  Function: R_SFMA_Sys_SyncPeripheral

  see: <r_sfma_sys.h>
*/
void R_SFMA_Sys_SyncPeripheral(void)
{
    R_DEV_SyncP();
}

/***************************************************************************
  Function: R_SFMA_Sys_Lock

  see: <r_sfma_sys.h>
*/
r_sfma_Error_t R_SFMA_Sys_Lock(const uint32_t Unit)
{
    r_sfma_Error_t sys_err = R_SFMA_ERR_OK;

    return sys_err;
}

/***************************************************************************
  Function: R_SFMA_Sys_Unlock

  see: <r_sfma_sys.h>
*/
r_sfma_Error_t R_SFMA_Sys_Unlock(const uint32_t Unit)
{
    r_sfma_Error_t sys_err = R_SFMA_ERR_OK;

    return sys_err;
}

/***************************************************************************
  Function: R_SFMA_Sys_AllLock

  see: <r_sfma_sys.h>
*/
r_sfma_Error_t R_SFMA_Sys_AllLock(void)
{
    r_sfma_Error_t sys_err = R_SFMA_ERR_OK;

    return sys_err;
}

/***************************************************************************
  Function: R_SFMA_Sys_AllUnlock

  see: <r_sfma_sys.h>
*/
r_sfma_Error_t R_SFMA_Sys_AllUnlock(void)
{
    r_sfma_Error_t sys_err = R_SFMA_ERR_OK;

    return sys_err;
}

/***************************************************************************
  Function: R_SFMA_Sys_Relax

  see: <r_sfma_sys.h>
*/
r_sfma_Error_t R_SFMA_Sys_Relax(const uint32_t Unit)
{
    r_sfma_Error_t sys_err = R_SFMA_ERR_OK;

    return sys_err;
}


/***************************************************************************
  Function: R_SFMA_Sys_GetCal

  see: <r_sfma_sys.h>
*/
uint32_t R_SFMA_Sys_GetCal(const uint32_t Unit)
{
    uint32_t base;
    uint32_t regval;
    uint32_t num_of_unit;

    num_of_unit = R_SFMA_Sys_NumberOfUnitGet();
    if (Unit >= num_of_unit)
    {
        /* error */
        regval = 0xFFFFFFFFuL;
    }
    else
    {
        /* get the base address */
        base = R_SFMA_Sys_BaseAddrGet(Unit);
        regval = R_SFMA_READ_REG(32, base + LOC_SYS_SFMA_CKDLY);
    }

    return regval;
}

/***************************************************************************
  Function: R_SFMA_Sys_UnitWindowSelect

  see: <r_sfma_sys.h>
*/
r_sfma_Error_t R_SFMA_Sys_UnitWindowSelect(const uint32_t Unit,
                                           const r_sfma_WindowMode_t Window)
{
    /* This function is not supported. */
    r_sfma_Error_t sys_err = R_SFMA_ERR_NOT_SUPPORTED;

    return sys_err;
}

/***************************************************************************
  Function: R_SFMA_Sys_UnitNumberGet

  see: <r_sfma_sys.h>
*/
r_sfma_Error_t R_SFMA_Sys_UnitNumberGet(const r_sfma_WindowMode_t Window,
                                        uint32_t * const Unit)
{
    /* This function is not supported. */
    r_sfma_Error_t sys_err = R_SFMA_ERR_NOT_SUPPORTED;

    return sys_err;
}

/***************************************************************************
  Function: R_SFMA_Sys_MinClockDivGet

  see: <r_sfma_sys.h>
*/
uint32_t R_SFMA_Sys_MinClockDivGet(const uint32_t Unit, const uint32_t Clock, const uint8_t TransferMode)
{
    r_dev_Device_t device = R_DEV_GetDev();
    uint32_t clock_div = 0;
    uint32_t max_clock = 0;
    
    if (Unit == LOC_SYS_SFMA0)
    {
        if(device == R_DEV_R7F701401) /* RH850/D1L1 */
        {
            max_clock = LOC_SYS_SFMA_MAX_CLK_40MHz;
        }
        else /* RH850/D1L2 ~ */
        {
            if (TransferMode == 0) /* SDR */
            {
                max_clock = LOC_SYS_SFMA_MAX_CLK_120MHz;
            }
            else /* DDR */
            {
                max_clock = LOC_SYS_SFMA_MAX_CLK_80MHz;
            }
        }
    }
    else if (Unit == LOC_SYS_SFMA1)
    {
        max_clock = LOC_SYS_SFMA_MAX_CLK_40MHz;
    }
    else /* Unit == LOC_SYS_SFMA2 */
    {
        max_clock = LOC_SYS_SFMA_MAX_CLK_80MHz;
    }
    
    /* Calculate with Round-Up (to avoid returning too small value.) */
    clock_div = (Clock + (max_clock - 1)) / max_clock;

        /* (Clock, max_clock) => clock_div */
        /* ------------------------------- */
        /* (160MHz, 120MHz)   =>    2      */
        /* (160MHz,  80MHz)   =>    2      */
        /* (160MHz,  40MHz)   =>    4      */
        /* (120MHz, 120MHz)   =>    1      */
        /* (120MHz,  80MHz)   =>    2      */
        /* (120MHz,  40MHz)   =>    3      */
        
    return clock_div;
}

