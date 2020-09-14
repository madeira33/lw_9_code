#include <thread>
#include <mutex>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <future>


using namespace std;

template<typename T>
class Matrix
{
private:
    static size_t parallel;
    size_t rows_;
    size_t columns_;
    vector<vector<T> > M;
    
public:
    
    Matrix() : rows_(0), columns_(0)
    {
    }
    Matrix(size_t rows, size_t columns) : rows_(rows), columns_(columns)
    {
        M.resize(rows_);
        for (size_t i = 0;  i < rows_; ++i)
        {
            M[i].resize(columns_);
        }
    }
    void PrintMatrix()
    {
        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < columns_; ++j)
            {
                cout << M[i][j] << " ";
            }
            cout << endl;
        }
    }
    static void SetParallel(size_t &num)
    {
        parallel = num;
    }
    static size_t GetParallel()
    {
        return parallel;
    }
    void LoadMatrix(const char* filename)
    {
        ifstream fin(filename);
        LoadMatrix(fin);
    }
    void LoadMatrix(ifstream& fin)
    {
        size_t rows, columns;
        fin >> rows >> columns;
        
        
        rows_ = rows;
        columns_ = columns;
        
        M.resize(rows);
        for (size_t i = 0 ; i < rows; ++i)
        {
            M[i].resize(columns);
            for (size_t j = 0; j < columns; ++j)
            {
                fin >> M[i][j];
            }
        }
    }
    Matrix Transpose() const
    {
        Matrix temp(columns_, rows_);
        for (size_t i =0; i < rows_; ++i)
        {
            for (size_t j =0; j < columns_; ++j)
            {
                temp.M[j][i] = M[i][j];
            }
            
        }
        return temp;
    }
    T mltp(const vector<T>& v1, const vector<T>& v2)
    {
        T ret = 0;
        
        for (size_t i = 0; i < v1.size(); ++i)
        {
            ret += v1[i] * v2[i];
        }
        return ret;
    }
    Matrix operator * ( const Matrix& rhs)
    {
        
        
        if (columns_ != rhs.rows_)
        {
            throw invalid_argument("Invalid arguments");
        }
        Matrix<T> temp(rows_, rhs.columns_);
        
        size_t counter = 0;
        vector<future<int>> futures(parallel);
        vector<promise<int>> promises(parallel);
        Matrix F = rhs.Transpose();
        
        
        mutex door;
        auto lambda  = [&](size_t x)
        {
            
            while (true)
            {
                door.lock();
                size_t cnt = counter;
                counter++;
                door.unlock();
                
                if (cnt >= rows_ * rhs.columns_)
                {
                    break;
                }
                
                temp.M[cnt / rhs.columns_][cnt % rhs.columns_ ] = mltp(M[cnt / rhs.columns_], F.M[cnt % rhs.columns_]);
            }
            
            promises[x].set_value_at_thread_exit(1);
        };
        for (size_t i = 0; i < futures.size() ; ++i)
        {
            futures[i] = promises[i].get_future();
        }
        for (size_t i =0 ; i < futures.size(); ++i)
        {
            thread(lambda, i).detach();
            
        }
        for (size_t i =0 ; i < futures.size(); ++i)
        {
            futures[i].wait();
            
        }
        for (size_t i = 0; i < parallel; ++i)
            cout << "thread  № " << i + 1 << " finished" << endl;
        return temp;
    }
    void SaveMatrix(const char* filename )
    {
        ofstream fout(filename);
        
        fout  << rows_ << " " << columns_ << endl;
        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < columns_; ++j)
            {
                fout << M[i][j] << " ";
            }
            fout << endl;
        }
        fout.close();
    }
};

template<typename T>
size_t Matrix<T>::parallel = 0;
