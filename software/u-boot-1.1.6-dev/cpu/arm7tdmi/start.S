/*
 * Startup Code for lpc2294 CPU-core
 *
 * (C) Copyright 2005
 * jay
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */


#include <config.h>
#include <version.h>


/*
 * Jump vector table
 */


.globl _start
_start:	ldr     pc, ResetAddr

        ldr     pc, UndefinedAddr
        ldr     pc, SWI_Addr
        ldr     pc, PrefetchAddr
        ldr     pc, DataAbortAddr
		.word	0xb8a06f58
        LDR     pc, IRQ_Addr
        LDR     pc, FIQ_Addr
        
ResetAddr:
  .word 0x50
//  	.word reset
UndefinedAddr:
	.word	0x81000004
//	.word undefined_instruction
SWI_Addr:
	.word	0x81000008
//	.word software_interrupt
PrefetchAddr:
	.word 0x8100000c
//	.word prefetch_abort
DataAbortAddr:
	.word 0x81000010
//	.word data_abort
Nouse:
	.word 0x8100001c
//	.word not_used
IRQ_Addr:
	.word 0x81000018
//	.word irq
FIQ_Addr:
	.word 0x8100001c
//	.word fiq
/*
 *************************************************************************
 *
 * Startup Code (reset vector)
 *
 * do important init only if we don't start from memory!
 * relocate u-boot to ram
 * setup stack
 * jump to second stage
 *
 *************************************************************************
 */

_TEXT_BASE:
	.word	TEXT_BASE

.globl _armboot_start
_armboot_start:
	.word _start

/*
 * These are defined in the board-specific linker script.
 */
.globl _bss_start
_bss_start:
	.word __bss_start

.globl _bss_end
_bss_end:
	.word _end

#ifdef CONFIG_USE_IRQ
/* IRQ stack memory (calculated at run-time) */
.globl IRQ_STACK_START
IRQ_STACK_START:
	.word	0x0badc0de

/* IRQ stack memory (calculated at run-time) */
.globl FIQ_STACK_START
FIQ_STACK_START:
	.word 0x0badc0de
#endif


/*
 * the actual reset code
 */
#define PINSEL0 0xE002C000
#define PINSEL2 0xE002C014
#define BCFG0 0xFFE00000
#define BCFG1 0xFFE00004
#define BCFG2 0xFFE00008
#define BCFG3 0xFFE0000C

reset:
	/*
	 * set the cpu to SVC32 mode,disable all interrupts
	 */
	mrs	r0,cpsr
	bic	r0,r0,#0x3f
	orr	r0,r0,#0x13
	orr r0,r0,#0x80
	orr r0,r0,#0x40
	msr	cpsr,r0
	
	/*
	 *Initialize external bus controller, determined by target board
	 */

	ldr r0,=PINSEL0
	ldr r1, =0x80050000
	str r1,[r0]
	
  	ldr r0,=PINSEL2
  	ldr r1,=0x0F814924
  	str r1,[r0]
  	ldr r0,=BCFG0
  	ldr r1,=0x1000348F
  	str r1,[r0]
  	ldr r0,=BCFG1
  	ldr r1,=0x20000c40
  	str r1,[r0]
  	ldr r0,=BCFG2
  	ldr r1,=0x2000ffe0
  	str r1,[r0]

	/*
	 * we do sys-critical inits only at reboot,
	 * not when booting from ram!
	 */

#ifndef CONFIG_SKIP_LOWLEVEL_INIT
	bl	cpu_init_crit
	/*
	 * before relocating, we have to setup RAM timing
	 * because memory timing is board-dependend, you will
	 * find a memsetup.S in your board directory.
	 */
	bl	lowlevel_init
#endif

#ifndef CONFIG_SKIP_RELOCATE_UBOOT
relocate:				/* relocate U-Boot to RAM	    */
	adr	r0, _start		/* r0 <- current position of code   */
	ldr	r1, _TEXT_BASE		/* test if we run from flash or RAM */
	cmp     r0, r1                  /* don't reloc during debug         */
	beq     stack_setup

	ldr	r2, _armboot_start
	ldr	r3, _bss_start
	sub	r2, r3, r2		/* r2 <- size of armboot            */
	add	r2, r0, r2		/* r2 <- source end address         */

