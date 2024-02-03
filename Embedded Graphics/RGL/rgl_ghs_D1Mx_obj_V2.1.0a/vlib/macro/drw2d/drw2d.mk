# ****************************************************************************
# PROJECT : VLIB toolbox
# FILE    : $Id: drw2d.mk 10613 2016-10-24 08:43:07Z dennis.ostermann $
# AUTHOR  : Michael Golczewski $
# ============================================================================ 
# DESCRIPTION
# debug driver makefile
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
#  
# LICENSEE has read, understood and accepted the terms and conditions defined in
# the license agreement, especially the usage rights. In any case, it is
# LICENSEE's responsibility to make sure that any user of the software complies
# with the terms and conditions of the signed license agreement.   
# ****************************************************************************

VLIB_MACRO_NAME:=drw2d
VLIB_USE_CDI 	:= yes
VLIB_INC += $(VLIB_ROOT)/macro/drw2d/lib

ifeq (,$(findstring USE_DRW2D_OGLES,$(VLIB_DEFINE)))
	ifeq ($(VLIB_DEVICE), d1lx)
	    VLIB_DEFINE += R_DRW2D_SYS_CPU
		VLIB_INC += $(VLIB_ROOT)/macro/drw2d/platform/cpu
		ifeq ($(VLIB_DRIVER_CONFIG),rgl_src)
			VLIB_VPATH += $(VLIB_ROOT)/macro/drw2d/platform/cpu
		else
			VLIB_LIB_VPATH += $(VLIB_ROOT)/macro/drw2d/platform/cpu
		endif
	else
		ifeq ($(VLIB_DEVICE), d1mx)
			VLIB_DEFINE += R_DRW2D_SYS_DHD
			VLIB_INC += $(VLIB_ROOT)/macro/drw2d/platform/davehd
			ifeq ($(VLIB_DRIVER_CONFIG),rgl_src)
				VLIB_VPATH += $(VLIB_ROOT)/macro/drw2d/platform/davehd
			else
				VLIB_LIB_VPATH += $(VLIB_ROOT)/macro/drw2d/platform/davehd
			endif
		endif
	endif
endif

# OS abstraction is always outside the library
ifneq (,$(findstring USE_ROS,$(VLIB_DEFINE)))
VLIB_VPATH  += $(VLIB_ROOT)/macro/drw2d/platform/os/ros
VLIB_DEFINE += R_DRW2D_OS_RENESAS
else
VLIB_VPATH += $(VLIB_ROOT)/macro/drw2d/platform/os/no_os
VLIB_DEFINE += R_DRW2D_OS_NO_OS
endif

include $(VLIB_ROOT)/compiler/macro.mk

