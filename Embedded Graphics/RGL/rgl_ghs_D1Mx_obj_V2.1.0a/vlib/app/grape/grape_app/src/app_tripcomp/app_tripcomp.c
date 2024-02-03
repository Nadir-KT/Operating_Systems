/*
****************************************************************************
PROJECT : GRAPE_APP
FILE    : $Id: app_tripcomp.c 11057 2016-11-10 08:23:04Z shinya.tomari $
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

/***********************************************************
  Title: Test Application Module
*/

/***********************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_config_d1x.h"
#include "r_cdi_api.h"
#include "fw_hmial_api.h"
#include "application.h"
#include "fw_osal_api.h"
#include "fw_vovial_api.h"

#ifdef R_DRW2D_SYS_DHD
#include "davehd_driver.h"
#include "r_cdi_api.h"
#include "r_util_dhd.h"
#include "r_drw2d_os.h"
#include "r_drw2d_api.h"
#include "r_config_drw2d.h"
#include "r_drw2d_ctx_dhd.h"
#else
#include "r_drw2d_os.h"
#include "r_drw2d_api.h"
#include "r_config_drw2d.h"
#include "r_drw2d_ctx_cpu.h"
#endif

#include "r_ddb_api.h"
#include "r_wm_api.h"

#include "img_format.h"
#include "img.h"
#include "img_drw2d.h"

#include "font.h"
#include "font_data.h"
#include "tp_font.h"

#include "app_tripcomp_image.h"
#include "app_tripcomp_menu.h"
#include "main.h"
#include "error.h"

#include <string.h>
#include <stdio.h>

/***********************************************************
  Section: Externals
*/

/***********************************************************
  External: loc_VRAM_heap

  CDI heap structure for VideoRAM.
*/
extern r_cdi_Heap_t loc_VRAM_heap;


/***********************************************************
  Section: Local Defines
*/

/***********************************************************
  Define: LOC_BGCOLOR

  Background color of the display.
*/

#define LOC_BGCOLOR                       0xff888888


/***********************************************************
  Define: LOC_APP_TEST_THREAD_STACKSIZE

  Size in bytes of the stack used by the application thread
*/

#define LOC_APP_TEST_THREAD_STACKSIZE     0x1400


/***********************************************************
  Define: LOC_APP_TEST_THREAD_PRIO

  Priority level of the application thread
*/
#define LOC_APP_TEST_THREAD_PRIO           0x22

/***********************************************************
  Define: LOC_STATUSBAR_TEXT

  Default text for the status bar.
*/
#define LOC_STATUSBAR_TEXT                 " 4:24 pm  Monday, Feb 16, 2015"

/***********************************************************
  Define: LOC_FAN_SPEED_TEXT

  Default text for the fan speed.
*/
#define LOC_FAN_SPEED_TEXT                 " Fan speed 0"

/***********************************************************
  Define: LOC_FAN_SPEED_MAX

  Maximum speed of the fan.
*/
#define LOC_FAN_SPEED_MAX                  5

/***********************************************************
  Define: LOC_VRAM_IMAGE_COUNT

  Number of images moved to VRAM.
*/
#define LOC_VRAM_IMAGE_COUNT               19


/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Variable: locSurface*

  The application's drawing/sprite/RLE surfaces.
*/
static void       *locSurface;
static void       *locSurfaceSprite;
static void       *locSurfaceSpriteTop;
static void       *locSurfaceRLE;


/***********************************************************
  Variable: locOldData*

  Pointers to keep the original image data.
*/
static void       *locOldBg;
static void       *locOldMicN;
static void       *locOldMicS;
static void       *locOldSyncN;
static void       *locOldSyncS;
static void       *locOldRadioN;
static void       *locOldRadioS;
static void       *locOldVolumeN;
static void       *locOldVolumeS;
static void       *locOldFuelGauge;
static void       *locOldFuelHand;
static void       *locOldWeatherIcon;
static void       *locOldWarningSign;
static void       *locOldFontArial19;
static void       *locOldFan_0;
static void       *locOldFan_1;
static void       *locOldFan_2;
static void       *locOldFan_3;
static void       *locOldFan_4;
static void       *locOldFanSelect;
static void       *locOldFanFocus;

/***********************************************************
  Variable: locAllocedVRAM

  Pointer array to store allocated VRAM blocks.
*/
static void       *locAllocedVRAM[LOC_VRAM_IMAGE_COUNT];

/***********************************************************
  Variable: locSprite*

  Sprite configurations for the sprites.
*/
static fw_vovial_Sprite_t locSpriteRadio;
static fw_vovial_Sprite_t locSpriteMic;
static fw_vovial_Sprite_t locSpriteVolume;
static fw_vovial_Sprite_t locSpriteSync;
static fw_vovial_Sprite_t locSpriteFuel;
static fw_vovial_Sprite_t locSpriteFuelHand;
static fw_vovial_Sprite_t locSpriteWeather;
static fw_vovial_Sprite_t locSpriteWarningSign;
static fw_vovial_Sprite_t locSpriteFan;
static fw_vovial_Sprite_t locSpriteFanSelect;

