# ****************************************************************************
# PROJECT : VLIB
# FILE    : $Id: ultraedit.mk 11006 2016-11-07 16:32:44Z michael.golczewski $ 
# ============================================================================ 
# DESCRIPTION
# Makefile rules for V800 w/ GHS compiler
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




UE_PROJDIR:=$(subst \,\\,$(shell cygpath -w $(VLIB_ROOT)))
UE_PROJDIR_CYG:=$(shell cygpath '$(UE_PROJDIR)')

#Tooldirs must be absolute in UE16
#UE_TOOLDIR:=$(shell pwd | xargs realpath --relative-base=$(UE_PROJDIR_CYG) | xargs cygpath -w)
UE_TOOLDIR:=$(shell cygpath -w $(shell pwd))



UE_FILES_TMPFILE:="/tmp/ue_files.temp"
UE_TMPFILE:="/tmp/ue.temp"
UE_NAME:=$(VLIB_APP).prj

ALL_DIRS        := $(sort $(VLIB_INC) $(VPATH))
MAKE_FILES_FULL := $(subst Makefile,$(VLIB_APP_DIR)/Makefile,$(VLIB_MAKE_FILES))

RC_FILE:=$(subst \,\\,$(shell echo $(shell pwd)/$(VLIB_APP).rc | xargs realpath --relative-base=$(UE_PROJDIR_CYG) | xargs cygpath -w))
MAP_FILE:=$(subst out.rc,map,$(RC_FILE))

.PHONY : ultraedit

ultraedit :	
#	@# create a tmp file with all required pathes and files
ifeq ($(APP_STORE),)
	@echo -n  >$(UE_FILES_TMPFILE)
endif
#	@# TODO: Add filtering for empty directories
#	@#  For all directories  | Check if dir exists                              | Relative path to vlib root                       | Windows Path     |      | Remove dups | Change Line Ending
	@for line in $(ALL_DIRS); do if [ -e "$$line" ];then echo $$line; fi; done  | xargs realpath --relative-base=$(UE_PROJDIR_CYG) | xargs cygpath -w | sort | uniq        | tr -d '\r' | awk '{printf"%s\\\n",$$0}' >> $(UE_FILES_TMPFILE)
#	@#  For all makefiles    | Relative path to vlib                            | Windows Path     |      | Remove dups | Change Line Ending
	@echo $(MAKE_FILES_FULL) | xargs realpath --relative-base=$(UE_PROJDIR_CYG) | xargs cygpath -w | sort | uniq        | tr -d '\r' | awk '{printf"%s\n",$$0}' >> $(UE_FILES_TMPFILE)
#	@#  For RC and MAP file  | Change Line Ending
	@echo $(RC_FILE)         | tr -d '\r' | awk '{printf"%s\n",$$0}' >> $(UE_FILES_TMPFILE)
	@echo $(MAP_FILE)        | tr -d '\r' | awk '{printf"%s\n",$$0}' >> $(UE_FILES_TMPFILE)

#	@# create ultra edit project file
	@echo Creating ultraedit project file $(UE_NAME)
	@echo -n  >$(UE_TMPFILE)
	@echo '[Project ID]' >>$(UE_TMPFILE)
	@echo 'Signature=UE Proj: v.1' >>$(UE_TMPFILE)
	@echo '[Project Information]' >>$(UE_TMPFILE)
	@echo 'Use Relative Directory=1' >>$(UE_TMPFILE)
	@echo 'Relative to Project File=1' >>$(UE_TMPFILE)
	@echo 'Project Start=D607080005000B00100028002A00000057' >>$(UE_TMPFILE)
	@echo Project Directory=$(UE_PROJDIR) >>$(UE_TMPFILE)
	@echo 'Filter=*.c; *.h; *.cc; *.cpp; *.s; *.850; *.ld; *.asm; *.rc; *.nwire; *.ice; *.map; *.minicube' >>$(UE_TMPFILE)
	@echo 'Include Sub Directories=0' >>$(UE_TMPFILE)
	@echo 'Project Tagfile=' >>$(UE_TMPFILE)
	@echo 'Project Wordfile=' >>$(UE_TMPFILE)
	@echo 'Create Tagfile=1' >>$(UE_TMPFILE)
	@echo 'Working Time=6665' >>$(UE_TMPFILE)
	@echo '[Files]' >>$(UE_TMPFILE)
	
#	@# insert all files into project file
#	@#  For all files to be listed | Add dot to relative paths | Remove cygwin references      |      | Remove dups | Change Line Ending
	@cat $(UE_FILES_TMPFILE)       | sed 's/^.[^:]/.\\\0/'     | sed -r 's/\/cygdrive\/c/C\:/' | sort | uniq        | tr -d '\r' | awk '{printf"%d=%s\n",FNR,$$0}' >> $(UE_TMPFILE)
	@echo '[Folders]' >>$(UE_TMPFILE)
	
