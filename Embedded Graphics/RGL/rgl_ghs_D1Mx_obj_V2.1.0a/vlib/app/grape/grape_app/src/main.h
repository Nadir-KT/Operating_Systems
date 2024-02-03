/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: main.h 14609 2017-08-31 11:55:45Z florian.zimmermann $
============================================================================ 
DESCRIPTION
Sample program for demonstration of the D1MX GFX features
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2014
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

#ifndef _MAIN_H_
#define _MAIN_H_


#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Title: Main Module Interface
  
  This header contains the declaration of all  specific 
  functions and types of the main module.
*/

/***********************************************************
  Section: Global Defines

  Definition of all global defines.
*/

#undef _EXTERN_
#ifndef _MAIN_GLOBALS_
#define _EXTERN_ extern 
#else
#define _EXTERN_
#endif


/***********************************************************
  Section: Global Types

  Global types of the main module are defined here.
*/

/***********************************************************
  Enum: CmdId_t

  Command identifier

  Values:
  CMD_START                 - Start an application
  CMD_STOP                  - Stop an application
  CMD_STOPSTART             - Stop running application and start the app_menu, 
                              if app_menu is included in gfx_appnote
  CMD_STOPME_STARTSELECTED  - Stop the app_menu and start selected application
*/

typedef enum 
{
    CMD_START,
    CMD_STOP,
    CMD_STOPSTART,
    CMD_STOPME_STARTSELECTED,
    CMD_EXIT
} CmdId_t;


/***********************************************************
  Type: Cmd_t

  Command type structure.

  The command structure is used to control the main loop of
  the program by different threads.

  Members:
  Par1    - 1st command specific parameter
  Par2    - 2nd command specific parameter
  Cmd     - Command identifier; see: <CmdId_t>
*/

typedef struct 
{
    uint32_t   Par1;
    uint32_t   Par2;
    CmdId_t    Cmd;
} Cmd_t;


/***********************************************************
  Section: Global Functions
*/

/***********************************************************
  Function: CmdSend

  Send a command to the main loop.

  The main loop thread waits for commands, executes them
  and goes to "wait for command" mode then. A call of this
  function will wakeup the main loop thread. The calling
  thread might be suspended, if its priority is lower than
  the one of the main loop.

  Parameters:
  Msg        - Command to execute

  Returns:
  void
*/

void CmdSend(Cmd_t *Msg);


/***********************************************************
  Function: GetCpuLoad

  Get current CPU load.

  Parameters:
  void

  Returns:
  CPU load in %
*/

uint8_t GetCpuLoad(void);

/*******************************************************************************
  Define: Display infos

   LOC_DISPLAY_NAME     - display string
   LOC_DISPLAY_WIDTH    - display width
   LOC_DISPLAY_HEIGHT   - display height
   LOC_DISPLAY_BPP      - bytes per pixel
*/

#define LOC_DISPLAY_NAME   "NEC_QVGA_TOUCH_240x320"
#define LOC_VOVIAL_UNIT 0
#define LOC_VOVIAL_FORMAT FW_VOVIAL_RGB565
#define LOC_DRW2D_FRAMEBUFFER_FORMAT R_DRW2D_PIXELFORMAT_RGB565
#define LOC_DISPLAY_WIDTH  240
#define LOC_DISPLAY_STRIDE 256
#define LOC_DISPLAY_HEIGHT 320
#define LOC_DISPLAY_BPP    2

//extern uint32_t loc_LVRAM_SIZE = 0;
extern uint32_t loc_VRAM0;
extern uint32_t loc_VRAM0_SIZE;
extern uint32_t loc_VRAM0_WRAP_ARGB6666;

#define VID_HP_BLOCK_SIZE     (128u)
#define VID_HP_BLOCK_NB       ((loc_VRAM0_SIZE-DHD_MEMORY_SIZE) / VID_HP_BLOCK_SIZE)
#define ALIGN(ADDR,BLOCK)     ( (ADDR+(BLOCK-1)) & ~(BLOCK-1) )
#define VRAM_HEAP_BASE        ALIGN((loc_VRAM0+DHD_MEMORY_SIZE), VID_HP_BLOCK_SIZE)


#ifdef __cplusplus
}
#endif

#endif /* _MAIN_H_ */

