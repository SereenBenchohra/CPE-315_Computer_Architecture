    .arch armv7-a
    .global intadd
	.syntax unified
	.thumb
	.thumb_func

intadd:
         @ Stack the return address (lr) in addition to a dummy register (ip) to
         @ keep the stack 8-byte aligned.
loop:    eors r2, r0, r1  @ Exclusive or product and multiplicand
         ands r3, r0, r1  @ Get the carry using and of product and multiplicand
         lsls r3, r3, #1  @ Shift the carry left one
         mov r0, r2       @ Put the exclusive or result in r0
         mov r1, r3       @ Put the carry in r1
         cmp r1, #0       @ If carry is 0, we are done
         bne loop
         bx lr

