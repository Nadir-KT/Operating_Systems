/*
****************************************************************************
PROJECT : BSP - GFX - VI SAA71xx init
FILE    : $Id: r_bsp_gfx_vi_saa71_init.c 4988 2015-03-30 13:06:17Z golczewskim $
============================================================================
DESCRIPTION
BSP Implementation D1X MANGO Board
Init Video Input converter SAA71xx.
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

#include "r_typedefs.h"     /* Renesas basic types, e.g. uint32_t */
#ifdef USE_BSP_STDIO
#include "r_bsp_stdio_api.h"
#endif
#include "r_bsp_gfx_vi_saa71_init.h"
#include "saa71xx_api.h"

/*******************************************************************************
  Section: Local Defines
*/

/*******************************************************************************
  Section: Local Constants
*/

/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_BSP_GFX_VI_SAA71xx_Enable

  Init Video Input converter SAA71xx.

  Parameters:
  void

  Returns:
  void
*/
void R_BSP_GFX_VI_SAA71xx_Enable(void)
{
    /* Init Video Input converter SAA71xx. */
    /*  - That means ITU656 (BT656) data is provided from
          analog S-video / CVBS camera to MCU.
    
            Note: The format selection S-video or CVBS is
                  made by setting in R_SAA71xx_Sys_CvbsSel() function.
    */
    R_SAA71xx_Error_t err;
    #ifdef USE_BSP_STDIO
    R_BSP_STDIO_Printf("Init SAA71xx VI converter\n");
    #endif
    err = R_SAA71xx_Init(0);

    if (err != R_SAA71XX_ERR_OK)
    {
        /* SAA71xx VI converter could NOT be found via IIC
            Possible reason can be:
             o Jumper setting missing
        */
        #ifdef USE_BSP_STDIO
        R_BSP_STDIO_Printf("\nERROR: SAA71xx VI converter init failed!\n\n");
        #endif
    }
}

