/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: img.c 11057 2016-11-10 08:23:04Z shinya.tomari $
============================================================================ 
DESCRIPTION
Image Module
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2014
                                  by 
                       Renesas Electronics (Europe) GmbH. 
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
Purpose: only for testing, not for mass production

DISCLAIMER 

LICENSEE has read, understood and accepted the terms and conditions defined in
the license agreement, especially the usage rights. In any case, it is
LICENSEE's responsibility to make sure that any user of the software complies
with the terms and conditions of the signed license agreement.

SAMPLE CODE is not part of the licensed software, as such it must not be used in
mass-production applications. It can only be used for evaluation and
demonstration purposes at customer's premises listed in the signed license
agreement.
****************************************************************************
*/

/***********************************************************
  Title: Image Module

  The module provides functions to setup textures in the GPU 
  memory or to release the memory used by a texture.
*/

/***********************************************************
  Section: Includes
*/

#include "r_typedefs.h"         /* Renesas basic types, e.g. uint32_t */
#include "img_format.h"
#include "r_cdi_api.h"
#include "fw_fsal_api.h"
#include "error.h"
#include "r_cdi_api.h"
#include <string.h>

#include "img.h"


extern r_cdi_Heap_t loc_VRAM_heap;


/***********************************************************
  Section: Local Functions
*/


/***********************************************************
  Function: locDecompressRLE

  Unpack RLE data to destination address.

  Parameters:
  Dst        - video memory destination address
  Src        - Pointer to source data
  Size       - Size of compressed data
  Bpp        - Number of bytes per pixel

  Returns:
  void
*/

static void locDecompressRLE(uint32_t Dst, uint8_t *Src, uint32_t Size, uint8_t Bpp)
{
    uint8_t  pixelcounter;
    uint8_t  nextpixelrle;
    uint8_t  tmp;
    uint8_t* rlestart;

    while (Size > 0)
    {
        tmp = *Src;
        pixelcounter = (tmp & 0x7F) + 1;
        nextpixelrle = (tmp >> 7);
        Src++;
        Size -= 1;
        if (Size == 0)
        {
            break;
        }
        rlestart = Src;
        while (pixelcounter > 0)
        {
            if (nextpixelrle != 0)
            {
                /* if rle package repeat same pixel */
                Src = rlestart;
            }
            memcpy((uint8_t*) Dst, Src, Bpp);
            Dst += Bpp;
            Src += Bpp;
            pixelcounter -= 1;
            if ((nextpixelrle == 0)
                || ((nextpixelrle != 0) && (pixelcounter == 1)))
            {
                if (Size >= Bpp)
                {
                    Size -= Bpp;
                }
                else
                {
                    Size = 0;
                }
            }
        }
    }
}


/***********************************************************
  Section: Global Functions

  Description see: <img.h>
*/

/***********************************************************
  Function: Init2DImg
*/

