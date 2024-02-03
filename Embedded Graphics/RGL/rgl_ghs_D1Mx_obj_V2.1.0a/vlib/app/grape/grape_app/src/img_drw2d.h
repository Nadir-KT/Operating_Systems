/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: img_drw2d.h 11057 2016-11-10 08:23:04Z shinya.tomari $
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

#ifndef IMG_DRW2D_H_
#define IMG_DRW2D_H_

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Title: Image Format DRW2D Helper

  An application can use these functions to prepare images
  in the GRAPE image format for DRW2D.
*/

/***********************************************************
  Section: Global Functions
*/

void IMG_GetRect(const Img_t *Img, r_drw2d_Rect_t *Rect);
void IMG_GetTexture(const Img_t *Img, r_drw2d_Texture_t *Texture, r_drw2d_TextureFlags_t Flags);

void IMG_Blit(r_drw2d_Device_t Dev, const Img_t *Img, uint32_t X, uint32_t Y);

#endif /* IMG_DRW2D_H_ */
