# ****************************************************************************
# PROJECT : GRAPE
# FILE    : $Id: boal.mk 11006 2016-11-07 16:32:44Z michael.golczewski $
# AUTHOR  : $Author: michael.golczewski $
# ============================================================================
# DESCRIPTION
# GRAPE board setup makefile
# ============================================================================
#                             C O P Y R I G H T                                    
# ============================================================================
#                            Copyright (c) 2013
#                                   by 
#                      Renesas Electronics (Europe) GmbH. 
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

#
# Set default configuration, if not configured
#

ifeq ($(GRAPE_BOAL_BOARD),)
GRAPE_BOAL_BOARD = dummy
endif

#
# Include and source path
#

ifeq ($(GRAPE_BOAL_BOARD),d1m2_mango)

endif

VLIB_INC  += $(VLIB_ROOT)/grape/boal/lib
VLIB_VPATH += $(VLIB_ROOT)/grape/boal/src/$(GRAPE_BOAL_BOARD)
