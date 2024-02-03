# ****************************************************************************
# PROJECT : GRAPE_APP
# FILE    : $Id: grape_app.mk 11006 2016-11-07 16:32:44Z michael.golczewski $
# ============================================================================
# DESCRIPTION
# GRAPE Application (Graphics Application Environment) framework
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
# GRAPE configuration and include
#

GRAPE_CLIBAL_ENABLE = yes
GRAPE_BOAL_ENABLE   = yes
GRAPE_OSAL_ENABLE   = yes
GRAPE_VOVIAL_ENABLE = yes
GRAPE_HMIAL_ENABLE  = yes
GRAPE_FSAL_ENABLE   = yes

#
# Minimum ROMFS data paths
#

ROMFS_PATH = $(VLIB_ROOT)/app/grape/grape_app/romfs/data
ROMFS_SF_PATH = $(VLIB_ROOT)/app/grape/grape_app/romfs/data_sf

# General gfx app framework
#

#
# Source code search path
#

VLIB_VPATH	+= $(VLIB_ROOT)/app/grape/grape_app/src
VLIB_INC	+= $(VLIB_ROOT)/app/grape/grape_app/src

VLIB_VPATH	+= $(VLIB_ROOT)/app/grape/grape_app/romfs/src/platform/$(VLIB_BOARD)

ifeq ($(APP_MENU), yes)
VLIB_CFLAGS	+= -DAPP_MENU
VLIB_INC	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_menu
VLIB_VPATH	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_menu
VLIB_VPATH  += $(VLIB_ROOT)/app/grape/grape_app/src/fonts/src
ROMFS_PATH  += $(VLIB_ROOT)/app/grape/grape_app/src/fonts/bin
endif

ifeq ($(APP_CLOCK), yes)
VLIB_CFLAGS	+= -DAPP_CLOCK
VLIB_INC	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_clock
VLIB_VPATH	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_clock
VLIB_VPATH	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_clock/image/src
ROMFS_PATH  += $(VLIB_ROOT)/app/grape/grape_app/src/app_clock/image/bin
endif

ifeq ($(APP_SIMPLEMT), yes)
VLIB_CFLAGS	+= -DAPP_SIMPLEMT
VLIB_INC	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_simplemt
VLIB_VPATH	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_simplemt
VLIB_VPATH	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_simplemt/image/src
ROMFS_PATH  += $(VLIB_ROOT)/app/grape/grape_app/src/app_simplemt/image/bin
endif

ifeq ($(APP_TEST), yes)
VLIB_CFLAGS	+= -DAPP_TEST
VLIB_INC	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_test
VLIB_VPATH	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_test
VLIB_VPATH	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_test/image/src
ROMFS_PATH  += $(VLIB_ROOT)/app/grape/grape_app/src/app_test/image/bin
endif

ifeq ($(APP_DRW2DCPU), yes)
VLIB_CFLAGS	+= -DAPP_DRW2DCPU
VLIB_INC	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_drw2dcpu
VLIB_VPATH	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_drw2dcpu
VLIB_VPATH	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_drw2dcpu/image/src
ROMFS_PATH  += $(VLIB_ROOT)/app/grape/grape_app/src/app_drw2dcpu/image/bin
endif

ifeq ($(APP_TRIPCOMP), yes)
VLIB_CFLAGS	+= -DAPP_TRIPCOMP
VLIB_INC	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_tripcomp
VLIB_VPATH	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_tripcomp
VLIB_VPATH	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_tripcomp/image/rom/src
ROMFS_PATH  += $(VLIB_ROOT)/app/grape/grape_app/src/app_tripcomp/image/rom/bin
VLIB_VPATH	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_tripcomp/image/sf/src
ROMFS_SF_PATH += $(VLIB_ROOT)/app/grape/grape_app/src/app_tripcomp/image/sf/bin
ifneq ($(APP_MENU), yes)
    VLIB_VPATH  += $(VLIB_ROOT)/app/grape/grape_app/src/fonts/src
    ROMFS_PATH  += $(VLIB_ROOT)/app/grape/grape_app/src/fonts/bin
endif
endif

ifeq ($(APP_TUTORIAL), yes)
VLIB_CFLAGS	+= -DAPP_TUTORIAL
VLIB_INC	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_tutorial
VLIB_VPATH	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_tutorial
VLIB_VPATH	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_tutorial/image/src
ROMFS_PATH  += $(VLIB_ROOT)/app/grape/grape_app/src/app_tutorial/image/bin
endif

