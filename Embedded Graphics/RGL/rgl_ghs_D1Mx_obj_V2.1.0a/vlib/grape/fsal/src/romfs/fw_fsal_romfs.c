/*
****************************************************************************
PROJECT : Grape - fsal
FILE    : $Id: fw_fsal_romfs.c 11006 2016-11-07 16:32:44Z michael.golczewski $
============================================================================ 
DESCRIPTION

============================================================================
                            C O P Y R I G H T                                    
============================================================================
                           Copyright (c) 2011
                                  by 
                       Renesas Electronics (Europe) GmbH. 
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
Purpose: only for testing

DISCLAIMER                                                                   
This software is supplied by Renesas Electronics Corporation and is only     
intended for use with Renesas products. No other uses are authorized. This   
software is owned by Renesas Electronics Corporation and is protected under  
all applicable laws, including copyright laws.                               
THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING  
THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT      
LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE   
AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.          
TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS       
ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE  
FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR   
ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE  
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.                             
Renesas reserves the right, without notice, to make changes to this software 
and to discontinue the availability of this software. By using this software,
you agree to the additional terms and conditions found by accessing the      
following link:                                                              
http://www.renesas.com/disclaimer *                                          
Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.

****************************************************************************
*/

/***********************************************************
  Title: File System Main Module 
*/

/***********************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "fw_fsal_api.h"
#include "romfs_api.h"
#include <string.h>


/***********************************************************
  Section: Local typedef
*/


/***********************************************************
  Section: Global Functions
*/

/***********************************************************
  Function: FW_FSAL_FOpen

  See: <fsal.h>
*/

void *FW_FSAL_FOpen(const int8_t *Name, int8_t *Mode) 
{
    R_ROMFS_File_t * ret;
    ret = (R_ROMFS_File_t *) R_ROMFS_FOpen ((const char*)Name, (char*)Mode);
    return (void *) ret;
}


/***********************************************************
  Function: FW_FSAL_FClose

  See: <fsal.h>
*/

int32_t FW_FSAL_FClose(void *Fptr) 
{
    return R_ROMFS_FClose((R_ROMFS_File_t *)Fptr);
}


/***********************************************************
  Function: FW_FSAL_FRead

  See: <fsal.h>
*/

uint32_t FW_FSAL_FRead(void *Buffer, uint32_t Size, uint32_t Count, void *Fptr)
{
    return R_ROMFS_FRead(Buffer, Size, Count, (R_ROMFS_File_t*) Fptr);
}


/***********************************************************
  Function: FW_FSAL_MMap

  See: <fsal.h>
*/

void *FW_FSAL_MMap(void *Addr, uint32_t Size, int32_t Prot,
                int32_t Flags, void *Fptr, uint32_t Offset) 
{
    return R_ROMFS_MMap(Addr, Size, Prot, Flags, (R_ROMFS_File_t*) Fptr, Offset);
}


/***********************************************************
  Function: FW_FSAL_MUnmap

  See: <fsal.h>
*/

int32_t FW_FSAL_MUnmap(void *addr, uint32_t Size) 
{
    return R_ROMFS_MUnmap(addr,Size);
}


/***********************************************************
  Function: FW_FSAL_Init

  See: <fsal.h>
*/

int32_t  FW_FSAL_Init(const void *PData) 
{
    return R_ROMFS_Init((R_ROMFS_Data_t*)PData);
}
