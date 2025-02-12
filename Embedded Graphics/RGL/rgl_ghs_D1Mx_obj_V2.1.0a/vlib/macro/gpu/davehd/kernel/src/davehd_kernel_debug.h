/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : Kernel debug mode support functions
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 11641 $
 *  $Date: 2016-12-09 19:40:31 +0900 (金, 09 12 2016) $
 *  $LastChangedBy: giuseppe.gorgoglione $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/    

#ifndef DAVEHD_KERNEL_DEBUG_H_INCLUDED
#define DAVEHD_KERNEL_DEBUG_H_INCLUDED

#include "davehd_types.h"
#include "davehd_settings.h"
#include "davehd_kernel_os.h"

/*----------------------------------------------------------------------------------------------------------*/
typedef struct tagdhd_gpu_debug_message {  
  dhd_uint8_t                      m_seqid[4];              /* global seqid at the time the message occurred (stored as 4 bytes to avoid unaligned 32bit access)*/
  dhd_char_t                       m_message[1];            /* dynamic size 0 terminated string*/
} dhd_gpu_debug_message_t;

/*----------------------------------------------------------------------------------------------------------*/
typedef struct tagdhd_gpu_debug_data {
  dhd_gpu_debug_message_t         *m_message_buffer;        /* first message  */
  dhd_lock_t                       m_message_lock;          /* used to sync changes to message list*/
  dhd_uint32_t                     m_seqid;                 /* global sequence id */
  dhd_uint32_t                     m_writepos;              /* byte offset into message buffer for next message*/

} dhd_gpu_debug_data_t;

/*----------------------------------------------------------------------------------------------------------*/

#ifdef DHD_KERNEL_DEBUG

void dhd_gpu_debug_note(dhd_gpu_device_data_t *a_device, dhd_char_t *a_format, ... );              /* PRQA S 5069 *//* $Misra: functionality requires printf syntax $*/
void dhd_gpu_debug_init(dhd_gpu_device_data_t *a_device);
void dhd_gpu_debug_shutdown(dhd_gpu_device_data_t *a_device);
void dhd_gpu_debug_flush(dhd_gpu_device_data_t *a_device, dhd_bool_t a_lock);
void dhd_gpu_debug_nextnote(dhd_gpu_device_data_t *a_device, dhd_int32_t a_len);
void dhd_gpu_debug_internal_output(dhd_gpu_device_data_t *a_device, dhd_gpu_debug_message_t *a_message);

/* Disable MISRA checks for macro expansion */
#if defined (__ghs__)
#pragma ghs startnomisra
#endif /* __ghs__ */

#define DHD_GPU_ASSERT( device, condition ) do {\
  if (!(condition)){\
    dhd_gpu_debug_note((device), "DHD_GPU_ASSERT("  #condition  ") failed (in %s: %d)", __FILE__, __LINE__);\
    return E_DHD_ERROR_INTERNAL;\
  }\
} while(!(condition))
#if defined (__ghs__)
#pragma ghs endnomisra
#endif

#else
/* PRQA S 3450 11 *//* $Misra: Function is exculded when DHD_KERNEL_DEBUG is not set $*/ 
/* PRQA S 3480 ++ *//* $Misra: Function is exculded when DHD_KERNEL_DEBUG is not set $*/

#ifdef PRQA
#pragma PRQA_MESSAGES_OFF 3480 
#endif

DHD_INLINE void dhd_gpu_debug_note(dhd_gpu_device_data_t *a_device, dhd_char_t *a_format, ... );  /* PRQA S 5069 *//* $Misra: functionality requires printf syntax $*/
DHD_INLINE void dhd_gpu_debug_init(dhd_gpu_device_data_t *a_device);
DHD_INLINE void dhd_gpu_debug_shutdown(dhd_gpu_device_data_t *a_device);
DHD_INLINE void dhd_gpu_debug_flush(dhd_gpu_device_data_t *a_device, dhd_bool_t a_lock);
DHD_INLINE void dhd_gpu_debug_nextnote(dhd_gpu_device_data_t *a_device, dhd_int32_t a_len);

DHD_INLINE void dhd_gpu_debug_note(dhd_gpu_device_data_t *a_device, dhd_char_t *a_format, ... ) {}  /* PRQA S 5069 *//* $Misra: functionality requires printf syntax $*/
DHD_INLINE void dhd_gpu_debug_init(dhd_gpu_device_data_t *a_device) {}
DHD_INLINE void dhd_gpu_debug_shutdown(dhd_gpu_device_data_t *a_device) {}
DHD_INLINE void dhd_gpu_debug_flush(dhd_gpu_device_data_t *a_device, dhd_bool_t a_lock) {}
DHD_INLINE void dhd_gpu_debug_nextnote(dhd_gpu_device_data_t *a_device, dhd_int32_t a_len) {}

/* Disable MISRA checks for macro expansion */
#if defined (__ghs__)
#pragma ghs startnomisra
#endif /* __ghs__ */
#define DHD_GPU_ASSERT( device, condition )
#if defined (__ghs__)
#pragma ghs endnomisra
#endif



#endif

/*----------------------------------------------------------------------------------------------------------*/



#endif /*DAVEHD_KERNEL_DEBUG_H_INCLUDED*/
