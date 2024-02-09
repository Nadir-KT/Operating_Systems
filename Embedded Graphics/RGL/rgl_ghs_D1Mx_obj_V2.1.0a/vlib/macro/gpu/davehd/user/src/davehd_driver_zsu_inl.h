/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : ZSA Unit (ZSU) related inline functions
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 4771 $
 *  $Date: 2015-02-10 20:01:30 +0900 (火, 10 2 2015) $
 *  $LastChangedBy: florian.zimmermann $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/    

#ifndef DAVEHD_DRIVER_ZSU_INL_H_INCLUDED
#define DAVEHD_DRIVER_ZSU_INL_H_INCLUDED
#include "davehd_registerparams.h"

/* PRQA S 3406 ++ *//* $Misra: #INLINE_FUNCTION_DEFINED_IN_HEADER $*/

/*----------------------------------------------------------------------------------------------------------*/
/* Disable all ZSA access. This functions changes the states of global alpha mask, stencil and depth tests*/
/* as well as ZSA read enable in order to ensure that no further ZSA Buffer read or write access will occur.*/
/* It should be used when ZSA Buffer address (dhd_set_zsabuffer_address) is no longer valid. Of course, the*/
/* states can also be set manually to values that ensure no read or write to ZSA will happen.*/
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
DHD_API_INLINE void dhd_disable_zsa(dhd_handle_t a_device) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  /* disable all features that would cause a zsa read or write*/
  device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_alpha_mask_write_mode        = 0; /* E_ALPHA_MASK_WRITE_MODE_KEEP*/
  device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_stencil_test_failed_op       = E_DHD_STENCIL_OP_KEEP;
  device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_stencil_depth_test_passed_op = E_DHD_STENCIL_OP_KEEP;
  device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_stencil_depth_test_failed_op = E_DHD_STENCIL_OP_KEEP;        
  device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_stencil_func = E_DHD_PASS_ALWAYS; 
  device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_depth_write = 0;
  device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_depth_func = E_DHD_PASS_ALWAYS; 
  device->m_shadow_state.m_named.m_zsa_ctrl.m_fields.m_zsc_read_enable  = 0;
  device->m_pending_mask.m_named.m_zsa_ctrl  = 1;
  device->m_pending_mask.m_named.m_zsu_ctrl1 = 1;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Specify zsabuffer address. The address must be in GPU accessible memory and is usually obtained by a */
/* call to dhd_get_buffer_gpuptr. Memory layout and pixel organization are specified separately using */
/* dhd_set_zsabuffer_format and dhd_set_framebuffer_size (sizes for framebuffer and zsa buffer must be equal).*/
/**/
/* The supplied address must always be aligned to the memory bus width of the GPU. Older HW revisions even */
/* require that the address is aligned to framebuffer cachelines. In case the address does not fulfill*/
/* the required alignment, it will not be set and an E_DHD_ERROR_INVALID_PTR is generated instead.*/
/* Whether or not the present device requires cacheline aligned framebuffer addresses can be queried using*/
/* dhd_get_bufferalignment_required.*/
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/*  a_address : Address of the top left pixel inside zsabuffer*/
/**/
/* Returns:*/
/*  E_DHD_OK if the address has been accepted or*/
/*  E_DHD_ERROR_INVALID_PTR if the address was not aligned correctly.*/
/**/
/* See Also:*/
/*  dhd_set_zsabuffer_format, dhd_set_framebuffer_size, dhd_get_bufferalignment_required*/
DHD_API_INLINE dhd_enum_t dhd_set_zsabuffer_address(dhd_handle_t a_device, dhd_gpu_ptr_t a_address) { /* PRQA S 3673 *//* $Misra: #NECESSARY_FOR_API $*/
  dhd_device_data_t *device = dhd_get_device( a_device );
  dhd_size_t address_bits = (dhd_size_t) a_address;  
  dhd_size_t zsc_line_length = (dhd_size_t)(device->m_description.m_bus_width * DHD_PARAM_ZSACACHE_LINE_LENGTH);
  
  if ((device->m_avoid_burstsplit == 1) && 
      ((address_bits & (dhd_size_t)(zsc_line_length - 1)) != 0)) {
    return E_DHD_ERROR_INVALID_PTR;
  }

  /* even in case no alignment to bursts is required, we need to align to the bus width*/
  if ((address_bits & (dhd_size_t)(device->m_description.m_bus_width - 1)) != 0) {
    return E_DHD_ERROR_INVALID_PTR;
  }
  
  device->m_shadow_state.m_named.m_zsc_start_address.m_value = (dhd_uint32_t) (dhd_size_t) a_address;
  device->m_pending_mask.m_named.m_zsc_start_address = 1;

  return E_DHD_OK;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Get zsabuffer address. GPU side address of current zsa buffer is returned as set by*/
/* dhd_set_zsabuffer_address. */
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/**/
/* Returns:*/
/*  Address of the top left pixel inside zsabuffer (address can not be assumed to be CPU accessible)*/
/**/
/* See Also:*/
/*  dhd_set_zsabuffer_address*/
DHD_API_INLINE dhd_gpu_ptr_t dhd_get_zsabuffer_address(dhd_handle_t a_device) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  return (dhd_gpu_ptr_t) (dhd_size_t) device->m_shadow_state.m_named.m_zsc_start_address.m_value;  
}

