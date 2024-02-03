
/***********************************************************
  Section: Global Functions
*/

        .text

/***********************************************************
  Function: R_OS_Trap0X

  Trap handler of the eeOS.

  This function is called by 'eeOS_Syscall'
  
  Parameters:
  
*/
        .globl _TRAP0
_TRAP0: 
        .globl _R_OS_Trap0X
_R_OS_Trap0X:
        addi	-4, sp, sp
	st.w	lp, 0[sp]
        
        /* sub-routine */
        jarl    _R_OS_Prv_CSysCall,lp

	ld.w	0[sp], lp
        addi	4, sp, sp
	eiret

        
       /* EOF */
