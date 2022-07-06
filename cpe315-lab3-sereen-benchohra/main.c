#include <stdio.h>

int multiply(int a, int b)
{
   return a * b;
}

int main()
{
   int a, b, mult;
   a = 11;
   b = 6;
   mult = multiply(11, 6);

   printf("%d * %d = %d\n", a, b, mult);

   return 0;
}
