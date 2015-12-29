#ifndef _PRINTF_P_H_
#define _PRINTF_P_H_


extern void _printf_P (char const *fmt0, ...);

#define printf(format, args...)   _printf_P(format , ## args)

#endif
