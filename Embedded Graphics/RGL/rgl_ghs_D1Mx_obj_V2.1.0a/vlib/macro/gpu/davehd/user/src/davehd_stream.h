/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : Stream building and upload
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 4436 $
 *  $Date: 2014-12-02 22:27:26 +0900 (火, 02 12 2014) $
 *  $LastChangedBy: florian.zimmermann $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/    

#ifndef DAVEHD_STREAM_H_INCLUDED
#define DAVEHD_STREAM_H_INCLUDED

#include "davehd_types.h"
#include "davehd_stcopcodes.h"

/* PRQA S 3406 ++ *//* $Misra: #INLINE_FUNCTION_DEFINED_IN_HEADER $*/

/*----------------------------------------------------------------------------------------------------------*/
dhd_enum_t dhd_commit_stream(dhd_device_data_t *a_device);
dhd_enum_t dhd_flush_stream_shadow(const dhd_device_data_t *a_device, dhd_uint32_t a_size);
dhd_uint32_t dhd_reclaim_job_structs(dhd_device_data_t *a_device, dhd_gpu_job_t *a_freejobs);
dhd_enum_t dhd_release_job_memory(const dhd_device_data_t *a_device, dhd_gpu_job_t *a_job);
static dhd_uint32_t dhd_checksum_get_stream_padding_size(dhd_device_data_t *a_device);
static dhd_uint32_t dhd_checksum_get_padding_size(dhd_device_data_t *a_device);
DHD_INLINE dhd_uint32_t * dhd_stream_space(dhd_device_data_t *a_device, dhd_uint32_t a_words);
DHD_INLINE dhd_uint32_t * dhd_stream_insert(dhd_device_data_t *a_device, dhd_uint32_t *a_sptr, dhd_uint32_t a_cmd, const dhd_uint32_t *a_data, dhd_uint32_t a_data_size, dhd_bool_t a_checksum_commit);
static dhd_uint32_t * dhd_stream_insert_word(dhd_device_data_t *a_device, dhd_uint32_t *a_sptr, dhd_uint32_t a_cmd);

/*----------------------------------------------------------------------------------------------------------*/

static dhd_uint32_t dhd_checksum_get_stream_padding_size(dhd_device_data_t *a_device) {   /* PRQA S 3480 *//* $Misra: #INLINE_FUNCTION_DEFINED_IN_HEADER $*/ 
#ifdef DHD_JOB_CHECKSUM_SUPPORTED
  if (a_device->m_checksum_enabled) {
    return 6;
  }
#endif
  return 0;
}

static dhd_uint32_t dhd_checksum_get_padding_size(dhd_device_data_t *a_device) {   /* PRQA S 3480 *//* $Misra: #INLINE_FUNCTION_DEFINED_IN_HEADER $*/ 
#ifdef DHD_JOB_CHECKSUM_SUPPORTED
  if (a_device->m_checksum_enabled) {
    return 2;
  }
#endif
  return 0;
}

DHD_INLINE dhd_uint32_t * dhd_stream_space(dhd_device_data_t *a_device, dhd_uint32_t a_words) {   /* PRQA S 3480 *//* $Misra: #INLINE_FUNCTION_DEFINED_IN_HEADER $*/ 
  dhd_uint32_t *dst;
  /* additional space at the end of each stream in dwords (adapt dhd_gpu_tlist_upload when this is less than 1)*/
  dhd_uint32_t words_padded = a_words + 2 + dhd_checksum_get_stream_padding_size(a_device);

  if (a_device->m_free_words < words_padded) {
    /* not enough space (note that this is also triggered if we are working on nullstream)*/
    DHD_NO_COVERAGE_BEGIN
    /* $Coverage: Can not be covered. Purely defensive programming$*/
    if (dhd_commit_stream(a_device) != E_DHD_OK) {
      /* stream space exhausted or some internal error? this is truly bad*/
      DHD_PANIC(a_device, "no stream space or internal error");
    }
    /* make sure new job does fit size requirement */
    if ((words_padded * sizeof(dhd_uint32_t)) > a_device->m_active_job->m_memory.m_size) {
      DHD_PANIC(a_device, "new job too small for stream space request");
    }
    DHD_NO_COVERAGE_END
  }

  /* increment stream position and return writepos  */
  dst = a_device->m_write_pos;
  a_device->m_write_pos += a_words;
  a_device->m_free_words -= a_words;

  return dst;
}

DHD_INLINE dhd_uint32_t * dhd_stream_insert(dhd_device_data_t *a_device, dhd_uint32_t *a_sptr, dhd_uint32_t a_cmd, const dhd_uint32_t *a_data, dhd_uint32_t a_data_size, dhd_bool_t a_checksum_commit) {   /* PRQA S 3480 *//* $Misra: #INLINE_FUNCTION_DEFINED_IN_HEADER $*/ 
  dhd_uint32_t i;
#ifdef DHD_JOB_CHECKSUM_SUPPORTED
  if(a_device->m_checksum_enabled) {
    a_device->m_checksum += a_cmd;
    for (i = 0; i < a_data_size; ++i) {
      a_device->m_checksum += a_data[i];
    }
    if (a_checksum_commit) {
      dhd_stc_opcode_t op;
      op.m_fields.m_opcode         = DHD_STCOP_NOP;
      op.m_fields.m_data_following = 1;
      op.m_fields.m_data_count     = 1 - 1;
      op.m_fields.m_arguments      = 0;
      *a_sptr = op.m_value;
      a_sptr++;
      *a_sptr = DHD_TARGET_CHECKSUM - a_device->m_checksum - op.m_value;
      a_sptr++;
      a_device->m_checksum = DHD_TARGET_CHECKSUM;
    }
  }
#endif
  *a_sptr = a_cmd;
  a_sptr++;
  for (i = 0; i < a_data_size; ++i) {
    *a_sptr = a_data[i];
    a_sptr++;
  }
  return a_sptr;
}

static dhd_uint32_t * dhd_stream_insert_word(dhd_device_data_t *a_device, dhd_uint32_t *a_sptr, dhd_uint32_t a_cmd) {   /* PRQA S 3480 *//* $Misra: #INLINE_FUNCTION_DEFINED_IN_HEADER $*/ 
  return dhd_stream_insert(a_device, a_sptr, a_cmd, (const dhd_uint32_t *) 0, 0, DHD_FALSE);
}


/*----------------------------------------------------------------------------------------------------------*/

#endif /*DAVEHD_STREAM_H_INCLUDED*/
