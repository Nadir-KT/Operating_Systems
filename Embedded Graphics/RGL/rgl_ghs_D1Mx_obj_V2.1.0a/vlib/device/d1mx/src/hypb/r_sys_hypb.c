/*
****************************************************************************
PROJECT : HYPB driver
FILE    : $Id: r_sys_hypb.c 13439 2017-05-10 08:43:53Z shinya.tomari $
============================================================================
DESCRIPTION
HYPB support functions for d1x
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2016
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
  Title: HYPB Driver support functions

  Implementation of the HYPB support functions for the D1x device.

*/

/***************************************************************************
  Section: Includes
*/
#include "r_typedefs.h"
#include "r_hypb_api.h"
#include "r_hypb_sys.h"
#include "r_config_hypb.h"
#include "r_dev_api.h"


/***************************************************************************
  Section: Local Defines
*/

/***************************************************************************
  Constant: LOC_REAC_CFG_CHECK_~

  Definitions to check the reading failure of the configuration register.
  
  LOC_READ_CFG_CHECK_RAM   - Check value of Hyper RAM.
  LOC_READ_CFG_CHECK_FLASH - Check value of Hyper Flash.
*/
#define LOC_READ_CFG_CHECK_RAM      (0x8F08uL)
#define LOC_READ_CFG_CHECK_FLASH    (0x8008uL)

/***************************************************************************
  Constant: LOC_WRITE_CFG_DEFAULT_~

  Definitions for configuration register default value.
  
  LOC_WRITE_CFG_DEFAULT_RAM   - Default value of Hyper RAM.
  LOC_WRITE_CFG_DEFAULT_FLASH - Default value of Hyper Flash.
*/
#define LOC_WRITE_CFG_DEFAULT_RAM   (0x8F1FuL)
#define LOC_WRITE_CFG_DEFAULT_FLASH (0x8EBBuL)

/***************************************************************************
  Constant: LOC_LATENCY_CODE_~

  Definitions for latency code field.
  
  LOC_LATENCY_CODE_MASK  - Mask of latency code field.
  LOC_LATENCY_CODE_SHIFT - Bit shift for latency code field.
*/
#define LOC_LATENCY_CODE_MASK       (0x00F0uL)
#define LOC_LATENCY_CODE_SHIFT      (4uL)

/***************************************************************************
  Constant: LOC_PARAM_SEC_~

  Definitions for parameter sector field.
  
  LOC_PARAM_SEC_MASK   - Mask of parameter sector field.
  LOC_PARAM_SEC_TOP    - Parameter sector value of Top.
  LOC_PARAM_SEC_BOTTOM - Parameter sector value of Bottom.
  LOC_PARAM_SEC_UNUSED - Parameter sector value of Unused.
  
*/
#define LOC_PARAM_SEC_MASK          (0x0300uL)
#define LOC_PARAM_SEC_TOP           (0x0000uL)
#define LOC_PARAM_SEC_BOTTOM        (0x0100uL)
#define LOC_PARAM_SEC_UNUSED        (0x0300uL)

/***************************************************************************
  Constant: LOC_STATUS_~

  Definitions for status register.
  
  LOC_STATUS_DEVICE_READY  - Bit of device ready.
  LOC_STATUS_ERASE_ERROR   - Bit of erase error.
  LOC_STATUS_PROGRAM_ERROR - Bit of program error.
  LOC_STATUS_LOCK_ERR      - Bit of lock error.
  
*/
#define LOC_STATUS_DEVICE_READY     (0x0080uL)
#define LOC_STATUS_ERASE_ERROR      (0x0020uL)
#define LOC_STATUS_PROGRAM_ERROR    (0x0010uL)
#define LOC_STATUS_LOCK_ERR         (0x0002uL)

/***************************************************************************
  Section: HYPB driver support functions

  Comments see: <r_hypb_sys.h>
*/

/***************************************************************************
  Function: R_HYPB_Sys_Init

  see: <r_hypb_sys.h>
*/
r_hypb_Error_t R_HYPB_Sys_Init(const uint32_t Unit)
{
    r_hypb_Error_t      sys_err = R_HYPB_ERR_OK;
    r_dev_Device_t      device;
    
    /* Check device version */
    device = R_DEV_GetDev();
    if ((device != R_DEV_R7F701441) /* RH850/D1M1A */
     && (device != R_DEV_R7F701442) /* RH850/D1M1-V2 */
     && (device != R_DEV_R7F701418))
    {
        sys_err = R_HYPB_ERR_DEVICE;
    }
    
    return sys_err;
}


/***************************************************************************
  Function: R_HYPB_Sys_DeInit

  see: <r_hypb_sys.h>
*/

r_hypb_Error_t R_HYPB_Sys_DeInit(const uint32_t Unit)
{
    return R_HYPB_ERR_OK;
}

/***************************************************************************
  Function: R_HYPB_Sys_BaseAddrGet

  see: <r_hypb_sys.h>
*/
uint32_t R_HYPB_Sys_BaseAddrGet(const uint32_t Unit)
{
    uint32_t addr = 0;

    if (Unit == 0)
    {
        addr = R_HYPB_BASE_ADD;
    }
    return addr;
}

