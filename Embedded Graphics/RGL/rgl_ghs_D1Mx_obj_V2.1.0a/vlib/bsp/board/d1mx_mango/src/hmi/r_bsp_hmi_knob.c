/*
****************************************************************************
PROJECT : BSP HMI Knob/Button
FILE    : $Id: r_bsp_hmi_knob.c 11411 2016-12-02 13:21:10Z tobyas.hennig $
============================================================================ 
DESCRIPTION
BSP Implementation D1Mx MANGO Board support sys functions - knob and button
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
#include "r_bsp_hmi_api.h"
#include "r_bsp_hmi_knob.h" /* BSP HMI Knob and Button */
#include "r_tauj_api.h"


/*******************************************************************************
  Section: Local variable 
*/

/*******************************************************************************
  Variable: locRight 
            locLeft 

  Debouncing counters for clean rotation recognition
*/
static int32_t locPinA = 0; 
static int32_t locPinB = 0; 

/*******************************************************************************
  Section: Local Functions
*/
static void locRtKnobAAction(void);
static void locRtKnobBAction(void);
static void locRtKnobCenterAction(void);
static void locConfigTauj(void); 
static void locTauj0ch0Isr (void); 
static void locTauj0ch1Isr (void); 
static void locButtonLeftAction(void);
static void locButtonUpAction(void);

#if USE_FOUT_INSTEAD_BSP_BTN_DOWN != 1
    static void locButtonDownAction(void);
#endif  /* USE_FOUT_INSTEAD_BSP_BTN_DOWN */

static void locButtonRightAction(void);
static void locButtonCenterAction(void);

/*
  Configure TAUJ to generate interrupt every 500uSec
*/
static void locConfigTauj(void)
{
    r_tauj_Error_t error = R_TAUJ_ERR_OK;
    uint8_t Inst = 0;
    error = R_TAUJ_Init(0);
    /* set the clock period for CK0 (TPS) */
    error += R_TAUJ_SetClockPeriod(Inst, R_TAUJ_CK_0,  R_TAUJ_TIME_INTERVAL_0);
    /* select the clock source for the timer channel CK_0 - 3 in the CMOR */
    error += R_TAUJ_SetClkSource(Inst, (r_tauj_Channel_t)0, R_TAUJ_CK_0);
    error += R_TAUJ_SetClkSource(Inst, (r_tauj_Channel_t)1, R_TAUJ_CK_0);
    /* select the mode for the timer channel (CMOR) */
    error += R_TAUJ_SetMode(Inst, (r_tauj_Channel_t)0, R_TAUJ_INTERVAL_TIMER_MODE);
    error += R_TAUJ_SetMode(Inst, (r_tauj_Channel_t)1, R_TAUJ_INTERVAL_TIMER_MODE);
    /* Write the count value in the CDR */
    error += R_TAUJ_SetPeriod(Inst, (r_tauj_Channel_t)0, /*0x15E0*/0x12C0 * 15);
    error += R_TAUJ_SetPeriod(Inst, (r_tauj_Channel_t)1, /*0x15E0*/0x12C0 * 15);
    /* Enable the output (TOE) */
    error += R_TAUJ_OutputDisable(Inst, R_TAUJ_CHANNEL_0);
    /* Enable the output (TOE) */
    error += R_TAUJ_OutputDisable(Inst, R_TAUJ_CHANNEL_1);
    /* Enable interrupt */
    R_TAUJ_EnableInt(0, R_TAUJ_INT_CH0);
    R_TAUJ_EnableInt(0, R_TAUJ_INT_CH1);
    /* Set ISR routine */
    R_TAUJ_SetIsrCallback(0, R_TAUJ_INT_CH0, &locTauj0ch0Isr);
    R_TAUJ_SetIsrCallback(0, R_TAUJ_INT_CH1, &locTauj0ch1Isr);    
}

/*******************************************************************************
  Function: locTauj0ch0Isr

   Every edge on inputA re-starts the corresponding timer channel. 
   This is the tiner ISR. 
   Input Debouncing: 
   When this ISR is called that means the input was stable for the defined timer
   And can be processed - Button action call  

    Parameters:
    void

    Returns:
    void
 */

static void locTauj0ch0Isr (void)
{
    uint8_t local_mask = 0; 
    /* Stop counter */
    R_TAUJ_Stop(0, R_TAUJ_CHANNEL_0);
    
    /* Clear flag */
    locPinA = 0; 
    
    /* check if the other pin is stable too */
    if (0 == locPinB)
    {
        local_mask   =  R_GPIO_ReadIntPin(BSP_KNOB_DIR_A_INTP);    
        local_mask <<=  (uint8_t)1;    
        local_mask  |=  R_GPIO_ReadIntPin(BSP_KNOB_DIR_B_INTP);    
        
        switch (local_mask)
        {
        case 0:
            R_BSP_BtnAction(BSP_BTN_LEFT_TURN);
            break;
        case 1:
            R_BSP_BtnAction(BSP_BTN_RIGHT_TURN);
            break;
        case 2:
            R_BSP_BtnAction(BSP_BTN_RIGHT_TURN);
            break;
        case 3:
            R_BSP_BtnAction(BSP_BTN_LEFT_TURN);
            break;
        default:
            break;
        }
    }
}

