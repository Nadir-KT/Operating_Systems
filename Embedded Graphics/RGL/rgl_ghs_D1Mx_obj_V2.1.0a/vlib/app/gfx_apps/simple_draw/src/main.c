/************************************************************************************************
PROJECT : simple_draw
FILE    : $Id: main.c 17203 2020-06-03 02:39:46Z shinya.tomari.wj@renesas.com $
============================================================================ 
DESCRIPTION
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

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"        /* Renesas basic types, e.g. uint32_t */
#include "r_bsp_api.h"         /* Board support package */
#include "r_bsp_hmi_api.h"     /* Board support package - HMI (Buttons + Rotary) */
#include "r_ddb_api.h"
#include "r_cdi_api.h"
#include "r_wm_api.h"
#include "davehd_driver.h"

#include "main.h"
#include "wm.h"
#include "mem_init.h"
#include "r_util_dhd.h"

#include "config.h"
#include "r_drw2d_os.h"
#include "r_drw2d_api.h"
#include "r_config_drw2d.h"
#include "r_drw2d_ctx_dhd.h"

#include "r_jcua_api.h"
#include "r_bsp_stdio_api.h"

/***********************************************************
  Section: Local Defines
*/
/* bitmap widths */
#define LOC_GEAR_W    (104)
/* bitmap heights */
#define LOC_GEAR_H    (104)
/* bitmap sizes */
#define LOC_GEAR_SIZE (LOC_GEAR_W * LOC_GEAR_H * 4)

#define LOC_SF_BMP_W   (30)
#define LOC_SF_BMP_H   (30)

#define LOC_LOGO_W     (36)
#define LOC_LOGO_H     (30)
#define LOC_LOGO_SIZE  (LOC_LOGO_W * LOC_LOGO_H * 4)

#if (defined(BSP_INIT_OCTA_FLASH_R) || defined(BSP_INIT_OCTA_RAM_RW) || defined(BSP_INIT_OCTA_PIN))
/* OCTA RAM/Flash */
#define LOC_EX_MEM_TYPE   (1)
#elif (defined(BSP_INIT_HYPB_FLASH_R) || defined(BSP_INIT_HYPB_RAM_RW) || defined(BSP_INIT_HYPB_PIN))
/* Hyper RAM/Flash */
#define LOC_EX_MEM_TYPE   (2)
#else
/* Serial Flash */
#define LOC_EX_MEM_TYPE   (0)
#endif

/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Variable: loc_WmWindow

  Pointer to the Window handles
*/
r_wm_Window_t loc_WmWindow;
static r_wm_Window_t loc_SpriteWindow;
static r_wm_Window_t loc_SpriteWindow2;

/***********************************************************
  Variable: loc_SpriteData

  Pointer to the Sprite handles
*/
static r_wm_Sprite_t loc_SpriteData;
static r_wm_Sprite_t loc_SpriteData2;

/***********************************************************
  Variable: loc_Discom

  Pointer to the DISCOM handles
*/
r_wm_Discom_t loc_Discom;

/***********************************************************
  Variable: loc_DiscomMismatch / loc_VocaMismatch

  Coutner of DISCOM/VOCA mismatch event
*/
uint32_t loc_DiscomCrcVal   = 0;
uint32_t loc_DiscomMismatch = 0;
uint32_t loc_VocaMismatch   = 0;

/***********************************************************

  Variable: loc_Drw2dDev

  Pointer to the Drw2D device handle
*/

static r_drw2d_Device_t loc_Drw2dDev;

/***********************************************************
  Variable: loc_Fb

  Framebuffer information for Drw2D
*/
static r_drw2d_Framebuffer_t loc_Fb;

/***********************************************************
  Variable: g_BlackKingBmp

  the Bitmap (external)
*/
extern const uint8_t g_BlackKingBmp[LOC_KING_SIZE];
extern const uint8_t g_GearBmp[LOC_GEAR_SIZE];
extern const uint8_t g_LogoRBmp[LOC_LOGO_SIZE];

