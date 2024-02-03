/*
****************************************************************************
PROJECT : DISCOM driver
============================================================================
DESCRIPTION
DISCOM support functions
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
  Title: DISCOM Support functions

    DISCOM (Display Output Comparator) driver Support functions
*/

/***************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_dev_api.h"
#include "r_discom_api.h"
#include "r_discom_sys.h"
#include "r_config_discom.h"

/***************************************************************************
  Section: Local Defines
*/

/***************************************************************************
  Section: Local Variables
*/

/***************************************************************************
  Variable: loc_DiscomState

  This value stores the state of the DISCOM driver.
*/
#ifdef R_BSP_SYS_INIT_USE
R_DISCOM_STATIC r_discom_State_t loc_DiscomState[R_DISCOM_MACRO_NUM];
#else
R_DISCOM_STATIC r_discom_State_t loc_DiscomState[R_DISCOM_MACRO_NUM] =
{
    R_DISCOM_STATE_UNINITIALIZED,   /* DISCOM0 */
    R_DISCOM_STATE_UNINITIALIZED,
    R_DISCOM_STATE_UNINITIALIZED,
    R_DISCOM_STATE_UNINITIALIZED
};
#endif /* R_BSP_RGL_INIT_USE */

/***************************************************************************
  Section: Local Functions
*/

/***************************************************************************
  Section: Global Functions
*/

/***************************************************************************
  Group: DISCOM driver support functions

  Comments see: <r_discom_sys.h>
*/

/***************************************************************************
  Function: R_DISCOM_Sys_InitGlobal
*/
void R_DISCOM_Sys_InitGlobal(void)
{
    uint32_t    unit;
    
    for (unit = 0u; unit < R_DISCOM_MACRO_NUM; unit++)
    {
        loc_DiscomState[unit] = R_DISCOM_STATE_UNINITIALIZED;
    }
}

/***************************************************************************
  Function: R_DISCOM_Sys_Init

  See: <r_discom_sys.h> for details
*/

r_discom_Error_t R_DISCOM_Sys_Init(const uint32_t Unit) 
{
    return R_DISCOM_ERR_OK;
}

/***************************************************************************
  Function: R_DISCOM_Sys_DeInit

  See: <r_discom_sys.h> for details
*/

r_discom_Error_t R_DISCOM_Sys_DeInit(const uint32_t Unit) 
{
    return R_DISCOM_ERR_OK;
}

/***************************************************************************
  Function: R_DISCOM_Sys_InterruptEnable

  See: <r_discom_sys.h> for details
*/

void R_DISCOM_Sys_InterruptEnable(const uint32_t   Unit)
{
}

/***************************************************************************
  Function: R_DISCOM_Sys_InterruptDisable

  See: <r_discom_sys.h> for details
*/

void R_DISCOM_Sys_InterruptDisable(const uint32_t Unit)
{
}

/***************************************************************************
  Function: R_DISCOM_Sys_BaseAddr

  See: <r_discom_sys.h> for details
*/

uint32_t R_DISCOM_Sys_BaseAddrGet(const uint32_t Unit) 
{
    uint32_t base;
    r_dev_Device_t  device  =  R_DEV_GetDev();

    switch (Unit)
    {
    case R_DISCOM_UNIT0:
        base = R_DISCOM_BASE_ADDR0;
        break;
    case R_DISCOM_UNIT1:
        base = R_DISCOM_BASE_ADDR1;
        break;
    case R_DISCOM_UNIT2:
        /* RH850/D1M2x and RH850/D1M1A */
        if (((device >= R_DEV_R7F701408) && (device <= R_DEV_R7F701412)) ||
            (device == R_DEV_R7F701441))
        {
            base = R_DISCOM_BASE_ADDR2;
        }
        else
        {
            base = 0;
        }
        break;
    case R_DISCOM_UNIT3:
        /* RH850/D1M2x and RH850/D1M1A */
        if (((device >= R_DEV_R7F701408) && (device <= R_DEV_R7F701412)) ||
            (device == R_DEV_R7F701441))
        {
            base = R_DISCOM_BASE_ADDR3;
        }
        else
        {
            base = 0;
        }
        break;
    default:
        base = 0;
        break;
    }
    
    return base;
}

/***************************************************************************
  Function: R_DISCOM_Sys_StateSet

  See: <r_discom_sys.h> for details
*/
r_discom_Error_t R_DISCOM_Sys_StateSet(const uint32_t         Unit,
                                       const r_discom_State_t State)
{
    r_discom_Error_t ret = R_DISCOM_ERR_OK;

    if (Unit < R_DISCOM_MACRO_NUM)
    {
        loc_DiscomState[Unit] = State;
    }
    else
    {
        ret = R_DISCOM_ERR_RANGE_UNIT;
    }

    return ret;
}

/***************************************************************************
  Function: R_DISCOM_Sys_StateGet

  See: <r_discom_sys.h> for details
*/
r_discom_State_t R_DISCOM_Sys_StateGet(const uint32_t      Unit)
{
    r_discom_State_t status;

    if (Unit < R_DISCOM_MACRO_NUM)
    {
        status = loc_DiscomState[Unit];
    }
    else
    {
        status = R_DISCOM_STATE_UNINITIALIZED;
    }

    return status;
}

/***************************************************************************
  Group: DISCOM Macro Driver OS interface

  Comments see: <r_discom_sys.h>
*/

/***************************************************************************
  Function: R_DISCOM_Sys_Lock

  See: <r_discom_sys.h> for details
*/
r_discom_Error_t R_DISCOM_Sys_Lock(const uint32_t Unit)
{
    r_discom_Error_t  ret;
    
    if (Unit < R_DISCOM_MACRO_NUM)
    {
        /* Please implement the lock process by mutex or semaphore */
        /* if DISCOM API is called from multi-thread. */
        
        ret = R_DISCOM_ERR_OK;
    }
    else
    {
        ret = R_DISCOM_ERR_RANGE_UNIT;
    }
    return ret;
}

/***************************************************************************
  Function: R_DISCOM_Sys_Unlock

  See: <r_discom_sys.h> for details
*/
r_discom_Error_t R_DISCOM_Sys_Unlock(const uint32_t Unit)
{
    r_discom_Error_t  ret;
    
    if (Unit < R_DISCOM_MACRO_NUM)
    {
        /* Please implement the unlock process depending on R_DISCOM_Sys_Lock. */
        
        ret = R_DISCOM_ERR_OK;
    }
    else
    {
        ret = R_DISCOM_ERR_RANGE_UNIT;
    }
    return ret;
}
