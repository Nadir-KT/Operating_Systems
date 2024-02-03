# ****************************************************************************
# PROJECT : VLIB driver
# FILE    : $Id: davehd.mk 11006 2016-11-07 16:32:44Z michael.golczewski $
# ============================================================================ 
# DESCRIPTION
# DAVEHD driver makefile
# ============================================================================
#                             C O P Y R I G H T                                    
# ============================================================================
#                            Copyright (c) 2014
#                                   by 
#                      Renesas Electronics (Europe) GmbH. 
#                            Arcadiastrasse 10
#                           D-40472 Duesseldorf
#                                Germany
#                           All rights reserved.
# ============================================================================
# DISCLAIMER
# 
# LICENSEE has read, understood and accepted the terms and conditions defined in
# the license agreement, especially the usage rights. In any case, it is
# LICENSEE's responsibility to make sure that any user of the software complies
# with the terms and conditions of the signed license agreement.

# ****************************************************************************


VLIB_MACRO_NAME:=gpu/davehd
VLIB_DEFINE    += RENESAS_D1X
VLIB_USE_CDI 	:= yes

VLIB_INC += $(VLIB_ROOT)/macro/gpu/davehd/kernel/inc
VLIB_INC += $(VLIB_ROOT)/macro/gpu/davehd/user/inc
VLIB_INC += $(VLIB_ROOT)/macro/gpu/davehd/common/inc
VLIB_INC += $(VLIB_ROOT)/macro/gpu/davehd/common/inc/platform
VLIB_INC += $(VLIB_ROOT)/macro/gpu/davehd/common/inc/platform/$(VLIB_DEVICE)

# driver platform implementations need internal driver headers
VLIB_INC += $(VLIB_ROOT)/macro/gpu/davehd/user/src
VLIB_INC += $(VLIB_ROOT)/macro/gpu/davehd/kernel/src

ifeq ($(VLIB_DRIVER_CONFIG),rgl_obj)
VLIB_LIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/kernel/src
VLIB_LIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/user/src
VLIB_LIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/common/inc
# the lockdown code (renesas-only)
VLIB_LIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/kernel/platform/$(VLIB_DEVICE)/bridge
else
VLIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/kernel/src
VLIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/user/src
VLIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/common/inc
# the lockdown code (renesas-only)
VLIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/kernel/platform/$(VLIB_DEVICE)/bridge
endif

# Platform code is always outside of the library
VLIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/kernel/platform/$(VLIB_DEVICE)
VLIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/kernel/platform/ree
ifneq (,$(findstring USE_THREADED_DHD,$(VLIB_DEFINE)))
VLIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/kernel/platform/$(VLIB_DEVICE)/threaded
else
VLIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/kernel/platform/$(VLIB_DEVICE)/non_threaded
endif
ifneq (,$(findstring USE_ROS,$(VLIB_DEFINE)))
VLIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/user/platform/$(VLIB_DEVICE)/ros
VLIB_INC   += $(VLIB_ROOT)/macro/gpu/davehd/user/platform/$(VLIB_DEVICE)/ros
else
VLIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/user/platform/$(VLIB_DEVICE)/no_os
VLIB_INC   += $(VLIB_ROOT)/macro/gpu/davehd/user/platform/$(VLIB_DEVICE)/no_os
endif

# sys part for HW near things like register addresses etc.
VLIB_INC   += $(VLIB_ROOT)/device/$(VLIB_DEVICE)/src/dhd
VLIB_VPATH += $(VLIB_ROOT)/device/$(VLIB_DEVICE)/src/dhd


ifneq (,$(findstring USE_OPENVG_API,$(VLIB_DEFINE)))
VLIB_INC   += $(VLIB_ROOT)/macro/gpu/davehd/openvg1.1/include
VLIB_INC   += $(VLIB_ROOT)/macro/gpu/davehd/openvg1.1/include/VG
VLIB_INC   += $(VLIB_ROOT)/macro/gpu/davehd/egl/include
VLIB_INC   += $(VLIB_ROOT)/macro/gpu/davehd/egl/include/EGL
VLIB_INC   += $(VLIB_ROOT)/macro/gpu/davehd/egl/src
VLIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/egl/src
VLIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/egl/src/$(VLIB_DEVICE)
ifeq ($(VLIB_DRIVER_CONFIG),rgl_obj)
VLIB_LIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/openvg1.1/src
else
VLIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/openvg1.1/src
endif

endif

ifneq (,$(findstring USE_DAVE2D_API,$(VLIB_DEFINE)))
VLIB_INC += $(VLIB_ROOT)/macro/gpu/davehd/dave2d_wrapper/inc
VLIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/dave2d_wrapper/src
endif

# license
VLIB_VPATH += $(VLIB_ROOT)/macro/gpu/davehd/docs


include $(VLIB_ROOT)/compiler/macro.mk
