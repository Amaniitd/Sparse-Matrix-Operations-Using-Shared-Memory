#include "bits/stdc++.h"
using namespace std;

void writeToBinFile(string txtfile, string binfile, int sz)
{
    ifstream file(txtfile);
    ofstream bin(binfile, ios::binary);
    if (file.is_open() && bin.is_open())
    {
        int n, m, k;
        file >> n >> m >> k;
        bin.write((char *)&n, sizeof(int));
        bin.write((char *)&m, sizeof(int));
        bin.write((char *)&k, sizeof(int));
        for (int i = 0; i < k; i++)
        {
            int i_index, j_index;
            file >> i_index >> j_index;
            bin.write((char *)&i_index, sizeof(int));
            bin.write((char *)&j_index, sizeof(int));
            for (int j = 0; j < m; j++)
            {
                for (int l = 0; l < m; l++)
                {
                    int value;
                    file >> value;
                    bin.write((char *)&value, sz);
                }
            }
        }
        file.close();
        bin.close();
    }
    else
    {
        cout << "Unable to open file";
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cout << "Usage: ./a.out <txtfile> <binfile> <size>" << endl;
        exit(1);
    }
    string txtfile = argv[1];
    string binfile = argv[2];
    int sz = atoi(argv[3]);
    writeToBinFile(txtfile, binfile, sz);
    return 0;
}