/***********************************************************
  Variable: locElement*

  TP GUI element configurations for all GUI elements.
*/
static tp_menu_Element_t locElementRadio;
static tp_menu_Element_t locElementMic;
static tp_menu_Element_t locElementVolume;
static tp_menu_Element_t locElementSync;
static tp_menu_Element_t locElementFuel;
static tp_menu_Element_t locElementFuelHand;
static tp_menu_Element_t locElementWeather;
static tp_menu_Element_t locElementWarningSign;
static tp_menu_Element_t locElementFan;
static tp_menu_Element_t locElementFanSelect;

/***********************************************************
  Variable: locSpriteFanAnimation

  Array for addresses of the fan's animation frames.
*/
static uint32_t locSpriteFanAnimation[5];

/***********************************************************
  Variable: locName

  Tool tips for some of the GUI elements.
*/
static char locNameRadio[]      = "Radio settings...";
static char locNameMic[]        = "Microphone settings...";
static char locNameVolume[]     = "Volume settings...";
static char locNameSync[]       = "Synchronization...";
static char locNameFanSelect[]  = "Fan control...";


/***********************************************************
  Variable: locStackAppTestThread

  d3D Thread local stack
*/
static uint32_t locStackAppTestThread[LOC_APP_TEST_THREAD_STACKSIZE >> 2];


/***********************************************************
  Variable: locAppTestThreadId

  Id of the test thread
*/

static uint32_t locAppTestThreadId;


/***********************************************************
  Variable: locQuit

  Signal flag to leave the application.
*/

static volatile uint8_t locQuit;


/***********************************************************
  Variable: locImplDev

  Drw2D Implementation Drawing handle
*/
#ifdef R_DRW2D_SYS_DHD
static struct r_drw2d_DeviceDHD_s locImplDev;
#else
static struct r_drw2d_DeviceCPU_s locImplDev;
#endif

/***********************************************************

  Variable: loc_Drw2dDev

  Pointer to the Drw2D device handle
*/

static r_drw2d_Device_t locDrw2dDev;


/***********************************************************

  Variable: locSema

  Semaphore to stop the render thread from running.
*/

static fw_osal_Sema_t locSema;


/***********************************************************

  Variable: locSemaArm

  Variable to arm locSema semaphore (arm == 1, disarm == 0).
*/

static int32_t locSemaArm;


/***********************************************************
  Variable: locRetVal

  Thread return value;
*/
static void *locRetVal;

/***********************************************************
  Variable: locStatusBar*

  Buffers for the status bar text and tool tip.
*/
static char locStatusBarText[32];
static char locStatusBarTip[32];

/***********************************************************
  Variable: locStatusBarTipCount

  Counter for how long the tool tip still has to be displayed.
*/
static int32_t locStatusBarTipCount;

/***********************************************************
  Variable: locStatusBarDirty

  Indicates if the status bar or tool tip has to be redrawn
  (0 = non dirty).
*/
static int32_t locStatusBarDirty;

/***********************************************************
  Variable: locWarningSignStatus

  Current state of the fuel warning sign (see
  locOnVBlankWarningSign).
*/
static int32_t locWarningSignStatus;

/***********************************************************
  Variable: locWarningSignStatus

  Current fan speed (0-5).
*/
static int32_t locFanSpeed;

/***********************************************************
  Variable: locFanSpeedText

  Buffer for the fan speed text to be changed during
  runtime (change of the speed digit).
*/
static char locFanSpeedText[] = LOC_FAN_SPEED_TEXT;


/***********************************************************
  Section: Local Functions
*/

static void locStatusBarShowTip(char *String);


/***********************************************************
  Function: locKnobPress

  KNOB pressed callback function.

  Parameters:
  void

  Returns:
  void
*/
static void locKnobPress(void)
{
    TP_GUI_ProcessInput(TP_GUI_INPUT_KNOB_PRESS);
}


/***********************************************************
  Function: locKnobRight

  KNOB right callback function.

  Parameters:
  void

  Returns:
  void
*/

static void locKnobRight(void)
{
    TP_GUI_ProcessInput(TP_GUI_INPUT_KNOB_RIGHT);
}


/***********************************************************
  Function: locKnobLeft

  KNOB left callback function.

  Parameters:
  void

  Returns:
  void
*/
static void locKnobLeft(void)
{
    TP_GUI_ProcessInput(TP_GUI_INPUT_KNOB_LEFT);
}


/*******************************************************************************
  Function: locSetRenderTarget

  Sets the target for the subsequent render commands to a current draw buffer
  of a window (its back-buffer).

  Parameters:
  Win - VOVIAL surface (window).

  Returns:
  A pointer to the buffer.
*/
static void* locSetRenderTarget(void* Win)
{
    r_drw2d_Framebuffer_t fb;
    fw_vovial_FbFormat_t fmt;

    /* Init framebuffer struct */
    fb.Flags = (r_drw2d_FramebufferFlags_t) 0;
    fb.Handle = 0; /* not used currently */
    FW_VOVIAL_GetBufferInfo(Win, &fmt, &fb.Buffer.Pitch, &fb.Buffer.Size.Width, &fb.Buffer.Size.Height);
    fb.Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
    fb.Buffer.Data = FW_VOVIAL_GetFrameBuffer(Win);
    R_DRW2D_FramebufferSet(locDrw2dDev, &fb);

    return fb.Buffer.Data;
}


/*******************************************************************************
  Function: locStatusBarShowTip

  Shows the given string as tool tip for a few seconds.

  Parameters:
   String - The tool tip string to be shown.

  Returns:
   void
*/
static void locStatusBarShowTip(char *String)
{
    strcpy(locStatusBarTip, String);
    locStatusBarTipCount = 80;
    locStatusBarDirty += 2;
}

