/*
****************************************************************************
PROJECT : VLIB macro device level driver
============================================================================
DESCRIPTION
SPEA system functions for d1x
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

/*******************************************************************************
  Title: dev_spea SPEA device functions

  Implementation of the SPEA support functions for the 
  device dev_spea.
*/

/*******************************************************************************
  Section: Includes
*/
#include "r_typedefs.h"
#include "r_dev_api.h"
#include "r_spea_api.h"
#include "r_spea_sys.h"
#include "r_config_spea.h"

#if defined (R_DBG_PRINT_DEV_REGS) || defined (R_DBG_PRINT_MSG)
#include<stdio.h>
#include "r_dbg_api.h"
#endif

/*******************************************************************************
  Section: Local Constants
*/

/* Sprite Engine update timing control register */
#define LOC_SPEA_UPDEN_ADR    (0xFFC06048u)
#define LOC_SPEA_UPDEN_VAL    (0x0F0Fu)

/*******************************************************************************
  Section: Local variables
*/

#ifdef R_BSP_SYS_INIT_USE
static  r_spea_State_t loc_SpeaState[R_SPEA_SU_LAST];
#else
static  r_spea_State_t loc_SpeaState[R_SPEA_SU_LAST] =
                                {
                                    R_SPEA_STATE_UNINITIALIZED, /* R_SPEA_RLE0     */
                                    R_SPEA_STATE_UNINITIALIZED, /* R_SPEA_RLE1     */
                                    R_SPEA_STATE_UNINITIALIZED, /* R_SPEA_RLE2     */
                                    R_SPEA_STATE_UNINITIALIZED, /* R_SPEA_RLE3     */
                                    R_SPEA_STATE_UNINITIALIZED, /* R_SPEA_SU_OFFSET*/
                                    R_SPEA_STATE_UNINITIALIZED, /* R_SPEA_SU0      */
                                    R_SPEA_STATE_UNINITIALIZED, /* R_SPEA_SU1      */
                                    R_SPEA_STATE_UNINITIALIZED, /* R_SPEA_SU2      */
                                    R_SPEA_STATE_UNINITIALIZED  /* R_SPEA_SU3      */
                                };
#endif

/*******************************************************************************
  Section: Global Functions

*/
/*******************************************************************************
  Function: R_SPEA_SYS_BaseAddr

  See: r_spea_sys.h for details
*/
void R_SPEA_SYS_InitGlobal(void)
{
    uint32_t i;
    
    for (i = 0u; i < (uint32_t)R_SPEA_SU_LAST; i++)
    {
        loc_SpeaState[i] = R_SPEA_STATE_UNINITIALIZED;
    }
}

/*******************************************************************************
  Function: R_SPEA_SYS_BaseAddr

  See: r_spea_sys.h for details
*/
uint32_t R_SPEA_SYS_BaseAddr(const uint32_t Unit)
{
    uint32_t ret;
    
    if (Unit == R_SPEA_UNIT0)
    {
        ret = R_SPEA_BASE;
    }
    else
    {
        ret = 0u;
    }

    return ret;
}

/*******************************************************************************
  Function: R_SPEA_SYS_HardwareInit

  See: r_spea_sys.h for details
*/
r_spea_Error_t R_SPEA_SYS_HardwareInit(const uint32_t Unit)
{
    r_spea_Error_t ret = R_SPEA_ERR_OK;

    if (Unit >= R_SPEA_MACRO_NUM)
    {
        ret = R_SPEA_ERR_NG;
    }
    else
    {
        /* Setup SPEAUPDEN register */
        /*  See H/W user's manual chapter 37.6 for the detail  */
        R_DEV_WRITE_REG(32, LOC_SPEA_UPDEN_ADR, LOC_SPEA_UPDEN_VAL);
    }
    return ret;
}

/*******************************************************************************
  Function: R_SPEA_Sys_HardwareDeInit

  See: r_spea_sys.h for details
*/
r_spea_Error_t R_SPEA_SYS_HardwareDeInit(uint32_t Unit)
{
    r_spea_Error_t ret = R_SPEA_ERR_OK;
    
    if (Unit >= R_SPEA_MACRO_NUM)
    {
        ret = R_SPEA_ERR_NG;
    }
    else
    {
        /* Do nothing */
    }
    return ret;
}

/*******************************************************************************
  Function: R_SPEA_SYS_ErrorHandler

  See: r_spea_sys.h for details
*/
void R_SPEA_SYS_ErrorHandler(const uint32_t Unit, r_spea_Error_t Error)
{
#ifdef R_SPEA_ERROR_DEBUG
    R_DEV_ERROR(Unit, (uint32_t)Error, "SPEA: Error occured");
#endif
}

/*******************************************************************************
  Function: R_SPEA_SYS_IsD1M1A

  See: r_spea_sys.h for details
*/
int8_t R_SPEA_SYS_IsD1M1A(void)
{
    r_dev_Device_t device;
    int8_t         d1m1a;

    device = R_DEV_GetDev();
    
    if (device == R_DEV_R7F701441)
    {
        d1m1a = R_TRUE;
    }
    else if (device == R_DEV_R7F701418)
    {
        d1m1a = R_TRUE;
    }
    else if (device == R_DEV_R7F701460)
    {
        d1m1a = R_TRUE;
    }
    else
    {
        d1m1a = R_FALSE;
    }
    return d1m1a;
}

/*******************************************************************************
  Function: R_SPEA_Sys_IsD1M1v2

  See: r_spea_sys.h for details
*/
int8_t R_SPEA_SYS_IsD1M1v2(void)
{
    r_dev_Device_t device;
    int8_t         d1m1v2;
    
    device = R_DEV_GetDev();

    if (device == R_DEV_R7F701442)
    {
        d1m1v2 = R_TRUE;
    }
    else
    {
        d1m1v2 = R_FALSE;
    }
    return d1m1v2;
}

/*******************************************************************************
  Function: R_SPEA_SYS_StateSet

  See: r_spea_sys.h for details
*/
r_spea_Error_t R_SPEA_SYS_StateSet(const uint32_t        Unit,
                                   const r_spea_State_t  State,
                                   const r_spea_Unit_t   SpUnit)
{
    r_spea_Error_t err = R_SPEA_ERR_OK;

    if (Unit < R_SPEA_MACRO_NUM)
    {
        if ((SpUnit >= R_SPEA_RLE0) && (SpUnit < R_SPEA_SU_LAST))
        {
            loc_SpeaState[SpUnit] = State;
        }
        else
        {
            err = R_SPEA_ERR_NG;
        }
    }
    else
    {
        err = R_SPEA_ERR_NG;
    }
    return err;
}

/*******************************************************************************
  Function: R_SPEA_SYS_StateGet

  See: r_spea_sys.h for details
*/
r_spea_State_t R_SPEA_SYS_StateGet(const uint32_t      Unit,
                                   const r_spea_Unit_t SpUnit)
{
    r_spea_State_t status;

    if (Unit < R_SPEA_MACRO_NUM)
    {
        if ((SpUnit >= R_SPEA_RLE0) && (SpUnit < R_SPEA_SU_LAST))
        {
            status = loc_SpeaState[SpUnit];
        }
        else
        {
            status = R_SPEA_STATE_UNINITIALIZED;
        }
    }
    else
    {
        status = R_SPEA_STATE_UNINITIALIZED;
    }

    return status;
}

