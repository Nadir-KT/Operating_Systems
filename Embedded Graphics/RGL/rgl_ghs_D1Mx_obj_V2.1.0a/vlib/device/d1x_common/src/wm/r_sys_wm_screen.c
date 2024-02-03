/*
****************************************************************************
PROJECT : WM
============================================================================ 
DESCRIPTION
WM system functions for D1x
============================================================================
                            C O P Y R I G H T
============================================================================
                       Copyright (c) 2013 - 2019
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
****************************************************************************
*/

/***********************************************************
  Title: D1Mx and D1Lx WM SYS API implementation  
*/

/***********************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_cdi_api.h"

/* display database with display timings */
#include "r_ddb_api.h"
#include "r_vdce_api.h"
#include "r_spea_api.h"

#include "r_config_wm.h"
#include "r_wm_api.h"
#include "r_wm_sys.h"
#include "r_sys_wm.h"

#include "r_dev_pin_types.h"
#include "r_dev_api.h"

#if (defined(R_WM_MSQ_DEQUEUE_PROFILING) || defined(R_WM_MSQ_ENQUEUE_PROFILING))
#include "r_profile.h"
#endif

#ifdef R_WM_SYS_ERROR_TRACKING
    #include "r_bsp_stdio_api.h"
#endif

/**********************************************************
  Section: Local Defines
*/

/***********************************************************
  Constants: Arithmetic constatnt values
*/

#define LOC_FLOAT_0_5              (0.5f)
#define LOC_FLOAT_N_0_5            (-0.5f)
#define LOC_FLOAT_LOG_COEF         (0.346607f)
#define LOC_FLOAT_POW_COEF         (0.33971f)
#define LOC_FLOAT_127              (127.0f)
#define LOC_FLOAT_POW_2_23         ((float32_t)(1uL << 23u))

/***********************************************************
  Constants: Gamma setting
*/
#define LOC_WM_GAMMA_ENTRY_NUM     (33u)
#define LOC_WM_GAMMA_COEF_CENTER_F (128.0f)  /* scaled by 1/128 */
#define LOC_WM_GAMMA_COEF_CENTER_U (128u)
#define LOC_WM_GAMMA_COEF_MIN      (0.25f)
#define LOC_VDCE_GAIN_MAX          (2047.0f)
#define LOC_VDCE_GAIN_CENTER       (1024.0f) /* scaled by 1/1024 */
#define LOC_VDCE_RGB_MAX           (255.0f)
#define LOC_GAMMA_SEG_SIZE         (8u)
#define LOC_VDCE_AREA_MIN          (1u)

/***********************************************************
  Constants: Scan line setting 
*/
#define LOC_WM_HEIGHT_THRESHOLD    (240u)
#define LOC_WM_SCANLINE_TIMING_H   (50u)  /* ScreenHeight - 50px in case of over threshold */
#define LOC_WM_SCANLINE_TIMING_L   (10u)  /* ScreenHeight - 10%  in case of under threshold */

/***********************************************************
  Constants: Pattern dither setting
*/
#define LOC_DITHER_PA_VAL           (3u)
#define LOC_DITHER_PB_VAL           (0u)
#define LOC_DITHER_PC_VAL           (2u)
#define LOC_DITHER_PD_VAL           (1u)

/***********************************************************
  Constants: Control interrupt number
*/
#define LOC_WM_SCREEN_INT_NUM       (9u)
#define LOC_WM_CAPTURE_INT_NUM      (2u)

typedef union
{
    float32_t fVal; 
    uint32_t  i32Val;
} r_wm_intFloat32_t;

/***********************************************************
  Section: Local Defines
*/


/***********************************************************
  Section: Global Variables
*/

/***********************************************************
  Section: Local Function Declarations
*/


R_WM_STATIC float32_t loc_FloorF(const float32_t x);
R_WM_STATIC int32_t   loc_RoundF(const float32_t f);
R_WM_STATIC float32_t loc_MinF(const float32_t a, const float32_t b);
R_WM_STATIC float32_t loc_MaxF(const float32_t a, const float32_t b);
R_WM_STATIC float32_t loc_Log2(const float32_t i);
R_WM_STATIC float32_t loc_Pow2(const float32_t i);
R_WM_STATIC float32_t loc_PowF(const float32_t a, const float32_t b);

