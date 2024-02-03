/*
****************************************************************************
PROJECT : GRAPE
FILE    : $Id: fw_vovial_api.h 11006 2016-11-07 16:32:44Z michael.golczewski $
AUTHOR  : $Author: michael.golczewski $
============================================================================ 
DESCRIPTION
Grape VOUT & VIN Abstraction Layer (VOVIAL)
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

#ifndef R_FW_VOVIAL_API_H_
#define R_FW_VOVIAL_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Title: VOVIAL
  
  Video Output/Input abstraction layer
  
  This header defines the interface which the application will
  require to use the Video output driver.
*/

/*******************************************************************************
  Section: Global Defines
*/

/*******************************************************************************
  Define: FW_VOVIAL_CRFLAG_XXX

  Flag for the surface creation

  FW_VOVIAL_CRFLAG_NONE            - No additional flag.
  FW_VOVIAL_CRFLAG_NO_ALLOC        - Do not allocate framebuffers
  FW_VOVIAL_CRFLAG_3D_SURFACE      - old: Surface will be used for EGL
  FW_VOVIAL_CRFLAG_OGLES_SURFACE   - Surface will be used with OGLES 
  FW_VOVIAL_CRFLAG_OVG_SURFACE     - Surface will be used with OVG
  FW_VOVIAL_CRFLAG_AA_ENABLE       - Anti-Aliasing is enabled
  FW_VOVIAL_CRFLAG_SPRITE          - Surface will be a sprite layer
  FW_VOVIAL_CRFLAG_RLE             - Surface will be a RLE layer
*/

#define FW_VOVIAL_CRFLAG_NONE            0u
#define FW_VOVIAL_CRFLAG_NO_ALLOC        1u
#define FW_VOVIAL_CRFLAG_3D_SURFACE      2u
#define FW_VOVIAL_CRFLAG_OGLES_SURFACE   4u
#define FW_VOVIAL_CRFLAG_OVG_SURFACE     8u
#define FW_VOVIAL_CRFLAG_AA_ENABLE      16u
#define FW_VOVIAL_CRFLAG_SPRITE         32u
#define FW_VOVIAL_CRFLAG_RLE            64u
  

/*******************************************************************************
  Section: Global Types
*/

/*******************************************************************************
  Enum: fw_vovial_FbFormat_t
  
  Colour format enumeration.
  
  The colour format supported by the VOAL are defined here.
  
  Values:  
  FW_VOVIAL_RGB565        - format rgb 565
  FW_VOVIAL_RGB888        - format rgb 888
  FW_VOVIAL_ARGB8888      - format argb 8888
  FW_VOVIAL_RGBA8888      - format rgba 8888
  FW_VOVIAL_ARGB4444      - format argb 4444
  FW_VOVIAL_RGB444        - format rgb 444
  FW_VOVIAL_RGBA4444      - format rgba 4444
  FW_VOVIAL_ARGB1555      - format argb 1555
  FW_VOVIAL_RGBA5551      - format rgba 5551
  FW_VOVIAL_RLE24ARGB8888 - format rle24 argb 8888
  FW_VOVIAL_RLE18ARGB8888 - format rle18 argb 8888
  FW_VOVIAL_RLE24RGB0888  - format rle24 rgb888
  FW_VOVIAL_RLE18RGB0888  - format rle18 rgb888
  FW_VOVIAL_RLE8CLUT8     - format rle8 clut 8
  FW_VOVIAL_RLE8CLUT4     - format rle8 clut 4
  FW_VOVIAL_RLE8CLUT1     - format rle1 clut 1

*/
typedef enum {
    FW_VOVIAL_RGB565,
    FW_VOVIAL_RGB888,
    FW_VOVIAL_ARGB8888,
    FW_VOVIAL_RGBA8888,
    FW_VOVIAL_ARGB4444,
    FW_VOVIAL_RGB444,
    FW_VOVIAL_RGBA4444,
    FW_VOVIAL_ARGB1555,
    FW_VOVIAL_RGBA5551,
    FW_VOVIAL_ALPHA8,
    FW_VOVIAL_RLE24ARGB8888,
    FW_VOVIAL_RLE18ARGB8888,
    FW_VOVIAL_RLE24RGB0888,
    FW_VOVIAL_RLE18RGB0888,
    FW_VOVIAL_RLE8CLUT8,
    FW_VOVIAL_RLE8CLUT4,
    FW_VOVIAL_RLE8CLUT1,
    FW_VOVIAL_CLUT8,
} fw_vovial_FbFormat_t;


