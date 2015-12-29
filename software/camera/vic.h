/*
 * vic.h
 *
 *  Created on: Oct 29, 2009
 *      Author: psampaio
 */

#ifndef VIC_H_
#define VIC_H_

/******************************************************************************
 *
 * MACRO Name: ISR_ENTRY()
 *
 * Description:
 *    This MACRO is used upon entry to an ISR.  The current version of
 *    the gcc compiler for ARM does not produce correct code for
 *    interrupt routines to operate properly with THUMB code.  The MACRO
 *    performs the following steps:
 *
 *    1 - Adjust address at which execution should resume after servicing
 *        ISR to compensate for IRQ entry
 *    2 - Save the non-banked registers r0-r12 and lr onto the IRQ stack.
 *    3 - Get the status of the interrupted program is in SPSR.
 *    4 - Push it onto the IRQ stack as well.
 *
 *****************************************************************************/
#define ISR_ENTRY() asm volatile(" sub   lr, lr,#4\n" \
                                 " stmfd sp!,{r0-r12,lr}\n" \
                                 " mrs   r1, spsr\n" \
                                 " stmfd sp!,{r1}")

/******************************************************************************
 *
 * MACRO Name: ISR_EXIT()
 *
 * Description:
 *    This MACRO is used to exit an ISR.  The current version of the gcc
 *    compiler for ARM does not produce correct code for interrupt
 *    routines to operate properly with THUMB code.  The MACRO performs
 *    the following steps:
 *
 *    1 - Recover SPSR value from stack
 *    2 - and restore  its value
 *    3 - Pop the return address & the saved general registers from
 *        the IRQ stack & return
 *
 *****************************************************************************/
#define ISR_EXIT()  asm volatile(" ldmfd sp!,{r1}\n" \
                                 " msr   spsr_c,r1\n" \
                                 " ldmfd sp!,{r0-r12,pc}^")

unsigned disableIRQ(void);
unsigned enableIRQ(void);
unsigned restoreIRQ(unsigned oldCPSR);
unsigned disableFIQ(void);
unsigned enableFIQ(void);
unsigned restoreFIQ(unsigned oldCPSR);


#endif /* VIC_H_ */
