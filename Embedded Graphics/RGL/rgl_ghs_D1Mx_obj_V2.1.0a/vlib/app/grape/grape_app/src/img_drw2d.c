/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: img_drw2d.c 11057 2016-11-10 08:23:04Z shinya.tomari $
============================================================================
DESCRIPTION
GRAPE (Graphics Application Environment)
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
  Section: Includes
*/

#include "r_typedefs.h"

#include "r_drw2d_os.h"
#include "r_drw2d_api.h"

#include "img_format.h"
#include "img_drw2d.h"

/***********************************************************
  Section: Local Functions
*/

uint32_t locIsSwizzled(const Img_t *Img)
{
    if((Img->Attributes & IMG_ATTRIBUTE_SWIZZLE) != 0)
    {
        return R_TRUE;
    }
    else
    {
        return R_FALSE;
    }
}

uint32_t locIsRLE(const Img_t *Img)
{
    if((Img->Attributes & IMG_ATTRIBUTE_RLE_COMPRESSED) != 0)
    {
        return R_TRUE;
    }
    else
    {
        return R_FALSE;
    }
}

r_drw2d_PixelFormat_t locGetPixelFormat(Img_ColorFormat_t ImgFormat)
{
    r_drw2d_PixelFormat_t Drw2dFormat = R_DRW2D_PIXELFORMAT_NONE;

    switch(ImgFormat)
    {
    case IMG_ALPHA8:
        Drw2dFormat = R_DRW2D_PIXELFORMAT_AL8;
        break;
    case IMG_RGB565:
        Drw2dFormat = R_DRW2D_PIXELFORMAT_RGB565;
        break;
    case IMG_ARGB8888:
        Drw2dFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
        break;
    case IMG_ARGB1555:
        Drw2dFormat = R_DRW2D_PIXELFORMAT_ARGB1555;
        break;
    case IMG_RGBA5551:
        Drw2dFormat = R_DRW2D_PIXELFORMAT_RGBA5551;
        break;
    case IMG_ARGB4444:
        Drw2dFormat = R_DRW2D_PIXELFORMAT_ARGB4444;
        break;
    case IMG_RGBA4444:
        Drw2dFormat = R_DRW2D_PIXELFORMAT_RGBA4444;
        break;
    default:
        /* Not supported */
        Drw2dFormat = R_DRW2D_PIXELFORMAT_NONE;
        break;
    }

    return Drw2dFormat;
}

/***********************************************************
  Section: Global Functions

  Description see: <img_drw2d.h>
*/


/***********************************************************
  Function: IMG_GetRect

  Returns size parameters for the given Img_t structure.

  Parameters:
  Img        - Pointer to the Img_t structure.
  Rect       - Pointer to the r_drw2d_Rect_t structure that will be filled with data.

  Returns:
  void
*/
void IMG_GetRect(const Img_t *Img, r_drw2d_Rect_t *Rect)
{
    Rect->Pos.X = R_DRW2D_2X(0);
    Rect->Pos.Y = R_DRW2D_2X(0);
    Rect->Size.Height = R_DRW2D_2X(Img->Height);
    Rect->Size.Width = R_DRW2D_2X(Img->Width);
}


/***********************************************************
  Function: IMG_GetTexture

  Returns texture data for a given Img_t structure

  Note that R_DRW2D_TEX_RLE and R_DRW2D_TEX_SWIZZLE flags are set automatically
  according to the given Img_t structure.

  Parameters:
  Img        - Pointer to the Img_t structure.
  Texture    - Pointer to the r_drw2d_Texture_t structure that will be filled with data
  Flags      - Additional texture drw2d flags (e.g. R_DRW2D_TEX_BILINEAR)

  Returns:
  void
*/
void IMG_GetTexture(const Img_t *Img, r_drw2d_Texture_t *Texture, r_drw2d_TextureFlags_t Flags)
{
    Texture->Buffer.Data = *Img->Data;
    Texture->Buffer.Pitch = Img->Width;
    Texture->Buffer.PixelFormat = locGetPixelFormat(Img->Colorformat);
    Texture->Buffer.Size.Height = Img->Height;
    Texture->Buffer.Size.Width = Img->Width;
    Texture->Flags = Flags;
    Texture->Handle = 0;

    if(locIsRLE(Img) == R_TRUE)
    {
        Texture->Flags |= R_DRW2D_TEX_RLE;
    }

    if(locIsSwizzled(Img) == R_TRUE)
    {
        Texture->Flags |= R_DRW2D_TEX_SWIZZLE;
    }
}


/***********************************************************
  Function: IMG_Blit

  Blits the image described by the given Img_t structure

  Parameters:
  Dev        - DRW2D Device Handle.
  Img        - Pointer to the Img_t structure.
  X          - X-Position for the blitted image's upper left edge (screen coordinate)
  Y          - Y-Position for the blitted image's upper left edge (screen coordinate)

  Returns:
  void
*/
void IMG_Blit(r_drw2d_Device_t Dev, const Img_t *Img, uint32_t X, uint32_t Y)
{
    r_drw2d_Rect_t dst_rect;
    r_drw2d_Rect_t src_rect;
    r_drw2d_Texture_t tex;

    if (Img->Data == 0)
    {
        return;
    }

    IMG_GetRect(Img, &src_rect);
    IMG_GetTexture(Img, &tex, R_DRW2D_TEX_NONE);

    dst_rect.Pos.X = R_DRW2D_2X(X);
    dst_rect.Pos.Y = R_DRW2D_2X(Y);
    dst_rect.Size = src_rect.Size;

    R_DRW2D_CtxFillMode(Dev, R_DRW2D_FILLMODE_TEXTURE);
    R_DRW2D_CtxTextureSet(Dev, 0, &tex);
    R_DRW2D_TextureBlit(Dev, &src_rect, &dst_rect);
}
