/*
****************************************************************************
PROJECT : VLIB Macro Driver  
============================================================================ 
DESCRIPTION
Driver for the ECM macro
============================================================================
                            C O P Y R I G H T                            
============================================================================
                           Copyright (c) 2012 - 2019
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
  Title: ECM Main Module

  Implementation of the API and other global functions.
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_dev_api.h"
#include "r_dev_err_types.h"

#include "r_config_ecm.h"
#include "r_ecm_api.h"
#include "r_ecm_main.h"
#include "r_ecm_sys.h"


/*******************************************************************************
  Section: Local Constants
*/

/*******************************************************************************
  Section: Global Variables
*/
r_ecm_Dev_t r_ecm_Dev[R_ECM_MACRO_NUM];

/*******************************************************************************
  Section: Local Functions
*/

/*******************************************************************************
  Function: loc_UpdateRegister

  There are several registers that all contain the same bitmasks.
  This is a univeral function to set the bit to <Enable> that corresponds to a certain error source.
  
  There are always two successive registers for the bitmasks.
  If the supplied <ErrSource> belongs to the second register, 
  the supplied <RegOffset> will be adjusted automatically.
  <ProtOffset> provides the offset address to the Protection Command 
  Register that is associated with the Configuration Register.

  Parameters:
  Unit          - Macro Instance number
  ErrSource     - Error source to be configured to the value of <enable>
  Enable        - Select if the Bit is set (1) or cleared (0)
  RegOffset     - Configuration Register Offset
  ProtOffset    - Protection Command Register Offset
  
  Returns:
  See <r_ecm_Error_t>
*/

static r_ecm_Error_t loc_UpdateRegister(uint32_t          Unit, 
                                 r_dev_EcmErrSel_t ErrSource, 
                                 uint32_t          Enable, 
                                 uint32_t          RegOffset, 
                                 uint32_t          ProtOffset);
static r_ecm_Error_t loc_UpdateRegister(uint32_t          Unit, 
                                 r_dev_EcmErrSel_t ErrSource, 
                                 uint32_t          Enable, 
                                 uint32_t          RegOffset, 
                                 uint32_t          ProtOffset)
{
    r_ecm_Error_t err = R_ECM_ERR_OK;
    uint32_t      base;
    uint32_t      reg_mask;
    uint32_t      reg_addr;
    uint32_t      prot_addr;
    uint32_t      reg_val;
    uint32_t      retry;
    uint32_t      prot_status = 0u;
    
    if (Unit >= R_ECM_MACRO_NUM) 
    {
        err = R_ECM_ERR_RANGE; 
    }
    else if (r_ecm_Dev[Unit].BaseAddr == 0u)
    {   
        /* please, no double-init*/
        err = R_ECM_ERR_NOTLOCKED; 
    }
    else
    {
        base = r_ecm_Dev[Unit].BaseAddr;
        reg_addr  = base + RegOffset;
        prot_addr = base + ProtOffset;
        
        /* There are always two configuration registers as we have more than 32 
           but less than 64 error sources and need one bit for each source 
           
           This function always expects to get the Register0 of the configuration
           to be changed. For error sources greater than 32, the register address
           is increased by 0x04. */
        if ((uint32_t)ErrSource >= 32u)
        {
            reg_addr += 4u;
        }

        reg_mask = 1uL << ((uint32_t)ErrSource & 0x1Fu);
        reg_val  = R_ECM_READ_REG(32, reg_addr);

        if (Enable == 0u)
        {
            /* Clear Masked Bit */
            reg_val &= (~reg_mask);
        }
        else
        {
            /* Set Masked Bit */
            reg_val |= reg_mask;
        }
        
        /* The write sequence for protected registers might be disrupted by interrupts, 
           that also write to the ECM. 
           Try several times to write the new value to the register, then fail. */
        retry  = 10u;
        while (retry > 0u) 
        {
            /* Write value */
            R_ECM_WRITE_PROT_REG(prot_addr, reg_addr, reg_val);
            
            /* Check for success and leave if result is positive */
            prot_status = R_ECM_READ_REG(8, base + R_ECM_BASE_OFFSET_COMMON + R_ECM_REG_PS);
            if (prot_status == 0u) /* 0:success 1: fail */
            {
                break;
            }
            
            /* Stop and report error if still unsuccessful after a certain number of retries. */
            retry--;
        }
    }

    return err;
}


