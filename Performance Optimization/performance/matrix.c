#include "matrix.h"
#include <omp.h>
int const UNROLL = 6;
/*
    called like:
    ```c
    matrix mat;
    allocate_matrix(&mat, 1, 2);
    ```
*/
int allocate_matrix(matrix **mat, int rows, int cols) {
    *mat = malloc(sizeof(matrix));
    (*mat)->dim.rows = rows;
    (*mat)->dim.cols = cols;
    (*mat)->data = calloc(rows * cols, sizeof(float));
    /*for (int i = 0; i < rows; ++i) {
        (*mat)->data[i] = calloc(cols, sizeof(float));
    }*/
    return 0;
}

int allocate_matrix_s(matrix **mat, shape s) {
    return allocate_matrix(mat, s.rows, s.cols);
}

int eye(matrix **mat, shape s) {
    assert(allocate_matrix_s(mat, s) == 0);
    // Make the result an identity matrix
    for (int i = 0; i < s.rows; i++) {
        (*mat)->data[i * s.cols + i] = 1;
    }
    return 0;
}

void free_matrix(matrix *mat) {
    free(mat->data);
    free(mat);
}

void dot_product(matrix *vec1, matrix *vec2, float *result) {
    assert(same_size(vec1, vec2) && vec1->dim.cols == 1);
    *result = 0;
    for (int i = 0; i < vec1->dim.rows; i++) {
        *result += vec1->data[i] * vec2->data[i];
    }
}

void outer_product(matrix *vec1, matrix *vec2, matrix *dst) {
    assert(vec1->dim.cols == 1 && vec2->dim.cols == 1 && vec1->dim.rows == dst->dim.rows && vec2->dim.rows == dst->dim.cols);
    #pragma omp parallel for
    for (int i = 0; i < vec1->dim.rows; i++) {
        for (int j = 0; j < vec2->dim.rows; j++) {
            dst->data[i * dst->dim.cols + j] = vec1->data[i] * vec2->data[j];
        }
    }
}

void matrix_power(matrix *mat, int pow, matrix *dst) {
    assert(mat != dst && same_size(mat, dst) && mat->dim.rows == mat->dim.cols);
    if (pow == 1) {
        copy(mat, dst);
        return;
    }
    if (pow == 2) {
        matrix_multiply(mat, mat, dst);
        return;
    }

    matrix* intermediate;
    eye(&intermediate, dst->dim);
    copy(intermediate, dst);
    for (int i = 0; i < pow; i++) {
        matrix_multiply(intermediate, mat, dst);
        copy(dst, intermediate);
    }
    free_matrix(intermediate);
}

void matrix_multiply(matrix *mat1, matrix *mat2, matrix *dst) {
    assert (mat1->dim.cols == mat2->dim.rows && dst->dim.rows == mat1->dim.rows && dst->dim.cols == mat2->dim.cols);
    free_matrix(dst);
    allocate_matrix(&dst, mat1->dim.rows, mat2->dim.cols);
    int floorDivide = (mat2->dim.cols / UNROLL) * UNROLL;

    #pragma omp parallel for
    for (int i = 0; i < mat1->dim.rows; i++) {
        for (int k = 0; k < mat1->dim.cols; k++) {
            /*for (int j = 0; j < mat2->dim.cols; j+=1) {
                dst->data[i * dst->dim.cols + j] += mat1->data[i * mat1->dim.cols + k] * mat2->data[mat2->dim.cols * k + j];
            }*/
            for (int j = 0; j < floorDivide; j+=UNROLL)
            {
                dst->data[i * dst->dim.cols + j] += mat1->data[i * mat1->dim.cols + k] * mat2->data[mat2->dim.cols * k + j];
                dst->data[i * dst->dim.cols + j + 1] += mat1->data[i * mat1->dim.cols + k] * mat2->data[mat2->dim.cols * k + j + 1];
                dst->data[i * dst->dim.cols + j + 2] += mat1->data[i * mat1->dim.cols + k] * mat2->data[mat2->dim.cols * k + j + 2];
                dst->data[i * dst->dim.cols + j + 3] += mat1->data[i * mat1->dim.cols + k] * mat2->data[mat2->dim.cols * k + j + 3];
                dst->data[i * dst->dim.cols + j + 4] += mat1->data[i * mat1->dim.cols + k] * mat2->data[mat2->dim.cols * k + j + 4];
                dst->data[i * dst->dim.cols + j + 5] += mat1->data[i * mat1->dim.cols + k] * mat2->data[mat2->dim.cols * k + j + 5];
            }
            for (int l = floorDivide; l < mat2->dim.cols; l++) {
                dst->data[i * dst->dim.cols + l] += mat1->data[i * mat1->dim.cols + k] * mat2->data[mat2->dim.cols * k + l];
            }
        }
    }

}

