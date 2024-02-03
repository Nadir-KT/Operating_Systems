/*
****************************************************************************
PROJECT : OCTA driver
FILE    : $Id: r_sys_octa.c 15447 2017-12-20 00:25:00Z shinobu.uehara $
============================================================================
DESCRIPTION
OCTA support functions for d1x
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2021
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
  Title: OCTA Driver support functions

  Implementation of the OCTA support functions for the D1x device.

*/

/***************************************************************************
  Section: Includes
*/
#include "r_typedefs.h"
#include "r_octa_api.h"
#include "r_octa_sys.h"
#include "r_config_octa.h"
#include "r_dev_api.h"


/***************************************************************************
  Section: Local Defines
*/

/***************************************************************************
  Constant: LOC_EXTERNAL_BASE_~

  Definitions to the base address of external address space.
  
  LOC_EXTERNAL_BASE_ADDR0   - Base address for Channel0.
  LOC_EXTERNAL_BASE_ADDR1   - Base address for Channel1.
*/
#define LOC_EXTERNAL_BASE_ADDR0      (0x60000000uL)
#define LOC_EXTERNAL_BASE_ADDR1      (0x70000000uL)

/***************************************************************************
  Constant: LOC_DEFAULT_DL_~

  Definitions to the default Dummy Length.
  
  LOC_DEFAULT_DL_FLASH   - Default Dummy Length for Flash memory.
  LOC_DEFAULT_DL_RAM     - Default Dummy Length for RAM.
*/
#define LOC_DEFAULT_DL_FLASH    (20uL)
#define LOC_DEFAULT_DL_RAM      (5uL)

/***************************************************************************
  Constant: LOC_BANK_SIZE

  Definitions to the bank size.
  
  LOC_BANK_SIZE   - bank size for Flash memory.
*/
#define LOC_BANK_SIZE    (0x1000000uL)

/*******************************************************************************
  Section: Local functions
*/
static uint32_t loc_OCTA_CmdBitPosShiftGet(const uint16_t    Pos);

/***************************************************************************
  Function: loc_OCTA_CmdBitPosShiftGet

  Description:
  This function gives back the shift value of the specified bit position.
  The shift value gives priority to the lowest bit position.
  
  Parameter:
    pos      - Bit position.

  Returns:
    shift value.
*/
static uint32_t loc_OCTA_CmdBitPosShiftGet(const uint16_t    Pos)
{
    uint32_t    shift = 0u;
    uint32_t    val;

    for (val = Pos; val > 0u; val >>= 1u)
    {
        if ((val & 1uL) == 0u)
        {
            shift++;
        }
    }
    return shift;
}

/***************************************************************************
  Section: OCTA driver support functions

  Comments see: <r_octa_sys.h>
*/

/***************************************************************************
  Function: R_OCTA_Sys_Init

  see: <r_octa_sys.h>
*/
r_octa_Error_t R_OCTA_Sys_Init(const uint32_t Unit)
{
    r_octa_Error_t      sys_err = R_OCTA_ERR_OK;
    r_dev_Device_t      device;
    
    /* Check device version */
    device = R_DEV_GetDev();
    if ((device != R_DEV_R7F701441) /* RH850/D1M1A */
     && (device != R_DEV_R7F701442) /* RH850/D1M1-V2 */
     && (device != R_DEV_R7F701418))
    {
        sys_err = R_OCTA_ERR_DEVICE;
    }
    
    return sys_err;
}


/***************************************************************************
  Function: R_OCTA_Sys_DeInit

  see: <r_octa_sys.h>
*/

r_octa_Error_t R_OCTA_Sys_DeInit(const uint32_t Unit)
{
    return R_OCTA_ERR_OK;
}

/***************************************************************************
  Function: R_OCTA_Sys_BaseAddrGet

  see: <r_octa_sys.h>
*/
uint32_t R_OCTA_Sys_BaseAddrGet(const uint32_t Unit)
{
    uint32_t addr = 0u;

    if (Unit == 0u)
    {
        addr = R_OCTA_BASE_ADD;
    }
    return addr;
}

/***************************************************************************
  Function: R_OCTA_Sys_ExternalAddrSpaceBaseAddrGet

  see: <r_octa_sys.h>
*/
uint32_t R_OCTA_Sys_ExternalAddrSpaceBaseAddrGet(const uint32_t Unit, const uint32_t Channel)
{
    uint32_t addr = 0u;

    if (Unit == 0u)
    {
        if (Channel == 0u)
        {
            addr = LOC_EXTERNAL_BASE_ADDR0;
        }
        else
        {
            addr = LOC_EXTERNAL_BASE_ADDR1;
        }
    }
    return addr;
}

/***************************************************************************
  Function: R_OCTA_Sys_DefaultDummyLengthGet

  see: <r_octa_sys.h>
*/
uint32_t R_OCTA_Sys_DefaultDummyLengthGet(const uint32_t Unit, const uint32_t Channel,
                                          const uint8_t DeviceType)
{
    uint32_t df_dummylength;

    if (DeviceType == 0u)
    {
        df_dummylength = LOC_DEFAULT_DL_FLASH; /* OCTA FLASH */
    }
    else
    {
        df_dummylength = LOC_DEFAULT_DL_RAM; /* OCTA RAM */
    }

    return df_dummylength;
}

/***************************************************************************
  Function: R_OCTA_Sys_BankSizeGet

  see: <r_octa_sys.h>
*/
uint32_t R_OCTA_Sys_BankSizeGet(void)
{
    return LOC_BANK_SIZE;
}