/*******************************************************************************
  Function: locTauj0ch1Isr

   Every edge on inputA re-starts the corresponding timer channel. 
   This is the tiner ISR. 
   Input Debouncing: 
   When this ISR is called that means the input was stable for the defined timer
   And can be processed

    Parameters:
    void

    Returns:
    void
 */

static void locTauj0ch1Isr (void)
{
    uint8_t local_mask = 0; 
    /* Stop counter */
    R_TAUJ_Stop(0, R_TAUJ_CHANNEL_1);
    
    /* Clear flag */
    locPinB = 0; 
    
    /* check if the other pin is stable too */
    if (0 == locPinA)
    {
        local_mask   =  R_GPIO_ReadIntPin(BSP_KNOB_DIR_A_INTP);    
        local_mask <<=  (uint8_t)1;    
        local_mask  |=  R_GPIO_ReadIntPin(BSP_KNOB_DIR_B_INTP);    
        
        switch (local_mask)
        {
        case 0:
            R_BSP_BtnAction(BSP_BTN_RIGHT_TURN);
            break;
        case 1:
            R_BSP_BtnAction(BSP_BTN_LEFT_TURN);
            break;
        case 2:
            R_BSP_BtnAction(BSP_BTN_LEFT_TURN);
            break;
        case 3:
            R_BSP_BtnAction(BSP_BTN_RIGHT_TURN);
            break;
        default:
            break;
        }
    } 
}

/*******************************************************************************
  Function: locRtKnobAAction

    This function is triggered by the pin input A. 
    For debopuncing purposed it just restarts a timer. 
    
    Parameters:
    void

    Returns:
    void

*/
static void locRtKnobAAction(void)
{
   locPinA = 1; 
   /* (re)start the "debouncing" timer channel */
   R_TAUJ_Start(0, R_TAUJ_CHANNEL_0);
}

/*******************************************************************************
  Function: locRtKnobBAction

    This function is triggered by the pin input B. 
    For debopuncing purposed it just restarts a timer. 
    
    Parameters:
    void

    Returns:
    void

*/
static void locRtKnobBAction(void)
{
   locPinB = 1; 
   /* (re)start the "debouncing" timer channel */
   R_TAUJ_Start(0, R_TAUJ_CHANNEL_1);
}

/*******************************************************************************
  Function: locRtKnobCenterAction

    This function will be asigned as the callback
    to the external interrupt connected to the rotary knob Center button.
    After recognition, the BSP Button action routine will be called with the
    right parameter.

    Parameters:
    void

    Returns:
    void

*/
static void locRtKnobCenterAction(void)
{
    uint8_t level = 1u; /* The buttons are low-active */
    level = R_GPIO_ReadIntPin(BSP_KNOB_PRESS_INTP);

    if (level == 0u) /* level is  LOW -> button is now pressed */
    {
        R_BSP_BtnAction(BSP_BTN_OK_PRESS);
    } 
    else 
    {
        R_BSP_BtnAction(BSP_BTN_OK_RELEASE);
    }
}


/***********************************************************
  Function: locButtonLeftAction
*/
static void locButtonLeftAction(void)
{
    R_BSP_BtnAction(BSP_BTN_LEFT_PRESS);
    R_BSP_BtnAction(BSP_BTN_LEFT_RELEASE);
}

/***********************************************************
  Function: locButtonUpAction
*/
static void locButtonUpAction(void)
{
    R_BSP_BtnAction(BSP_BTN_UP_PRESS);
    R_BSP_BtnAction(BSP_BTN_UP_RELEASE);
}

/***********************************************************
  Function: locButtonDownAction
*/
#if USE_FOUT_INSTEAD_BSP_BTN_DOWN != 1
static void locButtonDownAction(void)
{
    R_BSP_BtnAction(BSP_BTN_DOWN_PRESS);
    R_BSP_BtnAction(BSP_BTN_DOWN_RELEASE);
}
#endif  /* USE_FOUT_INSTEAD_BSP_BTN_DOWN */

/***********************************************************
  Function: locButtonRightAction
*/
static void locButtonRightAction(void)
{
    R_BSP_BtnAction(BSP_BTN_RIGHT_PRESS);
    R_BSP_BtnAction(BSP_BTN_RIGHT_RELEASE);
}

/***********************************************************
  Function: locButtonCenterAction
*/
static void locButtonCenterAction(void)
{
    R_BSP_BtnAction(BSP_BTN_CENTER_PRESS);
    R_BSP_BtnAction(BSP_BTN_CENTER_RELEASE);
}

