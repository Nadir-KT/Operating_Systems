/*
****************************************************************************
PROJECT : VLIB
============================================================================ 
DESCRIPTION
Generic ECM driver
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
  Title: ECM Interrupt Functions

  Handle the ECM interrupts.
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_dev_api.h"
#include "r_dev_err_types.h"
#include "r_config_ecm.h"
#include "r_ecm_api.h"
#include "r_ecm_sys.h"
#include "r_ecm_main.h"

/*******************************************************************************
  Section: Local Defines
*/

#define LOC_REG0_FIRST          (0u)
#define LOC_REG1_FIRST          (32u)
#define LOC_REG1_LAST           ((uint32_t)R_DEV_ECMERR_LAST)


/*******************************************************************************
  Section: Global API Functions
  
  For comments see: <r_ecm_api.h>
*/

/*******************************************************************************
  Function: R_ECM_SetIsrCallback

  See: <r_ecm_api.h> 
*/

r_ecm_Error_t R_ECM_SetIsrCallback(uint32_t          Unit, 
                                   r_ecm_Interrupt_t Interrupt,     
                                   r_dev_EcmErrSel_t ErrorSource,
                                   void              (*Isr)(void)) 
{
    r_ecm_Error_t err; 
    
    if ((Unit >= R_ECM_MACRO_NUM) || (Interrupt >= R_ECM_INT_NUM) 
     || (ErrorSource >= R_DEV_ECMERR_LAST)) 
    {
        err = R_ECM_ERR_RANGE; 
    } 
    else
    {
        r_ecm_Dev[Unit].FeintCallback[ErrorSource] = Isr;
        err = R_ECM_ERR_OK;
    }

    return err;
}


/*******************************************************************************
  Function: R_ECM_FeIntIsr

  See: <r_ecm_api.h> 
*/

void R_ECM_FeIntIsr(uint32_t Unit)
{
    uint32_t          err_source;
    uint32_t          reg_mask;
    uint32_t          reg_err_status[2];
    uint32_t          reg_err_active[2]; 
    uint32_t          base;

    base = r_ecm_Dev[Unit].BaseAddr;
    
    /* Read status and mask bit of currently checked error source */
    reg_err_active[0] = R_ECM_READ_REG(32, base + R_ECM_BASE_OFFSET_COMMON + R_ECM_REG_MICFG0);
    reg_err_status[0] = R_ECM_READ_REG(32, base + R_ECM_BASE_OFFSET_MASTER + R_ECM_REG_MESSTR0);
    /* Delete status bit if FENMI is inactive */
    reg_err_status[0] &= reg_err_active[0]; 

    if (reg_err_status[0] != 0u)
    {
        /* Go through each error source and execute the interrupt handler if required */
        for (err_source = LOC_REG0_FIRST; err_source < LOC_REG1_FIRST; err_source++)
        {
            reg_mask = 1uL << (err_source & 0x1Fu);
            
            /* Check status bit of currently checked error source */
            if ((reg_err_status[0] & reg_mask) != 0u)
            {
                /* If there is an active error for the currently checked error source,  */
                /* execeute the interrupt handler */
                if (r_ecm_Dev[Unit].FeintCallback[err_source] != R_NULL)
                {
                    r_ecm_Dev[Unit].FeintCallback[err_source]();
                }
                
                (void)R_ECM_ClearErrorSourceStatus(Unit, (r_dev_EcmErrSel_t)err_source);
            }
        }
    }
    
    /* Read status and mask bit of currently checked error source */
    reg_err_active[1] = R_ECM_READ_REG(32, base + R_ECM_BASE_OFFSET_COMMON + R_ECM_REG_MICFG1);
    reg_err_status[1] = R_ECM_READ_REG(32, base + R_ECM_BASE_OFFSET_MASTER + R_ECM_REG_MESSTR1);
    /* Delete status bit if FENMI is inactive */
    reg_err_status[1] &= reg_err_active[1]; 
    
    if (reg_err_status[1] != 0u)
    {
        /* Go through each error source and execute the interrupt handler if required */
        for (err_source = LOC_REG1_FIRST; err_source < LOC_REG1_LAST; err_source++)
        {
            reg_mask = 1uL << (err_source & 0x1Fu);
        
            /* Check status bit of currently checked error source */
            if ((reg_err_status[1] & reg_mask) != 0u)
            {
                /* If there is an active error for the currently checked error source,  */
                /* execeute the interrupt handler */
                if (r_ecm_Dev[Unit].FeintCallback[err_source] != R_NULL)
                {
                    r_ecm_Dev[Unit].FeintCallback[err_source]();
                }
                
                (void)R_ECM_ClearErrorSourceStatus(Unit, (r_dev_EcmErrSel_t)err_source);
            }
        }
    }
}