/*******************************************************************************
  Function: locStatusBarRender

  Render the status bar. The status bar must be marked as dirty by setting
  locStatusBarDirty != 0 to render the status bar.
  The value of locStatusBarDirty determines hfor how many frames the bar will be
  rendered, in order to also fill the back buffers.

  Parameters:
   void

  Returns:
   void
*/
static void locStatusBarRender(void)
{
    if(locStatusBarDirty != 0)
    {
        r_drw2d_IntRect_t rect;

        rect.Pos.X = rect.Pos.Y = 0;
        rect.Size.Width = 240;
        rect.Size.Height = 15;

        R_DRW2D_CtxClipRect(locDrw2dDev, &rect);
        R_DRW2D_FramebufferClear(locDrw2dDev);
        TP_FONT_SetFontHeight(TP_FONT_GetFontHeight()-2);

        if(locStatusBarTipCount > 0)
        {
            TP_FONT_Print(locDrw2dDev, 0, -1, 0xffffffff, locStatusBarTip);
        }
        else
        {
            TP_FONT_Print(locDrw2dDev, 0, -1, 0xffffffff, locStatusBarText);
        }

        --locStatusBarDirty;
    }
    else
    {
        if(locStatusBarTipCount > 0)
        {
            --locStatusBarTipCount;
            if(locStatusBarTipCount == 0)
            {
                locStatusBarDirty += 2;
            }
        }
    }
}


/***********************************************************
  Function: locVBlank

  VBlank callback function, called when a VBlank interrupt
  occurs.
*/
static void locVBlank(void)
{
    if(locSemaArm == 1)
    {
        FW_OSAL_SemaSignal(&locSema);
    }
}


/***********************************************************
  Function: locAppTestThread

  The application's main thread.

  Parameters:
  Arg        - Thread parameter

  Returns:
  ==0        - OK
  !=0        - NG
*/
static void *locAppTestThread(void *Arg)
{
    uint32_t i = 0;
    uint32_t TimeStart = 0;
    uint32_t TimeEnd = 1;
    float32_t FPS;

    R_DRW2D_CtxBgColor(locDrw2dDev, 0x00000000);
    TP_FONT_SelectFont(&FontArial19);

    locSetRenderTarget(locSurface);
    R_DRW2D_FramebufferClear(locDrw2dDev);

    TP_FONT_SetFontHeight(TP_FONT_GetFontHeight()+2);
    TP_FONT_Print(locDrw2dDev, 80, 31, 0xffc3b5ff, "14 °C");
    TP_FONT_Print(locDrw2dDev, 130, 31, 0xffffffff, "partly cloudy");

    R_DRW2D_GpuFinish(locDrw2dDev, R_DRW2D_FINISH_WAIT);
    FW_VOVIAL_SwapFrameBuffer(locSurface, i);
    ++i;

    locSemaArm = 1;
    FW_OSAL_SemaWait(&locSema);

    locSetRenderTarget(locSurface);
    R_DRW2D_FramebufferClear(locDrw2dDev);

    TP_FONT_SetFontHeight(TP_FONT_GetFontHeight()+2);
    TP_FONT_Print(locDrw2dDev, 80, 31, 0xffc3b5ff, "14 °C");
    TP_FONT_Print(locDrw2dDev, 130, 31, 0xffffffff, "partly cloudy");

    R_DRW2D_GpuFinish(locDrw2dDev, R_DRW2D_FINISH_WAIT);
    FW_VOVIAL_SwapFrameBuffer(locSurface, i);
    ++i;

    locSemaArm = 1;
    FW_OSAL_SemaWait(&locSema);

    /* The frame buffer is cleared now, enable all the surfaces. */
    FW_VOVIAL_EnableSurface(locSurface);
    FW_VOVIAL_EnableSurface(locSurfaceSprite);
    FW_VOVIAL_EnableSurface(locSurfaceSpriteTop);
    FW_VOVIAL_EnableSurface(locSurfaceRLE);

    while(!locQuit)
    {
        locSetRenderTarget(locSurface);

        locStatusBarRender();

        R_DRW2D_GpuFinish(locDrw2dDev, R_DRW2D_FINISH_WAIT);
        FW_VOVIAL_SwapFrameBuffer(locSurface, i);
        ++i;

        locSemaArm = 1;
        FW_OSAL_SemaWait(&locSema);
        if((i % 300) == 11)
        {
            TimeEnd = FW_OSAL_TimeGet();
            FPS = 300.0f / ((TimeEnd - TimeStart)/1000.0f);
            printf("FPS: %f\n", FPS);
            TimeStart = FW_OSAL_TimeGet();
        }
        TP_GUI_NotifyVBlank();
    }

    FW_OSAL_ThreadExit((void*) 666);

    return 0;
}