/***********************************************************
  Variable: loc_lRAM_heap, loc_VRAM_heap

  control structures for CDI heaps
*/
extern r_cdi_Heap_t       loc_lRAM_heap;
extern r_cdi_Heap_t       loc_VRAM_heap;

/***********************************************************
  Variable: loc_DHDDev

  DHD Drawing handle
*/
static struct r_drw2d_DeviceDHD_s loc_DHDDev;

/***********************************************************
  Variable: loc_ExitFlag

  flag to test exit pathes
*/
static int8_t loc_ExitFlag = R_FALSE;

/***********************************************************
  Variable: loc_GPU_Memory_Addr

  dedicated memory area for GPU (DaveHD)
*/
extern uint32_t loc_GPU_Memory_Addr;


/***********************************************************
  Variable: loc_Voca_voca_warping

  Voca monitor area structure
*/
extern r_wm_Voca_t loc_Voca_warping10;
extern r_wm_Voca_t loc_Voca_warping80;

/***********************************************************
  Section: Local Functions
*/

/***********************************************************
 Function: loc_DRW2DErrorHandler

 Error handler used for Drw2d errors
*/
static r_drw2d_Boolean_t loc_DRW2DErrorHandler(r_drw2d_Error_t  Error, void *UserData)
{
    loc_Error(1);
    return R_TRUE;
}


/***********************************************************
  Section: Local Functions
*/

/*******************************************************************************
  Function: locBtnOkPress + locRotaryCenterPress

  Button and Rotary press callback functions
*/
static void locBtnOkPress(void) {
    loc_ExitFlag = R_TRUE;
}

static void locRotaryCenterPress(void) {
    loc_ExitFlag = R_TRUE;
}


/*******************************************************************************
  Function: loc_RenderClear

  Render function called each frame
*/
static void loc_RenderClear(int32_t Frame)
{
    r_drw2d_Error_t ret = R_DRW2D_ERR_OK;

    /* Set green-ish clear color and fill framebuffer */
    ret |= R_DRW2D_CtxBgColor(loc_Drw2dDev, 0xff00ff00);
    ret |= R_DRW2D_FramebufferClear(loc_Drw2dDev);

    if (R_DRW2D_ERR_OK != ret)
    {
        loc_Error(ret);
    }
}

/*******************************************************************************
  Function: loc_RenderPrimitives

  Render function called each frame
*/
static void loc_RenderPrimitives(int32_t Frame)
{
    r_drw2d_Error_t ret = R_DRW2D_ERR_OK;
    r_drw2d_Rect_t  rect;
    r_drw2d_Point_t triangle[3];

    /* triangle */
    ret |= R_DRW2D_CtxBlendMode(loc_Drw2dDev, R_DRW2D_BLENDMODE_SRC);
    ret |= R_DRW2D_CtxFillMode(loc_Drw2dDev, R_DRW2D_FILLMODE_SOLID);
    ret |= R_DRW2D_CtxFgColor(loc_Drw2dDev, 0xffff0000u);
    triangle[0].X = R_DRW2D_2X(4);
    triangle[0].Y = R_DRW2D_2X(10);
    triangle[1].X = R_DRW2D_2X(66);
    triangle[1].Y = R_DRW2D_2X(124);
    triangle[2].X = R_DRW2D_2X(100);
    triangle[2].Y = R_DRW2D_2X(10);
    ret |= R_DRW2D_DrawTriangles(loc_Drw2dDev, triangle, 1, 0);

    /* rect */
    ret |= R_DRW2D_CtxFgColor(loc_Drw2dDev, 0xff0000ffu);
    rect.Pos.X       = R_DRW2D_2X((LOC_DISPLAY_WIDTH  - 100) / 2);
    rect.Pos.Y       = R_DRW2D_2X(200);
    rect.Size.Width  = R_DRW2D_2X(100);
    rect.Size.Height = R_DRW2D_2X(100);
    ret |= R_DRW2D_DrawRect(loc_Drw2dDev, &rect);

    if (R_DRW2D_ERR_OK != ret)
    {
        loc_Error(ret);
    }
}

