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
void trans(int M, int N, int A[N][M], int B[M][N]);
void trans_blocking(int M, int N, int A[N][M], int B[M][N], int blockSize);
void trans_64(int A[64][64], int B[64][64]);
void trans_64_correct(int A[64][64], int B[64][64]);

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
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    switch (M) {
        case 32:
            // This was working by default
            trans_blocking(M, N, A, B, 8);
            break;
        case 64:
            trans_64(A, B);
            break;
        case 61:
            // Turns out I can get good enough performance using the provided blocking method, just with a bigger block size
            trans_blocking(M, N, A, B, 21);
            break;
    }
}

void trans_blocking(int M, int N, int A[N][M], int B[M][N], int blocksize) {
    // Blocking with block size 8x8
    int blockX,blockY,i,j;
    for(blockX=0; blockX<N; blockX+=blocksize) {
        for(blockY=0; blockY<M; blockY+=blocksize) {
            for(i=blockX; (i<blockX+blocksize) && (i<N); i++) {
                for(j=blockY; (j<blockY+blocksize) && (j<M); j++ ) {
                    /* Diagonal entries in diagonal blocks force cache miss
                    *  In this case the cache has B_i,..,B_{i-1},A_i,B_{i+1},..,B_{i+7}
                    *  so writing into B_i will evict A_i. But we need to read A_i again
                    *  so we get another miss while moving A_i back in
                    *  To alleviate this problem we set B_i at the end, so (1) we
                    *  won't need to read A_i again and (2) we have B_i when i <- i+1
                    */
                    if(blockY == blockX && i == j) continue;
                    B[j][i] = A[i][j];
                }
                if(blockY == blockX) B[i][i] = A[i][i];
            }
        }
    }

    // Literally 1984
    if (M == 61 && N == 67) {
        blockX = B[0][0];
    }
}