R_WM_STATIC void      loc_FitCurveForVdce(const uint16_t          *const InValue, 
                                          const float32_t         *const OutValue, 
                                          const uint32_t                 NumEntries,
                                          r_vdce_GammaCorrect_t   *const FittedCurve);

R_WM_STATIC void      loc_GetGammaCurve(const uint32_t               Unit, 
                                        const uint8_t                Gamma, 
                                        r_vdce_GammaCorrect_t *const Curve);

/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Section: Local Functions
*/

/*
The following function are a replacement for math.h to ensure, the drivers are stdlib-free.
You may replace these functions by a library implementation if available.

MISRA disabled due to binary fiddling with floating point variables in <loc_Log2> and <loc_Pow2>.
*/


/***********************************************************
  Function: loc_FloorF
  
  Description: 
  Takes the input and returns the greatest integer less than or equal to it.
  
  Parameters: 
  x - Specifies the value to be converted.
  
  Return Value:
  float32_t
*/
R_WM_STATIC float32_t loc_FloorF(const float32_t x)
{
    int64_t y = (int64_t)x;
    
    if (x < 0.0f)
    {
        y--;
    }
    else 
    {
        /* Do nothing. */
    }
    return (float32_t)y;
}

/***********************************************************
  Function: loc_RoundF
  
  Description:
  0.5 is added to the input if it is greater than zero and subracted otherwise.
  
  Parameters:
  f - Specifies the value to be rounded off.
  
  Return Value: 
  int32_t
*/
R_WM_STATIC int32_t loc_RoundF(const float32_t f)
{
    float32_t temp_f = f;
    
    temp_f += (temp_f > 0.0f) ? LOC_FLOAT_0_5: LOC_FLOAT_N_0_5;
    return (int32_t)temp_f;
}


/*******************************************************************************
  Function: loc_minf

  Description:
  Returns the minimum of the two supplied integer values.
  You may replace this by a library implementation if available.

  Parameters:
  a,b  - Specifies the values to be compared
  
  Returns:
  int32_t
*/
R_WM_STATIC float32_t loc_MinF(const float32_t a, const float32_t b)
{
    return (a < b) ? a : b;
}

/*********************************************************** 
  Function: loc_MaxF
  
  Description:
  Returns the largest input.
  
  Parameters:
  a, b - Specifies the values to be compared.
  
  Return Value:
  float32_t
*/
R_WM_STATIC float32_t loc_MaxF(const float32_t a, const float32_t b)
{
    return (a > b) ? a : b;
}

/***********************************************************
  Function: loc_Log2
  
  Description:
  Calculates the logarithm of the input number to the base 2.
  
  Parameters:
  i - Specifies the value whose log has to be calculated.
  
  Return Value:
  float32_t
*/
R_WM_STATIC float32_t loc_Log2(const float32_t i)
{
    const float32_t     shift_23   = 1.0f / LOC_FLOAT_POW_2_23;
    const float32_t     log_bodge  = LOC_FLOAT_LOG_COEF;

    r_wm_intFloat32_t   log_val;
    float32_t           x_val;
    float32_t           y_val;
    
    /* Cast types via union as we want to keep the bits as they are */
    log_val.fVal = i;
    
    x_val  = (float32_t)log_val.i32Val;
    /* 1/(2^23) */
    x_val *=  shift_23; 
    x_val  =  x_val - LOC_FLOAT_127;
    
    y_val = x_val - loc_FloorF(x_val);
    y_val = (y_val - (y_val * y_val)) * log_bodge;
    
    return (x_val + y_val);
}

/***********************************************************
  Function: loc_Pow2

  Description:
  Calculates the base 2 power of the input value.
  Needed by <loc_PowF>.

  Parameters: 
  i - Specifies the value whose power has to be calculated.

  Return Value:
  float32_t
*/
R_WM_STATIC float32_t loc_Pow2(const float32_t i)
{
    const float32_t shift23   = LOC_FLOAT_POW_2_23;
    const float32_t pow_bodge = LOC_FLOAT_POW_COEF;

    r_wm_intFloat32_t fi_x;
    float32_t x_val;
    float32_t y_val;
    
    y_val = i - loc_FloorF(i);
    y_val = (y_val - (y_val * y_val)) * pow_bodge;
    
    x_val  = (i + LOC_FLOAT_127) - y_val;
    /* 2^23 */
    x_val *= shift23; 

    /* Cast types via union as we want to keep the bits as they are */
    fi_x.i32Val = (uint32_t)x_val;
    
    return (float32_t)fi_x.fVal;
}