/*******************************************************************************
  Enum: fw_vovial_Performance_t

  Type of performance value.

  Values:
  FW_VOVIAL_PERF_CPU - CPU time for drawing in ms
  FW_VOVIAL_PERF_GPU - GPU time for drawing in ms
*/

typedef enum {
    FW_VOVIAL_PERF_CPU,
    FW_VOVIAL_PERF_GPU
} fw_vovial_Performance_t;


/*******************************************************************************
  Enum: fw_vovial_SpriteStatus_t

  Type of sprite status.

  Values:

*/
typedef enum
{
    FW_VOVIAL_SPRITESTATUS_NOT_INITIALIZED = 0,
    FW_VOVIAL_SPRITESTATUS_DISABLED,
    FW_VOVIAL_SPRITESTATUS_ENABLED
} fw_vovial_SpriteStatus_t;


/*******************************************************************************
  Struct: fw_vovial_Sprite_t

  Description:

    Each sprite has a data structure of this type.

  Members:

    Surface             -   pointer to the parent surface
    Intern              -   for internal usage, do NOT modify
    Next                -   *internal* pointer to the next sprite in the queue
    Status              -   status of the sprite (should be set to
                            FW_VOVIAL_SPRITESTATUS_NOT_INITIALIZED before the
                            call to FW_VOVIAL_CreateSprite)
    Data                -   pointer to the image data of a sprite
    PosX / PosY / PosZ  -   position of the sprite on the window
    Width / Height      -   size of the sprite
*/
typedef struct fw_vovial_Sprite_s
{
    void *Surface;
    void *Intern; /* Do NOT modify */
    fw_vovial_SpriteStatus_t Status;
    void *Data;
    uint32_t PosX;
    uint32_t PosY;
    uint32_t PosZ;
    uint32_t Width;
    uint32_t Height;
} fw_vovial_Sprite_t;


/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: FW_VOVIAL_Init

  Initialize the Video output driver.

  Parameters:
  void
  
  Returns:
  void
*/

void FW_VOVIAL_Init(int32_t Vout, const char *DisplayId, void *CpuHeap, void *VidHeap);


/*******************************************************************************
  Function: FW_VOVIAL_DeInit

  Deinitialise the Video output driver.

  Parameters:
  void
  
  Returns:
  void
*/

void FW_VOVIAL_DeInit(int32_t Vout);


/*******************************************************************************
  Function: FW_VOVIAL_CreateSurface

  Create a VO surface.

  The VO surface contains the information about the framebuffers 
  (contain the data to be displayed): address and number, the 
  position and the size of the image, the colour format including 
  alpha.
    
  Parameters:
  Vout    - Device unit
  Format  - Colour Format, sees <VOVIAL_FbFormat_t>
  PosX    - X coordinate of the surface
  PosY    - Y coordinate of the surface
  PosZ    - Z coordinate of the surface
  Pitch   - New line pitch.
  Width   - Width of the surface to be created
  Height  - Height of the surface to be created
  Alpha   - Alpha value of the surface to be created
  Bufnum  - Number of buffer that the surface will used
  Flags   - Combination of FW_VOVIAL_CRFLAG_XXX flags

  Returns:
  void *  - pointer to the surface, see <_Surface_t>
*/

void *FW_VOVIAL_CreateSurface(int32_t Vout, fw_vovial_FbFormat_t Format, 
                         int32_t PosX, int32_t PosY, int32_t PosZ,
                         int32_t Pitch, int32_t Width, int32_t Height,
                         int32_t Alpha, int32_t BufNum, uint32_t Flags);