/*******************************************************************************
  Function: loc_RenderTexture_intROM

  Render function called each frame
*/
static void loc_RenderTexture_intROM(int32_t Frame)
{
    r_drw2d_Error_t ret = R_DRW2D_ERR_OK;
    r_drw2d_Rect_t  rect;
    r_drw2d_Texture_t tex_argb32;

    tex_argb32.Handle             = R_NULL;
    tex_argb32.Flags              = R_DRW2D_TEX_VT | R_DRW2D_TEX_BILINEAR;
    tex_argb32.Buffer.Pitch       = LOC_GEAR_W;
    tex_argb32.Buffer.Size.Width  = LOC_GEAR_W;
    tex_argb32.Buffer.Size.Height = LOC_GEAR_H;
    tex_argb32.Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
    tex_argb32.Buffer.Data        = (void*)g_GearBmp;

    rect.Pos.X       = R_DRW2D_2X(130);
    rect.Pos.Y       = R_DRW2D_2X(24);
    rect.Size.Width  = R_DRW2D_2X(LOC_GEAR_W);
    rect.Size.Height = R_DRW2D_2X(LOC_GEAR_H);

    /* Set texture */
    ret |= R_DRW2D_CtxTextureSet(loc_Drw2dDev, 0, &tex_argb32);
    ret |= R_DRW2D_CtxFillMode(loc_Drw2dDev, R_DRW2D_FILLMODE_TEXTURE);
    ret |= R_DRW2D_CtxBlendMode(loc_Drw2dDev, R_DRW2D_BLENDMODE_SRC_OVER);

    /* rotate and translate */
    ret |= R_DRW2D_CtxTextureIdentity(loc_Drw2dDev);
    ret |= R_DRW2D_CtxTextureTranslate(loc_Drw2dDev, rect.Pos.X+rect.Size.Width/2, rect.Pos.Y+rect.Size.Height/2);
    ret |= R_DRW2D_CtxTextureRotate(loc_Drw2dDev, R_DRW2D_2X(Frame * 3));
    ret |= R_DRW2D_CtxTextureTranslate(loc_Drw2dDev, -(rect.Size.Width/2), -(rect.Size.Height/2));

    ret |= R_DRW2D_CtxIdentity(loc_Drw2dDev);
    ret |= R_DRW2D_CtxTranslate(loc_Drw2dDev, rect.Pos.X+rect.Size.Width/2, rect.Pos.Y+rect.Size.Height/2, 0);
    ret |= R_DRW2D_CtxRotate(loc_Drw2dDev, R_DRW2D_2X(Frame * 3));
    ret |= R_DRW2D_CtxTranslate(loc_Drw2dDev, -(rect.Pos.X+rect.Size.Width/2), -(rect.Pos.Y+rect.Size.Height/2), 0);

    /* Draw texture into rectangle */
    ret |= R_DRW2D_DrawRect(loc_Drw2dDev, &rect);
    ret |= R_DRW2D_CtxTextureIdentity(loc_Drw2dDev);
    ret |= R_DRW2D_CtxIdentity(loc_Drw2dDev);

    if (R_DRW2D_ERR_OK != ret)
    {
        loc_Error(ret);
    }
}


