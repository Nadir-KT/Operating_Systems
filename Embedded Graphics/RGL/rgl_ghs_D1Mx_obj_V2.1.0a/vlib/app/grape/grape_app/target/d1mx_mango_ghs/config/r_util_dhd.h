/***********************************************************************************************
PROJECT : VLIB
FILE    : $Id: r_util_dhd.h 13725 2017-06-06 15:23:21Z florian.zimmermann $
============================================================================ 
DESCRIPTION
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
#ifndef R_UTIL_DHD_H_
#define R_UTIL_DHD_H_


#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************
  Section: Defines
*/
#define DHD_ALIGN(ADDR,BLOCK)      ( (ADDR+(BLOCK-1)) & ~(BLOCK-1) )

#ifndef DHD_USER_INSTANCES
  #define DHD_USER_INSTANCES (4u)
#endif

/* Jobsize: Number of bytes in one job buffer */
#ifndef DHD_JOBSIZE
  #define DHD_JOBSIZE      (512u)
#endif

/* Jobcount: Recommended number of jobs in flight per client */
#ifndef DHD_JOBCOUNT
  #define DHD_JOBCOUNT       (8u)
#endif

/* Ringsize: Number of bytes in ringbuffer, needs to be a power of two */
#ifndef DHD_RINGSIZE
  #define DHD_RINGSIZE    (1024u)
#endif

/* Use performance counters: Set to 1 to reserve memory for performance counters */
#ifndef DHD_USE_PC
  #define DHD_USE_PC      (0u)
#endif

/* one DHD context with a config with 2 tex units, 3 color unit passes and 4 const colors */
#define DHD_STATESTORAGE (480u) 

/* CAUTION: you have to adapt this when using dhd_alloc* manually or when using 
 * DHD performance counter or when using DHD command list feature! 
 *
 * Example: Each of the 4 available performance counters requires allocation of 4 bytes.
            Allocation is aligned to 128 bytes, thus for four PC's 512 additional bytes are required.
 */
#define DHD_RINGSIZE_ALLOC      DHD_ALIGN(DHD_RINGSIZE+4,128) 
#define DHD_STATESTORAGE_ALLOC  DHD_ALIGN(DHD_STATESTORAGE,128)
#define DHD_PC_ALLOC            DHD_ALIGN(DHD_USE_PC*4*128,128)
#define DHD_MEMORY_SIZE         (DHD_RINGSIZE_ALLOC+DHD_USER_INSTANCES*(DHD_STATESTORAGE_ALLOC+DHD_JOBCOUNT*DHD_JOBSIZE)+DHD_PC_ALLOC)
/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: SetupMemManager

  This function sets up the CDI memory management.
  It shows how the memory is initialised.

  Parameter:
    void

  Returns:
    void
*/
dhd_uint32_t R_UTIL_DHD_Config(dhd_gpu_ptr_t Base, dhd_uint32_t VidSize, r_cdi_Heap_t *Heap);
void R_UTIL_DHD_Init(dhd_uint32_t Unit);
dhd_uint32_t R_UTIL_DHD_DeInit(dhd_uint32_t Unit);

#ifdef R_USE_FASTSOFTDAVE
void R_UTIL_DHD_FSDRegBuf(void* Buf, uint32_t Size);
#endif


#ifdef __cplusplus
}
#endif

#endif /* R_UTIL_DHD_H_ */