/*******************************************************************************
  Function: SetupSprite

  Sets up the sprite configuration based on the given Img_t object and
  coordinates.

  Parameters:
   Image - The sprite's image.
   Conf  - The sprite configuration that will be filled.
   X     - X position of the sprite.
   Y     - Y position of the sprite.
   Z     - 0 for lower sprite layer, 1 for the sprite layer on top (for overlay
           effects)

  Returns:
   void
*/
static void SetupSprite(const Img_t *Image, fw_vovial_Sprite_t *Conf, uint16_t X, uint16_t Y, uint16_t Z)
{
    if(Z==1)
    {
        Conf->Surface = locSurfaceSpriteTop;
    }
    else
    {
        Conf->Surface = locSurfaceSprite;
    }
    Conf->Status = FW_VOVIAL_SPRITESTATUS_NOT_INITIALIZED;
    Conf->Data = *Image->Data;
    Conf->Width = Image->Width;
    Conf->Height = Image->Height;
    Conf->PosX = X;
    Conf->PosY = Y;
    Conf->PosZ = 0;
}


/*******************************************************************************
  Function: locOnVBlankFuelHand

  VBlank callback of the fuel gauge's hand sprite.
  The function is responsible for moving the hand.

  See app_tripmcomp_menu.h.
*/
static void locOnVBlankFuelHand(tp_menu_Element_t *Element)
{
    static int32_t count = 0;
    static int32_t status = 1;

    if((locWarningSignStatus == 0) && (Element->SpriteConf->PosX < 150))
    {
        locWarningSignStatus = 1;
    }
    else if((locWarningSignStatus != 0) && (Element->SpriteConf->PosX > 150))
    {
        locWarningSignStatus = 5;
    }

    if(1 == status)
    {
        /* Loading */
        TP_GUI_MenuMoveElement(Element, Element->SpriteConf->PosX + 1,
                               Element->SpriteConf->PosY,
                               Element->SpriteConf->PosZ);
        if(216 <= Element->SpriteConf->PosX)
        {
            status = 0;
        }
    }
    else
    {
        ++count;
        if((count % 30) == 0)
        {
            TP_GUI_MenuMoveElement(Element, Element->SpriteConf->PosX - 1,
                                   Element->SpriteConf->PosY,
                                   Element->SpriteConf->PosZ);
            if(132 >= Element->SpriteConf->PosX)
            {
                status = 1;
            }
        }
    }
}


/*******************************************************************************
  Function: locOnVBlankWarningSign

  VBlank callback of the fuel gauge's warning sign.
  The function is responsible for enabling and disabling the warning sign
  (blinking effect).

  See app_tripmcomp_menu.h.
*/
static void locOnVBlankWarningSign(tp_menu_Element_t *Element)
{
    static int32_t count = 0;

    switch(locWarningSignStatus)
    {
    case 0:
        TP_GUI_MenuDisableElement(Element);
        break;
    case 1:
        TP_GUI_MenuEnableElement(Element);
        count = 0;
        locWarningSignStatus = 2;
        break;
    case 2:
        ++count;
        if(count > 40)
        {
            locWarningSignStatus = 3;
        }
        break;
    case 3:
        TP_GUI_MenuDisableElement(Element);
        count = 0;
        locWarningSignStatus = 4;
        break;
    case 4:
        ++count;
        if(count > 40)
        {
            locWarningSignStatus = 1;
        }
        break;
    case 5:
        TP_GUI_MenuDisableElement(Element);
        locWarningSignStatus = 0;
        break;
    }
}

/*******************************************************************************
  Function: locOnFocusFan

  OnFocus callback of the fan.
  The function is responsible for the highlighting of the fan, when it gains
  focus. Therefore, an overlay effect with another sprite is used.

  See app_tripmcomp_menu.h.
*/
static void locOnFocusFan(tp_menu_Element_t *Element, int32_t Focus)
{
    if(1 == Focus)
    {
        TP_GUI_MenuEnableElement(&locElementFanSelect);
    }
    else
    {
        TP_GUI_MenuDisableElement(&locElementFanSelect);
    }
}

static int32_t locOnInputElement(tp_menu_Element_t *Element, tp_gui_Input_t Input)
{
    Cmd_t cmd;

    if(Input == TP_GUI_INPUT_KNOB_PRESS)
    {
        cmd.Cmd = CMD_STOPSTART;
        cmd.Par1 = 0;
        cmd.Par2 = (AppNum - 1);
        CmdSend(&cmd);

        return 1;
    }

    return 0;
}


/*******************************************************************************
  Function: locOnInputFan

  OnInput callback of the fan.
  The function is responsible for changing the highlight's color when the fan
  GUI element gains control and for setting the fan speed.

  See app_tripmcomp_menu.h.
*/
static int32_t locOnInputFan(tp_menu_Element_t *Element, tp_gui_Input_t Input)
{
    static int32_t Status = 0;

    if(0 == Status)
    {
        if(TP_GUI_INPUT_KNOB_PRESS == Input)
        {
            TP_GUI_MenuHighlightElement(&locElementFanSelect);
            Status = 1;

            return 1;
        }

        return 0;
    }
    else
    {
        switch(Input)
        {
        case TP_GUI_INPUT_KNOB_PRESS:
            TP_GUI_MenuLowlightElement(&locElementFanSelect);
            Status = 0;
            return 1;
        case TP_GUI_INPUT_KNOB_LEFT:
            if(locFanSpeed > 0)
            {
                --locFanSpeed;

                locFanSpeedText[sizeof(locFanSpeedText) - 2] = 48 + locFanSpeed;
            }
            locStatusBarShowTip(locFanSpeedText);
            break;
        case TP_GUI_INPUT_KNOB_RIGHT:
            if(locFanSpeed < 5)
            {
                ++locFanSpeed;

                locFanSpeedText[sizeof(locFanSpeedText) - 2] = 48 + locFanSpeed;
            }
            locStatusBarShowTip(locFanSpeedText);
            break;
        }

        if(0 == locFanSpeed)
        {
            locElementFan.AnimationSpeed = 0;
        }
        else
        {
            locElementFan.AnimationSpeed = 45 * (LOC_FAN_SPEED_MAX - locFanSpeed) + 30;
        }

        return 1;
    }
}