/***************************************************************************
  Function: R_HYPB_Sys_ConfigRegSet

  see: <r_hypb_sys.h>
*/
uint16_t R_HYPB_Sys_ConfigRegSet(const uint32_t                Unit, 
                                 const r_hypb_Config_t * const Config, 
                                 const uint16_t                RegVal)
{
    /* Sample for IS26KL512S and IS66WVH16M8BLL */
    
    uint16_t    out_reg_val;
    uint16_t    latency_code;
    uint16_t    param_sec;
    
    out_reg_val = RegVal;

    if (Config != R_NULL)
    {
        if (Config->DeviceType == R_HYPB_DEVICE_RAM)
        {
            /* Check miss read */
            if ((RegVal & LOC_READ_CFG_CHECK_RAM) != LOC_READ_CFG_CHECK_RAM)
            {
                out_reg_val = LOC_WRITE_CFG_DEFAULT_RAM;
            }
        }
        else
        {
            /* Check miss read */
            if ((RegVal & LOC_READ_CFG_CHECK_FLASH) != LOC_READ_CFG_CHECK_FLASH)
            {
                out_reg_val = LOC_WRITE_CFG_DEFAULT_FLASH;
            }
        }

        /* Set Latency Code */
        if (Config->Command != R_NULL)
        {
            latency_code = (uint16_t)((uint16_t)Config->Command->Latency.Code << LOC_LATENCY_CODE_SHIFT) 
                           & LOC_LATENCY_CODE_MASK;
            out_reg_val = (out_reg_val & ~LOC_LATENCY_CODE_MASK) | latency_code;
        }
        
        /* Set parameter sector */
        if (Config->DeviceType == R_HYPB_DEVICE_FLASH)
        {
            if ((Config->ParamSec.Size != 0) && (Config->ParamSec.Num != 0))
            {
                if (Config->ParamSec.Pos == R_HYPB_PARAM_SEC_TOP)
                {
                    param_sec = LOC_PARAM_SEC_TOP;
                }
                else
                {
                    param_sec = LOC_PARAM_SEC_BOTTOM;
                }
            }
            else
            {
                param_sec = LOC_PARAM_SEC_UNUSED;
            }
            out_reg_val = (out_reg_val & ~LOC_PARAM_SEC_MASK) | param_sec;
        }
    }
    return out_reg_val;
}


/***************************************************************************
  Function: R_HYPB_Sys_StatusRegCheck

  see: <r_hypb_sys.h>
*/
r_hypb_Error_t R_HYPB_Sys_StatusRegCheck(const uint32_t         Unit,
                                         const r_hypb_CmdType_t CmdType,
                                         const uint16_t         RegVal,
                                         const uint32_t         Count)
{
    /* Sample for IS26KL512S */
    
    r_hypb_Error_t sys_err = R_HYPB_ERR_OK;
    
    /* Check time-out */
    if (Count >= R_HYPB_STATUS_TIMEOUT)
    {
        sys_err = R_HYPB_ERR_TIMEOUT;
    }
    else
    {
        /* 0xFFFF is sometimes returned at first if Read status command is executed during programing. */
        /* Check device ready  0:Busy 1:Ready */
        if ((RegVal == 0xFFFF) || ((RegVal & LOC_STATUS_DEVICE_READY) == 0))
        {
            sys_err = R_HYPB_ERR_NOT_ACCEPTABLE;
        }
        else
        {
            if (CmdType == R_HYPB_CMD_TYPE_ERASE)
            {
                /* Check Error  0:Normal 1:Error */
                if ((RegVal & LOC_STATUS_ERASE_ERROR) != 0)
                {
                    /* Check Lock status 0:Normal 1:Lock Error */
                    if ((RegVal & LOC_STATUS_LOCK_ERR) != 0)
                    {
                        sys_err = R_HYPB_ERR_PROTECTED;
                    }
                    else
                    {
                        sys_err = R_HYPB_ERR_ABORTED;
                    }
                }
            }
            else if ((CmdType == R_HYPB_CMD_TYPE_WRITE) ||
                     (CmdType == R_HYPB_CMD_TYPE_WRITE_BUF_FLASH))
            {
                /* Check Error  0:Normal 1:Error */
                if ((RegVal & LOC_STATUS_PROGRAM_ERROR) != 0)
                {
                    /* Check Lock status  0:Normal 1:Lock Error */
                    if ((RegVal & LOC_STATUS_LOCK_ERR) != 0)
                    {
                        sys_err = R_HYPB_ERR_PROTECTED;
                    }
                    else
                    {
                        sys_err = R_HYPB_ERR_ABORTED;
                    }
                }
            }
            else
            {
                /* Do nothing */
            }
        }
    }
    return sys_err;
}


/***************************************************************************
  Function: R_HYPB_Sys_SyncPeripheral

  see: <r_hypb_sys.h>
*/
void R_HYPB_Sys_SyncPeripheral(void)
{
    R_DEV_SyncP();
}

/***************************************************************************
  Function: R_HYPB_Sys_Lock

  see: <r_hypb_sys.h>
*/
r_hypb_Error_t R_HYPB_Sys_Lock(const uint32_t Unit)
{
    return R_HYPB_ERR_OK;
}

/***************************************************************************
  Function: R_HYPB_Sys_Unlock

  see: <r_hypb_sys.h>
*/
r_hypb_Error_t R_HYPB_Sys_Unlock(const uint32_t Unit)
{
    return R_HYPB_ERR_OK;
}

/***************************************************************************
  Function: R_HYPB_Sys_Relax

  see: <r_hypb_sys.h>
*/
r_hypb_Error_t R_HYPB_Sys_Relax(const uint32_t Unit)
{
    return R_HYPB_ERR_OK;
}


