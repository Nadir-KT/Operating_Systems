/*
****************************************************************************

============================================================================
                             C O P Y R I G H T
============================================================================
                            Copyright (c) 2020
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

#include "r_typedefs.h"
#include "r_ddb_api.h"
#include "r_cdi_api.h"
#include "r_wm_api.h"

const uint16_t loc_ExpImageData_warping80[150] = { 
    0xc000,0x0000,0x0000,0x0000,0x00ff,0xc000,0x0000,0x0000,0x0000,0x00ff,0xf000,0x0fff,0x55ff,0xc000,0x00ff,0xf000,
    0xfe9f,0xd556,0xbc00,0x00ff,0xf03f,0x7fff,0xffd5,0x5f00,0x003f,0xf03f,0xf000,0x00f5,0x57c0,0x003f,0xf03f,0x0000,
    0x0039,0x55c0,0x003f,0xf000,0x0000,0x000d,0x55f0,0x003f,0xf000,0x0000,0x003d,0x55c0,0x003f,0xf000,0x0000,0x003d,
    0x55c0,0x003f,0xf000,0x0000,0x00f5,0x57c0,0x003f,0xf000,0x0000,0x0fd5,0x7f0c,0x000f,0xfc00,0x0000,0x3d55,0xfc00,
    0x000f,0xfc03,0xfff0,0x3957,0xc003,0x000f,0xfc03,0xfffc,0x3d57,0xc003,0x000f,0xfc03,0x956f,0x0f55,0xc003,0x000f,
    0xfc03,0x955f,0x03d5,0x7c03,0x000f,0xfc03,0xd55f,0x00f5,0x6f03,0x0003,0xfc03,0xd55f,0x003d,0x5f00,0x0003,0xfc03,
    0xd55f,0x000f,0x57c0,0x0003,0xfc03,0xeaaf,0x0003,0xf5fc,0x0003,0xff00,0xffff,0x0000,0x3d6f,0x0003,0xff00,0x0000,
    0x0000,0x03eb,0xc003,0xff00,0x0000,0x0000,0x00fa,0x8003,0xff00,0x0000,0x0000,0x000f,0xf000,0xff00,0x0000,0x0000,
    0x0000,0xc000,0xff00,0x0000,0x0000,0x0000,0x0000,0xff00,0x0000,0x0000,0x0000,0x0000,0xff00,0x0000,0x0000,0x0000,
    0x0000,0xff00,0x0000,0x0000,0x0000,0x0000
};

r_wm_Voca_t loc_Voca_warping80 = {
    0x000001, /* Threshold */ 
    0u,       /* AreaNum */
    200u, 290u, 40u, 30u, /* PosX, PosY, Width, Height */ 
    0u,       /* RamAddr */
    150u,      /* ExpSize */ 
    loc_ExpImageData_warping80, /* ExpImg */ 
    {
        {0xFF, 0xFF, 0xFF,  /* Clut[0].RUpper, GUpper, BUpper */ 
         0xF0, 0xF0, 0xF0}, /* Clut[0].RLower, GLower, BLower */ 
        {0x0F, 0x4F, 0x8F,  /* Clut[1].RUpper, GUpper, BUpper */ 
         0x00, 0x40, 0x80}, /* Clut[1].RLower, GLower, BLower */ 
        {0x0F, 0x4F, 0x9F,  /* Clut[2].RUpper, GUpper, BUpper */ 
         0x00, 0x40, 0x90}, /* Clut[2].RLower, GLower, BLower */ 
        {0xFF, 0xFF, 0xFF,  /* Clut[3].RUpper, GUpper, BUpper */ 
         0x00, 0x00, 0x00}  /* Clut[3].RLower, GLower, BLower */ 
    },
    R_NULL    /* Next */
};

