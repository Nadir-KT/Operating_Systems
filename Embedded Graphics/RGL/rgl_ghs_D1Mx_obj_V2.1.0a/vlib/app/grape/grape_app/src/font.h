/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: font.h 11057 2016-11-10 08:23:04Z shinya.tomari $
============================================================================ 
DESCRIPTION
Sample program for demonstration of the D1X GFX features
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

#ifndef _FONT_H_
#define _FONT_H_

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Title: Font Interface
  
  This header describes the structure of a bitmap font,
  which can be used by the framework.
*/


/***********************************************************
  Section: Global Types
*/

/***********************************************************
  Type: FontPos_t

  Character position and width information.

  The type is used to specify the position of a character
  inside a font texture.

  Members:
  Offset  - Pixel position in the texture
  Width   - Pixel width in the texture
*/

typedef struct {
    int Offset;
    int Width;
} FontPos_t;


/***********************************************************
  Type: Font_t

  Font data type.

  This structure contains the data of a font.

  Members:
  Texture - Pointer to the font's image structure
  Pos     - Array of character information
  Height  - Pixel height of the font
*/

typedef struct {
    Img_t           *Texture;
    const FontPos_t *Pos;
    int              Height;
} Font_t;


#ifdef __cplusplus
}
#endif

#endif /* _FONT_H_ */

