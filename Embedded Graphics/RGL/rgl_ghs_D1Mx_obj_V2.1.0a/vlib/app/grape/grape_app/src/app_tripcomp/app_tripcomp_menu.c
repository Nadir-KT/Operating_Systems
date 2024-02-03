/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: app_tripcomp_menu.c 11057 2016-11-10 08:23:04Z shinya.tomari $
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

/***********************************************************
  Title: Test Application Module
*/

/***********************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_config_d1x.h"

#include "r_ddb_api.h"
#include "fw_osal_api.h"
#include "fw_vovial_api.h"

#include "main.h"
#include "error.h"

#include "app_tripcomp_menu.h"

/***********************************************************
  Section: Local Defines
*/


/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Variable: locElementRoot

  The root element of the tp_menu_Element_t object list,
  containing the present elements of the menu.
*/
static tp_menu_Element_t *locElementRoot;


/***********************************************************
  Variable: locCurrElement

  Index of the currently selected element in the list (see
  locElementRoot).
*/
static int32_t locCurrElement = 0;


/***********************************************************
  Variable: locElementCount

  Number of element objects in the list (see
  locElementRoot).
*/
static int32_t locElementCount = 0;


/***********************************************************
  Variable: locShowToolTip

  A callback function that will bring the tool tip of the
  selected element to the screen. (See
  tp_menu_Element_t.Tip)
*/
static TP_ShowToolTip locShowToolTip;


/***********************************************************
  Section: Local Functions
*/


/******************************************************************************
  Function: locDefaultOnFocus

  Default implementation of an tp_menu_Element_t object's OnFocus callback
  function.
  The default implementation will swap the element's image against its
  highlighted (tp_menu_Element_t.AddressHighlighted) counterpart when the
  element gains focus.
  The action is reversed when the element looses focus.

  Parameters:
   Element - The element, which focus state changed.
   Focus   - 1 if the element has gained focus, 0 if focus has been lost.

  Returns:
   void
*/
static void locDefaultOnFocus(tp_menu_Element_t *Element, int32_t Focus)
{
    if(0 != Element->AddressHighlighted)
    {
        if(Focus == 1)
        {
            FW_VOVIAL_SetSpriteBuffer(Element->SpriteConf, (void*) Element->AddressHighlighted);
        }
        else
        {
            FW_VOVIAL_SetSpriteBuffer(Element->SpriteConf, (void*) Element->AddressNormal);
        }
    }
}


/******************************************************************************
  Function: locDefaultOnVBlank

  Default implementation of an tp_menu_Element_t object's OnVBlank callback
  function.
  The default implementation does nothing.

  Parameters:
   Element - The element, whose OnVBlank callback has been called.

  Returns:
   void
*/
static void locDefaultOnVBlank(tp_menu_Element_t *Element)
{

}

/******************************************************************************
  Function: locDefaultOnInput

  Default implementation of an tp_menu_Element_t object's OnInput callback
  function.
  The default implementation does nothing. Handling of the input is yielded to
  the next higher element by returning 0.

  Parameters:
   Element - The element, whose OnInput callback has been called.
   Input   - The input value that should be processed.

  Returns:
   0
*/
static int32_t locDefaultOnInput(tp_menu_Element_t *Element, tp_gui_Input_t Input)
{
    return 0;
}

/******************************************************************************
  Function: locAnimate

  Animates the given element according to its AnimationSpeed,
  AnimationCurrFrame and AddressAnimation fields by setting the underlying
  sprite's buffer to the next animation frame if necessary.
  Make sure, that the given element is an animated element with
  AddressAnimation != 0.

  The element is animated depending on an internal hardware timer to become
  (almost) independent of the CPU's current load.

  Parameters:
   Element - The element, that will be animated.

  Returns:
   void
*/
static void locAnimate(tp_menu_Element_t *Element)
{
    static uint32_t Time = 0;

    if(0 != Element->AnimationSpeed)
    {
        Time = FW_OSAL_TimeGet();

        Element->AnimationCurrFrame =   (Time / Element->AnimationSpeed)
                                      % Element->AnimationFrameCount;

        FW_VOVIAL_SetSpriteBuffer(Element->SpriteConf,
                     (void*) Element->AddressAnimation[Element->AnimationCurrFrame]);
    }
}


/******************************************************************************
  Function: locElementListAdd

  Adds the given element to the TP_GUI internal element list.

  Parameters:
   Element - The element, that will be added.

  Returns:
   void
*/
static void locElementListAdd(tp_menu_Element_t *Element)
{
    tp_menu_Element_t **root;

    root = &locElementRoot;

    while(*root != 0)
    {
        root = &((*root)->Next);
    }

    (*root) = Element;
    Element->Next = 0;
}


