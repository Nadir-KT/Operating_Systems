/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: main.c 14609 2017-08-31 11:55:45Z florian.zimmermann $
AUTHOR  : $Author: florian.zimmermann $
============================================================================ 
DESCRIPTION
GRAPE (Graphics Application Environment) Application Note
============================================================================
                            C O P Y R I G H T                                    
============================================================================
                           Copyright (c) 2012
                                  by 
                    Renesas Electronics (Europe) GmbH. 
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
Purpose: only for testing
 
DISCLAIMER                                                                   
This software is supplied by Renesas Electronics Corporation and is only     
intended for use with Renesas products. No other uses are authorized. This   
software is owned by Renesas Electronics Corporation and is protected under  
all applicable laws, including copyright laws.                               
THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING  
THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT      
LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE   
AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.          
TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS       
ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE  
FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR   
ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE  
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.                             
Renesas reserves the right, without notice, to make changes to this software 
and to discontinue the availability of this software. By using this software,
you agree to the additional terms and conditions found by accessing the      
following link:                                                              
http://www.renesas.com/disclaimer *                                          
Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.     


****************************************************************************
*/

/***********************************************************
  Title: Main Module 
  
  This module contains the main and the main loop functions.
*/

/***********************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_cdi_api.h"

#include "fw_clibal_api.h"
#include "fw_boal_api.h"
#include "fw_osal_api.h"
#include "fw_vovial_api.h"
#include "fw_hmial_api.h"
#include "fw_fsal_api.h"

#ifdef R_DRW2D_SYS_DHD
#include "davehd_driver.h"
#include "r_util_dhd.h"
#endif
#include "r_dev_api.h"
#include "img_format.h"
#include "romfs_api.h"
#include "font.h"
#include "img_data.h"
#include "fs_data.h"
#include "application.h"
#include "error.h"
#include "r_config_d1x.h"

#define _MAIN_GLOBALS_
#include "main.h"

#include <strings.h>
/***********************************************************
  Section: Local Defines
*/

/***********************************************************
  Define: LOC_BACKGROUND_STACKSIZE

  Stack size of the background thread.
*/
#define LOC_BACKGROUND_STACKSIZE 0x500


/***********************************************************
  Define: LOC_BACKGROUND_PRIO

  Priority of the background thread.
*/

#define LOC_BACKGROUND_PRIO      0x20


/***********************************************************
  Define: LOC_MAINLOOP_STACKSIZE

  Stack size of the main loop thread.
*/

#define LOC_MAINLOOP_STACKSIZE 0x1000


/***********************************************************
  Define: LOC_MAINLOOP_PRIO

  Priority of the main loop thread.
*/

#define LOC_MAINLOOP_PRIO      0x24


/***********************************************************
  Define: LOC_TIME_BASE

  Time in MS to measure the CPU performance.  
*/

#define LOC_TIME_BASE           200


/***********************************************************
  Define: LOC_PERF_PCT

  Multiplier to change to %  
*/

#define LOC_PERF_PCT            100

/*******************************************************************************
  Define: CPU_HP_BLOCK_NB

  Number of blocks the CPU heap is divided into.
  Heap Memory is allocated blockwise.
*/
#define CPU_HP_BLOCK_NB     0x400

/*******************************************************************************
  Define: CPU_HP_BLOCK_SIZE

  Size of one CPU heap block
*/
#define CPU_HP_BLOCK_SIZE   0x40



/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Variable: locStackBackGroundThread

  Stack of the background thread.
*/

static uint32_t locStackBackGroundThread[LOC_BACKGROUND_STACKSIZE >> 2];


/***********************************************************
  Variable: locStackMainLoopThread

  Stack of the main loop thread.
*/

static uint32_t locStackMainLoopThread[LOC_MAINLOOP_STACKSIZE >> 2];

/***********************************************************
  Variable: locSignalSema

  Semaphore to indicate availability of a command.
*/

static fw_osal_Sema_t  locSignalSema;

/***********************************************************
  Variable: locConfirmFlag

  Mutex to ensure, that a command trigger does not interrupt
  the command execution.
*/

