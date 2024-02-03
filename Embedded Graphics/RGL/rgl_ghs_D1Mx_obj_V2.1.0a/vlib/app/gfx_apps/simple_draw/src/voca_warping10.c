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

const uint16_t loc_ExpImageData_warping10[113] = { 
    0xfc00,0x0000,0x0000,0x00ff,0xc000,0x0000,0x0000,0x0ffc,0x003f,0xd5ff,0x0000,0xffc0,0x3eaf,0x55bc,0x000f,0xf03e,
    0xffff,0x55f0,0x00ff,0x0ff0,0x003d,0x5700,0x0ff0,0xf000,0x00d5,0x7000,0xff00,0x0000,0x0f57,0x000f,0xf000,0x0000,
    0xd5b0,0x03ff,0x0000,0x003d,0x5f00,0x3ff0,0x0000,0x0355,0xc003,0xff00,0x0003,0xd5f0,0x003f,0xf000,0x00f5,0x7c00,
    0x03ff,0x03ff,0x0d5f,0x0000,0x3fc0,0xfff3,0xd5c0,0x0003,0xfc0d,0x570d,0x5c03,0x003f,0xc0d5,0x7035,0x7030,0x03fc,
    0x0d57,0x0397,0xc300,0x3fc0,0xd570,0x0d6c,0x000f,0xfc0d,0x5700,0xf5f0,0x00ff,0xc0ea,0xb003,0xd700,0x0ffc,0x0fff,
    0x000f,0x7c00,0xffc0,0x0000,0x003e,0xf00f,0xfc00,0x0000,0x00fb,0x00ff,0xc000,0x0000,0x03f0,0x0ffc,0x0000,0x0000,
    0x0c00,0xff00,0x0000,0x0000,0x003f,0xf000,0x0000,0x0000,0x03ff,0x0000,0x0000,0x0000,0x3ff0,0x0000,0x0000,0x0003,
    0xff00
};

r_wm_Voca_t loc_Voca_warping10 = {
    0x000001, /* Threshold */ 
    1u,       /* AreaNum */
    178u, 290u, 30u, 30u, /* PosX, PosY, Width, Height */ 
    150u,     /* RamAddr */
    113u,     /* ExpSize */ 
    loc_ExpImageData_warping10, /* ExpImg */ 
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

