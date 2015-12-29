/*
 * irqh.S
 *
 *  Created on: Oct 30, 2009
 *      Author: psampaio
 */

.equ AIC,	0xfffff000


.text
.global uart0ISRH
.global uart1ISRH
.global isrSpurious

uart0ISRH:
	sub		lr, lr, #4
	stmfd	sp!, {r0-r12, lr}

	ldr		r1, =uart0ISR
	mov     lr, pc
	bx		r1

	ldmfd	sp!, {r0-r12, pc}^

uart1ISRH:
	sub		lr, lr, #4
	stmfd	sp!, {r0-r12, lr}

	ldr		r1, =uart1ISR
	mov     lr, pc
	bx		r1

	ldmfd	sp!, {r0-r12, pc}^

isrSpurious:
	sub		lr, lr, #4
	stmfd	sp!, {r0-r12, lr}

	ldmfd	sp!, {r0-r12, pc}^
