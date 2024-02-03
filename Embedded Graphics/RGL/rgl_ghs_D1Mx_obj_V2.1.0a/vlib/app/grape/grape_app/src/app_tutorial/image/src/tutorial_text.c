#include "r_typedefs.h"
#include "img_format.h"

static uint8_t * locImgPtr = (uint8_t*)0;

const Img_t Img_tutorial_text = {
    "tutorial_text.bin",
    0,
    120,
    19,
    IMG_ALPHA8,
    IMG_ATTRIBUTE_USE_FROM_PERSISTENT_MEMORY,
    0,
    0,
    0,
    &locImgPtr,
    2280
};


