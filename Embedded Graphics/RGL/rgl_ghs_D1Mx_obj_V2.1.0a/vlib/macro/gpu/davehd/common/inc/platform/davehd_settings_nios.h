/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : Compiletime settings, specifics for Altera Nios platform
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 4771 $
 *  $Date: 2015-02-10 20:01:30 +0900 (火, 10 2 2015) $
 *  $LastChangedBy: florian.zimmermann $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/    

#ifndef DAVEHD_SETTINGS_NIOS_H_INCLUDED
#define DAVEHD_SETTINGS_NIOS_H_INCLUDED

#include <system.h>
#include "davehd_device_data_size_default.h"

/* Nios does not support asynchronous mode -> disable asynchronous default*/
#define DHD_KERNEL_SYNCHRONOUS_ONLY

#define DHD_DEBUG_MODE
#define DHD_KERNEL_DEBUG
#define DHD_KERNEL_DEBUG_ERROR
#define DHD_INLINE_INTERFACE
#define DHD_KERNEL_TLIST_SUPPORT
#undef DHD_VIDMEM_MINALIGN_LOG2
#define DHD_VIDMEM_MINALIGN_LOG2        7
#undef DHD_MIN_JOB_SIZE
#define DHD_MIN_JOB_SIZE                512


#endif /*DAVEHD_SETTINGS_NIOS_H_INCLUDED*/