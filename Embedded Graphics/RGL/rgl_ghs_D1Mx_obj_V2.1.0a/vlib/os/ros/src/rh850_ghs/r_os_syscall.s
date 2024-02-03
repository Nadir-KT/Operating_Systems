
/***********************************************************
  Section: Global Functions
*/

        .text


/***********************************************************
  Function: _R_OS_SysCall

  OS sytem call.

  All OS calls are directed to this function. Its purpose 
  is to switch the CPU into a priviledged mode and to execute
  the desired OS functionality in priviledged mode.
  
  Parameters:
  
*/
        .globl _R_OS_SysCall
_R_OS_SysCall:    
        trap    0x00
        jmp     lp

                
       /* EOF */