/*******************************************************************************
  Function: FW_VOVIAL_SelectSurface

  Set the given surface as the one being worked on.
  
  I case f egl surface it calls the eglMakeCurrent() function

  Parameters:
  Id - Surface pointer, see <FW_VOVIAL_CreateSurface>.

  Returns:
  int32_t   - 0 if successful else -1
*/

int32_t FW_VOVIAL_SelectSurface(void *Id);

/*******************************************************************************
  Function: FW_VOVIAL_EnableSurface

  Enables the selected surface.

  Parameters:
  Id - Surface pointer, see <FW_VOVIAL_CreateSurface>.

  Returns:
  void
*/

void FW_VOVIAL_EnableSurface(void *Id);


/*******************************************************************************
  Function: FW_VOVIAL_DisableSurface

  Disables the selected surface.
    
  Parameters:
  Id - Surface pointer, see <FW_VOVIAL_CreateSurface>.

  Returns:
  void
*/

void FW_VOVIAL_DisableSurface(void *Id);


/*******************************************************************************
  Function: FW_VOVIAL_DeleteSurface
  
  Delete the given surface.
  
  This function disable the interrupts reuqired for by the 
  VO and free the memory which was allocated for the given 
  surface, see <FW_VOVIAL_CreateSurface>

  Parameters:
  Id - Surface pointer, see <FW_VOVIAL_CreateSurface>.

  Returns:
  void
*/

void FW_VOVIAL_DeleteSurface(void *Id);

/*******************************************************************************
  Function: FW_VOVIAL_GetNativeWindow

  Return the native window.

  Parameters:
  Id - Surface pointer, see <FW_VOVIAL_CreateSurface>.

  Returns:
  Pointer to the native window.
*/

void *FW_VOVIAL_GetNativeWindow(void *Id);

/*******************************************************************************
  Function: FW_VOVIAL_GetNativeDisplay

  Return the native Display.

  Parameters:
  Id - Surface pointer, see <FW_VOVIAL_CreateSurface>.

  Returns:
  Pointer to the native display.
*/
void *FW_VOVIAL_GetNativeDisplay(void *Id);

/*******************************************************************************
  Function: FW_VOVIAL_MoveSurface

  Moves the surface on the screen by keeping its dimensions.

  Parameters:
  Id   - Surface pointer, see <FW_VOVIAL_CreateSurface>.
  PosX - New horizontal position, x-coordinate.
  PosY - New vertical position, y-coordinate.
  PosZ - New z-coordinate.

  Returns:
  void
*/

void FW_VOVIAL_MoveSurface(void *Id, uint32_t PosX, uint32_t PosY, uint32_t PosZ);


/*******************************************************************************
  Function: FW_VOVIAL_ResizeSurface

  Changes the surface(window) dimensions.

  Parameters:
  Id     - Surface pointer, see <FW_VOVIAL_CreateSurface>.
  Pitch  - New line pitch.
  Width  - New surface width.
  Height - New surface height.

  Returns:
  void
*/

void FW_VOVIAL_ResizeSurface(void *Id, uint32_t Pitch, uint32_t Width, uint32_t Height );


/*******************************************************************************
  Function: FW_VOVIAL_GetFrameBuffer

  Gets the next free frame buffer available for rendering of a given surface.

  Parameters:
  Id     - Surface pointer, see <FW_VOVIAL_CreateSurface>.
    
  Returns:
  void *  - pointer to the render buffer.
*/

void   *FW_VOVIAL_GetFrameBuffer(void *Id);



/*******************************************************************************
  Function: FW_VOVIAL_GetFrameBufferIdx

  Gets the index of the current render buffer of a given surface.

  Parameters:
  Id     - Surface pointer, see <FW_VOVIAL_CreateSurface>.
    
  Returns:
  uint32  - Index of the render buffer
*/

int32_t FW_VOVIAL_GetFrameBufferIdx(void *Id);


