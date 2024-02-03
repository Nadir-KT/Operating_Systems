/*
****************************************************************************
PROJECT : JCUA driver
============================================================================
DESCRIPTION
JCUA support functions for d1x
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

SAMPLE CODE is not part of the licensed software, as such it must not be used in
mass-production applications. It can only be used for evaluation and
demonstration purposes at customers premises listed in the signed license
agreement.

****************************************************************************
*/

/***************************************************************************
  Title: JCUA Driver support functions

  Implementation of the JCUA driver support functions for the D1x device.
*/


/***************************************************************************
  Section: Includes
*/
#include    "r_typedefs.h"
#include    "r_jcua_api.h"
#include    "r_jcua_sys.h"
#include    "r_config_jcua.h"
#include    "r_dev_api.h"
#include    "r_tick_api.h"
#include    "r_ostm_api.h"

/***************************************************************************
  Section: Local Defines
*/
/***************************************************************************
  Constant: LOC_DEV_INT_ENABLE

  The offset value of the interrupt enable value.
*/
#define LOC_DEV_INT_ENABLE       (0x01u)

/***************************************************************************
  Constant: LOC_DEV_INT_DISABLE

  The offset value of the interrupt disable value.
*/
#define LOC_DEV_INT_DISABLE      (0x00u)


/***************************************************************************
  Constant: LOC_JCSWRST

  The address of the JCSWRST register (JCUA Software reset register)
*/
#define LOC_JCSWRST     (0xFFC06010u)

/***************************************************************************
  Constant: LOC_JCUA_JCUA0RES

  A constant for bit mask value of JCUA0RES(JCSWRST.JCUA0RES)

  LOC_JCUA0RES_MASK     - Mask value.
  LOC_JCUA0RES_ACTIVE   - Reset active
  LOC_JCUA0RES_INACTIVE - Reset inactive
*/
#define LOC_JCUA0RES_MASK           (0x00000100u)
#define LOC_JCUA0RES_ACTIVE         (0x00000100u)
#define LOC_JCUA0RES_INACTIVE       (0x00000000u)

/***************************************************************************
  Constant: LOC_RESET_WAIT_TIME_US

  Wait time before JCSWRST.
*/
#define LOC_RESET_WAIT_TIME_US       (30u)


/***************************************************************************
  Constant: LOC_BUS_RESET_TIMEOUT_MS / LOC_BUS_RESET_TIMEOUT_US

  The timeout [in milliseconds/microseconds] for JCUA bus reset.
  (JCUA Software reset via JCSWRST register)
*/
#define LOC_BUS_RESET_TIMEOUT_MS     (2u)
#define LOC_BUS_RESET_TIMEOUT_US     (LOC_BUS_RESET_TIMEOUT_MS * 1000u)


/***************************************************************************
  Constant: 

  LOC_USE_OSTM               - Timer select. (0: use TICK0(OSTM0)  1: use OSTM1)

  In case of LOC_USE_OSTM == 1, OSTM driver detect any decode time-out (header or data)
  and OSTM driver notify time-out error to JCUA driver calling R_JCUA_IsrTimeOut callback function.

  loc_OstmInit()
       |
  loc_OstmStart()
       |
       |                        <----- time-out occur!
       +---------------------+  <----- OSTM interrupt!
                             |
                       loc_OstmCallback()
                          call  R_JCUA_IsrTimeOut() -> mortify time-out to JCUA driver
                             |
       +---------------------+
       |

*/

#define LOC_USE_OSTM                     (1)

/***************************************************************************
  Constant: 

  LOC_OSTM_UNIT              - Use OSTM unit number.
  LOC_HEADER_TIMEOUT_US      - Time-out value of Header part. (in usec)
                               If time-out is not needed, please define as 0.
  LOC_DATA_TIMEOUT_US        - Time-out value of Image Data part. (in usec)
                               If time-out is not needed, please define as 0.
  LOC_MHZ_TO_HZ              - Coefficient to convert MHz to Hz.
  LOC_COUNT_FAIL_SAFE        - Compare count default value.
  LOC_UNUSED_TRRIGER         - The parameter is not used as start trigger
  LOC_TICK_UNIT              - Use TICK unit number.
  LOC_CPU_FREQ_MAX           - Maximum CPU clock frequency (Hz). 
                               It does not matter if it is greater than actual value.
  LOC_RESET_LOOP_MAX         - Maximum loop count

*/
#if (LOC_USE_OSTM == 1)
#define LOC_OSTM_UNIT                    (1u)
#define LOC_HEADER_TIMEOUT_US            (4000u) /* 4 msec */
#define LOC_DATA_TIMEOUT_US              (0u)    /* disable */
#define LOC_MHZ_TO_HZ                    (1000u * 1000u)
#define LOC_COUNT_FAIL_SAFE              (1u)
#define LOC_UNUSED_TRIGGER               (0u)
#define LOC_CPU_FREQ_MAX                 (240u) /* 240MHz */
#define LOC_RESET_LOOP_MAX               (LOC_CPU_FREQ_MAX * LOC_RESET_WAIT_TIME_US)
#else
#define LOC_TICK_UNIT                    (0u)
#endif