/******************************************************************************
  Function: locElementListGet

  Returns the element with the given index from the TP_GUI internal element
  list.

  Parameters:
   Idx - The index of the element which will be returned.

  Returns:
   A pointer to the tp_menu_Element_t object with the given index.
*/
static tp_menu_Element_t *locElementListGet(int32_t Idx)
{
    int32_t i = 0;
    tp_menu_Element_t *e = locElementRoot;

    while((i < Idx) && (e != 0))
    {
        e = e->Next;
        ++i;
    }

    return e;
}


/******************************************************************************
  Function: locSpriteHighlight

  Swaps the element's underlying sprite's image with the element's highlighted
  image.

  Parameters:
   Element - The element, that will be shown as highlighted.

  Returns:
   void
*/
static void locSpriteHighlight(tp_menu_Element_t *Element)
{
    FW_VOVIAL_SetSpriteBuffer(Element->SpriteConf,
                              (void*) Element->AddressHighlighted);
}


/******************************************************************************
  Function: locSpriteHighlight

  Swaps the element's underlying sprite's highlighted image with the element's
  lowlight image.

  Parameters:
   Element - The element, that will be shown as "lowlighted".

  Returns:
   void
*/
static void locSpriteLowlight(tp_menu_Element_t *Element)
{
    FW_VOVIAL_SetSpriteBuffer(Element->SpriteConf,
                              (void*) Element->AddressNormal);
}


/***********************************************************
  Section: Global Functions
*/


/******************************************************************************
  Function: TP_GUI_NotifyVBlank

  Notifies the TP_GUI framework that a VBlank interrupt occured. This MUST
  be called on every VBlank to enable the OnVBlank callback and animation
  functionality.

  Parameters:
   void

  Returns:
   void
*/
void TP_GUI_NotifyVBlank(void)
{
    tp_menu_Element_t *e;

    e = locElementRoot;
    while(0 != e)
    {
        if(e->AddressAnimation != 0)
        {
            locAnimate(e);
        }
        e->OnVBlank(e);
        e = e->Next;
    }
}


/******************************************************************************
  Function: TP_GUI_Init

  Initializes the TP_GUI framework. This function MUST be called before using
  any of the framework's functionality in order to reset internal parameters.

  Parameters:
   void

  Returns:
   void
*/
void TP_GUI_Init(void)
{
    locElementRoot  = 0;
	locElementCount = 0;
    locCurrElement  = 0;
    locShowToolTip  = 0;
}


/******************************************************************************
  Function: TP_GUI_TearDown

  Tears the TP_GUI framework down. This function MUST be called to shut down
  the framework and free the occupied ressources.

  Parameters:
   void

  Returns:
   void
*/
void TP_GUI_TearDown(void)
{
    tp_menu_Element_t *e = locElementRoot;

    while(e != 0)
    {
        FW_VOVIAL_DeleteSprite(e->SpriteConf);

        e = e->Next;
    }
}


/******************************************************************************
  Function: TP_GUI_MenuAddElement

  Adds the given element to the TP_GUI element list.

  Parameters:
   Element - The element, that will be added.

  Returns:
   void
*/
void TP_GUI_MenuAddElement(tp_menu_Element_t *Element)
{
    locElementListAdd(Element);

    if(0 == Element->OnFocus)
    {
        Element->OnFocus = locDefaultOnFocus;
    }

    if(0 == Element->OnVBlank)
    {
        Element->OnVBlank = locDefaultOnVBlank;
    }

    if(0 == Element->OnInput)
    {
        Element->OnInput = locDefaultOnInput;
    }


    FW_VOVIAL_CreateSprite(Element->SpriteConf);

    ++locElementCount;
}


/******************************************************************************
  Function: TP_GUI_Show

  Enables all currently added elements.

  Parameters:
   void

  Returns:
   void
*/
void TP_GUI_Show(void)
{
    tp_menu_Element_t *e = locElementRoot;

    locCurrElement = 0;
    if(0 != e)
    {
        locSpriteHighlight(e);
    }

    while(0 != e)
    {
        FW_VOVIAL_EnableSprite(e->SpriteConf);
        e = e->Next;
    }
}