/*******************************************************************************
  Function: FW_VOVIAL_GetPrevFrameBuffer

  Get the frame buffer that was previosly used for rendering.

  Parameters:
  Id     - Surface pointer, see <FW_VOVIAL_CreateSurface>.
    
  Returns:
  Pointer to render buffer.
*/

void *FW_VOVIAL_GetPrevFrameBuffer(void *Id);



/*******************************************************************************
  Function: FW_VOVIAL_GetCurFrameBuffer

  Get the frame buffer currently used for rendering.

  Parameters:
  Id     - Surface pointer, see <FW_VOVIAL_CreateSurface>.
    
  Returns:
  Pointer to render buffer.
*/

void *FW_VOVIAL_GetCurFrameBuffer(void *Id);

/*******************************************************************************
  Function: FW_VOVIAL_SetVisibleBuffer

  Set the current on-screen buffer of a surface.

  Parameters:
  Id     - Surface pointer, see <FW_VOVIAL_CreateSurface>.
  Buffer - Pointer to render buffer
    
  Returns:
  void
*/

void FW_VOVIAL_SetVisibleBuffer(void *Id, void *Buffer);

/*******************************************************************************
  Function: FW_VOVIAL_SetBufferAddr

  Sets the starting address of the buffer with the given index.
  This function will fail, if the surface was created without setting the
  <FW_VOVIAL_CRFLAG_NO_ALLOC> flag.

  Parameters:
  Id        - Surface pointer, see <FW_VOVIAL_CreateSurface>.
  BufNum    - Number of buffers, resp. number of starting addresses in
              Addresses array.
  Addresses - Array of buffer starting addresses.

  Returns:
  int32_t   - 0 if successful else -1
*/

int32_t FW_VOVIAL_SetBufferAddr(void *Id, int32_t BufNum, void **Addresses);

/*******************************************************************************
  Function: FW_VOVIAL_SetClut

  Sets the CLUT for the indexed color formats

  Parameters:
  Id        - Surface pointer, see <FW_VOVIAL_CreateSurface>.
  Size      - Size of the table to be set.
  Data      - Table of <Size> * 32-bit values each containing the BGRA value for its index.

  Returns:
  int32_t   - 0 if successful else -1
*/

int32_t FW_VOVIAL_SetClut(void *Id, uint32_t Size, void *Data);

/*******************************************************************************
  Function: FW_VOVIAL_GetVisibleBuffer

  Get the current visble buffer of a surface.

  Parameters:
  Id     - Surface pointer, see <FW_VOVIAL_CreateSurface>.
    
  Returns:
  void
*/

void* FW_VOVIAL_GetVisibleBuffer(void *Id);


/*******************************************************************************
  Function: FW_VOVIAL_SwapFrameBuffer

  Swaps the render buffers of a given surface.

  Parameters:
  Id         - Surface pointer, see <FW_VOVIAL_CreateSurface>.
  FrameCount - tbd

  Returns:
  void
*/

void FW_VOVIAL_SwapFrameBuffer(void *Id, uint32_t FrameCount);


/*******************************************************************************
  Function: FW_VOVIAL_RegisterVBlankIsr

  Sets the call back function for the VBlank Interrupt

  Parameters:
  Id     - Surface pointer, see <FW_VOVIAL_CreateSurface>.
  Isr    - Pointer to the callback function
    
  Returns:
  void
*/

void FW_VOVIAL_RegisterVBlankIsr(void *Id, void (*Isr)(void));


/*******************************************************************************
  Function: FW_VOVIAL_SetSurfaceAlpha

  Sets the the constant alpha value of the surface.

  Parameters:
  Id     - Surface pointer, see <FW_VOVIAL_CreateSurface>.
  Alpha  - Constant alpha value

  Returns:
  void
*/

void FW_VOVIAL_SetSurfaceAlpha(void *Id, 
                          uint32_t Alpha);


/*******************************************************************************
  Function: FW_VOVIAL_GetBufferInfo

  Returns the buffer information of a given surface.

  Parameters:
  Id      - Surface pointer, see <FW_VOVIAL_CreateSurface>.
  Format  - pointer to a <VOVIAL_FbFormat_tColour> variable. 
            The function will store the information 
            about the colour format at this address.
  Pitch   - New line pitch.
  Width   - pointer to a variable of type int32_t. The function 
            will store the width information at this address.
  Height  - pointer to a variable of type int32_t. The function 
            will store the height information  at this address.
    
  Returns:
  void
*/

