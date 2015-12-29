/*
 * irqh.S
 *
 *  Created on: Oct 30, 2009
 *      Author: psampaio
 */
.text
.global Timer0ISRH
.global enableIRQ

enableIRQ:
	mrs 	r12, CPSR
	bic 	r12, r12, #0x80
	msr 	CPSR_c, r12
	mov		pc,	lr

Timer0ISRH:
	sub		lr, lr, #4
	stmfd	sp!, {r0-r12, lr}

	ldr		r1, =Timer0Handler
	mov     lr, pc
	bx		r1

	ldmfd	sp!, {r0-r12, pc}^