void matrix_scale(matrix *mat, float scalar, matrix *dst) {
    assert(same_size(mat, dst));
    #pragma omp parallel for
    for (int x = 0; x < mat->dim.rows; x++) {
        for (int y = 0; y < mat->dim.cols; y++) {
            float val = scalar * (mat->data[y + x * mat->dim.cols]);
            dst->data[y + x * dst->dim.cols] = val;
        }
    } 
}
void apply_func(matrix* mat, matrix* dst, float (*f)(float)) {
    assert(same_size(mat, dst));
    for (int x = 0; x < mat->dim.rows; x++) {
        for (int y = 0; y < mat->dim.cols; y++) {
            float val = f(mat->data[y + x * mat->dim.cols]);
            dst->data[y + x * dst->dim.cols] = val;
        }
    } 
}

void matrix_multiply_elementwise(matrix *mat1, matrix *mat2, matrix *dst) {
    assert(same_size(mat1, mat2) && same_size(mat1, dst));
    #pragma omp parallel for 
    for (int x = 0; x < mat1->dim.rows; x++) {
        for (int y = 0; y < mat1->dim.cols; y++) {
            float mat1result = mat1->data[y + x * mat1->dim.cols];
            float mat2result = mat2->data[y + x * mat2->dim.cols];
            dst->data[y + x * dst->dim.cols] = mat1result * mat2result;
        }
    } 
}

void matrix_add(matrix *mat1, matrix *mat2, matrix *dst) {
    assert(same_size(mat1, mat2) && same_size(mat1, dst));
    #pragma omp parallel for
    for (int x = 0; x < mat1->dim.rows; x++) {
        for (int y = 0; y < mat1->dim.cols; y++) {
            float mat1result = mat1->data[y + x * mat1->dim.cols];
            float mat2result = mat2->data[y + x * mat2->dim.cols];
            dst->data[y + x * dst->dim.cols] = mat1result + mat2result;
        }
    } 
}

void matrix_transpose(matrix *m, matrix *dst) {
    assert(m->dim.rows == dst->dim.cols && m->dim.cols == dst->dim.rows);
    #pragma omp parallel for
    for (int x = 0; x < dst->dim.rows; x++) {
        for (int y = 0; y < dst->dim.cols; y++) {
            dst->data[y + x * dst->dim.cols] = m->data[x + y * m->dim.cols];
        }
    } 
    
}

void copy(matrix *src, matrix *dst) {
    assert(same_size(src, dst));
    #pragma omp parallel for
    for (int x = 0; x < src->dim.rows; x++) {
        for (int y = 0; y < src->dim.cols; y++) {
            dst->data[y + x * dst->dim.cols] = src->data[y + x * src->dim.cols];
        }
    } 
}


int get_rows(matrix *mat) {
    return mat->dim.rows;
}

int get_cols(matrix *mat) {
    return mat->dim.cols;
}

void get_matrix_as_array(float *arr, matrix *mat) {
    int z = 0;
    for (int i = 0; i < mat->dim.rows * mat->dim.cols; i++) {
        arr[z] = mat->data[i];
        z++;
    }
}

matrix* arr_to_matrix(float *arr, int rows, int cols) {
    matrix *m;
    allocate_matrix(&m, rows, cols);

    for (int x = 0; x < rows; x++) {
        for (int y = 0; y < cols; y++) {
            set_loc(m, x, y, arr[x * cols + y]);
        }
    } 
    return m;
}

void set_loc(matrix *mat, int row, int col, float val) {
    assert (row < mat->dim.rows && col < mat->dim.cols && row >= 0 && col >= 0);
    mat->data[row * mat->dim.cols + col] = val;
}

int same_size(matrix *mat1, matrix *mat2) {
    return mat1 && mat2 && mat1->dim.rows == mat2->dim.rows && mat1->dim.cols == mat2->dim.cols;
}

float get_loc(matrix *mat, int row, int col) {
    return mat->data[row * mat->dim.cols + col];
}