ifeq ($(APP_OGLES_TEST), yes)
VLIB_CFLAGS	+= -DAPP_OGLES_TEST -DOGLES20
VLIB_INC	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_ogles_test
VLIB_VPATH	+= $(VLIB_ROOT)/app/grape/grape_app/src/app_ogles_test
endif

ifeq ($(APP_3DDEMO), yes)
VLIB_CFLAGS += -DAPP_3DDEMO -DOGLES20
VLIB_INC += $(VLIB_ROOT)/app/grape/grape_app/src/app_3dDemo
VLIB_VPATH   += $(VLIB_ROOT)/app/grape/grape_app/src/app_3dDemo
ifeq ($(GRAPE_FSAL_MODE), romfs)
ROMFS_PATH += $(VLIB_ROOT)/app/grape/app_3dDemo/src/data
VLIB_VPATH   += $(VLIB_ROOT)/app/grape/app_3dDemo/romfs/src/platform/$(APP_BOARD)
endif
endif

ifeq ($(APP_DISPSYNCTEST), yes)
VLIB_CFLAGS  += -DAPP_DISPSYNC_TEST -DOGLES20
VLIB_INC += $(VLIB_ROOT)/app/grape/grape_app/src/app_dispsynctest
VLIB_VPATH   += $(VLIB_ROOT)/app/grape/grape_app/src/app_dispsynctest
endif

include $(VLIB_ROOT)/grape/grape.mk

#
# include generic compiler
#

include $(VLIB_ROOT)/compiler/compiler.mk

#
# Make sure the application is built by default
#

default: $(APP)


#
# Filesystem data
#

ifeq ($(ROMFS_ADDR),)
ROMFS_ADDR = 0x00000000
endif # ROMFS_ADDR
ROMFS_SF_ADDR = 0x10000000

ifeq ($(ROMFS_SF_ADDR),)
romfs:
	@echo Creating ROMFS
	@echo "" > ./makefs.sh
	@echo "mkdir -p $(VLIB_ROOT)/app/grape/grape_app/romfs/tmp" >> ./makefs.sh
	@for i in $(INCLUDE_FONT_ROM); do \
	  echo "cp -p $$i/* $(VLIB_ROOT)/app/grape/grape_app/romfs/tmp" >> ./makefs.sh; \
	done
	@for i in $(ROMFS_PATH); do \
	  echo "cp -p $$i/*.* $(VLIB_ROOT)/app/grape/grape_app/romfs/tmp" >> ./makefs.sh;  \
	done
	@echo "cd $(VLIB_ROOT)/app/grape/grape_app/romfs" >> ./makefs.sh
	@echo "./create.sh $(ROMFS_ADDR) $(VLIB_BOARD)" >> ./makefs.sh
	@chmod 755 ./makefs.sh
	@./makefs.sh
	@echo ROMFS created.
else
romfs:
	@echo Creating ROMFS
	@echo "" > ./makefs.sh
	@echo "mkdir -p $(VLIB_ROOT)/app/grape/grape_app/romfs/tmp" >> ./makefs.sh
	@echo "mkdir -p $(VLIB_ROOT)/app/grape/grape_app/romfs/tmp/cpu" >> ./makefs.sh
	@echo "mkdir -p $(VLIB_ROOT)/app/grape/grape_app/romfs/tmp/serial" >> ./makefs.sh
	@for i in $(INCLUDE_FONT_ROM); do \
	  echo "cp -p $$i/* $(VLIB_ROOT)/app/grape/grape_app/romfs/tmp/cpu" >> ./makefs.sh; \
	done
	@for i in $(ROMFS_PATH); do \
	  echo "cp -p $$i/*.* $(VLIB_ROOT)/app/grape/grape_app/romfs/tmp/cpu" >> ./makefs.sh;  \
	done
	@for i in $(INCLUDE_FONT_ROM); do \
	  echo "cp -p $$i/* $(VLIB_ROOT)/app/grape/grape_app/romfs/tmp/serial" >> ./makefs.sh; \
	done
	@for i in $(ROMFS_SF_PATH); do \
	   echo "cp -p $$i/*.* $(VLIB_ROOT)/app/grape/grape_app/romfs/tmp/serial" >> ./makefs.sh; \
	done
	@echo "cd $(VLIB_ROOT)/app/grape/grape_app/romfs" >> ./makefs.sh
	@echo "./create_sf.sh $(ROMFS_ADDR) $(ROMFS_SF_ADDR) $(VLIB_BOARD)" >> ./makefs.sh
	@chmod 755 ./makefs.sh
	@./makefs.sh
	@echo ROMFS created.
endif