copy_loop:
	ldmia	r0!, {r3-r10}		/* copy from source address [r0]    */
	stmia	r1!, {r3-r10}		/* copy to   target address [r1]    */
	cmp	r0, r2			/* until source end addreee [r2]    */
	ble	copy_loop

/*
	now copy to sram the interrupt vector - internal
*/
	adr	r0, real_vectors
	add	r2, r0, #1024
	ldr	r1, =0x81000000
	add	r1, r1, #0x08
vector_copy_loop:
	ldmia	r0!, {r3-r10}
	stmia	r1!, {r3-r10}
	cmp	r0, r2
	ble	vector_copy_loop


#endif	/* CONFIG_SKIP_RELOCATE_UBOOT */

	/* Set up the stack						    */
stack_setup:
	ldr	r0, _TEXT_BASE		/* upper 128 KiB: relocated uboot   */
	sub	r0, r0, #CFG_MALLOC_LEN	/* malloc area                      */
	sub	r0, r0, #CFG_GBL_DATA_SIZE /* bdinfo                        */
#ifdef CONFIG_USE_IRQ
	sub	r0, r0, #(CONFIG_STACKSIZE_IRQ+CONFIG_STACKSIZE_FIQ)
#endif
	sub	sp, r0, #12		/* leave 3 words for abort-stack    */

clear_bss:
	ldr	r0, _bss_start		/* find start of bss segment	    */
	ldr	r1, _bss_end		/* stop here			    */
	mov	r2, #0x00000000		/* clear			    */

clbss_l:
	str	r2, [r0]		/* clear loop...		    */
	add	r0, r0, #4
	cmp	r0, r1
	ble	clbss_l

	ldr	pc, _start_armboot

_start_armboot:	.word start_armboot

/*
 *************************************************************************
 *
 * CPU_init_critical registers
 *
 * setup important registers
 * setup memory timing
 *
 *************************************************************************
 */
#define MEMMAP 0xE01FC040
#define PLLCON 0xE01FC080
#define VPBDIV 0xE01FC100
#define PLLCFG 0xE01FC084
#define PLLFEED 0xE01FC08C
#define PLLSTAT 0xE01FC088

cpu_init_crit:
	/*remap to external memory*/
	ldr r0, =0xe01fc000
	ldr	r1, =0x3
	str	r1, [r0,#0x40]
	
	/*set pll parameters*/
    ldr r1, =0x1
    str r1, [r0,#0x80]
  

  	ldr r1, =0
  	str r1, [r0,#0x100]
  
  	ldr r1, =0x23
  	str r1,[r0, #0x84]

  	ldr r1, =0xaa
  	str r1, [r0, #0x8c]
  	ldr r1, =0x55
    str r1, [r0, #0x8c]
  
  	mov r1, r0
plltest_loop:
  	ldrh r0,[r1, #0x88]
  	tst r0,#0x400
  	beq plltest_loop
  
  	ldr r0, =0x3
  	str r0, [r1, #0x80]

  	ldr r0, =0xaa
  	str r0, [r1, #0x8c]
  	ldr r0, =0x55
  	str r0, [r1, #0x8c]

	mov	pc, lr


/*************************************************/
/*	interrupt vectors	*/
/*************************************************/
real_vectors:
	b	reset
	b	undefined_instruction
	b	software_interrupt
	b	prefetch_abort
	b	data_abort
	b	not_used
	b	irq
	b	fiq

/*************************************************/

undefined_instruction:
	mov	r6, #3
	b	reset

software_interrupt:
	mov	r6, #4
	b	reset

prefetch_abort:
	mov	r6, #5
	b	reset

data_abort:
	mov	r6, #6
	b	reset

not_used:
	/* we *should* never reach this */
	mov	r6, #7
	b	reset

irq:
	mov	r6, #8
	b	reset

fiq:
	mov	r6, #9
	b	reset
