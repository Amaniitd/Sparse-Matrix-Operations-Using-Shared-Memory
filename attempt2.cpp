#include "bits/stdc++.h"
#include "util.hpp"
#include "matrix_mul.hpp"
#include "library.hpp"

using namespace std;


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

void do_the_work(string input_file, string output_file)
{
    ifstream file(input_file, ios::binary);
    if(!file.is_open())
    {
        cout << "Unable to open file";
        exit(1);
    }
    int n, m, k;
    file.read((char *)&n, sizeof(int));
    file.read((char *)&m, sizeof(int));
    file.read((char *)&k, sizeof(int));
    cout << "Input file: " << "n = " << n << ", m = " << m << ", k = " << k << endl;
    vector<vector<pair<int, int>>> vertical_list(n);
    vector<vector<pair<int, int>>> horizonal_list(n);
    vector<int*> blocks;
    vector<int> i_indices;
    vector<int> j_indices;
    int itr = 0;
    for(int i = 0; i < k; i++)
    {
        int i_index, j_index;
        file.read((char *)&i_index, sizeof(int));
        file.read((char *)&j_index, sizeof(int));
        int *block = new int[m * m];
        for (int j = 0; j < m * m; j++)
        {
            block[j] = 0;
            file.read((char *)&block[j], 1);
        }
        horizonal_list[i_index].push_back({j_index, itr});
        vertical_list[j_index].push_back({i_index, itr++});
        blocks.push_back(block);
        i_indices.push_back(i_index);
        j_indices.push_back(j_index);
        if(i_index != j_index)
        {
            blocks.push_back(transpose(block, m));
            i_indices.push_back(j_index);
            j_indices.push_back(i_index);
            horizonal_list[j_index].push_back({i_index, itr});
            vertical_list[i_index].push_back({j_index, itr++});
        }
    }
    file.close();
    n/=m;
    // start time
    auto start = chrono::high_resolution_clock::now();

#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        sort(horizonal_list[i].begin(), horizonal_list[i].end());
        sort(vertical_list[i].begin(), vertical_list[i].end());
    }


    // multiply
    vector<vector<int*>> result(n);

#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        result[i].resize(n- i, 0);
    }
#pragma omp parallel
    {
    #pragma omp for schedule(dynamic, 1)
    for(int i = 0; i < n; i++)
    {
        if (horizonal_list[i].size() == 0)
        {
            continue;
        }
        for(int j = i; j < n; j++)
        {
            if (vertical_list[j].size() == 0)
            {
                continue;
            }
#pragma omp task
            {
                int l = 0;
                int p = 0;
                while (l < horizonal_list[i].size() && p < vertical_list[j].size())
                {
                    if (horizonal_list[i][l].first == vertical_list[j][p].first)
                    {
                        if (result[i][j - i] == 0)
                        {
                            result[i][j - i] = new int[m * m];
                            memset(result[i][j - i], 0, m * m * sizeof(int));
                        }
                        int *block = matrix_mul_block(blocks[horizonal_list[i][l].second], blocks[vertical_list[j][p].second], m);
                        for (int k = 0; k < m * m; k++)
                        {
                            result[i][j - i][k] = Outer(result[i][j - i][k], block[k]);
                            if (result[i][j - i][k] > MAX_VAL)
                            {
                                result[i][j - i][k] = MAX_VAL;
                            }
                        }
                        l++;
                        p++;
                    }
                    else if (horizonal_list[i][l].first < vertical_list[j][p].first)
                    {
                        l++;
                    }
                    else
                    {
                        p++;
                    }
            }
            }
        }
    }
    }
    // end time
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "Time taken by function: " << (float)duration.count() / 1000 << " ms" << endl;

    // write to file
    ofstream output(output_file, ios::binary);
    if(!output.is_open())
    {
        cout << "Unable to open file";
        exit(1);
    }
    int size = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = i; j < n; j++)
        {
            if (result[i][j-i] != 0)
            {
                size++;
            }
        }
    }
    int n2 = n * m;
    output.write((char *)&n2, sizeof(int));
    output.write((char *)&m, sizeof(int));
    output.write((char *)&size, sizeof(int));
    cout << "Output file: " << "n = " << n2 << ", m = " << m << ", k = " << size << endl;
    for (int i = 0; i < n; i++)
    {
        for (int j = i; j < n; j++)
        {
            if (result[i][j-i] != 0)
            {
                output.write((char *)&i, sizeof(int));
                output.write((char *)&j, sizeof(int));
                for (int l = 0; l < m * m; l++)
                {
                    output.write((char *)&result[i][j-i][l], 2);
                }
            }
        }
    }
    output.close();
    // cout << "File written" << endl;
}