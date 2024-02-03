/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: img.h 11057 2016-11-10 08:23:04Z shinya.tomari $
============================================================================ 
DESCRIPTION
Header for Texture Module
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

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Title: Image Module Interface

  An application can use these functions to setup textures
  in the GPU memory or release the memory used by a texture.
*/

/***********************************************************
  Section: Global Functions
*/

/***********************************************************
  Function: InitImg

  Prepare an image for drawing engine usage.

  If an image is located in the CPU memory, copy it to the 
  video memory. After the copy operation or if the image is
  already in the GPU memory (e.g. flash), adjust the pointer
  in the image structure.

  Parameters:
  Img        - Image structure

  Returns:
  Original image data pointer
*/

void *Init2DImg(Img_t *Img);


/***********************************************************
  Function: Deinit2DImg

  Remove an image from the GPU memory.

  If an image is located in the GPU memory, the GPU memory
  will be released.

  Parameters:
  Img        - Image structure
  Original   - Pointer to the original image data (e.g. CPU flash)

  Returns:
  void
*/

void Deinit2DImg(Img_t *Img, void *Original);


#ifdef __cplusplus
}
#endif

#endif /* _TEXTURE_H_ */