static volatile uint32_t locConfirmFlag;

/***********************************************************
  Variable: locCmd

  Command data.
*/

static Cmd_t locCmd;

/***********************************************************
  Variable: locCpuLoad

  CPU load information.
*/

static uint8_t locCpuLoad;

/***********************************************************
  Variable: locQuit

  Signal application exit.
*/

static volatile char locQuit;

r_cdi_Heap_t       loc_VRAM_heap;
r_cdi_Heap_t       loc_lRAM_heap;

/* these depend on the device and are initialised at runtime */
uint32_t loc_DHD_BASE            = 0;
uint32_t loc_VRAM0               = 0;
uint32_t loc_VRAM0_SIZE          = 0;
uint32_t loc_VRAM0_WRAP_ARGB6666 = 0;

#ifdef USE_LRAM_FB
r_cdi_Heap_t loc_VOVIALRAM_heap;
static uint8_t locFrameBufferLRAM[(LOC_DISPLAY_STRIDE * LOC_DISPLAY_HEIGHT * LOC_DISPLAY_BPP * 2 + 0x1000)] __attribute__((aligned(128)));
#endif

#pragma alignvar(8)
uint8_t                   loc_lRAM_heapMem[CPU_HP_BLOCK_NB * CPU_HP_BLOCK_SIZE];
static r_cdi_HpBlkIndex_t loc_lRAM_heapIdxList[CPU_HP_BLOCK_NB];


/***********************************************************
  Section: Local Functions
*/


/***********************************************************
  Function: locCmdGet

  Wait for a command.

  Parameters:
  void

  Returns:
  void
*/

static void locCmdGet(void) 
{
    FW_OSAL_SemaWait(&locSignalSema);
}


/***********************************************************
  Function: locGetOSPerf

  Process returning the number of free instruction cycles
  during LOC_TIME_BASE in milliseconds - for performance measurements.

  Parameters:
  void

  Returns:
  Counter value reached in LOC_TIME_BASE ms.
*/

static uint32_t locGetOSPerf(void)
{
    uint32_t cc;
    uint32_t time;
    uint32_t reftime;
   
    cc      = 0;
    time    = FW_OSAL_TimeGet();
    reftime = FW_OSAL_TimeGet() + LOC_TIME_BASE;
    while (time < reftime) 
    {
        time  = FW_OSAL_TimeGet();
        cc++;
    }
    return (cc);        
}



/***********************************************************
  Function: locBackGroundThread

  The background thread of the application.

  This thread can be used to measure e.g. remaining CPU
  time.

  Parameters:
  Arg        - Thread parameter (not used).

  Returns:
  void*
*/

static void *locBackGroundThread(void* Arg) 
{
    uint32_t reference;
    uint32_t measurement;

    reference = locGetOSPerf();
    while (!locQuit) 
    {
        measurement  = locGetOSPerf();             /* Get Free CPU activity         */
        if (measurement > reference) 
        {
            reference = measurement;
        }
        
        measurement *= LOC_PERF_PCT;               /* Multiply by 100 => (%)        */
        measurement /= reference;                 /* Divide per reference value    */ 
        measurement  = LOC_PERF_PCT - measurement; /* Convert the result            */
                                                   /* from "CPU free" to "CPU load" */
        locCpuLoad   = measurement;
    }
    return 0;
}


/***********************************************************
  Function: locCheckRomFS

  Check if the iROM file system is loaded and if it belongs
  to Grape App.

  Parameters:
  void

  Returns:
  ==0    - OK
  !=0    - NG
*/

static uint32_t locCheckRomFS(void)
{
    void            *fp;
    char            buffer[10];
    int             x;


    fp = FW_FSAL_FOpen("irom.txt", "rb");
    if (!fp)
    {
        return 1;
    }
    FW_FSAL_FRead(buffer, 1, sizeof(buffer), fp);
    FW_FSAL_FClose(fp);
    buffer[9] = 0;
    x = strcmp("GRAPE_APP", buffer);
    return x;
}

/***********************************************************
  Function: locCheckFlashFS

  Check if the flash file system is loaded and if it belongs
  to Grape App.

  Parameters:
  void

  Returns:
  ==0    - OK
  !=0    - NG
*/