/*******************************************************************************
  Function: SetupMenu

  Sets up the TP GUI's elements used in the demo and adds them to the TP GUI
  framework.

  Parameters:
   void

  Returns:
   void
*/
static void SetupMenu(void)
{
    uint32_t width = 0;

    width = 12;
    SetupSprite(&Img_tp_radio_normal, &locSpriteRadio, width, LOC_DISPLAY_HEIGHT - 39, 0);
    width += Img_tp_radio_normal.Width + 20;
    SetupSprite(&Img_tp_Mic_Normal, &locSpriteMic, width, LOC_DISPLAY_HEIGHT - 40, 0);
    width += Img_tp_Mic_Normal.Width + 20;
    SetupSprite(&Img_tp_volume_normal, &locSpriteVolume, width, LOC_DISPLAY_HEIGHT - 39, 0);
    width += Img_tp_volume_normal.Width + 20;
    SetupSprite(&Img_tp_Sync_Normal, &locSpriteSync, width, LOC_DISPLAY_HEIGHT - 40, 0);

    SetupSprite(&Img_tp_fuel_gauge, &locSpriteFuel, 107, LOC_DISPLAY_HEIGHT - 86, 0);
    SetupSprite(&Img_tp_fuel_hand, &locSpriteFuelHand, 132, LOC_DISPLAY_HEIGHT - 98, 1);
    SetupSprite(&Img_tp_Weather_Icon, &locSpriteWeather, 17, 20, 0);
    SetupSprite(&Img_tp_warning_sign, &locSpriteWarningSign, 109, LOC_DISPLAY_HEIGHT - 91, 0);
    SetupSprite(&Img_fan_0, &locSpriteFan, 7, LOC_DISPLAY_HEIGHT - 107, 0);
    SetupSprite(&Img_fan_select, &locSpriteFanSelect, 4, LOC_DISPLAY_HEIGHT - 110, 1);

    locElementRadio.AddressNormal = (uint32_t) *Img_tp_radio_normal.Data;
    locElementRadio.AddressHighlighted = (uint32_t) *Img_tp_radio_selected.Data;
    locElementRadio.AddressAnimation = 0;
    locElementRadio.IsFocusable = 1;
    locElementRadio.OnFocus = 0;
    locElementRadio.OnVBlank = 0;
    locElementRadio.OnInput = locOnInputElement;
    locElementRadio.SpriteConf = &locSpriteRadio;
    locElementRadio.Tip = locNameRadio;

    locElementMic.AddressNormal = (uint32_t) *Img_tp_Mic_Normal.Data;
    locElementMic.AddressHighlighted = (uint32_t) *Img_tp_Mic_Selected.Data;
    locElementMic.AddressAnimation = 0;
    locElementMic.IsFocusable = 1;
    locElementMic.OnFocus = 0;
    locElementMic.OnVBlank = 0;
    locElementMic.OnInput = locOnInputElement;
    locElementMic.SpriteConf = &locSpriteMic;
    locElementMic.Tip = locNameMic;

    locElementVolume.AddressNormal = (uint32_t) *Img_tp_volume_normal.Data;
    locElementVolume.AddressHighlighted = (uint32_t) *Img_tp_volume_selected.Data;
    locElementVolume.AddressAnimation = 0;
    locElementVolume.IsFocusable = 1;
    locElementVolume.OnFocus = 0;
    locElementVolume.OnVBlank = 0;
    locElementVolume.OnInput = locOnInputElement;
    locElementVolume.SpriteConf = &locSpriteVolume;
    locElementVolume.Tip = locNameVolume;

    locElementSync.AddressNormal = (uint32_t) *Img_tp_Sync_Normal.Data;
    locElementSync.AddressHighlighted = (uint32_t) *Img_tp_Sync_Selected.Data;
    locElementSync.AddressAnimation = 0;
    locElementSync.IsFocusable = 1;
    locElementSync.OnFocus = 0;
    locElementSync.OnVBlank = 0;
    locElementSync.OnInput = locOnInputElement;
    locElementSync.SpriteConf = &locSpriteSync;
    locElementSync.Tip = locNameSync;

    locElementFuel.AddressNormal = (uint32_t) *Img_tp_fuel_gauge.Data;
    locElementFuel.AddressHighlighted = 0u;
    locElementFuel.AddressAnimation = 0;
    locElementFuel.IsFocusable = 0;
    locElementFuel.OnFocus = 0;
    locElementFuel.OnVBlank = 0;
    locElementFuel.OnInput = 0;
    locElementFuel.SpriteConf = &locSpriteFuel;
    locElementFuel.Tip = 0;

    locElementFuelHand.AddressNormal = (uint32_t) *Img_tp_fuel_hand.Data;
    locElementFuelHand.AddressHighlighted = 0u;
    locElementFuelHand.AddressAnimation = 0;
    locElementFuelHand.IsFocusable = 0;
    locElementFuelHand.OnFocus = 0;
    locElementFuelHand.OnVBlank = locOnVBlankFuelHand;
    locElementFuelHand.OnInput = 0;
    locElementFuelHand.SpriteConf = &locSpriteFuelHand;
    locElementFuelHand.Tip = 0;

    locElementWeather.AddressNormal = (uint32_t) *Img_tp_Weather_Icon.Data;
    locElementWeather.AddressHighlighted = 0u;
    locElementWeather.AddressAnimation = 0;
    locElementWeather.IsFocusable = 0;
    locElementWeather.OnFocus = 0;
    locElementWeather.OnVBlank = 0;
    locElementWeather.OnInput = 0;
    locElementWeather.SpriteConf = &locSpriteWeather;
    locElementWeather.Tip = 0;

    locElementWarningSign.AddressNormal = (uint32_t) *Img_tp_warning_sign.Data;
    locElementWarningSign.AddressHighlighted = 0u;
    locElementWarningSign.AddressAnimation = 0;
    locElementWarningSign.IsFocusable = 0;
    locElementWarningSign.OnFocus = 0;
    locElementWarningSign.OnVBlank = locOnVBlankWarningSign;
    locElementWarningSign.OnInput = 0;
    locElementWarningSign.SpriteConf = &locSpriteWarningSign;
    locElementWarningSign.Tip = 0;

    locElementFan.AddressNormal = (uint32_t) *Img_fan_0.Data;
    locElementFan.AddressHighlighted = 0u;
    locElementFan.AddressAnimation = locSpriteFanAnimation;
    locElementFan.AnimationFrameCount = 5;
    locElementFan.AnimationSpeed = 45 * (LOC_FAN_SPEED_MAX - locFanSpeed) + 30;
    locElementFan.IsFocusable = 1;
    locElementFan.OnFocus = locOnFocusFan;
    locElementFan.OnVBlank = 0;
    locElementFan.OnInput = locOnInputFan;
    locElementFan.SpriteConf = &locSpriteFan;
    locElementFan.Tip = locNameFanSelect;

    locSpriteFanAnimation[0] = (uint32_t) *Img_fan_0.Data;
    locSpriteFanAnimation[1] = (uint32_t) *Img_fan_1.Data;
    locSpriteFanAnimation[2] = (uint32_t) *Img_fan_2.Data;
    locSpriteFanAnimation[3] = (uint32_t) *Img_fan_3.Data;
    locSpriteFanAnimation[4] = (uint32_t) *Img_fan_4.Data;

    locElementFanSelect.AddressNormal = (uint32_t) *Img_fan_select.Data;
    locElementFanSelect.AddressHighlighted = (uint32_t) *Img_fan_focus.Data;
    locElementFanSelect.AddressAnimation = 0;
    locElementFanSelect.IsFocusable = 0;
    locElementFanSelect.OnFocus = 0;
    locElementFanSelect.OnVBlank = 0;
    locElementFanSelect.OnInput = 0;
    locElementFanSelect.SpriteConf = &locSpriteFanSelect;
    locElementFanSelect.Tip = 0;

    TP_GUI_MenuAddElement(&locElementRadio);
    TP_GUI_MenuAddElement(&locElementMic);
    TP_GUI_MenuAddElement(&locElementVolume);
    TP_GUI_MenuAddElement(&locElementSync);
    TP_GUI_MenuAddElement(&locElementFuel);
    TP_GUI_MenuAddElement(&locElementFuelHand);
    TP_GUI_MenuAddElement(&locElementWeather);
    TP_GUI_MenuAddElement(&locElementWarningSign);
    TP_GUI_MenuAddElement(&locElementFan);
    TP_GUI_MenuAddElement(&locElementFanSelect);
}

