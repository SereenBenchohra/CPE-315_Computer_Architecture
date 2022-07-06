
/* Matrix multiplication: C = A * B.
*/

// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// includes, kernels
#include "matmul.h"

void matmul(float*, const float*, const float*, unsigned int, unsigned int, 
    unsigned int);

void PrintMat(Matrix);

Matrix AllocateMatrix(int height, int width, int init);
Matrix Allocate2ndMatrix(int height, int width);

////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv) {

  // Matrices for the program
  Matrix  M;
  Matrix  N;
  Matrix  P;
  // Number of elements in the solution matrix
  //  Assuming square matrices, so the sizes of M, N and P are equal

  srand(2204);

  // Check command line for input matrix files
  if(argc == 1) 
  {
    // No inputs provided
    // Allocate and initialize the matrices
    M  = AllocateMatrix(MATRIX_SIZE, MATRIX_SIZE, 1);
    N  = Allocate2ndMatrix(MATRIX_SIZE, MATRIX_SIZE);
    P  = AllocateMatrix(MATRIX_SIZE, MATRIX_SIZE, 0);
  }
  else
  {
    printf("Usage: matmul > outfile\n");
    return 1;
  }
  
  matmul(P.elements, M.elements, N.elements, HM, WM, WN);

  PrintMat(P);

  // Free matrices
  free(M.elements);
  M.elements = NULL;
  free(N.elements);
  N.elements = NULL;
  free(P.elements);
  P.elements = NULL;
  return 0;
}

// Allocate a matrix of dimensions height*width
//	If init == 0, initialize to all zeroes.  
//	If init == 1, perform random initialization.
Matrix AllocateMatrix(int height, int width, int init)
{
  Matrix M;
  M.width = M.pitch = width;
  M.height = height;
  int size = M.width * M.height;
  M.elements = NULL;

  M.elements = (float*) malloc(size*sizeof(float));

  for(unsigned int i = 0; i < M.height * M.width; i++)
  {
    M.elements[i] = (init == 0) ? (0.0f) : (rand() / (float)RAND_MAX);
  }
  return M;
}	

// Write a 16x16 floating point matrix to file
void PrintMat(Matrix M)
{
  /*** Print results ***/
  fprintf(stderr,"Printing Result Matrix:\n");
  for (unsigned int i=0; i<M.height; i++)
  {
    for (unsigned int j=0; j<M.width; j++) 
      fprintf(stdout, "%.2f ", M.elements[i*M.width+j]);
    fprintf(stdout, "\n"); 
  }
  fprintf(stderr,"******************************************************\n");
  fprintf(stderr,"Done.\n");
}
