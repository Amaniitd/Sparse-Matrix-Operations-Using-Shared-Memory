#include "bits/stdc++.h"
#include "util.hpp"
#include "matrix_mul.hpp"
#include "library.hpp"

using namespace std;

struct Block
{
    int i_index;
    int j_index;
    int *block;
};

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
    if (!file.is_open())
    {
        cout << "Unable to open file";
        exit(1);
    }
    int n, m, k;
    file.read((char *)&n, sizeof(int));
    file.read((char *)&m, sizeof(int));
    file.read((char *)&k, sizeof(int));

    vector<vector<Block>> vertical_list(n);
    vector<vector<Block>> horizonal_list(n);
    for (int i = 0; i < 2 * k; i += 2)
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
        Block b;
        b.i_index = i_index;
        b.j_index = j_index;
        b.block = block;
        horizonal_list[i_index].push_back(b);
        vertical_list[j_index].push_back(b);
        if (i_index == j_index)
            continue;
        b.block = transpose(block, m);
        b.j_index = i_index;
        b.i_index = j_index;
        horizonal_list[j_index].push_back(b);
        vertical_list[i_index].push_back(b);
    }
    file.close();
    // cout << "File read" << endl;
    n /= m;
    // start time
    auto start = chrono::high_resolution_clock::now();

#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        sort(horizonal_list[i].begin(), horizonal_list[i].end(), [](Block a, Block b)
             { if (a.j_index == b.j_index) return a.i_index < b.i_index; return a.j_index < b.j_index; });
        sort(vertical_list[i].begin(), vertical_list[i].end(), [](Block a, Block b)
             { if (a.i_index == b.i_index) return a.j_index < b.j_index; return a.i_index < b.i_index; });
    }

    // multiply
    vector<int> ans_i;
    vector<int> ans_j;
    vector<int *> ans_block;
#pragma omp parallel num_threads(8)
    {
#pragma omp for schedule(dynamic, 1)
        for (int i = 0; i < n; i++)
        {
            if (horizonal_list[i].size() == 0)
            {
                continue;
            }
            for (int j = i; j < n; j++)
            {
                if (vertical_list[j].size() == 0)
                {
                    continue;
                }
#pragma omp task
                {
                    int l = 0;
                    int p = 0;
                    bool flag = false;
                    int *result;
                    while (l < horizonal_list[i].size() && p < vertical_list[j].size())
                    {
                        if (horizonal_list[i][l].j_index == vertical_list[j][p].i_index)
                        {
                            if (!flag)
                            {
                                result = new int[m * m];
                                flag = true;
                            }
                            for (int x = 0; x < m; x++)
                            {
                                for (int y = 0; y < m; y++)
                                {
                                    for (int z = 0; z < m; z++)
                                    {
                                        result[x * m + y] = Outer(result[x * m + y], Inner(horizonal_list[i][l].block[x * m + z], vertical_list[j][p].block[z * m + y]));
                                    }
                                }
                            }
                            l++;
                            p++;
                        }
                        else if (horizonal_list[i][l].j_index < vertical_list[j][p].i_index)
                        {
                            l++;
                        }
                        else
                        {
                            p++;
                        }
                    }
                    if (flag)
                    {
#pragma omp critical
                        {
                            ans_i.push_back(i);
                            ans_j.push_back(j);
                            ans_block.push_back(result);
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
    if (!output.is_open())
    {
        cout << "Unable to open file";
        exit(1);
    }
    int size = ans_i.size();
    int n2 = n * m;
    output.write((char *)&n2, sizeof(int));
    output.write((char *)&m, sizeof(int));
    output.write((char *)&size, sizeof(int));

    for (int i = 0; i < size; i++)
    {
        output.write((char *)&ans_i[i], sizeof(int));
        output.write((char *)&ans_j[i], sizeof(int));
        for (int j = 0; j < m * m; j++)
        {
            output.write((char *)&ans_block[i][j], 2);
        }
    }
    output.close();
    // cout << "File written" << endl;
}