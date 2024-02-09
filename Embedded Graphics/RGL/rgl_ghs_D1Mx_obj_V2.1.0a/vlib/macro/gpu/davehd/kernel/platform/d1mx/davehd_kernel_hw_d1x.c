/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : Direct hardware access (register IO / IRQ access)
 *  Platform: Micro SoftDave (dummy implementation)
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 4170 $
 *  $Date: 2014-10-22 23:51:50 +0900 (水, 22 10 2014) $
 *  $LastChangedBy: florian.zimmermann $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/

#include "r_typedefs.h"
#include "davehd_kernel_driver.h"
#include "davehd_kernel_os.h"
#include "davehd_kernel_hw.h"
#include "davehd_stcregistermap.h"
#include "davehd_registertypes.h"
#include "davehd_os_hw.h"
#include "davehd_kernel_debug.h"
#include "davehd_guards.h"

/*----------------------------------------------------------------------------------------------------------*/
/* Write a HW register.*/
/**/
/* Parameters:*/
/*  a_base_address - register base address as specified in dhd_gpu_config_t when kernel mode was setup.*/
/*  a_index - register index.*/
/*  a_data - data to write.*/
/**/
void dhd_gpu_write_reg(void *a_base_address, dhd_uint32_t a_index, dhd_uint32_t a_data) {

  dhd_sys_regwrite( a_base_address, a_index, a_data );
}

/*----------------------------------------------------------------------------------------------------------*/
/* Write a HW register.*/
/**/
/* Parameters:*/
/*  a_base_address - register base address as specified in dhd_gpu_config_t when kernel mode was setup.*/
/*  a_index - register index.*/
/**/
/* Returns:*/
/*  32 bit data from register*/
/**/
dhd_uint32_t dhd_gpu_read_reg(void *a_base_address, dhd_uint32_t a_index) {

  return dhd_sys_regread( a_base_address, a_index );
}

/*----------------------------------------------------------------------------------------------------------*/
/* Perform a HW reset. Depending on the platform this can be quite complicated, as the core has no single*/
/* reset line.*/
/**/
/* Parameters:*/
/*  a_device - kernel device structure.*/
/**/
/* Returns:*/
/*  E_DHD_OK on success E_DHD_ERROR otherwise*/
dhd_enum_t dhd_gpu_reset_hw(dhd_gpu_device_data_t *a_device) {
  void* base_address = a_device->m_gpu_config.m_register_base_address;
  dhd_reg_stc_control_t control;
  dhd_reg_stc_interrupt_status_t irq_clear;
  dhd_uint32_t status;

  /* disable full control register, including all interrupts*/
  control.m_value = 0;
  dhd_gpu_write_reg(base_address, DHD_REG_STC_CONTROL, control.m_value);

  /* clear interrupt status*/
  irq_clear.m_value = 0;
  irq_clear.m_fields.m_mbierror_irq = 1;
  irq_clear.m_fields.m_pause_irq = 1;
  irq_clear.m_fields.m_stall_irq = 1;
  irq_clear.m_fields.m_stop_irq = 1;
  dhd_gpu_write_reg(base_address, DHD_REG_STC_INTERRUPT_CLEAR, irq_clear.m_value);

  /* get hw status*/
  status = dhd_gpu_read_reg(base_address, DHD_REG_STC_BUSY_STATUS);

  /* stop the hw if needed*/
  if (0 != status) {
    static const dhd_int32_t wait_timeout = 50;
    dhd_int32_t i;

    dhd_reg_stc_interrupt_status_t irq_status;

    if (status != 0x1) {
      /* this is not as expected: when the hw is busy, it should be waiting in the ring at the pause address*/
      /* and only the STC should be busy*/
      dhd_gpu_debug_note(a_device, "HW is in unexpected state for reset: Status is 0x%03x.", status);
      return E_DHD_ERROR;
    }

    /* issue a halt request*/
    control.m_fields.m_stream_halt_request = 1;
    dhd_gpu_write_reg(base_address, DHD_REG_STC_CONTROL, control.m_value);

    /* release the pause address to let the halt finish*/
    dhd_gpu_write_reg(base_address, DHD_REG_STC_RING_PAUSE_ADDRESS, 0);

    /* wait some time for the hw to stop TODO: could use stop IRQ here*/
    for (i=0; (0 != status) && (i<wait_timeout); i++) {
      dhd_gpu_relax_cpu(a_device);
      status = dhd_gpu_read_reg(base_address, DHD_REG_STC_BUSY_STATUS);
    }

    if (0 != status) {
      dhd_gpu_debug_note(a_device, "HW could not be stopped during dhd_gpu_reset_hw. Status is still 0x%03x after timeout.", status);
      return E_DHD_ERROR;
    }

    /* get irq status*/
    irq_status.m_value = dhd_gpu_read_reg(base_address, DHD_REG_STC_INTERRUPT_STATUS);

    if (irq_status.m_fields.m_stop_code != E_STOP_IRQ_CODE_STREAM_HALT) {
      dhd_gpu_debug_note(a_device, "HW stopped during dhd_gpu_reset_hw, but stop IRQ code is not as expected: 0x%x.", irq_status.m_fields.m_stop_code);
      return E_DHD_ERROR;
    }

    /* finally clear interrupt status*/
    dhd_gpu_write_reg(base_address, DHD_REG_STC_INTERRUPT_CLEAR, irq_clear.m_value);
  }
  return E_DHD_OK;
}


#ifdef DHD_USE_SYNC_IRQ
/*----------------------------------------------------------------------------------------------------------*/
/* Handle sync IRQ. called to handle platform specific stuff in case of a sync irq (e.g. wakeup kernel thread)*/
/**/
/* Parameters:*/
/*  a_device - kernel device structure.*/
void dhd_gpu_sync_irq(dhd_gpu_device_data_t *a_device) {

}
#endif /* DHD_USE_SYNC_IRQ */

/*----------------------------------------------------------------------------------------------------------*/
/* trampoline for irq callbacks ( used to hide signature difference between softdave and driver )*/
/**/
static void dhd_irq_callback(dhd_sys_handle_t a_caller);
static void dhd_irq_callback(dhd_sys_handle_t a_caller) {  
  dhd_sys_irq_t callback;
  dhd_gpu_device_data_t *device;

  callback = (dhd_sys_irq_t) dhd_sys_get_callback( a_caller );
  device = (dhd_gpu_device_data_t *) dhd_sys_get_device( a_caller );
  if (0 != callback) {
    callback( device );
  }
}

/*----------------------------------------------------------------------------------------------------------*/
/* Register IRQ callback.*/
/**/
/* Parameters:*/
/*  a_device - kernel device structure.*/
/*  a_callback - function pointer to IRQ handler (will be used by the driver to register it's IRQ)*/
/**/
void dhd_gpu_setup_irq(dhd_gpu_device_data_t *a_device, dhd_irq_t a_callback) {
  void *handle = a_device->m_gpu_config.m_register_base_address;

  dhd_sys_register_isr( handle, 0 );
  if (0 != a_callback) {
    dhd_sys_set_callback( handle, (a_callback));
    dhd_sys_set_device( handle, a_device );
    dhd_sys_register_isr( handle, &dhd_irq_callback );
  }
}

/*----------------------------------------------------------------------------------------------------------*/
/* Waste some cycles to ease polling loops. In polling mode the driver will have to relax in order to avoid*/
/* stressing the bus with too many register read requests.*/
/**/
/* Parameters:*/
/*  a_device - kernel device structure.*/

void dhd_gpu_relax_cpu(dhd_gpu_device_data_t *a_device) {
  asm("nop");
}