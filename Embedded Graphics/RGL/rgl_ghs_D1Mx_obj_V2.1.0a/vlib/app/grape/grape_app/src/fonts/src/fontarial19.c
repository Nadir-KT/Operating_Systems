#include "r_typedefs.h"

#include "img_format.h"
#include "img_data.h"
#include "font.h"

static const FontPos_t _FontArial19Pos[] = {
    { 0, 4}, /*   */
    { 4, 4}, /* ! */
    { 8, 8}, /* " */
    { 16, 9}, /* # */
    { 25, 9}, /* $ */
    { 34, 16}, /* % */
    { 50, 12}, /* & */
    { 62, 4}, /* ' */
    { 66, 5}, /* ( */
    { 71, 5}, /* ) */
    { 76, 6}, /* * */
    { 82, 9}, /* + */
    { 91, 4}, /* , */
    { 95, 5}, /* - */
    { 100, 4}, /* . */
    { 104, 4}, /* / */
    { 108, 9}, /* 0 */
    { 117, 9}, /* 1 */
    { 126, 9}, /* 2 */
    { 135, 9}, /* 3 */
    { 144, 9}, /* 4 */
    { 153, 9}, /* 5 */
    { 162, 9}, /* 6 */
    { 171, 9}, /* 7 */
    { 180, 9}, /* 8 */
    { 189, 9}, /* 9 */
    { 198, 6}, /* : */
    { 204, 6}, /* ; */
    { 210, 9}, /* < */
    { 219, 9}, /* = */
    { 228, 9}, /* > */
    { 237, 10}, /* ? */
    { 247, 16}, /* @ */
    { 263, 11}, /* A */
    { 274, 12}, /* B */
    { 286, 12}, /* C */
    { 298, 12}, /* D */
    { 310, 11}, /* E */
    { 321, 10}, /* F */
    { 331, 12}, /* G */
    { 343, 12}, /* H */
    { 355, 4}, /* I */
    { 359, 9}, /* J */
    { 368, 12}, /* K */
    { 380, 10}, /* L */
    { 390, 13}, /* M */
    { 403, 12}, /* N */
    { 415, 12}, /* O */
    { 427, 11}, /* P */
    { 438, 12}, /* Q */
    { 450, 12}, /* R */
    { 462, 11}, /* S */
    { 473, 10}, /* T */
    { 483, 12}, /* U */
    { 495, 11}, /* V */
    { 506, 15}, /* W */
    { 521, 11}, /* X */
    { 532, 10}, /* Y */
    { 542, 9}, /* Z */
    { 551, 5}, /* [ */
    { 556, 4}, /* \ */
    { 560, 5}, /* ] */
    { 565, 9}, /* ^ */
    { 574, 9}, /* _ */
    { 583, 5}, /* ` */
    { 588, 9}, /* a */
    { 597, 10}, /* b */
    { 607, 9}, /* c */
    { 616, 10}, /* d */
    { 626, 9}, /* e */
    { 635, 6}, /* f */
    { 641, 9}, /* g */
    { 650, 10}, /* h */
    { 660, 4}, /* i */
    { 664, 5}, /* j */
    { 669, 9}, /* k */
    { 678, 4}, /* l */
    { 682, 14}, /* m */
    { 696, 10}, /* n */
    { 706, 10}, /* o */
    { 716, 10}, /* p */
    { 726, 10}, /* q */
    { 736, 6}, /* r */
    { 742, 9}, /* s */
    { 751, 5}, /* t */
    { 756, 10}, /* u */
    { 766, 9}, /* v */
    { 775, 13}, /* w */
    { 788, 9}, /* x */
    { 797, 9}, /* y */
    { 806, 9}, /* z */
    { 815, 6}, /* { */
    { 821, 4}, /* | */
    { 825, 6}, /* } */
    { 831, 10}, /* ~ */
    { 841, 6}, /*  */
};

const Font_t FontArial19 = {
    (Img_t*)&Img_arial19,
    _FontArial19Pos,
    19
};

