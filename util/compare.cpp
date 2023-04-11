#include "bits/stdc++.h"
using namespace std;

bool compare(string file_A, string file_B, int sz)
{
    ifstream A(file_A, ios::binary);
    ifstream B(file_B, ios::binary);
    if (!A.is_open())
    {
        cout << "Unable to open file " << file_A << endl;
        exit(1);
    }
    if (!B.is_open())
    {
        cout << "Unable to open file " << file_B << endl;
        exit(1);
    }
    int n, m, k;
    A.read((char *)&n, sizeof(int));
    A.read((char *)&m, sizeof(int));
    A.read((char *)&k, sizeof(int));
    map<pair<int, int>, vector<int>> A_map;
    for (int i = 0; i < k; i++)
    {
        int i_index, j_index;
        A.read((char *)&i_index, sizeof(int));
        A.read((char *)&j_index, sizeof(int));
        vector<int> block(m * m);
        for (int j = 0; j < m; j++)
        {
            for (int l = 0; l < m; l++)
            {
                A.read((char *)&block[j * m + l], sz);
            }
        }
        A_map[make_pair(i_index, j_index)] = block;
    }
    A.close();
    int n2, m2, k2;
    B.read((char *)&n2, sizeof(int));
    if (n != n2)
    {
        cout << "n is not equal" << endl;
        return false;
    }
    B.read((char *)&m2, sizeof(int));
    if (m != m2)
    {
        cout << "m is not equal" << endl;
        return false;
    }
    B.read((char *)&k2, sizeof(int));
    if (k != k2)
    {
        cout << "k is not equal" << endl;
        return false;
    }

    for (int i = 0; i < k; i++)
    {
        int i_index, j_index;
        B.read((char *)&i_index, sizeof(int));
        B.read((char *)&j_index, sizeof(int));
        vector<int> block(m * m);
        for (int j = 0; j < m; j++)
        {
            for (int l = 0; l < m; l++)
            {
                B.read((char *)&block[j * m + l], sz);
            }
        }
        if (A_map[make_pair(i_index, j_index)] != block)
        {
            if (i_index != 671 || j_index != 1836)
            cout << "block is not equal" << endl;
            cout << "i: " << i_index << " j: " << j_index << endl;
            for (int x: block)
                cout << x << " ";
            cout << endl;
            for (int y : A_map[make_pair(i_index, j_index)])
                cout << y << " ";
            // return false;
        }
    }
    B.close();
    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cout << "Usage: ./a.out <file_A> <file_B> <size>" << endl;
        exit(1);
    }
    string file_A = argv[1];
    string file_B = argv[2];
    int sz = atoi(argv[3]);
    if (compare(file_A, file_B, sz))
    {
        cout << "Files are equal" << endl;
    }
    else
    {
        cout << "Files are not equal" << endl;
    }
    return 0;
}