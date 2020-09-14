#include "lw_9_code.hpp"

int main()
{
    ifstream fin("in.txt");
    size_t threads;
    fin >> threads;
    Matrix<double> A;
    A.SetParallel(threads);
    A.LoadMatrix(fin);
    Matrix<double> B;
    B.LoadMatrix(fin);
    Matrix<double> C = A*B;
    C.SaveMatrix("out.txt");

    return 0;
}

