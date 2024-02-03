# ****************************************************************************
# PROJECT : VLIB
# FILE    : $Id: Makefile 16855 2019-08-06 01:12:45Z shinya.tomari.wj@renesas.com $
# ============================================================================
# DESCRIPTION
# Makefile for simple_draw (DRW2D drawing) application
# ============================================================================
#                             C O P Y R I G H T                                    
# ============================================================================
#                            Copyright (c) 2016
#                                   by 
#                        Renesas Electronics (Europe) GmbH. 
#                            Arcadiastrasse 10
#                           D-40472 Duesseldorf
#                                Germany
#                           All rights reserved.
# ============================================================================
# Purpose: only for testing
# 
# DISCLAIMER                                                                   
# This software is supplied by Renesas Electronics Corporation and is only     
# intended for use with Renesas products. No other uses are authorized. This   
# software is owned by Renesas Electronics Corporation and is protected under  
# all applicable laws, including copyright laws.                               
# THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING  
# THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT      
# LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE   
# AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.          
# TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS       
# ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE  
# FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR   
# ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE  
# BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.                             
# Renesas reserves the right, without notice, to make changes to this software 
# and to discontinue the availability of this software. By using this software,
# you agree to the additional terms and conditions found by accessing the      
# following link:                                                              
# http://www.renesas.com/disclaimer *                                          
# Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.     
# ****************************************************************************

##############################################################################
#              Application configuration (mandatory part)
##############################################################################
# This is the name of your application subdirectory and will be used for the project
# files as well. Its directory name wil be used for project and *.out file naming 

VLIB_ROOT := $(firstword $(shell pwd | sed -re '{s/[Vv]-?[Ll][Ii][Bb][0-9]?/& /;}'))

include $(VLIB_ROOT)/compiler/setup.mk

VLIB_APP      		:= simple_draw.out
VLIB_COMPILER 		:= rh850_ghs
VLIB_DEVICE   		:= d1mx
VLIB_BOARD    		:= d1mx_mango

VLIB_RENESAS_MISRA_STRICT	:= no
VLIB_BUILD_LOG := off

##############################################################################
#              Application configuration (optional part)
##############################################################################
#
#DRIVER_CONFIG set to default (use config_default.mk) if not changed here 
#Possible options:
#
#  rgl
#  max
#  default
#
ifeq ($(VLIB_DRIVER_CONFIG),)
VLIB_DRIVER_CONFIG = rgl_obj
endif
ifeq ($(VLIB_DRIVER_OPTIMIZE),yes)
VLIB_CFLAGS       += -Ospeed
endif
ifeq ($(VLIB_DRIVER_DEBUG),)
VLIB_DEBUG_BUILD   := yes
else
VLIB_DEBUG_BUILD   := no
endif

#Add here  bsp extensions 
VLIB_BSP_DRIVER	:= stdio
VLIB_BSP_DRIVER	+= hmi
VLIB_BSP_DRIVER	+= gfx

#VLIB_DEFINE         += BSP_SHOW_INFO
#VLIB_DEFINE         += DEV_OVERRIDE=R_DEV_R7F701442


##############################################################################
#              Option memory part
##############################################################################
#
# As simple_draw specification, select one flash or RAM as texture storage.
#
# In case of D1M1(H), D1M2(H);
#  Only SerialFlash(ch0) can be used, please add difinition LOC_SFMA_UNIT=0
#
# In case of D1M1A or D1M1-V2;
#  When SerialFlash(ch0) is used, please add difinition LOC_SFMA_UNIT=0 
#  When SerialFlash(ch1) is used, please add difinition LOC_SFMA_UNIT=1 and BSP_USE_SFMA1 
#  When SerialFlash(ch2) is used, please add difinition LOC_SFMA_UNIT=2 and BSP_USE_SFMA2 
#  When OctaFlash        is used, please add difinition LOC_SFMA_UNIT=0 and BSP_INIT_OCTA_FLASH_R 
#  When OctaRAM          is used, please add difinition LOC_SFMA_UNIT=0 and BSP_INIT_OCTA_RAM_RW 
#  When HyperFlash       is used, please add difinition LOC_SFMA_UNIT=0 and BSP_INIT_HYPB_FLASH_R 
#  When HyperRAM         is used, please add difinition LOC_SFMA_UNIT=0 and BSP_INIT_HYPB_RAM_RW 
# 
# D1M1A/D1M1-V2 Mango-adapter bopard, external RAM/Flash is connected to [U2] and [U5].
#  SerialFlash(ch0) means on-board Serial Flash 1cs/2cs on [U5] 
#  SerialFlash(ch1) means on-board Serial Flash 1cs     on [U2] 
#  SerialFlash(ch2) means on-board Serial Flash 2cs     on [U5] 
#  OctaFlash        means on-board Octa Flash/Octa MCP  on [U5] 
#  OctaRAM          means on-board Octa RAM  /Octa MCP  on [U5] 
#  HyperFlash       means on-board Hyper Flash          on [U5] 
#  HyperRAM         means on-board Hyper RAM            on [U5] 
# 
#  Note: If SerialFlash(ch1) is seletecd, default display becomes NEC_QVGA_TOUCH_240x320.
#  See ../src/config.h for the detail.

# Use SFMA
VLIB_DEFINE         += LOC_SFMA_UNIT=0     # 0:SFMA0 1:SFMA1 2:SFMA2
#VLIB_DEFINE         += BSP_USE_SFMA1
#VLIB_DEFINE         += BSP_USE_SFMA2

# Use OCTA
#VLIB_DEFINE         += BSP_INIT_OCTA_FLASH_R
#VLIB_DEFINE         += BSP_INIT_OCTA_RAM_RW
#VLIB_DEFINE         += BSP_INIT_OCTA_PIN
#VLIB_DEFINE         += BSP_OCTA_FLASH_CH=0
#VLIB_DEFINE         += BSP_OCTA_RAM_CH=1

# Use HYPB
#VLIB_DEFINE         += BSP_INIT_HYPB_FLASH_R
#VLIB_DEFINE         += BSP_INIT_HYPB_RAM_RW
#VLIB_DEFINE         += BSP_INIT_HYPB_PIN

##############################################################################
# All further processing is intiated by the file below
# no need to bother about ;) 
#
# "parent" makefile
#
include $(VLIB_ROOT)/app/gfx_apps/simple_draw/simple_draw.mk


