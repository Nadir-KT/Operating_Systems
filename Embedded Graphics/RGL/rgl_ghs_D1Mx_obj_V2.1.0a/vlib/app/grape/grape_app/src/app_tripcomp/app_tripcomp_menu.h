/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: app_tripcomp_menu.h 11057 2016-11-10 08:23:04Z shinya.tomari $
============================================================================
DESCRIPTION
Sample program for demonstration of the D1MX GFX features
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

#ifndef _APP_TRIPCOMP_MENU_H_
#define _APP_TRIPCOMP_MENU_H_


/*******************************************************************************
  Section: Struct forward declarations
*/

typedef struct tp_menu_Element_t tp_menu_Element_t;


/*******************************************************************************
  Section: Enumerations
*/

/*******************************************************************************
  Enum: tp_gui_Input_t

  TP GUI input specifiers.

  Values:
  TP_GUI_INPUT_KNOB_LEFT  - Knob has been turned to the left.
  TP_GUI_INPUT_KNOB_RIGHT - Knob has been turned to the right.
  TP_GUI_INPUT_KNOB_PRESS - Knob has been pressed.
*/
typedef enum
{
    TP_GUI_INPUT_KNOB_LEFT,
    TP_GUI_INPUT_KNOB_RIGHT,
    TP_GUI_INPUT_KNOB_PRESS,
} tp_gui_Input_t;


/*******************************************************************************
  Section: Callback types
*/

/******************************************************************************
  Callback function type: TP_OnFocus

  A callback function that will bring the tool tip of the
  selected element to the screen.

  Parameters:
   ToolTip - A tool tip string.
  Returns:
   void
*/
typedef void (*TP_ShowToolTip)(char *ToolTip);


/******************************************************************************
  Callback function type: TP_OnFocus

  Called when an element's focus changes.

  Parameters:
   Element - The affected element.
   Focus   - 1 if the element gained focus, 0 if it lost focus.
  Returns:
   void
*/
typedef void (*TP_OnFocus)(tp_menu_Element_t *Element, int32_t Focus);


/******************************************************************************
  Callback function type: TP_OnFocus

  Called when an element's focus changes.

  Parameters:
   Element - The affected element.
   Focus   - 1 if the element gained focus, 0 if it lost focus.
  Returns:
   void
*/
typedef void (*TP_OnVBlank)(tp_menu_Element_t *Element);


/******************************************************************************
  Callback function type: TP_OnInput

  Called when an input event for the element occurred.

  Parameters:
   Element - The affected element.
   Input   - The input specifier.

  Returns:
   0 - If the input was not handled by the element and should be handled by a
       higher element.
   1 - If the input was handled by the element and no further processing of
       the input is needed.
*/
typedef int32_t (*TP_OnInput)(tp_menu_Element_t *Element, tp_gui_Input_t Input);


/*******************************************************************************
  Section: Structures
*/

/*******************************************************************************
  Typedef: tp_menu_Element_t

  Structure describing an menu element of TP GUI.

  Members:
   Next                - Internal: Next element in the list.
   SpriteConf          - Configuration of the underlying sprite.
   AddressNormal       - Address of the lowlighted element image.
   AddressHighlighted  - Address of the highlighted element image.
   AddressAnimation    - Array with addresses of the elements animation frames.
                         (Must be 0 if the element is not animated)
   AnimationFrameCount - Number of frame in the animation frame array (see
                         AddressAnimation).
   AnimationCurrFrame  - Internal: Currently shown frame.
   AnimationSpeed      - Number of milliseconds between two frames (0 stops
                         animation, 1 is the fastest speed).
   IsFocusable         - 1 if element can gain the focus, 0 otherwise.
   OnFocus             - Callback on changes of the element's focus.
   OnVBlank            - Callback on VBlank interrupts.
   OnInput             - Callback on user interface inputs.
   Tip                 - Tool tip string for the element.
*/
struct tp_menu_Element_t
{
    tp_menu_Element_t      *Next;
    fw_vovial_Sprite_t     *SpriteConf;
    uint32_t                AddressNormal;
    uint32_t                AddressHighlighted;
    uint32_t               *AddressAnimation;
    uint32_t                AnimationFrameCount;
    uint32_t                AnimationCurrFrame;
    uint32_t                AnimationSpeed;
    int32_t                 IsFocusable;
    TP_OnFocus              OnFocus;
    TP_OnVBlank             OnVBlank;
    TP_OnInput              OnInput;
    char                   *Tip;
};


/*******************************************************************************
  Section: API function declarations

  See "app_tripcomp_menu.c" for more infos.
*/

void TP_GUI_Init(void);
void TP_GUI_NotifyVBlank(void);
void TP_GUI_MenuAddElement(tp_menu_Element_t *Element);
tp_menu_Element_t *TP_GUI_MenuGetSelectedElement(void);
void TP_GUI_MenuHighlightElement(tp_menu_Element_t *e);
void TP_GUI_MenuLowlightElement(tp_menu_Element_t *e);
void TP_GUI_MenuEnableElement(tp_menu_Element_t *e);
void TP_GUI_MenuDisableElement(tp_menu_Element_t *e);
void TP_GUI_MenuMoveElement(tp_menu_Element_t *Element, uint32_t PosX,  uint32_t PosY, uint32_t PosZ);
void TP_GUI_Show(void);
tp_menu_Element_t *TP_GUI_MenuMoveFocus(int32_t Dir);
void TP_GUI_ProcessInput(tp_gui_Input_t Input);
void TP_GUI_RegisterShowToolTip(TP_ShowToolTip ShowToolTip);
void TP_GUI_TearDown(void);

#endif /* _APP_TRIPCOMP_MENU_H_ */
