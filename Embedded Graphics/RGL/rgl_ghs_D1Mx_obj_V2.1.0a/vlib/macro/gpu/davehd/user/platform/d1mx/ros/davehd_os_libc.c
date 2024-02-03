/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : Operatingsystem abstraction - LIBC implementation
 *  Platform: Any std C compilant system
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 7755 $
 *  $Date: 2016-02-19 18:23:16 +0900 (金, 19 2 2016) $
 *  $LastChangedBy: florian.zimmermann $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/    

#include "r_typedefs.h"
#include "r_cdi_api.h"
#include "davehd_kernel_os.h"
#include "davehd_os.h"
#include "davehd_os_libc.h"


#ifdef DHD_USER_USE_POOLING
/*----------------------------------------------------------------------------------------------------------*/
/**/
dhd_enum_t dhd_alloc_setup_pool(dhd_alloc_element_t a_element, dhd_alloc_pool_t *a_out) {
  /* initialize with invalid data*/
     
  uint32_t heap_addr = 0;
  a_out->m_memory = 0;
  a_out->m_size   = 0;
  a_out->m_align  = 0;
  /* create the requested pool*/
  switch (a_element) {
    case E_DHD_ALLOC_CLINE:
      a_out->m_size   = dhd_get_alloc_size(a_element, 1) * DHD_CSIZE( DHD_MAX_USER_SHADOW );

      heap_addr = R_CDI_HeapAlloc( a_out->m_size + DHD_CACHELINE_SIZE + sizeof( dhd_uint32_t ), CPU );

      a_out->m_memory = (void*) (( DHD_CACHELINE_SIZE - 1 +  sizeof( dhd_uint32_t )  + heap_addr )  & ~(DHD_CACHELINE_SIZE - 1));

      ((dhd_uint32_t*)(a_out->m_memory))[-1] = heap_addr;

      a_out->m_align  = DHD_CACHELINE_SIZE;
      break;

    case E_DHD_ALLOC_BUFFER:
      a_out->m_size   = dhd_get_alloc_size(a_element, 1) * DHD_MAX_USER_BUFFERS;
      a_out->m_memory = (void*)R_CDI_HeapAlloc( a_out->m_size, CPU );
      a_out->m_align  = sizeof( dhd_uint32_t );
      break;

    case E_DHD_ALLOC_GPUJOB:
      a_out->m_size   = dhd_get_alloc_size(a_element, 1) * DHD_MAX_USER_JOBS;
      a_out->m_memory = (void*)R_CDI_HeapAlloc( a_out->m_size, CPU );
      a_out->m_align  = sizeof( dhd_uint32_t );
      break;

      default:
        /* Nothing to do here */
        break;

  }
  /* handle error*/
  if (a_out->m_memory == 0) {
    return E_DHD_ERROR_NO_MEMORY;
  } else {
    return E_DHD_OK;
  }
}

/*----------------------------------------------------------------------------------------------------------*/
/**/
dhd_enum_t dhd_alloc_shutdown_pool(dhd_alloc_element_t a_element, dhd_alloc_pool_t *a_out) {

  uint32_t heap_addr = 0;

  switch (a_element) {
    case E_DHD_ALLOC_CLINE:
       heap_addr =((dhd_uint32_t*)(a_out->m_memory))[-1];
       R_CDI_HeapFree( heap_addr, CPU );
      break;

    case E_DHD_ALLOC_BUFFER:
      R_CDI_HeapFree( (uint32_t)a_out->m_memory, CPU );
      break;

    case E_DHD_ALLOC_GPUJOB:
       R_CDI_HeapFree( (uint32_t)a_out->m_memory, CPU );
       break;

    default:
        /* Nothing to do here */
        break;
  }
  return E_DHD_OK;
}

#else
static r_cdi_Heap_t *s_CPUHeap;
/*----------------------------------------------------------------------------------------------------------*/
/**/
void dhd_set_cpu_heap(r_cdi_Heap_t *a_heap) {
    s_CPUHeap = a_heap;
}
/*----------------------------------------------------------------------------------------------------------*/
/**/
void *dhd_alloc(dhd_alloc_element_t a_element, dhd_uint32_t a_count, dhd_uint32_t a_alignment) {
  (void) a_alignment;
  return (R_CDI_Alloc(s_CPUHeap, dhd_get_alloc_size(a_element, a_count)));
}

/*----------------------------------------------------------------------------------------------------------*/
/**/
void dhd_free(void *a_memory) {
  R_CDI_Free((uint32_t)a_memory, s_CPUHeap);
}

