#include "r_typedefs.h"
#include "img_format.h"

static uint8_t * locImgPtr = (uint8_t*)0;

const Img_t Img_tp_bg = {
    "tp_bg.bin",
    0,
    288,
    320,
    IMG_RGB888,
    IMG_ATTRIBUTE_RLE_COMPRESSED | IMG_ATTRIBUTE_USE_FROM_PERSISTENT_MEMORY,
    0,
    0,
    0,
    &locImgPtr,
    127005
};


