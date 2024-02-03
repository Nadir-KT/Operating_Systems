/*
****************************************************************************
PROJECT : VOCA driver
============================================================================
DESCRIPTION
VOCA support functions
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2019
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
  Title: VOCA Support functions

    VOCA (Video Output Checker A) driver Support functions
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_dev_api.h"
#include "r_config_voca.h"
#include "r_voca_api.h"
#include "r_voca_sys.h"

/***************************************************************************
  Section: Local Defines
*/

/***************************************************************************
  Section: Local Variables
*/

/***************************************************************************
  Variable: loc_VocaState

  This value stores the state of the VOCA driver.
*/
#ifdef R_BSP_SYS_INIT_USE
R_VOCA_STATIC r_voca_State_t loc_VocaState[R_VOCA_MACRO_NUM];
#else
R_VOCA_STATIC r_voca_State_t loc_VocaState[R_VOCA_MACRO_NUM] =
{
    R_VOCA_STATE_UNINITIALIZED
};
#endif /* R_BSP_RGL_INIT_USE */

/***************************************************************************
  Section: Local Functions
*/

/***************************************************************************
  Section: Global Functions
*/

/***************************************************************************
  Group: VOCA driver support functions

  Comments see: <r_voca_sys.h>
*/

/***************************************************************************
  Function: R_VOCA_Sys_InitGlobal
*/
void R_VOCA_Sys_InitGlobal(void)
{
    uint32_t    unit;
    
    for (unit = 0u; unit < R_VOCA_MACRO_NUM; unit++)
    {
        loc_VocaState[unit] = R_VOCA_STATE_UNINITIALIZED;
    }
}

/***************************************************************************
  Function: R_VOCA_Sys_Init

  See: <r_voca_sys.h> for details
*/

r_voca_Error_t R_VOCA_Sys_Init(const uint32_t Unit) 
{
    return R_VOCA_ERR_OK;
}
/***************************************************************************
  Function: R_VOCA_Sys_DeInit

  See: <r_voca_sys.h> for details
*/

r_voca_Error_t R_VOCA_Sys_DeInit(const uint32_t Unit) 
{
    return R_VOCA_ERR_OK;
}

/***************************************************************************
  Function: R_VOCA_Sys_InterruptEnable

  See: <r_voca_sys.h> for details
*/

void R_VOCA_Sys_InterruptEnable(const uint32_t   Unit)
{
}

/***************************************************************************
  Function: R_VOCA_Sys_InterruptDisable

  See: <r_voca_sys.h> for details
*/

void R_VOCA_Sys_InterruptDisable(const uint32_t Unit)
{
}

/***************************************************************************
  Function: R_VOCA_Sys_BaseAddr

  See: <r_voca_sys.h> for details
*/

uint32_t R_VOCA_Sys_BaseAddrGet(const uint32_t Unit)
{
    uint32_t base;

    if (Unit == R_VOCA_UNIT0)
    {
        base = R_VOCA_BASE_ADDR;
    }
    else
    {
        base = 0u;
    }

    return base;
}

/***************************************************************************
  Function: R_VOCA_Sys_MaxVideoChannelGet

  See: <r_voca_sys.h> for details
*/
uint32_t R_VOCA_Sys_MaxVideoChannelGet(const uint32_t Unit)
{
    uint32_t        ch = 0;
    r_dev_Device_t  device  =  R_DEV_GetDev(); 

    if (Unit < R_VOCA_MACRO_NUM)
    {
        if (((device >= R_DEV_R7F701404) && (device <= R_DEV_R7F701407)) ||
            (device == R_DEV_R7F701418) ||
            (device == R_DEV_R7F701442))
        {
            /* For D1M1(H), D1M1-V2 */
            ch = R_VOCA_MAX_VO_CH_NUM;
        }
        else if (((device >= R_DEV_R7F701408) && (device <= R_DEV_R7F701412)) ||
            (device == R_DEV_R7F701441))
        {
            /* For D1M1A, D1M2(H) */
            ch = R_VOCA_MAX_VO_CH_NUM2;
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

    return ch;
}

/***************************************************************************
  Function: R_VOCA_Sys_StateSet

  See: <r_voca_sys.h> for details
*/
r_voca_Error_t R_VOCA_Sys_StateSet(const uint32_t         Unit,
                                       const r_voca_State_t State)
{
    r_voca_Error_t ret = R_VOCA_ERR_OK;

    if (Unit < R_VOCA_MACRO_NUM)
    {
        loc_VocaState[Unit] = State;
    }
    else
    {
        ret = R_VOCA_ERR_RANGE_UNIT;
    }
    return ret;
}

/***************************************************************************
  Function: R_VOCA_Sys_StateGet

  See: <r_voca_sys.h> for details
*/
r_voca_State_t R_VOCA_Sys_StateGet(const uint32_t      Unit)
{
    r_voca_State_t status;

    if (Unit < R_VOCA_MACRO_NUM)
    {
        status = loc_VocaState[Unit];
    }
    else
    {
        status = R_VOCA_STATE_UNINITIALIZED;
    }

    return status;
}

/***************************************************************************
  Group: VOCA Macro Driver OS interface

  Comments see: <r_voca_sys.h>
*/

/***************************************************************************
  Function: R_VOCA_Sys_Lock

  See: <r_voca_sys.h> for details
*/
r_voca_Error_t R_VOCA_Sys_Lock(const uint32_t Unit)
{
    r_voca_Error_t  ret;
    
    if (Unit < R_VOCA_MACRO_NUM)
    {
        /* Please implement the lock process by mutex or semaphore */
        /* if VOCA API is called from multi-thread. */
        
        ret = R_VOCA_ERR_OK;
    }
    else
    {
        ret = R_VOCA_ERR_RANGE_UNIT;
    }
    return ret;
}

/***************************************************************************
  Function: R_VOCA_Sys_Unlock

  See: <r_voca_sys.h> for details
*/
r_voca_Error_t R_VOCA_Sys_Unlock(const uint32_t Unit)
{
    r_voca_Error_t  ret;
    
    if (Unit < R_VOCA_MACRO_NUM)
    {
        /* Please implement the unlock process depending on R_VOCA_Sys_Lock. */
        
        ret = R_VOCA_ERR_OK;
    }
    else
    {
        ret = R_VOCA_ERR_RANGE_UNIT;
    }
    return ret;
}

