#include <stdlib.h>
#include "matmul.h"

void matmul(int*, const int*, const int*, unsigned int, unsigned int, unsigned int);

////////////////////////////////////////////////////////////////////////////////
//! C = A * B
//! @param C          result matrix
//! @param A          matrix A 
//! @param B          matrix B
//! @param hA         height of matrix A
//! @param wB         width of matrix B
////////////////////////////////////////////////////////////////////////////////

void matmul(int* C, const int* A, const int* B, unsigned int hA, 
    unsigned int wA, unsigned int wB)
{
  for (unsigned int i = 0; i < hA; ++i)
    for (unsigned int j = 0; j < wB; ++j) {
      int sum = 0;
      for (unsigned int k = 0; k < wA; ++k) {
        sum += A[i * wA + k] * B[k * wB + j];
      }
      C[i * wB + j] = sum;
    }
}

