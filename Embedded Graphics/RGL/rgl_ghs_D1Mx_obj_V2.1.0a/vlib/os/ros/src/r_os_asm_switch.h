
#ifndef R_OS_ASM_SWITCH_H_
#define R_OS_ASM_SWITCH_H_

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
  Section: Global Assembler Functions
*/

/*******************************************************************************
  Function: R_OS_Asm_ThreadPrepareStack

  Initialize stack of the thread. 

  Parameters:
  Thread  - Pointer to thread structure with initialized SP
  Entry   - Entry point of the thread
  Arg     - Argument for the 'StartFunction'

  Returns:
  void
*/

void R_OS_Asm_ThreadPrepareStack(R_OS_ThreadData_t *Thread, void *Entry, void *Arg);


/*******************************************************************************
  Function: R_OS_Asm_ThreadSwitchPreemptive

  Execute a preemptive thread change.

  Save current thread status, change current thread and activate 
  it. This function is for preemptive thread changes; it has to 
  be called from an ISR.

  Parameters:
  void

  Returns:
  void
*/

void R_OS_Asm_ThreadSwitchPreemptive(void);


/*******************************************************************************
  Function: R_OS_Asm_ThreadSwitchCooperative

  Execute a cooperative thread change.

  Save current thread status, change current thread and activate 
  it. This function is for cooperative thread changes; it must 
  not be called from an ISR.

  Parameters:
  void

  Returns:
  void
*/

void R_OS_Asm_ThreadSwitchCooperative(void);


/*******************************************************************************
  Function: R_OS_Asm_ThreadActiveStart

  Activate current thread.

  Parameters:
  void

  Returns:
  void
*/

void R_OS_Asm_ThreadActiveStart(void);


#ifdef __cplusplus
}
#endif

#endif /* R_OS_ASM_SWITCH_H_ */