/*******************************************************************************
  Function: loc_RenderTexture_SFlash

  Render function called each frame
*/
static void loc_RenderTexture_SFlash(int32_t Frame, void* TextureAddr)
{
    r_drw2d_Error_t ret = R_DRW2D_ERR_OK;
    r_drw2d_Rect_t  rect;
    r_drw2d_Texture_t tex_argb32;
    uint32_t xPos, yPos;

    tex_argb32.Handle             = R_NULL;
    tex_argb32.Flags              = (r_drw2d_TextureFlags_t)0;
    tex_argb32.Buffer.Pitch       = LOC_SF_BMP_W;
    tex_argb32.Buffer.Size.Width  = LOC_SF_BMP_W;
    tex_argb32.Buffer.Size.Height = LOC_SF_BMP_H;
    tex_argb32.Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
    tex_argb32.Buffer.Data        = (void*)TextureAddr;

    /* Set texture */
    ret |= R_DRW2D_CtxTextureIdentity(loc_Drw2dDev);
    ret |= R_DRW2D_CtxIdentity(loc_Drw2dDev);
    ret |= R_DRW2D_CtxTextureSet(loc_Drw2dDev, 0, &tex_argb32);
    ret |= R_DRW2D_CtxFillMode(loc_Drw2dDev, R_DRW2D_FILLMODE_TEXTURE);
    ret |= R_DRW2D_CtxBlendMode(loc_Drw2dDev, R_DRW2D_BLENDMODE_MULTIPLY);
    xPos = R_DRW2D_2X(Frame);
    yPos = R_DRW2D_2X(LOC_DISPLAY_HEIGHT - 100);
    ret |= R_DRW2D_CtxTextureTranslate(loc_Drw2dDev, xPos, yPos);

    /* Draw texture into rectangle */
    rect.Pos.X       = R_DRW2D_2X(Frame);
    rect.Pos.Y       = R_DRW2D_2X(LOC_DISPLAY_HEIGHT - 100);
    rect.Size.Width  = R_DRW2D_2X(LOC_SF_BMP_W);
    rect.Size.Height = R_DRW2D_2X(LOC_SF_BMP_H);

    ret |= R_DRW2D_DrawRect(loc_Drw2dDev, &rect);
    ret |= R_DRW2D_CtxTextureIdentity(loc_Drw2dDev);

    if (R_DRW2D_ERR_OK != ret)
    {
       loc_Error(ret);
    }
}

/*******************************************************************************
  Function: loc_CreateSprite_SFlash

  Create sprite
*/
static void loc_CreateSprite_SFlash(void* TextureAddr)
{
    r_wm_Error_t  wmErr;
    
    loc_SpriteData.Data   = TextureAddr;
    loc_SpriteData.PosX   = 0;
    loc_SpriteData.PosY   = 0;
    loc_SpriteData.PosZ   = 0;
    loc_SpriteData.Width  = LOC_KING_W;
    loc_SpriteData.Height = LOC_KING_H;
    loc_SpriteData.Window = &loc_SpriteWindow;
    wmErr = R_WM_SpriteCreate(LOC_WM_UNIT, &loc_SpriteData);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
    wmErr = R_WM_SpriteEnable(LOC_WM_UNIT, &loc_SpriteData);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
}

/*******************************************************************************
  Function: loc_CreateSprite_SFlash

  Create sprite
*/
static void loc_CreateSprite_intROM(void)
{
    r_wm_Error_t  wmErr;
    
    /* Create Srite */
    loc_SpriteData2.Data   = (void*)g_LogoRBmp;
    loc_SpriteData2.PosX   = LOC_DISPLAY_WIDTH  - LOC_LOGO_W;
    loc_SpriteData2.PosY   = LOC_DISPLAY_HEIGHT - LOC_LOGO_H;
    loc_SpriteData2.PosZ   = 0;
    loc_SpriteData2.Width  = LOC_LOGO_W;
    loc_SpriteData2.Height = LOC_LOGO_H;
    loc_SpriteData2.Window = &loc_SpriteWindow2;
    
    wmErr = R_WM_SpriteCreate(LOC_WM_UNIT, &loc_SpriteData2);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
    wmErr = R_WM_SpriteEnable(LOC_WM_UNIT, &loc_SpriteData2);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
}

