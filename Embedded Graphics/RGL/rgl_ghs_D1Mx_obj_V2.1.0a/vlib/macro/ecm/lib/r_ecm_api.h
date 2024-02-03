/*
****************************************************************************
PROJECT : VLIB macro driver
============================================================================ 
DESCRIPTION
Generic part of the macro driver
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

#ifndef R_ECM_API_H_
#define R_ECM_API_H_

#include "r_dev_err_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
  Title: ECM Driver API 
  
  An application using ECM should only need to include this r_ecm_api.h
*/

/*******************************************************************************
  Section: Global Constants 
*/


/*******************************************************************************
  Constant:  R_ECM_VERSION_HI and R_ECM_VERSION_LO

  Driver version information  
*/

#define R_ECM_VERSION_HI 0
#define R_ECM_VERSION_LO 1

/*******************************************************************************
  Section: Global Types
*/

/*******************************************************************************
  Enum: r_ecm_Error_t

  ECM macro driver error code.
  
  If an error occures these enums give information about the
  reason.

  Values:
  R_ECM_ERR_OK           - No error
  R_ECM_ERR_NG           - Unspecified error
  R_ECM_ERR_RANGE        - Parameter error.
  R_ECM_ERR_LOCKED       - ECM driver is already initialized.
  R_ECM_ERR_NOTLOCKED    - ECM driver is not initialized.
*/

typedef enum
{
    R_ECM_ERR_OK         = 0,
    R_ECM_ERR_NG         = 1,
    R_ECM_ERR_RANGE      = 2,
    R_ECM_ERR_LOCKED     = 3,
    R_ECM_ERR_NOTLOCKED  = 4
} r_ecm_Error_t;

/*******************************************************************************
  Enum: r_ecm_Interrupt_t

  Ecm macro interrupt type. 
  
  List of all available interrupts for this macro.

  Values:
  R_ECM_INT_FEINT      - Fatal Exception Maskable-Interrupt
  R_ECM_INT_NUM        - Delimiter 
*/

typedef enum
{
    R_ECM_INT_FEINT,
    R_ECM_INT_NUM
} r_ecm_Interrupt_t;


/*******************************************************************************
  Section: Global API Functions
*/

/*******************************************************************************
  Function: R_ECM_GetVersionStr

  Get the driver version number in human readable form (string). 

  Parameters:
  void
  
  Returns:
  ReceiveString     - Pointer to receive string destination
  
*/

const int8_t * R_ECM_GetVersionStr(void); 

/*******************************************************************************
  Function: R_ECM_Init

  Driver init function.

  Parameters:
  Unit          - Macro instance number
 
  Returns:
  see: <r_ecm_Error_t>
*/

 r_ecm_Error_t R_ECM_Init(uint32_t Unit);


/*******************************************************************************
  Function: R_ECM_DeInit

  Driver deinit function.

  Parameters:
  Unit       - Macro Instance number

  Returns:
  see: <r_ecm_Error_t>
*/

 r_ecm_Error_t R_ECM_DeInit(uint32_t Unit);


/*******************************************************************************
  Group: ECM Error and ISR Functions
*/

/*******************************************************************************
  Function: R_ECM_SetErrorCallback

  Set a user function as error handler.
  
  When the driver detects any error, it will call this function.

  Parameters:
  ErrorCallback - Pointer to a user function
  
  Returns:
  void
*/

void R_ECM_SetErrorCallback(void (*ErrorCallback )(uint32_t Unit, uint32_t Error));


/*******************************************************************************
  Function: R_ECM_SetIsrCallback

  Assign a user callback for the given interrupt of a unit. 
  For each available error sources can be assigned as maskable-interrupt callback.
  
  The function will be called after the driver ISR has processed the interrupt.

  Parameters:
  Unit          - Instance number
  Interrupt     - What interrupt,  see: <r_ecm_Interrupt_t>
  ErrorSource   - What error source,  see: <r_dev_EcmErrSel_t>
  Isr           - Isr function pointer
  
  Returns:
  see: <r_ecm_Error_t>
*/

r_ecm_Error_t R_ECM_SetIsrCallback(uint32_t          Unit, 
                                   r_ecm_Interrupt_t Interrupt, 
                                   r_dev_EcmErrSel_t ErrorSource, 
                                   void (*Isr)(void));

/*******************************************************************************
  Function: R_ECM_FeIntIsr

  Driver Fatal-Exception Level Maskable-Interrupt interrupt service routine. 
  
  This function shall be called by device interrupt handler.

  Parameters:
  Unit       - Instance number
  
  Returns:
  void
*/

void R_ECM_FeIntIsr(uint32_t Unit);


/*******************************************************************************
  Function: R_ECM_ClearErrorSourceStatus

  Clear the error source status for the requested error source.
  Resets the error source status to normal.  Make sure, the error source is cleared.
  If the error input of this signal still alters an error, 
  the status will immediately be active again.
  
  Parameters:
  Unit              - Instance number
  ErrSource         - Error Source whose configuration should be written.
  
  Returns:
  see: <r_ecm_Error_t>
*/

r_ecm_Error_t R_ECM_ClearErrorSourceStatus(uint32_t Unit, r_dev_EcmErrSel_t ErrSource);

/*******************************************************************************
  Function: R_ECM_EnableFeint

  Enable or Disable the FE-level maksable interrupt generation for the requested error source.
  If enabled, the requested error source will be able cause an FEINT.
  
  Parameters:
  Unit              - Instance number
  ErrSource         - Error Source whose configuration should be written.
  Enable            - Set (1) to enable or (0) to disable the configuration for this error source.
  
  Returns:
  see: <r_ecm_Error_t>
*/

r_ecm_Error_t R_ECM_EnableFeint(uint32_t Unit, r_dev_EcmErrSel_t ErrSource, uint32_t enable);



#ifdef __cplusplus
}
#endif

#endif /* ECM_API_H  */
