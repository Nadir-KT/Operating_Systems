/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : Direct hardware access (register IO / IRQ access)
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 9286 $
 *  $Date: 2016-07-07 19:54:47 +0900 (木, 07 7 2016) $
 *  $LastChangedBy: michael.golczewski $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/    

#ifndef DAVEHD_KERNEL_HW_H_INCLUDED
#define DAVEHD_KERNEL_HW_H_INCLUDED

#include "davehd_types.h"

/*----------------------------------------------------------------------------------------------------------*/
typedef  void (DHD_IRQ_CALL * dhd_irq_t)(dhd_gpu_device_data_t *a_device);     /* irq callback function pointer*/

/*----------------------------------------------------------------------------------------------------------*/
typedef union tagdhd_gpu_ptr_conversion {
  dhd_gpu_ptr_t m_ptr;
  dhd_uint32_t  m_value;
} dhd_gpu_ptr_conversion_t;

/*----------------------------------------------------------------------------------------------------------*/
/* Reinterpret a gpu ptr as 32bit integer. */
/**/
DHD_INLINE dhd_uint32_t dhd_internal_pcast(dhd_gpu_ptr_t a_ptr);
DHD_INLINE dhd_uint32_t dhd_internal_pcast(dhd_gpu_ptr_t a_ptr) {   /* PRQA S 3450 *//* $Misra: #INLINE_FUNCTION_NO_DECLARATION $*/ /* PRQA S 3480 *//* $Misra: inline function $*/
  dhd_gpu_ptr_conversion_t convert;
  convert.m_ptr = a_ptr;
  return convert.m_value;
}
/*----------------------------------------------------------------------------------------------------------*/

void dhd_gpu_write_reg(void *a_base_address, dhd_uint32_t a_index, dhd_uint32_t a_data);
dhd_uint32_t dhd_gpu_read_reg(void *a_base_address, dhd_uint32_t a_index);
dhd_enum_t dhd_gpu_reset_hw(dhd_gpu_device_data_t *a_device);
void dhd_gpu_setup_irq(dhd_gpu_device_data_t *a_device, dhd_irq_t a_callback);
void dhd_gpu_relax_cpu(dhd_gpu_device_data_t *a_device);

/*----------------------------------------------------------------------------------------------------------*/

#endif /*DAVEHD_KERNEL_HW_H_INCLUDED*/