void locCleanUpVRAM(void)
{
    int32_t i;

    for(i = 0; i < LOC_VRAM_IMAGE_COUNT; ++i)
    {
        R_CDI_Free((uint32_t) locAllocedVRAM[i], &loc_VRAM_heap);
    }
}

void locMoveToVRAM(const Img_t *Image)
{
    static int32_t block_idx = 0;
    uint8_t *vram;
    int32_t i;
    uint32_t align, offset;

    /* Take 6 more bytes than needed to leave room for alignment. */
    vram = (uint8_t*) R_CDI_Alloc(&loc_VRAM_heap, Image->DataLength + 6);
    locAllocedVRAM[block_idx] = (void*) vram;
    block_idx = (block_idx + 1) % LOC_VRAM_IMAGE_COUNT;

    /* Get the offset within VRAM. */
    offset = (uint32_t) vram - loc_VRAM0;

    /* The data must be aligned to an address divisible by 6, since we are
     * dealing with a 3 bytes per pixel format and SPEA needs a 2 pixel
     * alignment. */
    align = 6 - (offset % 6);

    /* Make the address divisible by 3 */
    offset = offset + align;

    for(i = 0; i < Image->DataLength; ++i)
    {
        ((uint8_t*) loc_VRAM0 + offset)[i] = (*Image->Data)[i];
    }

    /* Compute address in wrapper address space. */
    offset = offset / 3 * 4;
    *Image->Data = (uint8_t*) (loc_VRAM0_WRAP_ARGB6666 + offset);
}

