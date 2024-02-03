# ****************************************************************************
# PROJECT : GRAPE
# FILE    : $Id: osal.mk 11006 2016-11-07 16:32:44Z michael.golczewski $
# AUTHOR  : $Author: michael.golczewski $
# ============================================================================ 
# DESCRIPTION
# GRAPE (Graphics Application Environment) framework
# ============================================================================
#                             C O P Y R I G H T                                    
# ============================================================================
#                            Copyright (c) 2013
#                                   by 
#                     Renesas Electronics (Europe) GmbH. 
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
#
# ****************************************************************************

#
# Set default configuration, if not configured
#

ifeq ($(GRAPE_OSAL_OS),)
GRAPE_OSAL_OS = posix
endif

#
# Configuration specific compiler and linker flags
#

ifeq ($(GRAPE_OSAL_OS),posix)
VLIB_LDFLAGS += -lpthread
endif


#
# Include OS
#

ifeq ($(GRAPE_OSAL_OS),r_os)
include $(VLIB_ROOT)/os/ros/ros.mk
endif


#
# Include and source path
#

VLIB_INC  += $(VLIB_ROOT)/grape/osal/lib
VLIB_INC  += $(VLIB_ROOT)/grape/osal/src/$(GRAPE_OSAL_OS)
VLIB_VPATH += $(VLIB_ROOT)/grape/osal/src/$(GRAPE_OSAL_OS)
