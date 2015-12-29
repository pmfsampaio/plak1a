/*
 * (C) Copyright 2009
 * Pedro Sampaio
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#define MAMCR 			0xE01FC000
#define MAMTIM 			0xE01FC004
#define VICIntEnClr 	0xFFFFF014
#define VICVectAddr 	0xFFFFF030
#define VICIntSelect 	0xFFFFF00C

.globl lowlevel_init
lowlevel_init:

/* Set MAM parameters */
	ldr r0, =MAMCR
  	ldr r1, =0x0
  	str r1, [r0]
  
  	ldr r0, =MAMTIM
  	ldr r1, =0x03 /*4 =0x3 */
  	str r1, [r0]
  
  	ldr r0, =MAMCR
  	ldr r1, =0x2
  	str r1, [r0]
  
  	ldr r0, =VICIntEnClr
  	ldr r1, =0xffffffff
  	str r1, [r0]
  
  	ldr r0, =VICVectAddr
  	ldr r1, =0x0
  	str r1, [r0]
  	ldr r0, =VICIntSelect
  	str r1, [r0]
  
	mov pc, lr