/*----------------------------------------------------------------------------------------------------------*/
/* Specify zsabuffer format. The zsabuffer format describes pixel organization (bits per channel and */
/* channel order) as well as channel counts (depth and stencil, depth only, alpha only, depth and alpha..)*/
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/*  a_format : Pixel format (see dhd_depth_org_t)*/
/**/
/* See Also:*/
/*  dhd_set_zsabuffer_address*/
DHD_API_INLINE void dhd_set_zsabuffer_format(dhd_handle_t a_device, dhd_depth_org_t a_format) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  device->m_shadow_state.m_named.m_zsa_ctrl.m_fields.m_zsa_buffer_org = a_format & 0xf;
  device->m_pending_mask.m_named.m_zsa_ctrl = 1;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Get zsabuffer format. Returns the current zsabuffer pixel organization (as set by */
/* dhd_set_zsabuffer_format) */
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/**/
/* Returns:*/
/*  Pixel format (see dhd_depth_org_t)*/
/**/
/* See Also:*/
/*  dhd_set_zsabuffer_format*/
DHD_API_INLINE dhd_depth_org_t dhd_get_zsabuffer_format(dhd_handle_t a_device) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  return (dhd_depth_org_t)(device->m_shadow_state.m_named.m_zsa_ctrl.m_fields.m_zsa_buffer_org | DHD_DEPTH_ORG_FLAG);
}

/*----------------------------------------------------------------------------------------------------------*/
/* Enable ZSA reads. Zsabuffer reads have to be enabled when either depth, stencil or global alpha states*/
/* depend on it's value or when only a part of the buffer is to be modified (e.g. stencil only)*/
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/*  a_enable : Boolean flag. 0 to disable zsa reads.*/
DHD_API_INLINE void dhd_set_zsa_read(dhd_handle_t a_device, dhd_bool_t a_enable) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  device->m_shadow_state.m_named.m_zsa_ctrl.m_fields.m_zsc_read_enable = a_enable;  
  device->m_pending_mask.m_named.m_zsa_ctrl  = 1;  
}

