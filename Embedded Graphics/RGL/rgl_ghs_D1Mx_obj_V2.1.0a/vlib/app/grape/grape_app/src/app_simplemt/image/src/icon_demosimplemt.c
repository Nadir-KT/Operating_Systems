#include "r_typedefs.h"
#include "img_format.h"

static uint8_t * locImgPtr = (uint8_t*)0;

const Img_t Img_icon_demosimplemt = {
    "icon_demosimplemt.bin",
    0,
    48,
    48,
    IMG_RGB565,
    IMG_ATTRIBUTE_USE_FROM_PERSISTENT_MEMORY,
    0,
    0,
    0,
    &locImgPtr,
    4608
};