void FW_VOVIAL_GetBufferInfo(void *Id, fw_vovial_FbFormat_t *Format, 
                        int32_t *Pitch, int32_t *Width, int32_t *Height);


/*******************************************************************************
  Function: FW_VOVIAL_GetSurfaceFps

  Returns the value of frames per second of a given surface.

  Parameters:
  Id      - Surface pointer, see <FW_VOVIAL_CreateSurface>.
    
  Returns:
  Fps value of the corresponding surface.
*/

float32_t FW_VOVIAL_GetSurfaceFps(void *Id);


/*******************************************************************************
  Function: FW_VOVIAL_SetSurfacePerfinfo

  Sets the value of performance information of a given surface.

  Parameters:
  Id      - Surface pointer, see <FW_VOVIAL_CreateSurface>.
  Index   - Info index
  Value   - Info value
    
  Returns:
  void
*/

void FW_VOVIAL_SetSurfacePerfinfo(void *Id, fw_vovial_Performance_t Index, uint32_t Value);


/*******************************************************************************
  Function: FW_VOVIAL_GetSurfacePerfinfo

  Returns the value of performance information of a given surface.

  Parameters:
  Id      - Surface pointer, see <FW_VOVIAL_CreateSurface>.
  Index   - Info index
    
  Returns:
  info value of the corresponding surface.
*/

uint32_t FW_VOVIAL_GetSurfacePerfinfo(void *Id, fw_vovial_Performance_t Index);


/***********************************************************
  Group: Sprites
*/

/***********************************************************
  Function: FW_VOVIAL_CreateSprite

  Description:
  Create a sprite as specified in the Sprite parameter on surface Surface.
  The sprite will not be visible until it is enabled.

  Parameter:
  Sprite    -   <fw_vovial_Sprite_t> structure pointer
                The structure has to be filled by the application before calling the function.

  Return value:
  0 if successful, -1 otherwise
*/
int32_t FW_VOVIAL_CreateSprite(fw_vovial_Sprite_t *Sprite);

/***********************************************************
  Function: FW_VOVIAL_EnableSprite

  Description:
  Enable the specified sprite. The sprite becomes visible on the screen.

  Parameter:
  Sprite    -   <fw_vovial_Sprite_t> structure pointer

  Return value:
  void
*/
void FW_VOVIAL_EnableSprite(fw_vovial_Sprite_t *Sprite);

/***********************************************************
  Function: FW_VOVIAL_DisableSprite

  Description:
  Disable the specified sprite. The sprite becomes not visible on the screen.

  Parameter:
  Sprite    -   <fw_vovial_Sprite_t> structure pointer

  Return value:
  void
*/

void FW_VOVIAL_DisableSprite(fw_vovial_Sprite_t *Sprite);

/***********************************************************
  Function: FW_VOVIAL_MoveSprite

  Description:
  Move the sprite to the specified X/Y and Z-order location.

  Parameter:
  Sprite            -   <fw_vovial_Sprite_t> structure pointer
  PosX/PosY/PosZ    -   New sprite position

  Return value:
  void
*/

void FW_VOVIAL_MoveSprite    (   fw_vovial_Sprite_t  *Sprite,
                                 uint32_t            PosX,
                                 uint32_t            PosY,
                                 uint32_t            PosZ
                             );

/***********************************************************
  Function: FW_VOVIAL_SetSpriteBuffer

  Description:
    Set the sprite buffer. This way the buffer initially
    set upon completion of the <FW_VOVIAL_SpriteCreate> can be changed.

  Parameter:
    Sprite            -   <fw_vovial_Sprite_t> structure pointer
    Buffer            -   New sprite buffer

  Return value:
    void
*/

void FW_VOVIAL_SetSpriteBuffer(fw_vovial_Sprite_t *Sprite, void *Buffer);