/*----------------------------------------------------------------------------------------------------------*/
/* Get ZSA read enable status. Retrieve if zsabuffer reads are currently enabled (see dhd_set_zsa_read*/
/* for details). */
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/**/
/* Returns:*/
/*  DHD_TRUE if zsa reads are enabled, DHD_FALSE otherwise.*/
/**/
/* See Also:*/
/*  dhd_set_zsa_read*/
DHD_API_INLINE dhd_bool_t dhd_get_zsa_read(dhd_handle_t a_device) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  return device->m_shadow_state.m_named.m_zsa_ctrl.m_fields.m_zsc_read_enable;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Set the stencil reference value and masks. All tests are performed against the reference and using the*/
/* masks. Before the stencil test the hardware will perform an AND operation with the stencil mask. Only bits */
/* set in the writemask are updated in the stencil buffer (this applies also to the clear operation)*/
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/*  a_mask : Mask to be applied before the stencil test*/
/*  a_ref : Stencil reference value to test against*/
/*  a_writemask : Mask to define what parts of the stencil buffer are to be updated*/
/**/
/* See Also:*/
/*  dhd_set_stencil_test*/
DHD_API_INLINE void dhd_set_stencil_ref(dhd_handle_t a_device, dhd_uint8_t a_mask, dhd_uint8_t a_ref, dhd_uint8_t a_writemask) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  device->m_shadow_state.m_named.m_zsu_ctrl2.m_fields.m_zsu_stencil_mask        = a_mask;
  device->m_shadow_state.m_named.m_zsu_ctrl2.m_fields.m_zsu_stencil_ref         = a_ref;
  device->m_shadow_state.m_named.m_zsu_ctrl2.m_fields.m_zsu_stencil_write_mask  = a_writemask;
  device->m_pending_mask.m_named.m_zsu_ctrl2 = 1;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Get stencil reference settings. Retrieve the currently active stencil reference value and masks (see */
/* dhd_set_stencil_ref for details). Passing a null pointer indicates that a specific information is not requested. */
/* The most important information (stencil reference value) is returned directly.*/
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/*  a_mask : Pointer to dhd_uint8_t that will receive the stencil mask (can be null)*/
/*  a_writemask : Pointer to dhd_uint8_t that will receive the stencil writemask (can be null)*/
/**/
/* Returns:*/
/*  Stencil reference value*/
/* */
/* See Also:*/
/*  dhd_set_stencil_ref*/
DHD_API_INLINE dhd_uint8_t dhd_get_stencil_ref(dhd_handle_t a_device, dhd_uint8_t *a_mask, dhd_uint8_t *a_writemask) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  if (a_mask != 0) {
    *a_mask = (dhd_uint8_t) device->m_shadow_state.m_named.m_zsu_ctrl2.m_fields.m_zsu_stencil_mask;
  }
  if (a_writemask != 0) {
    *a_writemask = (dhd_uint8_t) device->m_shadow_state.m_named.m_zsu_ctrl2.m_fields.m_zsu_stencil_write_mask;
  }

  return device->m_shadow_state.m_named.m_zsu_ctrl2.m_fields.m_zsu_stencil_ref;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Select stencil test. Fragments can be discarded based on the result of a stencil test operation. */
/* The stencil buffer is updated according to the a_fail operation if the stencil test failed. If it has*/
/* passed the update depends on wether the depth test has also passed. In this case a_zpass operation is*/
/* performed, otherwise a_zfail.*/
/**/
/* Stencil test E_DHD_PASS_LESS would pass if Reference value is less than Buffer value.*/
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/*  a_func : Stencil test function (see dhd_compare_op_t)*/
/*  a_fail : Stencil operation if stencil test failed, independent of depth test (see dhd_stencil_op_t)*/
/*  a_zpass : Stencil operation if stencil test passed and depth test passed (see dhd_stencil_op_t)*/
/*  a_zfail : Stencil operation if stencil test passed and depth test failed (see dhd_stencil_op_t)*/
/**/
/* See Also:*/
/*  dhd_set_stencil_ref, dhd_set_zsa_read*/
DHD_API_INLINE void dhd_set_stencil_test(dhd_handle_t a_device, dhd_compare_op_t a_func, dhd_stencil_op_t a_fail, 
                                         dhd_stencil_op_t a_zpass, dhd_stencil_op_t a_zfail) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_stencil_func                  = a_func;
  device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_stencil_test_failed_op        = a_fail;
  device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_stencil_depth_test_passed_op  = a_zpass;
  device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_stencil_depth_test_failed_op  = a_zfail;
  device->m_pending_mask.m_named.m_zsu_ctrl1 = 1;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Get stencil test settings. Retrieve the currently active stencil test operations and function (see */
/* dhd_set_stencil_test for details). Passing a null pointer indicates that a specific information is not requested. */
/* The most important information (stencil test function) is returned directly.*/
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/*  a_fail : Pointer to dhd_stencil_op_t that will receive the operation when test failed (can be null)*/
/*  a_zpass : Pointer to dhd_stencil_op_t that will receive the operation when test passed and depth test passed (can be null)*/
/*  a_zfail : Pointer to dhd_stencil_op_t that will receive the operation when test passed but depth test failed (can be null)*/
/**/
/* Returns:*/
/*  Stencil test function (see dhd_compare_op_t)*/
/* */
/* See Also:*/
/*  dhd_set_stencil_test*/
DHD_API_INLINE dhd_compare_op_t dhd_get_stencil_test(dhd_handle_t a_device, dhd_stencil_op_t *a_fail, dhd_stencil_op_t *a_zpass, dhd_stencil_op_t *a_zfail) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  if (a_fail != 0) {
    *a_fail = (dhd_stencil_op_t) device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_stencil_test_failed_op;
  }
  if (a_zpass != 0) {
    *a_zpass = (dhd_stencil_op_t) device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_stencil_depth_test_passed_op;
  }
  if (a_zfail != 0) {
    *a_zfail = (dhd_stencil_op_t) device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_stencil_depth_test_failed_op;
  }

  return (dhd_compare_op_t) device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_stencil_func;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Enable depth interpolation. When enabled the depth at each fragment is computed and depth testing */
/* activated.*/
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/*  a_enable : Boolean flag. 0 to disable z interpolation.*/
/**/
/* See Also:*/
/*  dhd_set_depth_test, dhd_set_depth_write*/
DHD_API_INLINE void dhd_set_depth_enable(dhd_handle_t a_device, dhd_bool_t a_enable) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  device->m_shadow_state.m_named.m_tex_global.m_fields.m_tex_depth_enable = a_enable;
  device->m_pending_mask.m_named.m_tex_global = 1;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Get depth interpolation status. Retrieve if depth interpolation is currently enabled (see dhd_set_depth_enable*/
/* for details). */
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/**/
/* Returns:*/
/*  DHD_TRUE if depth interpolation and testing is enabled, DHD_FALSE otherwise.*/
/**/
/* See Also:*/
/*  dhd_set_depth_enable*/
DHD_API_INLINE dhd_bool_t dhd_get_depth_enable(dhd_handle_t a_device) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  return device->m_shadow_state.m_named.m_tex_global.m_fields.m_tex_depth_enable;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Enable Z writes. This function can be used to disable depth updates in the Zsabuffer. Fragments will still*/
/* be depth tested even when depth writes are disabled.*/
/* Note that this will not disable writes to the stencil or global alpha parts of the current Zsabuffer. */
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/*  a_enable : Boolean flag. 0 to disable z writes.*/
/**/
/* See Also:*/
/*  dhd_set_depth_test, dhd_set_depth_enable*/
DHD_API_INLINE void dhd_set_depth_write(dhd_handle_t a_device, dhd_bool_t a_enable) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_depth_write  = a_enable;  
  device->m_pending_mask.m_named.m_zsu_ctrl1  = 1;  
}

/*----------------------------------------------------------------------------------------------------------*/
/* Get depth write status. Retrieve if Z writing is currently enabled (see dhd_set_depth_write*/
/* for details). */
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/**/
/* Returns:*/
/*  DHD_TRUE if depth writing to zsa buffer is enabled, DHD_FALSE otherwise.*/
/**/
/* See Also:*/
/*  dhd_set_depth_write*/
DHD_API_INLINE dhd_bool_t dhd_get_depth_write(dhd_handle_t a_device) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  return device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_depth_write;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Select depth test. Fragments can be discarded based on their depth value.  If the current fragment does not*/
/* pass the specified test compared to the buffered depth value it will be discarded.*/
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/*  a_compare : Depth compare function (see dhd_compare_op_t)*/
/**/
/* See Also:*/
/*  dhd_set_depth_write, dhd_set_depth_enable, dhd_set_zsa_read*/
DHD_API_INLINE void dhd_set_depth_test(dhd_handle_t a_device, dhd_compare_op_t a_func) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_depth_func = a_func;
  device->m_pending_mask.m_named.m_zsu_ctrl1 = 1;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Get depth test function. Retrieve currently active depth compare function (see dhd_set_depth_test*/
/* for details). */
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/**/
/* Returns:*/
/*  Depth compare function (see dhd_compare_op_t).*/
/**/
/* See Also:*/
/*  dhd_set_depth_test*/
DHD_API_INLINE dhd_compare_op_t dhd_get_depth_test(dhd_handle_t a_device) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  return (dhd_compare_op_t) device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_depth_func;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Choose global alpha mode. If present the global alpha information from the Zsabuffer can be combined*/
/* with the fragment alpha or updated based on the fragment alpha.*/
/* E_DHD_ALPHAMASK_MULTIPLY_CLEAR and E_DHD_ALPHAMASK_REPLACE_CLEAR allow clearing the global alpha buffer.*/
/* The clearing is performed after the global alpha has been read and used for modification of fragment alphas.*/
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/*  a_mode : Global alpha combine mode (see dhd_alphamask_mode_t)*/
/**/
DHD_API_INLINE void dhd_set_alphamask_mode(dhd_handle_t a_device, dhd_alphamask_mode_t a_mode) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_alpha_mask_enable     = a_mode & 1;
  device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_alpha_mask_mode       = (a_mode >> 1) & 1;
  device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_alpha_mask_write_mode = a_mode >> 2;  
  device->m_pending_mask.m_named.m_zsu_ctrl1 = 1;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Get global alpha mode. Retrieve currently active alphamask processing mode (see dhd_set_alphamask_mode*/
/* for details). */
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/**/
/* Returns:*/
/*  Global alpha combine mode (see dhd_alphamask_mode_t)*/
/**/
/* See Also:*/
/*  dhd_set_alphamask_mode*/
DHD_API_INLINE dhd_alphamask_mode_t dhd_get_alphamask_mode(dhd_handle_t a_device) {
  dhd_device_data_t *device = dhd_get_device( a_device );
  dhd_uint32_t alphamode;

  alphamode = device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_alpha_mask_enable | 
              (device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_alpha_mask_mode << 1) |
              (device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_alpha_mask_write_mode << 2);

  return (dhd_alphamask_mode_t) alphamode;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Enable discarding invisible fragments. Analogous to depth and stencil tests fragments can also be*/
/* rejected based on their coverage (after global alpha has been applied). Discarding is usually wanted*/
/* unless zero coverage fragments are intentionally converted back to visible pixels later (e.g. using*/
/* special colorization setups).*/
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/*  a_enable : Boolean flag. 0 to disable discarding of noncovered fragments.*/
/**/
DHD_API_INLINE void dhd_set_alphamask_test(dhd_handle_t a_device, dhd_bool_t a_enable) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  device->m_shadow_state.m_named.m_zsa_ctrl.m_fields.m_zsc_discard_noncovered = a_enable;
  device->m_pending_mask.m_named.m_zsa_ctrl = 1;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Get alphamask test state. Retrieve currently active alphamask test state (see dhd_set_alphamask_test*/
/* for details). */
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/**/
/* Returns:*/
/*  DHD_TRUE if rejection of zero coverage fragments is enabled, DHD_FALSE otherwise.*/
/**/
/* See Also:*/
/*  dhd_set_depth_test*/
DHD_API_INLINE dhd_bool_t dhd_get_alphamask_test(dhd_handle_t a_device) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  return device->m_shadow_state.m_named.m_zsa_ctrl.m_fields.m_zsc_discard_noncovered;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Set extra parameters for alphamask mode E_DHD_ALPHAMASK_ANTIALIAS.*/
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/*  a_bias : Stencil buffer value that represents winding order of 0.*/
/*  a_invert_on_opaque : Alphabuffer content is inverted for fully covered fragments.*/
/**/
DHD_API_INLINE void dhd_set_alphamask_aa(dhd_handle_t a_device, dhd_uint8_t a_bias, dhd_bool_t a_invert_on_opaque) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_antialias_invert_on_opaque = a_invert_on_opaque;
  device->m_shadow_state.m_named.m_zsu_ctrl2.m_fields.m_zsu_stencil_bias = a_bias;
  device->m_pending_mask.m_named.m_zsu_ctrl1 = 1;
  device->m_pending_mask.m_named.m_zsu_ctrl2 = 1;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Get extra parameters for E_DHD_ALPHAMASK_ANTIALIAS mode. Retrieve the currently active settings for */
/* alphamask mode E_DHD_ALPHAMASK_ANTIALIAS (see dhd_set_alphamask_aa for details). */
/* Passing a null pointer indicates that a specific information is not requested. */
/**/
/* Parameters:*/
/*  a_device : Driver context handle (as returned by dhd_init)*/
/*  a_bias : Pointer to dhd_uint8_t that will receive the winding order 0 representation (can be null)*/
/*  a_invert_on_opaque : Pointer to dhd_bool_t that will be set to DHD_TRUE if alpha inversion is enabled (can be null)*/
/* */
/* See Also:*/
/*  dhd_set_alphamask_aa*/
DHD_API_INLINE void dhd_get_alphamask_aa(dhd_handle_t a_device, dhd_uint8_t *a_bias, dhd_bool_t *a_invert_on_opaque) {
  dhd_device_data_t *device = dhd_get_device( a_device );

  if (a_bias != 0) {
    *a_bias = (dhd_uint8_t) device->m_shadow_state.m_named.m_zsu_ctrl2.m_fields.m_zsu_stencil_bias;
  }
  if (a_invert_on_opaque != 0) {
    *a_invert_on_opaque = device->m_shadow_state.m_named.m_zsu_ctrl1.m_fields.m_zsu_antialias_invert_on_opaque;
  }
}

/*----------------------------------------------------------------------------------------------------------*/

#endif /*DAVEHD_DRIVER_ZSU_INL_H_INCLUDED*/