/*******************************************************************************
  Section: Global API Functions
  
  See: <r_ecm_api.h>
*/

/*******************************************************************************
  Function: R_ECM_GetVersionStr

  See: <r_ecm_api.h> 
*/
const int8_t * R_ECM_GetVersionStr(void) 
{
    static const int8_t  loc_EcmDrvVer[] = R_VERSION(ECM, R_ECM_VERSION_HI, R_ECM_VERSION_LO);
    return (const int8_t *)loc_EcmDrvVer;
}

/*******************************************************************************
  Function: R_ECM_Init

  See: <r_ecm_api.h> 
*/

r_ecm_Error_t R_ECM_Init(uint32_t Unit) 
{
    uint32_t             err_source;
    r_ecm_Error_t        err;

    if (Unit >= R_ECM_MACRO_NUM) 
    {
        err = R_ECM_ERR_RANGE; 
    }
    else if (r_ecm_Dev[Unit].BaseAddr == 0u)
    {
        r_ecm_Dev[Unit].BaseAddr = R_ECM_Sys_BaseAddr(Unit);
        
        for (err_source = (uint32_t)R_DEV_ECMERR_FIRST; 
             err_source < (uint32_t)R_DEV_ECMERR_LAST; err_source++)
        {
            /* Init callback */
            r_ecm_Dev[Unit].FeintCallback[err_source] = R_NULL;
            
            /* Disable FEINT */
            (void)R_ECM_EnableFeint(Unit, (r_dev_EcmErrSel_t)err_source, 0u);
        }
        
        err = R_ECM_Sys_HardwareInit(Unit);
    }
    else 
    {   
        /* please, no double-init*/
        err = R_ECM_ERR_LOCKED; 
    }
    return err;
}


/*******************************************************************************
  Function: R_ECM_DeInit

  See: <r_ecm_api.h> 
*/

r_ecm_Error_t R_ECM_DeInit(uint32_t Unit)
{
    uint32_t             err_source;
    r_ecm_Error_t        err;

    if (Unit >= R_ECM_MACRO_NUM) 
    {
        err = R_ECM_ERR_RANGE; 
    }
    else
    {
        if (r_ecm_Dev[Unit].BaseAddr != 0u)
        {
            for (err_source = (uint32_t)R_DEV_ECMERR_FIRST; 
                 err_source < (uint32_t)R_DEV_ECMERR_LAST; err_source++)
            {
                /* Init callback */
                r_ecm_Dev[Unit].FeintCallback[err_source] = R_NULL;
                
                /* Disable FEINT */
                (void)R_ECM_EnableFeint(Unit, (r_dev_EcmErrSel_t)err_source, 0u);
            }

            err = R_ECM_Sys_HardwareDeInit(Unit);
            
            r_ecm_Dev[Unit].BaseAddr = 0;
        }
        else 
        {   
            /* nothing to do at this point of time  */
            err = R_ECM_ERR_OK; 
        }
    }
    return err;
}


/*******************************************************************************
  Function: R_ECM_ClearErrorSourceStatus

  See: <r_ecm_api.h> 
*/

r_ecm_Error_t R_ECM_ClearErrorSourceStatus(uint32_t Unit, r_dev_EcmErrSel_t ErrSource)
{
    r_ecm_Error_t err;
    uint32_t      reg_offset;
    uint32_t      port_offset;
    
    reg_offset  = R_ECM_BASE_OFFSET_COMMON + R_ECM_REG_ESSTC0;
    port_offset = R_ECM_BASE_OFFSET_COMMON + R_ECM_REG_PCMD1;
    
    err = loc_UpdateRegister(Unit, ErrSource, 1u, reg_offset, port_offset);
    
    return err;
}

/*******************************************************************************
  Function: R_ECM_EnableFeint

  See: <r_ecm_api.h> 
*/

r_ecm_Error_t R_ECM_EnableFeint(uint32_t Unit, r_dev_EcmErrSel_t ErrSource, uint32_t enable)
{
    r_ecm_Error_t err;
    uint32_t      reg_offset;
    uint32_t      port_offset;
    
    reg_offset  = R_ECM_BASE_OFFSET_COMMON + R_ECM_REG_MICFG0;
    port_offset = R_ECM_BASE_OFFSET_COMMON + R_ECM_REG_PCMD1;
    
    err = loc_UpdateRegister(Unit, ErrSource, enable, reg_offset, port_offset);
    
    return err;
}

/* END */