/***************************************************************************
  Function: R_OCTA_Sys_DummyDataRead

  see: <r_octa_sys.h>
*/
void R_OCTA_Sys_DummyDataRead(const uint32_t    Base,
                              const uint32_t    Addr,
                              const uint32_t    BankSize)
{
    uint32_t        dread = 0u;
    uint32_t        bank_adr;

    /* Synchronize the Write process */
    /* Dummy Read */
    bank_adr = (Addr & ~(BankSize - 1u));
    if (bank_adr == 0u)
    {
        dread = BankSize;
    }

    R_DEV_READ_REG(32, (Base + dread));
}

/***************************************************************************
  Function: R_OCTA_Sys_ConfigRegSet

  see: <r_octa_sys.h>
*/
uint16_t R_OCTA_Sys_ConfigRegSet(const uint32_t                  Unit,
                                 const uint32_t                  Channel,
                                 const r_octa_Config_t * const   Config,
                                 const uint16_t                  RegVal,
                                 const r_octa_StateType_t        Type)
{
    r_octa_Command_t*           cmd;
    uint32_t                    shift;
    uint16_t                    reg_val;

    cmd = Config->Command;
    reg_val = RegVal;

    if (Type == R_OCTA_TYPE_PCM)
    {
        /* PCM */
        if (Config->PreCycle == 0u)
        {
            reg_val &= (uint16_t)~cmd->PreCycle.BitPosition;
        }
        else
        {
            reg_val |= cmd->PreCycle.BitPosition;
        }
    }
    else if (Type == R_OCTA_TYPE_DTM)
    {
        shift = loc_OCTA_CmdBitPosShiftGet(cmd->FlashTransferType.BitPosition);
        /* DTM */
        reg_val &= (uint16_t)~cmd->FlashTransferType.BitPosition;
        reg_val |= (uint16_t)((uint32_t)Config->DataTransferMode << shift);
    }
    else if (Type == R_OCTA_TYPE_DOS)
    {
        /* DOS */
        if (Config->DataTransferMode == R_OCTA_MODE_OPI)
        {
            reg_val |= cmd->DOS.BitPosition;
        }
        else
        {
            reg_val &= (uint16_t)~cmd->DOS.BitPosition;
        }
    }
    else if (Type == R_OCTA_TYPE_DL)
    {
        /* DL */
        reg_val &= (uint16_t)~cmd->DummyCycle.BitMask;
        reg_val |= cmd->DummyCycle.BitSet;
    }
    else
    {
        /* check end */
    }

    return reg_val;
}

/***************************************************************************
  Function: R_OCTA_Sys_StatusRegCheck

  see: <r_octa_sys.h>
*/
r_octa_Error_t R_OCTA_Sys_StatusRegCheck(const uint32_t           Unit, 
                                         const uint32_t           Channel,
                                         const uint16_t           RegPos,
                                         const uint16_t           RegVal,
                                         const r_octa_StateType_t Type,
                                         const uint32_t           Count)
{
    r_octa_Error_t      sys_err = R_OCTA_ERR_OK;
    
    /* Check Timeout */
    if (Count >= R_OCTA_STATUS_TIMEOUT)
    {
        sys_err = R_OCTA_ERR_TIMEOUT;
    }
    else
    {
        if (Type == R_OCTA_TYPE_WEL)
        {
            /** Read Status Register (repeat until SR[1]=1) **/
            if ((RegVal & RegPos) == 0u)
            {
                sys_err = R_OCTA_ERR_NOT_ACCEPTABLE;
            }
            else
            {
                sys_err = R_OCTA_ERR_OK;
            }
        }
        else if (Type == R_OCTA_TYPE_WIP)
        {
            /** Read Status Register (repeat until SR[0]=0) **/
            if ((RegVal & RegPos) != 0u)
            {
                sys_err = R_OCTA_ERR_NOT_ACCEPTABLE;
            }
            else
            {
                sys_err = R_OCTA_ERR_OK;
            }
        }
        else
        {
            /* Nothing */
        }
    }
    return sys_err;
}

/***************************************************************************
  Function: R_OCTA_Sys_StatusRegSet

  see: <r_octa_sys.h>
*/
uint16_t R_OCTA_Sys_StatusRegSet(const uint32_t Unit,
                                 const uint32_t Channel,
                                 const uint16_t RegPos,
                                 const uint16_t RegVal,
                                 const uint8_t  Mode)
{
    uint16_t                    reg_val;

    reg_val = RegVal;

    /* Set Block protection */
    if (Mode == R_OCTA_MODE_UNPROTECT)
    {
        reg_val &= (uint16_t)~RegPos;
    }
    else
    {
        reg_val |= RegPos;
    } 

    return reg_val;
}

/***************************************************************************
  Function: R_OCTA_Sys_SyncPeripheral

  see: <r_octa_sys.h>
*/
void R_OCTA_Sys_SyncPeripheral(void)
{
    R_DEV_SyncP();
}

/***************************************************************************
  Function: R_OCTA_Sys_Lock

  see: <r_octa_sys.h>
*/
r_octa_Error_t R_OCTA_Sys_Lock(const uint32_t Unit)
{
    return R_OCTA_ERR_OK;
}

/***************************************************************************
  Function: R_OCTA_Sys_Unlock

  see: <r_octa_sys.h>
*/
r_octa_Error_t R_OCTA_Sys_Unlock(const uint32_t Unit)
{
    return R_OCTA_ERR_OK;
}

/***************************************************************************
  Function: R_OCTA_Sys_Relax

  see: <r_octa_sys.h>
*/
r_octa_Error_t R_OCTA_Sys_Relax(const uint32_t Unit)
{
    return R_OCTA_ERR_OK;
}


