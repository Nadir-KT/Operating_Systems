/*
****************************************************************************
PROJECT : WM
============================================================================ 
DESCRIPTION
WM configuration
============================================================================
                            C O P Y R I G H T
============================================================================
                       Copyright (c) 2013 - 2019
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
****************************************************************************
*/

#ifndef R_CONFIG_WM_H
#define R_CONFIG_WM_H


/***********************************************************
  Title: config_wm.h
  
  D1Mx Mango board WM Configuration
*/

/***********************************************************
  Section: Global Defines
************************************************************/

/***********************************************************
  Constant: MAX_LAYER_*

  default max dimensions given by VDCE/SPEA/VOCA on D1x
  
  Maximum pitch size can be changed up to 261120.
*/

#define R_WM_MAX_LAYER_PITCH            (8192u)  /* 0: no pitch supported */
#define R_WM_MAX_LAYER_WIDTH            (1280u)
#define R_WM_MAX_LAYER_HEIGHT           (1024u)
#define R_WM_MAX_LAYERS                 (4u)
#define R_WM_MAX_CAPTURE_LAYERS         (2u)
#define R_WM_MAX_SPRITES_PER_LAYER      (16u)
#define R_WM_MAX_VOCA_MONITOR_AREA      (16u)
#define R_WM_MAX_DISCOM_PER_UNIT        (2u)

/***************************************************************************
 Constants: Unit Number

 The definition of the unit number.
*/
#define R_WM_UNIT0                      (0u)
#define R_WM_UNIT1                      (1u)

/***********************************************************
  Constant: R_WM_DEV_NUM

  Number of maximum VOUT (WM) units
*/

#define R_WM_DEV_NUM                    (2u)

/***********************************************************
  Constants: Boolean

  Boolean for unsinged value.

  R_WM_TRUE   - True
  R_WM_FALSE  - False
*/
#define R_WM_TRUE                       (1u)
#define R_WM_FALSE                      (0u)

/***********************************************************
  Constant: R_WM_STATIC
  
  Definition for reserved word "static".
*/

#ifndef R_WM_BUILD_UNIT_TEST
  #define R_WM_STATIC static
#else
  #define R_WM_STATIC
#endif

#endif /* R_CONFIG_WM_H */
