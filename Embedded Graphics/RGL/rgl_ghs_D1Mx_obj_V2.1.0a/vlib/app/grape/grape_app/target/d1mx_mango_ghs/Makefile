# ****************************************************************************
# PROJECT : VLIB
# FILE    : $Id: Makefile 11006 2016-11-07 16:32:44Z michael.golczewski $
# ============================================================================
# DESCRIPTION
# Makefile for grape app
# ============================================================================
#                             C O P Y R I G H T                                    
# ============================================================================
#                            Copyright (c) 2012
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

VLIB_APP      	:= grape_app.out
VLIB_COMPILER 	:= rh850_ghs
VLIB_DEVICE  	:= d1mx
VLIB_BOARD    	:= d1mx_mango
VLIB_BUILD_LOG  := off
VLIB_RENESAS_MISRA_STRICT	:= no

##############################################################################
#              Application configuration (optional part)
##############################################################################
#
#DRIVER_CONFIG set to default (use config_default.mk) if not changed here 
#Possible options:
#
#  rgl_obj
#  rgl_src
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

#
# Target specific GRAPE configuration
#
GRAPE_OSAL_OS        = r_os
GRAPE_BOAL_BOARD     = d1x_mango
GRAPE_VOVIAL_TARGET  = wm
GRAPE_HMIAL_TARGET   = r_os
GRAPE_FSAL_TARGET    = romfs

VLIB_DEFINE += USE_THREADED_DHD

#
# Application
#
APP_MENU         = yes
APP_CLOCK        = yes
APP_DRW2DCPU     = yes
APP_SIMPLEMT     = yes
APP_TEST         = yes
APP_TUTORIAL     = yes
APP_TRIPCOMP     = yes
APP_OGLES_TEST   = no
APP_3DDEMO       = no
APP_DISPSYNCTEST = no

VLIB_BSP_DRIVER   := stdio
VLIB_BSP_DRIVER   += hmi
VLIB_BSP_DRIVER   += gfx

VLIB_INC          += $(VLIB_ROOT)/app/grape/grape_app/target/d1mx_mango_ghs/config
VLIB_VPATH        += $(VLIB_ROOT)/app/grape/grape_app/target/d1mx_mango_ghs/config

##############################################################################
#                    End of application configuration 
##############################################################################
# All further processing is intiated by the file below
# no need to bother about ;) 
#
# "parent" makefile
#
include $(VLIB_ROOT)/app/grape/grape_app/grape_app.mk


