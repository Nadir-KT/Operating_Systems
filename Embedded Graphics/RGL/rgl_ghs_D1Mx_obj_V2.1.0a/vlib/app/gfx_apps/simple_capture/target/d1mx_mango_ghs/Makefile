# ****************************************************************************
# PROJECT : VLIB
# FILE    : $Id: Makefile 17203 2020-06-03 02:39:46Z shinya.tomari.wj@renesas.com $
# ============================================================================
# DESCRIPTION
# Makefile for dave hd static drawinglist application
# ============================================================================
#                             C O P Y R I G H T                                    
# ============================================================================
#                            Copyright (c) 2014 - 2019
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
# Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.     
# ****************************************************************************

##############################################################################
#              Application configuration (mandatory part)
##############################################################################
# This is the name of your application subdirectory and will be used for the project
# files as well. Its directory name wil be used for project and *.out file naming 

VLIB_ROOT := $(firstword $(shell pwd | sed -re '{s/[Vv]-?[Ll][Ii][Bb][0-9]?/& /;}'))

include $(VLIB_ROOT)/compiler/setup.mk

VLIB_APP      		:= simple_capture.out
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

VLIB_USE_CDI 	:= yes

#Add here  bsp extensions 
VLIB_BSP_DRIVER	:= stdio
VLIB_BSP_DRIVER	+= hmi
VLIB_BSP_DRIVER	+= gfx

#VLIB_INC   += $(VLIB_ROOT)/app/gfx_apps/simple_capture/target/d1m2_mango_ghs/config

# uncomment the following two lines to enable a threaded DaveHD
# VLIB_DEFINE    += USE_THREADED_DHD
# include $(VLIB_ROOT)/os/ros/ros.mk

VLIB_DEFINE         += R_CVBS_VIDEO_CAPTURE_ENABLE
VLIB_DEFINE         += USE_VIN_NTSC=0		# 0:PAL   1:NTSC 
VLIB_DEFINE         += USE_VIN_SVIDEO=0		# 0:CVBS  1:S-VIDEO 

VLIB_DEFINE         += LOC_CAPTURE_SYNCHRONIZED=0
#VLIB_DEFINE         += USE_HDMI0_INTERFACE
VLIB_DEFINE         += USE_ITU_INSTEAD_VO0DATAENAB=1

# In case of D1M2(H), default VO0 DE signal is override with VIN1 signal.
# If USE_DATAENAB_INSTEAD_VO0HSYNC=1 is defined and Close JP98 1-2, 
# some monitor can be displayed without H-Sync. 
VLIB_DEFINE         += USE_DATAENAB_INSTEAD_VO0HSYNC=0 # 0:P43_0 is H-Sync 1:P43_0 is DE 

##############################################################################
# All further processing is intiated by the file below
# no need to bother about ;) 
#
# "parent" makefile
#
include $(VLIB_ROOT)/app/gfx_apps/simple_capture/simple_capture.mk


