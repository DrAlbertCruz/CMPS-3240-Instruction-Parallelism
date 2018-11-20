#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <immintrin.h>
#define UNROLL (4)

void dgemm( int n, double* A, double* B, double* C );
void initRandMat( int m, double* A );

int main( int arg, char *argv[] ) {
   // Initialize random
   srand( time( 0 ) );

   /* A matrix has two sizes. For these labs we assume that the matrix is square,
    * of size N x N 
    */
   const int N = 512;    
   printf( "Running matrix multiplication of size %d x %d", N, N );

   /* Create three N x N double precision floating point matrixes on the heap
    * using malloc
    */
   double A[N*N];   // First 'A' matrix
   double B[N*N];   // 'B' matrix
   double C[N*N];   // 'C' matrix
  
   /* The following code loads random values into the matrixes.
    * We don't need to initialize 'C' because it will hold the
    * result.
    */
   initRandMat( N, A );  // Initialize values in A
   initRandMat( N, B );  // ... safe for B

   // Carry out double-precision generic matrix multiplication 
   dgemm( N, A, B, C );

   return 0;
}

// Code to randomly initialize the matrix
void initRandMat( int n, double* A ) {
   for ( int i = 0; i < n; i++ ) 
      for ( int j = 0; j < n; j++ ) 
         A[ i + j * n ] = 2 * rand() / (double) RAND_MAX - 1; // [-1,1]
}

// DGEMM. From Computer Organization and Design, Patterson and Hennesey 5e
// An unoptimized version of a double precision matrix multiplication, widely
// known as 'DGEMM' for double-precision general matrix multiply (GEMM). 
// Assumes matrixes are square and of the same length ('n' is the length of 
// one side).
void dgemm( int n, double* A, double* B, double* C ) {
    for ( int i = 0; i < n; i+=UNROLL*4 ) {
        for ( int j = 0; j < n; j++ ) {	
	    __m256d c[4];

	    for ( int x = 0; x < UNROLL; x++ )
                c[x] = _mm256_loadu_pd(C+i+x*4+j*n);

            for( int k = 0; k < n; k++ ) {
                __m256d b = _mm256_broadcast_sd(B+k+j*n);
                for (int x = 0; x < UNROLL; x++)
                    c[x] = _mm256_add_pd( c[x],
                                          _mm256_mul_pd(_mm256_loadu_pd(A+n*k+x*4+i), b)
                      			);
            }
        
	    for ( int x = 0; x < UNROLL; x++ )
	        _mm256_storeu_pd(C+i+x*4+j*n, c[x]);
       	}
    }
}