/*******************************************************************************
  Function: loc_CreateDiscom

  Create Disocm
*/
static void loc_CreateDiscom(void)
{
    r_wm_Error_t  wmErr;
    
    /* Create Discom */
    loc_Discom.DiscomUnit = LOC_DISCOM_UNIT;
    loc_Discom.PosX       = LOC_DISPLAY_WIDTH  - LOC_LOGO_W;
    loc_Discom.PosY       = LOC_DISPLAY_HEIGHT - LOC_LOGO_H;
    loc_Discom.Width      = LOC_LOGO_W;
    loc_Discom.Height     = LOC_LOGO_H;
    loc_Discom.ExpCrc     = 0x718502BA;

    wmErr = R_WM_DiscomCreate(LOC_WM_UNIT, &loc_Discom);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
    wmErr = R_WM_DiscomEnable(LOC_WM_UNIT, &loc_Discom);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
}

/*******************************************************************************
  Function: loc_CreateVoca

  Create Video Output checker
*/
#if LOC_VOCA_ENABLE == 1 
static void loc_CreateVoca(void)
{
    r_wm_Error_t  wmErr;

    wmErr = R_WM_ScreenVocaInit(LOC_WM_UNIT);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
    wmErr = R_WM_ScreenVocaCreate(LOC_WM_UNIT, &loc_Voca_warping80);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
    wmErr = R_WM_ScreenVocaEnable(LOC_WM_UNIT, &loc_Voca_warping80);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
    wmErr = R_WM_ScreenVocaCreate(LOC_WM_UNIT, &loc_Voca_warping10);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
}
#endif

/*******************************************************************************
  Function: loc_Voca_MonitorAreaChange

  Change Video Output checker area
*/
#if LOC_VOCA_ENABLE == 1 
static void loc_Voca_MonitorAreaChange(int32_t dl_cnt)
{
    r_wm_Error_t  wmErr;
    r_wm_Voca_t   *enable_voca;
    r_wm_Voca_t   *disable_voca;

    if (dl_cnt & 1)
    {
        enable_voca  = &loc_Voca_warping10;
        disable_voca = &loc_Voca_warping80;
    }
    else
    {
        enable_voca  = &loc_Voca_warping80;
        disable_voca = &loc_Voca_warping10;
    }
    
    wmErr = R_WM_ScreenVocaDisable(LOC_WM_UNIT, disable_voca);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }

    wmErr = R_WM_ScreenVocaEnable(LOC_WM_UNIT, enable_voca);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
    
}
#endif

/*******************************************************************************
  Function: loc_CheckMismatchInt

  Change Video Output checker area
*/
static void loc_CheckMismatchInt(void)
{
    int8_t        led;
    
    /* Check mismatch event */ 
    if (loc_DiscomMismatch != 0)
    {
        /* Toggle LED25 (P16_0)  */
        led = R_BSP_HMI_GetLed(0);
        led = (led == 0) ? 100 : 0;
        R_BSP_HMI_SetLed(0, led);
        
        /* Print */
        R_BSP_STDIO_Printf("DISCOM mismatch, 0x%08X \n", loc_DiscomCrcVal);
        
        loc_DiscomMismatch--;
    }
#if LOC_VOCA_ENABLE == 1 
    if (loc_VocaMismatch != 0)
    {
        /* Toggle LED26 (P16_1)  */
        led = R_BSP_HMI_GetLed(1);
        led = (led == 0) ? 100 : 0;
        R_BSP_HMI_SetLed(1, led);
        
        /* Print */
        R_BSP_STDIO_Printf("VOCA mismatch \n");
        loc_VocaMismatch--;
    }
#endif
}

