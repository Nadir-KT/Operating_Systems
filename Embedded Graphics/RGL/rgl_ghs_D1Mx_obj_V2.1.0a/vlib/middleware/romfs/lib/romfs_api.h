/*
****************************************************************************
PROJECT : RO file system
FILE    : $Id: romfs_api.h 11006 2016-11-07 16:32:44Z michael.golczewski $
============================================================================ 
DESCRIPTION
Simple main function.
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

Warranty Disclaimer

Because the Product(s) is licensed free of charge, there is no warranty 
of any kind whatsoever and expressly disclaimed and excluded by Renesas, 
either expressed or implied, including but not limited to those for 
non-infringement of intellectual property, merchantability and/or 
fitness for the particular purpose. 
Renesas shall not have any obligation to maintain, service or provide bug 
fixes for the supplied Product(s) and/or the Application.

Each User is solely responsible for determining the appropriateness of 
using the Product(s) and assumes all risks associated with its exercise 
of rights under this Agreement, including, but not limited to the risks 
and costs of program errors, compliance with applicable laws, damage to 
or loss of data, programs or equipment, and unavailability or 
interruption of operations.

Limitation of Liability

In no event shall Renesas be liable to the User for any incidental, 
consequential, indirect, or punitive damage (including but not limited 
to lost profits) regardless of whether such liability is based on breach 
of contract, tort, strict liability, breach of warranties, failure of 
essential purpose or otherwise and even if advised of the possibility of 
such damages. Renesas shall not be liable for any services or products 
provided by third party vendors, developers or consultants identified or
referred to the User by Renesas in connection with the Product(s) and/or the 
Application.

****************************************************************************
*/

#ifndef R_ROMFS_H_
#define R_ROMFS_H_

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
  Title: ROM File System Interface
*/


/*******************************************************************************
  Section: Global Defines

  Definition of all global defines.
*/


/* Limits */
#define EE_ROMFS_MAXNAMELENGTH  256


/*******************************************************************************
  Section: Global Types

  Global types of the main module are defined here.
*/


typedef struct {
  uint32_t  Pos;
  uint32_t  Size;
  char     *Address;
  char      InUse;
} R_ROMFS_File_t;

typedef struct {
  uint32_t  Pos;
  uint32_t  Size;
  char     *FName;
  char      InUse;  
} R_ROMFS_Dir_t;

typedef struct {
  char        Name[EE_ROMFS_MAXNAMELENGTH];
  uint32_t    Size;
  char       *Address;
} R_ROMFS_Data_t;

/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_ROMFS_FOpen

  Open a file.

  Parameters:
  Name       - Filename
  Mode       - Modus (r,rw,...)

  Returns:
  ==0        - Cannot open File
  !=0        - Pointer to file handle
*/

R_ROMFS_File_t *R_ROMFS_FOpen(const char *Name, char *Mode);


/*******************************************************************************
  Function: R_ROMFS_FClose

  Close a file.

  Parameters:
  Fp         - File pointer

  Returns:
  ==0        - OK
  !=0        - NG
*/

int32_t R_ROMFS_FClose(R_ROMFS_File_t *Fp);


/*******************************************************************************
  Function: R_ROMFS_FRead

  Read from a file.

  Parameters:
  Buffer     - Pointer to a buffer, where data is read to
  Size       - Byte size of a single element
  Count      - Number of elements to read from the file
  Fp         - Pointer to a file handle

  Returns:
  Number of elements, which have been read
*/

uint32_t R_ROMFS_FRead(void *Buffer, uint32_t Size, uint32_t Count, R_ROMFS_File_t *Fp);

/*******************************************************************************
  Function: R_ROMFS_GetFCount
  
  Get number of files in current directory. 
  Current implementation only supports one directory (root).
  
  Parameters:  
  
  Returns:
  Number of files in current directory.
*/

uint32_t R_ROMFS_GetFCount(void);

/*******************************************************************************
  Function: R_ROMFS_DOpen
  
  Opens root directory.
  
  Parameters:
  
  Returns:
  ==0        - Cannot open root directory
  !=0        - Pointer to directory handle
*/

R_ROMFS_Dir_t *R_ROMFS_DOpen(void);

/*******************************************************************************
  Function: R_ROMFS_DClose

  Closes root directory.
    
  Parameters:
  Dp         - Directory pointer

  Returns:
  ==0        - OK
  !=0        - NG
*/

int32_t R_ROMFS_DClose(R_ROMFS_Dir_t *Dp);

/*******************************************************************************
  Function: R_ROMFS_DRead

  Updates directory with next file entry.

  Parameters:
  Dp         - Directory pointer

  Returns:
  Remaining number of directory entries.
*/

uint32_t R_ROMFS_DRead(R_ROMFS_Dir_t *Dp);

/*******************************************************************************
  Function: R_ROMFS_MMap

  Creates a new mapping in the virtual address space of the calling
  process.
  
  Parameters:
  Addr       - TBD
  Size       - TBD
  Prot       - TBD
  Flags      - TBD
  Fp         - Pointer to a file handle
  Offset     - TBD

  Returns:
  The address of the new mapping is returned as the result of the call.
*/

void *R_ROMFS_MMap(void *Addr, uint32_t Size, int32_t Prot, int32_t Flags, R_ROMFS_File_t *Fp, uint32_t Offset);


/*******************************************************************************
  Function: R_ROMFS_MUnmap

  The system call deletes the mappings for the specified address range,
  and causes further references to addresses within the range to generate
  invalid memory references.
  
  Parameters:
  Addr       - TBD
  Size       - TBD

  Returns:
  On success returns 0, on failure -1
*/

int32_t R_ROMFS_MUnmap(void *addr, uint32_t Size);


/*******************************************************************************
  Function: R_ROMFS_Init

  Init the file system.

  Parameters:
  Pdata      - Pointer to a file system strutcure

  Returns:
  ==0        - OK
  !=0        - NG
*/

int32_t  R_ROMFS_Init(const R_ROMFS_Data_t *PData);


#ifdef __cplusplus
}
#endif

#endif /* R_ROMFS_H_ */
