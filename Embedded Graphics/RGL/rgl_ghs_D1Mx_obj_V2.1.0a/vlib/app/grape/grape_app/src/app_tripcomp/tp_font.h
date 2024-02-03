/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: tp_font.h 11057 2016-11-10 08:23:04Z shinya.tomari $
============================================================================ 
DESCRIPTION
Write Module Interface
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

#ifndef _TP_FONT_H_
#define _TP_FONT_H_


#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Title: Write Module Interface
  
  This header is the interface to the DRW2D based bitmap
  write functions.
*/


/***********************************************************
  Section: Global Functions
*/

/***********************************************************
  Function: SelectFont

  Select font for operation.

  The 'write' module uses an internal pointer to the font,
  which is used by any text operation. This function sets
  that pointer.

  Parameters:
  Font       - Font to be used

  Returns:
  void
*/

void TP_FONT_SelectFont(const Font_t *Font);


/***********************************************************
  Function: GetFontHeight

  Get height of the font.

  The function returns the height in pixel of the selected
  font.

  Parameters:
  void

  Returns:
  Height of font.
*/

int TP_FONT_GetFontHeight(void);

void TP_FONT_SetFontHeight(int32_t Height);


/***********************************************************
  Function: GetCharWidth

  Get character width.

  The function returns the width of a character in pixel
  when using the currently selected font.

  Parameters:
  Character  - ASCII value of the character

  Returns:
  Character width in pixel.
*/

int TP_FONT_GetCharWidth(char Character);


/***********************************************************
  Function: GetTextWidth

  Return width of a text.

  The function returns the width of a text in pixel
  when using the currently selected font.

  Parameters:
  Text       - Pointer to a 0 terminated string

  Returns:
  Text width in pixel
*/

int TP_FONT_GetTextWidth(char *Text);


/***********************************************************
  Function: GetTextHeight

  Return height of a text.

  The function returns the height of a text in pixel
  when using the currently selected font.

  Parameters:
  Text       - Pointer to a 0 terminated string

  Returns:
  Text height in pixel
*/

int TP_FONT_GetTextHeight(char *Text);


/***********************************************************
  Function: Write

  Write a text.

  Use the currently selected buffer, font and color to draw
  a text at the given position.

  Parameters:
  Dev        - R_DRW2D handle
  PosX       - Display X position
  PosY       - Display Y position
  Color      - Color to multiply with the font texture.
  Text       - Pointer to a 0 terminated string

  Returns:
  void
*/

void TP_FONT_Print(
    r_drw2d_Device_t Dev,
    int              PosX,
    int              PosY,
    r_drw2d_Color_t  Color,
    char             *Text
);


 
#ifdef __cplusplus
}
#endif

#endif /* _WRITE_H_ */

