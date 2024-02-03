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

#ifndef R_ECM_SYS_H_
#define R_ECM_SYS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Title: Generic ECM Support Functions

  The generic ECM driver uses these functions. They have
  to be implemented within the driver library for a concrete
  device.
*/


/*******************************************************************************
  Section: Global Defines
*/

/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_ECM_Sys_HardwareInit

  Setup the HW part for the generic ECM.
  
  Parameters:
  Unit       - Instance number
  
  Returns:
  see: <r_ecm_Error_t>
*/

r_ecm_Error_t R_ECM_Sys_HardwareInit(uint32_t Unit);


/*******************************************************************************
  Function: R_ECM_Sys_HardwareDeInit

  Deinit of the HW part for the generic ECM.
  
  Parameters:
  Unit       - Instance number
  
  Returns:
  see: <r_ecm_Error_t>
*/

r_ecm_Error_t R_ECM_Sys_HardwareDeInit(uint32_t Unit);


/*******************************************************************************
  Function: R_ECM_Sys_BaseAddr

  Returns the base address of dedicated macro unit.
  
  Parameters:
  Unit       - Instance number
  
  Returns:
  Macro base address
*/

uint32_t R_ECM_Sys_BaseAddr(uint32_t Unit);



#ifdef __cplusplus
}
#endif


#endif /* R_ECM_SYS_H_ */