/*******************************************************************************
  Function: loc_App

  The render loop
*/
static void loc_App(void)
{
    uint32_t      frame;
    int32_t       ret = R_DRW2D_ERR_OK;
    r_wm_Error_t  wmErr;

    void*         textureAddr = R_NULL;
#if (LOC_VOWE_ENABLE == 1) && (LOC_DL_CHANGE != 0)
    int32_t       dl_cnt = 1;
#endif /* LOC_DL_CHANGE */
    
    loc_Identify();
#if (LOC_EX_MEM_TYPE == 1)
    R_BSP_STDIO_Printf("Using mode OCTA: OCTA Bus memory\n\n");
    textureAddr = loc_StartOCTA();
#elif (LOC_EX_MEM_TYPE == 2)
    R_BSP_STDIO_Printf("Using mode HYPB: Hyper Bus memory\n\n");
    textureAddr = loc_StartHYPB();
#elif (LOC_EX_MEM_TYPE == 0)
    R_BSP_STDIO_Printf("Using mode SFMA: Serial Flash/Q-SPI memory\n\n");
    textureAddr = loc_StartSFMA();
#else
#endif
    /* Init framebuffer struct */
    loc_Fb.Flags  = (r_drw2d_FramebufferFlags_t)0;
    loc_Fb.Handle = 0; /* not used currently */
    loc_Fb.Buffer.Pitch       = LOC_DISPLAY_STRIDE;
    loc_Fb.Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
    loc_Fb.Buffer.Size.Width  = LOC_DISPLAY_WIDTH;
    loc_Fb.Buffer.Size.Height = LOC_DISPLAY_HEIGHT;
    loc_Fb.Buffer.Data        = 0;

    loc_InitWM(&loc_lRAM_heap, &loc_VRAM_heap);
    wmErr = R_WM_FrameEndMark(LOC_WM_UNIT, 0);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
    wmErr = R_WM_FrameWait(LOC_WM_UNIT, 0);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }

    R_BSP_STDIO_Printf("Now using VOWE, JCUA and WM\n");
  #if LOC_VOWE_ENABLE == 1
    loc_VoweStart();
  #endif

    loc_CreateWindow(R_WM_WINMODE_FB,
                     0,
                     0,
                     0,
                     LOC_DISPLAY_WIDTH,
                     LOC_DISPLAY_HEIGHT,
                     LOC_DISPLAY_STRIDE,
                     R_WM_COLORFMT_ARGB8888,
                     &loc_WmWindow);
    loc_CreateWindow(R_WM_WINMODE_SPRITES,
                     0,
                     0,
                     1,
                     LOC_KING_W,
                     LOC_KING_H,
                     2048, /* HW restriction */
                     R_WM_COLORFMT_ARGB8888,
                     &loc_SpriteWindow);
    loc_CreateWindow(R_WM_WINMODE_SPRITES,
                     0,
                     0,
                     1,
                     LOC_DISPLAY_WIDTH,
                     LOC_DISPLAY_HEIGHT,
                     2048, /* HW restriction */
                     R_WM_COLORFMT_ARGB8888,
                     &loc_SpriteWindow2);
    
    loc_JcuaRun();
    
    /* start up gfx engine */
    ret |= R_DRW2D_Init();
    ret |= R_DRW2D_GlobalErrCallbackSet(&loc_DRW2DErrorHandler, R_NULL);
    ret |= R_DRW2D_Open(LOC_DRW2D_UNIT, R_DRW2D_UNIT_DHD0, &loc_DHDDev, &loc_Drw2dDev);

    /* assign texture and show sprite */
    if (textureAddr != R_NULL)
    {
        loc_CreateSprite_SFlash(textureAddr);
    }
    
    /* assign texture and show sprite */
    loc_CreateSprite_intROM();

    wmErr = R_WM_FrameEndMark(LOC_WM_UNIT, 0);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
    wmErr = R_WM_FrameWait(LOC_WM_UNIT, 0);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }

#if (LOC_EX_MEM_TYPE == 1)
    R_BSP_STDIO_Printf("Now using WM, DRW2D, SPEA, OCTA and VOWE\n");
#elif (LOC_EX_MEM_TYPE == 2)
    R_BSP_STDIO_Printf("Now using WM, DRW2D, SPEA, HYPB and VOWE\n");
