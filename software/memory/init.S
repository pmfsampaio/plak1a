/*****************************************************************
	Module includes the interrupt vectors and start-up code.
 ******************************************************************/

/* Stack Sizes */
.set  	UND_STACK_SIZE, 0x00000004		/* stack for "undefined instruction" interrupts is 4 bytes  */
.set  	ABT_STACK_SIZE, 0x00000004		/* stack for "abort" interrupts is 4 bytes                  */
.set  	FIQ_STACK_SIZE, 0x00000004		/* stack for "FIQ" interrupts  is 4 bytes         			*/
.set  	IRQ_STACK_SIZE, 0X00000004		/* stack for "IRQ" normal interrupts is 4 bytes    			*/
.set  	SVC_STACK_SIZE, 0x00000004		/* stack for "SVC" supervisor mode is 4 bytes  				*/

.set  	MODE_USR, 0x10            		/* Normal User Mode 										*/
.set  	MODE_FIQ, 0x11            		/* FIQ Processing Fast Interrupts Mode 						*/
.set  	MODE_IRQ, 0x12            		/* IRQ Processing Standard Interrupts Mode 					*/
.set  	MODE_SVC, 0x13            		/* Supervisor Processing Software Interrupts Mode 			*/
.set  	MODE_ABT, 0x17            		/* Abort Processing memory Faults Mode 						*/
.set  	MODE_UND, 0x1B            		/* Undefined Processing Undefined Instructions Mode 		*/
.set  	MODE_SYS, 0x1F            		/* System Running Priviledged Operating System Tasks  Mode	*/
.set  	I_BIT, 0x80               		/* when I bit is set, IRQ is disabled (program status registers) */
.set  	F_BIT, 0x40               		/* when F bit is set, FIQ is disabled (program status registers) */

.set	MEMMAP, 0xE01FC040
.text

.global _start

# Exception Vectors
_vectors:
		ldr     PC, ResetAddr
    	ldr     PC, UndefAddr
    	ldr     PC, SWIAddr
    	ldr     PC, PAbtAddr
    	ldr     PC, DAbtAddr
    	nop							/* Reserved Vector (holds Philips ISP checksum) */
    	ldr     PC, [PC,#-0xFF0]
    	ldr     PC, FIQAddr

ResetAddr:		.word   ResetHandler
UndefAddr:     	.word   UndefHandler
SWIAddr:       	.word   SWIHandler
PAbtAddr:      	.word   PAbortHandler
DAbtAddr:      	.word   DAbortHandler
			   	.word	0
			   	.word	0
FIQAddr:       	.word   FIQHandler

ResetHandler:
_start:
		ldr		r0, =MEMMAP
		mov		r1, #0x02
		str		r1, [r0]

		ldr   	r0, =__stack_top__
		msr   	CPSR_c, #MODE_UND|I_BIT|F_BIT 	/* Undefined Instruction Mode  */
		mov   	sp, r0
		sub   	r0, r0, #UND_STACK_SIZE
		msr   	CPSR_c, #MODE_ABT|I_BIT|F_BIT 	/* Abort Mode */
		mov   	sp, r0
		sub   	r0, r0, #ABT_STACK_SIZE
		msr   	CPSR_c, #MODE_FIQ|I_BIT|F_BIT 	/* FIQ Mode */
		mov   	sp, r0
		sub   	r0, r0, #FIQ_STACK_SIZE
		msr   	CPSR_c, #MODE_IRQ|I_BIT|F_BIT 	/* IRQ Mode */
		mov   	sp, r0
		sub   	r0, r0, #IRQ_STACK_SIZE
		msr   	CPSR_c, #MODE_SVC|I_BIT|F_BIT 	/* Supervisor Mode */
		mov   	sp, r0
		sub   	r0, r0, #SVC_STACK_SIZE
		msr   	CPSR_c, #MODE_SYS|I_BIT|F_BIT 	/* User Mode */
		mov   	sp, r0

		/*
		 * Clear .bss section
		 */
		ldr   	r1, =__bss_start__
		ldr   	r2, =__bss_end__
		ldr   	r3, =0
bss_clear_loop:
		cmp   	r1, r2
	   	strne 	r3, [r1], #+4
	   	bne   	bss_clear_loop

		b       main

.section .stack
		.space	1024 * 4
__stack_top__:

/****************************************************************************/
/*                         Default interrupt handler                        */
/****************************************************************************/

UndefHandler:
   b UndefHandler

SWIHandler:
   b SWIHandler

PAbortHandler:
   b PAbortHandler

DAbortHandler:
   b DAbortHandler

FIQHandler:
   b FIQHandler

.end
