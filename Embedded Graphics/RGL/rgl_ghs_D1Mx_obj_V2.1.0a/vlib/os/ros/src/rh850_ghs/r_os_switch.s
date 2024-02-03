
/***********************************************************
*
*                       Defines
*
************************************************************
*/

#define SAVEREGNUM      13
#define RH850G3M 				1
#if USED_CPU == 1
	#define USER_PSW        0x00030000 /* CU0 | CU1 */
#else
  #define USER_PSW        0x00040000  /* NPV */
/*#define USER_PSW        0x00050000*/    /* NPV | IMP */
#endif

/***********************************************************
*
*                         Code
*
************************************************************
*/        
        .text

        
/***********************************************************
*
*                 R_OS_Asm_ThreadPrepareStack
*/
        .globl _R_OS_Asm_ThreadPrepareStack
_R_OS_Asm_ThreadPrepareStack:
        ld.w    0[r6], r12      /* get SP */
        /* Save Arg parameter on the stack */
        addi	-4, r12, r12
        st.w    r8, 0*4[r12]
        /* Save permanent registers r20 - r29 */
        addi	-SAVEREGNUM*4, r12, r12
        st.w    r12, 0[r6]
        st.w    r7, 0*4[r12]    /* save user entry point */
        mov     USER_PSW, r7
        st.w    r7, 1*4[r12]    /* PSW */
        st.w    r20, 2*4[r12]
        st.w    r21, 3*4[r12]
        st.w    r22, 4*4[r12]
        st.w    r23, 5*4[r12]
        st.w    r24, 6*4[r12]
        st.w    r25, 7*4[r12]
        st.w    r26, 8*4[r12]
        st.w    r27, 9*4[r12]
        st.w    r28, 10*4[r12]
        st.w    r29, 11*4[r12]
        mov     FirstActication, r7
        st.w    r7,  12*4[r12] /* save lp - kernel mode entry point */
        jmp     [lp]


/***********************************************************
*
*                R_OS_Asm_ThreadSwitchPreemptive
*/        
        .globl _R_OS_Asm_ThreadSwitchPreemptive
_R_OS_Asm_ThreadSwitchPreemptive:
        mov	_R_OS_ThreadCurrent, r2
	ld.w	0[r2],r2
        cmp     r2, r0
        be      SwitchFromIdle


/***********************************************************
*
*                 R_OS_Asm_ThreadSwitchCooperative
*/
        .globl _R_OS_Asm_ThreadSwitchCooperative
_R_OS_Asm_ThreadSwitchCooperative:
        addi	-SAVEREGNUM*4, sp, sp
        stsr    0, r2           /* EIPC */
        st.w    r2, 0*4[sp]     /* save entry point */
        stsr    1, r2           /* EIPSW */
        st.w    r2, 1*4[sp]     /* PSW */
        st.w    r20, 2*4[sp]
        st.w    r21, 3*4[sp]
        st.w    r22, 4*4[sp]
        st.w    r23, 5*4[sp]
        st.w    r24, 6*4[sp]
        st.w    r25, 7*4[sp]
        st.w    r26, 8*4[sp]
        st.w    r27, 9*4[sp]
        st.w    r28, 10*4[sp]
        st.w    r29, 11*4[sp]
        st.w    lp, 12*4[sp]

        /* Save SP in current process */
        mov	_R_OS_ThreadCurrent, r2
	ld.w	0[r2],r2
        st.w    sp, 0[r2]
SwitchFromIdle: 
        mov     ___ghsend_stack, sp
        jarl    _R_OS_Prv_ThreadChange, lp


/***********************************************************
*
*                _R_OS_Asm_ThreadActiveStart
*/
        .globl _R_OS_Asm_ThreadActiveStart
_R_OS_Asm_ThreadActiveStart:
        /* Get SP of PCB, which shall be activated */
        mov	_R_OS_ThreadCurrent, r10
	ld.w	0[r10],r2
        cmp     r2, r0
        be      SwitchToIdle
        ld.w    0[r2], sp
        /* Load permanent registers r20 - r29 */
        ld.w    2*4[sp], r20
        ld.w    3*4[sp], r21
        ld.w    4*4[sp], r22
        ld.w    5*4[sp], r23
        ld.w    6*4[sp], r24
        ld.w    7*4[sp], r25
        ld.w    8*4[sp], r26
        ld.w    9*4[sp], r27
        ld.w    10*4[sp], r28
        ld.w    11*4[sp], r29
        ld.w    12*4[sp], lp

        ld.w    0*4[sp], r2    /* load entry point */

        di
        ldsr    r2, 0           /* EIPC */
        ld.w    1*4[sp], r2
        ldsr    r2, 1           /* EIPSW */
        addi	SAVEREGNUM*4, sp, sp
        jmp     [lp] /* back to trap eiret */        
        


FirstActication:        
        mov     LeaveThread, lp
        ld.w    0*4[sp], r6
        addi	4, sp, sp
        eiret        

LeaveThread:
        mov     0, r6
        jarl    _R_OS_ThreadExit, lp
1:       
        br     1b


        
SwitchToIdle:
        mov     _R_OS_Sys_Idle, r2
        ldsr    r2, 0           /* EIPC */
        mov     USER_PSW, r2
        ldsr    r2, 1           /* EIPSW */
        eiret        
        br      SwitchToIdle
                


        
        
       /* EOF */
