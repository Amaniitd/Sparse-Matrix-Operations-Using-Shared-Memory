
#include "util.hpp"
#include "library.hpp"

int MAX_VAL = pow (2, 16) - 1;

int *matrix_mul_block(int *A, int *B, int m)
{
    int *C = new int[m * m];
    memset(C, 0, m * m * sizeof(int));
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < m; j++)
        {
            for (int k = 0; k < m; k++)
            {
                C[i * m + j] = Outer(C[i * m + j], Inner(A[i * m + k], B[k * m + j]));
                C[i * m + j] = min(C[i * m + j], MAX_VAL);
            }
        }
    }
    return C;
}


void printMatrix(vector<vector<int *>> &A, int n, int m, int k)
{
    for (int i = 0; i < n / m; i++)
    {
        for (int j = 0; j < n / m; j++)
        {
            if (A[i][j] == 0)
            {
                for (int l = 0; l < m; l++)
                {
                    for (int p = 0; p < m; p++)
                    {
                        cout << 0 << " ";
                    }
                }
                continue;
            }
            for (int l = 0; l < m; l++)
            {
                for (int p = 0; p < m; p++)
                {
                    cout << A[i][j][l * m + p] << " ";
                }
            }
        }
        cout << endl;
    }
}