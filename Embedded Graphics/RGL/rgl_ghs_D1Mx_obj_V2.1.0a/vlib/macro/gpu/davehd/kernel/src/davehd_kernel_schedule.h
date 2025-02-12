/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : Job scheduling and state store/restore management
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 3935 $
 *  $Date: 2014-09-23 17:52:32 +0900 (火, 23 9 2014) $
 *  $LastChangedBy: golczewskim $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/    

#ifndef DAVEHD_KERNEL_SCHEDULE_H_INCLUDED
#define DAVEHD_KERNEL_SCHEDULE_H_INCLUDED

#include "davehd_types.h"

/*----------------------------------------------------------------------------------------------------------*/

dhd_enum_t dhd_gpu_schedule_job(dhd_gpu_device_data_t *a_device);
dhd_uint32_t dhd_gpu_mark_finished_jobs(dhd_gpu_device_data_t *a_device);
dhd_uint32_t dhd_gpu_query_jobsync(const dhd_gpu_device_data_t *a_device, dhd_bool_t a_direct);
dhd_uint32_t dhd_gpu_yield(dhd_gpu_device_data_t *a_device);

/*----------------------------------------------------------------------------------------------------------*/

#endif /*DAVEHD_KERNEL_SCHEDULE_H_INCLUDED*/