#endif /* DHD_USER_USE_POOLING */




#ifdef DHD_KERNEL_USE_POOLING
/*----------------------------------------------------------------------------------------------------------*/
/**/
dhd_enum_t dhd_kalloc_setup_pool(dhd_kalloc_element_t a_element, dhd_kalloc_pool_t *a_out) {
  /* initialize with invalid data*/
  a_out->m_memory = 0;
  a_out->m_size   = 0;
  a_out->m_align  = 0;
  /* create the requested pool*/
  switch (a_element) {
    case E_DHD_KALLOC_STATESTORE:
      a_out->m_size   = dhd_get_kalloc_size(a_element) * DHD_MAX_KERNEL_CLIENTS;
      a_out->m_memory = (void*)R_CDI_HeapAlloc( a_out->m_size, CPU );
      a_out->m_align  = sizeof( dhd_uint32_t );
      break;

    case E_DHD_KALLOC_CONNECTION:
      a_out->m_size   = dhd_get_kalloc_size(a_element) * DHD_MAX_KERNEL_CLIENTS;
      a_out->m_memory = (void*)R_CDI_HeapAlloc( a_out->m_size, CPU );
      a_out->m_align  = sizeof( dhd_uint32_t );
      break;

    case E_DHD_KALLOC_VMEMBLOCK:
      a_out->m_size   = dhd_get_kalloc_size(a_element) * DHD_MAX_VIDMEM_CHUNKS;
      a_out->m_memory = (void*)R_CDI_HeapAlloc( a_out->m_size, VIDEO );
      a_out->m_align  = sizeof( dhd_uint32_t );
      break;

    case E_DHD_KALLOC_VMEMHEAP:
      a_out->m_size   = dhd_get_kalloc_size(a_element) * DHD_MAX_GPU_COUNT;
      a_out->m_memory = (void*)R_CDI_HeapAlloc( a_out->m_size, VIDEO );
      a_out->m_align  = sizeof( dhd_uint32_t );
      break;
  }
  /* handle error*/
  if (a_out->m_memory == 0) {
    return E_DHD_ERROR_NO_MEMORY;
  } else {
    return E_DHD_OK;
  }
}

/*----------------------------------------------------------------------------------------------------------*/
/**/
dhd_enum_t dhd_kalloc_shutdown_pool(dhd_kalloc_element_t a_element, dhd_kalloc_pool_t *a_out) {
  R_CDI_HeapFree( (uint32_t)a_out->m_memory, CPU );
  return E_DHD_OK;
}



#else

/*----------------------------------------------------------------------------------------------------------*/
/* Allocate kernel CPU memory. Kernel CPU memory can only be allocated for dedicated 'elements'. It is not*/
/* possible to specify an arbitary size.*/
/**/
/* Parameters:*/
/*  a_element - Element to allocate memory for (see dhd_kalloc_element_t)*/
/*  a_alignment - Required alignment in bytes*/
/**/
/* Returns:*/
/*  address of the element or 0 in case of an error*/
/**/
/* See Also: dhd_kfree*/
void *dhd_kalloc(dhd_kalloc_element_t a_element, dhd_uint32_t a_alignment) {
  (void) a_alignment;
  return (R_CDI_Alloc(s_CPUHeap, dhd_get_kalloc_size(a_element)));
}

/*----------------------------------------------------------------------------------------------------------*/
/* Free kernel CPU memory. Memory allocated by dhd_kalloc must be released using this function.*/
/**/
/* Parameters:*/
/*  a_memory - Addressr (as returned by dhd_kalloc)*/
/**/
/* See Also: dhd_kalloc*/
void dhd_kfree(void *a_memory) {
  R_CDI_Free((uint32_t)a_memory, s_CPUHeap);
}

#endif /* DHD_KERNEL_USE_POOLING */

#ifdef DHD_PROFILE_MEM_USAGE
/*----------------------------------------------------------------------------------------------------------*/
/* Debug function: Dump memory information */
/**/
void dhd_dump_mem_heap_info(void) {
  dhd_debug_out("DHD_PROFILE_MEM_USAGE: dhd_dump_mem_heap_info not implemented for d1x\n");
}

/*----------------------------------------------------------------------------------------------------------*/
/* Debug function: Reset internal memory allocation information*/
/**/
void dhd_clear_mem_heap_info(void) {
  dhd_debug_out("DHD_PROFILE_MEM_USAGE: dhd_clear_mem_heap_info not implemented for d1x\n");
}

#endif /* DHD_PROFILE_MEM_USAGE*/