/***************************************************************************
  Section: Local Values
*/

#if (LOC_USE_OSTM == 1)
R_JCUA_STATIC int32_t  loc_TimerReached;
R_JCUA_STATIC uint32_t loc_PauseCouter;
R_JCUA_STATIC int32_t  loc_TiemOutCallback;
#endif

/***************************************************************************
  Variable: loc_JcuaState

  This value stores the status of JCUA driver.
*/
#ifdef R_BSP_SYS_INIT_USE
R_JCUA_STATIC r_jcua_State_t loc_JcuaState[R_JCUA_MACRO_NUM];
#else
R_JCUA_STATIC r_jcua_State_t loc_JcuaState[R_JCUA_MACRO_NUM] = {R_JCUA_STATE_UNINITIALIZED};
#endif

/***************************************************************************
  Section: local Functions
*/

#if (LOC_USE_OSTM == 1)
/***************************************************************************
  Function: loc_OstmStop

  Description:
  This function stops OSTM driver.

  Parameter:
  Counter - Pointer to store the remain counter to reach timeout. 

  Return value:
  See <r_jcua_Error_t>.
*/
R_JCUA_STATIC r_jcua_Error_t loc_OstmStop(uint32_t *const Counter);
R_JCUA_STATIC r_jcua_Error_t loc_OstmStop(uint32_t *const Counter)
{
    r_jcua_Error_t     error = R_JCUA_ERR_OK;
    r_ostm_Error_t     ostm_err;
    
    ostm_err = R_OSTM_Stop(LOC_OSTM_UNIT);
    if (ostm_err == R_OSTM_ERR_OK)
    {
        if (Counter != R_NULL)
        {
            *Counter = R_OSTM_GetCounterValue(LOC_OSTM_UNIT);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        error = R_JCUA_ERR_TIMER_CTRL;
    }
    return error;
}

/***************************************************************************
  Function: loc_OstmCallback

  Description:
  This function is called when the setting time has arrived.
  It will be called from ISR.

  Parameter:
  None.

  Return value:
  None.
*/
R_JCUA_STATIC void loc_OstmCallback(void);
R_JCUA_STATIC void loc_OstmCallback(void)
{
    loc_TimerReached = R_TRUE;
    (void)loc_OstmStop(R_NULL);
    /* Fall through if error occurs. */
    
    if (loc_TiemOutCallback == R_TRUE)
    {
        /* Emergency Stop */
        R_JCUA_Write32(LOC_JCSWRST, LOC_JCUA0RES_ACTIVE);
        
        R_JCUA_IsrTimeOut(R_JCUA_UNIT0);
        
        loc_TiemOutCallback = R_FALSE;
    }
    else
    {
        /* Do nothing */
    }
}

/***************************************************************************
  Function: loc_OstmInit

  Description:
  This function initializes OSTM driver and set a time-out callback function to OSTM driver.

  Parameter:
  None.

  Return value:
  See <r_jcua_Error_t>.
*/
R_JCUA_STATIC r_jcua_Error_t loc_OstmInit(void);
R_JCUA_STATIC r_jcua_Error_t loc_OstmInit(void)
{
    r_jcua_Error_t     error = R_JCUA_ERR_OK;
    r_ostm_Error_t     ostm_err;
    r_ostm_Parameter_t ostm_prm;
    
    /* Init OS timer */
    ostm_prm.Mode   = R_OSTM_INTERVAL; 
    ostm_prm.CmpVal = 0;
    ostm_prm.IntCtl = R_OSTM_START_INT_DISABLE;
    
    ostm_err = R_OSTM_Init(LOC_OSTM_UNIT, &ostm_prm);
    if (ostm_err == R_OSTM_ERR_OK)
    {
        /* Set callback */
        ostm_err = R_OSTM_SetIsrCallback(LOC_OSTM_UNIT, R_OSTM_INT_TINT, &loc_OstmCallback);
        if (ostm_err == R_OSTM_ERR_OK)
        {
            ostm_err = R_OSTM_EnableInt(LOC_OSTM_UNIT, R_OSTM_INT_TINT);
        }
        else
        {
            /* DO NOTHING */
        }
    }
    else
    {
        /* DO NOTHING */
    }
    if (ostm_err != R_OSTM_ERR_OK)
    {
        error = R_JCUA_ERR_TIMER_CTRL;
    }
    else
    {
        /* DO NOTHING */
    }

    return error;
}

/***************************************************************************
  Function: loc_OstmDeInit

  Description:
  This function de-initializes OSTM driver.

  Parameter:
  None.

  Return value:
  See <r_jcua_Error_t>.
*/
R_JCUA_STATIC r_jcua_Error_t loc_OstmDeInit(void);
R_JCUA_STATIC r_jcua_Error_t loc_OstmDeInit(void)
{
    r_jcua_Error_t     error = R_JCUA_ERR_OK;
    r_ostm_Error_t     ostm_err;
    
    ostm_err = R_OSTM_DisableInt(LOC_OSTM_UNIT, R_OSTM_INT_TINT);
    if (ostm_err == R_OSTM_ERR_OK)
    {
        /* Deinit OS timer */
        ostm_err = R_OSTM_DeInit(LOC_OSTM_UNIT);
    }
    else
    {
        /* DO NOTHING */
    }
    if (ostm_err != R_OSTM_ERR_OK)
    {
        error = R_JCUA_ERR_TIMER_CTRL;
    }
    else
    {
        /* DO NOTHING */
    }

    return error;
}

/***************************************************************************
  Function: loc_OstmStart

  Description:
  This function starts OSTM driver.
  If specified time or specified count is passed, <loc_OstmCallback> will called.

  Parameter:
  Usec  - Micro second time. If Usec != 0, Usec is used.
  Count - Count by OSTM clock frequency. If Usec == 0, Count is used.

  Return value:
  See <r_jcua_Error_t>.
*/

R_JCUA_STATIC r_jcua_Error_t loc_OstmStart(const uint32_t Usec, const uint32_t Count);
R_JCUA_STATIC r_jcua_Error_t loc_OstmStart(const uint32_t Usec, const uint32_t Count)
{
    r_jcua_Error_t     error = R_JCUA_ERR_OK;
    r_ostm_Error_t     ostm_err;
    uint32_t           cmp_val;
    uint32_t           ostm_clk;
    uint32_t           ostm_mhz;
    
    /* Get clock */
    ostm_clk = R_OSTM_GetClock(LOC_OSTM_UNIT);
    /* Hz->MHz */
    ostm_mhz = ostm_clk / LOC_MHZ_TO_HZ;
    if (ostm_mhz != 0u)
    {
        /* Set Compare value */
        if (Usec != LOC_UNUSED_TRIGGER)
        {
            cmp_val = ostm_mhz * Usec;
        }
        else if (Count != LOC_UNUSED_TRIGGER)
        {
            cmp_val = Count;
        }
        else
        {
            /* Set non-zero for Fail safe */
            cmp_val = LOC_COUNT_FAIL_SAFE;
        }
        ostm_err = R_OSTM_SetCmp(LOC_OSTM_UNIT, cmp_val);
        if (ostm_err == R_OSTM_ERR_OK)
        {
            /* Timer start */
            loc_TimerReached = R_FALSE;
            ostm_err = R_OSTM_Start(LOC_OSTM_UNIT);
        }
        else
        {
            /* DO NOTHING */
        }
        if (ostm_err != R_OSTM_ERR_OK)
        {
            error = R_JCUA_ERR_TIMER_CTRL;
        }
        else
        {
            /* DO NOTHING */
        }
    }
    else
    {
        error = R_JCUA_ERR_TIMER_CTRL;
    }

    return error;
}
#endif /* LOC_USE_OSTM */

/***************************************************************************
  Section: JCUA Driver Support Functions
*/

/***************************************************************************
  Group: JCUA Basic interface functions
*/

/***********************************************************
  Function: R_JCUA_Sys_InitGlobal

  see: <r_jcua_sys.h>
*/
void R_JCUA_Sys_InitGlobal(void)
{
    uint32_t i;
    
    for (i = 0u; i < R_JCUA_MACRO_NUM; i++)
    {
        loc_JcuaState[i] = R_JCUA_STATE_UNINITIALIZED;
    }
}
/***************************************************************************
  Function: R_JCUA_Sys_Init

  see: <r_jcua_sys.h>
*/

r_jcua_Error_t R_JCUA_Sys_Init(const uint32_t Unit)
{
    r_jcua_Error_t error;
    uint32_t       val;
#if (LOC_USE_OSTM == 0)
    uint32_t       timeout;
    uint32_t       timeout_limit;
#else
    uint32_t       exit_count = 0u;
#endif

    if (Unit == R_JCUA_UNIT0)
    {
        /* Initialize Unit 0 of JCUA macro. */

        /* Bus Reset for JCUA (with time-out) */
        /* D1x OPC Rev5.01 item #12 "JCUA reset operation"
           -> JCCMD.BRST must not be used. (JCU macro internal reset not sufficient)
           -> JCSWRST.JCUA0RES must be used instead. (JCU macro external reset)
              Refer to D1x UM section 44.6 "Software Reset Processing".
                Use the following software reset procedure to reset the JCUA unit:
                0. Wait for 30us.
                1. Set JCSWRST.JCUA0RES bit to 1, to generate the software reset condition.
                2. Read JCSWRST.JCUA0RES bit until bit is set to 1, to wait for software reset
                   is active.
                3. Set JCSWRST.JCUA0RES bit to 0, to release software reset condition.
                By this sequence all JCUA registers with exception
                of the JCSWRST register are initialized by a software reset.
        */
#if (LOC_USE_OSTM == 0)
        error = R_JCUA_ERR_OK;
        timeout = (R_TICK_GetTimeUS(LOC_TICK_UNIT) + LOC_RESET_WAIT_TIME_US);
        do
        {
            ;
        } while (timeout > R_TICK_GetTimeUS(LOC_TICK_UNIT));
        R_JCUA_Write32(LOC_JCSWRST, LOC_JCUA0RES_ACTIVE);
        val = LOC_JCUA0RES_INACTIVE;
        timeout = (R_TICK_GetTimeMS(LOC_TICK_UNIT) + LOC_BUS_RESET_TIMEOUT_MS);
        timeout_limit = R_TICK_GetTimeMS(LOC_TICK_UNIT);
        while ((val & LOC_JCUA0RES_MASK) == LOC_JCUA0RES_INACTIVE) 
        {
            if (timeout <= timeout_limit)
            {
                /* time-out occurred */
                error = R_JCUA_ERR_BUS_TIMEOUT;
                break;
            }
            else
            {
                val = R_JCUA_Read32(LOC_JCSWRST);
            }
        }
        if (error == R_JCUA_ERR_OK)
        {
            R_JCUA_Write32(LOC_JCSWRST, LOC_JCUA0RES_INACTIVE);

            /* Enable interrupt. */
            R_JCUA_Sys_InterruptEnable(R_JCUA_UNIT0);
        }
        else
        {
            /* DO NOTHING */
        }
#else
        /* Initialize OS timer unit */
        error = loc_OstmInit();
        if (error == R_JCUA_ERR_OK)
        {
            loc_TiemOutCallback = R_FALSE;
            error = loc_OstmStart(LOC_RESET_WAIT_TIME_US, LOC_UNUSED_TRIGGER);
            if (error == R_JCUA_ERR_OK)
            {
                /* 0. Wait for 30us. */
                while (loc_TimerReached == R_FALSE)
                {
                    exit_count++;
                    if (exit_count >= LOC_RESET_LOOP_MAX)
                    {
                        break;
                    }
                    else
                    {
                        /* DO NOTHING */
                    }
                }
                /* 1. Set JCSWRST.JCUA0RES bit to 1 */
                R_JCUA_Write32(LOC_JCSWRST, LOC_JCUA0RES_ACTIVE);
        
                error = loc_OstmStart(LOC_BUS_RESET_TIMEOUT_US, LOC_UNUSED_TRIGGER);
                if (error == R_JCUA_ERR_OK)
                {
                    /* 2. Read JCSWRST.JCUA0RES bit until bit is set to 1 */
                    val = LOC_JCUA0RES_INACTIVE;
                    while ((val & LOC_JCUA0RES_MASK) == LOC_JCUA0RES_INACTIVE)
                    {
                        if (loc_TimerReached == R_TRUE)
                        {
                            /* time-out occurred */
                            (void)loc_OstmStop(R_NULL);
                            error = R_JCUA_ERR_BUS_TIMEOUT;
                            break;
                        }
                        else
                        {
                            val = R_JCUA_Read32(LOC_JCSWRST);
                        }
                    }
                    if (error == R_JCUA_ERR_OK)
                    {
                        error = loc_OstmStop(R_NULL);
                        if (error == R_JCUA_ERR_OK)
                        {
                            /* 3. Set JCSWRST.JCUA0RES bit to 0 */
                            R_JCUA_Write32(LOC_JCSWRST, LOC_JCUA0RES_INACTIVE);
                    
                            /* Enable interrupt. */
                            R_JCUA_Sys_InterruptEnable(R_JCUA_UNIT0);
                        }
                        else
                        {
                            /* DO NOTHING */
                        }
                    }
                }
                else
                {
                    /* DO NOTHING */
                }
            }
            else
            {
                /* DO NOTHING */
            }
        }
        else
        {
            /* DO NOTHING */
        }
#endif
    }
    else
    {
        error = R_JCUA_ERR_RANGE_UNIT;
    }
    return error;
}

/***************************************************************************
  Function: R_JCUA_Sys_DeInit

  see: <r_jcua_sys.h>
*/

r_jcua_Error_t R_JCUA_Sys_DeInit(const uint32_t Unit)
{
    r_jcua_Error_t error;

    if (Unit == R_JCUA_UNIT0)
    {
        /* De-Initialize Unit 0 of JCUA macro. */
        /* Disable interrupt. */
        R_JCUA_Sys_InterruptDisable(R_JCUA_UNIT0);
#if (LOC_USE_OSTM == 1)
        error = loc_OstmDeInit();
#endif
    }
    else
    {
        error = R_JCUA_ERR_RANGE_UNIT;
    }

    return error;
}

/***************************************************************************
  Function: R_JCUA_Sys_InterruptEnable

  see: <r_jcua_sys.h>
*/

void R_JCUA_Sys_InterruptEnable(const uint32_t Unit)
{
    if (Unit == R_JCUA_UNIT0)
    {
        R_DEV_IntEnable(R_DEV_INT_JCU0EDI, LOC_DEV_INT_ENABLE);
        R_DEV_IntEnable(R_DEV_INT_JCU0DTI, LOC_DEV_INT_ENABLE);
    } 
    /* end if */
    else
    {
        /* DO NOTHING */
    }
}


/***************************************************************************
  Function: R_JCUA_Sys_InterruptDisable

  see: <r_jcua_sys.h>
*/

void R_JCUA_Sys_InterruptDisable(const uint32_t Unit)
{
    if (Unit == R_JCUA_UNIT0)
    {
        R_DEV_IntEnable(R_DEV_INT_JCU0DTI, LOC_DEV_INT_DISABLE);
        R_DEV_IntEnable(R_DEV_INT_JCU0EDI, LOC_DEV_INT_DISABLE);
    } 
    /* end if */
    else
    {
        /* DO NOTHING */
    }
}

/***************************************************************************
  Function: R_JCUA_Sys_BaseAddrGet

  see: <r_jcua_sys.h>
*/
uint32_t R_JCUA_Sys_BaseAddrGet(const uint32_t Unit)
{
    uint32_t base_addr;

    if (Unit == R_JCUA_UNIT0)
    {
        base_addr = R_JCUA_BASE_ADD;
    }
    else
    {
        base_addr = 0u;
    }

    return base_addr;
}

/***************************************************************************
  Function: R_JCUA_Sys_TimerStart

  see: <r_jcua_sys.h>
*/

r_jcua_Error_t R_JCUA_Sys_TimerStart(const uint32_t Unit, const uint32_t IsHeader)
{
    r_jcua_Error_t error = R_JCUA_ERR_OK;
    
    if (Unit == R_JCUA_UNIT0)
    {
#if (LOC_USE_OSTM == 1)
        if (IsHeader == (uint32_t)R_TRUE)
        {
#if (LOC_HEADER_TIMEOUT_US != 0)
            loc_TiemOutCallback = R_TRUE;
            error = loc_OstmStart(LOC_HEADER_TIMEOUT_US, LOC_UNUSED_TRIGGER);
#endif
        }
        else
        {
#if (LOC_DATA_TIMEOUT_US != 0)
            loc_TiemOutCallback = R_TRUE;
            error = loc_OstmStart(LOC_DATA_TIMEOUT_US, LOC_UNUSED_TRIGGER);
#endif
        }
#endif /* LOC_USE_OSTM */
    }
    else
    {
        error = R_JCUA_ERR_RANGE_UNIT;
    }
    
    return error;
}

/***************************************************************************
  Function: R_JCUA_Sys_TimerPause

  see: <r_jcua_sys.h>
*/
r_jcua_Error_t R_JCUA_Sys_TimerPause(const uint32_t Unit)
{
    r_jcua_Error_t error;
    
    if (Unit == R_JCUA_UNIT0)
    {
#if (LOC_USE_OSTM == 1)
        error = loc_OstmStop(&loc_PauseCouter);
#endif
    }
    else
    {
        error = R_JCUA_ERR_RANGE_UNIT;
    }
    
    return error;
}

/***************************************************************************
  Function: R_JCUA_Sys_TimerResume

  see: <r_jcua_sys.h>
*/
r_jcua_Error_t R_JCUA_Sys_TimerResume(const uint32_t Unit)
{
    r_jcua_Error_t error = R_JCUA_ERR_OK;
    
    if (Unit == R_JCUA_UNIT0)
    {
#if (LOC_USE_OSTM == 1)
        if (loc_TiemOutCallback == R_TRUE)
        {
            error = loc_OstmStart(LOC_UNUSED_TRIGGER, loc_PauseCouter);
        }
#endif
    }
    else
    {
        error = R_JCUA_ERR_RANGE_UNIT;
    }
    
    return error;
}

/***************************************************************************
  Function: R_JCUA_Sys_TimerStop

  see: <r_jcua_sys.h>
*/
r_jcua_Error_t R_JCUA_Sys_TimerStop(const uint32_t Unit)
{
    r_jcua_Error_t error;

    if (Unit == R_JCUA_UNIT0)
    {
#if (LOC_USE_OSTM == 1)
        loc_TiemOutCallback = R_FALSE;
        error = loc_OstmStop(R_NULL);
#endif
    }
    else
    {
        error = R_JCUA_ERR_RANGE_UNIT;
    }

    return error;
}

/***************************************************************************
  Function: R_JCUA_Sys_StateSet

  see: <r_jcua_sys.h>
*/
r_jcua_Error_t R_JCUA_Sys_StateSet(const uint32_t Unit,
                                   const r_jcua_State_t State)
{
    r_jcua_Error_t error = R_JCUA_ERR_OK;

    if (Unit == R_JCUA_UNIT0)
    {
        loc_JcuaState[Unit] = State;
    }
    else
    {
        error = R_JCUA_ERR_RANGE_UNIT;
    }

    return error;
}

/***************************************************************************
  Function: R_JCUA_Sys_StateGet

  see: <r_jcua_sys.h>
*/
r_jcua_State_t R_JCUA_Sys_StateGet(const uint32_t Unit)
{
    r_jcua_State_t state;

    if (Unit == R_JCUA_UNIT0)
    {
        state = loc_JcuaState[Unit];
    }
    else
    {
        state = R_JCUA_STATE_UNINITIALIZED;
    }

    return state;
}

/***************************************************************************
  Group: JCUA OS interface
*/

/***************************************************************************
  Function: R_JCUA_Sys_Lock

  see: <r_jcua_sys.h>
*/

r_jcua_Error_t R_JCUA_Sys_Lock(const uint32_t Unit)
{
    r_jcua_Error_t error = R_JCUA_ERR_OK;

    /* Check parameters. */
    if (Unit >= R_JCUA_MACRO_NUM)
    {
        error = R_JCUA_ERR_RANGE_UNIT;
    } 
    /* end if */
    else
    {
        /* Please implement the lock process by mutex or semaphore */
        /* if JCUA API is called from multi-thread. */
    }
    
    return error;
}

/***************************************************************************
  Function: R_JCUA_Sys_Unlock

  see: <r_jcua_sys.h>
*/

r_jcua_Error_t R_JCUA_Sys_Unlock(const uint32_t Unit)
{
    r_jcua_Error_t error = R_JCUA_ERR_OK;
    
    /* Check parameters. */
    if (Unit >= R_JCUA_MACRO_NUM)
    {
        error = R_JCUA_ERR_RANGE_UNIT;
    } 
    /* end if */
    else
    {
        /* Please implement the unlock process depending on R_JCUA_Sys_Lock. */
    }
    
    return error;
}
