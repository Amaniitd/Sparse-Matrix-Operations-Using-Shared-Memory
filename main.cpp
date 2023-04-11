
#include <iostream>
#include "matrix_mul.hpp"
#include <string>
using namespace std;

int main(int argc, char *argv[])
{
    string input_file = argv[1];
    string output_file = argv[2];
    do_the_work(input_file, output_file);
}
