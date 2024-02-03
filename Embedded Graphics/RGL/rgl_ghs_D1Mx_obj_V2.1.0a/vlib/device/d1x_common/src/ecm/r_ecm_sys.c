/*
****************************************************************************
PROJECT : VLIB macro device level driver
============================================================================ 
DESCRIPTION
ECM support functions for RH850/D1x
============================================================================
                            C O P Y R I G H T                            
============================================================================
                           Copyright (c) 2014 - 2019
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
  Title: dev_ecm ECM device functions

  Implementation of the ECM support functions for the device.
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_ecm_api.h"
#include "r_ecm_sys.h"
#include "r_config_ecm.h"

/*******************************************************************************
  Section: Global Functions

*/

/*******************************************************************************
  Function: R_ECM_Sys_BaseAddr
  
  See: <r_ecm_sys.h> for details
*/

uint32_t R_ECM_Sys_BaseAddr(uint32_t Unit) 
{
    uint32_t  base_addr = 0u;
    
    if (Unit == R_ECM_UNIT0)
    {
        base_addr = R_ECM_BASE;
    }
    return base_addr;
}

/*******************************************************************************
  Function: R_ECM_Sys_HardwareInit
  
  See: <r_ecm_sys.h> for details
*/

r_ecm_Error_t R_ECM_Sys_HardwareInit(uint32_t Unit) 
{
    return R_ECM_ERR_OK;
}
/*******************************************************************************
  Function: R_ECM_Sys_HardwareDeInit
  
  See: <r_ecm_sys.h> for details
*/

r_ecm_Error_t R_ECM_Sys_HardwareDeInit(uint32_t Unit) 
{
    return R_ECM_ERR_OK;
}



