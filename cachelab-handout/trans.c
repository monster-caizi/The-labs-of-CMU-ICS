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
 */

char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int temp_value0 = 0;
    int temp_value1 = 0;
    int temp_value2 = 0;
    int temp_value3 = 0;
    int temp_value4 = 0;
    int temp_value5 = 0;
    int temp_value6 = 0;
    int temp_value7 = 0;
    int i,j,k,l;
    if (N == 32)
    {
        for ( i = 0; i < N; i += 8)
        {
            for ( j = 0; j < M; j += 8)
            {
                for ( k = i; k < i + 8; ++k)
                {
                    temp_value0 = A[k][j];
                    temp_value1 = A[k][j+1];
                    temp_value2 = A[k][j+2];
                    temp_value3 = A[k][j+3];
                    temp_value4 = A[k][j+4];
                    temp_value5 = A[k][j+5];
                    temp_value6 = A[k][j+6];
                    temp_value7 = A[k][j+7];

                    B[j][k] = temp_value0;
                    B[j+1][k] = temp_value1;
                    B[j+2][k] = temp_value2;
                    B[j+3][k] = temp_value3;
                    B[j+4][k] = temp_value4;
                    B[j+5][k] = temp_value5;
                    B[j+6][k] = temp_value6;
                    B[j+7][k] = temp_value7;
                }
            }
        }
    }
    else if (N == 64) //Idea from https://www.cnblogs.com/liqiuhao/p/8026100.html, a great idea to learn
    {
        for ( i = 0; i < N; i += 8)
        {
            for ( j = 0; j < M; j += 8)
            {
                for ( k = i; k < i + 4; ++k)
                {
                // 1 2
                    temp_value0 = A[k][j];
                    temp_value1 = A[k][j+1];
                    temp_value2 = A[k][j+2];
                    temp_value3 = A[k][j+3];
                    temp_value4 = A[k][j+4];
                    temp_value5 = A[k][j+5];
                    temp_value6 = A[k][j+6];
                    temp_value7 = A[k][j+7];

                    B[j][k] = temp_value0;
                    B[j+1][k] = temp_value1;
                    B[j+2][k] = temp_value2;
                    B[j+3][k] = temp_value3;
                    B[j][k+4] = temp_value7;
                    B[j+1][k+4] = temp_value6;
                    B[j+2][k+4] = temp_value5;
                    B[j+3][k+4] = temp_value4;
                }

                for ( l = 0; l < 4; ++l)
                {
                    temp_value0 = A[i+4][j+3-l];
                    temp_value1 = A[i+5][j+3-l];
                    temp_value2 = A[i+6][j+3-l];
                    temp_value3 = A[i+7][j+3-l];
                    temp_value4 = A[i+4][j+4+l];
                    temp_value5 = A[i+5][j+4+l];
                    temp_value6 = A[i+6][j+4+l];
                    temp_value7 = A[i+7][j+4+l];

                    B[j+4+l][i] = B[j+3-l][i+4];
                    B[j+4+l][i+1] = B[j+3-l][i+5];
                    B[j+4+l][i+2] = B[j+3-l][i+6];
                    B[j+4+l][i+3] = B[j+3-l][i+7];

                    B[j+3-l][i+4] = temp_value0;
                    B[j+3-l][i+5] = temp_value1;
                    B[j+3-l][i+6] = temp_value2;
                    B[j+3-l][i+7] = temp_value3;
                    B[j+4+l][i+4] = temp_value4;
                    B[j+4+l][i+5] = temp_value5;
                    B[j+4+l][i+6] = temp_value6;
                    B[j+4+l][i+7] = temp_value7;
                } 
            }
        }
    }
    else
    {
         for(i=0;i<N;i++){
            k = i+16;
            for(j=0;j<M;j++){
                l = j+16;
                for(i=k-16;i<k&&i<N;i++){
                    for(j=l-16;j<l&&j<M;j++){
                        B[j][i]=A[i][j];
                    }
                }
                j--;
            }
            i--;
        }
    }
}

char transpose_submit_desc1[] = "Transpose submission1";
void transpose_submit1(int M, int N, int A[N][M], int B[M][N])
{
    int tempN,tempM;
    for(int i=0;i<N;i++)
    {   
        tempN = i+4; 
        for(int j=0;j<M;j++)
        {
             tempM = j+4;
             for(i=tempN-4;i<tempN;i++)
                 for(j=tempM-4;j<tempM;j++)
                    B[j][i] = A[i][j];
             j--;
        }
        i--;
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

    registerTransFunction(transpose_submit1, transpose_submit_desc1); 
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


void mmain()
{
    int a[32][32],b[32][32];
    for(int i=0;i<32*32;i++)
    {
         a[i/32][i-i/32] = i; 
    }
    transpose_submit(32,32,a,b);
}
