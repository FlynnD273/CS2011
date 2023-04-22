/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 * This functions is typically partitioned with a Switch Case staememnt or else if else 
 *  statements that hanlde the three cases, 32 X 32, 64 X 64 and 61 X 67. Here the 
*  function executes the same code for any size matrix .... Your job is to specialize it!  
* especially for the 64 X 64 matrix transpose
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
     {
            // Bocking with block size 8x8
            int x,y,i,j;
            for(x=0; x<N; x+=8)
                {
                    for(y=0; y<M; y+=8)
                        {
                            for(i=x; (i<x+8) && (i<N) ; i++)
                                {
                                    for(j=y; (j<y+8) && (j<M); j++ )
                                        {
                                            /* Diagonal entries in diagonal blocks force cache miss
                                             *  In this case the cache has B_i,..,B_{i-1},A_i,B_{i+1},..,B_{i+7}
                                             *  so writing into B_i will evict A_i. But we need to read A_i again
                                             *  so we get another miss while moving A_i back in
                                             *  To alleviate this problem we set B_i at the end, so (1) we
                                             *  won't need to read A_i again and (2) we have B_i when i <- i+1
                                             */
                                            if(y == x && i == j) continue;
                                            B[j][i] = A[i][j];
                                        }
                                    if(y == x) B[i][i] = A[i][i];
                                }
                        }
                }
        }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}
