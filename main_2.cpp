#include <bits/stdc++.h>
#include <omp.h>
// #include "library.hpp"
#include <chrono>
using namespace std;

int n, m, k, sz;
int* row_index;
int* col_index;
int** blocks;

vector<pair<int, int>> *csr;
vector<pair<int, int>> *csr_trans;

int cnt = 0;
vector<int> ans_row_index;
vector<int> ans_col_index;
vector<int*> ans_blocks;

int Inner(int a, int b) {return a * b;}
int Outer(int a, int b) {return a + b;}

void process(int row, int col) {
    bool flag = false;
    int* ans_block;

    int t1 = (csr[row]).size();
    int t2 = (csr_trans[col]).size();

    vector<pair<int, int>> * list1 = &csr[row];
    vector<pair<int, int>> * list2 = &csr_trans[col];

    int idx1 = 0;
    int idx2 = 0;

    while (idx1 < t1 && idx2 < t2)
    {
        int c = (list1->at(idx1)).first;
        int r = (list2->at(idx2)).first;

        if (r > c) {
            idx1++;
        }
        else if (r < c) {
            idx2++;
        }
        else {
            int* block_1 = blocks[(list1->at(idx1)).second];
            int* block_2 = blocks[(list2->at(idx2)).second];

            if (!flag){
                flag = true;
                ans_block = new int[m * m];
                memset(ans_block, 0, m * m * sizeof(int));
            }

            int sum, tmp;
            #pragma omp parallel for collapse(2) num_threads(8)
            for (int x = 0; x < m; x++) 
            {
                for (int y = 0; y < m; y++) 
                {
                    sum = Inner(block_1[x * m + 0], block_2[0 * m + y]);
                    for (int z = 1; z < m; z++) 
                    {
                        tmp = Inner(block_1[x * m + z], block_2[z * m + y]);
                        sum = Outer(sum, tmp);
                    }
                    ans_block[x * m + y] = Outer(sum, ans_block[x * m + y]);
                }
            }

            idx1++;
            idx2++;
        }
    }

    if (flag) {
        #pragma omp critical
        {
            cnt++;
            ans_row_index.push_back(row);
            ans_col_index.push_back(col);
            ans_blocks.push_back(ans_block);
        }
    }
} 

int main(int argc, char** argv)
{
    string inputfile, outputfile;
    inputfile = argv[1];
    outputfile = argv[2];
    cout << inputfile << " " << outputfile << endl;

    auto begin = std::chrono::high_resolution_clock::now();
    
    /* Reading the File */

    ifstream infile;
    infile.open(inputfile, ios::binary | ios::in);

    if (!infile.is_open()) {
        cout << "Error: Input File not opened" << endl;
        return 0;
    }

    infile.read((char*)&n, sizeof(n));
    infile.read((char*)&m, sizeof(m));
    infile.read((char*)&k, sizeof(k));

    cout << n << " " << m << " " << k << endl;

    sz = n / m;
    row_index = new int[2 * k];
    col_index = new int[2 * k];
    blocks = new int*[2 * k];

    csr = new vector<pair<int, int>>[sz];
    csr_trans = new vector<pair<int, int>>[sz];

    int i, j, tmp = 0;
    for (int x = 0; x < k; x++) {
        infile.read((char*)&i, sizeof(i));
        infile.read((char*)&j, sizeof(j));

        row_index[2 * x] = i;
        col_index[2 * x] = j;

        (csr[i]).push_back({j, 2 * x});
        (csr_trans[j]).push_back({i, 2 * x});

        if (i != j) {
            row_index[2 * x + 1] = j;
            col_index[2 * x + 1] = i;
            
            (csr[j]).push_back({i, 2 * x + 1});
            (csr_trans[i]).push_back({j, 2 * x + 1});
        }
        else {
            row_index[2 * x + 1] = -1;
            col_index[2 * x + 1] = -1;
        }

        int* block_1 = new int[m * m];
        int* block_2 = new int[m * m];

        for (int y = 0; y < m; y++) {
            for (int z = 0; z < m; z++) {
                infile.read((char*)&tmp, 1);
                block_1[y * m + z] = tmp;
                block_2[z * m + y] = tmp;
            }
        }

        blocks[2 * x] = block_1;
        blocks[2 * x + 1] = block_2;
    }

    #pragma omp parallel for num_threads(8)
    for (int i = 0; i < sz; i++) {
        sort((csr[i]).begin(), (csr[i]).end());
        sort((csr_trans[i]).begin(), (csr_trans[i]).end());
    }

    auto end = std::chrono::high_resolution_clock::now();
    float time = 1e-6 * (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin)).count();
    begin = std::chrono::high_resolution_clock::now();
    cout << time << "ms: reading time" << endl;

    /* Multipling the Matrix */

    #pragma omp parallel for num_threads(8) collapse(2) schedule(dynamic, 64)
    for (int row = 0; row < sz; row++) 
    {
        // #pragma omp parallel for
        for (int col = 0; col < sz; col++) 
        {
            if (row <= col) {
                process(row, col);
            }
        }
    }

    end = std::chrono::high_resolution_clock::now();
    time = 1e-6 * (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin)).count();
    begin = std::chrono::high_resolution_clock::now();
    cout << time << "ms: computation time" << endl;

    /* Writing the File */

    ofstream outfile;
    outfile.open(outputfile, ios::binary | ios::out);

    if (!outfile.is_open()) {
        cout << "Error: Output File not opened" << endl;
        return 0;
    }

    outfile.write((char*)&n, sizeof(n));
    outfile.write((char*)&m, sizeof(m));
    outfile.write((char*)&cnt, sizeof(cnt));

    for (int x = 0; x < cnt; x++) {
        i = ans_row_index[x];
        j = ans_col_index[x];

        outfile.write((char *)&i, sizeof(i));
        outfile.write((char *)&j, sizeof(j));

        for (int y = 0; y < m; y++) {
            for (int z = 0; z < m; z++) {
                tmp = ans_blocks[x][y * m + z];
                // tmp = min(tmp, 65535);
                outfile.write((char *)&tmp, 2);
            }
        }
    }

    end = std::chrono::high_resolution_clock::now();
    time = 1e-6 * (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin)).count();
    cout << time << "ms: writing time" << endl;

    cout << cnt << endl;

    return 0;
}