#elif (LOC_EX_MEM_TYPE == 0)
    R_BSP_STDIO_Printf("Now using WM, DRW2D, SPEA, SFMA and VOWE\n");
#else
    R_BSP_STDIO_Printf("Now using WM, DRW2D, SPEA, and VOWE\n");
#endif
    loc_ExitFlag = R_FALSE;
    
    /* Create DISCOM */
    loc_CreateDiscom();
    
#if LOC_VOCA_ENABLE == 1 
    /* Create VOCA */
    loc_CreateVoca();
#endif

    wmErr = R_WM_FrameEndMark(LOC_WM_UNIT, 0);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
    wmErr = R_WM_FrameWait(LOC_WM_UNIT, 0);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
    for (frame = 0; loc_ExitFlag == R_FALSE; frame++)
    {
        loc_Fb.Buffer.Data = R_WM_WindowNewDrawBufGet(LOC_WM_UNIT, &loc_WmWindow);
        ret |= R_DRW2D_FramebufferSet(loc_Drw2dDev, &loc_Fb);
        if (R_DRW2D_ERR_OK != ret)
        {
            loc_Error(ret);
        }

        /* render onto this backbuffer and wait for GPU to finish*/
        loc_RenderClear(frame);
        loc_RenderPrimitives(frame);
        loc_RenderTexture_intROM(frame);
        
        if (textureAddr != R_NULL)
        {
            loc_RenderTexture_SFlash(frame, textureAddr);
        }

        R_DRW2D_GpuFinish(loc_Drw2dDev, R_DRW2D_FINISH_WAIT);

        /* let WM care about the handling of the buffer swap */
        wmErr = R_WM_WindowSwap(LOC_WM_UNIT, &loc_WmWindow);
        if(R_WM_ERR_OK != wmErr)
        {
           loc_Error(1);
        }

        /* animate the sprite window, too */
        wmErr = R_WM_WindowMove(LOC_WM_UNIT, &loc_SpriteWindow, frame, loc_SpriteWindow.PosY, loc_SpriteWindow.PosZ);
        if(R_WM_ERR_OK != wmErr)
        {
           loc_Error(1);
        }

        if (frame == LOC_DISPLAY_WIDTH)
        {
#if LOC_VOWE_ENABLE == 1
  #if LOC_DL_CHANGE != 0
            /* change Display List */
            loc_Vowe_DLChange(dl_cnt);
            
    #if LOC_VOCA_ENABLE == 1 
            loc_Voca_MonitorAreaChange(dl_cnt);
    #endif

            dl_cnt++;

  #endif /* LOC_DL_CHANGE */
#endif 
            frame = 0;
        }
        else if (frame == (LOC_DISPLAY_WIDTH / 2))
        {
            /* Disable sprite (for DISCOM/VOCA test) */
            wmErr = R_WM_SpriteDisable(LOC_WM_UNIT, &loc_SpriteData2);
            if(R_WM_ERR_OK != wmErr)
            {
               loc_Error(1);
            }
        }
        else if (frame == ((LOC_DISPLAY_WIDTH / 2) + 1))
        {
            /* Enable sprite (for DISCOM/VOCA test) */
            wmErr = R_WM_SpriteEnable(LOC_WM_UNIT, &loc_SpriteData2);
            if(R_WM_ERR_OK != wmErr)
            {
               loc_Error(1);
            }
        }
        
        wmErr = R_WM_FrameEndMark(LOC_WM_UNIT, 0);
        if(R_WM_ERR_OK != wmErr)
        {
           loc_Error(1);
        }
        wmErr = R_WM_FrameWait(LOC_WM_UNIT, 0);
        if(R_WM_ERR_OK != wmErr)
        {
           loc_Error(1);
        }
        
        loc_CheckMismatchInt();
    }

    R_BSP_STDIO_Printf("Closing  DISCOM \n");
    wmErr |= R_WM_DiscomDisable(LOC_WM_UNIT, &loc_Discom);
    wmErr |= R_WM_DiscomDelete(LOC_WM_UNIT, &loc_Discom);