/***********************************************************
  Function: loc_PowF
  
  Description:
  The maximum error for this implementation of powf for the range of [0.5 .. 4] is 0.7%
  This is sufficient for its intended purpose.
  You may replace this by a library implementation if available.
  
  Parameters:
  a - Specifies the number whose power has to be calculated.
  b - Specifies the power.
  
  Return Value:
  float32_t
*/
R_WM_STATIC float32_t loc_PowF(const float32_t a, const float32_t b )
{
    return loc_Pow2(b * loc_Log2(a));
}

/*************************************************************************
  Function: loc_FitCurveForVdce
  
  Description:
  Takes two arrays <InValue>, <OutValue> of exactly 33 points, that represent 
  a colour correction curve. 
  
  <InValue> is expected to be in range of [0..256] including 256 for the last element!
  <OutValue is expected to be in range of [0..256] including 256 for the last element!
  
  Both series <InValue>, <OutValue> are required to start in the Origin (0,0), thus <InValue>[0] 
  and <OutValue>[0] need to be zero. 
  Non-matching <InValue> is an error! Non-matching <OutValue> will be fitted!
  
  Both series <InValue>, <OutValue> are required to be monotonically nondecreasing, thus 
  the next value is always bigger than the previous. Non-matching values will be fitted!

  Both series <InValue>, <OutValue> are required to have a slope not bigger than 2, thus 
  for a distance dIn: dOut < 2*dIn . Non-matching values will be fitted!
  
  Parameters:
  InValue       - 33 Reference points starting with ZERO
  OutValue      - 33 Reference points
  NumEntries    - Number of reference points
  FittedCurve   - Reference to the curve to be filled
  
  Return Value:
  None
*/ 
R_WM_STATIC void     loc_FitCurveForVdce(const  uint16_t       *const  InValue,
                                         const  float32_t      *const  OutValue,
                                         const  uint32_t               NumEntries,
                                         r_vdce_GammaCorrect_t *const  FittedCurve)
{
    const float32_t max_gain = LOC_VDCE_GAIN_MAX / LOC_VDCE_GAIN_CENTER;
    float32_t       fitted_out_value_last = 0.0f;
    float32_t       gain;
    float32_t       area_out_diff;
    uint32_t        i;
    uint16_t        area_size;
    
    for (i = 1u; i < NumEntries; i++)
    {
        /* Restrict the area size to positive values bigger than zero */
        if (InValue[i] <= InValue[i - 1u])
        {
            area_size = LOC_VDCE_AREA_MIN;
        }
        else
        {
            area_size = InValue[i] - InValue[i - 1u];
        }
        
        /* Calculate output value that lies within the HW restrictions */
        area_out_diff = OutValue[i] - fitted_out_value_last;
        /* Round to minimum/maximum value */
        area_out_diff = loc_MinF(area_out_diff, (float32_t)area_size * max_gain);
        area_out_diff = loc_MaxF(area_out_diff, 0.0f);
        
        /* Calculate the corresponding gain (slope) value  */
        gain = (area_out_diff * LOC_VDCE_GAIN_CENTER) / (float32_t)area_size;
        /* Round to maximum value, just in case */
        gain = loc_MinF(gain, LOC_VDCE_GAIN_MAX);
        
        /* Set to VDCE argument */
        FittedCurve->Gain[i - 1u] = (uint16_t)loc_RoundF(gain);
        FittedCurve->Area[i - 1u] = (uint8_t)InValue[i - 1u];
        
        fitted_out_value_last += area_out_diff;
    }
}

