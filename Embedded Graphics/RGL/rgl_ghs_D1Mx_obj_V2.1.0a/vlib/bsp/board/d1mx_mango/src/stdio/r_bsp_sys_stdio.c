/*
****************************************************************************
PROJECT : BSP STDIO
FILE    : $Id: r_bsp_sys_stdio.c 15313 2017-11-30 13:07:42Z florian.zimmermann $
============================================================================
DESCRIPTION
BSP Implementation D1X MANGO Board support sys functions - STDIO
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2014
                                  by 
                    Renesas Electronics (Europe) GmbH. 
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
Purpose: only for testing

DISCLAIMER                                                                   
This software is supplied by Renesas Electronics Corporation and is only     
intended for use with Renesas products. No other uses are authorized. This   
software is owned by Renesas Electronics Corporation and is protected under  
all applicable laws, including copyright laws.                               
THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING  
THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT      
LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE   
AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.          
TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS       
ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE  
FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR   
ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE  
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.                             
Renesas reserves the right, without notice, to make changes to this software 
and to discontinue the availability of this software. By using this software,
you agree to the additional terms and conditions found by accessing the      
following link:                                                              
http://www.renesas.com/disclaimer *                                          
Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.     

****************************************************************************

*/

/*******************************************************************************
  Section: Includes
*/
#include "r_typedefs.h"
#include "r_bsp_stdio_api.h"
#include "r_bsp_stdio_sys.h"

#ifndef USE_RLIN3
void R_BSP_SYS_STDIO_SetUart(uint32_t Unit) 
{
    Unit = Unit; 
}

/*******************************************************************************
  Function: R_BSP_SYS_STDIO_Init
*/
 
void R_BSP_SYS_STDIO_Init(void) 
{
    
}
 
/*******************************************************************************
  Function: R_BSP_SYS_STDIO_DeInit
*/
 
void R_BSP_SYS_STDIO_DeInit(void) 
{

}
 
/*******************************************************************************
  Function: R_BSP_SYS_STDIO_Write
*/

void R_BSP_SYS_STDIO_Write(uint8_t * Buffer, uint32_t Len)
{
    Buffer  = Buffer; 
    Len     = Len; 
}


/*******************************************************************************
  Function: R_BSP_SYS_STDIO_Read
*/

uint32_t R_BSP_SYS_STDIO_Read(uint8_t * String, uint32_t Max)
{
    uint32_t number = 0u;
    String = String;      
    return number;
}

/*******************************************************************************
  Function: R_BSP_SYS_STDIO_GetActCh
  
*/

uint8_t R_BSP_SYS_STDIO_GetActCh(void)
{
    /* When UART is not used, only debugger printf is possible */
    return 0xFF; 
}

#endif /* not USE_RLIN3 */

/*******************************************************************************
  Function: R_BSP_SYS_STDIO_AnWrite

    see <r_bsp_stdio_sys.h> for details
*/

r_Error_t R_BSP_SYS_STDIO_AnWrite(char *String, uint8_t Line) 
{
    /* Print to AlphaNumeric Display needs to be implemented here.
       Currently not implemented. By return of R_NG, the print will
       be relocated to R_BSP_STDIO_Printf (part of STDIO BSP)
       -> that means to the UART. */
    return R_NG;
}
