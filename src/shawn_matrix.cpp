#include <array>
#include <assert.h>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

template <typename T> class smatrix {
public:
  smatrix(T *a, int b, const char *c, array<int, 2> d) {
    // lets start with just 2d arrays. this will make it somewhat
    // inconvenient to refactor for higher dimensoinal arrays, but we can also
    // compose a higher order array as multiple 2d smatrices in the future, so
    // it should be okay to start with assuming 2d
    typedef T* start_pointer;
    typedef const T* const_start_pointer;
    typedef size_t size_type;
    typedef T value_type;

   
    data = a;
    data_size = b;
    datatype = c;
    dimensions = d;
    // actually might just not be possible to check if array is null lol
    if (data_size == 0) {
      throw invalid_argument(
          "Received empty data for array, unlikely this was intended");
    }

    if (data_size % dimensions[1] != 0 ||
        dimensions[0] != data_size / dimensions[1]) {
      throw invalid_argument("Given data size and dimensions don't match");
    }

    stride[0] = (data_size / dimensions[0]);
    stride[1] = 1;
  }
  const T &operator[](size_type i) {
    // overload for the [] operator, if many rows return a copy smatrix slice,
    // if 1 row return the value in data. this is so smatrix[i][j] just works
    // but also gives a way to do slices easily. prob just wanna leave it so
    // user must transpose their smatrix to get slice of a column, but
    // we could add another function to get column slices
    if (dimensions[0] > 1) {
      size_t row_size = dimensions[1];
      new T output_data[row_size];
      const int data_limit = *this.absolute_index(i, 0) + dimensions[1];
      copy(data, data_limit, output_data);
      array<int, 2> output_dimensions = {1, dimensions[1]};
      return smatrix(output_data, row_size, datatype, output_dimensions);
    } else {
      // we may need to return an smatrix with just 1 element in it instead
      // of int in this case depending on how finnicky it is to get it to
      // work with compiler, however, since this makes a variable amount of
      return data[i];
    }
  }
  smatrix<T>& smatrix<T>::operator=(const smatrix& right_hand_side) {
    if (&right_hand_side != this) {
      uncreate();
      create(right_hand_side.begin(), right_hand_side.end())
    }
    return *this;
  }
  void shape() {
    cout << "row x cols: " << dimensions[0] << " " << dimensions[1] << endl;
  }
  array<int, 2> get_dimensions() { return dimensions; }
  T get_data() { return data; }
  int get_size() { return data_size; }
  const char *get_datatype() { return datatype; }
  void print(int precision = 2, int width = 5) {
    // idk we change defaults for the width that feel right
    int column_index;
    cout << fixed << setprecision(precision);
    cout << "[";
    for (int row_index = 0; row_index <= dimensions[0] - 1; row_index++) {
      if (row_index != 0) {
        cout << " ";
      }
      cout << "[";
      cout << setw(width);
      for (int column_index = 0; column_index <= dimensions[1] - 1;
           column_index += stride[1]) {
        int abs_index = row_index * stride[0] + column_index;
        auto output = data[abs_index];
        cout << output;
        if (abs_index != data_size - 1 && column_index != dimensions[1] - 1) {
          cout << ", ";
        }
        if (row_index > 1000) {
          cout << "...";
          cout << "]]";
          cout << endl;
          return void();
        }
      }
      cout << "]";
      if (row_index * stride[0] < data_size - stride[0]) {
        cout << ",";
        cout << "\n";
      }
    }
    cout << "]";
    cout << endl;
    return void();
  }
  void transpose() {
    int temp2 = dimensions[0];
    dimensions[0] = dimensions[1];
    dimensions[1] = temp2;
    stride[0] = data_size / dimensions[0];
  }
  int absolute_index(int i, int j) {
    int absolute_index = i * stride[0] + j;
    return absolute_index;
  }
  smatrix copy() { return smatrix(data, data_size, datatype, dimensions); }

private:
  const int data_size;
  //Data should not be in its own variable, instead a function should create the data
  //T *data[SIZE];
  const char *datatype;
  array<int, 2> dimensions;
  // stride is the amount to move to format columns and rows, can simply
  // swap stride around to go from row major to column major
  array<int, 2> stride;
};