static uint32_t locCheckFlashFS(void)
{
    void            *fp;
    char            buffer[10];
    int             x;


    fp = FW_FSAL_FOpen("flash.txt", "rb");
    if (!fp)
    {
        return 1;
    }
    FW_FSAL_FRead(buffer, 1, sizeof(buffer), fp);
    FW_FSAL_FClose(fp);
    buffer[9] = 0;
    x = strcmp("GRAPE_APP", buffer);
    return x;
}

static void locMemInit(void)
{
    uint32_t x;
    r_dev_Device_t dev;

    dev = R_DEV_GetDev();
    /* D1L2 */
    if(dev == R_DEV_R7F701402 ||
       dev == R_DEV_R7F701403)
    {
        loc_VRAM0 = D1L_VRAM0;
        loc_VRAM0_SIZE = D1L_VRAM0_SIZE;
        loc_VRAM0_WRAP_ARGB6666 = D1L_VRAM0_WRAP_ARGB6666;
    }
    /* D1M1 */
    else if(dev == R_DEV_R7F701404 ||
            dev == R_DEV_R7F701405 ||
            dev == R_DEV_R7F701406 ||
            dev == R_DEV_R7F701407 ||
            dev == R_DEV_R7F701418 ||
            dev == R_DEV_R7F701442)
    {
        loc_VRAM0 = D1M1_VRAM0;
        loc_VRAM0_SIZE = D1M_VRAM0_SIZE;
        loc_VRAM0_WRAP_ARGB6666 = D1M1_VRAM0_WRAP_ARGB6666;
        loc_DHD_BASE = loc_VRAM0;
    }
    /* D1M2 */
    else if(dev == R_DEV_R7F701408 ||
            dev == R_DEV_R7F701410 ||
            dev == R_DEV_R7F701411 ||
            dev == R_DEV_R7F701412)
    {
        loc_VRAM0 = D1M2_VRAM0;
        loc_VRAM0_SIZE = D1M_VRAM0_SIZE + D1M2_VRAM1_SIZE;
        loc_VRAM0_WRAP_ARGB6666 = D1M2_VRAM0_WRAP_ARGB6666;
        loc_DHD_BASE = loc_VRAM0;
    }
    /* D1M1A */
    else if(dev == R_DEV_R7F701441 ||
            dev == R_DEV_R7F701460)
    {
        loc_VRAM0 = D1M1A_VRAM0;
        loc_VRAM0_SIZE = D1M1A_VRAM0_SIZE + D1M1A_VRAM1_SIZE;
        loc_VRAM0_WRAP_ARGB6666 = D1M1A_VRAM0_WRAP_ARGB6666;
        loc_DHD_BASE = loc_VRAM0;
    }
    else
    {
        while(1);
    }

    x = R_CDI_InitHeapManager((uint32_t)loc_lRAM_heapMem,
                              &loc_lRAM_heap,
                              loc_lRAM_heapIdxList,
                              CPU_HP_BLOCK_NB,
                              CPU_HP_BLOCK_SIZE);
    if (x == 1)
    {
        while(1);
    }

    x = R_CDI_InitHeapManager(VRAM_HEAP_BASE,
                              &loc_VRAM_heap,
                              0,
                              VID_HP_BLOCK_NB,
                              VID_HP_BLOCK_SIZE);
    if (x == 1)
    {
        while(1);
    }

#ifdef USE_LRAM_FB
    x = R_CDI_InitHeapManager((uint32_t) locFrameBufferLRAM,
                               &loc_VOVIALRAM_heap,
                               0,
                               sizeof(locFrameBufferLRAM) / 256,
                               256);

    if (x == 1)
    {
        while(1);
    }
#endif

}
/***********************************************************
  Function: locMainLoop

  The application main loop.

  This thread waits for commands sent by other threads
  and executes them.

  Parameters:
  Arg        - Thread parameter (not used).

  Returns:
  void
*/

