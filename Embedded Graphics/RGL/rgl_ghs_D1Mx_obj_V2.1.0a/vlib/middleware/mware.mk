#
# Just add the usual "USE_xxx" Defines
#
# First change macro name to upper case 
UPPER_CASE 		:= $(shell echo $(notdir $(VLIB_MW_NAME)) | tr a-z A-Z)
VLIB_DEFINE		+= USE_$(UPPER_CASE)
