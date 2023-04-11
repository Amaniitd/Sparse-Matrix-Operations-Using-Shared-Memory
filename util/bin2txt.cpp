#include "bits/stdc++.h"
using namespace std;

void writeToTxtFile(string infile, string outfile, int sz)
{
    ifstream file(infile, ios::binary);
    ofstream txtfile(outfile);
    if (file.is_open())
    {
        int n, m, k;
        file.read((char *)&n, sizeof(int));
        file.read((char *)&m, sizeof(int));
        file.read((char *)&k, sizeof(int));
        txtfile << n << " " << m << " " << k << endl;
        for (int i = 0; i < k; i++)
        {
            int i_index, j_index;
            file.read((char *)&i_index, sizeof(int));
            file.read((char *)&j_index, sizeof(int));
            txtfile << i_index << " " << j_index << endl;
            for (int j = 0; j < m; j++)
            {
                for (int l = 0; l < m; l++)
                {
                    int temp = 0;
                    file.read((char *)&temp, sz);
                    txtfile << temp << " ";
                }
                txtfile << endl;
            }
        }
        file.close();
        txtfile.close();
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
        cout << "Usage: ./a.out <binfile> <txtfile> <size (1 or 2)>" << endl;
        exit(1);
    }
    string infile = argv[1];
    string outfile = argv[2];
    int sz = atoi(argv[3]);
    writeToTxtFile(infile, outfile, sz);
    return 0;
}