/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_BSP_HMI_InitRotaryKnob

    Init Rotary Knob

    Parameters:
    void

    Returns:
    void

*/
void R_BSP_HMI_InitRotaryKnob(void)
{
    r_gpio_IntConfig_t int_cfg; 
 
    /* Rotary knob connections */
    int_cfg.Port     = BSP_KNOB_PRESS_PORT; 
    int_cfg.Pin      = BSP_KNOB_PRESS_PIN; 
    int_cfg.Trigger  = R_GPIO_INT_BOTH_EDGES; 
    int_cfg.Callback = &locRtKnobCenterAction; 
    R_GPIO_InitInt(BSP_KNOB_PRESS_INTP, &int_cfg);
    
    int_cfg.Port     = BSP_KNOB_DIR_A_PORT; 
    int_cfg.Pin      = BSP_KNOB_DIR_A_PIN; 
    int_cfg.Trigger  = R_GPIO_INT_BOTH_EDGES; 
    int_cfg.Callback = &locRtKnobAAction; 
    R_GPIO_InitInt(BSP_KNOB_DIR_A_INTP, &int_cfg);

    int_cfg.Port     = BSP_KNOB_DIR_B_PORT; 
    int_cfg.Pin      = BSP_KNOB_DIR_B_PIN; 
    int_cfg.Trigger  = R_GPIO_INT_BOTH_EDGES; 
    int_cfg.Callback = &locRtKnobBAction; 
    R_GPIO_InitInt(BSP_KNOB_DIR_B_INTP,  &int_cfg);

    /* Rotary knob Interrupts */
    R_GPIO_EnableInt(BSP_KNOB_DIR_A_INTP);
    R_GPIO_EnableInt(BSP_KNOB_DIR_B_INTP);
    R_GPIO_EnableInt(BSP_KNOB_PRESS_INTP);
    locConfigTauj(); 
}


/*******************************************************************************
  Function: R_BSP_HMI_InitButton

    This function will init Buttons

    Parameters:
    void

    Returns:
    void
*/
void R_BSP_HMI_InitButton(void)
{
    r_gpio_IntConfig_t int_cfg; 
 
    /* Button connections */
    int_cfg.Port     = BSP_BTN_LEFT_PORT; 
    int_cfg.Pin      = BSP_BTN_LEFT_PIN; 
    int_cfg.Trigger  = R_GPIO_INT_BOTH_EDGES; 
    int_cfg.Callback = &locButtonLeftAction; 
    R_GPIO_InitInt(BSP_BTN_LEFT_INTP, &int_cfg);
    
    int_cfg.Port     = BSP_BTN_UP_PORT; 
    int_cfg.Pin      = BSP_BTN_UP_PIN; 
    int_cfg.Trigger  = R_GPIO_INT_BOTH_EDGES; 
    int_cfg.Callback = &locButtonUpAction; 
    R_GPIO_InitInt(BSP_BTN_UP_INTP, &int_cfg);

#if USE_FOUT_INSTEAD_BSP_BTN_DOWN != 1
    int_cfg.Port     = BSP_BTN_DOWN_PORT; 
    int_cfg.Pin      = BSP_BTN_DOWN_PIN; 
    int_cfg.Trigger  = R_GPIO_INT_BOTH_EDGES; 
    int_cfg.Callback = &locButtonDownAction; 
    R_GPIO_InitInt(BSP_BTN_DOWN_INTP,  &int_cfg);
#endif  /* USE_FOUT_INSTEAD_BSP_BTN_DOWN */

    int_cfg.Port     = BSP_BTN_RIGHT_PORT; 
    int_cfg.Pin      = BSP_BTN_RIGHT_PIN; 
    int_cfg.Trigger  = R_GPIO_INT_BOTH_EDGES; 
    int_cfg.Callback = &locButtonRightAction; 
    R_GPIO_InitInt(BSP_BTN_RIGHT_INTP,  &int_cfg);
    
    int_cfg.Port     = BSP_BTN_CENTER_PORT; 
    int_cfg.Pin      = BSP_BTN_CENTER_PIN; 
    int_cfg.Trigger  = R_GPIO_INT_BOTH_EDGES; 
    int_cfg.Callback = &locButtonCenterAction; 
    R_GPIO_InitInt(BSP_BTN_CENTER_INTP,  &int_cfg);
    
    /* Button Interrupts */
    R_GPIO_EnableInt(BSP_BTN_LEFT_INTP);
    R_GPIO_EnableInt(BSP_BTN_UP_INTP);
#if USE_FOUT_INSTEAD_BSP_BTN_DOWN != 1
    R_GPIO_EnableInt(BSP_BTN_DOWN_INTP);
#endif  /* USE_FOUT_INSTEAD_BSP_BTN_DOWN */
    R_GPIO_EnableInt(BSP_BTN_RIGHT_INTP);
    R_GPIO_EnableInt(BSP_BTN_CENTER_INTP);
}