/******************************************************************************
  Function: TP_GUI_ProcessInput

  Processes the given input by delegating it to the element on focus. If the
  element declines handling of the input, the TP_GUI default processing is
  used.
  The framework calls the OnInput callback function of the element on focus.

  Parameters:
   Input - Input specifier.

  Returns:
   void
*/
void TP_GUI_ProcessInput(tp_gui_Input_t Input)
{
    int32_t OnInputRet;
    tp_menu_Element_t *e;
    e = locElementListGet(locCurrElement);

    OnInputRet = e->OnInput(e, Input);

    if(0 == OnInputRet)
    {
        switch(Input)
        {
        case TP_GUI_INPUT_KNOB_LEFT:
            TP_GUI_MenuMoveFocus(-1);
            break;
        case TP_GUI_INPUT_KNOB_RIGHT:
            TP_GUI_MenuMoveFocus(1);
            break;
        case TP_GUI_INPUT_KNOB_PRESS:
            break;
        }
    }
    else
    {

    }
}


/******************************************************************************
  TODO: DEPRECATED !??!?
*/
tp_menu_Element_t *TP_GUI_MenuMoveFocus(int32_t Dir)
{
    tp_menu_Element_t *e;
    e = locElementListGet(locCurrElement);
    e->OnFocus(e, 0);

    do
    {
        if(Dir < 0)
        {
            Dir = locElementCount - ((-1*Dir) % locElementCount);
        }
        locCurrElement = (locCurrElement + Dir) % locElementCount;
        e = locElementListGet(locCurrElement);
    }
    while(0 == e->IsFocusable);

    e->OnFocus(e, 1);

    if(0 != locShowToolTip)
    {
        locShowToolTip(e->Tip);
    }

    return e;
}


/******************************************************************************
  Function: TP_GUI_MenuHighlightElement

  Swaps the element's image with its highlighted counterpart.

  Parameters:
   Element - The element, that will be shown as highlighted.

  Returns:
   void
*/
void TP_GUI_MenuHighlightElement(tp_menu_Element_t *e)
{
    if(e->AddressHighlighted != 0)
    {
        locSpriteHighlight(e);
    }
}


/******************************************************************************
  Function: TP_GUI_MenuLowlightElement

  Swaps the element's image with its lowlighted counterpart.

  Parameters:
   Element - The element, that will be shown as lowlighted.

  Returns:
   void
*/
void TP_GUI_MenuLowlightElement(tp_menu_Element_t *e)
{
        locSpriteLowlight(e);
}


/******************************************************************************
  Function: TP_GUI_MenuDisableElement

  Disables (hides) the given element.

  Parameters:
   Element - The element, that will be disabled.

  Returns:
   void
*/
void TP_GUI_MenuDisableElement(tp_menu_Element_t *e)
{
    FW_VOVIAL_DisableSprite(e->SpriteConf);
}


/******************************************************************************
  Function: TP_GUI_MenuEnableElement

  Enables (shows) the given element.

  Parameters:
   Element - The element, that will be enabled.

  Returns:
   void
*/
void TP_GUI_MenuEnableElement(tp_menu_Element_t *e)
{
    FW_VOVIAL_EnableSprite(e->SpriteConf);
}


/******************************************************************************
  Function: TP_GUI_MenuGetSelectedElement

  Enables (shows) the given element.

  Parameters:
   void

  Returns:
   A pointer to the tp_menu_Element_t object that currently is on focus.
*/
tp_menu_Element_t *TP_GUI_MenuGetSelectedElement(void)
{
    tp_menu_Element_t *e;
    e = locElementListGet(locCurrElement);

    return e;
}


/******************************************************************************
  Function: TP_GUI_MenuMoveElement

  Enables (shows) the given element.

  Parameters:
   Element - The element that will be moved.
   PosX    - The element's new x position.
   PosY    - The element's new y position.
   PosZ    - The element's new z position.

  Returns:
   void
*/
void TP_GUI_MenuMoveElement(tp_menu_Element_t *Element, uint32_t PosX,  uint32_t PosY, uint32_t PosZ)
{
    FW_VOVIAL_MoveSprite(Element->SpriteConf, PosX, PosY, PosZ);
}


/******************************************************************************
  Function: TP_GUI_RegisterShowToolTip

  Registers a callback for displaying tool tips.

  When the focus switches to another element, the given function will be
  called with the newly selected element's tool tip string.

  Parameters:
   ShowToolTip - Function pointer of a TP_ShowToolTip function.

  Returns:
   void
*/
void TP_GUI_RegisterShowToolTip(TP_ShowToolTip ShowToolTip)
{
    locShowToolTip = ShowToolTip;
}
