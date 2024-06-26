#include <assert.h>
#include <vector>
#include <iostream>
using namespace std;

//#include <stdio.h>
//#include "cuda_runtime.h"
float CPUDotProduct(vector<float> vec1, vector<float> vec2) 
{
    //should assert that the two vectors are the same size
    assert(vec1.size() == vec2.size());
    int vector_index;
    float sum = 0;
    for (vector_index = 0; vector_index < vec1.size(); vector_index++)
    {
        sum = sum + (vec1[vector_index] * vec2[vector_index]);
    }
    return sum;
}

float GPUDotProduct(vector<float> vec1, vector<float> vec2)
{
}

//if we're doing this should prob check that row indices are correct
vector<vector<float>> NaiveMatMul(vector<vector<float>> mat1, vector<vector<float>> mat2)
{
    //columns of mat1 should be same size of rows of mat2
    assert(mat1.size() == mat2[0].size());
    int mat_1_col_index;
    int mat_2_col_index;
    int row_index;
    vector<float> column_multiple;
    float new_cell_value;
    vector<vector<float>> output_mat;
    vector<float> new_column;

    for (row_index = 0; row_index < mat1[0].size(); row_index++)
    {
        vector<float> row_multiple;
        for (mat_1_col_index = 0; mat_1_col_index < mat1.size(); mat_1_col_index++) 
        {
           row_multiple.push_back(mat1[mat_1_col_index][row_index]);
        }
        int mat_2_col_index;
        for ( mat_2_col_index= 0;  mat_2_col_index < mat2.size(); mat_2_col_index++)
        {
            column_multiple = mat2[mat_2_col_index];
            new_cell_value = CPUDotProduct(column_multiple, row_multiple);
            new_column.push_back(new_cell_value);
        }
        output_mat.push_back(new_column);
    }
    return output_mat;
}

int main() {
    vector<vector<float>> mat1;
    vector<float> vect{10,20,30};
    vector<float> vect2{20,30,40};
    vector<float> vect3{21,31,41};
    mat1.push_back(vect);
    mat1.push_back(vect2);
    mat1.push_back(vect3);
    vector<vector<float>> mat2;
    vector<float> vect4{10,20,30};
    vector<float> vect5{20,30,40};
    vector<float> vect6{21,31,41};
    mat2.push_back(vect4);
    mat2.push_back(vect5);
    mat2.push_back(vect6);
    vector<vector<float>> output = NaiveMatMul(mat1, mat2);
    for (vector<float> vec : output) 
    {
        for (float n : vec) 
        {
            cout << n << " ";
        }
    }
}
