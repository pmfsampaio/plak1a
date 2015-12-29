/*
 * vic.c
 *
 *  Created on: Oct 29, 2009
 *      Author: psampaio
 */

#define IRQ_MASK 0x00000080
#define FIQ_MASK 0x00000040
#define INT_MASK (IRQ_MASK | FIQ_MASK)

static inline unsigned __get_cpsr(void)
{
  unsigned long retval;
  asm volatile (" mrs  %0, cpsr" : "=r" (retval) : /* no inputs */  );
  return retval;
}

static inline void __set_cpsr(unsigned val)
{
  asm volatile (" msr  cpsr, %0" : /* no outputs */ : "r" (val)  );
}

unsigned disableIRQ(void)
{
  unsigned _cpsr;

  _cpsr = __get_cpsr();
  __set_cpsr(_cpsr | IRQ_MASK);
  return _cpsr;
}

unsigned restoreIRQ(unsigned oldCPSR)
{
  unsigned _cpsr;

  _cpsr = __get_cpsr();
  __set_cpsr((_cpsr & ~IRQ_MASK) | (oldCPSR & IRQ_MASK));
  return _cpsr;
}

unsigned enableIRQ(void)
{
  unsigned _cpsr;

  _cpsr = __get_cpsr();
  __set_cpsr(_cpsr & ~IRQ_MASK);
  return _cpsr;
}

unsigned disableFIQ(void)
{
  unsigned _cpsr;

  _cpsr = __get_cpsr();
  __set_cpsr(_cpsr | FIQ_MASK);
  return _cpsr;
}

unsigned restoreFIQ(unsigned oldCPSR)
{
  unsigned _cpsr;

  _cpsr = __get_cpsr();
  __set_cpsr((_cpsr & ~FIQ_MASK) | (oldCPSR & FIQ_MASK));
  return _cpsr;
}

unsigned enableFIQ(void)
{
  unsigned _cpsr;

  _cpsr = __get_cpsr();
  __set_cpsr(_cpsr & ~FIQ_MASK);
  return _cpsr;
}