#	@# insert tool shortcuts
	
	@echo '[Tools]' >>$(UE_TMPFILE)
	@echo 'Tool Cmd0="$(shell cygpath -w $$(dirname $$(which $(CCD))))\multi -nosplash $(VLIB_APP)"' >>$(UE_TMPFILE)
	@echo 'Tool Dir0="$(UE_TOOLDIR)"' >>$(UE_TMPFILE)
	@echo 'Tool Menu0="Debug"' >>$(UE_TMPFILE)
	@echo 'Bitmap Path0="$(VLIB_WINPATH)\compiler\debug.bmp"' >>$(UE_TMPFILE)
	@echo 'Show DOS Window0=0' >>$(UE_TMPFILE)
	@echo 'Capture0=1' >>$(UE_TMPFILE)
	@echo 'Capture Mode0=3' >>$(UE_TMPFILE)
	@echo 'WinProg0=0' >>$(UE_TMPFILE)
	@echo 'SaveAllFiles0=1' >>$(UE_TMPFILE)
	@echo 'Save Active File0=1' >>$(UE_TMPFILE)
	@echo 'Replace Type0=0' >>$(UE_TMPFILE)

	@echo 'Tool ID1=-1' >>$(UE_TMPFILE)
	@echo 'Tool Cmd1="C:\cygwin\bin\make.exe -j4"' >>$(UE_TMPFILE)
	@echo 'Tool Dir1="$(UE_TOOLDIR)"' >>$(UE_TMPFILE)
	@echo 'Tool Menu1="Make"' >>$(UE_TMPFILE)
	@echo 'Bitmap Path1="$(VLIB_WINPATH)\compiler\make.bmp"' >>$(UE_TMPFILE)
	@echo 'Show DOS Window1=1' >>$(UE_TMPFILE)
	@echo 'Capture Mode1=3' >>$(UE_TMPFILE)
	@echo 'WinProg1=0' >>$(UE_TMPFILE)
	@echo 'SaveAllFiles1=1' >>$(UE_TMPFILE)
	@echo 'Replace Type1=0' >>$(UE_TMPFILE)
	@echo 'Capture1=1' >>$(UE_TMPFILE)
	@echo 'Save Active File1=1' >>$(UE_TMPFILE)

	@echo 'Tool ID2=-1' >>$(UE_TMPFILE)
	@echo 'Tool Cmd2="C:\cygwin\bin\make.exe clean"' >>$(UE_TMPFILE)
	@echo 'Tool Dir2="$(UE_TOOLDIR)"' >>$(UE_TMPFILE)
	@echo 'Tool Menu2="Make Clean"' >>$(UE_TMPFILE)
	@echo 'Bitmap Path2="$(VLIB_WINPATH)\compiler\clean.bmp"' >>$(UE_TMPFILE)
	@echo 'Show DOS Window2=0' >>$(UE_TMPFILE)
	@echo 'Capture Mode2=3' >>$(UE_TMPFILE)
	@echo 'WinProg2=0' >>$(UE_TMPFILE)
	@echo 'SaveAllFiles2=1' >>$(UE_TMPFILE)
	@echo 'Replace Type2=0' >>$(UE_TMPFILE)
	@echo 'Capture2=1' >>$(UE_TMPFILE)
	@echo 'Save Active File2=1' >>$(UE_TMPFILE)

	@echo 'Tool ID3=-1' >>$(UE_TMPFILE)
	@echo 'Tool Cmd3="make svnupdate QUIET=0"' >>$(UE_TMPFILE)
	@echo 'Tool Dir3="$(UE_TOOLDIR)"' >>$(UE_TMPFILE)
	@echo 'Tool Menu3="SVN Update"' >>$(UE_TMPFILE)
	@echo 'Bitmap Path3="$(VLIB_WINPATH)/compiler/turtle.bmp"' >>$(UE_TMPFILE)
	@echo 'Show DOS Window3=0' >>$(UE_TMPFILE)
	@echo 'Capture Mode3=3' >>$(UE_TMPFILE)
	@echo 'WinProg3=0' >>$(UE_TMPFILE)
	@echo 'SaveAllFiles3=1' >>$(UE_TMPFILE)
	@echo 'Replace Type3=0' >>$(UE_TMPFILE)
	@echo 'Capture3=1' >>$(UE_TMPFILE)
	@echo 'Save Active File3=1' >>$(UE_TMPFILE)

	@echo 'Tool ID4=-1' >>$(UE_TMPFILE)
	@echo 'Tool Cmd4="mintty.exe -e /bin/xhere /bin/bash.exe "$(UE_TOOLDIR)""' >>$(UE_TMPFILE)
	@echo 'Tool Dir4="$(UE_TOOLDIR)"' >>$(UE_TMPFILE)
	@echo 'Tool Menu4="Bash Prompt"' >>$(UE_TMPFILE)
	@echo 'Bitmap Path4="$(VLIB_WINPATH)/compiler/cmd.bmp"' >>$(UE_TMPFILE)
	@echo 'Show DOS Window4=0' >>$(UE_TMPFILE)
	@echo 'Capture Mode4=3' >>$(UE_TMPFILE)
	@echo 'WinProg4=1' >>$(UE_TMPFILE)
	@echo 'SaveAllFiles4=0' >>$(UE_TMPFILE)
	@echo 'Replace Type4=0' >>$(UE_TMPFILE)
	@echo 'Capture4=0' >>$(UE_TMPFILE)
	@echo 'Save Active File4=0' >>$(UE_TMPFILE)

#	@# copy to final project file
	@awk '{printf"%s\r\n",$$0}' <$(UE_TMPFILE) >$(UE_NAME)
	@rm -f $(UE_TMPFILE)
	@rm -f $(UE_FILES_TMPFILE)

