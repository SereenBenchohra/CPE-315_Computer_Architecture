
////////////////////////////////////////////////////////////////////////////
// Here you'll implement the Shift-and-Add Algorithm described in class   //
//product = 0;
//while (multiplier != 0){
  
//  if(LSB(multiplier) == 1)
//     product = product + multiplicand 
//  multiplier = multiplier = multiplier >> 1
//  multiplicand = multiplicand << 1
//}

////////////////////////////////////////////////////////////////////////////
 
	.arch armv7-a
	.global intmul
        .thumb
        .thumb_func

intmul:
   push  {r4, r5, r6, r7, lr}  // r4 is multiplier, r5 is multiplicand, r6 product, r7 is LSB
   mov   r4, r0   //copy multiplier to r4
   mov   r5, r1   //copy multiplicand to r5
   mov   r6, #0    // product set to zero
   cmp   r4, #0    // check to see multiplier is zero
   
   beq   end       //if multiplier is zero then end

loop:
   mov   r7, r4   //copy multiplier into r7
   mov   r2, #1
   and   r7, r7, r2  // and the multiplier with 1 to get the least significant bit
   cmp   r7, #1    // check to see if LSB is 1
   beq   ADD
   
shift:
   lsr   r4, r4, #1    // shift multiplier to the right
   lsl   r5, r5, #1    // multiplicand to the left  
   cmp   r4, #0    // check to see if multiplier is 0
   bne   loop     // if not then loop again
   b     end           //otherwise end

ADD:
   mov   r1, r5   // move multiplicand to r1
   mov   r0, r6   // move product to r0
   bl    intadd   // add the product and multiplicand
   mov   r6, r0   // saving the new product into the old one

   b     shift    // after finished with the add go to the shift
   
end:
   mov   r0, r6    //move product to r0
   pop   {r4, r5, r6, r7, pc}  //pop everything

