#include "math.h"
#include "library.hpp"
#include "omp.h"
#include "util.hpp"
#include "matrix_mul.hpp"
#include "string.h"

int *transpose(int *A, int m)
{
    int *B = new int[m * m];
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < m; j++)
        {
            B[j * m + i] = A[i * m + j];
        }
    }
    return B;
}

vector<vector<int *>> readfile(string filename, int &n, int &m, int &k)
{
    ifstream file(filename, ios::binary);
    if (file.is_open())
    {
        file.read((char *)&n, sizeof(int));
        file.read((char *)&m, sizeof(int));
        file.read((char *)&k, sizeof(int));
        vector<vector<int *>> blocks(n / m, vector<int *>(n / m, 0));
        for (int i = 0; i < k; i++)
        {
            int i_index, j_index;
            file.read((char *)&i_index, sizeof(int));
            file.read((char *)&j_index, sizeof(int));
            int *block = new int[m * m];
            memset(block, 0, m * m * sizeof(int));
            for (int j = 0; j < m; j++)
            {
                for (int l = 0; l < m; l++)
                {
                    file.read((char *)&block[j * m + l], 1);
                }
            }
            blocks[i_index][j_index] = block;
            blocks[j_index][i_index] = transpose(block, m);
        }
        file.close();
        return blocks;
    }
    else
    {
        cout << "Unable to open file";
        exit(1);
    }
}

void writefile(string filename, vector<vector<int *>> &C, int n, int m)
{
    ofstream file(filename, ios::binary);
    if (file.is_open())
    {
        file.write((char *)&n, sizeof(int));
        file.write((char *)&m, sizeof(int));
        int k = 0;
        for (int i = 0; i < n / m; i++)
        {
            for (int j = i; j < n / m; j++)
            {
                if (C[i][j] != 0)
                {
                    k++;
                }
            }
        }
        file.write((char *)&k, sizeof(int));
        for (int i = 0; i < n / m; i++)
        {
            for (int j = i; j < n / m; j++)
            {
                if (C[i][j] != 0)
                {
                    file.write((char *)&i, sizeof(int));
                    file.write((char *)&j, sizeof(int));
                    for (int l = 0; l < m; l++)
                    {
                        for (int p = 0; p < m; p++)
                        {
                            file.write((char *)&C[i][j][l * m + p], 2);
                        }
                    }
                }
            }
        }
        file.close();
    }
    else
    {
        cout << "Unable to open file";
        exit(1);
    }
}


vector<vector<int *>> matrix_mul(vector<vector<int *>> &A, int n, int m)
{
    vector<vector<int *>> C(n / m, vector<int *>(n / m, 0));

#pragma omp parallel
{
#pragma omp for 
    for (int i = 0; i < n / m; i++)
    {
        for (int j = i; j < n / m; j++)
        {
#pragma omp task
            {
                for (int k = 0; k < n / m; k++)
                {
                    if (A[i][k] != 0 && A[k][j] != 0)
                    {
                        if (C[i][j] == 0)
                        {
                            C[i][j] = new int[m * m];
                            memset(C[i][j], 0, m * m * sizeof(int));
                        }
                        int *temp = matrix_mul_block(A[i][k], A[k][j], m);
                        for (int l = 0; l < m * m; l++)
                        {
                            C[i][j][l] = Outer(C[i][j][l], temp[l]);
                            if (C[i][j][l] > MAX_VAL)
                            {
                                C[i][j][l] = MAX_VAL;
                            }
                        }
                        delete[] temp;
                    }
                }
            }
        }
    }
}
    return C;
}

void do_the_work(string input_file, string output_file){
    int n, m, k;
    vector<vector<int*>> blocks = readfile(input_file, n, m, k);
    // start the clock
    auto start = chrono::high_resolution_clock::now();
    vector<vector<int*>> C = matrix_mul(blocks, n, m);
    // stop the clock
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "Time taken by function: " << (float)duration.count() / 1000 << " ms" << endl;
    writefile(output_file, C, n, m);
}