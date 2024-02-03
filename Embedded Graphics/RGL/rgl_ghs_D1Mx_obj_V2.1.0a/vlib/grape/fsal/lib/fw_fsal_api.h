/*
****************************************************************************
PROJECT : Grape - file system AL
FILE    : $Id: fw_fsal_api.h 11006 2016-11-07 16:32:44Z michael.golczewski $
============================================================================ 
DESCRIPTION
File system application Layer.
============================================================================
                            C O P Y R I G H T                                    
============================================================================
                           Copyright (c) 2011
                                  by 
                       Renesas Electronics (Europe) GmbH. 
                           Oberrather Str. 4
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

#ifndef R_FW_FSAL_API_H_
#define R_FW_FSAL_API_H_

#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************
  Title: FSAL
  
  File System Abstraction Layer
*/


/***********************************************************
  Section: Global Defines

  Definition of all global defines.
*/

/***********************************************************
  Section: Global Functions
*/

/***********************************************************
  Function: FW_FSAL_FOpen

  Open a file.

  Parameters:
  Name       - Filename
  
  Mode       - Modus (r,rw,...)

  Returns:
    pointer to File handler. 
*/

void *FW_FSAL_FOpen(const int8_t *Name, int8_t *Mode);


/***********************************************************
  Function: FW_FSAL_FClose

  Close a file.

  Parameters:
  Fptr         - File pointer

  Returns:
  ==0        - OK
  !=0        - NG
*/

int32_t FW_FSAL_FClose(void *Fptr);


/***********************************************************
  Function: FW_FSAL_FRead

  Read from a file.

  Parameters:
  Buffer     - Pointer to a buffer, where data is read to
  Size       - Byte size of a single element
  Count      - Number of elements to read from the file
  Fptr       - Pointer to a file handle

  Returns:
  Number of elements, which have been read
*/

uint32_t FW_FSAL_FRead(void *Buffer, uint32_t Size, uint32_t Count, void *Fptr);


/***********************************************************
  Function: FW_FSAL_MMap

  Creates a new mapping in the virtual address space of the calling
  process.
  
  Parameters:
    Addr       - TBD
    Size       - TBD
    Prot       - TBD
    Flags      - TBD
    Fptr         - Pointer to a file handle
    Offset     - TBD

  Returns:
  The address of the new mapping is returned as the result of the call.
*/

void *FW_FSAL_MMap(void *Addr, uint32_t Size, int32_t Prot, int32_t Flags, void *Fptr, uint32_t Offset);


/***********************************************************
  Function: FW_FSAL_MUnmap

  The system call deletes the mappings for the specified address range,
  and causes further references to addresses within the range to generate
  invalid memory references.
  
  Parameters:
  Addr       - TBD
  Size       - TBD

  Returns:
  On success returns 0, on failure -1
*/

int32_t FW_FSAL_MUnmap(void *addr, uint32_t Size);


/***********************************************************
  Function: eeRomFs_Init

  Init the file system.

  Parameters:
  Pdata      - Pointer to a file system strutcure

  Returns:
  ==0        - OK
  !=0        - NG
*/

int32_t FW_FSAL_Init(const void *PData);

/***********************************************************
  Function: FW_FSAL_GetSize

  Get the file size (in bytes)

  Parameters:
  Fptr         - Pointer to a file handle

  Returns:
  ==0        - OK
  !=0        - the file size
*/

int32_t FW_FSAL_GetSize(void *Fptr);

#ifdef __cplusplus
}
#endif

#endif /* R_FW_FSAL_API_H_ */
