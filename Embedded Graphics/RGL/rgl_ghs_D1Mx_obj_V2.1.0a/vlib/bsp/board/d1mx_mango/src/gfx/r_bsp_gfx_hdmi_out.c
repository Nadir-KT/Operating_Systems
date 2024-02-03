/*
****************************************************************************
PROJECT : BSP - GFX - HDMI converter
FILE    : $Id: r_bsp_gfx_hdmi_out.c 4988 2015-03-30 13:06:17Z golczewskim $
============================================================================
DESCRIPTION
BSP Implementation - D1M2 MANGO Board
Init the HDMI converter
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
#include "r_dev_api.h"
#include "r_gpio_api.h"
#include "r_bsp_gfx_hdmi_out.h" /* init HDMI output converters */


/*******************************************************************************
  Section: Local Defines
*/

/*  HDMI signals controlled by GPIO
    
    BSEL common of HDMI0 and HDMI1 is
        P2_0 (in case Jumper JP104 set)
        or P3_0 (in case Jumper JP105 set)
*/

#define HDMI_GPIO_BSEL0_PORT    2
#define HDMI_GPIO_BSEL0_PIN     0

#define HDMI_GPIO_BSEL1_PORT    3
#define HDMI_GPIO_BSEL1_PIN     0


/*******************************************************************************
  Section: Local Constants
*/

static const r_dev_PinConfig_t loc_HDMIPinConfig[] = 
/*  Port Pin Func      Dir      Feedback     Pull        OpenDrain   HiCurr   HiSpeed  InputType */
{
    {HDMI_GPIO_BSEL0_PORT, HDMI_GPIO_BSEL0_PIN, 0, R_DEV_PIN_OUT, 0, R_DEV_PIN_PULLNO, 0, 0, 0, R_DEV_PIN_CMOS1},  
    {HDMI_GPIO_BSEL1_PORT, HDMI_GPIO_BSEL1_PIN, 0, R_DEV_PIN_OUT, 0, R_DEV_PIN_PULLNO, 0, 0, 0, R_DEV_PIN_CMOS1},  
    /* delimiter - do not remove */
    {0, R_DEV_PIN_LAST,0,R_DEV_PIN_OUT,0,R_DEV_PIN_PULLNO,     0,      0,         0,    R_DEV_PIN_CMOS1}  
};


/*******************************************************************************
  Section: Local Functions
*/

/*******************************************************************************
  Function: R_BSP_GFX_HDMI_Init
  Init the HDMI converter

  Parameters:
  void

  Returns:
  void
*/
void R_BSP_GFX_HDMI_Init(void)
{
    /* Init 24bit parallel Video to HDMI/DVI converter; TFP410PAP
        Use configuration by hardware setup (by fixed pin levels of TFP410PAP).

    Set PDZ = high and ISEL/RSTZ = low
        This means, that chip configuration is specified
        by the configuration pins
        (BSEL = high, DSEL = X, EDGE = low, VREF = high(DVDD))
        and state pins (PDZ = high, DKEN = low).
        -> Config above means 24bit single-ended parallel video w/ falling clock edge.

        Note: I2C configuration is not performed. It is only required in case
                DE sync signal shall be generated by the TFP410PAP itself
                based on provided H-/V-Sync signals. But that's not the case.
                

        On D1M2 Mango Board there are 2 units of HDMI output converters.
            1) U32 used for Video Output 0
            2) U24 used for Video Output 1
                Pin states as shown above for this 2 HDMI output converters
                are given by default Pull-Up / -Downs with exception of
                a) BSEL/SCL signal and b) DSEL/SDA signal.
                As signal b) DSEL is don't care (X),
                the init sequence just need to ensure to set signal a) BSEL = high.
     */
    
    /* Assign Pins */
    /*  set signal HDMI BSEL = 1 (option 0: case Jumper JP104 is set) */ 
    /*  set signal HDMI BSEL = 1 (option 1: case Jumper JP105 is set) */ 
    /*   (In case Jumper JP104 and JP105 are not set,
          the pull-up resistor R63 will generate BSEL = 1 in any case.) */
    R_DEV_PinInit(loc_HDMIPinConfig);
    R_GPIO_WritePin(HDMI_GPIO_BSEL0_PORT, HDMI_GPIO_BSEL0_PIN, 1);
    R_GPIO_WritePin(HDMI_GPIO_BSEL1_PORT, HDMI_GPIO_BSEL1_PIN, 1);
}