#if LOC_VOCA_ENABLE == 1 
    R_BSP_STDIO_Printf("Closing  VOCA\n");
    wmErr |= R_WM_ScreenVocaDisable(LOC_WM_UNIT, &loc_Voca_warping10);
    wmErr |= R_WM_ScreenVocaDelete(LOC_WM_UNIT,  &loc_Voca_warping10);
    wmErr |= R_WM_ScreenVocaDisable(LOC_WM_UNIT, &loc_Voca_warping80);
    wmErr |= R_WM_ScreenVocaDelete(LOC_WM_UNIT,  &loc_Voca_warping80);
    wmErr |= R_WM_ScreenVocaDeInit(LOC_WM_UNIT);
#endif
    wmErr |= R_WM_FrameEndMark(LOC_WM_UNIT, 0);
    wmErr |= R_WM_FrameWait(LOC_WM_UNIT, 0);
    if (R_WM_ERR_OK != wmErr)
    {
        loc_Error(wmErr);
    }
    
    R_BSP_STDIO_Printf("Closing  DRW2D and VOWE\n");
    ret |= R_DRW2D_Close(loc_Drw2dDev);
    ret |= R_DRW2D_Exit();
    if (R_DRW2D_ERR_OK != ret)
    {
        loc_Error(ret);
    }
  #if LOC_VOWE_ENABLE == 1
    loc_VoweStop();
  #endif

    R_BSP_STDIO_Printf("Closing  WM and SPEA\n");
    wmErr |= R_WM_SpriteDisable(LOC_WM_UNIT, &loc_SpriteData);
    wmErr |= R_WM_SpriteDelete(LOC_WM_UNIT, &loc_SpriteData);
    wmErr |= R_WM_SpriteDisable(LOC_WM_UNIT, &loc_SpriteData2);
    wmErr |= R_WM_SpriteDelete(LOC_WM_UNIT, &loc_SpriteData2);
    wmErr |= R_WM_WindowDelete(LOC_WM_UNIT, &loc_SpriteWindow);
    wmErr |= R_WM_WindowDelete(LOC_WM_UNIT, &loc_SpriteWindow2);
    wmErr |= R_WM_WindowDelete(LOC_WM_UNIT, &loc_WmWindow);
    wmErr |= R_WM_FrameEndMark(LOC_WM_UNIT, 0);
    wmErr |= R_WM_FrameWait(LOC_WM_UNIT, 0);
    wmErr |= R_WM_DevDeinit(LOC_WM_UNIT);
    if (R_WM_ERR_OK != wmErr)
    {
        loc_Error(wmErr);
    }

#if (LOC_EX_MEM_TYPE == 1)
    loc_StopOCTA();
#elif (LOC_EX_MEM_TYPE == 2)
    loc_StopHYPB();
#elif (LOC_EX_MEM_TYPE == 0)
    loc_StopSFMA();
#else
#endif
}

/*******************************************************************************
  Function: loc_Error

  This function is called when an error occurs.
  The application will hang.
*/

void loc_Error(int32_t Err)
{
    while (Err)
    {
    }
}


/*******************************************************************************
  Function: main

  entry after startup
*/
int32_t main(void)
{
    R_BSP_Init();
    
    R_BSP_SetButtonCallback(BSP_BTN_OK_PRESS,       locBtnOkPress);
    R_BSP_SetButtonCallback(BSP_BTN_CENTER_PRESS,   locRotaryCenterPress);

    loc_SetupMemManager();
    R_UTIL_DHD_Init(LOC_DHD_UNIT);
    R_UTIL_DHD_Config((dhd_gpu_ptr_t)loc_GPU_Memory_Addr, DHD_MEMORY_SIZE, &loc_lRAM_heap);

    loc_App();
    R_UTIL_DHD_DeInit(LOC_DHD_UNIT);

    return(0);
}