/***************************************************************************
  Function: loc_GetGammaCurve
  
  Description:
  Fill the supplied Struct with a color correction curve, that matches the given Gamma Coefficient.
  
  Parameters:
  Unit  - Specifies the WM unit number.
  Gamma - Gamma coeficcient [32..256]. Internally scaled by 1/128. 
          Floating-point range is [0.25f .. 2.0f]
  Curve - Color correction curve to be input into VDCE.
  
  Return Value:
  None
*/ 
R_WM_STATIC void     loc_GetGammaCurve(const uint32_t               Unit, 
                                       const uint8_t                Gamma, 
                                       r_vdce_GammaCorrect_t *const Curve)
{
    float32_t       f_gamma;
    float32_t       gamma_inverse;
    float32_t       out[LOC_WM_GAMMA_ENTRY_NUM];
    uint16_t        in[LOC_WM_GAMMA_ENTRY_NUM];
    uint16_t        segment;

    /* For the gamma correction curve, */
    /* we take a constant size of the segements so they are equally distributed */
    f_gamma = (float32_t)Gamma / LOC_WM_GAMMA_COEF_CENTER_F;
    f_gamma = loc_MaxF(f_gamma, LOC_WM_GAMMA_COEF_MIN);
    
    /* Step 0 is special. The curve starts always in the origin (0,0) */
    in[0]  = 0u;
    out[0] = 0.0f;
    
    for (segment = 1u; segment < LOC_WM_GAMMA_ENTRY_NUM; segment++)
    {
        /* Input brightness value */
        in[segment] = segment * LOC_GAMMA_SEG_SIZE;
        
        /* Output value as we want it */
        gamma_inverse = 1.0f / f_gamma;
        
        out[segment] = LOC_VDCE_RGB_MAX * 
                       loc_PowF((float32_t)in[segment] / LOC_VDCE_RGB_MAX, gamma_inverse);
    }
    
    /*  Create a curve over the input/output values, staying in the capabilities of the VDCE */
    loc_FitCurveForVdce(in, out, LOC_WM_GAMMA_ENTRY_NUM, Curve);
}


/***********************************************************
  Section: WM Driver Support Functions
  
  Comments see: <WM Support Functions>
*/

/***********************************************************
  Group: WM Video Output Screen interface
*/

/***********************************************************
  Function: R_WM_Sys_ScreenTimingSetByName
  
  Description:
  This function sets a name identifier for particular timing. 
  The new values will be active only after executing R_WM_ScreenEnable. 
  State will become Display_Initialized after execution this function. 
  For reconfiguration of SCANLINE or usage of either interrupt in application code, 
  call R_WM_DevEventRegister.
  
  Parameters:
  Unit - Specifies the WM unit number.
  Name - Specifies the name (identifier), which is used to lookup for the correct
         screen timing in the display data base. 
         Refer to VDCE UM: R_VDCE_DisplayTimingSet.
  
  Return Value:
  uint32_t
*/
uint32_t R_WM_Sys_ScreenTimingSetByName(const uint32_t         Unit,
                                        const int8_t    *const Name)
{
    r_ddb_Timing_t      *timing;
    uint32_t            ret;
    
    if ((Unit >= R_WM_DEV_NUM) || (Name == R_NULL) || (*Name == 0))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        timing = R_DDB_GetDisplayTiming((const char*)Name); /* PRQA S 310 */
        ret = R_WM_Sys_ScreenTimingSet(Unit, timing);
    }

    return ret;
}

