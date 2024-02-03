/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: app_clock_config.h 11057 2016-11-10 08:23:04Z shinya.tomari $
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
  Section: Local Defines
*/

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
  Macro: _NEEDLE_INIT(i)

  Initialization instructions for needle #i.
  All parameters are initialized (see <_Needle_t>).
*/

#define _NEEDLE_INIT(i) locNeedle[i].AngleSideA     = _N ## i ## _ANGLE_SIDE_A;  \
                        locNeedle[i].AngleSideB     = _N ## i ## _ANGLE_SIDE_B;  \
                        locNeedle[i].Angle          = _N ## i ## _ANGLE_SIDE_A;  \
                        locNeedle[i].WidthTop       = _N ## i ## _WIDTH_TOP;     \
                        locNeedle[i].Length         = _N ## i ## _LENGTH;        \
                        locNeedle[i].CenterCoord.X  = _N ## i ## _BASE_POSX;     \
                        locNeedle[i].CenterCoord.Y  = _N ## i ## _BASE_POSY;     \
                        locNeedle[i].CenterRadius   = _N ## i ## _BASE_WIDTH;    \
                        locNeedle[i].MainColor      = _N ## i ## _MAIN_COLOR;    \


/***********************************************************
  Constants: Pi

  Values of Pi for angle calculations.

  _PI_DEC      - Value of Pi in decimal base
  _PI_DEG      - Value of Pi in degrees
*/


#define _PI_DEC         3.14159265358979323846
#define _PI_DEG         180


/***********************************************************
  Macro: _DEGREE(arg)

  Unit conversion of an angle value from radians to degrees.
*/

#define _DEGREE(arg)    (arg) / _PI_DEG * _PI_DEC


/***********************************************************
  Constant: _NEEDLE_NBR

  Total number of needles to be used by the demonstration.
*/

#define _NEEDLE_NBR     3


/***********************************************************
  Constant: _IMAGE_NBR

  Total number of images to be used by the demonstration.
*/

#define _IMAGE_NBR      1


/***********************************************************
  Constants: Blur configurations

  Values for eeRgl_DrawSetBlur function.
  4-bit shift is required for eeRgl_DrawWidth_t type.

  _BLUR_NULL        - Blur effect disabled
  _BLUR_SHADOW      - Blur value for shadow items
*/

#define _BLUR_NULL      (1 << 4)
#define _BLUR_SHADOW    (5 << 4)


/***********************************************************
  Constant: _SHADOW_OFFSET

  Value for eeRgl_DrawRender* functions.
  4-bit shift is required for eeRgl_DrawPoint_t/eeRgl_DrawWidth_t types.
*/

#define _SHADOW_OFFSET  (5 << 4)


/***********************************************************
  Constant: _SOLID_CIRCLE

  Value to draw a solid circle with eeRgl_DrawRenderCircle functions.
*/

#define _SOLID_CIRCLE   0


/***********************************************************
  Constant: _TAIL_REDUC

  Length reduction ratio between the body of the needle and its "tail" part.
*/

#define _TAIL_REDUC     5


/***********************************************************
  Constants: Initial Values of Needle Parameters

  Values applied with _NEEDLE_INIT(i) macro for needle #i.

  _Ni_SCALE_SIDE_A      - Maximum angle for scale drawing [0; 360]
  _Ni_SCALE_SIDE_B      - Minimum angle for scale drawing [0; 360]
  _Ni_ANGLE_SIDE_A      - Maximum angle for needle rotation [0; 360]
  _Ni_ANGLE_SIDE_B      - Minimum angle for needle rotation [0; 360]
  _Ni_WIDTH_TOP         - Needle top width in pixels
  _Ni_WIDTH_BASE        - Needle base width in pixels
  _Ni_STYLE             - Drawing style (see <DrawingStyle_t>)
  _Ni_DRIVE             - Needle animation (see <RotationDrive_t>)
  _Ni_DIRECTION         - Needle rotation direction (see <RotationDirection_t>)
  _Ni_LENGTH            - Needle length in pixels
  _Ni_SPEED             - Rotation angle added at each frame in degrees
  _Ni_BASE_POSX         - Horizontal position of the needle base
  _Ni_BASE_POSY         - Vertical position of the needle base
  _Ni_BASE_WIDTH        - Width of the needle base drawing
  _Ni_SCALE_RADIUS      - Radius of the scale drawing
  _Ni_SCALE_WIDTH       - Width of the scale drawing
  _Ni_MAIN_COLOR        - Color of Needle
  _Ni_MAIN_OD_COLOR     - 
  _Ni_SCALE_COLOR       - Color of Scale
*/

/* Needle #2 Configuration - Seconds */
#define _N2_ANGLE_SIDE_A        360
#define _N2_ANGLE_SIDE_B          0
#define _N2_WIDTH_TOP             1
#define _N2_LENGTH              90
#define _N2_BASE_POSX           121
#define _N2_BASE_POSY           131
#define _N2_BASE_WIDTH            3
#define _N2_MAIN_COLOR            COLOR_BORDEAUX

/* Needle #1 Configuration - Minutes */
#define _N1_ANGLE_SIDE_A        360
#define _N1_ANGLE_SIDE_B          0
#define _N1_WIDTH_TOP             3
#define _N1_LENGTH               80
#define _N1_BASE_POSX           121
#define _N1_BASE_POSY           131
#define _N1_BASE_WIDTH            5
#define _N1_MAIN_COLOR            COLOR_BLUE

/* Needle #0 Configuration - Hours */
#define _N0_ANGLE_SIDE_A        360
#define _N0_ANGLE_SIDE_B          0
#define _N0_WIDTH_TOP             5
#define _N0_LENGTH               50
#define _N0_BASE_POSX           121
#define _N0_BASE_POSY           131
#define _N0_BASE_WIDTH            5
#define _N0_MAIN_COLOR            COLOR_BLUE

