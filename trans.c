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
void trans1(int A[32][32], int B[32][32]);
void trans2(int A[64][64], int B[64][64]);
void trans2plus(int A[64][64], int B[64][64]);
void trans3(int A[67][61], int B[61][67]);
/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if (M == 32 && N == 32) return trans1(A, B);
    if (M == 64 && N == 64) return trans2plus(A, B);
    if (M == 61 && N == 67) return trans3(A, B);
}

void trans1(int A[32][32], int B[32][32])
{
    int i, j, i1, v1, v2, v3, v4, v5, v6, v7, v8;
    for (i = 0; i < 32; i += 8)
	for (j = 0; j < 32; j += 8)
	    for(i1 = i; i1 < i + 8; i1++)
	    {
		v1 = A[i1][j+0]; v2 = A[i1][j+1]; v3 = A[i1][j+2]; v4 = A[i1][j+3];
		v5 = A[i1][j+4]; v6 = A[i1][j+5]; v7 = A[i1][j+6]; v8 = A[i1][j+7];
		B[j+0][i1] = v1; B[j+1][i1] = v2; B[j+2][i1] = v3; B[j+3][i1] = v4;
                B[j+4][i1] = v5; B[j+5][i1] = v6; B[j+6][i1] = v7; B[j+7][i1] = v8;
            }
	
}
void trans2(int A[64][64], int B[64][64])
{
    int i, j, i1, v1, v2, v3, v4;
    for (i = 0; i < 64; i += 8)
	for (j = 0; j < 64; j += 4)
	    for(i1 = i; i1 < i + 8; i1 += 1)
	    {
		v1 = A[i1][j+0]; v2 = A[i1][j+1]; v3 = A[i1][j+2]; v4 = A[i1][j+3];
		B[j+0][i1] = v1; B[j+1][i1] = v2; B[j+2][i1] = v3; B[j+3][i1] = v4;
	    }
}
void trans2plus(int A[64][64], int B[64][64])
{
    int i, j, x, y, x1, x2, x3, x4, x5, x6, x7, x8;
    for (i = 0; i < 64; i += 8)
	for (j = 0; j < 64; j += 8)
	{
	    for (x = i; x < i + 4; x++)
	    {
         	x1 = A[x][j+0]; x2 = A[x][j+1]; x3 = A[x][j+2]; x4 = A[x][j+3];
		x5 = A[x][j+4]; x6 = A[x][j+5]; x7 = A[x][j+6]; x8 = A[x][j+7];

		B[j+0][x] = x1; B[j+1][x] = x2; B[j+2][x] = x3; B[j+3][x] = x4;
		B[j][x+4] = x5; B[j+1][x+4] = x6; B[j+2][x+4] = x7; B[j+3][x+4] = x8;
	    }
	    for (y = j; y < j + 4; y++)
	    { 
		x5 = A[i+4][y]; x6 = A[i+5][y]; x7 = A[i+6][y]; x8 = A[i+7][y];
		x1 = B[y][i+4]; x2 = B[y][i+5]; x3 = B[y][i+6]; x4 = B[y][i+7];
	       
		B[y][i+4] = x5; B[y][i+5] = x6; B[y][i+6] = x7; B[y][i+7] = x8;
		B[y+4][i] = x1; B[y+4][i+1] = x2; B[y+4][i+2] = x3; B[y+4][i+3] = x4;
	    }
	    for (x = i + 4; x < i + 8; x++)
	    {
		x1 = A[x][j+4]; x2 = A[x][j+5]; x3 = A[x][j+6]; x4 = A[x][j+7];
		B[j+4][x] = x1; B[j+5][x] = x2; B[j+6][x] = x3; B[j+7][x] = x4;
	    }
	}
}
void trans3(int A[67][61], int B[61][67])
{
    int i, j, x, y;
    for (i = 0; i < 67; i += 17)
	for (j = 0; j < 61; j += 17)
	    for (x = i; x < i + 17 && x < 67; x++)
		for (y = j; y < j + 17 && y < 61; y++)
		    B[y][x] = A[x][y];
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

