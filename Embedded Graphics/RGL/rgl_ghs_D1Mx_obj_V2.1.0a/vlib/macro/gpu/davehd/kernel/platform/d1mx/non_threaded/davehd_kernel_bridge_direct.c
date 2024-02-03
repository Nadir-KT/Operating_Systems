/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : Kernel callgate for user mode (non-threaded)
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 11006 $
 *  $Date: 2016-11-08 01:32:44 +0900 (火, 08 11 2016) $
 *  $LastChangedBy: michael.golczewski $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/

#include "davehd_kernel_driver.h"
#include "davehd_kernel_bridge.h"
#include "davehd_kernel_schedule.h"
#include "davehd_kernel_client.h"
#include "r_typedefs.h"


void dhd_gpu_bridge_wakeup(dhd_bridge_wakeup_token a_token) {
}

void dhd_gpu_dispatch_loop(void) {
}

/*----------------------------------------------------------------------------------------------------------
 * Perform a call to kernel mode. Calls into the kernel mode will block (and potentially send the caller
 * to sleep) until the request has been processed.
 *
 * Parameters:
 *  a_bridge - client side bridge handle (as returned by dhd_gpu_bridge_init_client).
 *  a_function - identifier for the function to be called (see dhd_gpu_call_entry_t).
 *  a_parameters - pointer to a dhd_gpu_call_data_t with the section relevant for a_function filled out.
 *
 * Returns:
 *  depends on function that was called
 *
 * See Also: <link !!Bridge, Kernel Mode Interface>
 */
dhd_uint32_t dhd_gpu_call(dhd_handle_t a_bridge, dhd_gpu_call_entry_t a_function, dhd_gpu_call_data_t *a_parameters) {
  dhd_uint32_t result = 0;
  dhd_gpu_call_data_t parameters = *a_parameters; /* local copy of parameters (kernel space visible memory)*/
  /* local kernel call can dispatch directly */
  if ( a_function == E_DHD_CALL_INIT ) {
    dhd_gpu_preprocess( &parameters );
  }
  result = dhd_gpu_dispatch( a_function, &parameters );

  if ((a_function == E_DHD_CALL_WAIT_FOR_JOB) &&
      (    result == E_DHD_ERROR_SCHEDULE_FOR_WAIT)) {
    /* should not happen: We should be blocking in non-threaded mode! -> m_force_synchronous seems not to have been set on dhd_gpu_init() call*/
    result = E_DHD_ERROR_FATAL;
  }
  return result;
}


/*----------------------------------------------------------------------------------------------------------
 * Create kernel thread which dispatches commands of its client(s).
 * Implementation for non-threaded KMD is empty.
 *
 * Parameters:
 * See Also: dhd_gpu_dispatch_loop
 */
void dhd_gpu_bridge_init_server(void) {

}


/*----------------------------------------------------------------------------------------------------------
 * Shut down kernel side of bridge. Counterpart for dhd_gpu_bridge_init_server.
 *
 * See Also: dhd_gpu_bridge_init_server
 */
void dhd_gpu_bridge_shutdown_server(void) {

  /* wake up all remaining clients */
  dhd_gpu_bridge_wakeup_all();
}


/*----------------------------------------------------------------------------------------------------------
 * Initialize client side of bridge. Each user mode client has to call this function in order to obtain
 * the bridge handle required to perform gpu calls.
 *
 * Returns:
 *  bridge handle
 *
 * See Also: dhd_gpu_bridge_shutdown_client
 */
dhd_handle_t dhd_gpu_bridge_init_client(void) {
  return (dhd_handle_t)1;
}


/*----------------------------------------------------------------------------------------------------------
 * Shut down client side of bridge. After all communication is finished a client must close it's side of the
 * bridge using this function
 *
 * Parameters:
 *  a_bridge - client side bridge handle (as returned by dhd_gpu_bridge_init_client).
 *
 * Returns:
 *  E_DHD_OK if the client was successfully shut down
 *  E_DHD_ERROR_FATAL if the given client bridge could not be found
 * See Also: dhd_gpu_bridge_init_client 
 */
dhd_uint32_t dhd_gpu_bridge_shutdown_client(dhd_handle_t a_bridge) {

  return E_DHD_OK;
}

/*----------------------------------------------------------------------------------------------------------
 * Asynchronously add a GPU call, discarding  the response.
 * This is designed to be only called from interrupt context.
 * Note: This platform's implementation does not allow a gpu_trigger to happen while the last one is
 * still running!
 */
void dhd_gpu_trigger(dhd_handle_t a_bridge, dhd_gpu_call_entry_t a_function, dhd_gpu_call_data_t *a_parameters) {

}
