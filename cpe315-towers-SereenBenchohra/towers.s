	.arch armv6
	.fpu vfp
	.text

@ print function is complete, no modifications needed
    .global	print
print:
	stmfd	sp!, {r3, lr}
	mov	r3, r0
	mov	r2, r1
	ldr	r0, startstring
	mov	r1, r3
	bl	printf
	ldmfd	sp!, {r3, pc}

startstring:
	.word	string0

    .global	towers
towers:
   	/* Save call-saved registers to stack */
	push	{r4, r5, r6, r7, r8, lr}
	
	/* Save a copy of all 3 incoming parameters */
	mov	r7, r1
	mov	r6, r2

	cmp	r0, #1	/* Compare numDisks with 2 or (numDisks - 2)*/
	ble	if
   	b  else
if:
	mov	r1, r2  /* set print function's start to incoming start */
	mov	r0, r7	/* set print function's end to goal */
	bl	print	/* call print function */
	mov	r0, #1  /* Set return register to 1 */
   	b  endif	/* branch to endif */

else:
   rsb	r8, r1, #6 	/* Use a callee-saved varable for temp and set it to 6 */
	sub	r8, r8, r2	/* Subract start from temp and store to itself */
	sub	r5, r0, #1	/* subtract 1 from original numDisks and store it to numDisks parameter */
	mov	r2, r8
	mov	r0, r5		
	bl	towers		/* Call towers function */
	mov	r4, r0		/* Save result to callee-saved register for total steps */
	mov	r2, r6
	mov	r1, r7
	mov	r0, #1		/* Set numDiscs parameter to 1 */
	bl	towers		/* Call towers function */
	add	r4, r4, r0	/* Add result to total steps so far */
	mov	r2, r6		/* set goal parameter to original goal */
	mov	r1, r8		/* set start parameter to temp */
	mov	r0, r5		/* Set numDisks parameter to original numDisks - 1 */
	bl	towers
	add	r0, r4, r0	/* Add result to total steps so far and save it to return register */
	
endif:
   pop	{r4, r5, r6, r7, r8, pc} /* Restore Registers */

@ Function main is complete, no modifications needed
    .global	main
main:
	str	lr, [sp, #-4]!
	sub	sp, sp, #20
	ldr	r0, printdata
	bl	printf
	ldr	r0, printdata+4
	add	r1, sp, #12
	bl	scanf
	ldr	r0, [sp, #12]
	mov	r1, #1
	mov	r2, #3
	bl	towers
	str	r0, [sp]
	ldr	r0, printdata+8
	ldr	r1, [sp, #12]
	mov	r2, #1
	mov	r3, #3
	bl	printf
	mov	r0, #0
	add	sp, sp, #20
	ldr	pc, [sp], #4
end:

printdata:
	.word	string1
	.word	string2
	.word	string3

string0:
	.asciz	"Move from peg %d to peg %d\n"
string1:
	.asciz	"Enter number of discs to be moved: "
string2:
	.asciz	"%d"
	.space	1
string3:
	.ascii	"\n%d discs moved from peg %d to peg %d in %d steps."
	.ascii	"\012\000"
