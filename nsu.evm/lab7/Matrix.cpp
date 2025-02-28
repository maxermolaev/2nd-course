#include "Matrix.h"
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <cstdint>
#include <ctime>
#include <random>

Matrix::Matrix(int size) {
    m_size = size;
    mas = new float[size * size];
}

Matrix::Matrix(const Matrix& matrix2) {
    m_size = matrix2.m_size;
    mas = new float[m_size * m_size];
    for (int i = 0; i < m_size * m_size; i++) {
        mas[i] = matrix2.mas[i];
    }
}

Matrix::~Matrix() {
    delete[] mas;
}

Matrix Matrix::Transpose() const {
    Matrix new_matrix(m_size);
    for (int i = 0; i < m_size; i++) {
        for (int j = 0; j < m_size; j++) {
            new_matrix.mas[j + m_size * i] = mas[i + m_size * j];
        }
    }
    return new_matrix;
}

Matrix& Matrix::operator=(const Matrix& matrix2) {
    if (&matrix2 != this) {
        for (int i = 0; i < m_size * m_size; i++) {
            mas[i] = matrix2.mas[i];
        }
    }
    return (*this);
}

Matrix& Matrix::operator+=(const Matrix& matrix2) {
    for (int i = 0; i < m_size * m_size; i++) {
        mas[i] += matrix2.mas[i];
    }
    return (*this);
}

Matrix& Matrix::operator-=(const Matrix& matrix2) {
    for (int i = 0; i < m_size * m_size; i++) {
        mas[i] -= matrix2.mas[i];
    }
    return (*this);
}

Matrix& Matrix::operator/=(float n) {
    for (int i = 0; i < m_size * m_size; i++) {
        mas[i] /= n;
    }
    return (*this);
}

Matrix Matrix::operator*(const Matrix& matrix2) const {
    Matrix mult_matrix(m_size);
    for (int i = 0; i < m_size; i++) {
        float* mult_matrix_line = mult_matrix.mas + i * mult_matrix.m_size;
        for (int j = 0; j < m_size; j++) {
            mult_matrix_line[j] = 0;
        }
        for (int k = 0; k < m_size; k++) {
            float matrix_item = mas[i * m_size + k];
            float* matrix2_line = matrix2.mas + k * matrix2.m_size;
            for (int m = 0; m < m_size; m ++) {
                mult_matrix_line[m] += matrix_item * matrix2_line[m];
            }
        }
    }
    return mult_matrix;
}

int Matrix::size() const {
    return m_size;
}

float* Matrix::data() const {
    return mas;
}

void Matrix::FillIdentity() {
    for (int i = 0; i < m_size; i++) {
        for (int j = 0; j < m_size; j++) {
            mas[i * m_size + j] = (i == j) ? 1 : 0;
        }
    }
}

void Matrix::FillRandom() {
    srand(clock());
    for (int i = 0; i < m_size * m_size; i++) {
        mas[i] = (float) (rand() % 1000) / 10;
    }
}

float GetA1(const Matrix& A) {
    int size = A.size();
    float* sums = new float[size];
    for (int i = 0; i < size; i++) {
        sums[i] = 0;
    }
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            sums[j] += std::abs(A.data()[i * size + j]);
        }
    }
    float max_sum = *std::max_element(sums, sums + size);
    delete[] sums;
    return max_sum;
}

float GetAinf (const Matrix& A) {
    int size = A.size();
    float max_sum = INT32_MIN;
    for (int i = 0; i < size; i++) {
        float current_sum = 0;
        for (int j = 0; j < size; j++) {
            current_sum += std::abs(A.data()[i * size + j]);
        }
        max_sum = std::max(max_sum, current_sum);
    }
    return max_sum;
}

Matrix GetB(const Matrix& A) {
    Matrix B = A.Transpose();
    B /= GetA1(A) * GetAinf(A);
    return B;
}

Matrix GetR(const Matrix& A, const Matrix& B) {
    Matrix R(A.size());
    R.FillIdentity();
    R -= B * A;
    return R;
}

Matrix Matrix::Reverse (int cnt) const {
    Matrix reversed_matrix(m_size);
    reversed_matrix.FillIdentity();
    if (cnt < 2) {
        return reversed_matrix;
    }
    Matrix B = GetB(*this);
    Matrix R = GetR(*this, B);
    Matrix current_series_number = R;
    for (int i = 0; i < cnt - 2; i++) {
        reversed_matrix += current_series_number;
        current_series_number = current_series_number * R;
    }
    reversed_matrix += current_series_number;
    reversed_matrix = reversed_matrix * B;
    return reversed_matrix;
}

void Matrix::PrintMatrix() const {
    for (int i = 0; i < m_size; i++) {
        for (int j = 0; j < m_size; j++) {
            printf("%.3f\t", mas[i * m_size + j]);
        }
        printf("\n");
    }
}
