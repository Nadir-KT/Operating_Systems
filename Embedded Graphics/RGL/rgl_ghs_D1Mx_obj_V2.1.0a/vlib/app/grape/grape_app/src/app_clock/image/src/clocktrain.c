#include "r_typedefs.h"
#include "img_format.h"

static uint8_t * locImgPtr = (uint8_t*)0;

const Img_t Img_clocktrain = {
    "clocktrain.bin",
    0,
    240,
    320,
    IMG_ARGB8888,
    IMG_ATTRIBUTE_RLE_COMPRESSED | IMG_ATTRIBUTE_USE_FROM_PERSISTENT_MEMORY,
    0,
    0,
    0,
    &locImgPtr,
    198797
};