static void *locMainLoop(void * Arg) 
{
    uint32_t err;
    char active;

    FW_OSAL_ThreadCreate(locBackGroundThread, 0, locStackBackGroundThread, 
                      LOC_BACKGROUND_STACKSIZE, LOC_BACKGROUND_PRIO);
    FW_OSAL_SemaCreate(&locSignalSema);
    FW_CLIBAL_Init();
    FW_BOAL_OsLevelInit();
    FW_HMIAL_Init();

#ifdef USE_LRAM_FB
    /* Use local RAM as framebuffer since VRAM is too small. */
    FW_VOVIAL_Init(LOC_VOVIAL_UNIT, LOC_DISPLAY_NAME, &loc_lRAM_heap, &loc_VOVIALRAM_heap);
#else
    FW_VOVIAL_Init(LOC_VOVIAL_UNIT, LOC_DISPLAY_NAME, &loc_lRAM_heap, &loc_VRAM_heap);
#endif

    FW_FSAL_Init(&RomFileSystemData[0]);

    err = locCheckRomFS();
    if (err)
    {
        Error(ERR_FILESYSTEM_BROKEN);
    }

    err = locCheckFlashFS();
    if (err)
    {
        FW_CLIBAL_PrintF("Flash section of ROM file system not available.\n");
    }

    FW_CLIBAL_PrintF("hello grape_app\n");

    active = AppNum - 1;
    (AppList[AppNum - 1]->Init)();
    FW_HMIAL_SetControl(AppList[(AppNum - 1)]->Control);
    locConfirmFlag = 1;
    locQuit = 0;

    while (!locQuit) 
    {
        locCmdGet();
        switch (locCmd.Cmd) 
        {
        case CMD_START:
            active = locCmd.Par1;
            (AppList[locCmd.Par1]->Init)();
            FW_HMIAL_SetControl(AppList[locCmd.Par1]->Control);
        break;
        case CMD_STOP:
            FW_HMIAL_SetControl(0);
            (AppList[locCmd.Par1]->DeInit)();
        break;
        case CMD_STOPSTART:
            FW_HMIAL_SetControl(0);
            (AppList[active]->DeInit)();
            (AppList[locCmd.Par2]->Init)();
            FW_HMIAL_SetControl(AppList[locCmd.Par2]->Control);
            active = locCmd.Par2;
        break;
        case CMD_EXIT:
            FW_HMIAL_SetControl(0);
            (AppList[active]->DeInit)();
            locQuit = 1;
        break;
        default:
            locQuit = 1;
        break;
        }
        locConfirmFlag = 1;
    }

    FW_VOVIAL_DeInit(0);
    FW_HMIAL_DeInit();
    FW_BOAL_OsLevelDeInit();
    FW_CLIBAL_DeInit();
    return 0;
}


/***********************************************************
  Section: Global Functions

  Description see: <main.h>
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

void CmdSend(Cmd_t *Cmd) 
{
    if (locConfirmFlag == 0) 
    {
        return; 
    }
    locCmd = *Cmd;
    locConfirmFlag = 0;
    FW_OSAL_SemaSignal(&locSignalSema);
}


/***********************************************************
  Function: GetCpuLoad

  Get current CPU load.

  Parameters:
  void

  Returns:
  CPU load in %
*/

uint8_t GetCpuLoad(void) 
{
    return locCpuLoad;
}


/***********************************************************
  Function: main
  
  Initialize the board, OS and create the main thread
*/

int main(void) 
{
    FW_BOAL_LowLevelInit();
    FW_OSAL_InitOS();
    locMemInit();

#ifdef R_DRW2D_SYS_DHD
    R_UTIL_DHD_Init(0);
    R_UTIL_DHD_Config((dhd_gpu_ptr_t)loc_DHD_BASE, DHD_MEMORY_SIZE, &loc_lRAM_heap);
#endif

    FW_OSAL_ThreadCreate(locMainLoop, 0, locStackMainLoopThread, 
                      LOC_MAINLOOP_STACKSIZE, LOC_MAINLOOP_PRIO);
    FW_OSAL_StartOS();
    FW_BOAL_LowLevelDeInit();  /* Depending on the OS, you may never get here */
    return 0;
}
