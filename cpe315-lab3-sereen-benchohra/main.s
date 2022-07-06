.data
.balign 4
   string: .asciz " %d * %d = %d\n"
.text

.global main
.extern printf


main:

   push {ip, lr}

   mov   r0, #-999
   mov   r1, #98
   bl intmul
   mov   r3, r0
   mov   r1, #-999
   mov   r2, #98
   ldr   r0, =string
   bl printf

   pop {ip, pc}

