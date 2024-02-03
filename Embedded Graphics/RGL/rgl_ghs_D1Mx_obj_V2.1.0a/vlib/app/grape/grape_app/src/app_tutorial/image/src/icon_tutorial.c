#include "r_typedefs.h"
#include "img_format.h"

static uint8_t * locImgPtr = (uint8_t*)0;

const Img_t Img_icon_tutorial = {
    "icon_tutorial.bin",
    0,
    48,
    46,
    IMG_ARGB1555,
    IMG_ATTRIBUTE_USE_FROM_PERSISTENT_MEMORY,
    0,
    0,
    0,
    &locImgPtr,
    4416
};


