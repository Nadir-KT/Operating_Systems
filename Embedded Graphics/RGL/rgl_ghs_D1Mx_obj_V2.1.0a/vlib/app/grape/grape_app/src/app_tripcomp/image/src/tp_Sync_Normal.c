#include "r_typedefs.h"
#include "img_format.h"

static uint8_t * locImgPtr = (uint8_t*)0;

const Img_t Img_tp_Sync_Normal = {
    "tp_Sync_Normal.bin",
    0,
    40,
    40,
    IMG_ARGB8888,
    IMG_ATTRIBUTE_USE_FROM_PERSISTENT_MEMORY,
    0,
    0,
    0,
    &locImgPtr,
    6400
};