/***********************************************************
  Function: locInit

  Test application init function.

  The function creates the required surfaces and initializes
  Drw2D.
  Afterwards, the space invaders alien image is initialized
  and the app's main thread is created.

  Parameters:
  void

  Returns:
  void
*/
static void locInit(void)
{
    r_drw2d_Error_t err;

    locQuit = 0;
    locSemaArm = 0;

    locSurface = FW_VOVIAL_CreateSurface(
            LOC_VOVIAL_UNIT,
            FW_VOVIAL_ARGB8888,
            0,
            0,
            3,
            LOC_DISPLAY_STRIDE,
            LOC_DISPLAY_WIDTH,
            64,
            0xff,
            2,
            0);

    if(locSurface == 0)
    {
        Error(ERR_VOVIAL);
    }

    locSurfaceSprite = FW_VOVIAL_CreateSurface(
                LOC_VOVIAL_UNIT,
                FW_VOVIAL_ARGB8888,
                0,
                0,
                1,
                0,
                LOC_DISPLAY_WIDTH,
                LOC_DISPLAY_HEIGHT,
                0xff,
                1,
                FW_VOVIAL_CRFLAG_SPRITE);

    if(locSurfaceSprite == 0)
    {
        Error(ERR_VOVIAL);
    }

    locSurfaceSpriteTop = FW_VOVIAL_CreateSurface(
                    LOC_VOVIAL_UNIT,
                    FW_VOVIAL_ARGB8888,
                    0,
                    0,
                    2,
                    0,
                    LOC_DISPLAY_WIDTH,
                    LOC_DISPLAY_HEIGHT,
                    0xff,
                    1,
                    FW_VOVIAL_CRFLAG_SPRITE);

    if(locSurfaceSpriteTop == 0)
    {
        Error(ERR_VOVIAL);
    }

    locSurfaceRLE = FW_VOVIAL_CreateSurface(
                LOC_VOVIAL_UNIT,
                FW_VOVIAL_RLE24RGB0888,
                0,
                0,
                0,
                LOC_DISPLAY_STRIDE,
                LOC_DISPLAY_WIDTH,
                LOC_DISPLAY_HEIGHT,
                0xff,
                1,
                FW_VOVIAL_CRFLAG_RLE);

    if(locSurfaceRLE == 0)
    {
        Error(ERR_VOVIAL);
    }

    err = R_DRW2D_Init();
    if(err != R_DRW2D_ERR_OK)
    {
        Error(ERR_DRW2D);
    }

    err = R_DRW2D_Open(0, R_DRW2D_UNIT_DHD0, &locImplDev, &locDrw2dDev);
    if(err != R_DRW2D_ERR_OK)
    {
        Error(ERR_DRW2D);
    }

    locOldBg = Init2DImg((Img_t*) &Img_tp_bg);
    locOldMicN = Init2DImg((Img_t*) &Img_tp_Mic_Normal);
    locOldMicS = Init2DImg((Img_t*) &Img_tp_Mic_Selected);
    locOldSyncN = Init2DImg((Img_t*) &Img_tp_Sync_Normal);
    locOldSyncS = Init2DImg((Img_t*) &Img_tp_Sync_Selected);
    locOldRadioN = Init2DImg((Img_t*) &Img_tp_radio_normal);
    locOldRadioS = Init2DImg((Img_t*) &Img_tp_radio_selected);
    locOldVolumeN = Init2DImg((Img_t*) &Img_tp_volume_normal);
    locOldVolumeS = Init2DImg((Img_t*) &Img_tp_volume_selected);
    locOldFuelGauge = Init2DImg((Img_t*) &Img_tp_fuel_gauge);
    locOldFuelHand = Init2DImg((Img_t*) &Img_tp_fuel_hand);
    locOldWeatherIcon = Init2DImg((Img_t*) &Img_tp_Weather_Icon);
    locOldWarningSign = Init2DImg((Img_t*) &Img_tp_warning_sign);
    locOldFan_0 = Init2DImg((Img_t*) &Img_fan_0);
    locOldFan_1 = Init2DImg((Img_t*) &Img_fan_1);
    locOldFan_2 = Init2DImg((Img_t*) &Img_fan_2);
    locOldFan_3 = Init2DImg((Img_t*) &Img_fan_3);
    locOldFan_4 = Init2DImg((Img_t*) &Img_fan_4);
    locOldFanSelect = Init2DImg((Img_t*) &Img_fan_select);
    locOldFanFocus = Init2DImg((Img_t*) &Img_fan_focus);
    locOldFontArial19 = Init2DImg((Img_t*) FontArial19.Texture);

    locMoveToVRAM(&Img_tp_Mic_Normal);
    locMoveToVRAM(&Img_tp_Mic_Selected);
    locMoveToVRAM(&Img_tp_Sync_Normal);
    locMoveToVRAM(&Img_tp_Sync_Selected);
    locMoveToVRAM(&Img_tp_radio_normal);
    locMoveToVRAM(&Img_tp_radio_selected);
    locMoveToVRAM(&Img_tp_volume_normal);
    locMoveToVRAM(&Img_tp_volume_selected);
    locMoveToVRAM(&Img_tp_fuel_gauge);
    locMoveToVRAM(&Img_tp_fuel_hand);
    locMoveToVRAM(&Img_tp_Weather_Icon);
    locMoveToVRAM(&Img_tp_warning_sign);
    locMoveToVRAM(&Img_fan_0);
    locMoveToVRAM(&Img_fan_1);
    locMoveToVRAM(&Img_fan_2);
    locMoveToVRAM(&Img_fan_3);
    locMoveToVRAM(&Img_fan_4);
    locMoveToVRAM(&Img_fan_select);
    locMoveToVRAM(&Img_fan_focus);

    FW_VOVIAL_SetBufferAddr(locSurfaceRLE, 1, (void**) Img_tp_bg.Data);

    strcpy(locStatusBarText, LOC_STATUSBAR_TEXT);
    locWarningSignStatus = 0;
    locStatusBarDirty = 2;
    locFanSpeed = 3;

    TP_GUI_Init();
    TP_GUI_RegisterShowToolTip(locStatusBarShowTip);

    SetupMenu();

    TP_GUI_Show();
    TP_GUI_MenuDisableElement(&locElementFanSelect);

    FW_OSAL_SemaCreate(&locSema);

    FW_VOVIAL_RegisterVBlankIsr(locSurface, &locVBlank);

    locAppTestThreadId = FW_OSAL_ThreadCreate(locAppTestThread, 0,
                                           locStackAppTestThread,
                                           LOC_APP_TEST_THREAD_STACKSIZE,
                                           LOC_APP_TEST_THREAD_PRIO);
}


