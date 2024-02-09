/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : Kernel tlist dumping support functions
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 9286 $
 *  $Date: 2016-07-07 19:54:47 +0900 (木, 07 7 2016) $
 *  $LastChangedBy: michael.golczewski $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/    

#ifndef DAVEHD_KERNEL_TLIST_H_INCLUDED
#define DAVEHD_KERNEL_TLIST_H_INCLUDED

#include "davehd_types.h"
#include "davehd_settings.h"

/*----------------------------------------------------------------------------------------------------------*/

#ifdef DHD_KERNEL_TLIST_SUPPORT

dhd_enum_t dhd_gpu_tlist_start(dhd_tlist_t *a_tlist, const dhd_char_t *a_file);
dhd_enum_t dhd_gpu_tlist_stop(dhd_tlist_t  *a_tlist);
void dhd_gpu_tlist_printf(dhd_tlist_t  *a_tlist, dhd_char_t *a_format, ...);

dhd_enum_t dhd_gpu_tlist_enable_internal(dhd_tlist_t *a_tlist, const dhd_char_t *a_file);
dhd_enum_t dhd_gpu_tlist_write_internal(dhd_tlist_t *a_tlist, const dhd_char_t *a_string);
dhd_enum_t dhd_gpu_tlist_alloc(dhd_tlist_t *a_tlist, dhd_uint32_t a_size, dhd_uint32_t a_tag, 
                               dhd_memory_usage_t a_usage, const dhd_vidmem_t *a_result);
dhd_enum_t dhd_gpu_tlist_free(dhd_tlist_t *a_tlist, const dhd_vidmem_t *a_memory);
dhd_enum_t dhd_gpu_tlist_memdump(dhd_gpu_device_data_t *a_device, dhd_tlist_t *a_tlist, const dhd_gpu_ptr_t a_src, dhd_uint32_t a_size);
dhd_enum_t dhd_gpu_tlist_upload(dhd_tlist_t *a_tlist, const dhd_uint8_t *a_src, dhd_gpu_ptr_t a_dest, dhd_uint32_t a_size, dhd_uint32_t a_closure);
dhd_enum_t dhd_gpu_tlist_execute(dhd_tlist_t *a_tlist, dhd_gpu_ptr_t a_address, dhd_uint32_t a_size, dhd_uint32_t a_jobid);
dhd_enum_t dhd_gpu_tlist_regwrite(dhd_tlist_t *a_tlist, dhd_uint32_t a_address, dhd_uint32_t a_value, const dhd_char_t *a_comment);
dhd_enum_t dhd_gpu_tlist_wait_for_syncid(dhd_tlist_t *a_tlist, dhd_uint32_t a_syncid);

/* dhd_gpu_write_reg_tlist writes to a register and at the same time to a tlist*/
#ifdef DHD_KERNEL_DUMP_KERNEL_TLIST
  #define dhd_gpu_write_reg_tlist(base, reg, val, tlist, comment) (void) dhd_gpu_tlist_regwrite(tlist, reg, val, comment),\
  dhd_gpu_write_reg(base, reg, val)                                                                  /* PRQA S 3453 *//* $Misra: #PERF_MACRO_FOR_PERFORMANCE $*/  
#else
  #define dhd_gpu_write_reg_tlist(base, reg, val, tlist, comment) dhd_gpu_write_reg(base, reg, val)  /* PRQA S 3453 *//* $Misra: #PERF_MACRO_FOR_PERFORMANCE $*/  
#endif

#else

#if defined (__ghs__)
#pragma ghs startnomisra
#endif /* __ghs__ */
#define dhd_gpu_write_reg_tlist(base, reg, val, tlist, comment) dhd_gpu_write_reg((base), (reg), (val))    /* PRQA S 3453 *//* $Misra: #PERF_MACRO_FOR_PERFORMANCE $*/
#if defined (__ghs__)
#pragma ghs endnomisra
#endif


#endif

/*----------------------------------------------------------------------------------------------------------*/

#endif /*DAVEHD_KERNEL_TLIST_H_INCLUDED*/