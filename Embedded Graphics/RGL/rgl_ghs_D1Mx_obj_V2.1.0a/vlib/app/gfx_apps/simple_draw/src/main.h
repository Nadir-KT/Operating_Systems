/***********************************************************************************************
PROJECT : simple_draw
FILE   : $Id: main.h 16855 2019-08-06 01:12:45Z shinya.tomari.wj@renesas.com $
============================================================================ 
DESCRIPTION
header for simple_draw
============================================================================
                            C O P Y R I G H T
============================================================================
                       Copyright (c) 2013 - 2014
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
#ifndef MAIN_H_
#define MAIN_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

#if LOC_RING_BUFFER_MODE == 0
  #define LOC_VOWE_DL_ADDR_LENGTH_1  (VOWE_DL_FanWise_Warping80_240_320_LENGTH)
  #define LOC_VOWE_DL_ADDR_LENGTH_2  (VOWE_DL_FanWise_Warping10_240_320_LENGTH)
  #define LOC_VOWE_DL_ADDR_1         (VOWE_DL_FanWise_Warping80_240_320)
  #define LOC_VOWE_DL_ADDR_2         (VOWE_DL_FanWise_Warping10_240_320)
  #define LOC_VOWE_LSPR              (VOWE_LSPR_FanWise_Warping80_240_320)
  #define LOC_VOWE_WORK_BUFF_MODE    (R_VOWE_FRAME_BUFFER_MODE)
  #define LOC_VOWE_WORK_BUFF_SIZE    (LOC_VOWE_FRAME_BUF_SIZE)
#else
  #define LOC_VOWE_DL_ADDR_LENGTH_1  (VOWE_DL_FanWise_Warping80_240_320_RingBuffer_LENGTH)
  #define LOC_VOWE_DL_ADDR_LENGTH_2  (VOWE_DL_FanWise_Warping10_240_320_RingBuffer_LENGTH)
  #define LOC_VOWE_DL_ADDR_1         (VOWE_DL_FanWise_Warping80_240_320_RingBuffer)
  #define LOC_VOWE_DL_ADDR_2         (VOWE_DL_FanWise_Warping10_240_320_RingBuffer)
  #define LOC_VOWE_LSPR              (VOWE_LSPR_FanWise_Warping80_240_320_RingBuffer)
  #define LOC_VOWE_WORK_BUFF_MODE    (R_VOWE_RING_BUFFER_MODE)
  #define LOC_VOWE_WORK_BUFF_SIZE    (LOC_VOWE_RING_BUF_SIZE)
#endif


/* bitmap widths */
#define LOC_KING_W    (30)
/* bitmap heights */
#define LOC_KING_H    (30)
/* bitmap sizes */
#define LOC_KING_SIZE (LOC_KING_W * LOC_KING_H * 4)

/*******************************************************************************
  Section: Local Functions
*/

void loc_Error(int32_t Err);

void  loc_Identify(void);
void* loc_StartSFMA(void);
void  loc_StopSFMA(void);
void* loc_StartOCTA(void);
void  loc_StopOCTA(void);
void* loc_StartHYPB(void);
void  loc_StopHYPB(void);

void loc_VoweStart(void);
void loc_VoweStop(void);
void loc_Vowe_DLChange(int32_t dl_cnt);

void loc_JcuaRun(void);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
