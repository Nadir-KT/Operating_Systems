/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: app_menu.h 11057 2016-11-10 08:23:04Z shinya.tomari $
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

#ifndef _APP_MENU_H_
#define _APP_MENU_H_

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Title: Test Application Interface
*/


/***********************************************************
  Section: Global Defines
*/

/***********************************************************
  Define: MENUICON_SPACE_ICON

  Width of a icon menu icon.
*/

#define MENUICON_SPACE_ICON     20


/***********************************************************
  Define: MENUICON_WIDTH

  Width of a icon menu icon.
*/

#define MENUICON_WIDTH          48


/***********************************************************
  Define: MENUICON_HEIGHT

  Height of a icon menu icon.
*/

#define MENUICON_HEIGHT         46


/***********************************************************
  Define: POPUPICON_WIDTH

  Width of a popup menu icon
*/

#define POPUPICON_WIDTH         26


/***********************************************************
  Define: POPUPICON_HEIGHT

  Height of a popup menu icon
*/

#define POPUPICON_HEIGHT        25


/***********************************************************
  Define: POPUPLINE_WIDTH

  Width of a popup menu line
*/

#define POPUPLINE_WIDTH         190


/***********************************************************
  Define: POPUPLINE_HEIGHT

  Height of a popup menu line
*/

#define POPUPLINE_HEIGHT        28


/***********************************************************
  Define: TOP_TEXT_POSY

  Menu top text Y position.
*/

#define TOP_TEXT_POSY         0


/***********************************************************
  Define: TOP_TEXT_HEIGHT

  Menu top text height.
*/

#define TOP_TEXT_HEIGHT        32


/***********************************************************
  Define: BOTTOM_TEXT_POSY

  Menu bottom text Y position.
*/

#define BOTTOM_TEXT_POSY        312


/***********************************************************
  Define: BOTTOM_TEXT_HEIGHT

  Menu bottom text height.
*/

#define BOTTOM_TEXT_HEIGHT        28


/***********************************************************
  Define: MENU_ICON_HEIGHT

  Icon menu height.
*/

#define MENU_ICON_HEIGHT        280


/***********************************************************
  Define: COLOR_WHITE

  White color used in the feature demo.
*/

#define COLOR_WHITE              0x00ffffff


/***********************************************************
  Define: COLOR_YELLOW

  Yellow color used in the feature demo.
*/

#define COLOR_YELLOW            ((255<<24) | (255 << 16) | (216 << 8) | (0))


/***********************************************************
  Define: COLOR_BLACK

  Black color used in the feature demo.
*/

#define COLOR_BLACK              0x00000000


/***********************************************************
  Define: COLOR_GREY

  Grey color used in the feature demo.
*/

#define COLOR_GREY              ((255<<24) | (162 << 16) | (162 << 8) | (162))


/***********************************************************
  Define: COLOR_LIGHT_GREY

  Light grey color used in the feature demo.
*/

#define COLOR_LIGHT_GREY        ((255<<24) | (210 << 16) | (210 << 8) | (210))


/***********************************************************
  Define: COLOR_BLUE

  Blue color used in the feature demo.
*/

#define COLOR_BLUE               0x000000ff


/***********************************************************
  Define: COLOR_GREEN

  Green color used in the feature demo.
*/

#define COLOR_GREEN              0x0000ff00


/***********************************************************
  Define: COLOR_RED

  Red color used in the feature demo.
*/

#define COLOR_RED                0x00ff0000


/***********************************************************
  Define: COLOR_BORDEAUX

  Dark red color used in the feature demo.
*/

#define COLOR_BORDEAUX          ((255<<24) | (144 << 16) | (0 << 8) | (0))

/***********************************************************
  Section: Global Types
*/

/***********************************************************
  Section: Global Variables
*/

/***********************************************************
  Variable: AppMenu

  Simple test application.
*/

extern const App_t AppMenu;


#ifdef __cplusplus
}
#endif

#endif /* _APP_MENU_H_ */
