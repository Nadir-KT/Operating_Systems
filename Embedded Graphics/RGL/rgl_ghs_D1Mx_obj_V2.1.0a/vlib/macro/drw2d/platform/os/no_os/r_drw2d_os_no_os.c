/*
****************************************************************************
PROJECT : DRW2D
FILE    : r_drw2d_os_no_os.c
============================================================================ 
DESCRIPTION
DRW2D NO-OS abstraction layer
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2013 - 2021
                                  by
                       Renesas Electronics (Europe) GmbH.
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================

DISCLAIMER

LICENSEE has read, understood and accepted the terms and conditions defined in
the license agreement, especially the usage rights. In any case, it is
LICENSEE's responsibility to make sure that any user of the software complies
with the terms and conditions of the signed license agreement.

****************************************************************************
*/

#include <r_typedefs.h>
#include <r_drw2d_os.h>

#ifdef R_DRW2D_SYS_DHD
#include <r_cdi_api.h>
#include <r_sys_davehd.h>
#endif

/***********************************************************
  Section: Local Functions
*/


/***********************************************************
  Section: Thread functions
*/



/***********************************************************
  Function: R_DRW2D_OS_Thread_Create
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Thread_Create(r_drw2d_OS_ThreadInfo_t *ThreadInfo) 
{
    return R_DRW2D_OS_ERR_OK;
}



/***********************************************************
  Function: R_DRW2D_OS_Thread_Wait
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Thread_Wait(r_drw2d_OS_ThreadInfo_t *ThreadInfo,
                                          uint32_t              Timeout
                                          )
{
    return R_DRW2D_OS_ERR_OK;
}



/***********************************************************
  Function: R_DRW2D_OS_Thread_GetId
*/
r_drw2d_OS_ThreadId_t R_DRW2D_OS_Thread_GetId(void)
{
   
    return R_DRW2D_OS_ERR_OK;
}



/***********************************************************
  Function: R_DRW2D_OS_Thread_Destroy
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Thread_Destroy(r_drw2d_OS_ThreadInfo_t *ThreadInfo)
{
    return R_DRW2D_OS_ERR_OK;
}


/***********************************************************
  Section: Mutex functions
*/



/***********************************************************
  Function: R_DRW2D_OS_Mutex_Create
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Mutex_Create(r_drw2d_OS_Mutex_t *Mutex)
{
    return R_DRW2D_OS_ERR_OK;
}



/***********************************************************
  Function: R_DRW2D_OS_Mutex_Destroy
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Mutex_Destroy(r_drw2d_OS_Mutex_t *Mutex)
{
    return R_DRW2D_OS_ERR_OK;
}



/***********************************************************
  Function: R_DRW2D_OS_Mutex_Lock
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Mutex_Lock(r_drw2d_OS_Mutex_t *Mutex)
{
    return R_DRW2D_OS_ERR_OK;
}



/***********************************************************
  Function: R_DRW2D_OS_Mutex_Unlock
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Mutex_Unlock(r_drw2d_OS_Mutex_t *Mutex)
{
    return R_DRW2D_OS_ERR_OK;
}





/***********************************************************
  Section: Semaphore functions
*/



/***********************************************************
  Function: R_DRW2D_OS_Semaphore_Create
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Semaphore_Create(r_drw2d_OS_Semaphore_t *Sem, 
                                               uint32_t                InitialValue
                                               )
{
    return R_DRW2D_OS_ERR_OK;
}



/***********************************************************
  Function: R_DRW2D_OS_Semaphore_Destroy
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Semaphore_Destroy(r_drw2d_OS_Semaphore_t *Sem)
{
    return R_DRW2D_OS_ERR_OK;
}



/***********************************************************
  Function: R_DRW2D_OS_Semaphore_Wait
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Semaphore_Wait(r_drw2d_OS_Semaphore_t *Sem)
{
    return R_DRW2D_OS_ERR_OK;
}



/***********************************************************
  Function: R_DRW2D_OS_Semaphore_Post
*/
r_drw2d_OS_Error_t R_DRW2D_OS_Semaphore_Post(r_drw2d_OS_Semaphore_t *Sem)
{
    return R_DRW2D_OS_ERR_OK;
}





/***********************************************************
  Section: Memory utility functions
*/



/***********************************************************
  Function: R_DRW2D_OS_Mem_Alloc
*/
void *R_DRW2D_OS_Mem_Alloc(uint32_t NumBytes, uint32_t Alignment, r_drw2d_OS_Mem_t Type)
{
    void         *ret = R_NULL;
    
#ifdef R_DRW2D_SYS_DHD
    if(NumBytes > 0)
    {
        r_cdi_HpType_t cdi_HpType = (Type == R_DRW2D_OS_MEM_CPU) ? CPU : VIDEO;
        if (0 == Alignment)
        {
            Alignment = 1;
        }
        
        /* Check power of 2  */
        if(0 == (Alignment & (Alignment - 1)))
        {
            uint8_t      *actAddr = R_NULL;
            r_cdi_Heap_t *cpu_heap;

            cpu_heap = R_SYS_DHD_GetCpuHeap();
            if (R_NULL != cpu_heap)
            {
                actAddr = (uint8_t*)R_CDI_Alloc(cpu_heap, NumBytes + Alignment + sizeof(r_cdi_HpType_t));
            }
            
            if(R_NULL != actAddr)
            {
                *(r_cdi_HpType_t*)actAddr = cdi_HpType;
                actAddr += sizeof(r_cdi_HpType_t);
                
                if(Alignment > 1)
                {
                    uint32_t actAddrOff = (((uint32_t)actAddr) & (Alignment - 1));
                    uint32_t actAddrSkip = (Alignment - actAddrOff);
                    actAddr = (actAddr + actAddrSkip);
                    actAddr[-1] = actAddrSkip;
                    ret = actAddr;
                }
                else
                {
                    *actAddr = 1;
                    ret = (actAddr + 1);
                }
            }
        }
    }
#endif

    return ret;
}

/***********************************************************
  Function: R_DRW2D_OS_Mem_Free
*/
void R_DRW2D_OS_Mem_Free(void *Addr)
{
#ifdef R_DRW2D_SYS_DHD
    r_cdi_Heap_t *cpu_heap;
    
    if(R_NULL != Addr)
    {
        uint8_t *actAddr = Addr;
        actAddr = (actAddr - actAddr[-1]);
        actAddr -= sizeof(r_cdi_HpType_t);

        cpu_heap = R_SYS_DHD_GetCpuHeap();
        if (R_NULL != cpu_heap)
        {
            (void)R_CDI_Free((uint32_t)actAddr, cpu_heap);
        }
    }
#endif
}



/***********************************************************
  Function: R_DRW2D_OS_Mem_Clear
*/
void R_DRW2D_OS_Mem_Clear(void *Addr, uint32_t NumBytes)
{
    uint32_t  i;
    
    if(R_NULL != Addr)
    {
        for (i = 0; i < NumBytes; i++)
        {
            ((uint8_t *)Addr)[i] = 0;
        }
    }
}


