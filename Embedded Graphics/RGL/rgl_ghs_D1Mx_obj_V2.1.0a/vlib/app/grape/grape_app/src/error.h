/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: error.h 11057 2016-11-10 08:23:04Z shinya.tomari $
============================================================================ 
DESCRIPTION
Sample program for demonstration of the D1X features
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

#ifndef _ERROR_H_
#define _ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Title: Error Module Interface
  
  This is the interface to the application's error
  handling.
*/

/***********************************************************
  Section: Global Defines
*/

/***********************************************************
  Constants: GFX_APPNOTE Error Codes

  Error codes of the GFX_APPNOTE.

  ERR_OK                     - No error
  ERR_NG                     - Unspecified error
  ERR_TEXTURE_SETUP_FAILED   - Setup of a texture failed
  ERR_FILESYSTEM_BROKEN      - File system check failed
*/

#define ERR_OK                    (0)
#define ERR_NG                    (-10)
#define ERR_TEXTURE_SETUP_FAILED  (-20)
#define ERR_FILESYSTEM_BROKEN     (-30)
#define ERR_DRW2D                 (-40)
#define ERR_VOVIAL                (-50)


/***********************************************************
  Section: Global Functions
*/

/***********************************************************
  Function: Error

  Application error handling.

  If an application function detects an error, it will call
  this function with 'Err != 0'.

  Parameters:
  Err       - Error code (0 == no error)

  Returns:
  void
*/

void Error(int Err);


#ifdef __cplusplus
}
#endif

#endif /* _ERROR_H_ */