void *Init2DImg(Img_t *Img)
{
    void     *data;
    void     *old;
    uint32_t  size;
    uint8_t   bpp;
    void     *fp;
    uint32_t  mm_data_addr;

    if (!Img)
    {
        return 0;
    }
    
    if (Img->Attributes & IMG_ATTRIBUTE_USE_FROM_PERSISTENT_MEMORY)
    {
        /* don't copy image to video memory,
            directly use it from persistent memory (Flash) */

        if ((Img->Attributes & IMG_ATTRIBUTE_RLE_DECOMPRESS) != 0)
        {
            /* Cannot decompress RLE image without using RAM */
            return 0;
        }

        if (*Img->Data == 0)
        {  /* check, if ROMFS is to be used */
            /* use ROMFS */
            fp = FW_FSAL_FOpen((const int8_t*)Img->Name, "rb");
            if (!fp)
            {
                return 0; 
            }
            mm_data_addr = (uint32_t)FW_FSAL_MMap(0, 0, 0, 0, fp, 0);
            if (!mm_data_addr)
            {
                return 0;
            }          
            FW_FSAL_MUnmap((void*)mm_data_addr, 0);
            FW_FSAL_FClose(fp);
            /* simply return image data address (read from ROMFS) */
            old          = *Img->Data;
            *Img->Data   = (void*)mm_data_addr;
        }
        else
        {
            /* simply return image data address */
            old          = *Img->Data;
        }
    }
    else
    {
        /* copy image data to video memory */
    
       /* check colourformat and calc bpp */
       switch (Img->Colorformat)
       {
           case IMG_ARGB8888:
           case IMG_RGBA8888:
               bpp = 4;
               break;
           
           case IMG_RGBX8888:
               /* RGB888 is a 4 byte aligned */
               bpp = 4;
               break;

           case IMG_ARGB6666:
           case IMG_RGBA6666:
           case IMG_RGB888:
               /* RGB888 is a 3 byte format w/o 4byte alignment when RLE compressed */
               bpp = 3;
               break;
               
           case IMG_ARGB1555:
           case IMG_RGBA5551:
           case IMG_ARGB4444:
           case IMG_RGBA4444:
           case IMG_RGB565:
               bpp = 2;
               break;
               
           case IMG_ALPHA8:
               bpp = 1;
               break;
               
           default:
               Error(ERR_TEXTURE_SETUP_FAILED);
               return *Img->Data;
       }
       
       /* calc image size (decompressed) */
       size = (Img->Height * Img->Width * bpp);
          
       data = (void*)R_CDI_Alloc(&loc_VRAM_heap, size);
       if (!data)
       {
           Error(ERR_TEXTURE_SETUP_FAILED);
           return *Img->Data;
       }
       
       if ((Img->Attributes & IMG_ATTRIBUTE_RLE_DECOMPRESS) != 0)
       {
           /* rle decompress image by CPU, only if image is of rle format
              and IMG_MODE_RLE flag is not set (means the image should not be
              decompressed by the Drawing engine during runtime */
           if (*Img->Data == 0)
           {  /* check, if ROMFS is to be used */
               /* use ROMFS */
               fp = FW_FSAL_FOpen((const int8_t*)Img->Name, "rb");
               if (!fp)
               {
                   R_CDI_Free((uint32_t)data, &loc_VRAM_heap);
                   return 0; 
               }
               mm_data_addr = (uint32_t)FW_FSAL_MMap(0, 0, 0, 0, fp, 0);
               if (!mm_data_addr)
               {
                   R_CDI_Free((uint32_t)data, &loc_VRAM_heap);
                   return 0;
               }
               locDecompressRLE((uint32_t)data, (uint8_t*)mm_data_addr, Img->DataLength, bpp);
               FW_FSAL_MUnmap((void*)mm_data_addr, 0);
               FW_FSAL_FClose(fp);
           }
           else
           {
               locDecompressRLE((uint32_t)data, *Img->Data, Img->DataLength, bpp);
           }
       }
       else
       {
           if (*Img->Data == 0)
           {  /* check, if ROMFS is to be used */
               /* use ROMFS */
               fp = FW_FSAL_FOpen((const int8_t*)Img->Name, "rb");
               if (!fp)
               {
                   R_CDI_Free((uint32_t)data, &loc_VRAM_heap);
                   return 0; 
               }
               mm_data_addr = (uint32_t)FW_FSAL_MMap(0, 0, 0, 0, fp, 0);
               if (!mm_data_addr)
               {
                   R_CDI_Free((uint32_t)data, &loc_VRAM_heap);
                   return 0;
               }
               /*PF_D2DSendToVidMemSync(Dev, data, (void*)mm_data_addr, Img->DataLength);*/
               memcpy(data, (void*)mm_data_addr, Img->DataLength);
               FW_FSAL_MUnmap((void*)mm_data_addr, 0);
               FW_FSAL_FClose(fp);
           }
           else
           {
               /*PF_D2DSendToVidMemSync(Dev, data, (void*)*Img->Data, Img->DataLength);*/
               memcpy(data, (void*)*Img->Data, Img->DataLength);
           }
       }
       old        = *Img->Data;
       *Img->Data = data;
    }
    return old;
}


/***********************************************************
  Function: Deinit2DImg
*/

void Deinit2DImg(Img_t *Img, void *Original)
{
    uint32_t err;
    
    if (!Img)
    {
        return;
    }

    if (Img->Attributes & IMG_ATTRIBUTE_USE_FROM_PERSISTENT_MEMORY)
    {
        /* don't copy image to video memory,
            directly use it from persistent memory (Flash) */
        /*    -> nothing to free */
    }
    else
    {
        /* copy image to video memory */
        /*    -> video memory can be freed now */
        err = R_CDI_Free((uint32_t)*Img->Data, &loc_VRAM_heap);
        if (err)
        {
            Error(err);
        }
    }
    *Img->Data = Original;
}
