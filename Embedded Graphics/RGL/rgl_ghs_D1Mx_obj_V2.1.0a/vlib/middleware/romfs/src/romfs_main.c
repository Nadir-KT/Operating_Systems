/*
****************************************************************************
PROJECT : RO file system
FILE    : $Id: romfs_main.c 11006 2016-11-07 16:32:44Z michael.golczewski $
============================================================================ 
DESCRIPTION
RO file system function.
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


/*******************************************************************************
  Title: File System Main Module 
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "romfs_api.h"
#include <string.h>

/*******************************************************************************
  Section: Local Defines
*/

/* Limits */
#define LOC_ROMFS_MAXOPEN        2

/*******************************************************************************
  Section: Local Variables
*/


static R_ROMFS_Data_t const *locRoot;
static R_ROMFS_File_t locFHandle[LOC_ROMFS_MAXOPEN];
static R_ROMFS_Dir_t  locDHandle[LOC_ROMFS_MAXOPEN];

/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_ROMFS_FOpen

  SR_: <romfs_api.h>
*/

R_ROMFS_File_t *R_ROMFS_FOpen(const char *Name, char *Mode) {
    int i;
    int j;

    /* Check if FS is initialized */
    if (locRoot == 0) {
        /* No valid pointer */
        return 0;
    }

    /* Search free handle */
    i = 0;
    while (i < LOC_ROMFS_MAXOPEN) {
        if (locFHandle[i].InUse == 0) {
            break;
        }
        i++;
    }
    if (i >= LOC_ROMFS_MAXOPEN) {
        /* No free entry */
        return 0;
    }

    /* Find file */
    j = 0;
    while (locRoot[j].Address) {
        if (strcmp(Name, locRoot[j].Name) == 0) {
            /* File found */
            locFHandle[i].InUse   = 1;
            locFHandle[i].Pos     = 0;
            locFHandle[i].Size    = locRoot[j].Size;
            locFHandle[i].Address = locRoot[j].Address;
            return &locFHandle[i];
        }
        j++;
    }
    /* File not found */
    return 0;
}


/*******************************************************************************
  Function: R_ROMFS_FClose

  See: <romfs_api.h>
*/

int32_t R_ROMFS_FClose(R_ROMFS_File_t *Fp) {
    if (Fp == 0) {
        return -1;
    }
    Fp->InUse = 0;
    return 0;
}


/*******************************************************************************
  Function: R_ROMFS_FRead

  See: <romfs_api.h>
*/

uint32_t R_ROMFS_FRead(void *Buffer, uint32_t Size, uint32_t Count, R_ROMFS_File_t *Fp) {
    uint32_t n;

    if (Fp == 0) {
        return 0;
    }
    if (Buffer == 0) {
        return 0;
    }
    n = Size * Count;
    if ((Fp->Pos + n) >= Fp->Size) {
        n = Fp->Size - Fp->Pos;
    }
    memcpy(Buffer, Fp->Address + Fp->Pos, n);
    Fp->Pos += n;
    n /= Size;
    return n;
}

/*******************************************************************************
  Function: R_ROMFS_GetFCount

  See: <romfs_api.h>
*/

uint32_t R_ROMFS_GetFCount() {
    uint32_t i;

    /* Check if FS is initialized */
    if (locRoot == 0) {
        /* No valid pointer */
        return 0;
    }

    /* Parse all files */
    i = 0;
    while (locRoot[i].Address) {
        i++;
    }
    return i;
}

/*******************************************************************************
  Function: R_ROMFS_DOpen

  See: <romfs_api.h>
*/

R_ROMFS_Dir_t *R_ROMFS_DOpen() {
    int i;

    /* Check if FS is initialized */
    if (locRoot == 0) {
        /* No valid pointer */
        return 0;
    }

    /* Search free handle */
    i = 0;
    while (i < LOC_ROMFS_MAXOPEN) {
        if (locDHandle[i].InUse == 0) {
            break;
        }
        i++;
    }
    if (i >= LOC_ROMFS_MAXOPEN) {
        /* No free entry */
        return 0;
    }

    /* Return first entry */    
    locDHandle[i].InUse   = 1;
    locDHandle[i].Pos     = 0;      
    locDHandle[i].Size    = R_ROMFS_GetFCount();     
    locDHandle[i].FName   = (char *)locRoot[0].Name;
    
    return &locDHandle[i];
}

/*******************************************************************************
  Function: R_ROMFS_DClose

  See: <romfs_api.h>
*/

int32_t R_ROMFS_DClose(R_ROMFS_Dir_t *Dp) {
    /* Check if directory handle is initialized */
    if (Dp == 0) {
        return -1;
    }
    Dp->InUse = 0;
    return 0;
}

/*******************************************************************************
  Function: R_ROMFS_DRead

  See: <romfs_api.h>
*/

uint32_t R_ROMFS_DRead(R_ROMFS_Dir_t *Dp) {

    uint32_t n;

    /* Check if FS is initialized */
    if (locRoot == 0) {
        /* No valid pointer */
        return 0;
    }
    /* Check if directory handle is initialized */
    if (Dp == 0) {
        return 0;
    }
    
    n = Dp->Size - Dp->Pos;
    
    if (n != 0){
        Dp->FName = (char *)locRoot[Dp->Pos].Name;
        Dp->Pos += 1;   
    }
    
    return n;
}

/*******************************************************************************
  Function: R_ROMFS_MMap

    See: <romfs_api.h>
*/

void *R_ROMFS_MMap(void *Addr, uint32_t Size, int32_t Prot,
                    int32_t Flags, R_ROMFS_File_t *Fp, uint32_t Offset) {
    return (Fp->Address + Offset);
}


/*******************************************************************************
  Function: R_ROMFS_MUnmap

    See: <romfs_api.h>
*/

int32_t R_ROMFS_MUnmap(void *addr, uint32_t Size) {
    return 0;
}


/*******************************************************************************
  Function: R_ROMFS_Init

  See: <romfs_api.h>
*/

int32_t  R_ROMFS_Init(const R_ROMFS_Data_t *PData) {
    locRoot = PData;
    return 0;
}