/***********************************************************
  Function: R_WM_Sys_ScreenTimingSet
  
  Description:
  This function sets display timing. The new values will be active only after executing 
  R_WM_ScreenEnable. State will become Display_Initialized after execution this function.
  For reconfiguration of SCANLINE or usage of either interrupt in application code, 
  call R_WM_DevEventRegister.
  
  Parameters:
  Unit   - Specifies the WM unit number.
  Timing - Specifies the timing parameter contains all the information to drive the
           display. Refer to VDCE UM: R_VDCE_DisplayTimingSet.
  
  Return Value:
  uint32_t
*/ 
uint32_t R_WM_Sys_ScreenTimingSet(const uint32_t              Unit, 
                                  const r_ddb_Timing_t *const Timing)
{
    uint32_t            ret = R_WM_SYS_OK;
    uint32_t            scan_line;
    r_vdce_Error_t      vdce_res;
    r_wm_DevSys_t       *dev_sys = R_WM_Sys_GetDevSys(Unit);

    if ((Unit >= R_WM_DEV_NUM) || (Timing == R_NULL))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        vdce_res = R_VDCE_DisplayTimingSet(Unit, Timing);
        if (vdce_res != R_VDCE_ERR_OK)
        {
            ret = R_WM_SYS_NG;
        }
        else
        {
            /* set the interrupt service routines */
            /* SCANLINE interrupt */
            if (Timing->ScreenHeight > LOC_WM_HEIGHT_THRESHOLD)
            {
                scan_line = Timing->ScreenHeight - LOC_WM_SCANLINE_TIMING_H;
            }
            else
            {
                scan_line = Timing->ScreenHeight - 
                            (Timing->ScreenHeight / LOC_WM_SCANLINE_TIMING_L);
            }
            vdce_res = R_VDCE_IntcScanlineSet(Unit, scan_line);
            if (vdce_res != R_VDCE_ERR_OK)
            {
                ret = R_WM_SYS_NG;
            }
            else
            {
                /* Store the Screen width and height */
                dev_sys->ScreenWidth  = (int32_t)Timing->ScreenWidth;
                dev_sys->ScreenHeight = (int32_t)Timing->ScreenHeight;
            }
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_ScreenEnable
  
  Description:
  This function enables the screen.
  
  Parameters:
  Unit    - Specifies the WM unit number.
  Enabled - Specifies whether the screen is enabled or not.
  
  Return Value: 
  uint32_t
*/ 

uint32_t R_WM_Sys_ScreenEnable(const uint32_t Unit, 
                               const uint32_t Enabled)
{
    uint32_t                ret = R_WM_SYS_OK;
    uint32_t                i;
    r_vdce_Error_t          vdce_res;
    r_wm_EventId_t          screen_int[LOC_WM_SCREEN_INT_NUM] = {
                                R_WM_EVENT_VBLANK,
                                R_WM_EVENT_SCANLINE,
                                R_WM_EVENT_LAYER1_VBLANK,
                                R_WM_EVENT_LAYER0_UNDERFLOW,
                                R_WM_EVENT_LAYER1_UNDERFLOW,
                                R_WM_EVENT_LAYER2_UNDERFLOW,
                                R_WM_EVENT_LAYER3_UNDERFLOW,
                                R_WM_EVENT_OIR_VBLANK,      
                                R_WM_EVENT_OIR_SCANLINE    
                            };
    r_wm_EventId_t          capture_int[LOC_WM_CAPTURE_INT_NUM] = {
                                R_WM_EVENT_VI_VBLANK,
                                R_WM_EVENT_VI_OVERFLOW
                            };
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        for (i = 0u; i < LOC_WM_SCREEN_INT_NUM; i++)
        {
            /* Enable/Disable interrupt */
            ret = R_WM_Sys_EnableEvent(Unit, screen_int[i], Enabled);
            if (ret != R_WM_SYS_OK)
            {
                break;
            }
            else
            {
                /* Do nothing */
            }
        }
        if (ret == R_WM_SYS_OK)
        {
            if (Enabled == R_WM_TRUE)
            {
                /* Enable disaplay */
                vdce_res = R_VDCE_DisplayEnable(Unit);
                if (vdce_res != R_VDCE_ERR_OK)
                {
                    ret = R_WM_SYS_NG;
                }
                else
                {
                    ret = R_WM_Sys_DevVocaInit(Unit);
                }
            }
            else
            {
                for (i = 0u; i < LOC_WM_CAPTURE_INT_NUM; i++)
                {
                    /* Disable capture interrupt */
                    ret = R_WM_Sys_EnableEvent(Unit, capture_int[i], Enabled);
                    if (ret != R_WM_SYS_OK)
                    {
                        break;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }

                if (ret == R_WM_SYS_OK)
                {
                    /* Disable disaplay */
                    vdce_res = R_VDCE_DisplayDisable(Unit);
                    if (vdce_res != R_VDCE_ERR_OK)
                    {
                        ret = R_WM_SYS_NG;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                else
                {
                    /* Do nothing */
                }
            }
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_ScreenBgColorSet
  
  Description:
  This function set the screen background color that is seen, if no window (or a transparent one) 
  is on top of it. The default
  back ground color of the Screen is black (Red = 0, Green = 0, Blue = 0).
  
  Parameters:
  Unit  - Specifies thw WM unit number.
  Red   - Specifies the red color component factor in scale 0 to 255.
  Green - Specifies the green color component factor in scale 0 to 255.
  Blue  - Specifies the color component factor in scale 0 to 255.
  
*/
uint32_t R_WM_Sys_ScreenBgColorSet(const uint32_t Unit, 
                                   const uint8_t  Red, 
                                   const uint8_t  Green, 
                                   const uint8_t  Blue)
{
    uint32_t        ret;
    r_vdce_Error_t  disp_col;
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        disp_col = R_VDCE_DisplayColorSet(Unit, Red, Green, Blue);
        if (disp_col != R_VDCE_ERR_OK)
        {
            ret = R_WM_SYS_NG;
        }
        else
        {
            ret = R_WM_SYS_OK;
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_ScreenColorCurveSet
  
  Description:
  This function sets a curve to be used as custom gamma or color correction curve. 
  Using this curve, each RGB color
  channel is individually corrected according to the given curve. For the correction in VDCE, 
  the incoming color
  information of each channel is split into 32 equally sized segments each covering a range 8 
  color values. For each of these 8 values in a segment, the same gain factor applies.
 
  Parameters:
  Unit       - Specifies the WM unit number.
  ColorCurve - Pointer to Color curve table.
  NumEntries - Specifies the number of entries in the Color Curve table. Set this to 33.
  
  Return Value:
  R_WM_SYS_OK - Success
  R_WM_SYS_NG - Fail
  
*/ 
uint32_t R_WM_Sys_ScreenColorCurveSet(const uint32_t                Unit,
                                      const r_wm_ClutEntry_t *const ColorCurve,
                                      const uint32_t                NumEntries
                                     )
{
    uint32_t                ret;
    uint16_t                i;
    float32_t               out_value_r[LOC_WM_GAMMA_ENTRY_NUM];
    float32_t               out_value_g[LOC_WM_GAMMA_ENTRY_NUM];
    float32_t               out_value_b[LOC_WM_GAMMA_ENTRY_NUM];
    uint16_t                in_value[LOC_WM_GAMMA_ENTRY_NUM];
    r_vdce_Error_t          vdce_res;
    r_vdce_GammaCorrect_t   gamma_r = {0};
    r_vdce_GammaCorrect_t   gamma_g = {0};
    r_vdce_GammaCorrect_t   gamma_b = {0};
    r_vdce_Gamma_t          vdce_gamma;

    if ((Unit >= R_WM_DEV_NUM) || (R_NULL == ColorCurve) || (NumEntries != LOC_WM_GAMMA_ENTRY_NUM))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_PARAM_INCORRECT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        vdce_gamma.R = &gamma_r;
        vdce_gamma.G = &gamma_g;
        vdce_gamma.B = &gamma_b;

        for (i = 0u; i < LOC_WM_GAMMA_ENTRY_NUM; i++)
        {
            in_value[i] = i * LOC_GAMMA_SEG_SIZE;
            out_value_r[i] = (float32_t)ColorCurve[i].R;
            out_value_g[i] = (float32_t)ColorCurve[i].G;
            out_value_b[i] = (float32_t)ColorCurve[i].B;
        }
        /* Convert gamma  */
        loc_FitCurveForVdce(in_value, out_value_r, LOC_WM_GAMMA_ENTRY_NUM, &gamma_r);
        loc_FitCurveForVdce(in_value, out_value_g, LOC_WM_GAMMA_ENTRY_NUM, &gamma_g);
        loc_FitCurveForVdce(in_value, out_value_b, LOC_WM_GAMMA_ENTRY_NUM, &gamma_b);
        
        /* Set to VDCE */
        vdce_res = R_VDCE_DisplayGammaCorrectSet(Unit, &vdce_gamma);
        if (vdce_res != R_VDCE_ERR_OK)
        {
            ret = R_WM_SYS_NG;
        }
        else
        {
            ret = R_WM_SYS_OK;
        }
    }
    return ret;
}

/***************************************************************************
  Function: R_WM_Sys_ScreenGammaSet
  
  Description:
  This function set the output gamma correction. This function will overwrite the settings of
  R_WM_ScreenColorCurveSet. Also for Gamma Correction Curves, the slope is limited to the range .
  If parts of the Gamma Curve exceed this range, this function will
  choose the closest match based on the given limitations.
  
  Parameters:
  Unit       - Specifies the WM unit number.
  GammaRed   - Specifies the gamma correction factor for red.
  GammaGreen - Specifies the gamma correction factor for green.
  GammaBlue  - Specifies the gamma correction factor for blue.

  Return Value:
  uint32_t
*/ 
uint32_t R_WM_Sys_ScreenGammaSet(const uint32_t Unit, 
                                 const uint8_t  GammaRed, 
                                 const uint8_t  GammaGreen, 
                                 const uint8_t  GammaBlue)
{
    uint32_t                ret = R_WM_SYS_OK;
    r_vdce_Error_t          vdce_res;
    r_vdce_GammaCorrect_t   vdce_gamma_red =   {0};
    r_vdce_GammaCorrect_t   vdce_gamma_green = {0};
    r_vdce_GammaCorrect_t   vdce_gamma_blue =  {0};
    r_vdce_Gamma_t          vdce_gamma = {R_NULL};
    
    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        if ((GammaRed   == LOC_WM_GAMMA_COEF_CENTER_U) && 
            (GammaGreen == LOC_WM_GAMMA_COEF_CENTER_U) && 
            (GammaBlue  == LOC_WM_GAMMA_COEF_CENTER_U))
        {
            /* All one? Then turn off Gamma correction */
            vdce_res = R_VDCE_DisplayGammaCorrectSet(Unit, R_NULL);
        }
        else
        {
            vdce_gamma.R = &vdce_gamma_red;
            loc_GetGammaCurve(Unit, GammaRed, &vdce_gamma_red);
            
            /* Same as red gamma? Reuse red calculation */
            if (GammaGreen == GammaRed)
            {
                vdce_gamma.G = &vdce_gamma_red;
            }
            else
            {
                vdce_gamma.G = &vdce_gamma_green;
                loc_GetGammaCurve(Unit, GammaGreen, &vdce_gamma_green);
            }
            /* Same as red gamma? Reuse red calculation */
            if (GammaBlue == GammaRed)
            {
                vdce_gamma.B = &vdce_gamma_red;
            }
            /* Same as green gamma? Reuse green calculation */
            else if (GammaBlue == GammaGreen)
            {
                vdce_gamma.B = &vdce_gamma_green;
            }
            else
            {
                vdce_gamma.B = &vdce_gamma_blue;
                loc_GetGammaCurve(Unit, GammaBlue, &vdce_gamma_blue);
            }
            vdce_res = R_VDCE_DisplayGammaCorrectSet(Unit, &vdce_gamma);
        }
        /* Check result */
        if (vdce_res != R_VDCE_ERR_OK)
        {
            ret = R_WM_SYS_NG;
        }
        else
        {
            /* Do nothing. */
        }
    }
    return ret;
}

/***********************************************************
  Function: R_WM_Sys_ScreenColorFormatSet

  Description: 
  This function set the color format for the signals of the specified video output.
  Default format for the video output is RGB888.

  Parameters:
  Uint   - Specifies the WM unit number.
  OutFmt - Specifies the color mode of the video output.

  Return Value:
  uint32_t
*/ 
uint32_t R_WM_Sys_ScreenColorFormatSet(const uint32_t            Unit,
                                       const r_wm_OutColorFmt_t  OutFmt)
{
    uint32_t            format;
    uint32_t            flags;
    uint32_t            ret = R_WM_SYS_OK;
    r_vdce_Error_t      vdce_res;
    r_vdce_OutEndian_t  vdce_out_endian;
    r_vdce_OutFormat_t  vdce_out_format;
    r_vdce_OutSwap_t    vdce_out_swap;
    r_vdce_DitherMd_t   vdce_dither_md;
    r_vdce_Dither_t     vdce_dither;

    if (Unit >= R_WM_DEV_NUM)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_INVALID_WM_UNIT, R_WM_SYS_NO_INFO);
        ret = R_WM_SYS_NG;
    }
    else
    {
        flags  = ((uint32_t)OutFmt & ((uint32_t)R_WM_OUTCOLORFMT_FLAG_MASK));
        
        /* Parse flags endian */
        if ((flags & (uint32_t)R_WM_OUTCOLORFMT_FLAG_ENDIAN) == 0u)
        {
            /* Default value when flag is not set. */
            vdce_out_endian = R_VDCE_OUT_ENDIAN_LITTLE;
        }
        else
        {
            vdce_out_endian = R_VDCE_OUT_ENDIAN_BIG;
        }

        /* Parse flags SWAP */
        if ((flags & (uint32_t)R_WM_OUTCOLORFMT_FLAG_SWAP_BR) == 0u)
        {
            /* Default value when flag is not set. */
            vdce_out_swap = R_VDCE_OUT_SWAP_BR_OFF;
        }
        else
        {
            vdce_out_swap = R_VDCE_OUT_SWAP_BR_ON;
        }
        
        /* Parse flags Dither */
        if ((flags & (uint32_t)R_WM_OUTCOLORFMT_FLAG_DITHER) == 0u)
        {
            /* Default value when flag is not set. */
            vdce_dither_md = R_VDCE_DTH_MD_TRU;
        }
        else
        {
            /* Select: Random pattern dither */
            vdce_dither_md = R_VDCE_DTH_MD_RAND;  
        }

        format = ((uint32_t)OutFmt & (~(uint32_t)R_WM_OUTCOLORFMT_FLAG_MASK));
        /* Parse color formats */
        switch(format)
        {
        case (uint32_t)R_WM_OUTCOLORFMT_RGB888:
            vdce_out_format = R_VDCE_OUT_FORMAT_RGB888;
            break;
        case (uint32_t)R_WM_OUTCOLORFMT_RGB666:
            vdce_out_format = R_VDCE_OUT_FORMAT_RGB666;
            break;
        case (uint32_t)R_WM_OUTCOLORFMT_RGB565:
            vdce_out_format = R_VDCE_OUT_FORMAT_RGB565;
            break;
        default:
            vdce_out_format = R_VDCE_OUT_FORMAT_LAST;
            break;
        }
        
        if (vdce_out_format == R_VDCE_OUT_FORMAT_LAST)
        {
            /* Unsupported color format */
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_DISPLAY_OUTPUT_FORMAT_SET, R_WM_SYS_NO_INFO);
            ret = R_WM_SYS_NG; 
        }
        else
        {
            /* Configure VDCE */
            vdce_res = R_VDCE_DisplayOutEndianSet(Unit, vdce_out_endian);
            if (vdce_res != R_VDCE_ERR_OK)
            {
                ret = R_WM_SYS_NG;
            }
            else
            {
                vdce_res = R_VDCE_DisplayOutSwapBR(Unit, vdce_out_swap);
                if (vdce_res != R_VDCE_ERR_OK)
                {
                    ret = R_WM_SYS_NG;
                }
                else
                {
                    vdce_res = R_VDCE_DisplayOutFormatSet(Unit, vdce_out_format);
                    if (vdce_res != R_VDCE_ERR_OK)
                    {
                        ret = R_WM_SYS_NG;
                    }
                    else
                    {

                        vdce_dither.Sel = vdce_dither_md;
                        /* Dummy values for Pa-Pd. */
                        /*  Only needed for 2x2 pattern dither (R_VDCE_DTH_MD_2X2) */
                        vdce_dither.Pa  = LOC_DITHER_PA_VAL; 
                        vdce_dither.Pb  = LOC_DITHER_PB_VAL; 
                        vdce_dither.Pc  = LOC_DITHER_PC_VAL;
                        vdce_dither.Pd  = LOC_DITHER_PD_VAL;
                        
                        vdce_res = R_VDCE_DisplayCalibrationSet(
                                            Unit, R_NULL, R_NULL, &vdce_dither);
                        if (vdce_res != R_VDCE_ERR_OK)
                        {
                            ret = R_WM_SYS_NG;
                        }
                        else
                        {
                            /* Do nothing. */
                        }
                    }
                }
            }
        }
    }
    return ret;
}

