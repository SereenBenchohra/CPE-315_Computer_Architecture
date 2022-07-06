////////////////////////////////////////////////////////////////////////////////
// You're implementing the following function in ARM Assembly
//! C = A * B
//! @param C          result matrix
//! @param A          matrix A 
//! @param B          matrix B
//! @param hA         height of matrix A
//! @param wB         width of matrix B
//
//void matmul(int* C, const int* A, const int* B, unsigned int hA, 
//    unsigned int wA, unsigned int wB)
//{
//  for (unsigned int i = 0; i < hA; ++i)
//    for (unsigned int j = 0; j < wB; ++j) {
//      int sum = 0;
//      for (unsigned int k = 0; k < wA; ++k) {
//        sum += A[i * wA + k] * B[k * wB + j];
//      }
//      C[i * wB + j] = sum;
//    }
//}
////////////////////////////////////////////////////////////////////////////////

	.arch armv7-a
        .text
	.global matmul
        .align  1
        .syntax unified
        .thumb
        .thumb_func
matmul:
   // r4 = result matrix (C). r5 = param A. r6 = param B. r7 = hA = wA. r8 = i, r9 = j, r10 = sum, r11 = k
   push  {r4, r5, r6, r7, r8, r9, r10, r11, lr} 
   mov   r4, r0   //saves C 
   mov   r5, r1   //Saves A
   mov   r6, r2   //Saves B
   mov   r7, r3   //Saves the height of Matrix A
   mov   r8, #-1   //set i as 0

loopI:
   mov   r0, r8   //set i as first parameter
   mov   r1, #1   //set 1 as second paramter
   bl    intadd   //increment i by 1
   mov   r8, r0   //store i back into r8
   cmp   r8, r7   //compare i with hA
   bge   end      //if greater than or equal to go to end
     
   mov   r9, #0   //set j as 0 (until loop i)
 
loopJ:   
   cmp   r9, r7   //compare j with wB
   bge   loopI    //if less than jump to loopJ   
 
   mov   r10, #0  //set sum to 0
   mov   r11, #0   //set k to 0

loopK:   
   cmp   r11, r7   //compare k to wA
   blt   SUM       //if less than jump to SUM

   //C[i * wB + j]

   mov   r0, r8         // set i as first param
   mov   r1, r7         // set wB as second param
   bl intmul            // multiply i * wB

   mov   r1, r9   //set j as second parameter for add
   bl    intadd   //add (i * wB) + j
   lsl   r0, r0, #2  //muliply by 4
   mov   r1, r4   //move address of C
   bl    intadd   //add original address of C with new value
   str   r10, [r0]    // stores sum to new C address

   mov   r0, r9   //set j as first parameter
   mov   r1, #1   //set 1 as second parameter
   bl    intadd   //increment j by 1
   mov   r9, r0   //save j back to r9
   b     loopJ    //jump back to loopJ

SUM:
   mov   r0, r8         // set i as first param
   mov   r1, r7         // set wA as second
   bl intmul            // multiply i *wA
   mov   r1, r11   //set k as second parameter
   bl    intadd   //add (i * wA) + k
   lsl   r0, r0, #2  //multiply by 4
   mov   r1, r5   //set original address of A as second parameter
   bl    intadd   //add original address to new value

   push  {r0}  //push new address to stack
   // mul   r0, r11, r7    //multiply k * wB
   mov   r0, r11
   mov   r1, r7
   bl    intmul
   mov   r1, r9   //set j as second parameter
   bl    intadd   //add (k * wB) + j
   lsl   r0, r0, #2  //multiply by 4
   mov   r1, r6   //set original address of B as second parameter
   bl intadd   //add original address to new value
   
   mov   r3, r0 //move new B address to r3
   pop   {r2}  //pop the new A address to r2
   ldr   r0, [r2] //load value from A address
   ldr   r1, [r3] //load value from B address
   // mul   r0, r0, r1  //multiply value from A to value from B
   bl intmul
   mov   r1, r10  //set sum as second parameter
   bl intadd   //add sum to multiplied value
   mov   r10, r0  //move value back to sum register

   mov   r0, r11  //set k as first parameter
   mov   r1, #1   //set 1 as second parameter
   bl intadd   //increment k by 1
   mov  r11, r0   //save k back to r11
   b     loopK //jump back to loopK

end:
   //pop stuff
   pop  {r4, r5, r6, r7, r8, r9, r10, r11, pc} 

