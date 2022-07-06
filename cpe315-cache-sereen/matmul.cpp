#include <stdlib.h>
#include "matmul.h"

Matrix Allocate2ndMatrix(int height, int width, int init);

void matmul( float*, const float*, const float*, unsigned int, unsigned int, unsigned int);

////////////////////////////////////////////////////////////////////////////////
//! C = A * B
//! @param C          result matrix
//! @param A          matrix A 
//! @param B          matrix B
//! @param hA         height of matrix A
//! @param wB         width of matrix B
////////////////////////////////////////////////////////////////////////////////

/* You'll need to modify this function such that matrix B is accessed
 * correctly once you change the memory layout to column-major. */
void matmul(float* C, const float* A, const float* B, unsigned int hA, 
    unsigned int wA, unsigned int wB)
{
  for (unsigned int i = 0; i < hA; i+=2)
    for (unsigned int j = 0; j < wB; j+=2) {
      double sum1 = 0;
      double sum2 = 0;
      double sum3 = 0;
      double sum4 = 0; 
      for (unsigned int k = 0; k < wA; ++k) {
        double a = A[i * wA + k];
        double b = B[j * wB + k];
        sum1 += a * b;
        
        a = A[i * wA + k];
        b = B[(j+1) * wB + k];
        sum2 += a * b;
        
        a = A[(i+1) * wA + k];
        b = B[j * wB + k];
        sum3 += a * b;
        
        a = A[(i+1) * wA + k];
        b = B[(j+1) * wB + k];
        sum4 += a * b; 
      }
      C[i * wB + j] = (float)sum1;
      C[i * wB + (j+ 1)] = (float)sum2;
      C[(i + 1) * wB + j] = (float)sum3;
      C[(i+1) * wB + (j+1)] = (float)sum4; 
    }
}

// Allocate a matrix of dimensions height*width
Matrix Allocate2ndMatrix(int height, int width)
{
  Matrix M;
  M.width = M.pitch = width;
  M.height = height;
  int size = M.width * M.height;
  M.elements = NULL;

  M.elements = (float*) malloc(size*sizeof(float));

  /* This is a row-major allocation and initialization.
   * You need to modify this function which is only called
   * for Matrix B such that a column-major ordering is
   * performed. */
  for(unsigned int i = 0; i < M.height; i++)
   {
      for(unsigned int j = 0; j < M.height; j++)
         M.elements[i] = (rand() / (float)RAND_MAX);
      
   }
  
  return M;
}	