void trans_64(int A[64][64], int B[64][64]) {
    // Blocking with block size 8x8
    int aBlockX,aBlockY, bBlockX, bBlockY,i,j, temp;

    for(aBlockX = 0; aBlockX < 64; aBlockX += 8) {
        for(aBlockY = 0; aBlockY < 64; aBlockY += 8) {
            bBlockX = aBlockY;
            bBlockY = aBlockX;

            // Copy rows into matrix B, offset by 2 to avoid cache conflicts

            /*
                +---+---+
                |*A |*B |
                +---+---+
                | C | D |
                +---+---+
            */
            // 6 misses
            for (j = 0; j < 2; j++) {
                for (i = 0; i < 8; i++) {
                    B[bBlockY + 2 + j][bBlockX + i] = A[aBlockY + j][aBlockX + i];
                }
            }

            for (j = 0; j < 2; j++) {
                for (i = 0; i < 8; i++) {
                    B[bBlockY + j][bBlockX + i] = A[aBlockY + 2 + j][aBlockX + i];
                }
            }

            // Swap rows
            // 2 misses
            for (j = 0; j < 2; j++) {
                for (i = 0; i < 8; i++) {
                    temp = B[bBlockY + j][bBlockX + i];
                    B[bBlockY     + j][bBlockX + i] = B[bBlockY + 2 + j][bBlockX + i];
                    B[bBlockY + 2 + j][bBlockX + i] = temp;
                }
            }

            // Then transpose mini-matrix

            /*
                +---+---+
                |*A | B |
                +---+---+
                | C | D |
                +---+---+
            */
            // Free action
            for (j = 0; j < 3; j++) {
                for (i = j + 1; i < 4; i++) {
                    temp = B[bBlockY + j][bBlockX + i];
                    B[bBlockY + j][bBlockX + i] = B[bBlockY + i][bBlockX + j];
                    B[bBlockY + i][bBlockX + j] = temp;
                }
            }

            /*
                +---+---+
                | A |*B |
                +---+---+
                | C | D |
                +---+---+
            */
            // Free action
            for (j = 0; j < 3; j++) {
                for (i = j + 1; i < 4; i++) {
                    temp = B[bBlockY + j][bBlockX + 4 + i];
                    B[bBlockY + j][bBlockX + 4 + i] = B[bBlockY + i][bBlockX + 4 + j];
                    B[bBlockY + i][bBlockX + 4 + j] = temp;
                }
            }

            /*
                +---+---+
                | A |*B |
                +---+---+
                | C | D |
                +---+---+
            */
            // Swap rows for cache optimization when swapping quadrants
            // Free action
            for (j = 0; j < 2; j++) {
                for (i = 0; i < 4; i++) {
                    temp = B[bBlockY + j][bBlockX + 4 + i];
                    B[bBlockY     + j][bBlockX + 4 + i] = B[bBlockY + 2 + j][bBlockX + 4 + i];
                    B[bBlockY + 2 + j][bBlockX + 4 + i] = temp;
                }
            }

            // Copy rows into matrix B, offset by 2 to avoid cache conflicts

            /*
                +---+---+
                | A | B |
                +---+---+
                |*C |*D |
                +---+---+
            */
            // 6 misses
            for (j = 0; j < 2; j++) {
                for (i = 0; i < 8; i++) {
                    B[bBlockY + 4 + 2 + j][bBlockX + i] = A[aBlockY + 4 + j][aBlockX + i];
                }
            }

            for (j = 0; j < 2; j++) {
                for (i = 0; i < 8; i++) {
                    B[bBlockY + 4 + j][bBlockX + i] = A[aBlockY + 4 + 2 + j][aBlockX + i];
                }
            }

            // Swap rows
            // Free action
            for (j = 0; j < 2; j++) {
                for (i = 0; i < 8; i++) {
                    temp = B[bBlockY + 4 + j][bBlockX + i];
                    B[bBlockY + 4     + j][bBlockX + i] = B[bBlockY + 4 + 2 + j][bBlockX + i];
                    B[bBlockY + 4 + 2 + j][bBlockX + i] = temp;
                }
            }

            // Then transpose mini-matrix

            /*
                +---+---+
                | A | B |
                +---+---+
                |*C | D |
                +---+---+
            */
            // Free action
            for (j = 0; j < 3; j++) {
                for (i = j + 1; i < 4; i++) {
                    temp = B[bBlockY + 4 + j][bBlockX + i];
                    B[bBlockY + 4 + j][bBlockX + i] = B[bBlockY + 4 + i][bBlockX + j];
                    B[bBlockY + 4 + i][bBlockX + j] = temp;
                }
            }

            /*
                +---+---+
                | A | B |
                +---+---+
                | C |*D |
                +---+---+
            */
            // Free action
            for (j = 0; j < 3; j++) {
                for (i = j + 1; i < 4; i++) {
                    temp = B[bBlockY + 4 + j][bBlockX + 4 + i];
                    B[bBlockY + 4 + j][bBlockX + 4 + i] = B[bBlockY + 4 + i][bBlockX + 4 + j];
                    B[bBlockY + 4 + i][bBlockX + 4 + j] = temp;
                }
            }

            /*
                +---+---+
                | A | B |
                +---+---+
                |*C | D |
                +---+---+
            */
            // Swap rows for cache optimization when swapping quadrants
            // Free action
            for (j = 0; j < 2; j++) {
                for (i = 0; i < 4; i++) {
                    temp = B[bBlockY + 4 + j][bBlockX + i];
                    B[bBlockY + 4     + j][bBlockX + i] = B[bBlockY + 4 + 2 + j][bBlockX + i];
                    B[bBlockY + 4 + 2 + j][bBlockX + i] = temp;
                }
            }

            /*
                +---+---+
                | A |*C |
                +---+---+
                |*B | D |
                +---+---+
            */
            // Swap lower left and upper right quadrants
            // Split row operations in two to reuse cache
            // 4 misses
            for (j = 0; j < 2; j++) {
                for (i = 0; i < 4; i++) {
                    temp = B[bBlockY + 4 + j][bBlockX + i];
                    B[bBlockY + 4 + j][bBlockX     + i] = B[bBlockY + 2 + j][bBlockX + 4 + i];
                    B[bBlockY + 2 + j][bBlockX + 4 + i] = temp;
                }
            }
            
            // 4 misses
            for (j = 0; j < 2; j++) {
                for (i = 0; i < 4; i++) {
                    temp = B[bBlockY + j][bBlockX + 4 + i];
                    B[bBlockY         + j][bBlockX + 4 + i] = B[bBlockY + 2 + 4 + j][bBlockX + i];
                    B[bBlockY + 2 + 4 + j][bBlockX     + i] = temp;
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
    // registerTransFunction(trans, trans_desc); 
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
