# ****************************************************************************
# PROJECT : GRAPE
# FILE    : $Id: grape.mk 11006 2016-11-07 16:32:44Z michael.golczewski $
# AUTHOR  : $Author: michael.golczewski $
# ============================================================================ 
# DESCRIPTION
# GRAPE (Graphics Application Environment) framework
# ============================================================================
#                             C O P Y R I G H T                                    
# ============================================================================
#                            Copyright (c) 2011
#                                   by 
#                     Renesas Electronics (Europe) GmbH. 
#                            Arcadiastrasse 10
#                           D-40472 Duesseldorf
#                                Germany
#                           All rights reserved.
# ============================================================================
# Purpose: only for testing
# 
# Warranty Disclaimer
# 
# Because the Product(s) is licensed free of charge, there is no warranty 
# of any kind whatsoever and expressly disclaimed and excluded by Renesas, 
# either expressed or implied, including but not limited to those for 
# non-infringement of intellectual property, merchantability and/or 
# fitness for the particular purpose. 
# Renesas shall not have any obligation to maintain, service or provide bug 
# fixes for the supplied Product(s) and/or the Application.
# 
# Each User is solely responsible for determining the appropriateness of 
# using the Product(s) and assumes all risks associated with its exercise 
# of rights under this Agreement, including, but not limited to the risks 
# and costs of program errors, compliance with applicable laws, damage to 
# or loss of data, programs or equipment, and unavailability or 
# interruption of operations.
# 
# Limitation of Liability
# 
# In no event shall Renesas be liable to the User for any incidental, 
# consequential, indirect, or punitive damage (including but not limited 
# to lost profits) regardless of whether such liability is based on breach 
# of contract, tort, strict liability, breach of warranties, failure of 
# essential purpose or otherwise and even if advised of the possibility of 
# such damages. Renesas shall not be liable for any services or products 
# provided by third party vendors, developers or consultants identified or
# referred to the User by Renesas in connection with the Product(s) and/or the 
# Application.
# ****************************************************************************

#
# Include GRAPE components
#

ifeq ($(GRAPE_CLIBAL_ENABLE),yes)
include $(VLIB_ROOT)/grape/clibal/clibal.mk
endif

ifeq ($(GRAPE_BOAL_ENABLE),yes)
include $(VLIB_ROOT)/grape/boal/boal.mk
endif

ifeq ($(GRAPE_OSAL_ENABLE),yes)
include $(VLIB_ROOT)/grape/osal/osal.mk
endif

ifeq ($(GRAPE_VOVIAL_ENABLE),yes)
include $(VLIB_ROOT)/grape/vovial/vovial.mk
endif

ifeq ($(GRAPE_G3DAL_ENABLE),yes)
include $(VLIB_ROOT)/grape/g3dal/g3dal.mk
endif

ifeq ($(GRAPE_FSAL_ENABLE),yes)
include $(VLIB_ROOT)/grape/fsal/fsal.mk
endif

ifeq ($(GRAPE_G2DAL_ENABLE),yes)
include $(VLIB_ROOT)/grape/g2dal/g2dal.mk
endif

ifeq ($(GRAPE_HMIAL_ENABLE),yes)
include $(VLIB_ROOT)/grape/hmial/hmial.mk
endif