/***********************************************************
  Function: locDeInit

  Test application deinit function.

  The function releases all created resources and joins the
  app's thread by setting locQuit to 1, which will cause
  the main thread to end.

  Parameters:
  void

  Returns:
  void
*/

static void locDeInit(void)
{
    /* Signal thread to quit */
    locQuit = 1;

    FW_OSAL_ThreadJoin(locAppTestThreadId, &locRetVal);

    TP_GUI_TearDown();

    locCleanUpVRAM();

    Deinit2DImg((Img_t*) &Img_tp_bg, locOldBg);
    Deinit2DImg((Img_t*) &Img_tp_Mic_Normal, locOldMicN);
    Deinit2DImg((Img_t*) &Img_tp_Mic_Selected, locOldMicS);
    Deinit2DImg((Img_t*) &Img_tp_Sync_Normal, locOldSyncN);
    Deinit2DImg((Img_t*) &Img_tp_Sync_Selected, locOldSyncS);
    Deinit2DImg((Img_t*) &Img_tp_radio_normal, locOldRadioN);
    Deinit2DImg((Img_t*) &Img_tp_radio_selected, locOldRadioS);
    Deinit2DImg((Img_t*) &Img_tp_volume_normal, locOldVolumeN);
    Deinit2DImg((Img_t*) &Img_tp_volume_selected, locOldVolumeS);
    Deinit2DImg((Img_t*) &Img_tp_fuel_gauge, locOldFuelGauge);
    Deinit2DImg((Img_t*) &Img_tp_fuel_hand, locOldFuelHand);
    Deinit2DImg((Img_t*) &Img_tp_Weather_Icon, locOldWeatherIcon);
    Deinit2DImg((Img_t*) &Img_tp_warning_sign, locOldWarningSign);
    Deinit2DImg((Img_t*) &Img_fan_0, locOldFan_0);
    Deinit2DImg((Img_t*) &Img_fan_1, locOldFan_1);
    Deinit2DImg((Img_t*) &Img_fan_2, locOldFan_2);
    Deinit2DImg((Img_t*) &Img_fan_3, locOldFan_3);
    Deinit2DImg((Img_t*) &Img_fan_4, locOldFan_4);
    Deinit2DImg((Img_t*) &Img_fan_select, locOldFanSelect);
    Deinit2DImg((Img_t*) &Img_fan_focus, locOldFanFocus);
    Deinit2DImg((Img_t*) FontArial19.Texture, locOldFontArial19);

    R_DRW2D_Close(locDrw2dDev);
    R_DRW2D_Exit();

    FW_VOVIAL_DeleteSurface(locSurface);
    FW_VOVIAL_DeleteSurface(locSurfaceRLE);
    FW_VOVIAL_DeleteSurface(locSurfaceSprite);
    FW_VOVIAL_DeleteSurface(locSurfaceSpriteTop);

    FW_OSAL_SemaDestroy(&locSema);
}



/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Variable: locAppTestControl

  Control function structure.

  List of callback functions for each control element.
*/

static const fw_hmial_Control_t locAppTripCompControl = {
    locKnobPress,    /* KNOB press callback */
    0,               /* KNOB release callback  */
    locKnobRight,    /* KNOB right callback */
    locKnobLeft,     /* KNOB left callback  */
    0,               /* BUTTON up press callback */
    0,               /* BUTTON up release callback */
    0,               /* BUTTON down press callback  */
    0,               /* BUTTON down release callback */
    0,               /* BUTTON middle press callback */
    0,               /* BUTTON middle release callback */
    0,               /* BUTTON left press callback */
    0,               /* BUTTON left release callback */
    0,               /* BUTTON right press callback */
    0                /* BUTTON right release callback */
};

/***********************************************************
  Section: Global Variables

  Description see: <app_test.h>
*/

/***********************************************************
  Variable: AppTest
*/

const App_t AppTripComp = {
    &Img_icon_tripcomp,
    "Trip Computer Demo",
    "Uses D1L features to implement a trip computer",
    &locAppTripCompControl,
    0,
    locInit,
    locDeInit
};