// template <typename T>
// T CPUDotProduct(T mat1, T mat2)
//{
//     //should assert that the two vectors are the same size
//     array<int, 2> dimensions_1 = mat1.get_dimensions();
//     array<int, 2> dimensions_2 = mat2.get_dimensions();
//     const int mat1_size = mat1.get_size();
//     const int mat2_size = mat2.get_size();
//     if (mat1_size != 1 and mat2_size != 1) {
//     if (dimensions_1 != dimensions_2) {
//         throw invalid_argument("Dimensions of mat1 and mat2 do not match");
//     }
//     if (mat1_size != mat2_size) {
//         throw invalid_argument("Size of mat1 and mat2 data do not match");
//     }
//     }
//     const int output_size;
//     array<int, 2> output_dimensions;
//     if (mat1_size == 1) {
//         output_size = mat2_size;
//         output_dimensions = dimensions_2;
//     }
//     else if (mat2_size == 1) {
//         output_size = mat1_size;
//         output_dimensions = dimensions_1;
//     }
//     else {
//         //do compiler optimizations get this
//         output_size = mat1_size;
//         output_dimensions = dimensions_1;
//     }
//     array<T, mat1_size> output_data;
//     int i;
//     for (i = 0; i < output_size; i++)
//     {
//         output_data[i] = *mat1.get_data()[i] * *mat2.get_data()[i];
//     }
//     return smatrix(&output_data, output_size, mat1.get_datatype(),
//     dimensions_1);
// }
//

template <typename T>
smatrix<T> CPUMatMul(smatrix<T> mat1, smatrix<T> mat2) {
  const array<int, 2> dimensions_1 = mat1.get_dimensions();
  const array<int, 2> dimensions_2 = mat2.get_dimensions();
  array<int, 2> output_dimensions;
  output_dimensions[0] = dimensions_1[0];
  output_dimensions[1] = dimensions_2[1];
  const size_t output_size = dimensions_1[0] * dimensions_2[1];
  T *output_data = new T[output_size];
  // the output dimensions are the rows of mat1 x the columns of mat2
  // rows of mat1 should be same size of columns of mat2
  if (dimensions_1[0] != dimensions_2[1]) {
    throw invalid_argument("Rows of mat1 do not match cols of mat2");
  }
  T new_cell_value;
  int i;
  int j;
  for (j = 0; j < dimensions_1[0]; j++) {
    for (i = 0; i < dimensions_2[1]; i++) {
      int shared_index;
      for (shared_index = 0; shared_index < dimensions_1[0]; shared_index++) {
        int mat1_abs_index = mat1.absolute_index(shared_index, j);
        int mat2_abs_index = mat2.absolute_index(i, shared_index);
        new_cell_value =
            *mat1.get_data()[mat1_abs_index] * *mat2.get_data()[mat2_abs_index];
        output_data[i + j] = new_cell_value;
      }
    }
  }
  // it doesn't feel intuitive that this will work, will the actual data
  // output_data points to still be there once we leave this scope? be careful
  // about this
  smatrix return_value(output_data, output_size, mat1.get_datatype(),
                       output_dimensions);
  return return_value;
}

int main() {
  float input[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
  array<int, 2> dims = {4, 3};
  smatrix mat1(input, 12, "float", dims);
  smatrix mat2(input, 12, "float", dims);
  mat1.print();
  cout << mat1.absolute_index(1, 2) << endl;
  mat1.transpose();
  mat1.print();
  smatrix mat3 = CPUMatMul(mat1, mat2);
  mat3.print();
  free(mat3);
}