/***********************************************************
  Function: FW_VOVIAL_DeleteSprite

  Description:
  Remove the sprite from the host window.

  Parameter:
  Sprite            -   <fw_vovial_Sprite_t> structure pointer

  Return value:
  void
*/

void FW_VOVIAL_DeleteSprite(fw_vovial_Sprite_t *Sprite);


/***********************************************************
  Function: FW_VOVIAL_CreateCaptureSurface
  
  Open a Video In device and create a corresponding Captue surface.
  
  Parameters:
    Vin         - Video input device number (0, 1...)
    Format      - colour format
    Bufnum      - number of cpature buffer
    SrcPitch    - pitch of the image
    SrcWidth    - Width of the input image
    SrcHeight   - height of the input image
    SrcMode     - source mode ( currently not used. The mode is automatically set to interlaced)
    
  Return:
    pointer to the capture Surface
*/
void *FW_VOVIAL_CreateCaptureSurface(int32_t Vin, fw_vovial_FbFormat_t Format, int32_t Bufnum,
                                  int32_t SrcPitch, int32_t SrcWidth, int32_t SrcHeight, int32_t SrcMode,
                                  int32_t CropWidth, int32_t CropHeight, int32_t CropXOff, int32_t CropYOff);


/***********************************************************
  Function: FW_VOVIAL_DeleteCaptureSurface
  
  Reset the given capture surface and close the video input device
  
  Parameters:
    Id  - Pointer to the Capture surface to be deleted
    
  Return:
    None
*/
void FW_VOVIAL_DeleteCaptureSurface(void *Id);

/***********************************************************
  Function: FW_VOVIAL_EnableCaptureSurface
  
  Enables the given capture Surface
  
  Parameters:
    Id  - Pointer to the Capture surface to be deleted
    
  Return:
    None
  
*/
void FW_VOVIAL_EnableCaptureSurface(void *Id);

/***********************************************************
  Function: FW_VOVIAL_DisableCaptureSurface
  
  Disables the given capture surface
  
  Parameters:
    Id  - Pointer to the Capture surface to be deleted
    
  Return:
    None
*/
void FW_VOVIAL_DisableCaptureSurface(void *Id);

/***************************************************************************************
  Function: FW_VOVIAL_GetCaptureWindowSurface
*/
void* FW_VOVIAL_GetCaptureWindowSurface(void *Id);


/***********************************************************
  Function: FW_VOVIAL_SetCaptSurfaceCrop
  
  Set the Cropping parameters for the given capture surface
  
  Parameters:
    Id      - pointer to the capture surface
    Width   - cropping rectangle width
    Height  - cropping rectangle height
    Left    - cropping rectangle horizontal offset
    Top     - cropping rectangle vertical offset
  
  Return:
    -1 if NOK
     0 if OK
    
*/
int32_t FW_VOVIAL_SetCaptSurfaceCrop(void *Id, int32_t Width, int32_t Height, int32_t Left, int32_t Top);

/***********************************************************
  Function: FW_VOVIAL_VcaptControl
  
  Set new conrol value for the capture surface
  
  Parameters:
    Id          - pointer to the capture surface
    Control_id  - control ID which the new value will apply to. See the v4l2 control ID definition
    Value       - new value of the user control.
  
  Return:
    -1 if NOK
     0 if OK
*/
int32_t FW_VOVIAL_VcaptControl(void *Id, uint32_t Control_id, int32_t Value);

/***********************************************************
  Function: FW_VOVIAL_BindCaptVO
  
  This function bind a cpature surface to a FB surface
  
  Parameters:
    CaptSurf    - pointer to the capture surface
    FbSurf      - pointer to the framebuffer surface
  
  Return:
    None
*/
void FW_VOVIAL_BindCaptVO(void *CaptSurf, void *FbSurf);

void FW_VOVIAL_StartCapture(void * Id);
void FW_VOVIAL_StopCapture(void * Id);



#ifdef __cplusplus
}
#endif

#endif /* R_FW_VOVIAL_API_H_ */
