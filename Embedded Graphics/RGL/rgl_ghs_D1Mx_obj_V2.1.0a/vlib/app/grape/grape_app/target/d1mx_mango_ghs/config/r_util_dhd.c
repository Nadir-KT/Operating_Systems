/************************************************************************************************
PROJECT : 
FILE    : $Id: r_util_dhd.c 11006 2016-11-07 16:32:44Z michael.golczewski $
============================================================================ 
DESCRIPTION
DHD Init function
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
#include "r_typedefs.h"     /* Renesas basic types, e.g. uint32_t */
#include "r_cdi_api.h"

#include "davehd_kernel_driver.h"
#include "davehd_kernel_bridge.h"
#include "davehd_os_libc.h"
#include "r_sys_davehd.h"
#include "r_util_dhd.h"
#include "fw_osal_api.h"

#include <string.h>

/*******************************************************************************
  Section: Local defines
*/
#define LOC_DHD_PRIO             0x21
#define LOC_DHD_STACKSIZE        0x500

/*******************************************************************************
  Section: Local variables
  
*/

#ifdef USE_THREADED_DHD
static uint32_t locStackDHDThread[LOC_DHD_STACKSIZE >> 2];
#endif

/*******************************************************************************
  Section: Local functions

*/

static void* locDHDDispatch(void* Dummy)
{
    while(Dummy == R_NULL)
    {
        dhd_gpu_dispatch_loop();
    }
    return R_NULL;
}


/*----------------------------------------------------------------------------------------------------------*/
/* Initialise all DHD interrupts from CPU side */
/**/
/* Parameters: */
/*  Unit - macro unit number */
void R_UTIL_DHD_Init(dhd_uint32_t Unit)
{
    R_SYS_DHD_IntCInit(Unit);
}

/*----------------------------------------------------------------------------------------------------------*/
/* Configure and Init DHD  */
/**/
/* Parameters: */
/*  Base - Base address of VIDEO working RAM */
/*  VidSize - Available RAM in bytes */
dhd_uint32_t R_UTIL_DHD_Config(dhd_gpu_ptr_t Base, dhd_uint32_t VidSize, r_cdi_Heap_t *Heap)
{
    dhd_gpu_call_data_t gpucall;
    dhd_gpu_config_t gpu_config;
    dhd_uint32_t ret;
    uint8_t sync = 1;
#ifdef USE_THREADED_DHD
    sync = 0;
#endif
    dhd_gpu_bridge_init_server();
    dhd_set_cpu_heap(Heap);

#ifdef USE_THREADED_DHD
    FW_OSAL_ThreadCreate(locDHDDispatch, 0, locStackDHDThread,
                       LOC_DHD_STACKSIZE, LOC_DHD_PRIO);
#endif

    /* prepare device description structure */
    memset(&gpu_config, 0, sizeof(gpu_config));
    gpu_config.m_sizeof = sizeof(gpu_config);

    /* init hardware */
    gpu_config.m_register_base_address = (void*)R_SYS_DHD_GetBaseAddr(0);

    gpu_config.m_vidmem_base_address   = Base;
    gpu_config.m_vidmem_size           = VidSize;

    /* init device description */
    gpu_config.m_default_jobsize   = DHD_JOBSIZE;
    gpu_config.m_default_jobcount  = DHD_JOBCOUNT;
    gpu_config.m_default_ringsize  = DHD_RINGSIZE;
    gpu_config.m_force_synchronous = sync;

    /* try to init kernel mode driver instance 0 */
    gpucall.m_init.m_instance = dhd_gpu_prepare_instance(0);
    gpucall.m_init.m_config   = &gpu_config;
    ret = dhd_gpu_call( 0, E_DHD_CALL_INIT, &gpucall);
    switch(ret)
    {
        case E_DHD_OK:
            break;
        case E_DHD_ERROR_HW_REVISION_MISMATCH:
            /* "ERROR: HW revision not supported by this driver */
        case E_DHD_ERROR_INVALID_ARGUMENT:
            /* "ERROR: requested HW instance not available; */
        default:
            /* "ERROR: failed to initialize kernel mode driver */
            while(1) {}
     }
    return ret;
}


/*----------------------------------------------------------------------------------------------------------*/
/* DeInit DHD  */
/**/
/* Parameters: */
/*  Base - Base address of VIDEO working RAM */
/*  VidSize - Available RAM in bytes */
dhd_uint32_t R_UTIL_DHD_DeInit(dhd_uint32_t Unit)
{
    dhd_gpu_call_data_t gpucall;
    dhd_uint32_t ret;

    /* try to init kernel mode driver instance 0 */
    gpucall.m_shutdown.m_instance = 0;
    ret = dhd_gpu_call( 0, E_DHD_CALL_SHUTDOWN, &gpucall);
    switch(ret)
    {
        case E_DHD_OK:
            break;
        case E_DHD_ERROR_HW_REVISION_MISMATCH:
            /* "ERROR: HW revision not supported by this driver */
        case E_DHD_ERROR_INVALID_ARGUMENT:
            /* "ERROR: requested HW instance not available; */
        default:
            /* "ERROR: failed to initialize kernel mode driver */
            while(1) {}
     }
    R_SYS_DHD_IntCDeInit(Unit);

    return ret;
}

